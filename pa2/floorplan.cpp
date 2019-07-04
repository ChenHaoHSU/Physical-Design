/****************************************************************************
  FileName     [ floorplann.cpp ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#include <iostream>
#include <cassert>
#include "floorplanner.h"

using namespace std;

/* extern Global variable */
extern time_t start;


bool FloorPlanner::floorplan() 
{
   cout << "> Processing ... ";
   cout << endl;

   int iter = 0;
   int numProcess = 0;
   int numtry = 0;

   while(!isTimeout(start)){
      initSol();
      int rejectNum = 0;
      ++numProcess;
      iter = 0;
      _N = 0;
      while(_T > T_FROZEN){
         ++numtry;
         ++_N;
         ++iter;
         _delta_cost_cnt = 0;
         _delta_sum = 0;
         rejectNum = 0;
         for(int i = 0; i < _L; ++i){
            genNeighbor();
            _delta = getCost() - _Cost;

            _delta_cost_cnt++;
            _delta_sum += fabs(_delta);

            if(_delta <= 0){
               updateCost();
               record();
            }
            else{   // delta > 0
               if(shinbwei()){
                  updateCost();
                  record();
               }
               else{
                  recover();
                  ++rejectNum;
               }
            }
         } 

         _T = T(iter);
         if(iter > _I) {
            break;
         }
      }

      if(!_optSol.empty()) break;
   }

   writeRecord();

   cout << "> #Process: " << numProcess << endl;
   cout << "> Try " << numtry << " floorplans." << endl;
   cout << "> Floorplan result:            " << endl;
   cout << "  - Chip Boundary = (0,0) (" << _Width << "," << _Height << ")" << endl;
   cout << "  - Chip Area     = " << getArea() << endl;
   cout << "  - Wire Length   = " << getTTLWireLen() << endl;
   cout << "  - Final Cost    = " << getFinalCost() << endl;

   return true;
}


void FloorPlanner::initSol()
{  
   if(!(rand() % 6))
      srand((unsigned)time(NULL));

   /* Build a new B*-Tree */
   if(_btree) delete _btree;
   _btree = new Btree(_blockList.size());

   /* Random generate a tree */
   for(unsigned i = 0; i < _blockList.size(); ++i){
      _blockList[i]->setParent(0);
      _blockList[i]->setLeft(0);
      _blockList[i]->setRight(0);
      _btree->insertRnd(_blockList[i]);
   }

   _btree->packing();

   /* Initial Perturbation */
   initPerturb();

   /* Initialize */
   _Cost = getCost();
   _optSol.resize(0);
   _uphill_sum = 0;
   _uphill_avg_cnt = 0;
   _delta_sum = 0;
   _delta_cost_cnt = 0;

   /* Initialize SA arguments */
   _T = T_INIT;
   _N = N_INIT;
   _P = P_INIT;
   _k = K_INIT;
   // _L = L_INIT;
   _L = _blockList.size() * 2 + 10;
   _C = C_INIT;
   // _I = ITER_LIMIT;
   _I = _blockList.size() * 20 + 10;

   _w = W_INIT;
   _x = X_INIT;
   _y = Y_INIT;
   _z = Z_INIT;

   /* debuggers */
   // _btree->printTree();
   // _btree->printList();
   // _btree->printInvList();
   // _btree->printLines();

}


OptType FloorPlanner::rndSelectType() const
{
   int r = (rand() % 3) + 2;

   if(r == 0)      return LEFT_ROTATE; 
   else if(r == 1) return RIGHT_ROTATE;
   else if(r == 2) return DELETE_INSERT;
   else if(r == 3) return SWAP_TWO_BLOCKS;
   else if(r == 4) return ROTATE_BLOCK;
   else{}

   cerr << "Oops... Something wrong here... rndSelectType()" << endl;
   assert(false);
   return OPT_TOT;
}


void FloorPlanner::initPerturb()
{
   assert(_btree != 0);
   
   _Anorm = _btree->getArea();
   _Wnorm = getTTLWireLen();

   _w = W_INIT;
   _x = X_INIT;
   _y = Y_INIT;
   _z = Z_INIT;

   double curCost = 0;
   double lastCost = 0;
   double delta = 0;
   double uphillsum = 0;
   double sumA = 0;
   double sumW = 0;
   int uphillcnt = 0;

   for(unsigned i = 0; i < INIT_PERTURB_NUM; ++i){
      genNeighbor();
      curCost = getCost();
      delta = curCost - lastCost;
      if(delta > 0){
         uphillsum += delta;
         ++uphillcnt;
      }
      sumA += getArea();
      sumW += getTTLWireLen();
      lastCost = curCost;
   }

   _P = P_INIT;
   _Anorm = sumA / INIT_PERTURB_NUM;
   _Wnorm = sumW / INIT_PERTURB_NUM;

   _T1 = ((double)uphillsum / (double)uphillcnt) / ((-1)*log(_P));
   _T = _T1 = 4;


}


inline bool FloorPlanner::shinbwei() const
{
   assert(_T > 0);
   return (getRnd() <= exp(-((double)_delta)/(_T)));
}


bool FloorPlanner::isInvFit() const
{
   return (_Width <= _outlineHeight && _Height <= _outlineWidth);
}


void FloorPlanner::record()
{
   if(isFitin()){
      if(getFinalCost() < _minCost){
         // _minCost = getArea();
         _minCost = getFinalCost();
         _optSol.resize(0);
         for(unsigned i = 0; i < _blockList.size(); ++i){
            int x = _blockList[i]->getX();
            int y = _blockList[i]->getY();
            int w = _blockList[i]->getWidth();
            int h = _blockList[i]->getHeight();
            _optSol.push_back(BlockR(x, y, w, h));
         }
      }
   }
   else if(isInvFit()){
      rotateAll();
      if(getFinalCost() < _minCost){
         // _minCost = getArea();
         _minCost = getFinalCost();
         _optSol.resize(0);
         for(unsigned i = 0; i < _blockList.size(); ++i){
            int x = _blockList[i]->getX();
            int y = _blockList[i]->getY();
            int w = _blockList[i]->getWidth();
            int h = _blockList[i]->getHeight();
            _optSol.push_back(BlockR(x, y, w, h));
         }
      }
      rotateAll();
   }
   else{ return; }

}


void FloorPlanner::updateCost() // update _Anorm, _Wnorm, and _Cost
{
   _Cost = getCost();
}


void FloorPlanner::rotateAll()
{
   for(unsigned i = 0; i < _blockList.size(); ++i)
      _blockList[i]->rotateXY();
}


double FloorPlanner::T(int iter)
{

  if(iter <= _k){
      return (_T1 * abs(_delta_sum / _delta_cost_cnt) / _N / _C);
   }
   else if(iter > _k){
      double tmp = (_T1 * abs(_delta_sum / _delta_cost_cnt) / _N);
      return tmp;
   }
   else{
      assert(false);
   }
}


void FloorPlanner::genNeighbor()
{
   _recoverMsg->reset();
 
   switch(rndSelectType()){

      case LEFT_ROTATE:
         genLeftRotate();
         break;

      case RIGHT_ROTATE:
         genRightRotate();
         break;

      case DELETE_INSERT:
         genDeleteInsert();
         break;

      case SWAP_TWO_BLOCKS:
         genSwapTwoBlocks();
         break;

      case ROTATE_BLOCK:
         genRotateBlock();
         break;

      default:
         assert(false);
         break;
   }

   _btree->packing();

   /* update _Width and _Height after packing */
   _Width = _btree->getWidth();
   _Height= _btree->getHeight();
   // _WireLen = getTTLWireLen();
}


void FloorPlanner::genLeftRotate()
{
   int n = rand() % _blockList.size();
   int c = 0;

   while(_blockList[n]->getRight() == 0){
      if(++c > 10) {
         _recoverMsg->setType(LEFT_ROTATE);
         return;
      }
      n = rand() % _blockList.size();
   }

   Block* r = _blockList[n]->getRight();

   _btree->leftRotateB(_blockList[n]);
   if(r)  _recoverMsg->pushBlock(_blockList[n]->getParent());
   _recoverMsg->setType(LEFT_ROTATE);

   // cout << ">> LeftRotate " << _blockList[n]->getName() << endl;
}


void FloorPlanner::genRightRotate()
{
   int n = rand() % _blockList.size();
   int c = 0;

   while(_blockList[n]->getLeft() == 0){
      if(++c > 10) {
         _recoverMsg->setType(RIGHT_ROTATE);
         return;
      }
      n = rand() % _blockList.size();
   }

   Block* l = _blockList[n]->getLeft();

   _btree->rightRotateB(_blockList[n]);
   if(l) _recoverMsg->pushBlock(_blockList[n]->getParent());
   _recoverMsg->setType(RIGHT_ROTATE);

   // cout << ">> RightRotate " << _blockList[n]->getName() << endl;
}


void FloorPlanner::genDeleteInsert()
{
   int n = rand() % _blockList.size();
   _recoverMsg->pushBlock(_btree->deleteAndInsert(_blockList[n]));
   _recoverMsg->pushBlock(_blockList[n]);
   _recoverMsg->setType(DELETE_INSERT);

   // cout << ">> DeleteInsert " << _blockList[n]->getName() << endl;

}


void FloorPlanner::genSwapTwoBlocks()
{
   int m = rand() % _blockList.size();
   int n = rand() % _blockList.size();
   while(m == n) n = rand() % _blockList.size();

   _btree->swapB(_blockList[m], _blockList[n]);
   _recoverMsg->pushBlock(_blockList[m]);
   _recoverMsg->pushBlock(_blockList[n]);
   _recoverMsg->setType(SWAP_TWO_BLOCKS);

   // cout << ">> Swap " << _blockList[m]->getName() << " " << _blockList[n]->getName() << endl;

}


void FloorPlanner::genRotateBlock()
{
   int n = rand() % _blockList.size();
   _blockList[n]->rotate();
   _recoverMsg->pushBlock(_blockList[n]);
   _recoverMsg->setType(ROTATE_BLOCK);

   // cout << ">> Rotate " << _blockList[n]->getName() << endl;
}


void FloorPlanner::recover()
{
   switch(_recoverMsg->getType()){

      case LEFT_ROTATE:
         if(_recoverMsg->getBlock(0))
            _btree->rightRotateB(_recoverMsg->getBlock(0));
         break;

      case RIGHT_ROTATE:
         if(_recoverMsg->getBlock(0))
            _btree->leftRotateB(_recoverMsg->getBlock(0));
         break;

      case DELETE_INSERT:
         _btree->swapB(_recoverMsg->getBlock(0), _recoverMsg->getBlock(1));
         break;

      case SWAP_TWO_BLOCKS:
         _btree->swapB(_recoverMsg->getBlock(0), _recoverMsg->getBlock(1));
         break;

      case ROTATE_BLOCK:
         _recoverMsg->getBlock(0)->rotate();
         break;

      default:
         assert(false);
         break;
   }

   _btree->packing();

   /* update _Width and _Height after packing */
   _Width = _btree->getWidth();
   _Height= _btree->getHeight();
   // _WireLen = getTTLWireLen();
   
}

