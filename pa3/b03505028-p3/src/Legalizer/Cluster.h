/****************************************************************************
  FileName     [ Cluster.h ]
  Authors      [ Chen-Hao Hsu ]
  Objective    [ 2017 Physical Design - Programming Assignment 3 
                 (Cell Legalization) ]
  Date         [ 2017/5/20 created ]
****************************************************************************/
#ifndef CLUSTER_H
#define CLUSTER_H

#include <iostream>
#include <vector>

using namespace std;

class Cluster
{
   friend class CLegal;

public:
   Cluster(double e = 0, double q = 0, double w = 0):_e(e), _q(q), _w(w) 
   { 
      _x = 0; 
      _n_first = _n_last = 0;
   }
   ~Cluster() {}

private:
   double _e; // total weight
   double _q; // optimal position = _q / _e
   double _w; // total width

   double _x; // left bottom x

   int _n_first; // index
   int _n_last; // index

};

#endif // CLUSTER_H