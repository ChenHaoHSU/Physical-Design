/****************************************************************************
  FileName     [ buildRSG.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/

#include "SteinerRouter.h"

using namespace std;

extern time_t start;

bool sortPinY(const Pin& p1, const Pin& p2)
{
   if(p1.y() < p2.y()) return true;
   else if(p1.y() == p2.y() && p1.x() < p2.x()) return true;
   else return false;
}

bool sortPinX(const Pin& p1, const Pin& p2)
{
   if(p1.x() < p2.x()) return true;
   // else if(p1.x() == p2.x() && p1.y() < p2.y()) return true;
   else return false;
}

bool sortPinXPY(const Pin& p1, const Pin& p2)
{
   return p1.x() + p1.y() < p2.x() + p2.y();
}


bool sortPinXMY(const Pin& p1, const Pin& p2)
{
   return p1.x() - p1.y() < p2.x() - p2.y();
}


void SteinerRouter::buildRSG() 
{
   cout << "> Building RSG ... " << endl;

   /*
     sort _pinList by y-coordinate in non-decreasing order,
     break tie by x-coordinate
   */
   ::sort(_pinList.begin(), _pinList.end(), sortPinY);

   vector<int> R3_best;
   vector<int> R4_best;
   vector<int> R5_best;
   vector<int> R6_best;
   R3_best.resize(_numP);
   R4_best.resize(_numP);
   R5_best.resize(_numP);
   R6_best.resize(_numP);
   for(int i = 0; i < _numP; ++i){
      R3_best[i] = INT_MAX;
      R4_best[i] = INT_MAX;
      R5_best[i] = INT_MAX;
      R6_best[i] = INT_MAX;
   }

   bool R1_done = false;
   bool R2_done = false;
   bool R7_done = false;
   bool R8_done = false;

   int best_R1 = -1; // index of _pinList
   int best_R2 = -1; // index of _pinList
   int best_R7 = -1; // index of _pinList
   int best_R8 = -1; // index of _pinList

   int best_R1_Len = INT_MAX;
   int best_R2_Len = INT_MAX;
   int best_R7_Len = INT_MAX;
   int best_R8_Len = INT_MAX;

   Region rg = R_TOT;
   int tmpLen; // M_DISTANCE(refP, askP)

   bool first_done = false;

   // int checknum = 0;
   // int totalnum = 0;
   for(int k = 0; k < 2; ++k){
     // totalnum = 0;

     for(unsigned i = 0; i < _pinList.size(); ++i){
        Pin& refP = _pinList[i];

        // cout << endl << "dealing with " << i << " " << refP.node() << endl;

        R1_done = false;
        R2_done = false;
        R7_done = false;
        R8_done = false;
        best_R1 = -1; // index of _pinList
        best_R2 = -1; // index of _pinList
        best_R7 = -1; // index of _pinList
        best_R8 = -1; // index of _pinList
        best_R1_Len = INT_MAX;
        best_R2_Len = INT_MAX;
        best_R7_Len = INT_MAX;
        best_R8_Len = INT_MAX;

        // checknum = 0;
        for(unsigned j = i + 1; j < _pinList.size(); ++j){
           // ++checknum;
           // ++totalnum;
           Pin& askP = _pinList[j];
           rg = getRegion(refP, askP);

           // cout << askP.node() << " " << rg << endl;

           if(R1_done && R2_done && R7_done && R8_done) break;

           /* if it is no more need to check the region */
           if(!R1_done) R1_done = (askP.y() - refP.y() > best_R1_Len);
           if(!R2_done) R2_done = (askP.y() - refP.y() > best_R2_Len);
           if(!R7_done) R7_done = (askP.y() - refP.y() > best_R7_Len);
           if(!R8_done) R8_done = (askP.y() - refP.y() > best_R8_Len);

           /* calculate M_DISTANCE(refP, askP) */
           tmpLen = ADIFF(refP.x(), askP.x()) + ADIFF(refP.y(), askP.y());

           /* update the best node of region rg */
           if(rg == R1 && !R1_done){
              R5_best[j] = (R5_best[j] > tmpLen ? tmpLen : R5_best[j]);
              best_R1_Len = (best_R1_Len > tmpLen ? tmpLen : best_R1_Len);
              if(tmpLen <= best_R1_Len && tmpLen <= R5_best[j]){
                 // if((tmpLen < best_R1_Len && !first_done)
                 //   || (tmpLen < R5_best[j] && first_done))
                    best_R1 = j;
                 best_R1_Len = tmpLen;
                 if(!first_done)
                    R5_best[j] = tmpLen;
                 if(first_done)
                    R5_best[j] = INT_MIN;
              }
           }
           else if(rg == R2 && !R2_done){
              R6_best[j] = (R6_best[j] > tmpLen ? tmpLen : R6_best[j]);
              best_R2_Len = (best_R2_Len > tmpLen ? tmpLen : best_R2_Len);
              if(tmpLen <= best_R2_Len && tmpLen <= R6_best[j]){
                 // if((tmpLen < best_R2_Len && !first_done)
                 //   || (tmpLen < R6_best[j] && first_done))
                    best_R2 = j;
                 best_R2_Len = tmpLen;
                 if(!first_done)
                    R6_best[j] = tmpLen;
                 if(first_done)
                    R6_best[j] = INT_MIN;
              }
           }
           else if(rg == R7 && !R7_done){
              R3_best[j] = (R3_best[j] > tmpLen ? tmpLen : R3_best[j]);
              best_R7_Len = (best_R7_Len > tmpLen ? tmpLen : best_R7_Len);
              if(tmpLen <= best_R7_Len && tmpLen <= R3_best[j]){
                 // if((tmpLen < best_R7_Len && !first_done)
                 //   || (tmpLen < R3_best[j] && first_done))
                    best_R7 = j;
                 best_R7_Len = tmpLen;
                 if(!first_done)
                    R3_best[j] = tmpLen;
                 if(first_done)
                    R3_best[j] = INT_MIN;
              }

           }
           else if(rg == R8 && !R8_done){
              R4_best[j] = (R4_best[j] > tmpLen ? tmpLen : R4_best[j]);
              best_R8_Len = (best_R8_Len > tmpLen ? tmpLen : best_R8_Len);
              if(tmpLen <= best_R8_Len && tmpLen <= R4_best[j]){
                 // if((tmpLen < best_R8_Len && !first_done)
                 //   || (tmpLen < R4_best[j] && first_done))
                    best_R8 = j;
                 best_R8_Len = tmpLen;
                 if(!first_done)
                    R4_best[j] = tmpLen;
                 if(first_done)
                    R4_best[j] = INT_MIN;
              }
           }
           else{}

        }

        /* Build edges */
        if(first_done){
          if(best_R1 > 0)
            _eList.push_back(Edge(i, best_R1));
          if(best_R2 > 0)
            _eList.push_back(Edge(i, best_R2));
          if(best_R7 > 0)
            _eList.push_back(Edge(i, best_R7));
          if(best_R8 > 0)
            _eList.push_back(Edge(i, best_R8));
        }
        // cout << "[" << i << "] " << checknum << " / " << _numP << endl;
     }

     first_done = true;
     // cout << "totalnum = " << totalnum << endl;
   }

   cout << "  - RSG #Edges = " << _eList.size() << endl;
   cout << "  - Time       = " << getTime(start) << " seconds. " << endl << endl;

   // resultGNU(true);
   // resultGNU();

}


inline Region SteinerRouter::getRegion(const Pin& refP, const Pin& askP)
{
   /* 
    * askP is in what region of refP ?
    * [Note] When askP is between R1 and R2, it belongs to R1
    *        You can infer other situation, can't you?
    */
   if(refP.y() < askP.y()){
      if(refP.x() < askP.x()){
         if(ADIFF(refP.x(), askP.x()) <= ADIFF(refP.y(), askP.y()))
            return R1;
         else
            return R2;
      }
      else if(refP.x() > askP.x()){
         if(ADIFF(refP.x(), askP.x()) >= ADIFF(refP.y(), askP.y()))
            return R7;
         else
            return R8;
      }
      else // refP.x() == askP.x()
         return R8;

   }
   else if(refP.y() > askP.y()){
      if(refP.x() < askP.x()){
         if(ADIFF(refP.x(), askP.x()) >= ADIFF(refP.y(), askP.y()))
            return R3;
         else
            return R4;
      }
      else if(refP.x() > askP.x()){
         if(ADIFF(refP.x(), askP.x()) <= ADIFF(refP.y(), askP.y()))
            return R5;
         else
            return R6;
      }
      else // refP.x() == askP.x()
         return R4;
   }
   else{ // refP.y() == askP.y()
      if(refP.x() < askP.x())
         return R2;
      else if(refP.x() > askP.x())
        return R6;
      else
        assert(false);
   }

}





/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/




// #define NEW_METHOD

// #ifdef NEW_METHOD
// void SteinerRouter::buildRSG()
// {
//    cout << "> Building RSG ... " << endl;

//    ::sort(_pinList.begin(), _pinList.end(), sortPinXPY);

//    map<int, int> A1, A2, A3, A4;
//    map<int, int>::iterator iter;

//    printPinList();

//    for(unsigned i = 0; i < _pinList.size(); ++i){
//       Pin& refP = _pinList[i];
//       cout << endl << "dealing with " << refP.node() << endl;
//       cout << "A1.size = " << A1.size() << endl;
//       // R1, R2
//       bool flagR1 = true;
//       Pin bestP;
//       if(!A1.empty()){
//         iter = A1.begin();
//         do{
//            Pin& askP = _pinList[(*iter).second];
//            cout << "check " << askP.node() << endl;
//            if(refP.x() - refP.y() > askP.x() - askP.y() && getRegion(refP, askP) == R5 && !flagR1){
//            if(refP.x() - refP.y() >= askP.x() - askP.y()) flagR1 = false;
//            if(bestP.x() - bestP.y() >= askP.x() - askP.y()  && !flagR1){
//            if(refP.x() - refP.y() > askP.x() - askP.y()){
//               cout << "..erase " << askP.node() << endl;
      //         if(iter == A1.begin()){
      //            A1.erase(iter);
      //            break;
      //         }
      //         if(A1.empty()) break;
      //      }
      //      else if(getRegion(refP, askP) == R5 && flagR1){
      //           bestP = askP;
      //           cout << "..erase " << askP.node() << endl;
      //           _eList.push_back(Edge(i, (*iter).second));
      //           A1.erase(iter);
      //           flagR1 = false;
      //           break;
      //      }
      //      else {}
      //   } while (++iter != A1.end());
      // }

      // bool flagR2 = true;
      // if(!A2.empty()){
      //   iter = A2.begin();
      //   do{
      //      Pin& askP = _pinList[(*iter).second];
      //      // cout << "check " << askP.node() << endl;
      //      if(getRegion(refP, askP) == R6 && !flagR2){
      //         // cout << "..erase " << askP.node() << endl;
      //         A2.erase(iter);
      //         if(A2.empty()) break;
      //      }
      //      else if(getRegion(refP, askP) == R6 && flagR2){
      //           _eList.push_back(Edge(i, (*iter).second));
      //           A2.erase(iter);
      //           flagR2 = false;
      //           // break;
      //      }
      //      else {}
      //   } while (++iter != A2.end());
      // }

      // cout << "INSERT " << refP.node() << endl;
      // A1[refP.x() - refP.y()] = i;
      // A1[-refP.x()] = i;
      // A2[-refP.x()] = i;

   // }

  // ::sort(_pinList.begin(), _pinList.end(), sortPinXMY);


  // for(unsigned i = 0; i < _pinList.size(); ++i){
  //     Pin& refP = _pinList[i];
  //     bool flagR3 = true;
  //     if(!A3.empty()){
  //       iter = A3.begin();
  //       do{
  //          Pin& askP = _pinList[(*iter).second];
  //          // cout << "check " << askP.node() << endl;
  //          if(getRegion(refP, askP) == R7 && !flagR3){
  //             // cout << "..erase " << askP.node() << endl;
  //             A3.erase(iter);
  //             if(A3.empty()) break;
  //          }
  //          else if(getRegion(refP, askP) == R7 && flagR3){
  //               _eList.push_back(Edge(i, (*iter).second));
  //               A3.erase(iter);
  //               flagR3 = false;
  //               // break;
  //          }
  //          else {}
  //       } while (++iter != A3.end());
  //     }

  //     bool flagR4 = true;
  //     if(!A4.empty()){
  //       iter = A4.begin();
  //       do{
  //          Pin& askP = _pinList[(*iter).second];
  //          // cout << "check " << askP.node() << endl;
  //          if(getRegion(refP, askP) == R8 && !flagR4){
  //             // cout << "..erase " << askP.node() << endl;
  //             A4.erase(iter);
  //             if(A4.empty()) break;
  //          }
  //          else if(getRegion(refP, askP) == R8 && flagR4){
  //               _eList.push_back(Edge(i, (*iter).second));
  //               A4.erase(iter);
  //               flagR4 = false;
  //               // break;
  //          }
  //          else {}
  //       } while (++iter != A4.end());
  //     }

  //     cout << "INSERT " << refP.node() << endl;
  //     A3[-refP.x()] = i;
  //     A4[-refP.x()] = i;

  //  }
/*
  for(unsigned i = 0; i < _pinList.size(); ++i){
      Pin& refP = _pinList[i];
      if(!A3.empty()){
        iter = A3.end();
        --iter;
        do{
           Pin& askP = _pinList[(*iter).second];
           if(getRegion(refP, askP) == R7){
              _eList.push_back(Edge(i, (*iter).second));
              A3.erase(iter);
              break;
           }
        } while (iter-- != A3.begin());
      }

      if(!A4.empty()){
        iter = A4.end();
        --iter;
        do{
           Pin& askP = _pinList[(*iter).second];
           if(getRegion(refP, askP) == R8){
              _eList.push_back(Edge(i, (*iter).second));
              A4.erase(iter);
              break;
           }
        } while (iter-- != A4.begin());
      }
      A3[refP.x() + refP.y()] = i;
      A4[refP.x() + refP.y()] = i;
   }
*/

//    cout << "  - RSG #Edges = " << _eList.size() << endl << endl;

//    // resultGNU(true);
//    resultGNU();
// }