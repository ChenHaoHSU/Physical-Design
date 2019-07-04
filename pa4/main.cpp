/****************************************************************************
  FileName     [ main.cpp ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 4 
                 (Steiner-Tree Construction) ]
  Date         [ 2017/5/28 created ]
****************************************************************************/

#include <iostream>
#include <ctime>
#include "SteinerRouter.h"
#include "Util.h"

using namespace std;

time_t start;

int main(int argc, char** argv)
{
   start = clock();

   if(argc != 3 && argc != 4){
      cerr << "Usage: ./router <input.txt> <output.txt> [-plt]" << endl;
      cerr << "e.g. ./router case1.in case1.out" << endl;
      cerr << "e.g. ./router case2.in case2.out -plt" << endl;
      return -1;
   }

   SteinerRouter router;

   if(!router.parseAll(argv[1])){
      cerr << "Error: Parse failed." << endl;
      return -1;
   }

   router.buildRSG();
   router.buildMST();
   router.buildSteiner();
   router.refineRST();
   
   if(!router.writeResult(argv[2])){
      cerr << "Error: Output failed." << endl;
      return -1;
   }

   cout << ">> Running Time: " << getTime(start) 
        << " seconds."      << endl << endl
        << "////////////////////"   << endl
        << "Benchmark: " << argv[1] << endl
        << "Output   : " << argv[2] << endl;

   /* Output Gnuplot Format */
   if(argc == 4){
      router.outputGNUgraph();
      router.outputGNUtree();
      cout << "Gnuplot  : " << argv[1] << ".graph.plt (graph plot)" << endl;
      cout << "Gnuplot  : " << argv[1] << ".tree.plt  (only tree plot)" << endl;
   }

   return 0;
}
