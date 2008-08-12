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

#include <iostream>
#include "twoDimDist.h"
#include "kernelFunction.h"
#include <math.h>

using namespace std;

TwoDimDist::TwoDimDist(double* av, double* bv, int nv, int* indx, QMutex* mutx, int* prog, int sig, int nb, double mnx, double mxx, double mny, double mxy){
  a = av;
  b = bv;
  n = nv;
  index = indx;
  mutex = mutx;
  progress = prog;
  if(nb < 10){
    nb = 10;
  }
  bn = nb;
  sigma = (double)((sig * bn)/100); 
  dist = new double*[bn];
  for(int i=0; i < bn; i++){
    dist[i] = new double[bn];
    for(int j=0; j < bn; j++){
      dist[i][j] = 0;
    }
  }
  // ok.. check if the user has set mins and maxes..
  if((mnx == 0 && mxx == 0 && mny ==0 && mxy == 0) || mxx - mnx == 0 || mxy - mny == 0){
    maxesUserSet = false;
  }else{
    maxesUserSet = true;
    minX = mnx;
    maxX = mxx;
    minY = mny;
    maxY = mxy;
  }
}

TwoDimDist::~TwoDimDist(){
  cout << "Deleting a TwoDimDist .... but deleting a and b, but not bumch esle.. " << endl;
  // delete the a and b, as we are probably not going to use these again.. 
  delete []a;
  delete []b;   // and it really looks like it would be inconvenient to do otherwise.. 
  delete []index;
}

distMap* TwoDimDist::distribution(){
  return(new distMap(bn, bn, dist, maxX, minX, maxY, minY, maxDist, minDist, index, a, b, n));   
}

void TwoDimDist::run(){
  // first determine the min and maximum positions.. of the thingies..
  cout << "Dism mapper run function " << endl;
  if(n < 10){
    cerr << "TwoDimDist run function n, is smaller than 10, this makes no sense,, return.. " << n << endl;
    return;
  }
  if(!maxesUserSet){
    maxX = a[0];
    minX = a[0];
    maxY = b[0];
    minY = b[0];
    //cout << "maxes and mins initi.. " << endl;
    for(int i=0; i < n; i++){
      if(maxX < a[i]){ maxX = a[i]; }
      if(minX > a[i]){ minX = a[i]; }
      if(maxY < b[i]){ maxY = b[i]; }
      if(minY > b[i]){ minY = b[i]; }
    }
  }
  //cout << "maxes and mins set " << endl;
  double xrange = maxX - minX;
  double yrange = maxY - minY;
  double x, y;   // for the paramters.. 
  double xi, yi;

  //  sigma = (xrange + yrange) / 2;  // seems as reasonable as any other number,, though I'm not sure
  //sigma = (double)(bn/2);
  // sigma = 5.0;   -- sigma is set in the constructor.. 
                                  // certainly could be the case that we should do differently, but.. 
  // Come to think of it, sigma should probably be different for the two dimensions, or rather that we should
  // map the two dimensions onto the grid,, and then use the grid dimensions, which would sort of become similar..
  // oh well, I suppose that we can try that out later on when we have some sort of functioning interface.

  maxDist = 0;
  // min dist value is anyway 0, so no need to worry.. 
  //cout << "max dist set and stuff. " << endl;
  for(int i=0; i < bn; i++){
    //cout << i << "  " << endl;
    //x = (xrange * ((double)i) / ((double)bn)) + minX;
    //cout << "and the x value is : " << x << endl;
    //cout << "and then something else, and something apart from that " << endl;
    //cout << "and bn is equal to .. " << bn << endl;
    xi = (double)i;
    for(int j=0; j < bn; j++){
      //cout << j << "   ";
      //y = (yrange * ((double)j) / ((double)bn)) + minY;
      //cout << "and y is : " << y << endl;
      yi = (double)j;
      for(int k=0; k < n; k++){
	// calculate distance between point k, and current grid point.
	// which is mapped onto the x and y coordinates given above..
	//cout << "\t\t\tk : " << k << endl;
	// first map the coordinates to the grid system..
	x = ((double)bn) * (a[k] - minX)/xrange;
	y = ((double)bn) * (b[k] - minY)/yrange;
	// then calculate the distance..
	double d = sqrt( ((x - xi)*(x-xi)) + ((y-yi)*(y-yi)) );
	//	double d = sqrt( ((x - a[k])*(x- a[k])) + ((y-b[k])*(y-b[k])) );
	//cout << "d is : " << d << endl;
	dist[i][j] += kf.radial(sigma, d);
	//cout << "calculated the radial thingy.. " << endl;
      }
      if(dist[i][j] > maxDist){ maxDist = dist[i][j]; }
    }
    // let's work out the mindist.. 
    if(bn > 0){
      minDist = dist[0][0];
      for(int i=0; i < bn; i++){
	for(int j=0; j < bn; j++){
	  if(minDist > dist[i][j]){ minDist = dist[i][j]; }
	}
      }
    }
    //cout << endl;
    mutex->lock();
    *progress = (100 * (i+1)) / bn;
    //    cout << "i is : " << i << "  bn is " << bn << "  so progress is " << *progress << endl;
    mutex->unlock();
    //cout << "set the progress to something or other . " << endl;
  }
  // and that is pretty much all that we need to do I think... 

}
