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

#include "kernel.h"
#include <math.h>
#include <vector>

using namespace std;

// constructor has already been defined, as this object doesn't yet have any state
// we don't need to worry too much about it.. -- we should perhaps have made it a 
// functor, but am trying to avoid using new and delete too much..

double kernelFunction::radial(double sigma, double distance){
  return( exp(-(distance*distance) / (2 * (sigma * sigma))) );
}

double kernelFunction::radial(double base, double sigma, double distance){
  return( pow(base, -(distance*distance) / (2 * (sigma * sigma)) ) );
}

double kernelFunction::radial(double sigma, double* a, double* b, int n){
  double d = 0;
  // d is the euclidean distance..
  for(int i=0; i < n; i++){
    d += ((a[i] - b[i])*(a[i] - b[i]));    // this looks ugly, but I find it to be much faster than calling pow or something..
  }
  // since we are going to use the square of the ed, we don't need to square root it to get the E.D.
  return(exp(- d / (2 * (sigma * sigma))) );
}

double kernelFunction::radial(double sigma, vector<double> a, vector<double> b){
  double d = 0;
  if(a.size() != b.size()){
    cerr << "kernelFunction function, vectors are of different size .. returning 0 .. " << endl;
    return(0);
  }
  for(int i=0; i < a.size(); i++){
    d += ((a[i] - b[i])*(a[i] - b[i]));
  }
  return(exp(- d / (2 * (sigma * sigma))) );
}

// if we want more functions, we can put them in later..  
  
  
	   
