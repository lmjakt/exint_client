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

#ifndef TWODPLOT_H
#define TWODPLOT_H

#include "../dataStructs.h"  // for PointCorrdinate  -- so that I can set something.. 
#include <qwidget.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <vector>
#include <map>

// first a struct for holding 2d positions..  -- derived from the vector of PointCoordinates (which can be n-dimensional..)

struct twoDPos {
  int x;
  int y;
  float fx;
  float fy;
  std::vector<QColor> colors;
  std::vector<float> v;        // the actual values.. for other purposes.. 
  double sum;                  // the sum of the v's.. substitute for mean.. it's easier.. no need to divide.. 
  bool marked;
  twoDPos(){
    x = y = 0;
    fx = fy = 0;
    marked = false;
  }
  twoDPos(float xv, float yv){      // 
    x = y = 0;                    // let the user set these as it's dependent on size of the thingy and other stuff. 
    fx = xv;
    fy = yv;
    marked = false;
  }
};

class TwoDPlot : public QWidget
{
  Q_OBJECT

    public :
    TwoDPlot(QWidget* parent=0, const char* name=0);

  public slots:
    void plot(std::vector<std::vector<float> >& v, std::vector<int>& exIndex, std::vector<int>& marks);
  void plot();     // replot without new values. Mostly if we have a new size. Wasteful if lots of window movement, but this i not likely to be the main thing..
  void setCoordinates(std::vector<PointCoordinate> pc);
  
  private slots :
    void toggleBackground();

  private :
  std::map<int, twoDPos> pos;                // all positions..
  std::vector<std::vector<float> > values;
  std::vector<int> exptIndex;
  std::vector<int> markedOnes;      // if we want to update positions -- we need to remember these.. 

  // and some other data that we need
  float maxV;                            // the maxValue
  float minV;
  float maxX, minX, minY, maxY;                      // max X and Y coordinates in the set of coordinates, -so that I can work out how to scale things when drawing.
  int probeNo;                                       // use this to get an idea of how to draw things.. 
  QPixmap* pixmap;          // the actual drawing, so that I can easily do copy and paste and save as something else.. 
  //  QPixmap* background;      // memory intensive as hell, but hey it's cheap these days.. 
  bool drawBackground;      // if true, then plot the background, on the basis of the intensities
  QPopupMenu* menu;         // for setting various bits and pieces.. 

  void paintEvent(QPaintEvent* e);       // well the painting has to be done somewhere.. eh.. (just use the pixmap..)
  void plotBackground(QPainter* p);     // use the painter p, and draw the background before putting in the little squares and stuff. 
  void adjustPositions();               // adjust the positions in the appropriate manner.. 
  void mousePressEvent(QMouseEvent* e);  // for the menu.. 
  void showEvent(QShowEvent* e);
  
  protected :
    void resizeEvent(QResizeEvent* e);

};



#endif
