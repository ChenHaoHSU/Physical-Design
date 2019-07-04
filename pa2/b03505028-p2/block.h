/****************************************************************************
  FileName     [ block.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <cassert>
#include "mydefine.h"

using namespace std;


class Block
{
   friend class Btree;
   friend class FloorPlanner;

public:
   Block() {
      _name = "XXX";
      _width = _height = _x = _y = 0;
      _parent = _left = _right = 0;
   }
   ~Block() {}
   
   // Basic assign functions
   void setName(string s) { _name = s; }
   void setWidth(int w) { _width = w; }
   void setHeight(int h) { _height = h; }
   void setX(int x) { _x = x; }
   void setY(int y) { _y = y; }
   void setParent(Block* b) { _parent = b; }
   void setLeft(Block* b) { _left = b; }
   void setRight(Block* b) { _right = b; }

   // Basic access functions
   string getName() const { return _name; }
   int getWidth() const { return _width; }
   int getHeight() const { return _height; }
   int getX() const { return _x; }
   int getY() const { return _y; }
   int getArea() const { return (_width * _height); }
   Block* getParent() const { return _parent; }
   Block* getLeft() const { return _left; }
   Block* getRight() const { return _right; }

   void sleep()
   {
      if(_width >= _height) return;
      int tmp = _width;
      _width = _height;
      _height = tmp;
   }

   void stand()
   {
      if(_width <= _height) return;
      int tmp = _width;
      _width = _height;
      _height = tmp;
   }

   void rotateXY()
   {
      rotate();
      int tmp = _x;
      _x = _y;
      _y = tmp;
   }

   void rotate() 
   {
      // rotate means swap _width and _height
      int tmp = _width;
      _width = _height;
      _height = tmp;
   }

   friend ostream& operator << (ostream& os, const Block& b)
   {
      os << b._name << " " << b._width << " " << b._height;
      return os;
   }

   void printPLR() const
   {
      cout << _name << " ";
      cout << "P: " << (_parent == 0 ? "0" : _parent->getName()) << " ";
      cout << "L: " << (_left == 0 ? "0" : _left->getName()) << " ";
      cout << "R: " << (_right == 0 ? "0" : _right->getName());
      cout << endl;
   }

private:
   string _name;
   
   int    _width;
   int    _height;
   int    _x;    
   int    _y;

   Block* _parent;
   Block* _left;
   Block* _right;

};


class BlockV
{
public:
   BlockV(int n = 0, string s = ""): _no(n), _name(s) {}
   ~BlockV() {}

   string getName() const { return _name; }
   int getNo () const { return _no; }

   size_t operator () () const
   {
      size_t k = 5000011;
      int n = (_name.length() > 4 ? 4 : _name.length());
      for(int i = 0; i < n; ++i)
         k ^= ((size_t)_name[i] << (i*8));
      return k;
   }
   bool operator == (const BlockV& n) const { return (_name == n.getName()); }

private:
   int    _no;
   string _name;
   
};



class BlockR
{
   friend class FloorPlanner;

public:
   BlockR(int x = 0, int y = 0, int w = 0, int h = 0):
            _x(x), _y(y), _width(w), _height(h) {}
   ~BlockR() {}

private:
   int _x;
   int _y;
   int _width;
   int _height;
};

#endif // BLOCK_H