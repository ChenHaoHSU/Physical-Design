/****************************************************************************
  FileName     [ main.cpp ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#include <iostream>
#include <ctime>
#include "floorplanner.h"

using namespace std;

/* Global variable */
time_t start; // global timer


int main(int argc, char** argv)
{
   start = clock(); // timer start

   if(argc != 5){
      cerr << "Usage: ./Floorplan.exe <alpha> <block_file_name> <net_file_name> <output_file_name>" << endl;
      cerr << "e.g. ./Floorplan.exe 0.5 input.block input.nets output.result" << endl;
      return -1;
   }

   FloorPlanner planner;

   if(!planner.parseAll(argv[1], argv[2], argv[3])){
      cerr << "Error: Parse failed." << endl;
      return -1;
   }

   if(!planner.floorplan()){
      cerr << "Error: Floorplanning failed." << endl;
      return -1;
   }

   if(!planner.writeResult(argv[4])){
      cerr << "Error: Output failed." << endl;
      return -1;
   }

   time_t end = clock(); // timer end
   float t_used = (float)(end - start) / CLOCKS_PER_SEC;
   cout << ">> Running Time: " << t_used << " seconds." << endl;

   #ifdef GNU_PLOT
   planner.resultGnuplot();
   #endif

   return 0;
}
