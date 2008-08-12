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

#ifndef COMPINPUTWINDOW_H
#define COMPINPUTWINDOW_H

#include "../dataStructs.h"
#include "../exptinfo.h"
#include <qwidget.h>
#include <qrect.h>     // easier than a qregion.. 
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <vector>
#include <map>

using namespace std;        // bad.... bad..



class CompInputWindow : public QWidget
{
  Q_OBJECT
    public :
    CompInputWindow(map<float, exptInfo>* em, QWidget* parent=0, const char* name=0);
  
  // something to get the points...
  exptPoints currentPoints();

  public slots :
    void setActive(map<int, bool> cmap);   // which slots are active.. -- based on chips, not samples
  void setActive(vector<int> samples);     // the sample dbaseIndex

  private :
    map<float, exptInfo>* eMap;
  vector<drawPoint> drawPoints;  // assign during drawing..
  bool selectionLock; 
  int hiddenNo;
  int pointSize;                 // the size of points to be drawn..
  int labelWidth;
  int hmargin;                   // the horizontal margin
  int vmargin;                   // the vertical margin.
  int displayInfo;               // displayInfo about this one.. 
  QPixmap* pixmap;               // having this one around facilitates doing copy .. but.. probably there's other ways. 
  QPopupMenu* menu;
  drawPoint* movingPoint;        // if a point is moving.. then this is it.
  int lastY;                     // how much to move a point by.. 
  
  void paintEvent(QPaintEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  
  private slots :
    void resetPoints();
  void allPointsActive();
  void allChips();
  void noPointsActive();
  void toggleSelectionLock();
  
  public slots :
    void setPointSize(int ps){
    pointSize = ps;
    update();
  }

};         // then later add all the slots for all the other stuff..


#endif
