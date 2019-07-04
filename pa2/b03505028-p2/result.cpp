/****************************************************************************
  FileName     [ result.cpp ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#include "floorplanner.h"

// GUI
#include "gnuplot_i.h"

using namespace std;

/* extern Global variable */
extern time_t start;

double FloorPlanner::getCost() const
{
   static double a = 0;
   static double l = 0;

   a = (double)getArea() / _Anorm;
   l = (_WireLen / _Wnorm);

   // double d = (getArea() - _totalArea) / getArea();

   double R = _Height > _Width ? 
               ((double)_Width / (double)_Height) : 
               ((double)_Height / (double)_Width);
   double Rs = _outlineHeight > _outlineWidth ? 
               ((double)_outlineWidth / (double)_outlineHeight) : 
               ((double)_outlineHeight / (double)_outlineWidth);
   double r = R - Rs;


   // cout << getArea() << " " << _Anorm << endl;
   // cout << ">> " << 0.1 * r * r << " " << (0.1 * a) << " " << (1* d) << endl;

   assert(a >= 0);

   return (_w * r * r  + (_x * a) + _z * l);
}


double FloorPlanner::getFinalCost() const
{
   double a = _Width * _Height;
   double l = getTTLWireLen();
   double r = (_alpha * a) + ((1 - _alpha) * l);
   // cout << ">> " << a << " " << l << " " << r << endl;
   return r;
}


int FloorPlanner::getTTLWireLen() const
{
   int sum = 0;
   for(unsigned i = 0, n = _netList.size(); i < n; ++i)
      sum += _netList[i]->getHPWL();
   return sum;
}


void FloorPlanner::writeRecord()
{
   assert(!_optSol.empty());
   assert(_optSol.size() == _blockList.size());
   _Width = 0;
   _Height = 0;

   for(unsigned i = 0; i < _optSol.size(); ++i){
      _blockList[i]->_x = _optSol[i]._x;
      _blockList[i]->_y = _optSol[i]._y;
      _blockList[i]->_width = _optSol[i]._width;
      _blockList[i]->_height = _optSol[i]._height;

      if(_blockList[i]->_x + _blockList[i]->_width > _Width)
         _Width = _blockList[i]->_x + _blockList[i]->_width;
      if(_blockList[i]->_y + _blockList[i]->_height > _Height)
         _Height = _blockList[i]->_y + _blockList[i]->_height;
   }

   assert(_Width <= _outlineWidth && _Height <= _outlineHeight);
}


bool FloorPlanner::writeResult(char* outfileName)
{ 
   ofstream outf(outfileName, ios::out);

   if(!outf){
      cerr << "Error: Cannot open net file \"" << outfileName << "\"." << endl;
      return false; 
   }

   result(outf);

   /* print the result on the screen */
   // result(cout);
   // resultGUI(cout);

   #ifdef CAD_CONTEST_GUI
      ofstream cad_outf("cad.out", ios::out);
      resultGUI(cad_outf);
      cout << "> The result has been written to \"cad.out\"." << endl;
      cad_outf.close();
   #endif 

   cout << "> The result has been written to \"" << outfileName << "\"." << endl;
   outf.close();

   return true; 
}


void FloorPlanner::result(ostream& os)
{
   // writeRecord();
   os << getFinalCost() << endl;
   os << getTTLWireLen() << endl;
   os << getArea() << endl;
   os << getWidth() << " " << getHeight() << endl;
   os << getTime(start) << endl;
   for(unsigned i = 0; i < _blockList.size(); ++i){
      os << _blockList[i]->getName() << " "
         << _blockList[i]->getX() << " "
         << _blockList[i]->getY() << " "
         << (_blockList[i]->getX() + _blockList[i]->getWidth()) << " "
         << (_blockList[i]->getY() + _blockList[i]->getHeight()) << endl;
   }
}


void FloorPlanner::resultGUI(ostream& os)
{
   /* Use 2017 CAD contest Problem B evaluator - "eval" to see the block placement */

   #define SCALE 1
   static int scale = 1;

   if(!_optSol.empty())
      writeRecord();

   int ww = (int)(_outlineWidth * 1.3);
   int hh = (int)(_outlineHeight * 1.3);

   os << "ViaCost = 20" << endl;
   os << "Spacing = 5" << endl;
   os << "Boundary = (0,0) (" << ww << "," << hh << ")" << endl;
   os << "#MetalLayers = 1" << endl;
   os << "#RoutedShapes = " << _blockList.size() << endl;
   os << "#RoutedVias = 0" << endl;
   os << "#Obstacles = 2" << endl;
   for(unsigned i = 0; i < _blockList.size(); ++i){
      os << "RoutedShape M1 "
         << "(" << _blockList[i]->getX() * scale << ","
         << _blockList[i]->getY() * scale << ") ("
         << (_blockList[i]->getX() + _blockList[i]->getWidth()) * scale << ","
         << (_blockList[i]->getY() + _blockList[i]->getHeight()) * scale << ")" 
         << endl;
   }
   for(unsigned i = 0; i < 4; ++i)
      os << "Obstacle M1 " << "(0,0) "
         << "(" << _outlineWidth << "," << _outlineHeight << ")"
         << endl;
}


void FloorPlanner::resultGnuplot() 
{
   // if(!_optSol.empty())
   //    writeRecord();

   try{
      Gnuplot gnu;
      gnu.reset_plot();
      int w = (_outlineWidth > _Width ? _outlineWidth * 1.5 : _Width * 1.2);
      int h = (_outlineHeight > _Height ? _outlineHeight * 1.5 : _Height * 1.2);
      int k = (w > h ? w : h);
      gnu.set_xrange(0, k).set_yrange(0, k);
      gnu << "set palette rgbformula -7,2,-7\n set cbrange [0:5]\n";

      stringstream ssj, ss5, ss6, ss7, ss8;
      ssj << 20000;
      ss5 << 0;
      ss6 << 0;
      ss7 << _outlineWidth;
      ss8 << _outlineHeight;

      gnu << ("set object " + ssj.str() + " rect from "
               + ss5.str() + "," + ss6.str() + " to "
               + ss7.str() + "," + ss8.str() + " fc rgb \"grey\" behind\n");

      for(unsigned i = 0; i < _blockList.size(); ++i){
         stringstream ssi, ss1, ss2, ss3, ss4, ssmx,ssmy;
         ssi << (i + 1);
         ss1 << (_blockList[i]->getX());
         ss2 << (_blockList[i]->getY());
         ss3 << (_blockList[i]->getX() + _blockList[i]->getWidth());
         ss4 << (_blockList[i]->getY() + _blockList[i]->getHeight());
         ssmx << (_blockList[i]->getX() + _blockList[i]->getWidth()  / 2);
         ssmy << (_blockList[i]->getY() + _blockList[i]->getHeight() / 2);
         string name = _blockList[i]->getName();

         gnu << ("set object " + ssi.str() + " rect from "
               + ss1.str() + "," + ss2.str() + " to "
               + ss3.str() + "," + ss4.str() + " fc rgb \"gold\" front \n");

         gnu << ("set label \"" + _blockList[i]->getName() + "\" " + " at " 
              + ssmx.str() + "," + ssmy.str() + " " + " front center\n");

      }
      // gnu << "pause 1\n";
      gnu.plot();
      
      cout << "Press [Enter] to end ... ";
      cin.get();

   }
   catch (GnuplotException ge){
     cout << ge.what() << endl;
   }

}




