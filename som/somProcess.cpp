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

#include "somProcess.h"
#include <qthread.h>
#include <qmutex.h>
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

mapDescription::mapDescription(vector<vector<float> > mc, int pc1, int pc2, int w, int h, vector<vector<float> > ic, vector<int> indx, vector<int> membship){
  // check stuff to make sure it's ok..
  if(!mc.size()){
    cerr << "  what 's going oin.. mc is empty " << endl;
    return;
  }
  int dimNo = mc[0].size();
  if(pc1 >= dimNo || pc2 >= dimNo){
    cerr << "pc1 or pc2 is larger than dimNo,, ehh. " << endl;
    return;
  }
  if(w * h != mc.size()){
    cerr << "     w times h doesnnt equal mc.size " << endl;
    return;
  }
  if(ic.size() != indx.size()){
    cerr << "    ic size is not equal to index. size() or someting.. " << endl;
    return;
  }
  // and then check to make sure all of the points have the right dimensions..
  for(int i=0; i < mc.size(); i++){
    if(mc[i].size() != dimNo){
      cout << "mc i size is not the same as dimNo... bugger i :" << i << endl;
      return;
    }
  }
  for(int i=0; i < ic.size(); i++){
    if(ic[i].size() != dimNo){
      cerr << "ic i size is not the same as dim No... ok.a.d.d ldkd i: " << i << endl;
      return;
    }
  }
  if(membship.size() != indx.size()){
    cout << "membship size is wrong, doing nothing... " << endl;
    return;
  }
  //for(int i=0; i < membship.size(); i++){
  //  if(membship[i] >= mc.size()){
  //  cout << "membship i is larger than mc size doesn't make much sense i: " << i << "  membship i : " << membship[i] << endl;
  //  return;
  // }
  //}
  // if we get here just copy everything in to the things..
  mapcd = mc;
  mc1 = pc1;
  mc2 = pc2;
  mw = w;
  mh = h;
  ivalues = ic;
  index = indx;
  membership = membship;
}

SomProcess::SomProcess(vector<int> indx, vector<vector<float> > v, QMutex* mutx, vector<mapDescription>* maps){
  values = v;
  index = indx;
  mutex = mutx;
  nodeSqrt = 50;        // make a map with 10000 nodes, -i.e. 100 x 100..  -- that's a big map, but I think thats what we are looking for.. 
  nodeNo = nodeSqrt * nodeSqrt;
  initOk = true;    // check below..
  parentMaps = maps;
  sigmaMultiplier = 1.0;
  alpha = 0.6;     // defaults.. 
  // make some checks..
  cout << "SomProces being created." << endl;
  if(index.size() != values.size() || index.size() < 2){    // two points being kind of necessary.. 
    initOk = false;
    return;     // am I allowed to do this??
  }
  dimNo = values[0].size();
  if(dimNo < 2){
    initOk = false;
    return;             // but the constructor doesn't return or does it?? hmm, we'll find out.. 
  }
  for(int i=0; i < values.size(); i++){
    if(values[i].size() != dimNo){
      initOk = false;
      return;
    }
  }
  // initialise the points..
  smap.resize(nodeNo);
  for(int i=0; i < nodeNo; i++){
    smap[i].resize(dimNo);
  }
  // and call initMap.. 
  
  initMap();
}

SomProcess::~SomProcess(){
  cout << "deleting a som process " << endl;
}

float SomProcess::sqEuclidean(vector<float>& v1, vector<float>& v2){
  if(v1.size() != v2.size()){
    // technically we could go through the number of dimensions they have in common 
    // and come up with some sort of number,, though that might not make an awful lot 
    // of sense..
    return(-1);
  }
  float d = 0;
  for(int i=0; i < v1.size(); i++){
    d += ((v1[i] - v2[i]) * (v1[i] - v2[i]));
  }
  return(d);
}

void SomProcess::setFactors(float a, float s){
  alpha = a;
  sigmaMultiplier = s;
  cout << "somprocess set alpha to : " << alpha << "  and sigmaMultiplier to : " << sigmaMultiplier << endl;
}

void SomProcess::initMap(){
  // called after the dimensions have been created and stuff. 
  // could move some of the stuff from the constructor down here.. and return a boolean or something.
  
  // Have to do several things.. 
  // 1. determine in which plane to initialise the map,, -
  //    which we will do by determining which two dimensions have the 
  //    largest standard deviation.. -then we will put a 2 D map in the plane of these two 
  //    dimensions.. at the mean level of all of the other dimensions... - or something like that
  //    --- there are probable better ways of doing this,, but I'm not sure how we define something as 
  //   being a hyperplane otherwise.. 

  /// --- hmm ,we could vary the points in the other planes,, -if they vary continuously,  by a fixed factor, then
  //      we still have a plane,, -- i.e. the distances between points are all the same, and it's a flat plane of sorts..
  //      -- I think it probably doesn't matter too much 
  
  // -- work out the variance and mean for each dimension..
  vector<float> variances(dimNo, 0);
  vector<float> means(dimNo, 0);
  // may not be the fastest, but first work out the means, and then do the variances..
  for(int i=0; i < values.size(); i++){
    for(int j=0; j < values[i].size(); j++){
      means[j] += values[i][j];        // faster to do it here (and hence more times or to do an extra loop?? 
    }
  }
  // loop and divide the numbers by the means..
  for(int i=0; i < means.size(); i++){
    means[i] = means[i] / values.size();
  }
  // and then work out the variances..
  for(int i=0; i < values.size(); i++){
    for(int j=0; j < values[i].size(); j++){
      variances[j] += ((values[i][j] - means[j]) * (values[i][j] - means[j]));
    }
  }
  /// -- divide by n-1,, - not really necessary, but there you go..
  for(int i=0; i < variances.size(); i++){
    variances[i] = variances[i] / (values.size()-1);
  }
  // -- and then we can use the value of the variation to determine the size of our map..
  int no1, no2;    // the highest and the lowest index.. 
  no1 = no2 = 0;
  for(int i=0; i < variances.size(); i++){
    // hmm,, how to do this..
    if(variances[no1] < variances[i]){
      no2 = no1;
      no1 = i;
    }
  }
  /// note the above doesn't work if the first index is the highest values.. hmm. --- but we can't know that before hand..
  /// -hmm -- if that happens, than no1 and no2 will both be 0, not so good.. hmm, --- hmm, I feel braindead today, I'm sure there
  /// is a better way of doing this, but it will take me longer to think of it than to just write the ugly code.. forgive me..
  if(no1 == 0){
    no2 = 1;
    for(int i=1; i < variances.size(); i++){
      if(variances[no2] < variances[i]){
	no2 = i;
      }
    }
  }
  mc1 = no1;
  mc2 = no2;
  ///////// UGLY, AS HELL, BUT THERE YOU GO.. 
  // ok let's make a map then.. 
  float x, y;      // the two critical components.. that are varied..
  float xStd = sqrt(variances[no1]);
  float yStd = sqrt(variances[no2]);
  float xMean = means[no1];
  float yMean = means[no2];    // not necessary, but it's easier to read.
  // also set the learning parameter here..
  sigma = sigmaMultiplier * variances[no1];
  //  alpha = 0.5;        // just a guess.. as to what might be good.. 
  cout << "setting sigma to : " << sigma << "  and alpha to : " << alpha << endl;
  // make the map extend to 3 standard deviations from the mean.. (i.e. either side)
  for(int i=0; i < nodeSqrt; i++){
    x = xMean +   ( (float)(i - (nodeSqrt/2)) * 6 * xStd)/nodeSqrt;
    for(int j=0; j < nodeSqrt; j++){
      y = yMean +   ( (float)(j - (nodeSqrt/2)) * 6 * yStd)/nodeSqrt;
      for(int k=0; k < dimNo; k++){
	smap[i * nodeSqrt + j][k] = means[k];
      }
      // and override the two most variable regions.. or something like that.. 
      smap[i * nodeSqrt + j][no1] = x;
      smap[i * nodeSqrt + j][no2] = y;
    }
  }
  // so this is a very simple hyperplane.. -- just lying in the middle of everthing 
  // actually it's not quite proper, as it's offset to the left by a little bit, but it's not by a huge amount..
  // so maybe we can think about that later on at some point.. 
}

void SomProcess::run(){
  // now we actually have to do something new here.. olaaa laaa... I say, I say.. funky stuff, eh.. 
  
  // for each value.. find the closest map point, move the map point towards that value, using one equation,
  // then move all the other map points using a different equation... -- this is the usual description of the algorithm.
  // -- however, I think that we can reverse it.. by..
  //    ----- NOONONONON CAN'T do that because the neighbourhood function is defined on the basis of the distance of the
  //                            the map nodes to the winning node, not to the input node.. oh, well, what can you do.. 
 
  if(!initOk){
    cerr << "SomProcess not initialised correctly, forget this man" << endl;
    return;
  }

  // let's go!!
  bool keepGoing = true;
  int generation = 0;
  float sigmaModifier = 0.98;     // how much to decrease by
  float alphaModifier = 0.97;     // as above.. simple formula... 
  vector<int> membership(values.size(), -1);         // which node does the input vector belong to..  
  
  while(keepGoing){
    // note at this point the map is already initialised, and for the sake of completeness we should show the initial map to
    // the parent process..
    mapDescription mapd(smap, mc1, mc2, nodeSqrt, nodeSqrt, values, index, membership);
    //cout << "membership size is : " << membership.size() << " and mapd membership size is : " << mapd.membership.size() << endl;
    mutex->lock();
    parentMaps->push_back(mapd);
    mutex->unlock();
    float distanceMoved = 0;
    int winner;    // the index of the closest point.. 
    float winningDistance;
    int changeCount = 0;
    float bestDistance = 0;
    for(int i=0; i < values.size(); i++){
      // first set the closestPoint and the smallest Distance..
      winningDistance = sqEuclidean(values[i], smap[0]);    // since we are just comparing we don't need to square root.... 
      winner = 0;
      for(int j=0; j < smap.size(); j++){
	float distance = sqEuclidean(values[i], smap[j]);
	if(distance < winningDistance){
	  winningDistance = distance;
	  winner = j;
	}
      }
      if(membership[i] != winner){
	membership[i] = winner;
	changeCount++;
      }
      bestDistance += winningDistance;
      // move the winning node towards the input vector.. 
      for(int j=0; j < smap[winner].size(); j++){
	smap[winner][j] = smap[winner][j] + (alpha * (values[i][j] - smap[winner][j]));
      }
      // and now we have to go through all the points and move them
      for(int j=0; j < smap.size(); j++){
	if(j != winner){
	  distanceMoved += moveNode(smap[j], smap[winner], values[i]);
	}
      }
    }
    ///do stuff to let the world know something has happened..
    cout << "Completed Generation no: " << generation << endl
	 << "total distance moved by nodes : " << distanceMoved << endl
	 << "The change count is now : " << changeCount << endl
	 << "And the best distance is now : " << bestDistance << endl;
    generation++;
    sigma = sigma * sigmaModifier;
    alpha = alpha * alphaModifier;
    cout << "sigma is now : " << sigma << endl << "alpha is now: " << alpha << endl;
    // let's have get out clause,, no more than a hundred generations to begin with..
    if(generation > 50){
      keepGoing = false;
    }
  }
  cout << "end of the self organising mapping.. how to deal with issues, that is what I don't really know" << endl;
  for(int i=0; i < membership.size(); i++){
    int x = membership[i] / nodeSqrt;
    int y = membership[i] % nodeSqrt;
    cout << "i : " << i << "\texpt: " << index[i] << "\tx : " << x << "\ty: " << y << endl;
  }
}
  

float SomProcess::moveNode(vector<float>& node, vector<float>& winner, vector<float>& target){
  if(node.size() != target.size()){
    cerr << "SomProcess::moveNode node and target have different dimensions, by by. " << endl;
    exit();
  }
  // calculate the neigbouhood function
  float d = sqEuclidean(node, winner);
  float nf = exp(-d/sigma);
  //cout << "moveNode : distance " << d << "   neighbourhood : " << nf << endl; 
  // and modifiy..
  float delta = 0;     // calculate the total distance moved.. 
  for(int i=0; i < node.size(); i++){
    float d = (alpha * nf * (target[i] - node[i]));
    //cout << "  " << target[i] << ":" << node[i];
    delta += (d * d);
    node[i] =   node[i] + d;    // that's really simple in fact.. funny that..
  }
  //cout << endl;
  return(sqrt(delta));
}
