//Copyright Notice
/*
    eXintegrator integrated expression analysis system
    Copyright (C) 2004  Martin Jakt & Okada Mitsuhiro
  
    This file is part of the eXintegrator integrated expression analysis system. 
    eXintegrator is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version. 

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    PS. If you can think of a better name, please let me know...
*/
//End Copyright Notice

#ifndef TWODIMDIST_H
#define TWODIMDIST_H

#include "kernelFunction.h"
#include <qthread.h>
#include <qmutex.h>
#include <vector>

// a minimal mapping function that makes a 2 dimensional grid distribution
// graph using a radial kernel function to even out the graph.. 

struct distMap {
  int on;
  int in;   // outer and inner n, -- keep the same here, but in case we want to have non square plots.. 
  double** values;
  std::vector<int> index;
  std::vector<double> a;
  std::vector<double> b;   // a --> x  and b --> y .. well, a --> outer loop, b --> inner loop
  //double** dist;
  double minX, maxX, minY, maxY, maxDistValue, minDistValue;  // 
  distMap(int o, int i, double** v, double max, double mix, double may, double miy, double mdv, double mindv, int* ind, double* av, double* bv, int n){
    on = o;
    in = i;
    values = v;
    maxX = max;
    minX = mix;
    maxY = may;
    minY = miy;
    maxDistValue = mdv;
    minDistValue = mindv;
    a.resize(n);
    b.resize(n);
    index.resize(n);
    for(int i=0; i < n; i++){
      a[i] = av[i];
      b[i] = bv[i];
      index[i] = ind[i];
    }
  }
  ~distMap(){
    for(int i=0; i < on; i++){
      delete []values[i];
    }
    delete []values;
  }
};

class TwoDimDist : public QThread
{

  public :
    TwoDimDist(double* av, double* bv, int nv, int* indx, QMutex* mutx, int* prog, int sig, int nb=100, double mnx=0, double mxx=0, double mny=0, double mxy=0);
  // basically, two arrays of floats, an int telling the number of members, 
  // a qmutex that is used to update the progress int (0-100) telling the parent of the 
  // the progress.. And number of buckets defaulting to a 100.
  ~TwoDimDist();              // Delete a and b, but nothing else.
  distMap* distribution();     // return the distribution 

  protected :
    void run();

  private :
    bool maxesUserSet;
    double* a;
  double* b;
  int* index;
  int n;
  QMutex* mutex;
  int* progress;
  int bn;   // bucket number  -- just do square maps for now.. 
  double** dist;

  double sigma;   // parameter for the kernel function.. 
  kernelFunction kf;   // set up before we enter a new thread. 

  double maxX, minX, maxY, minY, maxDist, minDist;  // important distribution parameters.. 

};

#endif

