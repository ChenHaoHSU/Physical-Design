/****************************************************************************
  FileName     [ parser.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/

#include "SteinerRouter.h"

using namespace std;

extern time_t start;

bool SteinerRouter::parseAll(char* infileName)
{
   cout << "> Parsing \"" << infileName << "\" ..." << endl;

   ifstream inf(infileName, ios::in);
   if(!inf){
      cerr << "Error: Cannot open input file \"" << infileName << "\"." << endl;
      return false;
   }

   if(!parseBoundary(inf)){
      cerr << "Error: Fail to parse Boundary." << endl;
      return false;
   }

   if(!parseNumPins(inf)){
      cerr << "Error: Fail to parse NumPins." << endl;
      return false;
   }

   if(!parseAllPins(inf)){
      cerr << "Error: Fail to parse all pins." << endl;
      return false;
   }

   inf.close();

   _benchmark = infileName;

   cout << "  - Boundary = " << _boundaryLL << " " << _boundaryUR << endl;
   cout << "  - NumPins  = " << _numP << endl;
   cout << "  - Time     = " << getTime(start) << " seconds. " << endl << endl;

   // printPinList();

   // resultGNU(true);
   // resultGNU();
   
   return true;
}


bool SteinerRouter::parseBoundary(ifstream& inf)
{
   string tmpstr = "";
   string tmpFull = "";
   string tmpstr1 = "";
   string tmpstr2 = "";
   Node tmpnode;

   inf >> tmpstr;
   if(tmpstr != "Boundary"){
      cerr << "Error: No \"Boundary\"." << endl;
      return false;
   }

   tmpstr = "";
   inf >> tmpstr;
   if(tmpstr != "="){
      cerr << "Error: No \"=\" after Boundary." << endl;
      return false;
   }

   // (LL_x, LL_y)
   inf >> tmpstr1;
   size_t s = tmpstr1.length();
   if(tmpstr1[s - 1] != ','){
      cerr << "Error: Lack of \',\' after (LL_x, LL_y)." << endl;
      return false;
   }
   tmpstr1.resize(s - 1); // remove the tail ','

   if(!myStr2Node(tmpstr1, tmpnode)){
      cerr << "Error: " << tmpstr1 << " is illigal.(LL_x, LL_y)" << endl;
      return false; 
   }

   _boundaryLL = tmpnode; 

   // (UR_x, UR_y)
   inf >> tmpstr2;
   if(!myStr2Node(tmpstr2, tmpnode)){
      cerr << "Error: " << tmpstr2 << " is illigal.(UR_x, UR_y)" << endl;
      return false; 
   }

   _boundaryUR = tmpnode;

   if(_boundaryLL.first >= _boundaryUR.first || _boundaryLL.second >= _boundaryUR.second){
      cerr << "Error: Boundary [" << _boundaryLL << " " << _boundaryUR << "] is illigal." << endl;
      return false; 
   }

   return true;
}


bool SteinerRouter::parseNumPins(ifstream& inf)
{
   string tmpstr = "";
   int tmpint = 0;

   inf >> tmpstr;
   if(tmpstr != "NumPins"){
      cerr << "Error: No \"NumPins\"." << endl;
      return false;
   }

   tmpstr = "";
   inf >> tmpstr;
   if(tmpstr != "="){
      cerr << "Error: No \"=\" after NumPins." << endl;
      return false;
   }

   tmpstr = "";
   inf >> tmpstr;
   if(!myStr2PosInt(tmpstr, tmpint)){
      cerr << "Error: " << tmpstr << " is illigal." << endl;
      return false; 
   }

   _numP = tmpint;

   return true;
}


bool SteinerRouter::parseAllPins(ifstream& inf)
{
   string tmpstr = "";
   string name = "";
   Node tmpnode;

   HashSet<PinH> hashP;  // check duplicate names
   HashSet<NodeH> hashN; // check duplicate nodes
   hashP.init(getHashSize(_numP));
   hashN.init(getHashSize(_numP));

   for(int i = 0; i < _numP; ++i){

      tmpstr = "";
      inf >> tmpstr;
      if(tmpstr != "PIN"){
         cerr << "Error: No \"PIN\". (The " << (i + 1) << "-th pin)" << endl;
         return false;
      }

      tmpstr = "";
      inf >> tmpstr;
      name = tmpstr;
      if(!hashP.insert(PinH(tmpstr))){
         cerr << "Error: There are two pins have identical name \"" << tmpstr << "\"." << endl;
         return false;
      }

      tmpstr = "";
      inf >> tmpstr;
      if(!myStr2Node(tmpstr, tmpnode)){
         cerr << "Error: " << tmpstr << " is illigal. (The " << (i + 1) << "-th pin)" << endl;
         return false; 
      }

      if(!isInBoundary(tmpnode)){
         cerr << "Error: " << tmpnode << " is out of the boundary [" 
              << _boundaryLL << " " << _boundaryUR 
              << "]. (The " << (i + 1) << "-th pin)" << endl;
         return false; 
      }

      if(!hashN.insert(NodeH(tmpnode.first, tmpnode.second))){
         cerr << "Error: There are two pins have identical node \"" << tmpstr << "\"." << endl;
         return false;
      }

      _pinList.push_back(Pin(tmpnode.first, tmpnode.second, name));

   }

   tmpstr = "";
   inf >> tmpstr;
   if(tmpstr != ""){
      cerr << "Error: Extra input \"" << tmpstr << "\"." << endl;
      return false; 
   }

   return true;

}


inline bool SteinerRouter::isInBoundary(const Node& node)
{
   return (_boundaryLL.first <= node.first && _boundaryLL.second <= node.second) &&
          (_boundaryUR.first >= node.first && _boundaryUR.second >= node.second);
}

