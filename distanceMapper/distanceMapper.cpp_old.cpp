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
#include <values.h>
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
}

dpoint::dpoint(int i, int dimensions){
  index = i;
  stress = 0;
  coordinates.resize(dimensions);
  forceVectors.resize(dimensions);
  for(int i=0; i < dimensions; i++){
    coordinates[i] = 0;
    forceVectors[i] = 0;
  }
}

float dpoint::adjustVectors(dpoint& p, float d){
  //cout << endl << "adjustVectors : " << endl;
  
  if(coordinates.size() != p.coordinates.size()){
    cerr << "point coordinate mismatch size thingy : " << endl;
    return(stress);
  }
  vector<float> coordDists(coordinates.size());
  float D = 0;                 // the actual euclidean distance.. 
  for(int i=0; i < coordinates.size(); i++){
    //cout << "\t" << i << " this : " << coordinates[i] << "  that: " << p.coordinates[i] << endl;
    coordDists[i] = p.coordinates[i] - coordinates[i];
    if(coordDists[i] == 0){                // disaster.. 
      coordDists[i] = MINFLOAT;                   // it's cheating, but it will cause some sort of movement away from each other (or towards.)
      // maybe we don't have to initialise anything then.. we can just cheat. and set everything
      // to 0 in the beginning.. -- giving us reproducible results as well. hmm 
      cout << "cheating, setting coorDists to  " << MINFLOAT <<  "  for : " << i << "   for point : " << index << "  compared to : " << p.index << endl;
    }
    D += (coordDists[i] * coordDists[i]);
    // I know that the above looks really very clumsy, but it is in fact much faster than using the pow function
  }
  D = sqrt(D);   // alleluliahh
  //  float delta = (D - d);   // i.e. delta is positive if we want to move towards point p by some measure.. 
  // how about ...
  float delta = (D- d) * fabs(log(D/d));    // scale by the fold difference.. 

  //cout << "Distance is : " << D << "   d : " << d << "   delta : " << delta << endl;
                                  // adjust the force vectors by some measure
  stress += fabs(delta);     // the absolute amount of stress on the point.. 
  //stress += (delta*delta);     // the absolute amount of stress on the point.. 
  //// multiply each coordinate distance by (delta / D) and add to the force Vectors..
  /// 
  ///  if delta is negative (the points are too close to each other,, then the coordinate distances will be multiplied by 
  ///  a negative number.. and will tend to increase the distance between the two points.. I think.. hmm.
  //cout << "Force Vectors increment  : " << endl;
  vector<float> compForces(coordinates.size());
  for(int i=0; i < coordinates.size(); i++){
    //if(delta > 0){
    if(fabs(delta) > MINFLOAT){
      forceVectors[i] += (delta * coordDists[i])/D;
      compForces[i] = (delta * coordDists[i])/D;   // Squaring seems to get us out of normal range of values.. bugger.  
    }else{
      cout << "delta is : " << delta << "   D is " << D << "  and d is " << d << endl;
      compForces[i] = 0;
    }
    //forceVectors[i] += (delta * delta * coordDists[i])/D;
    //compForces[i] = (delta * delta * coordDists[i])/D;
    // big problems are bigger than small problems.. 
      //}else{
      //forceVectors[i] += (delta * coordDists[i])/(D*2);        // repulsive forces count for less at longer further distances. 
      //compForces[i] = (delta * coordDists[i])/(D*2);
      //}
    //cout << "\t" << i << " : " << (delta * coordDists[i])/D;
  }
  components.push_back(componentVector(compForces, (delta > 0)));
  //  cout << endl << endl;
  return(stress);     // not really useful as it's an intermediate value.. 
}

float dpoint::move(float forceMultiplier){
  float distanceMoved = 0;
  for(int i=0; i < coordinates.size(); i++){
    coordinates[i] += forceMultiplier * forceVectors[i];
    distanceMoved += ((forceMultiplier * forceVectors[i]) * (forceMultiplier * forceVectors[i]));
  }
  return(sqrt(distanceMoved));     // i.e. the euclidean distance.. 
}

void dpoint::resetForces(){
  stress = 0;
  for(int i=0; i < forceVectors.size(); i++){
    forceVectors[i] = 0;
  }
  for(int i=0; i < components.size(); i++){
    components[i].forces.resize(0);  // ??
  }
  components.resize(0);
}

DistanceMapper::DistanceMapper(vector<int> expI, vector<vector<float> > d, int dims,  QMutex* mutx, vector<vector<dpoint> >* prntPoints, QObject* prnt){
  calculating = false;
  expts = expI;
  distances = d;
  dimensionality = dims;
  parent = prnt;
  parentPoints = prntPoints;
  pointMutex = mutx;

  srand(time(0));
  //moveFactor = 0.005;      // just keep it below 1,, above one will lead to strange behaviour.. 
  moveFactor = 0.01;      // just keep it below 1,, above one will lead to strange behaviour.. 
  // make sure that the distances are all the same.. 
  if(expts.size() != distances.size()){
    cout << "bugger everytyhing is going to die man.. " << endl;
    cout << "distances size is : " << distances.size() << endl
	 << "experiments size is : " << expts.size() << endl;
  }
  // count the total distance in the thingy.. as counted by the program..
  totalDistance = 0;
  for(int i=0; i < distances.size(); i++){
    for(int j=0; j < distances[i].size(); j++){
      totalDistance += distances[i][j];
    }
  }
  
  points.reserve(expts.size());
  for(int i=0; i < expts.size(); i++){
    points.push_back(dpoint(expts[i], dimensionality));    // 2 dimensional coordinates.. i.e. map to a flat surface .. 
  }
  initialisePoints();
  // these are all now initialised to the origin which is sort of alright.. I suppose... hooo yeahh.
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

void DistanceMapper::updatePosition(int i, float x, float y){
  wait();
  //  calculating = true;  -- Not sure about this change.. 
  if(points.size() > i){
    if(points[i].coordinates.size() > 1){
      points[i].coordinates[0] = x;
      points[i].coordinates[1] = y;
    }
  }
  adjustVectors();
  pointMutex->lock();
  parentPoints->push_back(points);     // this should cause a copy if I understand correctly..
  pointMutex->unlock();
  //run();
}

float DistanceMapper::adjustVectors(){
  float totalStress = 0;
  for(int i=0; i < expts.size(); i++){
    float stress;
    for(int j=0; j < expts.size(); j++){      // crash if the dimensions of the distances are not good.. what the hell though..
      if(i != j){
	stress = points[i].adjustVectors(points[j], distances[i][j]);
      }
    }
    totalStress += stress;
  }
  return totalStress;
}

float DistanceMapper::movePoints(){
  float movedDistance = 0;
  for(int i=0; i < points.size(); i++){
    movedDistance += points[i].move(moveFactor);
  }
  return(movedDistance);
}

void DistanceMapper::resetPoints(){
  for(int i=0; i < points.size(); i++){
    points[i].resetForces();
  }
}

void DistanceMapper::run(){
  calculating = true;
  int generationCounter = 0;
  bool keepOnGoing = true;
  while(keepOnGoing){     // initially just keep running.. until we add some control factors into the situation..
    float stress = adjustVectors();
    // update the parent..
    pointMutex->lock();
    parentPoints->push_back(points);     // this should cause a copy if I understand correctly..
    pointMutex->unlock();
    // QCustomEvent* event = new QCustomEvent(25341);
    //qApp->postEvent(parent, event);    // let the viewer check periodically instead.. 
    /// and let's gene
    float distanceMoved = movePoints();
    resetPoints();
    cout << "DistanceMapper generation : " << generationCounter++ << " stress : " << stress << "  moved : " << distanceMoved
	 << "   total distance : " << totalDistance << endl;
    if(generationCounter > 500){ keepOnGoing = false; }
    //   if(distanceMoved < 0.1 || generationCounter > 200){ keepOnGoing = false; }
  }
  calculating = false; 
}

void DistanceMapper::initialisePoints(){
  calculating = true;
  for(int i=0; i < points.size(); i++){
    for(int j=0; j < points[i].coordinates.size(); j++){
      points[i].coordinates[j] = float(100) * rand()/RAND_MAX;
    }
  }
} 


    
