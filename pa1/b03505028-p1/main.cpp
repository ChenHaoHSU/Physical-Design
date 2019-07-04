/****************************************************************************
  FileName     [ main.cpp ]
  PackageName  [ pa1 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 1 
                 (2-Way F-M Circuit Partitioning) ]
  Date         [ 2017/3/18 created ]
****************************************************************************/

#include <iostream>
#include <ctime>
#include "Net.h"

using namespace std;

int main(int argc, char** argv)
{
   time_t start = clock();

   if(argc != 3){
      cerr << "Usage: ./fm <input_file_name> <output_file_name>" << endl;
      cerr << "e.g. ./fm input_1.dat output_1.dat" << endl;
      return -1;
   }

   NetManager mgr;

   if(!mgr.parseAll(argv[1])){
      cerr << "Error: Parse failed." << endl;
      return -1;
   }

   if(!mgr.FM()){
      cerr << "Error: Partition failed." << endl;
      return -1;
   }

   if(!mgr.writeResult(argv[2])){
      cerr << "Error: Output failed." << endl;
      return -1;
   }

   // mgr.result(cout); // print the result on the screen

   time_t end = clock();
   float t_used = (float)(end - start) / CLOCKS_PER_SEC;
   cout << ">> Running Time: " << t_used << " seconds." << endl;

   return 0;

}