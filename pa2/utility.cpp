/****************************************************************************
  FileName     [ utility.cpp ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#include "floorplanner.h"

using namespace std;

double FloorPlanner::getRnd() const
{
   // only return 0.0000... ~ 0.9999...
   return ((double)(rand() % RANDOM_RANGE) / RANDOM_RANGE);
}


void FloorPlanner::calTTLArea()
{
   _totalArea = 0;
   for(unsigned i = 0; i < _blockList.size(); ++i)
      _totalArea += _blockList[i]->getArea();
}

void FloorPlanner::printBlockList() const
{
   cout << "BlockList.size = " << _blockList.size() << endl;
   for(unsigned i = 0; i < _blockList.size(); ++i)
      cout << *_blockList[i] << endl;
}


void FloorPlanner::printTerminalList() const
{
   cout << "TerminalList.size = " << _terminalList.size() << endl;
   for(unsigned i = 0; i < _terminalList.size(); ++i)
      cout << *_terminalList[i] << endl;
}


void FloorPlanner::printNetList() const
{
   cout << "NetList.size = " << _netList.size() << endl;
   for(unsigned i = 0; i < _netList.size(); ++i){
      cout << "[" << i << "]" << endl;
      cout << *_netList[i] << endl;
   }
}


void FloorPlanner::printRnd() const
{
   int a = 0, b = 0, c = 0, d = 0;
   for(unsigned i = 0; i < 10000000; ++i){ 
      double k = getRnd();
      // cout << k << endl;
      if(k < 0.25) a++;
      else if (k < 0.5) b++;
      else if (k < 0.75) c++;
      else d++;
   }

   cout << a << " " << b << " " << c << " " << d << endl;
}


float FloorPlanner::getTime(const time_t& start) const
{
   time_t end = clock();
   float t_used = (float)(end - start) / CLOCKS_PER_SEC;
   return t_used;
}


bool FloorPlanner::isTimeout(const time_t& start) const
{
   time_t end = clock();
   float t_used = (float)(end - start) / CLOCKS_PER_SEC;
   if(t_used > TIME_LIMIT){
      cerr << "> Warning: Break by timeout (" << TIME_LIMIT << " seconds)." << endl;
      return true;
   }
   else
      return false;
}


void FloorPlanner::clear()
{
   for(unsigned i = 0, n = _blockList.size(); i < n; ++i)
      delete _blockList[i];

   for(unsigned i = 0, n = _terminalList.size(); i < n; ++i)
      delete _terminalList[i];

   for(unsigned i = 0, n = _netList.size(); i < n; ++i)
      delete _netList[i];

   if(_btree) delete _btree;
   if(_recoverMsg) delete _recoverMsg;
}


size_t FloorPlanner::getHashSize(size_t s) const
{
   if (s < 8) return 7;
   if (s < 16) return 13;
   if (s < 32) return 31;
   if (s < 64) return 61;
   if (s < 128) return 127;
   if (s < 512) return 509;
   if (s < 2048) return 1499;
   if (s < 8192) return 4999;
   if (s < 32768) return 13999;
   if (s < 131072) return 59999;
   if (s < 524288) return 100019;
   if (s < 2097152) return 300007;
   if (s < 8388608) return 900001;
   if (s < 33554432) return 1000003;
   if (s < 134217728) return 3000017;
   if (s < 536870912) return 5000011;
   return 7000003;
}