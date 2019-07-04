/****************************************************************************
  FileName     [ SteinerRouter.h ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/
#ifndef STEINER_ROUTER_H
#define STEINER_ROUTER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <climits>
#include <sstream>
#include <map>

#include "myHashSet.h"
#include "Pin.h"
#include "Edge.h"
#include "Util.h"

using namespace std;

class SteinerRouter
{
public:
   SteinerRouter() {}
   ~SteinerRouter() {}

   // parsers
   bool parseAll(char*);
   bool parseBoundary(ifstream&);
   bool parseNumPins(ifstream&);
   bool parseAllPins(ifstream&);

   // main 
   void buildRSG();
   void buildMST();
   void buildSteiner();
   void refineRST();

   void mergeGrp(const int&, const int&);
   void steinerEgdes(const int&, const int&);
   void buildHVList();
   void VlineOverlapRemoval(const int&, const int&);
   void HlineOverlapRemoval(const int&, const int&);

   // result
   bool writeResult(char*);
   void result(ostream&) const;
   void resultGNU(bool autoplay = false) const;
   void outputGNUgraph() const;
   void outputGNUtree() const;

   // debugging msg
   void printPinList() const;
   void printEList() const;
   void printMSTList() const;
   void printRSTList() const;
   void printRSMTList() const;
   void printHList() const;
   void printVList() const;
   void printEGainList() const;

private:
   Node                 _boundaryLL;
   Node                 _boundaryUR;
   int                  _numP;
   string               _benchmark;

   vector<Pin>          _pinList;
   vector<Node>          _order;
   vector<Edge>         _eList;

   vector<Edge>         _mstList;
   vector<EdgeHV>       _rstList;

   vector<EdgeHV>       _hList;
   vector<EdgeHV>       _vList;
  

   vector<EdgeHV>       _rsmtList;

   vector<vector<int> > _grps;

   vector<EGain>        _eGainList;

   size_t               _mstCost;
   size_t               _rstCost;
   size_t               _rsmtCost;

   inline Region getRegion(const Pin&, const Pin&);
   inline bool isInBoundary(const Node&);
   void calRSTCost();
   void calRSMTCost();
   int calGain(const int&, const int&);

};


#endif // STEINER_ROUTER_H