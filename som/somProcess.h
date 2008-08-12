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

#ifndef SOMPROCESS_H
#define SOMPROCESS_H

#include <qthread.h>
#include <vector>
#include <qmutex.h>

using namespace std;

struct mapDescription {   // a description of a map
  vector<vector<float> > mapcd;   // the full set of coordinates.. 
  int mc1, mc2;          // the major coordinates.
  int mw, mh;              // width and height of the map
  vector<vector<float> > ivalues;  // the input vectors used in the construction
  vector<int> index;               // the index for these..
  vector<int> membership;          // the membership.. 
  mapDescription(){
    mc1 = mc2 = 0;
    mw = mh = 0;
    mapcd.resize(0);
  }           
  ~mapDescription(){}
  mapDescription(vector<vector<float> > mc, int pc1, int pc2, int w, int h, vector<vector<float> > ic, vector<int> indx, vector<int> membship);
};

class SomProcess : public QThread
{
  public :
  SomProcess(vector<int> indx, vector<vector<float> > v, QMutex* mutx, vector<mapDescription>* maps);   // indices, and n-dimensional values.. of some sort
  ~SomProcess();
  
  void setFactors(float a, float s);     // set the sigma and alpha values..  
  void initMap();                // initialize the map positions -- public so we can restart the mapping or continue it, or whatever really.. 
  
  private :
  vector<int> index;
  vector<vector<float> > values;
  vector<vector<float> > smap;    // the map positions.. 
  vector<mapDescription>* parentMaps;   // change these at the end of each generation.. -- lock mutex..before. and then change.. 
  int nodeSqrt;                   // the squareRoot of the number of nodes in the map,, --use a square map initially,, 
                                  // later we can perhaps change this depending on the actual dimensions of the two fields.. 
  int nodeNo;                     // just for convenience.. 
  bool initOk;                   // if not ok, then do nothing in the run process.. or something like that..
  QMutex* mutex;
  int dimNo;
  float alpha;                    // the learning rate
  float sigma;                    // the variance of the 
  float sigmaMultiplier;          // sigma is set depending on some values here, but we can modify the multiplier thingy.. 
  int mc1, mc2;                   // the major components.. 
  // some functions..
  float sqEuclidean(vector<float>& v1, vector<float>& v2);              // need a local function.. for thread safety.. -should be a better way of doing this..
  ///////// the above returns the square of the euclidean, as anyway, this is what we will use in a lot of the equations.. 
  float moveNode(vector<float>& node, vector<float>& winner, vector<float>& target);     // moves the node towards the target..
                                                                   // using the sqrt and aother things.. -- i.e. modifies the arguments.. 

  protected :
  void run();
};

#endif
