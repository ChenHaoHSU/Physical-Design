/****************************************************************************
  FileName     [ Util.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/

#include "Util.h"

using namespace std;

float getTime(const time_t& start)
{
   time_t end = clock();
   return (float)(end - start) / CLOCKS_PER_SEC;
}


bool myStr2Int(const string& str, int& num)
{
   num = 0;
   size_t i = 0;
   int sign = 1;

   if (str[0] == '-') { sign = -1; i = 1; }
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += int(str[i] - '0');
      }
      else{
         cerr << "Error: " << str << " is NOT an integer." << endl;
         return false;
      }
   }
   num *= sign;
   
   return true;
}


bool myStr2Node(const string& str, Node& n)
{
   // Correct input should be (xxx, yyy) 
   // xxx, yyy are integers (positive and negative are acceptable)

   if(str.length() == 0){
      cerr << "Error: \"" << str << "\" is not valid node." << endl; 
      return false;   
   }

   // Find the positions of '(' ',' ')' 
   size_t pos1, pos2, pos3;
   pos1 = str.find_first_of('(', 0);
   pos2 = str.find_first_of(',', 0);
   pos3 = str.find_first_of(')', 0);
   
   // Check if '(' ',' ')' exist
   if(pos1 == string::npos || pos2 == string::npos || pos3 == string::npos){
      cerr << "Error: \"" << str << "\" is not valid node." << endl; 
      return false;
   }

   // Check if pos are correct
   if(!(pos1 < pos2 && pos2 < pos3) || pos1 != 0 || pos3 != str.length() - 1){
      cerr << "Error: \"" << str << "\" is not valid node." << endl; 
      return false;
   }

   // Cut the integer parts from str
   string tmpstr1, tmpstr2; // first , second
   tmpstr1 = str.substr(pos1 + 1, pos2 - pos1 - 1);
   tmpstr2 = str.substr(pos2 + 1, pos3 - pos2 - 1);

   // Transfer the cut strings to int 
   int tmpint1, tmpint2; // first , second
   if(!myStr2Int(tmpstr1, tmpint1)){
      cerr << "Error: \"" << str << "\" is not valid node." << endl; 
      return false;
   }
   if(!myStr2Int(tmpstr2, tmpint2)){
      cerr << "Error: \"" << str << "\" is not valid node." << endl; 
      return false;
   }

   // Assign values to Node n
   n.first = tmpint1;
   n.second = tmpint2;

   return true;
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


void delay(int d)
{
   for(int i = 0; i < 1 * (1 << 20); ++i)
      for(int i = 0; i < 1 * (1 << 10); ++i){}
}