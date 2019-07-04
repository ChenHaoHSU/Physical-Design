/****************************************************************************
  FileName     [ contourline.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/17 created ]
****************************************************************************/

#ifndef CONTOURLINE_H
#define CONTOURLINE_H

#include <iostream>
#include <climits>
#include <cassert>

#include "mydefine.h"

using namespace std;


class ContourLine
{
   friend class ContourMgr;

public:
   ContourLine(ContourLine* f = 0, ContourLine* b = 0, Interval i = Interval(0,0), int h = 0):
         _front(f), _back(b), _intvl(i), _height(h) {}
   ~ContourLine() {}

   int getFirst() const { return _intvl.first; }
   int getSecond() const { return _intvl.second; }
   void resizeFromFront(int s) { _intvl.second = s; }
   void resizeFromBack(int s) { _intvl.first = s; }
   bool isValid() const { return ((_intvl.second - _intvl.first) > 0); }

private:
   ContourLine* _front; // closer to _dummy
   ContourLine* _back;  // farther from _dummy
   Interval     _intvl; // interval of this line
   int          _height; // represent the height of this interval

};


class ContourMgr
{
public:
   ContourMgr() {
      _dummy = new ContourLine;
      _maxY = 0;
      setWidth(INT_MAX); 
   }
   ~ContourMgr() { clear(); delete _dummy; }

   int getMaxY() const { return _maxY; }
   void setWidth(int);

   int insert(const Interval, int);
   void deleteL(ContourLine* die);
   void reset();
   void printLines() const;

   void clear();

private:
   ContourLine* _dummy;
   int          _width;
   int          _maxY;

};

#endif // CONTOURLINE_H