/****************************************************************************
  FileName     [ mydefine.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#ifndef MYDEFINE_H
#define MYDEFINE_H

#include <utility>
#include <iostream>

using namespace std;

/*
*  [ GNUPLOT ] (GUI)
*  - Turn it on to "see" the floorplan result visually.
*  - If it does not work, try to install gnuplot.
*    Command line:
*       $ sudo apt-get install gnuplot
*
*  - (#include "gnuplot_i.h")
*/
// #define GNU_PLOT


/*
*  [ GUI of 2017 CAD Contest Problem B ]
*  - Output the result in the format of 
*    2017 CAD Contest Problem B evaluator - eval.
*  - Output file name will always be "cad.out".
*  - Command line:
*    $ ./eval cad.out
*    M1.svg will be generated. Open it and you will see the result.
*/
// #define CAD_CONTEST_GUI


/* SA arguments initial value */
#define T_INIT       100000.0
#define T_FROZEN     0.000001
#define P_INIT       0.987
#define L_INIT       200 
#define N_INIT       0
#define K_INIT       7
#define C_INIT       100
#define ITER_LIMIT   1000

#define W_INIT       0.5
#define X_INIT       0.5
#define Y_INIT       0.00
#define Z_INIT       0.0

/* Other define */
#define TIME_LIMIT   3000.0
#define RANDOM_RANGE 100000
#define NULL_SIZE    33

#define INIT_PERTURB_NUM 100

/* Class Declaration */
class FloorPlanner;
class Block;
class BlockV;
class BlockR;
class Terminal;
class TerminalV;
class Net;
class Btree;
class RecoverMsg;
class ContourMgr;
class ContourLine;

typedef pair<int, int> Interval;

/* Perturbation Modes */
enum OptType
{
   LEFT_ROTATE     = 0,
   RIGHT_ROTATE    = 1,
   DELETE_INSERT   = 2,
   SWAP_TWO_BLOCKS = 3,
   ROTATE_BLOCK    = 4,
   // dummy end
   OPT_TOT         = 5
};

enum Direction
{
   LEFT       = 0,
   RIGHT      = 1,
   // dummy end
   DIRECT_TOT = 2
};

#endif // MYDEFINE_H 


