/****************************************************************************
  FileName     [ Net.cpp ]
  PackageName  [ pa1 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 1 
                 (2-Way F-M Circuit Partitioning) ]
  Date         [ 2017/3/18 created ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <cstring>
#include <stdlib.h>
#include <climits>
#include <algorithm>
#include "Net.h"

/*******************************************
                Parser 
********************************************/

bool NetManager::parseAll(const char* infileName)
{
   cout << "> Parsing \"" << infileName << "\" ..." << endl;
   time_t start = clock();

   ifstream inf(infileName, ios::in);

   if(!inf){
      cerr << "Error: Cannot open input file \"" << infileName << "\"." << endl;
      return false; 
   }

   if(!parseRValue(inf)){
      cerr << "Error: r_value is illigal." << endl;
      return false;
   }

   int lineNo = 1;
   while(inf){
      ++lineNo;
      if(!parseNet(inf)){
         cerr << "Error: Line " << lineNo << ", wrong NET format." << endl;
         return false;
      }
   }  

   buildCells();
   setBounds();

   inf.close();

   time_t ptime = clock();
   float t_used = (float)(ptime - start) / CLOCKS_PER_SEC;
   cout << "> Parsing Time: " << t_used << " seconds." << endl;

   return true;
}


bool NetManager::parseRValue(ifstream& inf)
{
   inf >> _rvalue;
   if(_rvalue <= 0.0 || _rvalue >= 1.0){
      cerr << "Error: \"" << _rvalue << "\" is out of range. (0 < r < 1)" << endl;
      return false;
   }
   return true;
}


bool NetManager::parseNet(ifstream& inf)
{
   string netStr;
   string tok;
 
   // parse "NET"
   inf >> netStr;
   if(inf.eof()) return true;
   if(netStr != "NET"){
      cerr << "Error: Wrong format. (Should begin with \"NET\")." << endl;
      return false;
   }

   // parse nx
   Net* newNet;
   inf >> netStr;
   if(inf.eof()){
      cerr << "Error: Missing net name after \"NET\"." << endl;
      return false;
   }
   if(!parseNX(netStr, newNet)){
      return false;
   }

   // parse cx
   while(inf >> netStr){
      if(netStr == ";") break;
      if(!parseCX(netStr, newNet)){
         return false;
      }
   }

   return true;
}


bool NetManager::parseNX(const string& netStr, Net*& newNet)
{
   if(netStr == ";"){
      cerr << "Error: Missing net name after \"NET\"." << endl;
      return false;
   }
   newNet = new Net;
   newNet->setName(netStr);
   _netList.push_back(newNet);
   return true;
}


bool NetManager::parseCX(const string& netStr, Net*& newNet)
{
   static int cellcounter = 0;

   CellV tmp(0, netStr);
   if(_hash.query(tmp)){
      for(unsigned i = 0; i < newNet->getCellNum(); ++i)
         if(tmp.getNo() == newNet->getTmpCell(i))
            return true;

      newNet->pushTmpCell(tmp.getNo());
   }
   else{
      assert(_hash.insert(CellV(cellcounter, netStr)));
      newNet->pushTmpCell(cellcounter);
      _maxCellIdx = cellcounter;
      ++cellcounter;
   }

   return true;

}


void NetManager::buildCells()
{
   _cellList.resize(_maxCellIdx + 1);
   HashSet<CellV>::iterator iter = _hash.begin();
   for(;iter != _hash.end(); ++iter){
      Cell* newCell = new Cell((*iter).getNo(), (*iter).getName());
      _cellList[(*iter).getNo()] = newCell;
   }

   bool warn = false;
   int P = 0;

   for(unsigned i = 0; i < _netList.size(); ++i){

      if(_netList[i]->getCellNum() == 1) warn = true;

      for(unsigned j = 0, n = _netList[i]->getCellNum(); j < n; ++j){
         int idx = _netList[i]->getTmpCell(j);
         _netList[i]->pushCell(_cellList[idx]);
         _cellList[idx]->pushNet(_netList[i]);
         ++P;
      }
   }

   for(unsigned i = 0; i < _cellList.size(); ++i)
      if(!_cellList[i]->isNetEmpty()) 
         ++_cellNum;

   cout << "> #Nets: " << _netList.size() << endl;
   cout << "> #Cells: " << _cellList.size() << endl;
   cout << "> #Pins: " << P << endl;

   if(warn) 
      cout << "> Warning: Some nets contain only one cell!!" << endl;

}


void NetManager::setBounds()
{
   _upperBound = ((1 + _rvalue) / 2) * _cellNum;
   _lowerBound = ((1 - _rvalue) / 2) * _cellNum;
}


/*******************************************
                FM 
********************************************/

bool NetManager::FM() 
{
   int iterNum = 0;
   time_t start = clock(), end;

   initPartition();
   
   while(_Gain >= 0){
      cout << "> #Iterations: " << ++iterNum << " ..." << flush << '\r'; 
      // cout << "> #Iterations: " << ++iterNum << " ... " ; 

      initAll();
      initGain();
      buildBucketList();

      Cell* maxGainCell = getMaxNBalanceCell();
      BucketList* fromPart = 0;
      BucketList* toPart   = 0;
      
      while(maxGainCell){
         fromPart = maxGainCell->getPart();
         toPart = (fromPart == _PartA ? _PartB : _PartA);

         maxGainCell->lock();

         updateBeforeMove(maxGainCell, fromPart, toPart);
         moveCell(maxGainCell, fromPart, toPart);
         updateAfterMove(maxGainCell, fromPart, toPart);
         
         maxGainCell = getMaxNBalanceCell();
      }

      updateGain();
      // cout << "G = " << _Gain << " ,Cutsize = " << getCutSize() << endl;

      if(checkTime(start, end)){ 
         cerr << endl << "> Warning: Break before " << TIME_LIMIT << " seconds!! ";
         break;
      }
      if(checkRepeat()){
         cerr << endl << "> Warning: Break by infinite loop (" << REPEAT_LIMIT << " times)!! ";
         break;
      }
      if(checkSameCutSize()){
         cerr << endl << "> Warning: The same CutSize (" << getCutSize(false) << ") is illegal!! ";
         break;
      }

   }

   cout << endl << "> Final Cutsize: " << getCutSize(false) << endl;
   cout << "> Balanced: " << _lowerBound << "(min) < " 
        << _NumA << "(G1), " << _NumB << "(G2) < "
        << _upperBound << "(max)."  << endl;

   return true;
}


void NetManager::initPartition()
{
   _PartA = new BucketList;
   _PartB = new BucketList;
   
   /* Method 1 */
   /*
   unsigned m = _cellNum / 2;

   for(unsigned i = 0; i < m; ++i){
      if(_cellList[i]->isValid()){
         _cellList[i]->setPart(_PartA);
         ++_NumA;
         for(unsigned j = 0; j < _cellList[i]->netNum(); ++j)
            (*_cellList[i])._nets[j]->increA();
      }
      else
         ++m;
   }

   for(unsigned i = m; i < _cellList.size(); ++i){
      if(_cellList[i]->isValid()){
         _cellList[i]->setPart(_PartB);
         ++_NumB;
         for(unsigned j = 0; j < _cellList[i]->netNum(); ++j)
            (*_cellList[i])._nets[j]->increB();
      }
   }
   */

   /* Method 2 */
   /*
   for(unsigned i = 0; i < _cellList.size(); ++i){
      if(_cellList[i]->isValid()){
         if(i % 2){
            _cellList[i]->setPart(_PartB);
            ++_NumB;
            for(unsigned j = 0; j < _cellList[i]->netNum(); ++j)
               (*_cellList[i])._nets[j]->increB();
         }
         else{
            _cellList[i]->setPart(_PartA);
            ++_NumA;
            for(unsigned j = 0; j < _cellList[i]->netNum(); ++j)
               (*_cellList[i])._nets[j]->increA();
         }
      }
   }
   */

   /* Method 3 */
   /*
   vector<Cell*> sortlist = _cellList;
   ::sort(sortlist.begin(), sortlist.end(), CellSort());

   unsigned m = _cellNum / 2;

   for(unsigned i = 0; i < m; ++i){
      if(sortlist[i]->isValid()){
         sortlist[i]->setPart(_PartA);
         ++_NumA;
         for(unsigned j = 0; j < sortlist[i]->netNum(); ++j)
            (*_cellList[sortlist[i]->getNo()])._nets[j]->increA();
      }
      else
         ++m;
   }

   for(unsigned i = m; i < sortlist.size(); ++i){
      if(sortlist[i]->isValid()){
         sortlist[i]->setPart(_PartB);
         ++_NumB;
         for(unsigned j = 0; j < sortlist[i]->netNum(); ++j)
            (*_cellList[sortlist[i]->getNo()])._nets[j]->increB();
      }
   }
   */

   /* Method 4 */
   
   vector<Cell*> sortlist = _cellList;
   ::sort(sortlist.begin(), sortlist.end(), CellSort());
   
   for(unsigned i = 0; i < sortlist.size(); ++i){
      if(sortlist[i]->isValid()){
         if(i % 2){
            sortlist[i]->setPart(_PartB);
            ++_NumB;
            for(unsigned j = 0; j < sortlist[i]->netNum(); ++j)
               (*_cellList[sortlist[i]->getNo()])._nets[j]->increB();
         }
         else{
            sortlist[i]->setPart(_PartA);
            ++_NumA;
            for(unsigned j = 0; j < sortlist[i]->netNum(); ++j)
               (*_cellList[sortlist[i]->getNo()])._nets[j]->increA();
         }
      }
   }
   

   cout << "> (Initial Cutsize: " << getCutSize() << ")" << endl;
   
}


void NetManager::initAll()
{
   _solSeq.clear();

   for(unsigned i = 0; i < _cellList.size(); ++i)
      if(_cellList[i]->isValid()){
         _cellList[i]->unlock();
         _cellList[i]->setGain(0);
         _cellList[i]->setFront(0);
         _cellList[i]->setBack(0);
      }
}


void NetManager::initGain()
{
   BucketList* tmpPart = 0;
   unsigned tmpNumA = 0;
   unsigned tmpNumB = 0;

   for(unsigned i = 0; i < _cellList.size(); ++i)
      if(_cellList[i]->isValid())
         for(unsigned j = 0; j < _cellList[i]->netNum(); ++j){
            tmpPart = _cellList[i]->getPart();
            tmpNumA = (*_cellList[i])._nets[j]->getNumA();
            tmpNumB = (*_cellList[i])._nets[j]->getNumB();

            if(tmpPart == _PartA && tmpNumA == 1)
               _cellList[i]->increGain();

            else if(tmpPart == _PartA && tmpNumB == 0)
               _cellList[i]->decreGain();

            else if(tmpPart == _PartB && tmpNumB == 1)
               _cellList[i]->increGain();

            else if(tmpPart == _PartB && tmpNumA == 0)
               _cellList[i]->decreGain();

            else{}
         }

}


void NetManager::buildBucketList()
{
   if(_maxPinNum == 0){
      for(unsigned i = 0; i < _cellList.size(); ++i)
         if(_cellList[i]->netNum() > _maxPinNum && _cellList[i]->isValid())
            _maxPinNum = _cellList[i]->netNum();

      _PartA->setMaxPinNum(_maxPinNum);
      _PartB->setMaxPinNum(_maxPinNum);
   }

   _PartA->clearList();
   _PartB->clearList();
   _PartA->resize(_maxPinNum);
   _PartB->resize(_maxPinNum);

   for(int i = _cellList.size() - 1; i >= 0; --i){   // Method 1
   // for(unsigned i = 0; i < _cellList.size(); ++i){      // Method 2
      if(_cellList[i]->isValid()){

         if(_cellList[i]->getPart() == _PartA){
            if(_cellList[i]->getGain() > _PartA->getMaxGain())
               _PartA->setMaxGain(_cellList[i]->getGain());
            _PartA->insertC(_cellList[i]);
         }

         else if(_cellList[i]->getPart() == _PartB){
            if(_cellList[i]->getGain() > _PartB->getMaxGain())
               _PartB->setMaxGain(_cellList[i]->getGain());
            _PartB->insertC(_cellList[i]);
         }

         else
            assert(false);
      }
   }

   _MaxGain = (_PartA->getMaxGain() >= _PartB->getMaxGain() ?
                   _PartA->getMaxGain() : _PartB->getMaxGain());
}


Cell* NetManager::getMaxNBalanceCell()
{
   if(_PartA->getMaxGain() == INT_MIN && _PartB->getMaxGain() == INT_MIN)
      return 0;

   Cell* tmpCell = 0;
   BucketList* tmpPart1 = 0;
   BucketList* tmpPart2 = 0;

   tmpPart1 = (_PartA->getMaxGain() >= _PartB->getMaxGain() ? _PartA : _PartB);
   tmpPart2 = (_PartA->getMaxGain() >= _PartB->getMaxGain() ? _PartB : _PartA);
   
   tmpCell = tmpPart1->getMaxGainCell(this);
   if(tmpCell != 0) 
      return tmpCell;
   else{
      tmpCell = tmpPart2->getMaxGainCell(this);
      if(tmpCell != 0)
         return tmpCell;
      else
         return 0;
   }

   return 0;
}


Cell* BucketList::getMaxGainCell(NetManager* mgr)
{
   if(_maxGain == INT_MIN) return 0;

   Cell* tmpCell = _list[idx(_maxGain)];

   if(tmpCell){
      if(mgr->isBalance(tmpCell))
         return tmpCell;
      else
         return 0;
   }

   return 0;
}


bool NetManager::isBalance(Cell* c) const
{
   if(c->getPart() == _PartA){
      if((_NumA - 1) >= _lowerBound && (_NumB + 1) <= _upperBound)
         return true;  
   }
   else if(c->getPart() == _PartB){
      if((_NumB - 1) >= _lowerBound && (_NumA + 1) <= _upperBound)
         return true;  
   }
   else
      assert(false);

   return false;
}


void BucketList::updateMaxGain(Cell* cell)
{
   if(cell->getBack() != 0) return;
   for(int i = (int)idx(cell->getGain()) - 1; i >= 0; --i)
      if(_list[i] != 0){
         _maxGain = i - _maxPinNum;
         return;
      }

   _maxGain = INT_MIN;
   return;
}


void BucketList::updateMaxGain(int g)
{
   if(g == INT_MIN) return;
   int tmpIdx = idx(g);
   if(_list[tmpIdx] != 0) return;
   for(int i = tmpIdx - 1; i >= 0; --i)
      if(_list[i] != 0){
         _maxGain = i - _maxPinNum;
         return;
      }

   _maxGain = INT_MIN;
   return;
}


void NetManager::updateBeforeMove(Cell* maxGainCell, BucketList* fromPart, BucketList* toPart)
{
   PARTITION p = (maxGainCell->getPart() == _PartA ? AAA : BBB);

   for(unsigned i = 0, n = maxGainCell->netNum(); i < n; ++i){
      Net* tmpNet = maxGainCell->getNet(i);
      if(p == AAA){
         if(tmpNet->getNumB() == 0){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(!tmpCell->getLocked()){
                  tmpCell->increGain();
                  if(tmpCell->getGain() > _PartA->getMaxGain())
                     _PartA->setMaxGain(tmpCell->getGain());
                  _PartA->deleteC(tmpCell, idx(tmpCell->getGain() - 1));
                  _PartA->insertC(tmpCell);
               }
            }
         }
         else if(tmpNet->getNumB() == 1){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(tmpCell->getPart() == _PartB && !tmpCell->getLocked()){
                  tmpCell->decreGain();
                  _PartB->deleteC(tmpCell, idx(tmpCell->getGain() + 1));
                  _PartB->insertC(tmpCell);
               }
            }
         }
         else {}
      }
      else if(p == BBB){
         if(tmpNet->getNumA() == 0){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(!tmpCell->getLocked()){
                  tmpCell->increGain();
                  if(tmpCell->getGain() > _PartB->getMaxGain())
                     _PartB->setMaxGain(tmpCell->getGain());
                  _PartB->deleteC(tmpCell, idx(tmpCell->getGain() - 1));
                  _PartB->insertC(tmpCell);
               }
            }
         }
         else if(tmpNet->getNumA() == 1){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(tmpCell->getPart() == _PartA && !tmpCell->getLocked()){
                  tmpCell->decreGain();
                  _PartA->deleteC(tmpCell, idx(tmpCell->getGain() + 1));
                  _PartA->insertC(tmpCell);
               }
            }
         }
         else {}
      }
      else 
         assert(false);
   }

   // cout << "***Before... " << endl;
   // _PartA->printList();
   // _PartB->printList();

}


void NetManager::moveCell(Cell* maxGainCell, BucketList* fromPart, BucketList* toPart)
{
   PARTITION p = (maxGainCell->getPart() == _PartA ? AAA : BBB);

   if(p == AAA){ 
      _PartA->deleteC(maxGainCell, idx(maxGainCell->getGain())); 
      maxGainCell->setPart(_PartB);       
      --_NumA; ++_NumB;
   }
   else if(p == BBB){ 
      _PartB->deleteC(maxGainCell, idx(maxGainCell->getGain()));
      maxGainCell->setPart(_PartA);
      ++_NumA; --_NumB;
   }
   else assert(false);

   for(unsigned i = 0; i < maxGainCell->netNum(); ++i){
      Net* tmpNet = maxGainCell->getNet(i);
      if(p == AAA){
         tmpNet->moveA2B();
      }
      else if(p == BBB){
         tmpNet->moveB2A();
      }
      else
         assert(false);
   }

   _solSeq.push_back(maxGainCell->getNo());

   assert(maxGainCell->getLocked());

   // cout << "***move... " << maxGainCell->getNo() << endl;
   // _PartA->printList();
   // _PartB->printList();

}


void NetManager::updateAfterMove(Cell* maxGainCell, BucketList* fromPart, BucketList* toPart)
{
   PARTITION p = (maxGainCell->getPart() == _PartA ? AAA : BBB);

   for(unsigned i = 0, n = maxGainCell->netNum(); i < n; ++i){
      Net* tmpNet = maxGainCell->getNet(i);
      if(p == AAA){
         if(tmpNet->getNumB() == 0){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(!tmpCell->getLocked()){
                  tmpCell->decreGain();
                  _PartA->deleteC(tmpCell, idx(tmpCell->getGain() + 1));
                  _PartA->insertC(tmpCell);
               }
            }
         }
         else if(tmpNet->getNumB() == 1){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(tmpCell->getPart() == _PartB && !tmpCell->getLocked()){
                  tmpCell->increGain();
                  if(tmpCell->getGain() > _PartB->getMaxGain())
                     _PartB->setMaxGain(tmpCell->getGain());
                  _PartB->deleteC(tmpCell, idx(tmpCell->getGain() - 1));
                  _PartB->insertC(tmpCell);
               }
            }
         }
         else {}
      }
      else if(p == BBB){
         if(tmpNet->getNumA() == 0){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(!tmpCell->getLocked()){
                  tmpCell->decreGain();
                  _PartB->deleteC(tmpCell, idx(tmpCell->getGain() + 1));
                  _PartB->insertC(tmpCell);
               }
            }
         }
         else if(tmpNet->getNumA() == 1){
            for(unsigned j = 0, m = tmpNet->getCellNum(); j < m; ++j){
               Cell* tmpCell = (*tmpNet)._cells[j];
               if(tmpCell->getPart() == _PartA && !tmpCell->getLocked()){
                  tmpCell->increGain();
                  if(tmpCell->getGain() > _PartA->getMaxGain())
                     _PartA->setMaxGain(tmpCell->getGain());
                  _PartA->deleteC(tmpCell, idx(tmpCell->getGain() - 1));
                  _PartA->insertC(tmpCell);
               }
            }
         }
         else {}
      }
      else 
         assert(false);
   }

   _PartA->updateMaxGain(_PartA->getMaxGain());
   _PartB->updateMaxGain(_PartB->getMaxGain());

   // cout << "***After..." << endl;
   // _PartA->printList();
   // _PartB->printList();

}


void NetManager::updateGain()
{
   int partialSum = 0;
   int maxPartialSum = INT_MIN; 
   vector<unsigned> solutions;

   // Calculate maxPartialSum and also collect the corresponding "Index" of _solSeq
   for(unsigned i = 0; i < _solSeq.size()-1; ++i){
      partialSum += _cellList[_solSeq[i]]->getGain();
      if(partialSum > maxPartialSum){
         maxPartialSum = partialSum;
         solutions.resize(0);
         solutions.push_back(i);
      }
      else if(partialSum == maxPartialSum){
         solutions.push_back(i);
      }
      else {}
   }
  
   if(maxPartialSum >= 0){
      int maxBalanceIdx = 0;
      int tmpNumA = _NumA, tmpNumB = _NumB;
      int balanceFactor = INT_MAX;          // abs(tmpNumA - tmpNumB)
      vector<int> bArray;                   // balanceFactors of every movement

      // Calculate balanceFactor of every movement
      for(unsigned i = 0; i < _solSeq.size()-1; ++i){
         Cell* tmpCell = _cellList[_solSeq[i]];
         if(tmpCell->getPart() == _PartA){
            --tmpNumA; ++tmpNumB;
         }
         else if(tmpCell->getPart() == _PartB){
            ++tmpNumA; --tmpNumB;
         }
         else
            assert(false);

         bArray.push_back(abs(tmpNumA - tmpNumB));
      }

      // Get the most balanced Index
      for(unsigned i = 0; i < solutions.size(); ++i)
         if(bArray[solutions[i]] < balanceFactor){
            balanceFactor = bArray[solutions[i]];
            maxBalanceIdx = solutions[i];
         }

      // Actually move the cells
      for(int i = 0; i <= maxBalanceIdx; ++i){
         Cell* tmpCell = _cellList[_solSeq[i]];
         if(tmpCell->getPart() == _PartA){
            tmpCell->moveA2B();
            tmpCell->setPart(_PartB);
            --_NumA; ++_NumB;
         }
         else if(tmpCell->getPart() == _PartB){
            tmpCell->moveB2A();
            tmpCell->setPart(_PartA);
            ++_NumA; --_NumB;
         }
         else
            assert(false);
      }
   }

   // Update _Gain
   _Gain = maxPartialSum;

}


void BucketList::insertC(Cell* cell)
{
   int tmpGain = cell->getGain();

   if(_list[idx(tmpGain)]){
      _list[idx(tmpGain)]->setFront(cell);
      cell->setBack(_list[idx(tmpGain)]);
      cell->setFront(0);
      _list[idx(tmpGain)] = cell;
   }
   else{
      _list[idx(tmpGain)] = cell;
      cell->setBack(0);
      cell->setFront(0);
   }

   if(tmpGain > _maxGain) _maxGain = tmpGain;

}


void BucketList::deleteC(Cell* cell, const int idx)
{
   Cell* front = cell->getFront();
   Cell* back  = cell->getBack();

   if(front != 0 && back != 0){
      front->setBack(back);
      back->setFront(front);
   }
   else if(front == 0 && back == 0){
      /*** Important!! ***/ 
      // Check if the cell is in _list ......
      // Do not make the same mistake AGAIN!!!
      if(_list[idx] == cell)
         _list[idx] = 0;
      else {}
   }
   else if(front == 0){
      _list[idx] = back;
      back->setFront(0);
   }
   else if(back == 0){
      front->setBack(0);
   }
   else
      assert(false);

   cell->setBack(0);
   cell->setFront(0);

}


void Cell::moveA2B()
{
   for(unsigned i = 0; i < _nets.size(); ++i)
      _nets[i]->moveA2B();
}


void Cell::moveB2A()
{
   for(unsigned i = 0; i < _nets.size(); ++i)
      _nets[i]->moveB2A();
}


inline int NetManager::idx(int i) const
{
   assert((i + _maxPinNum) >= 0);
   return (i + _maxPinNum);
}


inline int BucketList::idx(int i) const
{
   assert((i + _maxPinNum) >= 0);
   return (i + _maxPinNum);
}


void NetManager::clear()
{
   for(unsigned i = 0; i < _netList.size(); ++i)
      delete _netList[i];

   for(unsigned i = 0; i < _cellList.size(); ++i)
      delete _cellList[i];

   if(_PartA) delete _PartA;
   if(_PartB) delete _PartB;

}


inline bool NetManager::checkTime(const time_t& start, time_t& end) const
{
   end = clock();
   float t_used = (float)(end - start) / CLOCKS_PER_SEC;
   return (t_used > TIME_LIMIT);
}


inline bool NetManager::checkRepeat() const
{
   static int curGain = INT_MIN;
   static int counter = 0;

   if(_Gain == curGain){
      ++counter;
   }
   else{
      curGain = _Gain;
      counter = 0;
   }

   return (counter >= REPEAT_LIMIT);
}


inline bool NetManager::checkSameCutSize() const
{
   static unsigned cutsize = INT_MAX;
   static unsigned regsize = INT_MAX;

   if(_Gain <= 0) return false;

   if(cutsize < getCutSize()){
      regsize = cutsize;
      cutsize = getCutSize(false);
   }
   else{
      if(regsize == getCutSize(false)) {
         return true;
      }
      else{
         cutsize = getCutSize(false);
      }
   }
   
   return false;
}

/*******************************************
            Result  Msg
********************************************/

bool NetManager::writeResult(const char* outfile)
{
   fstream outf(outfile, ios::out);
   if(!outf){
      cerr << "Error: Cannot open output file \"" << outfile << "\"." << endl;
      return false; 
   }

   result(outf);
   cout << "> The result has been written to \"" << outfile << "\"." << endl;

   outf.close();

   return true;
}


void NetManager::result(ostream& os) const
{ 
   os << "Cutsize = " << getCutSize() << endl;

   os << "G1 " << _NumA << endl;
   for(unsigned i = 0; i < _cellList.size(); ++i){
      if(_cellList[i]->isValid())
         if(_cellList[i]->getPart() == _PartA)
            os << _cellList[i]->getName() << " ";
   }
   os << ";" << endl;

   os << "G2 " << _NumB << endl;
   for(unsigned i = 0; i < _cellList.size(); ++i){
      if(_cellList[i]->isValid())
         if(_cellList[i]->getPart() == _PartB)
            os << _cellList[i]->getName() << " ";
   }
   os << ";" << endl;
}


unsigned NetManager::getCutSize(bool calculate) const
{
   static unsigned cnt = 0;

   if(!calculate) 
      return cnt;
   else 
      cnt = 0;

   for(unsigned i = 0; i < _netList.size(); ++i)
      if(_netList[i]->getNumA() != 0 && _netList[i]->getNumB() != 0)
         ++cnt;
      
   return cnt; 

}


/*******************************************
            Debugging Msg
********************************************/

void BucketList::printList() const
{
   Cell* tmpCell = 0;
   cout << "BucketList = " << this << endl;
   cout << "maxGain = " << _maxGain << endl;
   for(unsigned i = 0; i < _list.size(); ++i){
      if(_list[i]){
         cout << "list[" << ((int)i - (int)_maxPinNum) << "] ";
         tmpCell = _list[i];
         do{
            cout << tmpCell->getNo() <<"(" << tmpCell->getLocked() << ") ";
            tmpCell = tmpCell->getBack();
         }while(tmpCell != 0);
         cout << endl;
      }
   }
   // for(unsigned i = 0; i < _list.size(); ++i){
   //    if(_list[i]){
   //       cout << "list[" << ((int)i - (int)_maxPinNum) << "] ";
   //       cout << _list[i]->getNo() <<"(" << _list[i]->getPart() << ") " << endl;
   //    }
   // }
   cout << "---------------" << endl;
}


void NetManager::printCellGain() const
{
   for(unsigned i = 0; i < _cellList.size(); ++i)
      cout << "c" << _cellList[i]->getNo() 
           << " gain = " << _cellList[i]->getGain() << endl;
}


ostream& operator << (ostream& os, const Cell& e) 
{
   for(unsigned i = 0; i < e._nets.size(); ++i)
      os << e._nets[i]->getName() << " ";
   return os;
}


ostream& operator << (ostream& os, const Net& e) 
{
   for(unsigned i = 0; i < e._cells.size(); ++i)
      os << e._cells[i]->getNo() << " ";
   return os;
}


void Cell::cellInfo() const
{
   cout << "No: " << _cellNo << endl;
   cout << "Part: " << _part << endl;
   cout << "Gain: " << _gain << endl;
   cout << "Locked: " << _locked << endl;

   if(_front)
      cout << "Front: " << _front->getNo() << endl;
   else
      cout << "Front: " << 0 << endl;

   if(_back)
      cout << "Back: " << _back->getNo() << endl;
   else
      cout << "Back: " << 0 << endl;

   cout << "------------------------" << endl;
}


void NetManager::printAllCell() const
{
   for(unsigned i = 0; i < _cellList.size(); ++i)
      _cellList[i]->cellInfo();
}


void NetManager::printAllNet() const
{
   for(unsigned i = 0; i < _netList.size(); ++i){
      cout << _netList[i]->getName() << ": ";
      for(unsigned j = 0; j < _netList[i]->getCellNum(); ++j)
         cout << _netList[i]->_cells[j]->getName() << " ";
      cout << endl;
   }
}