/****************************************************************************
  FileName     [ Net.h ]
  PackageName  [ pa1 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 1 
                 (2-Way F-M Circuit Partitioning) ]
  Date         [ 2017/3/18 created ]
****************************************************************************/

#ifndef NET_H
#define NET_H

#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <ctime>
#include "myHashSet.h"

using namespace std;


#define TIME_LIMIT   3300  // second
#define REPEAT_LIMIT 12
#define HASH_INIT    1000003


class Cell;
class Net;
class BucketList;
class CellV;
class NetManager;
struct CellSort;   // for cell sorting by its _nets.size() 

enum PARTITION { AAA = 0, BBB = 1 };


class Cell
{
   friend class NetManager;
   
public:
   Cell(int n = 0, string s = "DEFAULT"): _cellNo(n), _name(s) { _gain = 0; _front = _back = 0; _locked = false; }
   ~Cell() {}

   // Basic Access Function
   int getNo() const { return _cellNo; }
   int getGain() const { return _gain; }
   string getName() const { return _name; }
   bool getLocked() const { return _locked; }
   bool isNetEmpty() const { return _nets.empty(); }
   bool isValid() const { return (!_nets.empty()); }
   size_t netNum() const { return _nets.size(); }
   Net* getNet(int n) const { return _nets[n]; }
   Cell* getFront() const { return _front; }
   Cell* getBack() const { return _back; }
   BucketList* getPart() const { return _part; }

   // Basic Assign Function
   void pushNet(Net* net) { _nets.push_back(net); }
   void setPart(BucketList* p) { _part = p; }
   void setGain(int g) { _gain = g; }
   void setFront(Cell* f) { _front = f; }
   void setBack(Cell* b) { _back = b; }
   void setName(string s) { _name = s; }

   void increGain() { ++_gain; }
   void decreGain() { --_gain; }

   void lock() { _locked = true; }
   void unlock() { _locked = false; }

   void moveA2B();
   void moveB2A();

   void cellInfo() const;

   friend ostream& operator << (ostream&, const Cell&);

private:
   int          _cellNo;
   int          _gain;
   Cell*        _front;
   Cell*        _back;
   BucketList*  _part;
   vector<Net*> _nets;
   string       _name;
   bool         _locked;

};


class Net
{
   friend class NetManager;

public:
   Net(){ _numA = _numB = 0; }
   ~Net() {}

   // Basic access functions
   unsigned getNumA() const { return _numA; }
   unsigned getNumB() const { return _numB; }
   unsigned getCellNum() const { return _tmpCells.size(); }
   int getTmpCell(const unsigned& n) const { return _tmpCells[n]; }
   string getName() const { return _name; }
   Cell* getCell(unsigned i) const { return (i < _cells.size() ? _cells[i] : 0); }

   // Basic assign functions
   void setName(const string& s) { _name = s; }
   void setNumA(int n) { _numA = n; }
   void setNumB(int n) { _numB = n; }

   void moveA2B() { --_numA; ++_numB; } // from A to B
   void moveB2A() { ++_numA; --_numB; } // from B to A

   void decreA() { --_numA; }
   void decreB() { --_numB; }
   void increA() { ++_numA; }
   void increB() { ++_numB; }

   void pushCell(Cell* cell) { _cells.push_back(cell); }
   void pushTmpCell(int n) { _tmpCells.push_back(n); }

   friend ostream& operator << (ostream&, const Net&); 
   
private:
   string        _name;
   unsigned      _numA;
   unsigned      _numB;
   vector<int>   _tmpCells;
   vector<Cell*> _cells;

};


class BucketList
{
   friend class NetManager;
   
public:
   BucketList() { _maxGain = INT_MIN; }
   ~BucketList() {}

   void insertC(Cell*);
   void deleteC(Cell*, const int);
   void clearList() { _list.clear(); _list.resize(0); }
   void resize(int maxP) { _list.resize(((maxP * 2) + 1), 0); }
   void setMaxPinNum(int m) { _maxPinNum = m; }
   void setMaxGain(int m) { _maxGain = m; }
   void updateMaxGain(Cell*);
   void updateMaxGain(int);

   int getMaxGain() const { return _maxGain; }
   Cell* getMaxGainCell(NetManager*);

   void printList() const;

private:
   int           _maxGain;
   unsigned      _maxPinNum;
   vector<Cell*> _list;

   inline int idx(int) const;

};


class  CellV
{
public:
   CellV(int n = 0, string s = ""): _no(n), _name(s) {}
   ~CellV() {}

   string getName() const { return _name; }
   int getNo () const { return _no; }

   size_t operator () () const
   {
      size_t k = 5000011;
      int n = (_name.length() > 4 ? 4 : _name.length());
      for(int i = 0; i < n; ++i)
         k ^= ((size_t)_name[i] << (i*8));
      // cout << (k % HASH_INIT)<< endl;
      return k;
   }
   bool operator == (const CellV& n) const { return (_name == n.getName()); }

   
private:
   int    _no;
   string _name;
};


class NetManager
{
public:
   NetManager() { 
      _maxPinNum = 0; 
      _Gain = 1; 
      _maxCellIdx = _cellNum = _NumA = _NumB = 0;
      _PartA = _PartB = 0;
      _solSeq.clear();
      _hash.init(HASH_INIT);
   }
   ~NetManager() { clear(); }

   // Parsers
   bool parseAll(const char*);
   bool parseRValue(ifstream&);
   bool parseNet(ifstream&);
   bool parseNX(const string&, Net*&);
   bool parseCX(const string&, Net*&);
   void buildCells();

   // Basic Access Functions
   size_t getCellNum() const { return _cellList.size(); }
   size_t getNetNum() const { return _netList.size(); }
   unsigned getCutSize(bool calculate = true) const;

   // Main function
   bool FM();
   bool writeResult(const char*);

   // FM Functions
   void initPartition();
   void initAll();
   void initGain();
   void buildBucketList();

   Cell* getMaxNBalanceCell();

   void updateBeforeMove(Cell*, BucketList*, BucketList*);
   void moveCell(Cell*, BucketList*, BucketList*);
   void updateAfterMove(Cell*, BucketList*, BucketList*);
   void updateGain();

   void setBounds();
   bool isBalance(Cell* c) const;
         
   void result(ostream&) const;
   void printCellGain() const;
   void printAllCell() const;
   void printAllNet() const;

   void clear();

private:
   vector<Net*>   _netList;
   vector<Cell*>  _cellList;
   vector<int>    _solSeq;
   HashSet<CellV> _hash;

   BucketList*    _PartA;
   BucketList*    _PartB;

   unsigned       _maxPinNum;
   unsigned       _NumA;
   unsigned       _NumB;

   double         _rvalue;
   double         _upperBound;
   double         _lowerBound;

   int            _Gain;
   int            _MaxGain;
   int            _maxCellIdx;
   int            _cellNum;

   // Helper Functions
   inline bool checkTime(const time_t&, time_t&) const;
   inline bool checkRepeat() const;
   inline bool checkSameCutSize() const;
   inline int idx(int) const;
   inline int abs(int a) const { return (a > 0 ? a : (a * (-1.0))); }

};


struct CellSort
{
   bool operator () (Cell* c1, Cell* c2)
   {
      return (c1->netNum() > c2->netNum());
   }
};


#endif  // NET_H