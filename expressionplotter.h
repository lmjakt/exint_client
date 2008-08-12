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

#ifndef EXPRESSIONPLOTTER_H
#define EXPRESSIONPLOTTER_H

#include <qwidget.h>
#include <vector>
#include <qpoint.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qfont.h>

using namespace std;

class ExpressionPlotter: public QWidget
{
  Q_OBJECT
    public:
  enum PlotType {
      RAW, MODEL
  };
  ExpressionPlotter(PlotType tp, QWidget* parent=0, const char *name=0);  // the constructor..
  void paintEvent(QPaintEvent *e);                 // so it updates..
  void paintLines(vector< vector<float> >& v, vector<int>& exIndex, vector<int>& marks, vector<float>& devs);
  void paintLines(QPainter* p, int w, int h);   // paint current data using an external painter.. // no standard for size, but we can do later. 
  void paintLines(QPainter* p, vector< vector<float> >& v, vector<int>& exIndex);   // paint data using an external painter.. // 
 

 
 private:
  vector< vector<float> > values;                 // contains the values
  vector<int> exptIndex;                          // the experimental indices, so I know how to label the X-axis. 
  vector<int> markedOnes;                         // indices of experiments that I want to mark.. 
  vector<float> devsFromMean;                     // one for each probe pair,, how many deviations from the mean profile.. 
  float threshold;                                // how many devs from mean do we allow.. 
  vector<QColor*>  colours;                 // a vector of colours.. 
  float maxV;
  float minV;                                     // minimum and maximum values.
  int penWidth;
  int xo;
  int yo;
  int rxo;
  bool hasData;                            // if it has data or not..
  QPixmap* pixmap;
  QPopupMenu* menu;
  PlotType plotType;
  
  int currentExperiment;

  void paintLines();                                   // called by redraw functions..
  void setMinMax(vector< vector<float> >& v);        // to set min and max values.. ok la.. 
  void setColours();                                  // just called at the beginning set colours. 
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);  // for mouseover thingy.. 
  void showExpDetails(int x, int y);

  private slots :
    void copyImage();
  void selectFont();
  void exportPS();

 signals:
  void plotterStatusMessage(QString);
  void toggleSurfacePlot();
  void showPoints();
  void selectedNewFont(QFont);
  void exportEPS(ExpressionPlotter::PlotType);    // request the plotwindow to export eps either PlotType or 
  void showExperimentDetails(int);  // show some details for the current experiemnt.. 
  void toggleSampleInfoWidget();

  public slots:
    void setPenWidth(int);
  void setProbePairThreshold(float t);

  protected :

};

#endif
