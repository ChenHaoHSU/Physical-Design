/****************************************************************************
  FileName     [ SteinerRouter.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/

#include <iostream>
#include <ctime>
#include "SteinerRouter.h"

using namespace std;


void SteinerRouter::printPinList() const
{
   for(unsigned i = 0; i < _pinList.size(); ++i){
      cout << "[" << i << "] " 
           << _pinList[i] << " "
           << _pinList[i].name() << endl;
   }
   cout << endl;
}


void SteinerRouter::printEList() const
{
   int idx1 = 0;
   int idx2 = 0;
   for(unsigned i = 0; i < _eList.size(); ++i){
      idx1 = _eList[i].v1();
      idx2 = _eList[i].v2();
      cout << "[" << i << "] " 
           << _pinList[idx1].node() << " "
           << _pinList[idx2].node() << " "
           << _eList[i].len() << endl;
   }
   cout << endl;
}


void SteinerRouter::printMSTList() const
{
   int idx1 = 0;
   int idx2 = 0;
   for(unsigned i = 0; i < _mstList.size(); ++i){
      idx1 = _mstList[i].v1();
      idx2 = _mstList[i].v2();
      cout << "[" << i << "] " 
           << _pinList[idx1].node() << " "
           << _pinList[idx2].node() << " "
           << _mstList[i].len()     << " "
           << _mstList[i].done()    << endl;
   }
   cout << endl;
}


void SteinerRouter::printRSTList() const
{
   for(unsigned i = 0; i < _rstList.size(); ++i){
      cout << "[" << i << "] " 
           << _rstList[i].n1() << " "
           << _rstList[i].n2() << " "
           << _rstList[i].len() << " "
           << _rstList[i].HVstr() << endl;
   }
   cout << endl;
}


void SteinerRouter::printRSMTList() const
{
   for(unsigned i = 0; i < _rsmtList.size(); ++i){
      cout << "[" << i << "] " 
           << _rsmtList[i].n1() << " "
           << _rsmtList[i].n2() << " "
           << _rsmtList[i].len() << " "
           << _rsmtList[i].HVstr() << endl;
   }
   cout << endl;
}

void SteinerRouter::printHList() const
{
  for(unsigned i = 0; i < _hList.size(); ++i){
      cout << "[" << i << "] " 
           << _hList[i].n1() << " "
           << _hList[i].n2() << " "
           << _hList[i].len() << " "
           << _hList[i].HVstr() << endl;
   }
   cout << endl;
}


void SteinerRouter::printVList() const
{
  for(unsigned i = 0; i < _vList.size(); ++i){
      cout << "[" << i << "] " 
           << _vList[i].n1() << " "
           << _vList[i].n2() << " "
           << _vList[i].len() << " "
           << _vList[i].HVstr() << endl;
   }
   cout << endl;
}

void SteinerRouter::printEGainList() const
{
   for(unsigned i = 0; i < _eGainList.size(); ++i){
      EGain eg = _eGainList[i];
      Pin p1 = _pinList[_mstList[eg.e1].v1()];
      Pin p2 = _pinList[_mstList[eg.e1].v2()];
      Pin p3 = _pinList[_mstList[eg.e2].v1()];
      Pin p4 = _pinList[_mstList[eg.e2].v2()];

      cout << "[" << i << "] "
           << "[" << p1.node() << " " << p2.node() << "] "
           << "[" << p3.node() << " " << p4.node() << "] "
           << eg.gain << endl;
   }
   cout << endl;
}