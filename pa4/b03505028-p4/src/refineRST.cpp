/****************************************************************************
  FileName     [ refineRST.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/31 created ]
****************************************************************************/

#include "SteinerRouter.h"

using namespace std;

extern time_t start;

void SteinerRouter::refineRST()
{
   cout << "> Refining RST ... " << endl;

   int hSize = 0;
   int vSize = 0;

   /* refine V-lines */
   int listPt = 0;
   int nowX = 0;
   if(!_vList.empty()) nowX = _vList[0].x1();
   vector<vector<int> > grpList;
   grpList.resize(_vList.size());
   for(int i = 0, n = _vList.size(); i < n; ++i){
      if(nowX == _vList[i].x1()){
         grpList[listPt].push_back(i);
      }
      else{
         listPt++;
         nowX = _vList[i].x1();
         grpList[listPt].push_back(i);
      }
   }

   int nowBY = 0;
   int nowPt = 0;
   for(unsigned i = 0; i < grpList.size(); ++i){
      if(grpList[i].empty()) break;

      nowBY = _vList[ grpList[i][0] ].y1() + 1; // sure for init
      nowPt = grpList[i][0];
      for(unsigned j = 0; j < grpList[i].size(); ++j){
         int& idx = grpList[i][j];
         if(nowBY > _vList[idx].y1()){
            nowBY = max(nowBY, _vList[idx].y2());
            continue;
         }
         else{
            VlineOverlapRemoval(nowPt, idx - 1);
            nowPt = idx;
            nowBY = _vList[idx].y2();
         }
      }
      VlineOverlapRemoval(nowPt, grpList[i].back());
   }

   vSize = _rsmtList.size();

   //////////////////////////////////////////////
   //////////////////////////////////////////////

   /* refine H-lines */
   grpList.clear();
   grpList.resize(_hList.size());

   listPt = 0;
   int nowY = 0;
   if(!_hList.empty()) nowY = _hList[0].y1();
   for(int i = 0, n = _hList.size(); i < n; ++i){
      if(nowY == _hList[i].y1()){
         grpList[listPt].push_back(i);
      }
      else{
         listPt++;
         nowY = _hList[i].y1();
         grpList[listPt].push_back(i);
      }
   }
 
   int nowBX = 0;
   nowPt = 0;
   for(unsigned i = 0; i < grpList.size(); ++i){
      if(grpList[i].empty()) break;

      nowBX = _hList[ grpList[i][0] ].x1() + 1; // sure for init
      nowPt = grpList[i][0];
      for(unsigned j = 0; j < grpList[i].size(); ++j){
         int& idx = grpList[i][j];
         if(nowBX > _hList[idx].x1()){
            nowBX = max(nowBX, _hList[idx].x2());
            continue;
         }
         else{
            HlineOverlapRemoval(nowPt, idx - 1);
            nowPt = idx;
            nowBX = _hList[idx].x2();
         }
      }
      HlineOverlapRemoval(nowPt, grpList[i].back());
   }

   int eSize = _rsmtList.size();
   hSize = eSize - vSize;

   calRSMTCost();

   double delta = _mstCost - _rsmtCost;
   double mstCost = _mstCost;

   cout << "  - RSMT #H-lines = " << hSize << endl;
   cout << "  - RSMT #V-lines = " << vSize << endl;
   cout << "  - RSMT #Edges   = " << eSize << endl;
   cout << "  - RSMT Length   = " << _rsmtCost << endl;
   cout << "  - Improvement   = " << delta / mstCost << endl;
   cout << "  - Time          = " << getTime(start) << " seconds. " << endl << endl;

   // resultGNU(true);
   // resultGNU();

}


bool sortNodeY(const Node& n1, const Node& n2)
{
   return n1.second < n2.second;
}


bool sortNodeX(const Node& n1, const Node& n2)
{
   return n1.first < n2.first;
}


void SteinerRouter::VlineOverlapRemoval(const int& f, const int& b)
{
   // f : front
   // b : back

   vector<Node> nodeList;

   if(f > b || f < 0 || b < 0) 
      return;
   else if(f == b){
      _rsmtList.push_back(_vList[f]);
      return;
   }
   else{
      for(int i = f; i <= b; ++i){
         nodeList.push_back(_vList[i].n1());
         nodeList.push_back(_vList[i].n2());
      }

      ::sort(nodeList.begin(), nodeList.end(), sortNodeY);

      for(unsigned i = 0; i < nodeList.size() - 1; ++i){
         if(nodeList[i] != nodeList[i + 1]){
            _rsmtList.push_back(EdgeHV(nodeList[i], nodeList[i + 1]));
         }
      }
   }
}



void SteinerRouter::HlineOverlapRemoval(const int& f, const int& b)
{
   // f : front
   // b : back

   vector<Node> nodeList;

   if(f > b || f < 0 || b < 0) 
      return;
   else if(f == b){
      _rsmtList.push_back(_hList[f]);
      return;
   }
   else{
      for(int i = f; i <= b; ++i){
         nodeList.push_back(_hList[i].n1());
         nodeList.push_back(_hList[i].n2());
      }

      ::sort(nodeList.begin(), nodeList.end(), sortNodeX);

      for(unsigned i = 0; i < nodeList.size() - 1; ++i){
         if(nodeList[i] != nodeList[i + 1]){
            _rsmtList.push_back(EdgeHV(nodeList[i], nodeList[i + 1]));
         }
      }
   }
}


void SteinerRouter::calRSMTCost()
{
   size_t totalLen = 0;
   for(unsigned i = 0; i < _rsmtList.size(); ++i)
      totalLen += (size_t)_rsmtList[i].len();
   _rsmtCost = totalLen;
}


