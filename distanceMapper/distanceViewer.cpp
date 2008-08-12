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

#include "distanceViewer.h"
#include "distanceMapper.h"
#include "pointDrawer.h"
#include "tdPointDrawer.h"
//#include "../som/somDrawer.h"
#include "../customWidgets/fSpinBox.h"
#include <qwidget.h>
#include <qlayout.h>
#include <vector>
#include <qmutex.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <iostream>

using namespace std;

DistanceViewer::DistanceViewer(vector<int> expI, vector<vector<float> > d, QString cName, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  cout << "Distance Viewer constructor " << endl;
  captionName = cName;  // for offspring windows.. maybe.. mainly.. 
  setCaption(cName);
  distances = d;
  experiments = expI;
  
  cout << "reserving points  " << endl;
  points.reserve(5000);
  cout << "points reservedd " << endl;

  drawer = new PointDrawer();
  cout << "made point drawer " << endl;
  drawer->setCaption(captionName);
  drawer->resize(500, 500);
  drawer->show();
  connect(drawer, SIGNAL(updatePosition(int, float, float)), this, SLOT(updatePosition(int, float, float)) );    // could be direct, but I want to be careful
  connect(drawer, SIGNAL(compareCells(vector<int>, vector<int>)), this, SIGNAL(compareCells(vector<int>, vector<int>)) );
  connect(drawer, SIGNAL(setCoordinates()), this, SLOT(setcoords()) );


  //tdDrawer = new TDPointDrawer(this, "tdDrawer");
  cout << "making mapper " << endl;
  mapper = new DistanceMapper(experiments, distances, 2, &pointMutex, &points, (QObject*)this);    // mapper updates points, then 
  cout << "mapper made " << endl;
  //som = new SomProcess(experiments, distances, &somMutex, &soms);
  //somDrawer = new SomDrawer();     // make it toplevel..
  //somDrawer->setCaption(captionName);
  //cout << "somDrawer made up " << endl;
  //somDrawer->resize(500, 500);
  //cout << "somDrawer resized " << endl;
  //somDrawer->show();
  //cout << "showing somdrawer " << endl;
  // and a timeer..
  
  
  frameTimer = new QTimer(this, "frameTimer");
  connect(frameTimer, SIGNAL(timeout()), this, SLOT(updateFrame()) );
  
  cout << "frame timer made " << endl;

  watchTimer = new QTimer(this, "watchTimer");
  connect(watchTimer, SIGNAL(timeout()), this, SLOT(updatePoints()) );

  cout << "watchTimer made " << endl;
  //somTimer = new QTimer(this, "somTimer");
  //connect(somTimer, SIGNAL(timeout()), this, SLOT(updateSom()) );

  QLabel* sodLabel = new QLabel("Self Organising Deltoids", this, "sodLabel");
  //QLabel* somLabel = new QLabel("Self Organising Map", this, "somLabel");

  QPushButton* replayButton = new QPushButton("Replay", this, "replayButton");
  connect(replayButton, SIGNAL(clicked()), this, SLOT(replay()) );
  cout << "replay button made " << endl;

  QPushButton* continueButton = new QPushButton("Continue", this, "continueButton");
  connect(continueButton, SIGNAL(clicked()), this, SLOT(continueMapping()) );   // is this legal ?? 

  QPushButton* restartButton = new QPushButton("Restart", this, "restartButton");
  connect(restartButton, SIGNAL(clicked()), this, SLOT(restart()) );   // is this legal ??   

  //QPushButton* somReplay = new QPushButton("Replay Map", this, "somReplay");
  //connect(somReplay, SIGNAL(clicked()), this, SLOT(replayMap()) );

  //QPushButton* somRestart = new QPushButton("Restart SOM", this, "somRestart");
  //connect(somRestart, SIGNAL(clicked()), this, SLOT(restartSom()) );

  //somAlpha = new FSpinBox(0.001, 0.999, 100, this, "somAlpha");
  //somSigma = new FSpinBox(0.1, 10, 100, this, "somSigma");    // not the acutal sigma value, but a sigma multiplier.. 
  //maxDev = new FSpinBox(0, 10, 100, this, "maxDev");
  //maxDev->setFValue(2.5);

  //somAlpha->setFValue(0.5);
  //somSigma->setFValue(2.5);
  
  QPushButton* startButton = new QPushButton("start", this, "startButton");
  connect(startButton, SIGNAL(clicked()), this, SLOT(start()) );

  QPushButton* deleteButton =  new QPushButton("Delete", this, "deleteButton");
  connect(deleteButton, SIGNAL(clicked()), this, SIGNAL(deleteMe()) );

  cout << "start button made, setting up the layout " << endl;
  // set up the layout..
  QVBoxLayout* vbox = new QVBoxLayout(this);
  //  vbox->addWidget(drawer);    // and more later on..
  //vbox->addWidget(tdDrawer);    // and more later on..
  QHBoxLayout* sodButtons = new QHBoxLayout();
  vbox->addWidget(sodLabel);
  vbox->addLayout(sodButtons);
  sodButtons->addWidget(replayButton);
  sodButtons->addWidget(continueButton);
  sodButtons->addWidget(restartButton);
  //vbox->addWidget(somLabel);
  //QHBoxLayout* somButtons = new QHBoxLayout();
  //vbox->addLayout(somButtons);
  //somButtons->addWidget(somReplay);
  //somButtons->addWidget(somRestart);
  //somButtons->addWidget(somAlpha);
  //somButtons->addWidget(somSigma);
  //somButtons->addWidget(maxDev);

  cout << "sod buttons done going for the bottom row " << endl;
  QHBoxLayout* bottomRow = new QHBoxLayout();
  vbox->addLayout(bottomRow);
  bottomRow->addWidget(deleteButton);
  bottomRow->addStretch();
  bottomRow->addWidget(startButton);
  cout << "and the start Button added, must be the drawing of these that causes the problem " << endl;
  //  resize(500, 400);
  
  cout << "And what's now, constructor finished " << endl;
  // and start the mapper.. -- it's probably too fast, but there you go.. we could use a timer to do updates,, but hmm
}

DistanceViewer::~DistanceViewer(){
  cout << "destroying distance viewer .. hahhaha" << endl;
  //som->wait();
  mapper->wait();
  //somDrawer->hide();
  //delete somDrawer;


  drawer->hide();
  delete drawer;

  delete mapper;    // main things to delete.. 
  
  // and delete the data that we are not going to be using anymore.. 
  for(int i=0; i < points.size(); i++){
    cout << "deleting from : " << i << endl;
    cout << "points " << i << "   size is " << points[i].size() << endl;
    for(int j=0; j < points[i].size(); j++){
      delete points[i][j];
    }
  }
}

void DistanceViewer::start(){
  cout << "start called " << endl;
  followFrame = 0;
  //somFrame = 0; 
  watchTimer->start(20);  // 33 fps.. 
  mapper->start();   // for now, let's just run one of these babies.. !!
  //somDrawer->setMaxDev(maxDev->fvalue());
  //som->setFactors(somAlpha->fvalue(), somSigma->fvalue());
  //som->start();
  //somTimer->start(400);
}

void DistanceViewer::restart(){
  mapper->wait();
  // we have to delete the old points, this maybe dangerous as if the drawer were to try to draw something at this point we
  // will get a segmentation fault as the two vectors share the pointers. Need to do something about that.. perhaps..
  drawer->emptyData();           // so the drawer doesn't try to draw any of these points after they've been deleted..
  for(int i=0; i < points.size(); i++){
    cout << "deleting from : " << i << endl;
    for(int j=0; j < points[i].size(); j++){
      delete points[i][j];
    }
  }
  points.resize(0);    // forget the old points, we could store them somehwere, but maybe not.. 
  mapper->initialisePoints();
  followFrame = 0;
  mapper->start();
  watchTimer->start(20);
}

void DistanceViewer::continueMapping(){
  mapper->wait();
  mapper->start();  // is that possible ??
  watchTimer->start(20);
}

// void DistanceViewer::restartSom(){
//   som->wait();          // BAD, as this might take some time, nevertheless, it is probably safter..
//   soms.resize(0);
//   somFrame = 0;
//   somDrawer->setMaxDev(maxDev->fvalue());
//   som->setFactors(somAlpha->fvalue(), somSigma->fvalue());
//   som->initMap();
//   som->start();
//   somTimer->start(400);
// }

void DistanceViewer::customEvent(QCustomEvent* e){
  if(e->type() == 25341){
    updatePoints();
  }else{
    cout << "unknown QCustomEvent type : " << e->type() << endl;
  }
}

void DistanceViewer::updatePoints(){
  // we assume that by this time the points vector has been appended by the thingy..
  // so all we do is read off the last member of it..
  pointMutex.lock();    // hmm   -- stops the other thread if in the middle of this .. 
  if(!mapper->calculating && followFrame >= points.size()){
    cout << "stopping the timer.. " << endl;
    watchTimer->stop();
    pointMutex.unlock();
    return;
  }
  cout << "calling setData followFrame is : " << followFrame << "  and points size is : " << points.size() << endl;
  if(points.size() <= followFrame){
    cerr << "think I should be updating points, but points size is 0,, oh well" << endl;
    pointMutex.unlock();
    return;
  }
  vector<dpoint*> localPoints = points[followFrame];
  followFrame++;
  pointMutex.unlock();   // slows down calculation in favour of drawing.. ?? good or bad, I'm not sure.. 

//   for(int i=0; i < localPoints.size(); i++){
//     cout << "Viewer : point " << i << "  index " << localPoints[i]->index << "  stress : " << localPoints[i]->stress << "  x: " << localPoints[i]->coordinates[0] << "  y : " << localPoints[i]->coordinates[1] << endl;
//   }
  drawer->setData(localPoints);


  //tdDrawer->setModel(localPoints);
  /// this is terribly inefficient as it involves lots of copying of vectors, but I have a feeling that 
  /// it will be fast enough anyway.. 
}

// void DistanceViewer::updateSom(){
//   somMutex.lock();
//   if(som->finished() && somFrame >= soms.size()){
//     cout << "stopping the some timer " << endl;
//     somTimer->stop();
//     somMutex.unlock();
//     return;
//   }
//   if(somFrame >= soms.size()){
//     cout << "somFrame is larger than or equal to soms.. : " << somFrame << endl;
//     somMutex.unlock();
//     return;
//   }
//   mapDescription ld = soms[somFrame];
//   somFrame++;
//   somMutex.unlock();
//   //  cout << "ld membership size is : " << ld.membership.size() << endl;
//   somDrawer->setData(ld);     // this will involve lots of copying, but never mind.. 
// }

void DistanceViewer::replay(){
  cout << "replay function " << endl;
  frame = 0;
  //somDrawer->setMaxDev(maxDev->fvalue());
  frameTimer->start(10);    // 25 fps.. as not so many frames..
}

// void DistanceViewer::replayMap(){
//   somFrame = 0;
//   somTimer->start(250);   // 4 frames per second..
// }

void DistanceViewer::updateFrame(){
  cout << "calling updateFrame frame is : " << frame << endl;
  if(frame >= points.size()){
    frameTimer->stop();
    return;
  }
  pointMutex.lock();
  vector<dpoint*> localPoints = points[frame];
  pointMutex.unlock();

  drawer->setData(localPoints);


  //tdDrawer->setModel(localPoints);
  frame++;
}

void DistanceViewer::updatePosition(int i, float x, float y){
  mapper->updatePosition(i, x, y);
  pointMutex.lock();
  vector<dpoint*> localPoints = points.back();
  pointMutex.unlock();


  drawer->setData(localPoints);


  //  mapper->start();
  // watchTimer->start(30);
}

void DistanceViewer::setcoords(){
  cout << "Distnace viewer set coordinates .. " << endl; 
  vector<PointCoordinate> cords;   // just push it back..
  //  vector<dpoint> pts = points.last();     // just the most recent .. -- I forget the function that does this.. 
  vector<dpoint*> pts = points[points.size() -1];
  for(uint i=0; i < pts.size(); i++){        
    cords.push_back(PointCoordinate(pts[i]->index, pts[i]->coordinates, pts[i]->dimNo));
  }
  emit setCoordinates(cords);   // have these set in the clientWindow, and then have drawing things use a pointer.. to something.. 
}
