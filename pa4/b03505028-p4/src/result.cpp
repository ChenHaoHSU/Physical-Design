/****************************************************************************
  FileName     [ result.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/

#include "SteinerRouter.h"
#include "gnuplot_i.h"

using namespace std;

bool SteinerRouter::writeResult(char* outfileName)
{
   ofstream outf(outfileName, ios::out);
   if(!outf){
      cerr << "Error: Cannot open output file \"" << outfileName << "\"." << endl;
      return false;
   }

   result(outf);
   
   outf.close();

   // result(cout);
   return true;
}


void SteinerRouter::result(ostream& outf) const
{
   /*
    * [ Output format ]
    *
    *   NumRoutedPins = number
    *   WireLength = number
    *   H-line (x1,y) (x2,y)
    *   V-line (x,y1) (x,y2) 
    *   ...
    *   
    *   Note:
    *   H-line/V-line can be output in any order
    */

   outf << "NumRoutedPins = " << _numP << endl;
   outf << "WireLength = " << _rsmtCost << endl;
   for(unsigned i = 0; i < _rsmtList.size(); ++i){
      outf << _rsmtList[i].HVstr() << " "
           << _rsmtList[i].n1()    << " "
           << _rsmtList[i].n2()    << endl;
   }
}

void SteinerRouter::resultGNU(bool autoplay) const
{
   try{
      Gnuplot gnu;
      gnu.reset_plot(); 

      double deltaX = _boundaryUR.first - _boundaryLL.first;
      double deltaY = _boundaryUR.second - _boundaryLL.second;
      double marginX = 0.1 * deltaX;
      double marginY = 0.1 * deltaY;
      double LX = _boundaryLL.first - marginX;
      double RX = _boundaryUR.first + marginX;
      double LY = _boundaryLL.second - marginY;
      double UY = _boundaryUR.second + marginY;

      gnu.set_xrange(LX, RX).set_yrange(LY, UY);
      gnu << "set palette rgbformula -7,2,-7\n set cbrange [0:5]\n";

      int idx = 1;

      /* print boundary */
      stringstream ssj, ss5, ss6, ss7, ss8;
      ssj << ++idx;
      ss5 << _boundaryLL.first;
      ss6 << _boundaryLL.second;
      ss7 << _boundaryUR.first;
      ss8 << _boundaryUR.second;
      
      gnu << ("set object " + ssj.str() + " rect from "
               + ss5.str() + "," + ss6.str() + " to "
               + ss7.str() + "," + ss8.str() + " fc rgb \"grey\" behind\n");
      
      /* print vertices */
      for(unsigned i = 0; i < _pinList.size(); ++i){
         stringstream ssx, ssy;
         ssx << _pinList[i].x();
         ssy << _pinList[i].y();
         gnu << ("set object circle at first " 
             + ssx.str() + "," + ssy.str()
             + " radius char 0.25 fillstyle empty border lc rgb '#aa1100' front\n");
      }


      /* print _eList */
      for(unsigned i = 0; i < _eList.size(); ++i){
            stringstream ss1, ss2, ss3, ss4, ssi;
            ssi << (++idx);
            ss1 << _pinList[_eList[i].v1()].x();
            ss2 << _pinList[_eList[i].v1()].y();
            ss3 << _pinList[_eList[i].v2()].x();
            ss4 << _pinList[_eList[i].v2()].y();
            gnu << ("set arrow " +  ssi.str() + " from "
                + ss1.str() + "," + ss2.str() + " to "
                + ss3.str() + "," + ss4.str() + " nohead lc rgb \"white\" lw 1front \n");
      }

      /* print _mstList */
      for(unsigned i = 0; i < _mstList.size(); ++i){
            stringstream ss1, ss2, ss3, ss4, ssi;
            ssi << (++idx);
            ss1 << _pinList[_mstList[i].v1()].x();
            ss2 << _pinList[_mstList[i].v1()].y();
            ss3 << _pinList[_mstList[i].v2()].x();
            ss4 << _pinList[_mstList[i].v2()].y();
            gnu << ("set arrow " +  ssi.str() + " from "
                + ss1.str() + "," + ss2.str() + " to "
                + ss3.str() + "," + ss4.str() + " nohead lc rgb \"blue\" lw 1 front\n");
      }


      /* print _rstList */
      for(unsigned i = 0; i < _rstList.size(); ++i){
            stringstream ss1, ss2, ss3, ss4, ssi;
            ssi << (++idx);
            ss1 << _rstList[i].n1().first;
            ss2 << _rstList[i].n1().second;
            ss3 << _rstList[i].n2().first;
            ss4 << _rstList[i].n2().second;
            gnu << ("set arrow " +  ssi.str() + " from "
                + ss1.str() + "," + ss2.str() + " to "
                + ss3.str() + "," + ss4.str() + " nohead lc rgb \"black\" lw 3 front\n");
      }


      /* print _rsmtList */
      for(unsigned i = 0; i < _rsmtList.size(); ++i){
            stringstream ss1, ss2, ss3, ss4, ssi;
            ssi << (++idx);
            ss1 << _rsmtList[i].n1().first;
            ss2 << _rsmtList[i].n1().second;
            ss3 << _rsmtList[i].n2().first;
            ss4 << _rsmtList[i].n2().second;
            gnu << ("set arrow " +  ssi.str() + " from "
                + ss1.str() + "," + ss2.str() + " to "
                + ss3.str() + "," + ss4.str() + " nohead lc rgb \"black\" lw 2 front\n");
      }


      gnu.plot();

      if(!autoplay){
         cout << "Press [Enter] to end ... ";
         cin.get();
      }
      else{
         delay();
      }

   }
   catch (GnuplotException ge){
     cout << ge.what() << endl;
   }

}


void SteinerRouter::outputGNUgraph() const
{
   string fileName = _benchmark + ".graph.plt";

   ofstream outf(fileName.c_str(), ios::out);
   outf << "set title \"Steiner tree of " << _benchmark << "\" " << endl;
   outf << "set size ratio -1" << endl;
   outf << "set nokey" << endl;

   double deltaX = _boundaryUR.first - _boundaryLL.first;
   double deltaY = _boundaryUR.second - _boundaryLL.second;
   double marginX = 0.1 * deltaX;
   double marginY = 0.1 * deltaY;

   outf << "set xrange[" << _boundaryLL.first - marginX << ":" << _boundaryUR.first + marginX << "]" << endl;
   outf << "set yrange[" << _boundaryLL.second - marginY << ":" << _boundaryUR.second + marginY << "]" << endl;
   outf << "set palette rgbformula -7,2,-7\n set cbrange [0:5]\n";

   int idx = 1;

   /* print boundary */
   stringstream ssj, ss5, ss6, ss7, ss8;
   ssj << ++idx;
   ss5 << _boundaryLL.first;
   ss6 << _boundaryLL.second;
   ss7 << _boundaryUR.first;
   ss8 << _boundaryUR.second;
   
   outf << ("set object " + ssj.str() + " rect from "
            + ss5.str() + "," + ss6.str() + " to "
            + ss7.str() + "," + ss8.str() + " fc rgb \"grey\" behind\n");
   
   /* print vertices */
   for(unsigned i = 0; i < _pinList.size(); ++i){
      stringstream ssx, ssy;
      ssx << _pinList[i].x();
      ssy << _pinList[i].y();
      outf << ("set object circle at first " 
          + ssx.str() + "," + ssy.str()
          + " radius char 0.25 fillstyle empty border lc rgb '#aa1100' front\n");
   }


   /* print _eList */
   for(unsigned i = 0; i < _eList.size(); ++i){
         stringstream ss1, ss2, ss3, ss4, ssi;
         ssi << (++idx);
         ss1 << _pinList[_eList[i].v1()].x();
         ss2 << _pinList[_eList[i].v1()].y();
         ss3 << _pinList[_eList[i].v2()].x();
         ss4 << _pinList[_eList[i].v2()].y();
         outf << ("set arrow " +  ssi.str() + " from "
             + ss1.str() + "," + ss2.str() + " to "
             + ss3.str() + "," + ss4.str() + " nohead lc rgb \"white\" lw 1 front \n");
   }

   /* print _mstList */
   for(unsigned i = 0; i < _mstList.size(); ++i){
         stringstream ss1, ss2, ss3, ss4, ssi;
         ssi << (++idx);
         ss1 << _pinList[_mstList[i].v1()].x();
         ss2 << _pinList[_mstList[i].v1()].y();
         ss3 << _pinList[_mstList[i].v2()].x();
         ss4 << _pinList[_mstList[i].v2()].y();
         outf << ("set arrow " +  ssi.str() + " from "
             + ss1.str() + "," + ss2.str() + " to "
             + ss3.str() + "," + ss4.str() + " nohead lc rgb \"blue\" lw 1 front\n");
   }


   /* print _rstList */
   // for(unsigned i = 0; i < _rstList.size(); ++i){
   //       stringstream ss1, ss2, ss3, ss4, ssi;
   //       ssi << (++idx);
   //       ss1 << _rstList[i].n1().first;
   //       ss2 << _rstList[i].n1().second;
   //       ss3 << _rstList[i].n2().first;
   //       ss4 << _rstList[i].n2().second;
   //       outf << ("set arrow " +  ssi.str() + " from "
   //           + ss1.str() + "," + ss2.str() + " to "
   //           + ss3.str() + "," + ss4.str() + " nohead lc rgb \"black\" lw 2 front\n");
   // }


   /* print _rsmtList */
   for(unsigned i = 0; i < _rsmtList.size(); ++i){
         stringstream ss1, ss2, ss3, ss4, ssi;
         ssi << (++idx);
         ss1 << _rsmtList[i].n1().first;
         ss2 << _rsmtList[i].n1().second;
         ss3 << _rsmtList[i].n2().first;
         ss4 << _rsmtList[i].n2().second;
         outf << ("set arrow " +  ssi.str() + " from "
             + ss1.str() + "," + ss2.str() + " to "
             + ss3.str() + "," + ss4.str() + " nohead lc rgb \"black\" lw 2 front\n");
   }

   outf << "plot '-' with lines  " << endl;
   outf << "0 0" << endl;
   outf << "EOF" << endl;
   // wait
   outf << "pause -1 'Press any key'" << endl;

   outf.close();

}



void SteinerRouter::outputGNUtree() const
{
   string fileName = _benchmark + ".tree.plt";
   ofstream outf(fileName.c_str(), ios::out);

   outf << "set title \"Steiner tree of " << _benchmark << "\" " << endl;
   outf << "set size ratio -1" << endl;
   outf << "set nokey" << endl;

   double deltaX = _boundaryUR.first - _boundaryLL.first;
   double deltaY = _boundaryUR.second - _boundaryLL.second;
   double marginX = 0.1 * deltaX;
   double marginY = 0.1 * deltaY;

   outf << "set xrange[" << _boundaryLL.first - marginX << ":" << _boundaryUR.first + marginX << "]" << endl;
   outf << "set yrange[" << _boundaryLL.second - marginY << ":" << _boundaryUR.second + marginY << "]" << endl;
   outf << "set palette rgbformula -7,2,-7\n set cbrange [0:5]\n";
 
   int idx = 1;

   /* print boundary */
   stringstream ssj, ss5, ss6, ss7, ss8;
   ssj << ++idx;
   ss5 << _boundaryLL.first;
   ss6 << _boundaryLL.second;
   ss7 << _boundaryUR.first;
   ss8 << _boundaryUR.second;

   outf << ("set object " + ssj.str() + " rect from "
           + ss5.str() + "," + ss6.str() + " to "
           + ss7.str() + "," + ss8.str() + " fc rgb \"grey\" behind\n");

   /* print vertices */
   for(unsigned i = 0; i < _pinList.size(); ++i){
      stringstream ssx, ssy;
      ssx << _pinList[i].x();
      ssy << _pinList[i].y();
      outf << ("set object circle at first " 
          + ssx.str() + "," + ssy.str()
          + " radius char 0.25 fillstyle empty border lc rgb '#aa1100' front\n");
   }


   /* print _eList */
   for(unsigned i = 0; i < _rsmtList.size(); ++i){
      stringstream ss1, ss2, ss3, ss4, ssi;
      ssi << (++idx);
      ss1 << _rsmtList[i].x1();
      ss2 << _rsmtList[i].y1();
      ss3 << _rsmtList[i].x2();
      ss4 << _rsmtList[i].y2();
      outf << ("set arrow " +  ssi.str() + " from "
           + ss1.str() + "," + ss2.str() + " to "
           + ss3.str() + "," + ss4.str() + " heads lc rgb \"black\" lw 1front \n");
   }
  
   outf << "plot '-' with lines  " << endl;
   outf << "0 0" << endl;
   outf << "EOF" << endl;

   // wait
   outf << "pause -1 'Press any key'" << endl;

   outf.close();
}