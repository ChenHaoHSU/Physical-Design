/****************************************************************************
  FileName     [ testGen.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) Benchmark Generator ]
  Date         [ 2017/5/29 created ]
****************************************************************************/

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <cassert>

#include "myHashSet.h"

using namespace std;

/* User-defined parameter */
///////////////////////////////
#define BOUNDARY_LX 0
#define BOUNDARY_LY 0
#define BOUNDARY_RX 1000000
#define BOUNDARY_UY 1000000

#define NAME_LEN    2

#define ASCII_LOWER 33
#define ASCII_UPPER 126
#define ASCII_A     65
#define ASCII_Z     90
#define ASCII_a     97
#define ASCII_z     122
///////////////////////////////


/* type define */
typedef pair<int, int> Node;


/* function declaration */
bool myStr2PosInt(const string&, int&);
ostream& operator << (ostream&, const Node&);
size_t getHashSize(size_t);
float getTime(const time_t&);
void initCharList();


/* global variable */
vector<char> charList;


class NodeH
{
public:
   NodeH(int x = 0, int y = 0):_x(x), _y(y) {}
   ~NodeH() {}
   
   int x() const { return _x; }
   int y() const { return _y; }

   Node node() const { return Node(_x, _y); }

   size_t operator () () const
   {
      size_t k = 0;
      k ^= (_x << 16);
      k ^= (_y << 16);
      k ^= (_x);
      k ^= (_y);
      return k;
   }

   bool operator == (const NodeH& p) const 
   { 
      return (_x == p.x() && _y == p.y());
   } 

   int _x;
   int _y;

};


int main(int argc, char** argv)
{
   time_t start = clock(); 

   if(argc != 3){
      cerr << "Usage: ./testGen <#Pins> <fileName>" << endl;
      cerr << "e.g. ./testGen 50 case1.in" << endl;
      return -1;
   }

   string numstr = argv[1];
   
   size_t numPin;
   int tmpint;
   size_t brx = BOUNDARY_RX;
   size_t blx = BOUNDARY_LX;
   size_t buy = BOUNDARY_UY;
   size_t bly = BOUNDARY_LY;

   if(!myStr2PosInt(numstr, tmpint)){
      cerr << "Error: " << numstr << " is illigal." << endl;
      return false; 
   }
   numPin = tmpint;

   if(numPin > (brx - blx) * (buy - bly)){
      cerr << "Error: Impossible to generate " << numPin << " in [("
           << BOUNDARY_LX << "," << BOUNDARY_LY << "), ("
           << BOUNDARY_RX << "," << BOUNDARY_UY << ")]." << endl;
      return false; 
   }

   ofstream outf(argv[2], ios::out);
   if(!outf){
      cerr << "Error: Cannot open output file \"" << argv[2] << "\"." << endl;
      return -1;
   }

   srand((unsigned)time(NULL));

   // Boundary
   outf << "Boundary = (" << BOUNDARY_LX << "," << BOUNDARY_LY << "), ("
        << BOUNDARY_RX << "," << BOUNDARY_UY << ")" << endl;

   // NumPins
   outf << "NumPins = " << numPin << endl;

   // Pins
   initCharList();
   unsigned charListSize = charList.size();

   HashSet<NodeH> hashN;
   hashN.init(getHashSize(numPin));
   NodeH n;
   int xrange = BOUNDARY_RX - BOUNDARY_LX;
   int yrange = BOUNDARY_UY - BOUNDARY_LY;

   for(size_t i = 0; i < numPin; ++i){
      // PIN
      outf << "PIN" << " ";

      // pin name
      // outf << "p" << i << " ";
      outf << i << "_";
      for(int j = 0; j < NAME_LEN; ++j)
        outf << charList[(rand() % charListSize)];
      outf << " ";

      // pin node
      while(true){
        n._x = (rand() % xrange) + BOUNDARY_LX;
        n._y = (rand() % yrange) + BOUNDARY_LY;

        if(hashN.insert(n)) 
           break;
      }
      outf << n.node() << endl;
   }
   
   outf.close();

   cout << "> Successfully generate!" << endl
        << "> Running Time: " << getTime(start) << " seconds." << endl
        << "////////////////////"   << endl
        << "New Benchmark: " << argv[2] << endl;

   return 0;
}


bool myStr2PosInt(const string& str, int& num)
{
   if(str.length() == 0){
      cerr << "Error: \"" << str << "\" is NOT a non-negative integer." << endl;
      return false;
   }

   for(unsigned i = 0; i < str.length(); i++){
      if(!isdigit(str[i])){
         cerr << "Error: \"" << str << "\" is NOT a non-negative integer." << endl;
         return false;
      }
   }

   num = atoi(str.c_str());
   assert(num >= 0);
   
   return true;
}


ostream& operator << (ostream& os, const Node& n)
{
   os << "(" << n.first << "," << n.second << ")";
   return os;
}


size_t getHashSize(size_t s) 
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


float getTime(const time_t& start)
{
   time_t end = clock();
   return (float)(end - start) / CLOCKS_PER_SEC;
}


void initCharList()
{ 
   /* ascii 33 ~ 126 */
   // for(int i = ASCII_LOWER; i <= ASCII_UPPER; ++i){
   //    charList.push_back(char(i));
   // }

   /* [A~Z a~z] */
   for(int i = ASCII_A; i <= ASCII_Z; ++i){
      charList.push_back(char(i));
   }
   for(int i = ASCII_a; i <= ASCII_z; ++i){
      charList.push_back(char(i));
   }

   /* debugging */
   // for(unsigned i = 0; i < charList.size(); ++i){
   //    cout << charList[i] << endl;
   // }

}
