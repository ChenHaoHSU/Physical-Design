/****************************************************************************
  FileName     [ net.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#ifndef NET_H
#define NET_H

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <climits>

#include "block.h"
#include "terminal.h"

using namespace std;

class Net
{
public:
   Net() { _blocks.resize(0); _terminals.resize(0); }
   ~Net() {}

   void pushBlock(Block* b) { _blocks.push_back(b); }
   void pushTerminal(Terminal* t) { _terminals.push_back(t); }

   int getHPWL() const
   {
      // Looking for the max X, Y and min X, Y
      static int minX, maxX, minY, maxY;
      static int x, y, w, h, mx, my; // tmp x y w h
      
      minX = minY = INT_MAX;
      maxX = maxY = INT_MIN;

      for(unsigned i = 0, n = _blocks.size(); i < n; ++i){
         x = _blocks[i]->getX();
         y = _blocks[i]->getY();
         w = _blocks[i]->getWidth();
         h = _blocks[i]->getHeight();
         mx = x + (w / 2);
         my = y + (h / 2);

         if(mx > maxX) maxX = mx;
         if(mx < minX) minX = mx;
         if(my > maxY) maxY = my;
         if(my < minY) minY = my;
      }

      for(unsigned i = 0, n = _terminals.size(); i < n; ++i){
         x = _terminals[i]->getX();
         y = _terminals[i]->getY();

         if(x > maxX) maxX = x;
         if(x < minX) minX = x;
         if(y > maxY) maxY = y;
         if(y < minY) minY = y;
      }

      return ((maxX - minX) + (maxY - minY));
   }


   friend ostream& operator << (ostream& os, const Net& n)
   {
      cout << "degree: " << n._blocks.size() << endl;
      for(unsigned i = 0; i < n._blocks.size(); ++i)
         os << n._blocks[i]->getName() << endl;
      for(unsigned i = 0; i < n._terminals.size(); ++i)
         os << n._terminals[i]->getName() << endl;
      os << "-----------------------";
      return os;
   }

private:
   vector<Block*>    _blocks;
   vector<Terminal*> _terminals;

};

#endif // NET_H