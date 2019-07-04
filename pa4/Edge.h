/****************************************************************************
  FileName     [ Edge.h ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/29 created ]
****************************************************************************/
#ifndef EDGE_H
#define EDGE_H

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>

#include "Util.h"

using namespace std;

class Edge
{
public:
   Edge(int v1 = 0, int v2 = 0):_v1(v1), _v2(v2) { _done = false; }
   ~Edge() {}

   int v1() const { return _v1; }
   int v2() const { return _v2; }
   int len() const { return _len; }
   void setLen(int l) { _len = l; }

   bool operator == (const Edge& e)
   {
      return (_v1 == e.v1() && _v2 == e.v2());
   }

   void pushNeighbor(int n) { _neighbor.push_back(n); }
   int& neighbor(int i) { return _neighbor[i]; }
   int neighborSize() const { return _neighbor.size(); }
   void popback() { _neighbor.pop_back(); }

   bool done() const { return _done; }
   void setDone() { _done = true; }
   void resetDone() { _done = false; }

   static bool sortNB(const int& a, const int& b)
   {
      return a > b;
   }

   void sortNeighbor() { std::sort(_neighbor.begin(), _neighbor.end(), sortNB); }
   void printNeighbor() const
   {
      for(unsigned i = 0; i < _neighbor.size(); ++i)
         cout << _neighbor[i] << " ";
   }



private:
   int _v1; // index of _pinList
   int _v2; // index of _pinList

   int _len;

   bool _done;

   vector<int> _neighbor; // index of _mstList (sorted)

};


class EdgeHV
{
public:
   EdgeHV() {}
   // EdgeHV(Node n1, Node n2) : _n1(n1), _n2(n2) {}
   EdgeHV(Node n1, Node n2) 
   {
      // V-line
      if(n1.first == n2.first){
         if(n1.second < n2.second){
            _n1 = n1;
            _n2 = n2;
         }
         else{
            _n1 = n2;
            _n2 = n1;
         }
      }
      // H-line
      else if(n1.second == n2.second){
         if(n1.first < n2.first){
            _n1 = n1;
            _n2 = n2;
         }
         else{
            _n1 = n2;
            _n2 = n1;
         }
      }
      else{
         cout << n1 << " " << n2 << endl;
         assert(false);
      }
   }
   
   ~EdgeHV() {}

   string HVstr() const
   {
      if(_n1.first == _n2.first)
         return "V-line";
      else if(_n1.second == _n2.second)
         return "H-line";
      else{
         cout << "QQ... " << _n1 << " " << _n2 << endl;
         assert(false);
      }
   }

   LINE isHV() const
   {
      if(_n1.first == _n2.first && _n1.second != _n2.second)
         return VLINE;
      else if(_n1.second == _n2.second && _n1.first != _n2.first)
         return HLINE;
      else{
         return TOTLINE;
      }
   }

   int y1() const { return _n1.second; }
   int x1() const { return _n1.first; }
   int y2() const { return _n2.second; }
   int x2() const { return _n2.first; }

   int minX() const { return min(_n1.first, _n2.first); }
   int minY() const { return min(_n1.second, _n2.second); }
   int maxX() const { return max(_n1.first, _n2.first); }
   int maxY() const { return max(_n1.second, _n2.second); }

   Node n1() const { return _n1; }
   Node n2() const { return _n2; }

   int len() const
   {
      return (ADIFF(_n1.first, _n2.first) + ADIFF(_n1.second, _n2.second));
   }

private:
   Node _n1;
   Node _n2;

};

#endif // EDGE_H