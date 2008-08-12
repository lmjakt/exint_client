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

#ifndef STATWINDOW_H
#define STATWINDOW_H

#include "statWidget.h"
#include "distChooser.h"
#include "twoDimDistLauncher.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include "../probe_set/probe_set.h"    // for the vitalStatistics struct -- bloody hell this is really not the place eh.. 

using namespace std;

class StatWindow : public QWidget
{
  Q_OBJECT

    public:
  StatWindow(vitalStatistics* s, QWidget* parent=0, const char* name = 0);
  ~StatWindow();
  
  public slots:
    void newStats();       // reread data from stats and replot.. maybe.. !!

  private slots :
    void newT(float min, float max);
  void changeAxis(int);    // signal emitted from the thingy.. 
  void setLogAxis(bool b);
  void loadGenes();
  void readStatsFromFile();
  //void newYT(float min, float max);
  //void newZT(float min, float max);
    //void plotStats();          // package the data and send to 
  // work out how to do the thresholds later on !!
  private :
    vitalStatistics* stats;
  plotData data;       // for passing on to the statWidget.h
  StatWidget* statwidget;   // the open GL plotting..
  TwoDimDistLauncher* twoDimLauncher;
  QHBoxLayout* hbox;
  int activeNo;   // number of active probe sets.. 
  QLabel* activeLabel; // display activeNo.. 
  vector<DistChooser*> dists;
  map<DistChooser*, int> distMap;       // the ints refer to the data vector in the stats structure.. hmm 
  map<int, vector<float>*> axisMap;     // ints are 1, 2 or 3 for X, Y and Z axises.. hmmm. this is looking dangerous.  second int's refer to the indices..
  map<int, int> distAxisMap;            // in dists,, , 1, 2 or 3.   
  vector<float> emptyData;   // for thingy.. it's spiffy. 
  vector<bool> active;       // is the point active or not.. ? 
  vector< set<DistChooser*> > activityVotes;  // which determines the value in the active boolean..
  //  DistChooser* dist1;
  //DistChooser* dist2;
  //DistChooser* dist3;         // for the various things.. 
  signals :
    void setIndex(vector<int>, QString);
};

#endif
