/****************************************************************************
  FileName     [ floorplanner.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#ifndef FLOORPLANNER_H
#define FLOORPLANNER_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <climits>
#include <cassert>
#include <cmath>
#include <climits>
#include <sstream>

#include "mydefine.h"
#include "block.h"
#include "net.h"
#include "terminal.h"
#include "btree.h"
#include "recovermsg.h"
#include "myHashSet.h"

using namespace std;

class FloorPlanner
{
public:
   FloorPlanner() {
      _T = T_INIT;
      _P = P_INIT;
      _Cost = 0.0;
      _Anorm = _Wnorm = 1.0;
      _delta = 0;
      _Width = _Height = INT_MAX;
      _WireLen = 2;
      _btree = 0;
      _recoverMsg = new RecoverMsg;
      srand((unsigned)time(NULL));
      _minCost = INT_MAX;
   }
   ~FloorPlanner() { clear(); }

   // Parsers
   bool parseAll(char*, char*, char*);
   bool parseAlpha(char*);
   bool parseBlockFile(char*);
   bool parseNetFile(char*);
   // Parse block file
   bool parseOutline(ifstream&);
   bool parseNumBlocks(ifstream&);
   bool parseNumTerminals(ifstream&);
   bool parseBlocks(ifstream&);
   bool parseTerminals(ifstream&);
   // Parse net file
   bool parseNumNets(ifstream&);
   bool parseNets(ifstream&);
   bool parseOneNet(ifstream&, Net*, const int&);

   // Floorplan
   bool floorplan();
   void initPerturb();
   bool isTimeout(const time_t&) const;
   void initSol();
   void recover();
   void genNeighbor();
   void updateCost(); // update _Anorm, _Wnorm, and _Cost
   bool shinbwei() const; 
   void rotateAll();
   void record();
   void writeRecord();
   OptType rndSelectType() const;
   double T(int);

   // Perturbation Modes: Generate Neighbor
   void genLeftRotate();
   void genRightRotate();
   void genDeleteInsert();
   void genSwapTwoBlocks();
   void genRotateBlock();

   // Result
   bool writeResult(char*);
   void result(ostream&);
   void resultGUI(ostream&);
   void resultGnuplot();
   double getCost() const;
   double getFinalCost() const;
   double getArea() const { return (double)(_Width * _Height); }
   double getOutlineArea() const { return (_outlineWidth * _outlineHeight); }
   int getTTLWireLen() const;
   int getWidth() const { return _Width; }
   int getHeight() const { return _Height; }

   // Debugging msg
   void printBlockList() const;
   void printTerminalList() const;
   void printNetList() const;
   void printRnd() const;

   void clear();

private:
   double             _alpha;
   int                _outlineWidth;
   int                _outlineHeight;

   vector<Net*>       _netList;
   vector<Block*>     _blockList;
   vector<Terminal*>  _terminalList;

   HashSet<BlockV>    _blockHashSet;
   HashSet<TerminalV> _terminalHashSet;

   Btree*             _btree;
   RecoverMsg*        _recoverMsg;

   double             _Cost;
   double             _Anorm;
   double             _Wnorm;
   double             _delta;
   double             _P;
   double             _C;
   double             _T;
   double             _T1;

   int                _k;
   int                _L; 
   int                _N;
   int                _I; // #iter upper bound
   int                _Width;
   int                _Height;
   int                _WireLen;

   double             _w;
   double             _x;
   double             _y;
   double             _z;

   int                _uphill_avg_cnt;
   double             _uphill_sum;
   int                _delta_cost_cnt;
   double             _delta_sum;

   double             _minCost;
   double             _totalArea;
   vector<BlockR>     _optSol;

   // helper functions 
   bool myStr2Int(const string& str, int& num);
   bool isFloat(string s);
   float getTime(const time_t&) const;
   double getRnd() const;
   size_t getHashSize(size_t) const;
   bool isFitin() const { return (_Width <= _outlineWidth && _Height <= _outlineHeight); }
   bool isInvFit() const;
   void calTTLArea();

};


#endif //FLOORPLANNER_H