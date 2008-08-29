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

#include "distanceMapper.h"
#include <qthread.h>
#include <math.h>
#ifdef Q_OS_MACX
#include <limits.h>
#include <float.h>
#define MINFLOAT (FLT_MIN)
#else
#include <values.h>
#endif
#include <qmutex.h>
#include <qapplication.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <time.h>



using namespace std;

dpoint::dpoint(){
  index = 0;
  stress = 0;
  coordinates = new float[1];
  forceVectors = new float[1];
  dimNo = 1;
  components = new componentVector*[1];
  componentNo = 0;
  componentSize = 1;

}

dpoint::dpoint(int i, int dimensions, int compNo){
  index = i;
  stress = 0;
  dimNo = dimensions;
  coordinates = new float[dimNo];
  forceVectors = new float[dimNo];
  if(compNo < 1){ compNo = 1; }
  components = new componentVector*[compNo];     // where do I add these, I'm not sure.. we'll have to work it out later.. 
  memset((void*)components, 0, sizeof(componentVector*) * compNo); 
  componentSize = compNo;
  componentNo = 0;
  //  coordinates.resize(dimensions);
  // forceVectors.resize(dimensions);
  for(int i=0; i < dimNo; i++){
    coordinates[i] = 0;
    forceVectors[i] = 0;
  }
  
}

dpoint::~dpoint(){
  //  cout << "dpoint destructor .. " << endl;
  //cout << "\t\tdpoint destructor comp no is : " << componentNo << endl;
    for(int i=0; i < componentNo; i++){
	delete components[i];
    }
    delete []components;  // should just delete the pointers, not the thing they point to (which are other pointers.. )
    delete []forceVectors;
    delete []coordinates;
}

dpoint* dpoint::copy(bool includeComponents){
    // make a new thingy and copy it.. 
    dpoint* np = new dpoint(index, dimNo, componentSize);        // note I don't need to copy the components, as these will be evaluated.. 
    np->stress = stress;
    memcpy(np->coordinates, coordinates, sizeof(float)*dimNo);
    memcpy(np->forceVectors, forceVectors, sizeof(float)*dimNo);
    if(!includeComponents)
	return(np);
    for(int i=0; i < componentNo; ++i)
	np->components[i] = components[i]->copy();
    return(np);
}

void dpoint::assignValues(dpoint* p){
    if(dimNo > p->dimNo || componentSize > p->componentSize){
	cerr << "dpoint::assignValues unable to assign values since point sizes do not appear to match" << endl;
	cerr << "this : " << dimNo << "  " << componentSize << "  p: " << (long)p << "  "  << p->dimNo << "  " << p->componentSize << endl;
	exit(1);
//	return;
    }
    p->componentNo = componentNo;     /// possible memory leak here. But deleting the pointers would be a possible seg fault. so.. 
    memcpy(p->coordinates, coordinates, sizeof(float) * dimNo);
    memcpy(p->forceVectors, forceVectors, sizeof(float) * dimNo);
    p->stress = stress;
    for(int i=0; i < componentNo; ++i){
	p->components[i]->attractive = components[i]->attractive;
	p->components[i]->forceNo = components[i]->forceNo;   /// But this seems very inadvisable.
	memcpy(p->components[i]->forces, components[i]->forces, sizeof(float) * components[i]->forceNo);
    }
}

// Since we don't need to add any more memory, we can just decrease the number that tell us how
// many dimensions we are using. For the dpoint that is just changing the dimNo itself. For the
// componentVectors, all that is necessary is to change forceNo.
int dpoint::shrinkDimNo(uint s){
    if((uint)dimNo - s < 2){
	cerr << "Attempt to shrink dpoint dimNo to " << (uint)dimNo - s << "  foiled. " << endl;
	return(0);
    }
    dimNo = (uint)dimNo - s;
    for(int i = 0; i < componentNo; ++i){
	components[i]->forceNo = dimNo;
    }
    //cout << "shrank dpoint with address : " << (long)this << "  to " << dimNo << endl;
    return(dimNo);
}

void dpoint::addComponent(componentVector* cv){
  //void dpoint::addComponent(float* f, int n, bool a){
  //  cout << "point with address : " << (long)this << "  adding component vector component No " << componentNo << "  size : " << componentSize << endl;
    if(componentNo < componentSize){
	delete components[componentNo];  // This should be safe in C++ as long as the pointer is set to 0.
	components[componentNo] = cv;
	componentNo++;
	return;
    }
    cout << endl << "\t\t\tGROWING THE COMPONENTS FROM " << componentSize << "  to " << componentSize * 2 + 1 << endl << endl;
    componentSize = componentSize * 2 + 1;    // just in case we start with 0.
    componentVector** newComps = new componentVector*[componentSize];
    memcpy(newComps, components, sizeof(componentVector*) * componentNo);  // which should be ok..
    newComps[componentNo] = cv;
    componentNo++;
    delete []components;
    components = newComps;
}

float dpoint::adjustVectors(dpoint* p, float d, bool linear){
  if(dimNo != p->dimNo){
    cerr << "point coordinate mismatch size thingy : " << endl;
    return(stress);
  }
  // we can speed up the process by making coordDists a member, so we don't have to new it every time (bigger memory use though)
  float* coordDists = new float[dimNo];

  float D = 0;                 // the actual euclidean distance.. 
  for(int i=0; i < dimNo; i++){
    coordDists[i] = p->coordinates[i] - coordinates[i];
    if(coordDists[i] == 0){                // disaster.. 
      coordDists[i] = MINFLOAT;                   // it's cheating, but it will cause some sort of movement away from each other (or towards.)
      // maybe we don't have to initialise anything then.. we can just cheat. and set everything
      // to 0 in the beginning.. -- giving us reproducible results as well. hmm 
      cerr << "cheating, setting coorDists to  " << MINFLOAT <<  "  for : " << i << "   for point : " << index << "  compared to : " << p->index << endl;
    }
    D += (coordDists[i] * coordDists[i]);
    // I know that the above looks really very clumsy, but it is in fact much faster than using the pow function
  }
  D = sqrt(D);   // alleluliahh
  //  float delta = (D - d);   // i.e. delta is positive if we want to move towards point p by some measure.. 
  // how about ...
  float delta;
  if(linear){
    delta = (D- d); // linear  by the fold difference.. 
  }else{
    delta = (D- d) * fabs(log(D/d));    // scale by the fold difference.. 
  }

                                  // adjust the force vectors by some measure
  stress += fabs(delta);     // the absolute amount of stress on the point.. 
  float* compForces = new float[dimNo];
  for(int i=0; i < dimNo; i++){
    if(fabs(delta) > MINFLOAT){
      forceVectors[i] += (delta * coordDists[i])/D;
      compForces[i] = (delta * coordDists[i])/D;   // Squaring seems to get us out of normal range of values.. bugger.  
    }else{
      compForces[i] = 0;
    }
  }
  addComponent(new componentVector(compForces, dimNo, (delta > 0)));
  delete []coordDists;
  return(stress);     // not really useful as it's an intermediate value.. 
}

float dpoint::move(float forceMultiplier){
  float distanceMoved = 0;
  for(int i=0; i < dimNo; i++){
    coordinates[i] += forceMultiplier * forceVectors[i];
    distanceMoved += ((forceMultiplier * forceVectors[i]) * (forceMultiplier * forceVectors[i]));
  }
  return(sqrt(distanceMoved));     // i.e. the euclidean distance.. 
}

void dpoint::resetForces(){              /// THIS GIVES RISE TO A MEMORY LEAK IF NOT USED WITH EXTREME CARE AS IT LEAVES LOTS OF HANGING POINTERS
  stress = 0;
  for(int i=0; i < dimNo; i++){
    forceVectors[i] = 0;
  }
  componentNo = 0;        // that way we don't have to allocate any new memory, and we should see a large speedup..
  //  for(int i=0; i < dimNo; i++){
  //  components[i].forces.resize(0);  // ??
  //}
  //components.resize(0);
}

DistanceMapper::DistanceMapper(vector<int> expI, vector<vector<float> > d, int dims,  QMutex* mutx, vector<vector<dpoint*> >* prntPoints, QObject* prnt, vector<float>* stressLevels){
  //cout << "beginning of distance mapper constructor " << endl;
  calculating = false;
  expts = expI;
  distances = d;
  dimensionality = dims;
  currentDimNo = dims;
  parent = prnt;
  parentPoints = prntPoints;
  pointMutex = mutx;
  errors = stressLevels;

  srand(time(0));
  //moveFactor = 0.005;      // just keep it below 1,, above one will lead to strange behaviour.. 
  moveFactor = 0.005;      // just keep it below 1,, above one will lead to strange behaviour.. 
  // make sure that the distances are all the same.. 
  if(expts.size() != distances.size()){
    cerr << "bugger everytyhing is going to die man.. " << endl;
    cerr << "distances size is : " << distances.size() << endl
	 << "experiments size is : " << expts.size() << endl;
  }
  // count the total distance in the thingy.. as counted by the program..
  totalDistance = 0;
  for(uint i=0; i < distances.size(); i++){
    for(uint j=0; j < distances[i].size(); j++){
      totalDistance += distances[i][j];
    }
  }
  // though that really is overcounting by a fair stretch as we count in both directions.. 
  points.reserve(expts.size());
  for(uint i=0; i < expts.size(); i++){
    points.push_back(new dpoint(expts[i], dimensionality, expts.size()));    // 2 dimensional coordinates.. i.e. map to a flat surface .. 
  }
  initialisePoints();
  // these are all now initialised to the origin which is sort of alright.. I suppose... hooo yeahh.
  //cout << "end of distance mapper constructor " << endl;
}

DistanceMapper::~DistanceMapper(){
  cout << "destryoing the DistanceMapper object, hoho yeahh " << endl;
}

void DistanceMapper::restart(){
  wait();   // wait for the thread to finish, this could end up taking a long time..
  calculating = true;
  initialisePoints();  // ok lalal 
  //run();    // and start ..
}


//////////// This function is called from the distanceViewer 
//////////// when a point has been moved manually

void DistanceMapper::updatePosition(int i, float x, float y){
  wait();
  //  calculating = true;  -- Not sure about this change.. 
  if(points.size() > (uint)i){
    if(points[i]->dimNo > 1){
      points[i]->coordinates[0] = x;
      points[i]->coordinates[1] = y;
    }
  }
  adjustVectors();
  if(0){
      pointMutex->lock();
      parentPoints->push_back(points);     // this should cause a copy if I understand correctly..
      pointMutex->unlock();
      clonePoints();                       // necessary to make sure we have new points.. 
  }else{
      updateParentPoints();
  }
  movePoints();
  resetPoints();
  //run();
}


float DistanceMapper::adjustVectors(bool linear){
  // Looks like we could optimise this one,, in some way.. 
  float totalStress = 0;
  for(uint i=0; i < expts.size(); i++){
    float stress = 0;
    for(uint j=0; j < expts.size(); j++){      // crash if the dimensions of the distances are not good.. what the hell though..
      if(i != j){
	stress = points[i]->adjustVectors(points[j], distances[i][j], linear);
      }
    }
    totalStress += stress;
  }
  return totalStress;
}

float DistanceMapper::movePoints(){
  float movedDistance = 0;
  for(uint i=0; i < points.size(); i++){
    movedDistance += points[i]->move(moveFactor);
  }
  return(movedDistance);
}

void DistanceMapper::resetPoints(){
  for(uint i=0; i < points.size(); i++){
    points[i]->resetForces();
  }
}

void DistanceMapper::run(){
  calculating = true;
  int generationCounter = 0;

  //bool keepOnGoing = true;
  bool linear = true;
  int halfPeriod = iterationNo;
  int mappingPeriods = 1 + (currentDimNo - 2); 
  mappingPeriods = mappingPeriods < 1 ? 1 : mappingPeriods;
  
  pointMutex->lock();
  errors->resize(halfPeriod * mappingPeriods);
  errors->assign(halfPeriod * mappingPeriods, 0.0);
  pointMutex->unlock();

//  while(keepOnGoing){     // initially just keep running.. until we add some control factors into the situation..
  for(int i=0; i < mappingPeriods; ++i){
      for(int j=0; j < halfPeriod; ++j){
	  //int half = generationCounter / halfPeriod;
	  // don't swap to non linear.. 
	  //    linear = !(half % 2);
	  
	  float stress = adjustVectors(linear);
	  pointMutex->lock();
	  (*errors)[generationCounter] = stress;
	  pointMutex->unlock();
	  // update the parent..
	  if(0){
	      pointMutex->lock();
	      parentPoints->push_back(points);     // this should cause a copy if I understand correctly..
	      pointMutex->unlock();
	      clonePoints();     // need to make sure we don't touch the same vectors again.. 
	  }else{
	      updateParentPoints();
	  }
	  cout << "DistanceMapper generation : " << generationCounter++ << " stress : " << stress  // << "  moved : " << distanceMoved
	       << "   total distance : " << totalDistance << "   Linear is : " << linear << endl;
	  movePoints();
	  resetPoints();
      }
      if(currentDimNo > 2){
	  cout << "\nReducing dimensionality\n" << endl;
	  reduceDimensionality();
	  currentDimNo--;
      }
  }
  cout << "distanceMapper is done .." << endl;
  calculating = false; 
}

void DistanceMapper::reInitialise(){
    for(uint i=0; i < points.size(); ++i)
	delete points[i];
    points.resize(0);
    for(uint i=0; i < expts.size(); i++)
	points.push_back(new dpoint(expts[i], dimensionality, expts.size()));    // 2 dimensional coordinates.. i.e. map to a flat surface .. 
    initialisePoints();
    currentDimNo = dimensionality;
    cout << "DistanceMapper::reinitialised with dimNo : " << dimensionality << " and with point # " << points.size() << endl;
}

void DistanceMapper::setDim(int dim, int iter){
    if(dim < 2 || iter < 2){
	cerr << "DistanceMapper, attempt to set dim to " << dim << "  and iterNo to " << iter << "  failed" << endl;
	return;
    }
    dimensionality = dim;
    iterationNo = iter;
    reInitialise();
}

void DistanceMapper::initialisePoints(){
  calculating = true;
  for(uint i=0; i < points.size(); i++){
    for(int j=0; j < points[i]->dimNo; j++){
      points[i]->coordinates[j] = float(100) * rand()/RAND_MAX;
    }
  }
} 

void DistanceMapper::updateParentPoints(){
    // this basically sets the last parent points to something else..
    pointMutex->lock();
    if(!parentPoints->size()){
	parentPoints->push_back(points);
	clonePoints();
	pointMutex->unlock();
	return;
    }
    // Otherwise we go throught the points and assign the appropriate values
    //  cout << "Assigning to the end of parentPoints of size " << parentPoints->size() << endl;
    for(uint i=0; i < parentPoints->back().size() && i < points.size(); ++i){
//	cout << "calling assignValues " << i << endl;
	points[i]->assignValues(parentPoints->back()[i]);
    }
    pointMutex->unlock();
}
	


void DistanceMapper::clonePoints(){
  // because we are handing over control of the points to the owner, we need to have some new points to 
  // manipulate. Because we are using pointers for everything, we will need to copy the thingy.. probably the best way 
  // would be to provide a copy constructor.. -- using = ,, but hmm,, I think a copy command might be better.. 
//    cout << "clone points .. " << endl;
  for(uint i=0; i < points.size(); i++){
    points[i] = points[i]->copy();    // this doesn't copy the componentVectors, nor the stress, but should be enough.. Note this is could easily lead to memory leaks..

  }
}

void DistanceMapper::reduceDimensionality(){
//    cout << "My Points : " << endl;
    for(uint i=0; i < points.size(); ++i){
	int dno = points[i]->shrinkDimNo(1);
	//cout << "my point now : " << dno << " dims" << endl;
    }
    //   cout << "And the parent points size is " << parentPoints->size() << endl;
    pointMutex->lock();
    for(uint i=0; i < parentPoints->back().size(); ++i){
	int dno = parentPoints->back()[i]->shrinkDimNo(1);
	//cout << "parent point now : " << (long)parentPoints->back()[i] << "  " << dno << " dims" << endl;
    }
    pointMutex->unlock();
}
