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

#ifndef SOMDRAWER_H
#define SOMDRAWER_H

#include <qwidget.h>
#include <vector>
#include <qcolor.h>
#include <qpen.h>
#include "somProcess.h"       // for the mapDescription.. 
#include "../statPlotter/distChooser.h"

using namespace std;

//    a class that draws a som, and it's nodes based on very generic descriptions of these
//    doesn't rely on special structs, but just takes a series of vector data .. ..

class SomDrawer : public QWidget
{
  Q_OBJECT
    public :
    SomDrawer(QWidget* parent=0, const char* name=0);
  ~SomDrawer();
  
  void setData(mapDescription md);
  void setMaxDev(float f);
  
  private :
    void paintEvent(QPaintEvent* e);
  float euclidean(vector<float>& v1, vector<float>& v2);
  QPen color(float d);   // return a color component based on the d value.. (int between 0 - 255);

  // and something for the data to be stored in..
  mapDescription smap;
  vector<float> fwd;
  vector<float> down;   // forward and down distances, calculate so we can display some measure of distance 
                        // between points.. 

  // values used to determine drawing..
  DistChooser* distDrawer;       // so I can see the distribution of the euclidean distances.. 
  float maxX, minX, maxY, minY;
  float maxEuclidean, minEuclidean;
  float euclidMean, euclidStd;            // mean and std deviation of the euclid scores.. 
  int nodeDiameter;                       // how big to draw nodes of the map.
  int pointDiameter;
  int margin;                         // we want some space around.. -- have same x and y margin.
  float maxDev;                       // number of maximum deviations for the colour range.. 
};

#endif
