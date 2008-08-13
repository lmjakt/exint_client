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

#ifndef DISTANCEVIEWER_H
#define DISTANCEVIEWER_H

#include "distanceMapper.h"
#include "pointDrawer.h"
#include "stressPlotter.h"
#include "tdPointDrawer.h"
#include "../dataStructs.h"
//#include "../som/somProcess.h"
//#include "../som/somDrawer.h"
#include "../customWidgets/fSpinBox.h"
#include <qmutex.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qstring.h>
#include <vector>

using namespace std;

class DistanceViewer : public QWidget
{
  Q_OBJECT
    public :
    DistanceViewer(vector<int> expI, vector<vector<float> > d, QString cName, QWidget* parent=0, const char* name=0);
  ~DistanceViewer();
  

  private slots :
  void start();    // start the mapper.. 
  void replay();                           // play again.. 
  void updateFrame();                      // for replay action..
  void continueMapping();                  // continue the mapping.. -- just calls start..  
  void restart();                           // restart the thread ?? possible?? or not.. 
  void updatePosition(int i, float x, float y);   // update the position of thingy indexed by i, to x and y.. see how this goes.. 
  void updatePoints();                     // send a vector to the thingy.. 
  void setcoords();                        // work out the coordinates and send them up the food chain.. 
  //  void updateSom();        
  //void replayMap();
  //void restartSom();                       // restart the som.. 

  private :

    void customEvent(QCustomEvent* e);     // receive updated from the thread..--- no function, we can change.. later.. 


  QString captionName;                    // something for the captions.. 
  vector<vector<dpoint*> > points;          // keep points in here.. receive from the thingy. 
                                           // keep whole history in here, so that I can replay things..
  vector<dpoint*> localPoints;             // a copy of the points that is never touched by the mapper.
  vector<float> stressValues;              // to monitor the reduction in error... 
  //vector<mapDescription> soms;             // self organising maps.. of some sort.. 
  vector<vector<float> > distances;
  vector<int> experiments;                 // might as well keep a copy in here.. it's easy enough.. 
  QMutex pointMutex;                       // lock this to access the points above.. 
  QMutex somMutex;
  //SomProcess* som;                         // a self organising map.. let's see what we can do with it.. 
  //SomDrawer* somDrawer;                    // something for drawing those funky maps.. hooo hoo yeahh.
  //FSpinBox* somAlpha;
  //FSpinBox* somSigma;
  //FSpinBox* maxDev;    // maxDev for the drawing thingy.. 

  DistanceMapper* mapper;                  // runs the thread and stuff....... hoo hoo yeahh.
  PointDrawer* drawer;                     // draws the point, and so far is the only thing that gets displayed. 
  StressPlotter* stressPlotter;            // plots the reduction in stress.. 
  //  TDPointDrawer* tdDrawer;                 // 3 dimensions.. !! 

  QTimer* frameTimer;
  QTimer* watchTimer;        // check if I have any more frames.. start when starting.. 
  //QTimer* somTimer;          // check if the maps have been udated..
  int frame;    // which frame am I t0..
  int followFrame;   // I'm following frames.. 
  //int somFrame;      // which frame am I drawing.. 
  
  signals :
    void compareCells(vector<int>, vector<int>);
  void setCoordinates(vector<PointCoordinate>);
  void deleteMe();
};

#endif


