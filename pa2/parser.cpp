/****************************************************************************
  FileName     [ parser.cpp ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <stdlib.h>
#include <sstream>

#include "floorplanner.h"
#include "myHashSet.h"

using namespace std;


bool FloorPlanner::parseAll(char* alpha, char* blockfile, char* netfile)
{
   if(!parseAlpha(alpha)){
      cerr << "Error: Fail to parse alpha." << endl;
      return false;
   }

   if(!parseBlockFile(blockfile)){
      cerr << "Error: Fail to parse \"" << blockfile << "\"." << endl;
      return false;
   }

   if(!parseNetFile(netfile)){
      cerr << "Error: Fail to parse \"" << netfile << "\"." << endl;
      return false;
   }

   cout << "> Complete parsing. " << endl;

   calTTLArea();
   
   /* debuggers */ 
   // printBlockList();
   // printTerminalList();
   // printNetList();
   // cout << "_alpha = " << _alpha << endl;
   // cout << "#Blocks = " << _blockList.size() << endl;
   // cout << "#Terminals = " << _terminalList.size() << endl;
   // cout << "#Net = " << _netList.size() << endl;

   return true;
}


/***********************************************
          Parse ALPHA
***********************************************/
bool FloorPlanner::parseAlpha(char* alpha)
{
   // cout << "> Parsing alpha ..." << endl;

   string tmpstr = alpha;
   if(!isFloat(tmpstr)){
      cerr << "Error: Alpha(\"" << alpha << "\") is illegal." << endl;
      return false;
   }

   _alpha = atof(alpha);
   if(_alpha < 0 || _alpha >= 1){
      cerr << "Error: Alpha(\"" << _alpha << "\") is out of range(0 <= alpha < 1)." << endl;
      return false; 
   }

   return true;
}


/***********************************************
          Parse BLOCK FILE
***********************************************/
bool FloorPlanner::parseBlockFile(char* blockfile)
{
   ifstream inf(blockfile, ios::in);

   if(!inf){
      cerr << "Error: Cannot open block file \"" << blockfile << "\"." << endl;
      return false; 
   }

   if(!parseOutline(inf)){
      cerr << "Error: Fail to parse Outline." << endl;
      return false;
   }

   if(!parseNumBlocks(inf)){
      cerr << "Error: Fail to parse NumBlocks." << endl;
      return false;
   }

   if(!parseNumTerminals(inf)){
      cerr << "Error: Fail to parse NumBlocks." << endl;
      return false;
   }

   if(!parseBlocks(inf)){
      cerr << "Error: Fail to parse Blocks." << endl;
      return false;
   }

   if(!parseTerminals(inf)){
      cerr << "Error: Fail to parse Terminals." << endl;
      return false;
   }

   inf.close();

   return true;
}


bool FloorPlanner::parseOutline(ifstream& inf) 
{
   string tmpstr = "";

   inf >> tmpstr;
   if(tmpstr != "Outline:"){
      cerr << "Error: Wrong format. (Should be \"Outline:\")." << endl;
      return false;
   }

   tmpstr = "";
   inf >> tmpstr;
   if(!myStr2Int(tmpstr, _outlineWidth)){
      cerr << "Error: Outline Width(\"" << tmpstr << "\") is illegal." << endl;
      return false; 
   }

   tmpstr = "";
   inf >> tmpstr;
   if(!myStr2Int(tmpstr, _outlineHeight)){
      cerr << "Error: Outline Height(\"" << tmpstr << "\") is illegal." << endl;
      return false; 
   }

   return true;
}


bool FloorPlanner::parseNumBlocks(ifstream& inf) 
{
   string tmpstr1 = "";
   string tmpstr2 = "";

   inf >> tmpstr1;
   if(tmpstr1 != "NumBlocks:"){
      cerr << "Error: Wrong format. (Should be \"NumBlocks:\")" << endl;
      return false;
   }

   inf >> tmpstr2;
   int numBlocks = 0;
   if(!myStr2Int(tmpstr2, numBlocks)){
      cerr << "Error: NumBlocks(\"" << tmpstr2 << "\") is illegal." << endl;
      return false; 
   }

   _blockList.resize(numBlocks);
   for(int i = 0; i < numBlocks; ++i)
      _blockList[i] = new Block;

   _blockHashSet.init(getHashSize(numBlocks)); 

   return true;
}


bool FloorPlanner::parseNumTerminals(ifstream& inf) 
{
   string tmpstr1 = "";
   string tmpstr2 = "";

   inf >> tmpstr1;
   if(tmpstr1 != "NumTerminals:"){
      cerr << "Error: Wrong format. (Should be \"NumTerminals:\")" << endl;
      return false;
   }

   inf >> tmpstr2;
   int numTerminals = 0;
   if(!myStr2Int(tmpstr2, numTerminals)){
      cerr << "Error: NumTerminals(\"" << tmpstr2 << "\") is illegal." << endl;
      return false; 
   }

   _terminalList.resize(numTerminals);
   for(int i = 0; i < numTerminals; ++i)
      _terminalList[i] = new Terminal;

   _terminalHashSet.init(getHashSize(numTerminals));

   return true;
}


bool FloorPlanner::parseBlocks(ifstream& inf) 
{
   string tmpname, tmpwidth, tmpheight;
   int tmpint;

   for(unsigned i = 0, n = _blockList.size(); i < n; ++i){
      
      tmpname = tmpwidth = tmpheight = "";
      tmpint = -1;

      // Parse Block Name
      inf >> tmpname;
      if(tmpname == ""){
         cerr << "Error: Lack of blocks.(" << ((int)i - (int)n) << ")" << endl;
         return false; 
      }
      _blockList[i]->setName(tmpname);

      // Parse Block Width
      inf >> tmpwidth;
      if(!myStr2Int(tmpwidth, tmpint)){
         cerr << "Error: Block " << tmpname << " has illegal width (\"" << tmpwidth << "\")." << endl;
         return false; 
      }
      _blockList[i]->setWidth(tmpint);

      // Parse Block Height
      inf >> tmpheight;
      if(!myStr2Int(tmpheight, tmpint)){
         cerr << "Error: Block " << tmpname << " has illegal height(\"" << tmpheight << "\")." << endl;
         return false; 
      }
      _blockList[i]->setHeight(tmpint);

      // Insert to Hashset
      if(!_blockHashSet.insert(BlockV(i, tmpname))){
         cerr << "Error: Two blocks have the same name \"" << tmpname << "\"." << endl;
         return false;
      }

   }

   return true;
}


bool FloorPlanner::parseTerminals(ifstream& inf) 
{
   string tmpname, tmpstr, tmpx, tmpy;
   int tmpint = 0;

   for(unsigned i =0, n = _terminalList.size(); i < n; ++i){
      tmpname = tmpstr = tmpx = tmpy = "";
      tmpint = -1;

      // Parse Terminal Name
      inf >> tmpname;
      if(tmpname == ""){
         cerr << "Error: Lack of terminals.(" << ((int)i - (int)n) << ")" << endl;
         return false; 
      }
      _terminalList[i]->setName(tmpname);

      // Parse "terminal"
      inf >> tmpstr;
      if(tmpstr != "terminal"){
         cerr << "Error: Wrong format. (Should be \"terminal\" after " << tmpname << ")" << endl;
         return false;
      }

      // Parse x coordinate
      inf >> tmpx;
      if(!myStr2Int(tmpx, tmpint)){
         cerr << "Error: Terminal " << tmpname << " has illegal x coordinate (\"" << tmpx << "\")" << endl;
         return false;
      }
      _terminalList[i]->setX(tmpint);

      // Parse y coordinate
      inf >> tmpy;
      if(!myStr2Int(tmpy, tmpint)){
         cerr << "Error: Terminal " << tmpname << " has illegal y coordinate (\"" << tmpy << "\")" << endl;
         return false;
      }
      _terminalList[i]->setY(tmpint);

      // Insert to Hashset
      if(!_terminalHashSet.insert(TerminalV(i, tmpname))){
         cerr << "Error: Two terminals have the same name \"" << tmpname << "\"." << endl;
         return false;
      }
   }

   string extrastr = "";
   if(inf >> extrastr){
      cerr << "Error: Extra input in block file. (\"" << extrastr << "\")." << endl;
      return false;
   }

   return true;
}


/***********************************************
          Parse NET FILE
***********************************************/
bool FloorPlanner::parseNetFile(char* netfile)
{
   ifstream inf(netfile, ios::in);

   if(!inf){
      cerr << "Error: Cannot open net file \"" << netfile << "\"." << endl;
      return false; 
   }

   if(!parseNumNets(inf)){
      cerr << "Error: Fail to parse Nets." << endl;
      return false;
   }

   if(!parseNets(inf)){
      cerr << "Error: Fail to parse Nets." << endl;
      return false;
   }

   inf.close();

   return true;
}


bool FloorPlanner::parseNumNets(ifstream& inf) 
{
   string tmpstr = "";

   inf >> tmpstr;
   if(tmpstr != "NumNets:"){
      cerr << "Error: Wrong format. (Should be \"NumNets:\")" << endl;
      return false;
   }

   tmpstr = "";
   inf >> tmpstr;
   int numNets = 0;
   if(!myStr2Int(tmpstr, numNets)){
      cerr << "Error: NumNets(\"" << tmpstr << "\") is illegal." << endl;
      return false; 
   }

   _netList.resize(numNets);
   for(int i = 0; i < numNets; ++i)
      _netList[i] = new Net;

   return true;
}


bool FloorPlanner::parseNets(ifstream& inf)
{
   string tmpstr;
   int tmpint;

   for(unsigned i = 0, n = _netList.size(); i < n; ++i){
      // Pasre "NetDegree:"
      inf >> tmpstr;
      if(tmpstr != "NetDegree:"){
         cerr << "Error: Wrong format. (Should be \"NetDegree:\")" << endl;
         return false;
      }

      // Parse Net Degree
      tmpstr = "";
      inf >> tmpstr;
      if(!myStr2Int(tmpstr, tmpint)){
         cerr << "Error: NetDegree(\"" << tmpstr << "\") is illegal.(net " << (i + 1) << ")"<< endl;
         return false;
      }

      // Parse One Net
      if(!parseOneNet(inf, _netList[i], tmpint)){
         cerr << "Error: Fail to parse a net with net degree " << tmpint << "." << endl;
         return false;
      }
   }

   string extrastr = "";
   if(inf >> extrastr){
      cerr << "Error: Extra input in net file. (\"" << extrastr << "\")." << endl;
      return false;
   }

   return true;
}


bool FloorPlanner::parseOneNet(ifstream& inf, Net* net, const int& n)
{
   string tmpname;

   for(int i = 0; i < n; ++i){

      tmpname = "";
      inf >> tmpname;
      BlockV tmpblockV(-1, tmpname);
      TerminalV tmpterminalV(-1, tmpname);

      // Check if lack of blocks or terminals
      if(tmpname == ""){
         cerr << "Error: Lack of blocks or terminals.(" << (i - n) << ")" << endl;
         return false; 
      }

      // Check if in _blockList
      if(_blockHashSet.query(tmpblockV)){
         assert(tmpblockV.getNo() >= 0);
         net->pushBlock(_blockList[tmpblockV.getNo()]);
      }
      // Check if in _terminalList
      else if(_terminalHashSet.query(tmpterminalV)){
         assert(tmpterminalV.getNo() >= 0);
         net->pushTerminal(_terminalList[tmpterminalV.getNo()]);
      }

      else{
         cerr << "Error: Neither a block nor a terminal is named \"" << tmpname << "\"." << endl;
         return false;
      }
   }

   return true;
}


bool FloorPlanner::myStr2Int(const string& str, int& num)
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


bool FloorPlanner::isFloat(string s)
{
    istringstream iss(s);
    float dummy;
    iss >> noskipws >> dummy;
    return iss && iss.eof(); // Result converted to bool
}