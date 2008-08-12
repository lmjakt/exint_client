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

#ifndef TWODIMDISTPLOTTER_H
#define TWODIMDISTPLOTTER_H

#include "../stat/twoDimDist.h"
#include <qwidget.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qrect.h>

struct boolSet {
  bool top;
  bool bot;
  bool right;
  bool left;
  bool inside;
  boolSet(){
    top = false;
    bot = false;
    right = false;
    left = false;
    inside = false;
  }
  bool somethingTrue(){
    return(top || bot || right || left || inside);   // should return true if anything is true inside.. 
  }
  void reset(){
    top = false;
    bot = false;
    right = false;
    left = false;
    inside = false;
  }
};

class TwoDimDistPlotter : public QWidget
{
  Q_OBJECT
    public :
    TwoDimDistPlotter(QWidget* parent=0, const char* name=0);
  ~TwoDimDistPlotter();

  void setData(distMap* d, int sgma, bool newData=true);
  
  private slots :
    void toggleStats();
  void toggleCrossHair();

  private :
    distMap* data;
  QPixmap* pixmap;    // yeah I know, but it plots nicely..
  QPopupMenu* menu;
  bool showStats;
  bool showCrossHair;    // eventually moveable, but for now, let's just have a centralised one.. 
  int sigma;
  QString statString;   // set to something that describes the statistics.. 
  QRect selectRect;
  bool selecting;
  boolSet edgeSelected;   // see if an edge is selected..  --
  int lastX, lastY;       // for moving stuff.. 

  void paintEvent(QPaintEvent* e);
  void resizeEvent(QResizeEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void checkEdges(boolSet& edge, int x, int y);

};

#endif

  
