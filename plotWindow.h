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

#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include "expressionplotter.h"
#include "2dplot/2dplot.h"
#include "dataStructs.h"     // for PointCoordinates  (so that I can get the coordinates from somewhere.. ).. 
#include "probe_set/probe_set.h"
#include "glPlotter/glPlotter.h"
#include "stat/pointPlotter.h"
#include "exportWindow/exportWidget.h"  
#include "exportWindow/psDialog.h"
#include <qpainter.h>
#include <qwidget.h>
#include <qslider.h>
#include <qfiledialog.h>
#include <vector>

using namespace std;

class PlotWindow : public QWidget
{
  Q_OBJECT
    public:
  PlotWindow(vector<int>* es, vector<int>* me, int id, QWidget* parent=0, const char* name=0);
  bool isActive();
  
  public slots:
  void plot(probe_set* pset);   // pass as pointer rather than reference so I don't forget!
  void plot(probe_set* pset, QPainter* p, bool rawData);   // for plotting somewhere else.. 
  void plot();
  void inspectProbeData(probe_data p_data);
  void setPenWidth(int);     // just for passing the message on.. 
  void replot();
  void setCoordinates(vector<PointCoordinate> pts);
  void setActiveWindow(bool b);

  private slots :
    void toggleSurfacePlot();
  void setProbePairThreshold(float t);        // tells the plotters what threshold to use for the probe pairs.
  void setFonts(QFont f);
  void showExportDialog(ExpressionPlotter::PlotType type);
  void exportPS(QString fname, ExpressionPlotter::PlotType type, int, int);
  void clonePlot();  

 private:
  bool hasData;
  bool active;
  int plotId;
  ExpressionPlotter* raw;
//  ExpressionPlotter* norm;
  ExpressionPlotter* model;     // well sort of my model based one.. 
  TwoDPlot* posPlot;            // plots expression data with cell types grouped in a 2 dimensional grid..
//  GLPlotter* glRaw;             // just checking how this goes.. !
  PointPlotter* pointPlotter;   // for plotting one dimensional numbers.. 
  vector<int>* exptSelection;   // vector defined and manipulated elsewhere, be careful!!
  vector<int>* markedExperiments;   // vector defined and manipulated elsewhere, be careful!!
  probe_set p;   // dangerous. hmm. -- I guess less dangerous.. 
  probe_set plotted_pset;  // the one that we've actually plotted
  probe_data pd; // tries to work out if we are in sync on the gene level, and so on.. 
  float probePairThreshold;

  PSDialog* exportDialog;

  signals :
      void showExperimentDetails(int);
  void toggleSampleInfoWidget();
  void clonePlot(int);  // the id of this plot window.. 
  //ExportWidget* exportDetails;  // this actually will be stuck into the exportDialog.. hmm. 
  //QSlider* xPos;
  //QSlider* yPos;
  //QSlider* zPos;
  
  //QSlider* xScale;
  //QSlider* yScale;
  //QSlider* zScale;
  
  //QSlider* xRot;
  //QSlider* yRot;
  //QSlider* zRot;
};

#endif
