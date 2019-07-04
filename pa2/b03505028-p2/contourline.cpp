/****************************************************************************
  FileName     [ contourline.cpp ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/17 created ]
****************************************************************************/

#include "contourline.h"

using namespace std;


void ContourMgr::setWidth(int w) 
{ 
   _width = w;
   _dummy->_front = 0;
   _dummy->_back = new ContourLine(_dummy, 0, Interval(0, _width));
}


int ContourMgr::insert(const Interval inter, int h) 
{
   /*
      h: height of the inserted block
      inter.first = Xmin of the inserted block
      inter.second = Xmax of the inserted block
   */


   /* avoid the null node ... (no width) */
   if(inter.first == inter.second) return 0;
   
   assert(inter.first < inter.second);

   /* 
      First, locate the begining line 
      and ending line of the inserted block 
   */
   ContourLine* firstLine = 0; 
   ContourLine* lastLine = 0;
   int max = 0;

   ContourLine* pt = _dummy->_back;
   // Locate first line where inter.first should be in
   while(pt != 0){
      if(pt->getFirst() <= inter.first && pt->getSecond() > inter.first){
         firstLine = pt;
         if(pt->_height > max)
            max = pt->_height;
         break;
      }
      pt = pt->_back;
   }

   // Locate last line where inter.second should be in
   while(pt != 0){
      if(pt->_height > max)
         max = pt->_height;
      if(pt->getFirst() < inter.second && pt->getSecond() >= inter.second){
         lastLine = pt;
         break;
      }
      pt = pt->_back;
   }

   /* 
      Finally, below are two cases for 
      maintaining the list "continuous" 
      and delete the "invalid" lines 
   */
   // Case 1: only one line concerned
   if(firstLine == lastLine){ 
      ContourLine* middleLine = new ContourLine(firstLine, 0, inter, max + h);
      ContourLine* backLine = new ContourLine(middleLine, firstLine->_back, 
                   Interval(inter.second, firstLine->getSecond()), firstLine->_height);
      middleLine->_back = backLine;
      if(firstLine->_back) firstLine->_back->_front = backLine;
      firstLine->_back = middleLine;

      firstLine->resizeFromFront(inter.first);

      // delete the invalid line (i.e. Interval(X,X)) with interval width = 0
      if(!firstLine->isValid())
         deleteL(firstLine);
      if(!backLine->isValid())
         deleteL(backLine);
   }

   // Case 2: across more than one line
   else{ 
      firstLine->resizeFromFront(inter.first);
      lastLine->resizeFromBack(inter.second);
      ContourLine* middleLine = new ContourLine(firstLine, lastLine, inter, max + h);

      ContourLine* pt = firstLine->_back;
      
      firstLine->_back = middleLine;
      lastLine->_front = middleLine;

      // delete the lines covered by the new bigger one
      while(pt != lastLine && pt != 0){
         ContourLine* tmp = pt->_back;
         delete pt;
         pt = tmp;
      }

      // delete invalid line (i.e. Interval(X,X)) with interval width = 0
      if(!firstLine->isValid())
         deleteL(firstLine);
      if(!lastLine->isValid())
         deleteL(lastLine);
   }

   if((max + h) > _maxY) _maxY = (max + h); // update the _maxY of the overall contour line

   return max; // return lower y for the block!!
}


void ContourMgr::deleteL(ContourLine* die)
{
   assert(die != _dummy);
   die->_front->_back = die->_back;
   if(die->_back != 0)
      die->_back->_front = die->_front;
   delete die;
}


void ContourMgr::reset()
{
   clear();
   setWidth(INT_MAX); 
   _maxY = 0;
}


void ContourMgr::printLines() const
{
   ContourLine* pt = _dummy->_back;
   while(pt != 0){

      cout << pt->_height << " "
           << "(" << pt->getFirst() << "," 
           << pt->getSecond() << ") ";

      // Print pt->_front
      if(pt->_front) 
         cout << "F: " << pt->_front->_intvl.first << " "
              << pt->_front->_intvl.second << " ";
      else cout << "F: xxx ";

      // Print pt->_back
      if(pt->_back) 
         cout << "B: " << pt->_back->_intvl.first << " " 
                       << pt->_back->_intvl.second << " ";
      else cout << "B: xxx ";
      cout << endl;

      pt = pt->_back;
   }
}


void ContourMgr::clear()
{
   ContourLine* pt = _dummy->_back;
   while(pt != 0){
      ContourLine* tmp = pt->_back;
      delete pt;
      pt = tmp;
   }
   _dummy->_back = _dummy->_front = 0;
}

