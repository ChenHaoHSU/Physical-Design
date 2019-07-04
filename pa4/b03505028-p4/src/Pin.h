/****************************************************************************
  FileName     [ Pin.h ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/
#ifndef PIN_H
#define PIN_H

#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include "Util.h"

using namespace std;

class Pin
{
public:
   Pin(int xx = 0, int yy = 0, string name = "DEFAULT_NAME")
      :_name(name) 
   {
      _node.first = xx;
      _node.second = yy;
      _grp = -1;
      _R1 = false;
      _R2 = false;
      _R3 = false;
      _R4 = false;
      _R5 = false;
      _R6 = false;
      _R7 = false;
      _R8 = false;
   }
   ~Pin() {}

   // get functions
   int x() const { return _node.first; }
   int y() const { return _node.second; }
   int grp() const { return _grp; }
   void setGrp(int g) { _grp = g; }
   Node node() const { return _node; }
   string name() const { return _name; }


   friend ostream& operator << (ostream& os, const Pin& p)
   {
      os << p.node();
      return os;
   }

   bool L() const { return _L; }
   bool R() const { return _R; }
   bool T() const { return _T; }
   bool B() const { return _B; }

   void setL() { _L = true; }
   void setR() { _R = true; }
   void setT() { _T = true; }
   void setB() { _B = true; }

   void updateLRTB(const Node& n)
   {
      // cout << _node << " " << n << " ... " << endl; 
      bool y = n.second - _node.second > 0;
      bool x = n.first - _node.first > 0;
      bool y0 = n.second - _node.second == 0;
      bool x0 = n.first - _node.first == 0;

      if(y && x0){
        // cout << "T" << endl;
        _T = true;
      }
      else if(!y && x0){ 
        // cout << "B" << endl;
         _B = true;
       }
      else if(x && y0){
        // cout << "R" << endl;
         _R = true;
       }
      else if(!x && y0){ 
        // cout << "L" << endl;
         _L = true;
       }
      else{
         // cout << "No!!" << endl;
      } 
      

   }

   void pushE(int e) { _edges.push_back(e); }
   int edgeSize() const { return _edges.size(); }
   int& edges(int i) { return _edges[i]; }
   void printR() const 
   {
      cout << _node << endl;
      cout << _R1 << " " << _R2 << " " << _R3 << " " << _R4 << " , " 
           << _R5 << " " << _R6 << " " << _R7 << " " << _R8 << endl;
      cout << endl;
   }

   bool   _R1;
   bool   _R2;
   bool   _R3;
   bool   _R4;
   bool   _R5;
   bool   _R6;
   bool   _R7;
   bool   _R8;

private:
   Node   _node;
   string _name;

   int    _grp;

   bool   _L; // left
   bool   _R; // right
   bool   _T; // top
   bool   _B; // bottom

   vector<int> _edges; // index of _mstList (sorted)

};


// for checking duplicate names of pins
class PinH
{
public:
   PinH(string s = ""):_name(s) {}
   ~PinH() {}

   string name() const { return _name; }

   size_t operator () () const
   {
      size_t k = 0, n = (_name.length() <= 5)? _name.length(): 5;
      for (size_t i = 0; i < n; ++i)
         k ^= (_name[i] << (i*6));
      return k;
   }

   bool operator == (const PinH& p) const 
   { 
      return (_name == p.name());
   }


private:
   string _name;
   
};

// for checking duplicate node of pins
class NodeH
{
public:
   NodeH(int x = 0, int y = 0):_x(x), _y(y) {}
   ~NodeH() {}
   
   int x() const { return _x; }
   int y() const { return _y; }

   Node node() const { return Node(_x, _y); }

   size_t operator () () const
   {
      size_t k = 0;
      k ^= (_x << 16);
      k ^= (_y << 16);
      k ^= (_x);
      k ^= (_y);
      return k;
   }

   bool operator == (const NodeH& p) const 
   { 
      return (_x == p.x() && _y == p.y());
   } 

   int _x;
   int _y;

};


class EGain
{
  friend class SteinerRouter;

public: 
   EGain(int ee1 = 0, int ee2 = 0, int g = 0) 
   {
      gain = g;
      e1 = ee1;
      e2 = ee2;
   }
   ~EGain() {}

   int   e1; // index of _mstList
   int   e2; // index of _mstList
   int   gain;

};



#endif // PIN_H