/****************************************************************************
  FileName     [ btree.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/7 created ]
****************************************************************************/

#ifndef BTREE_H
#define BTREE_H

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <climits>
#include <cstdlib>
#include <cstdio>

#include "block.h"
#include "mydefine.h"
#include "contourline.h"

using namespace std;

class Btree
{
public:
   Btree(size_t ns= NULL_SIZE) {
      _dummy = new Block;
      _root = _dummy;
      _size = 0;
      _nulls.resize(ns);
      for(unsigned i = 0; i < _nulls.size(); ++i){
         _nulls[i] = new Block;
         insertRnd(_nulls[i]);
      }
   }
   ~Btree() { 
      delete _dummy;
      for(unsigned i = 0; i < _nulls.size(); ++i)
         delete _nulls[i];
   }

   // Basic assign functions
   void setRoot(Block* b) { _root = b; }
   void setContourMgrW(int w) { _contourMgr.setWidth(w); }

   // Basic access functions
   int getWidth() const { return _width; }
   int getHeight() const { return _height; }
   int getArea() const { return (_width * _height); }
   bool empty() const { return (_size == 0); }
   Block* getRoot() const { return _root; }

   // B*-tree perturbation methods
   void swapB(Block*, Block*);
   void generalSwap(Block*, Block*);
   void connectSwap(Block*, Block*);
   void samepaSwap(Block*, Block*);
   void leftRotateB(Block*);
   void rightRotateB(Block*);
   Block* deleteAndInsert(Block*); // return the chosen null block*

   // Packing 
   void packing();
   void treePack(Block*);
   void compactX(Block*, bool);
   void compactY(Block*);

   // Binary tree operations
   void insertT(Block*);  // Basic Tree Insertion (key comparison have to be decided)
   void insertRnd(Block*);
   void deleteT(Block*);
   void setRoot(Block*, Block*, Block*);
   void treeTransplate(Block*, Block*, bool kill);
   void reset();
   Block* treeMin(Block* r) const;
   Block* treeMax(Block* r) const;
   Block* treeSuccessor(Block*) const;
   Block* treePredeccessor(Block*) const;

   // Debugging functions
   void printTree() const;    // print the tree Graph
   void printList() const;    // traverse the tree to print list
   void printInvList() const; // inversely traverse the tree to print inverse list
   void printLines() const;   // print the contourlines
   void treePrint(Block*, int) const;
   void play();


private:
   Block*         _dummy;        
   Block*         _root;
   size_t         _size;
   int            _width;
   int            _height;

   ContourMgr     _contourMgr;
   vector<Block*> _nulls;

};

#endif // BTREE_H