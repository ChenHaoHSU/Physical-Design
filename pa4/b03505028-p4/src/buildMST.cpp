/****************************************************************************
  FileName     [ buildMSTandMBT.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/30 created ]
****************************************************************************/

#include "SteinerRouter.h"

using namespace std;


extern time_t start;

bool sortEdgeLen(const Edge& e1, const Edge& e2)
{
   return e1.len() < e2.len();
}


void SteinerRouter::buildMST()
{ 
   cout << "> Building MST ... " << endl;

   for(unsigned i = 0; i < _eList.size(); ++i){
      Edge& E = _eList[i];
      Pin& P1 = _pinList[E.v1()];
      Pin& P2 = _pinList[E.v2()];
      E.setLen(ADIFF(P1.x(), P2.x()) + ADIFF(P1.y(), P2.y()));
   }

   ::sort(_eList.begin(), _eList.end(), sortEdgeLen);

   // set every pin a grp
   _grps.resize(_numP);
   for(unsigned i = 0; i < _pinList.size(); ++i){
      _pinList[i].setGrp(i);
      _grps[i].push_back(i);
   }

   int g1, g2;
   size_t totalLen = 0;
   for(unsigned i = 0; i < _eList.size(); ++i){
      Edge& e = _eList[i];
      Pin& p1 = _pinList[e.v1()];
      Pin& p2 = _pinList[e.v2()];
      g1 = p1.grp();
      g2 = p2.grp();

      if(g1 == g2) continue;

      totalLen += (size_t)e.len();
      _mstList.push_back(e);
      mergeGrp(g1, g2);
   }

   _mstCost = totalLen;

   cout << "  - MST #Edges = " << _mstList.size() << endl;
   cout << "  - MST Length = " << _mstCost << endl;
   cout << "  - Time       = " << getTime(start) << " seconds. " << endl << endl;
   
   assert(_mstList.size() == (unsigned)(_numP - 1));

   // resultGNU(true);
   // resultGNU();
}


void SteinerRouter::mergeGrp(const int& g1, const int& g2)
{
   assert(!_grps[g1].empty() && !_grps[g2].empty());

   if(_grps[g1].size() < _grps[g2].size()){ // g1 die, g2 alive
      for(unsigned i = 0; i < _grps[g1].size(); ++i){
         _pinList[_grps[g1][i]].setGrp(g2);
         _grps[g2].push_back(_grps[g1][i]);
      }
   }
   else{
      for(unsigned i = 0; i < _grps[g2].size(); ++i){
         _pinList[_grps[g2][i]].setGrp(g1);
         _grps[g1].push_back(_grps[g2][i]);
      }
   }
}
