/****************************************************************************
  FileName     [ Util.h ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cassert>

using namespace std;

typedef pair<int, int> Node;

#define max(x,y)   ((x)>(y)?(x):(y))
#define min(x,y)   ((x)<(y)?(x):(y))
#define abs(x)     ((x)<0?(-x):(x))
#define ADIFF(x,y) ((x)>(y)?(x-y):(y-x))  // Absolute difference

float getTime(const time_t&);
bool myStr2Int(const string&, int&);
bool myStr2Node(const string&, Node&);
bool myStr2PosInt(const string&, int&);
ostream& operator << (ostream& os, const Node& n);
size_t getHashSize(size_t s);
void delay(int d = 1);

enum Region
{
   R1 = 1,
   R2 = 2,
   R3 = 3,
   R4 = 4,
   R5 = 5,
   R6 = 6,
   R7 = 7,
   R8 = 8,
   // dummy end
   R_TOT = 9
};

enum LINE
{
   HLINE = 0,
   VLINE = 1,
   // dummy end
   TOTLINE = 2
};

#endif // UTIL_H