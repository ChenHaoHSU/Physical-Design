#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

#define MAX_CELL_IN_NET 10
#define MAX_CELL_IDX    500000

int main(int argc, char** argv)
{
   if(argc != 4){
      cerr << "Usage: ./testGen <r_value> <#NET> <output_file_name>" << endl;
      cerr << "e.g. ./testGen 0.5 20 1.dat" << endl;
      return -1;
   }

   fstream outf(argv[3], ios::out);

   if(atof(argv[1]) < 1 && atof(argv[1]) > 0){
      outf << argv[1] << endl;
   }
   else{
      cerr << "Error: 0 < r < 1, " << argv[1] << " is out of range!" << endl;
      return -1;
   }

  srand((unsigned)time(NULL));

  bool cellTable[MAX_CELL_IDX];

  for(int i = 0; i < MAX_CELL_IDX; ++i)
     cellTable[i] = false;

  for(int i = 1; i <= atoi(argv[2]); ++i){
     cout << i << " / " << argv[2] << flush << '\r';
     outf << "NET " ;
     // for(unsigned i = 0; i < 3; ++i)
     //     outf << char(60 + (rand() % 67));
     //  outf << ' ';
     outf << 'n' << i << " ";

     for(int j = 0, n = (rand() % MAX_CELL_IN_NET) * 2000 + 1; j < n; j++){
            int idx = (rand() % MAX_CELL_IDX) + 1;
               outf << 'c' << idx << ' ';
               // for(unsigned i = 0; i < 5; ++i)
               //    outf << char(60 + (rand() % 67));
               // outf << ' ';
     }

     outf << ";" << endl;
     for(int i = 0; i < MAX_CELL_IDX; ++i)
        cellTable[i] = false;
   }

   outf.close();

   return 0;
}