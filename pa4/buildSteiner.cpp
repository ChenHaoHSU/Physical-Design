/****************************************************************************
  FileName     [ buildSteiner.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/30 created ]
****************************************************************************/

#include "SteinerRouter.h"

using namespace std;

extern time_t start;

bool secondTime = false;
int stage = 0;

bool sortEdgeLenL2S(const Edge& e1, const Edge& e2) // long to short
{
   return e1.len() > e2.len();
}

bool sortHList(const EdgeHV& e1, const EdgeHV& e2)
{
   if(e1.y1() < e2.y1()) return true;
   else if(e1.y1() == e2.y1() && e1.minX() < e2.minX()) return true;
   else return false;
}

bool sortVList(const EdgeHV& e1, const EdgeHV& e2)
{
   if(e1.x1() < e2.x1()) return true;
   else if(e1.x1() == e2.x1() && e1.minY() < e2.minY()) return true;
   else return false;
}

bool sortEGain(const EGain& e1, const EGain& e2)
{
   return e1.gain > e2.gain;
}

void SteinerRouter::buildSteiner()
{
   cout << "> Building RST ... " << endl;

   // sort mst edges by length from long to short
   ::sort(_mstList.begin(), _mstList.end(), sortEdgeLenL2S);

   // assign every node to edge
   for(unsigned i = 0; i < _mstList.size(); ++i){
      Pin& p1 = _pinList[_mstList[i].v1()];
      Pin& p2 = _pinList[_mstList[i].v2()];
      p1.pushE(i);
      p2.pushE(i);
   }

   // assign every neighbor edge to edge
   // from its two nodes
   for(int i = 0; i < (int)_mstList.size(); ++i){
      Pin& p1 = _pinList[_mstList[i].v1()];
      Pin& p2 = _pinList[_mstList[i].v2()];

      for(int j = 0; j < p1.edgeSize(); ++j)
         if(p1.edges(j) != i)
            _mstList[i].pushNeighbor(p1.edges(j));

      for(int j = 0; j < p2.edgeSize(); ++j)
         if(p2.edges(j) != i)
            _mstList[i].pushNeighbor(p2.edges(j));
   }

   for(unsigned i = 0; i < _mstList.size(); ++i)
      _mstList[i].sortNeighbor();

   ////////////////
   for(int i = 0; i < (int)_mstList.size(); ++i){
      Edge& nowE = _mstList[i];

      for(int j = 0; j < nowE.neighborSize(); ++j){
         int nei = nowE.neighbor(j);
         // if(nei < i){
            _eGainList.push_back(EGain(i, nei, calGain(i, nei)));
         // }
      }
   }
   ::sort(_eGainList.begin(), _eGainList.end(), sortEGain);

   int idx1, idx2;
   secondTime = true;
   stage = 1;
   // Node nn = Node( 6454545, 3923726 );

   for(unsigned i = 0; i < _eGainList.size(); ++i){
      idx1 = _eGainList[i].e1;
      idx2 = _eGainList[i].e2;

      if(_mstList[idx1].done() || _mstList[idx2].done())
         continue;

      _mstList[idx1].setDone();
      _mstList[idx2].setDone();
      steinerEgdes(idx1, idx2);

      // Node p1 = _pinList[_mstList[idx1].v1()].node();
      // Node p2 = _pinList[_mstList[idx1].v2()].node();
      // Node p3 = _pinList[_mstList[idx2].v1()].node();
      // Node p4 = _pinList[_mstList[idx2].v2()].node();

      // if(p1 == nn || p2 == nn || p3 == nn || p4 == nn){

      //    _pinList[_mstList[idx1].v1()].printR();
      //    _pinList[_mstList[idx1].v2()].printR();
      //    _pinList[_mstList[idx2].v1()].printR();
      //    _pinList[_mstList[idx2].v2()].printR();
      //    // resultGNU();
      // }
   }

   // printMSTList();
   // resultGNU();

   int minus_one = -1;
   for(unsigned i = 0; i < _mstList.size(); ++i){
      if(_mstList[i].done()) 
         continue;
      else{
         steinerEgdes(i, minus_one);
      }


      // Node p1 = _pinList[_mstList[i].v1()].node();
      // Node p2 = _pinList[_mstList[i].v2()].node();

      // if(p1 == nn || p2 == nn){
      //    _pinList[_mstList[i].v1()].printR();
      //    _pinList[_mstList[i].v2()].printR();
      //    // resultGNU();
      // }


   }
   ///////////////


/*
   int selectE = 0;
   secondTime = false;
   for(unsigned k = 0; k < 2; ++k){
      for(unsigned i = 0; i < _mstList.size(); ++i){
         Edge& nowE = _mstList[i];

         if(nowE.done()) continue;

         nowE.setDone();
         selectE = -1;

         for(int j = nowE.neighborSize() - 1; j >= 0; --j){
            if(!_mstList[nowE.neighbor(j)].done()){
               selectE = nowE.neighbor(j);
               break;
            }
         }
         steinerEgdes(i, selectE);
         // resultGNU();

      }
      secondTime = true;
   }
*/

   calRSTCost();
   buildHVList();
   cout << "  - RST #H-lines = " << _hList.size() << endl;
   cout << "  - RST #V-lines = " << _vList.size() << endl;
   cout << "  - RST #Edges   = " << (_vList.size() + _hList.size()) << endl;
   cout << "  - RST Length   = " << _rstCost << endl;
   cout << "  - Time         = " << getTime(start) << " seconds. " << endl << endl;

   // resultGNU(true);
   // resultGNU();
}


int SteinerRouter::calGain(const int& m, const int& s)
{
   Node tmpNode1;
   Node tmpNode2;

   int mp1 = _mstList[m].v1();
   int mp2 = _mstList[m].v2();

   int sp1 = _mstList[s].v1();
   int sp2 = _mstList[s].v2();

   // cout << "mainE = " << _pinList[_mstList[m].v1()].node() << " " 
   //      <<_pinList[_mstList[m].v2()].node() << endl;
   // cout << "selectE = " << _pinList[_mstList[s].v1()].node() << " " 
   //      <<_pinList[_mstList[s].v2()].node() << endl;

   // common node
   int commonV = (mp1 == sp1 || mp1 == sp2 ? mp1 : mp2);
   // main edge's another node
   int m_otherV = (commonV == mp1 ? mp2 : mp1);
   // selected edge's another node
   int s_otherV = (commonV == sp1 ? sp2 : sp1);

   Node commonN = _pinList[commonV].node();
   Node m_otherN = _pinList[m_otherV].node();
   Node s_otherN = _pinList[s_otherV].node();

   bool csX = (commonN.first - s_otherN.first > 0);
   bool csY = (commonN.second - s_otherN.second > 0);
   bool cmX = (commonN.first - m_otherN.first > 0);
   bool cmY = (commonN.second - m_otherN.second > 0);

   // cout << "check .... " << endl;
   // cout << "commonN = " << commonN << endl;
   // cout << "m_otherN = " << m_otherN << endl;
   // cout << "s_otherN = " << s_otherN << endl;

   // case1: opposite region
   if(csX ^ cmX && csY ^ cmY){
      return 0;
   }
   // case3: same region
   else if(!(csX ^ cmX) && !(csY ^ cmY)){
      tmpNode1.first = (ADIFF(commonN.first, m_otherN.first) < ADIFF(commonN.first, s_otherN.first)) ?
                       m_otherN.first : s_otherN.first;
      tmpNode1.second = (ADIFF(commonN.second, m_otherN.second) < ADIFF(commonN.second, s_otherN.second)) ?
                       m_otherN.second : s_otherN.second;
      return ADIFF(tmpNode1.first, commonN.first);
      return max(ADIFF(tmpNode1.first, commonN.first), ADIFF(tmpNode1.second, commonN.second));

   }
   else{ // case2: neighbor region
      if(csY ^ cmY){ // common X, opposite Y
         return min(ADIFF(commonN.first, m_otherN.first), ADIFF(commonN.first, s_otherN.first));
      }
      else{ // common Y, opposite X
         return min(ADIFF(commonN.second, m_otherN.second), ADIFF(commonN.second, s_otherN.second));
      }

   }
}


void SteinerRouter::steinerEgdes(const int& m, const int& s)
{
   // m: index of _mstList of main edge
   // s: index of _mstList of selected edge

   Node tmpNode1;
   Node tmpNode2;

   if(s < 0){  // s == -1
      Pin& tmpP1 = _pinList[_mstList[m].v1()];
      Pin& tmpP2 = _pinList[_mstList[m].v2()];
      Node tmpN1 = tmpP1.node();
      Node tmpN2 = tmpP2.node();

      bool isC_H = tmpN1.second > tmpN2.second;
      bool isC_R = tmpN1.first > tmpN2.first;

      if((tmpP1._R2 && !isC_H) || (tmpP1._R3 && !isC_H ) 
        || (tmpP1._R6 && isC_H) || (tmpP1._R7 && isC_H )
        || (tmpP2._R5 && !isC_R ) || (tmpP2._R4 && !isC_R ) 
        || (tmpP2._R1 && isC_R ) || (tmpP2._R8 && isC_R )){
         tmpNode1.first = tmpN1.first;
         tmpNode1.second = tmpN2.second;
         _rstList.push_back(EdgeHV(tmpN1, tmpNode1));
         _rstList.push_back(EdgeHV(tmpN2, tmpNode1));
      
      }
      else{
         tmpNode1.first = tmpN2.first;
         tmpNode1.second = tmpN1.second;
         _rstList.push_back(EdgeHV(tmpN1, tmpNode1));
         _rstList.push_back(EdgeHV(tmpN2, tmpNode1));
      
      }

      tmpP1.updateLRTB(tmpNode1);
      tmpP2.updateLRTB(tmpNode1);

      return;
   }

   int mp1 = _mstList[m].v1();
   int mp2 = _mstList[m].v2();

   int sp1 = _mstList[s].v1();
   int sp2 = _mstList[s].v2();

   // cout << "mainE = " << _pinList[_mstList[m].v1()].node() << " " 
   //      <<_pinList[_mstList[m].v2()].node() << endl;
   // cout << "selectE = " << _pinList[_mstList[s].v1()].node() << " " 
   //      <<_pinList[_mstList[s].v2()].node() << endl;

   // common node
   int commonV = (mp1 == sp1 || mp1 == sp2 ? mp1 : mp2);
   // main edge's another node
   int m_otherV = (commonV == mp1 ? mp2 : mp1);
   // selected edge's another node
   int s_otherV = (commonV == sp1 ? sp2 : sp1);

   Node commonN = _pinList[commonV].node();
   Node m_otherN = _pinList[m_otherV].node();
   Node s_otherN = _pinList[s_otherV].node();

   bool csX = (commonN.first - s_otherN.first > 0);
   bool csY = (commonN.second - s_otherN.second > 0);
   bool cmX = (commonN.first - m_otherN.first > 0);
   bool cmY = (commonN.second - m_otherN.second > 0);

   // cout << "check .... " << endl;
   // cout << "commonN = " << commonN << endl;
   // cout << "m_otherN = " << m_otherN << endl;
   // cout << "s_otherN = " << s_otherN << endl;

   // case1: opposite region
   if(csX ^ cmX && csY ^ cmY){
      // if(!secondTime){
         _mstList[m].resetDone();
         _mstList[s].resetDone();
         return;
      // }
      Pin& commonP = _pinList[commonV];
      Pin& m_otherP = _pinList[m_otherV];

      bool isC_H = commonN.second > m_otherN.second;
      bool isC_R = commonN.first > m_otherN.first;

      if((isC_H && commonP.B()) || (isC_R && m_otherP.L())
         || (!isC_H && commonP.T()) || (!isC_R && m_otherP.R())){
         tmpNode1.first = commonN.first;
         tmpNode1.second = m_otherN.second;
         _rstList.push_back(EdgeHV(commonN, tmpNode1));
         _rstList.push_back(EdgeHV(m_otherN, tmpNode1));
      }
      else{
         tmpNode1.first = m_otherN.first;
         tmpNode1.second = commonN.second;
         _rstList.push_back(EdgeHV(commonN, tmpNode1));
         _rstList.push_back(EdgeHV(m_otherN, tmpNode1));
      }

      commonP.updateLRTB(tmpNode1);
      m_otherP.updateLRTB(tmpNode1);

      _mstList[s].resetDone();

   }
   // case3: same region
   else if(!(csX ^ cmX) && !(csY ^ cmY)){
      // cout << "case3!!" << commonN << " " << m_otherN << " " << s_otherN << endl;

      tmpNode1.first = (ADIFF(commonN.first, m_otherN.first) < ADIFF(commonN.first, s_otherN.first)) ?
                       m_otherN.first : s_otherN.first;
      tmpNode1.second = (ADIFF(commonN.second, m_otherN.second) < ADIFF(commonN.second, s_otherN.second)) ?
                       m_otherN.second : s_otherN.second;

      _rstList.push_back(EdgeHV(s_otherN, tmpNode1));
      _rstList.push_back(EdgeHV(m_otherN, tmpNode1));

      if(ADIFF(tmpNode1.first, commonN.first) > ADIFF(tmpNode1.second, commonN.second)){
         tmpNode2.first = tmpNode1.first;
         tmpNode2.second = commonN.second;
      }
      else{
         tmpNode2.second = tmpNode1.second;
         tmpNode2.first = commonN.first;
      }

      _rstList.push_back(EdgeHV(commonN, tmpNode2));
      _rstList.push_back(EdgeHV(tmpNode1, tmpNode2));
      _mstList[s].setDone();


      Pin& commonP = _pinList[commonV];
      Pin& m_otherP = _pinList[m_otherV];
      Pin& s_otherP = _pinList[s_otherV];

      s_otherP.updateLRTB(tmpNode1);
      m_otherP.updateLRTB(tmpNode1);
      commonP.updateLRTB(tmpNode2);

   }
   else{ // case2: neighbor region
      Pin& commonP = _pinList[commonV];
      Pin& m_otherP = _pinList[m_otherV];
      Pin& s_otherP = _pinList[s_otherV];

      if(csY ^ cmY){ // common X, opposite Y

         if(commonN.first < m_otherN.first){
            if(commonP._R2 || commonP._R7){
               _mstList[m].resetDone();
               _mstList[s].resetDone();
               return;
            }

            if(m_otherN.second > s_otherN.second){
               if(s_otherP._R4 || m_otherP._R5) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R6 = true;
                  s_otherP._R3 = true;
               }
            }
            else{
               if(s_otherP._R5 || m_otherP._R4) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R3 = true;
                  s_otherP._R6 = true;
               }
            }
            commonP._R1 = true;
            commonP._R8 = true;
         }
         else{ 
            if(commonP._R3 || commonP._R6){
               _mstList[m].resetDone();
               _mstList[s].resetDone();
               return;
            }
            
            if(m_otherN.second > s_otherN.second){
               if(s_otherP._R1 || m_otherP._R8) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R7 = true;
                  s_otherP._R2 = true;
               }
            }
            else{
               if(s_otherP._R8 || m_otherP._R1) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R2 = true;
                  s_otherP._R7 = true;
               }
            }
            commonP._R4 = true;
            commonP._R5 = true;
         }


         tmpNode1.first = (ADIFF(commonN.first, m_otherN.first) < ADIFF(commonN.first, s_otherN.first)) ?
                          m_otherN.first : s_otherN.first;
         tmpNode1.second = commonN.second;
         tmpNode2.first = (ADIFF(commonN.first, m_otherN.first) > ADIFF(commonN.first, s_otherN.first)) ?
                          m_otherN.first : s_otherN.first;
         tmpNode2.second = commonN.second;

         _rstList.push_back(EdgeHV(commonN, tmpNode1));

         _rstList.push_back(EdgeHV(tmpNode1, tmpNode2));

         if(m_otherN.first == tmpNode1.first){
            _rstList.push_back(EdgeHV(m_otherN, tmpNode1));
            _rstList.push_back(EdgeHV(s_otherN, tmpNode2));

         }
         else{
            _rstList.push_back(EdgeHV(s_otherN, tmpNode1));
            _rstList.push_back(EdgeHV(m_otherN, tmpNode2));
         }

         // set region line occupied?
         // _pinList[commonV].updateLRTB(tmpNode1);
         // _pinList[commonV].updateLRTB(tmpNode2);
         // _pinList[m_otherV].updateLRTB(tmpNode1);
         // _pinList[m_otherV].updateLRTB(tmpNode2);
         // _pinList[s_otherV].updateLRTB(tmpNode1);
         // _pinList[s_otherV].updateLRTB(tmpNode2);

         if(commonN.first < m_otherN.first){
            _pinList[commonV].setR();
         }
         else{
            _pinList[commonV].setL();
         }

         if(m_otherN.second > s_otherN.second){
            _pinList[m_otherV].setB();
            _pinList[s_otherV].setT();
         }
         else{
            _pinList[m_otherV].setT();
            _pinList[s_otherV].setB();
         }


      }
      else{ // common Y, opposite X

         if(commonN.second < m_otherN.second){
            if(commonP._R1 || commonP._R4){
               _mstList[m].resetDone();
               _mstList[s].resetDone();
               return;
            }

            if(m_otherN.first > s_otherN.first){
               if(s_otherP._R7 || m_otherP._R6) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R5 = true;
                  s_otherP._R8 = true;
               }
            }
            else{
               if(s_otherP._R6 || m_otherP._R7) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R8 = true;
                  s_otherP._R5 = true;
               }
            }
            commonP._R2 = true;
            commonP._R3 = true;
         }
         else{ 
            if(commonP._R5 || commonP._R8){
               _mstList[m].resetDone();
               _mstList[s].resetDone();
               return;
            }

            if(m_otherN.first > s_otherN.first){
               if(s_otherP._R2 || m_otherP._R3) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R4 = true;
                  s_otherP._R1 = true;
               }
            }
            else{
               if(s_otherP._R3 || m_otherP._R2) {
                  _mstList[m].resetDone();
                  _mstList[s].resetDone();
                  return;
               }
               else{
                  m_otherP._R1 = true;
                  s_otherP._R4 = true;
               }
            }
            commonP._R6 = true;
            commonP._R7 = true;
         }

         tmpNode1.second = (ADIFF(commonN.second, m_otherN.second) < ADIFF(commonN.second, s_otherN.second)) ?
                          m_otherN.second : s_otherN.second;
         tmpNode1.first = commonN.first;
         tmpNode2.second = (ADIFF(commonN.second, m_otherN.second) > ADIFF(commonN.second, s_otherN.second)) ?
                          m_otherN.second : s_otherN.second;
         tmpNode2.first = commonN.first;

         _rstList.push_back(EdgeHV(commonN, tmpNode1));

         _rstList.push_back(EdgeHV(tmpNode1, tmpNode2));

         if(m_otherN.second == tmpNode1.second){
            _rstList.push_back(EdgeHV(m_otherN, tmpNode1));
            _rstList.push_back(EdgeHV(s_otherN, tmpNode2));

         }
         else{
            _rstList.push_back(EdgeHV(s_otherN, tmpNode1));
            _rstList.push_back(EdgeHV(m_otherN, tmpNode2));
         }

         // set region line occupied?
         if(commonN.second < m_otherN.second){
            _pinList[commonV].setT();
         }
         else{
            _pinList[commonV].setB();
         }

         if(m_otherN.first > s_otherN.first){
            _pinList[m_otherV].setL();
            _pinList[s_otherV].setR();
         }
         else{
            _pinList[m_otherV].setR();
            _pinList[s_otherV].setL();
         }

      }


      _pinList[commonV].updateLRTB(tmpNode1);
      _pinList[commonV].updateLRTB(tmpNode2);
      _pinList[m_otherV].updateLRTB(tmpNode1);
      _pinList[m_otherV].updateLRTB(tmpNode2);
      _pinList[s_otherV].updateLRTB(tmpNode1);
      _pinList[s_otherV].updateLRTB(tmpNode2);

      _mstList[s].setDone();

   }

}



void SteinerRouter::calRSTCost()
{
   size_t totalLen = 0;
   for(unsigned i = 0; i < _rstList.size(); ++i){
      totalLen += (size_t)_rstList[i].len();
   }
   _rstCost = totalLen;
}


void SteinerRouter::buildHVList()
{
   for(unsigned i = 0; i < _rstList.size(); ++i){

      if(_rstList[i].isHV() == HLINE)
         _hList.push_back(_rstList[i]);

      else if(_rstList[i].isHV() == VLINE)
         _vList.push_back(_rstList[i]);

      else
         assert(_rstList[i].len() == 0);
   }

   ::sort(_hList.begin(), _hList.end(), sortHList);
   ::sort(_vList.begin(), _vList.end(), sortVList);

}

