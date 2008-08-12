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

#ifndef MEANPLOTWINDOW_H
#define MEANPLOTWINDOW_H

#include "../exptinfo.h"
#include "../probeSetWidget/probeSetWidget.h"
#include "meanExpressionPlotter.h"
#include "keyWindow.h"
#include <qwidget.h>
#include <set>
#include <map>

using namespace std;

class MeanPlotWindow : public QWidget
{
  Q_OBJECT
    public:
  MeanPlotWindow(vector<int>* ve, map<float, exptInfo>* em, QWidget* parent=0, const char* name=0);

  public slots:
    void toggleProbeSet(ProbeSetWidget*, bool);
  void addProbeSet(ProbeSetWidget*);
  void removeProbeSet(ProbeSetWidget*);
  void setPenWidth(int);
  void collectData();      // collect and collate the appropriate data followed by drawstuff. 
  void dontUpdate(bool synced);

  private slots:
      void setFonts(QFont f);
  
  private :
    MeanExpressionPlotter* raw;
  MeanExpressionPlotter* norm;
  KeyWindow* keyWindow;
  set<ProbeSetWidget*> pSets;
  /// and some even funkier datastructures
  //map<ProbeSetWidget*, vector<float> > normalisedMeans;
  vector< vector<float> > rawMeans;
  vector< vector<float> > normMeans;
  vector< vector<int> > exptIndices;
  vector<QString> keys;
  map<float, exptInfo>* eMap;        // the map of experiments and their indices and all sorts.. 
  bool dontUpdatePlot; 

  void drawStuff();
  vector<int>* exptSelection;          // so we know for which ones we need to be doing the selection.. 
  /// so we don't have to recalculate this one each time. as that would be silly
  // just try to make sure that we don't leave dangling pointers.. 
};

#endif
