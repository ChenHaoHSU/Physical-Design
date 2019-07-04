/****************************************************************************
  FileName     [ btree.cpp ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/7 created ]
****************************************************************************/

#include "btree.h"

using namespace std;


void Btree::swapB(Block* b1, Block* b2)
{
   assert(b1 != 0 && b2 != 0);

   if(b1 == b2->_parent)
      connectSwap(b1, b2); // b1 is b2'parent
   else if(b1 == b2->_left || b1 == b2->_right)
      connectSwap(b2, b1); // b2 is b1'parent
   else if(b1->_parent == b2->_parent) // b1 and b2 have the same parent
      samepaSwap(b1, b2);
   else
      generalSwap(b1, b2);
}



void Btree::generalSwap(Block* b1, Block* b2)
{
   Block* tmpparent = b1->_parent;
   Block* tmpleft   = b1->_left;
   Block* tmpright  = b1->_right;

   // replace b2 with b1
   b1->_parent = b2->_parent;
   b1->_left   = b2->_left;
   b1->_right  = b2->_right;

   if(b2 == b2->_parent->_left)
      b2->_parent->_left = b1;
   else
      b2->_parent->_right = b1;

   if(b2->_left) b2->_left->_parent = b1;
   if(b2->_right) b2->_right->_parent = b1;


   if(b2->_parent == _dummy){
      _dummy->_left = _dummy->_right = b1;
      _root = b1;
   }

   // replace b1 with b2
   b2->_parent = tmpparent;
   b2->_left   = tmpleft;
   b2->_right  = tmpright;

   if(b1 == tmpparent->_left)
      tmpparent->_left = b2;
   else
      tmpparent->_right = b2;

   if(tmpleft) tmpleft->_parent = b2;
   if(tmpright) tmpright->_parent = b2;

   if(tmpparent == _dummy){
      _dummy->_left = _dummy->_right = b2;
      _root = b2;
   }

}


void Btree::connectSwap(Block* b1, Block* b2)
{
   // b1 is b2's parent
   static bool isLeft;

   Block* tmpparent = b1->_parent;
   Block* tmpleft   = b1->_left;
   Block* tmpright  = b1->_right;

   if(b2 == b1->_left) isLeft = true;
   else isLeft = false;

   if(isLeft){
      // b1 setting 
      b1->_left = b2->_left;
      b1->_right = b2->_right;
      if(b2->_left) b2->_left->_parent = b1;
      if(b2->_right) b2->_right->_parent = b1;
      b1->_parent = b2;

      // b2 setting 
      b2->_left = b1;
      b2->_right = tmpright;
      if(tmpright) tmpright->_parent = b2;
      b2->_parent = tmpparent;
      if(b1 == tmpparent->_left)
         tmpparent->_left = b2;
      else
         tmpparent->_right = b2;
   }
   else{
      // b1 setting 
      b1->_left = b2->_left;
      b1->_right = b2->_right;
      if(b2->_left) b2->_left->_parent = b1;
      if(b2->_right) b2->_right->_parent = b1;
      b1->_parent = b2;

      // b2 setting 
      b2->_right = b1;
      b2->_left = tmpleft;
      if(tmpleft) tmpleft->_parent = b2;
      b2->_parent = tmpparent;
      if(b1 == tmpparent->_left)
         tmpparent->_left = b2;
      else
         tmpparent->_right = b2;
   }

   if(tmpparent == _dummy){
      _dummy->_left = _dummy->_right = b2;
      _root = b2;
   }

}


void Btree::samepaSwap(Block* b1, Block* b2)
{
   static bool isLeft;

   if(b1 == b2) return;
   
   if(b1 == b1->_parent->_left) isLeft = true;
   else isLeft = false;

   Block* tmpparent = b1->_parent;
   Block* tmpleft   = b1->_left;
   Block* tmpright  = b1->_right;

   if(isLeft){
      tmpparent->_left = b2;
      tmpparent->_right = b1;
   }
   else{
      tmpparent->_left = b1;
      tmpparent->_right = b2; 
   }

   b1->_left = b2->_left;
   b1->_right = b2->_right;
   if(b2->_left) b2->_left->_parent = b1;
   if(b2->_right) b2->_right->_parent = b1;
   b2->_left = tmpleft;
   b2->_right = tmpright;
   if(tmpleft) tmpleft->_parent = b2;
   if(tmpright) tmpright->_parent = b2;

}

void Btree::leftRotateB(Block* x)
{
   assert(x != 0);

   Block* y = x->_right;
   if(y == 0) return;

   x->_right = y->_left; 

   if(y->_left != 0)
      y->_left->_parent = x;

   y->_parent = x->_parent;

   if(x->_parent == _dummy){
      _dummy->_right = _dummy->_left = y;
      y->_parent = _dummy;
      _root = y;
   }

   else if(x == x->_parent->_left)
      x->_parent->_left = y;

   else
      x->_parent->_right = y;

   y->_left = x;
   x->_parent = y;
   
}

void Btree::rightRotateB(Block* x)
{
   assert(x != 0);

   Block* y = x->_left;
   if(y == 0) return;

   x->_left = y->_right; 

   if(y->_right != 0)
      y->_right->_parent = x;

   y->_parent = x->_parent;

   if(x->_parent == _dummy){
      _dummy->_right = _dummy->_left = y;
      y->_parent = _dummy;
      _root = y;
   }

   else if(x == x->_parent->_left)
      x->_parent->_left = y;

   else
      x->_parent->_right = y;

   y->_right = x;
   x->_parent = y;

}


Block* Btree::deleteAndInsert(Block* b)
{
   int r = rand() % _nulls.size();
   swapB(b, _nulls[r]);
   return _nulls[r];
}


void Btree::packing()
{
   if(empty()) return;

   _width = _height = 0;
   
   _contourMgr.reset();
   _root->_x = _root->_y = 0;
   _contourMgr.insert(Interval(_root->_x, _root->_x + _root->_width), _root->_height);
   treePack(_root);

   // _contourMgr.reset();
   // _root->_x = _root->_y = 0;
   // _contourMgr.insert(Interval(_root->_y, _root->_y + _root->_height), _root->_width);
   // compactX(_root);

   // _contourMgr.reset();
   // _root->_x = _root->_y = 0;
   // _contourMgr.insert(Interval(_root->_x, _root->_x + _root->_width), _root->_height);
   // compactY(_root);

   assert(_contourMgr.getMaxY() == _height);

}


void Btree::treePack(Block* b)
{  
   Block* left = b->_left;
   Block* right = b->_right;
   Interval inter;

   // Update the boundary of the floorplan
   if(b->_x + b->_width > _width) 
      _width = b->_x + b->_width;
   if(b->_y + b->_height > _height) 
      _height = b->_y + b->_height;

   if(left){
      left->_x = b->_x + b->_width;
      inter.first = left->_x;
      inter.second = left->_x + left->_width;
      left->_y = _contourMgr.insert(inter, left->_height);
      treePack(left);
   }
   if(right){
      right->_x = b->_x;
      inter.first = right->_x;
      inter.second = right->_x + right->_width;
      right->_y = _contourMgr.insert(inter, right->_height);
      treePack(right);
   }
   return;
}


void Btree::compactX(Block* b, bool isFirst)
{
   if(b == 0 && isFirst) {
      b = _root;
      _contourMgr.reset();
      _root->_x = _root->_y = 0;
      _contourMgr.insert(Interval(_root->_y, _root->_y + _root->_height), _root->_width);
   }

   Block* left = b->_left;
   Block* right = b->_right;
   Interval inter;

   if(b->_x + b->_width > _width) 
      _width = b->_x + b->_width;
   if(b->_y + b->_height > _height) 
      _height = b->_y + b->_height;
   
   if(right){
      inter.first = right->_y;
      inter.second = right->_y + right->_height;
      right->_x = _contourMgr.insert(inter, right->_width);
      compactX(right, false);
   }
   if(left){
      inter.first = left->_y;
      inter.second = left->_y + left->_height;
      left->_x = _contourMgr.insert(inter, left->_width);
      compactX(left, false);
   }
   return;  
}



void Btree::compactY(Block* b)
{
   Block* left = b->_left;
   Block* right = b->_right;
   Interval inter;
   
   if(left){
      inter.first = left->_x;
      inter.second = left->_x + left->_width;
      left->_y = _contourMgr.insert(inter, left->_height);
      compactY(left);
   }
   if(right){
      inter.first = right->_x;
      inter.second = right->_x + right->_width;
      right->_y = _contourMgr.insert(inter, right->_height);
      compactY(right);
   }
   return;  
}


Block* Btree::treeMin(Block* r) const
{
   assert(r != 0);
   while(r->_left != 0){ r = r->_left; }
   return r;
}


Block* Btree::treeMax(Block* r) const
{
   assert(r != 0);
   while(r->_right != 0){ r = r->_right; }
   return r;
}


void Btree::setRoot(Block* r, Block* left = 0, Block* right = 0)
{
   _root = r;
   _root->_parent = _dummy;
   _dummy->_left = _dummy->_right = _root;
   _root->_left = left;
   _root->_right = right;
}


void Btree::treePrint(Block* pt, int n) const
{
   for(int i = 0; i < n;i++)
      cout << "  "; 

   if(pt == 0){
      cout << "[0]" << endl;
      return;
   }
   else{
      cout << pt->_name << endl;
      treePrint(pt->_right, n+1); // upper one: right
      treePrint(pt->_left , n+1); // lower ont: left
   }

}


void Btree::treeTransplate(Block* die, Block* cover, bool kill = true)
{
   if(cover == 0){
      if(die == (die->_parent)->_left) 
         (die->_parent)->_left = 0;
      else
         (die->_parent)->_right = 0;
   }
   else{
      cover->_parent = die->_parent;
      if(die == (cover->_parent)->_left) 
         (cover->_parent)->_left = cover;
      else
         (cover->_parent)->_right = cover;
   }
}


void Btree::insertT(Block* b) 
{ 
   if(empty()){
      _root = b;
      _dummy->_left = _dummy->_right = _root;
      b->_parent = _dummy;
   }
   else{
      Block* pt = _root;
      Block* pos = pt;
      while(pt != 0){
         pos = pt;
         if(b->_width > pt->_width) // wider width goes to left child
            pt = pt->_left;
         else
            pt = pt->_right;
      }
      if(b->_width > pos->_width)
         pos->_left = b;
      else
         pos->_right = b;

      b->_parent = pos;
   }
   _size++;
   assert(_size > 0);

}


void Btree::insertRnd(Block* b)
{ 
   static bool LR = false; // true: left, false: right

   if(empty()){
      _root = b;
      _dummy->_left = _dummy->_right = _root;
      b->_parent = _dummy;
   }
   else{
      Block* pt = _root;
      Block* pos = pt;

      while(pt != 0){
         pos = pt;
         if(rand() % 2){
            pt = pt->_left;
            LR = true;
         }
         else{
            pt = pt->_right;
            LR = false;
         }     
      }
      if(LR)
         pos->_left = b;
      else
         pos->_right = b;
      b->_parent = pos;
   }
   ++_size;
   assert(_size > 0);
}


void Btree::deleteT(Block* pt)
{
   assert(_size != 0);
   bool isRoot = false;
   if (pt->_parent == _dummy) isRoot = true;
   //case 1
   if(pt->_left == 0 && pt->_right == 0){
      if(!isRoot){
         if(pt == (pt->_parent)->_left) 
            (pt->_parent)->_left = 0;
         else
            (pt->_parent)->_right = 0;
      }
      else{
         _dummy->_left = 0;
         _dummy->_right = 0;
         _root = _dummy;
      }
   }
   //case 2
   else if (pt->_left != 0 && pt->_right == 0){
      if(!isRoot)
         treeTransplate(pt, pt->_left);
      else
         setRoot(pt->_left, (pt->_left)->_left, (pt->_left)->_right);
   }
   //case 3
   else if (pt->_left == 0 && pt->_right != 0){
      if(!isRoot)
         treeTransplate(pt, pt->_right);
      else
         setRoot(pt->_right, (pt->_right)->_left, (pt->_right)->_right);
   }
   //case 4
   else{
      Block* y = treeMin(pt->_right);
      if(!isRoot){
         if(y->_parent != pt){
            treeTransplate(y, y->_right, false);
            y->_right = pt->_right;
            (y->_right)->_parent = y;
         }
         y->_parent = pt->_parent;
         if(pt == (pt->_parent)->_right)
            (pt->_parent)->_right = y;
         else
            (pt->_parent)->_left = y;
         y->_left = pt->_left;
         (y->_left)->_parent = y;
      }
      else{
         if(y->_parent != pt){
            treeTransplate(y, y->_right, false);
            y->_right = pt->_right;
            if((pt->_right)->_right)
               (y->_right)->_parent = y;
         }
         setRoot(y, pt->_left, y->_right);
         (pt->_left)->_parent = _root;
      }
   }

   --_size;
   assert(_size >= 0);
   return;
}


Block* Btree::treeSuccessor(Block* r) const
{
   assert(r != 0);
   if (r->_right != 0) 
      return treeMin(r->_right);

   Block* tmp = r->_parent;
   while (tmp != 0 && r == tmp->_right){
      r = tmp;
      tmp = tmp->_parent;
   }
   if(tmp == 0) 
      return r;
   else 
      return tmp;       //the last one will return _dummy!
}


Block* Btree::treePredeccessor(Block* r) const
{
   assert(r != 0);
   if (r->_left != 0) 
      return treeMax(r->_left);

   Block* tmp = r->_parent;
   while (tmp != 0 && r == tmp->_left){
      r = tmp;
      tmp = tmp->_parent;
   }
   if(tmp == 0) 
      return r;
   else 
      return tmp;       //the last one will return _dummy!
}


void Btree::printList() const
{
   Block* pt = treeMin(_root);
   int i = 0;
   while(pt != _dummy){
      cout << "[" << i++ << "] " << pt->getName() << endl;
      pt = treeSuccessor(pt);
   }
}


void Btree::printInvList() const
{
   Block* pt = treeMax(_root);
   int i = 0;
   while(pt != _dummy){
      cout << "[" << i++ << "] " << pt->getName() << endl;
      pt = treePredeccessor(pt);
   }
}

void Btree::printTree() const 
{ 
   if(!empty()) 
     treePrint(_root, 0); 
}

void Btree::printLines() const 
{ 
   _contourMgr.printLines(); 
}


void Btree::reset()
{
_root = _dummy;
_dummy->_left = _dummy->_right = 0;
_size = 0;
}


void Btree::play()
{
   
}

