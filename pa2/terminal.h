/****************************************************************************
  FileName     [ terminal.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include "mydefine.h"

using namespace std;

class Terminal
{
public:
   Terminal() {
      _name = "YYY";
      _x = _y = 0;
   }
   ~Terminal() {}

   // Basic assign functions
   void setName(string s) { _name = s; }
   void setX(int x) { _x = x; }
   void setY(int y) { _y = y; }

   // Basic access functions
   string getName() const { return _name; }
   int getX() const { return _x; }
   int getY() const { return _y; }

   friend ostream& operator << (ostream& os, const Terminal& t)
   {
      os << t._name << " " << t._x << " " << t._y;
      return os;
   }

private:
   string _name;
   int    _x;
   int    _y;

};


class TerminalV
{
public:
   TerminalV(int n = 0, string s = ""): _no(n), _name(s) {}
   ~TerminalV() {}

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
   bool operator == (const TerminalV& n) const { return (_name == n.getName()); }

   
private:
   int    _no;
   string _name;

};


#endif // TERMINAL_H