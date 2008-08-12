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

#ifndef ORDERWIDGET_H
#define ORDERWIDGET_H

#include "../../exptinfo.h"
#include "../../dataStructs.h"
#include <qwidget.h>
#include <qrect.h>     // easier than a qregion.. 
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <vector>
#include <map>
#include <set>
#include <qpoint.h>

using namespace std;        // bad.... bad..



class OrderWidget : public QWidget
{
  Q_OBJECT
    public :
    OrderWidget(QWidget* parent=0, const char* name=0);
  
  // something to get the points...
  exptPoints currentPoints();

  public slots :
    void setActive(map<int, bool> cmap);   // which slots are active.. 
  void setExperiments(multimap<float, exptInfo> em);
  void setPointSize(int ps){
    pointSize = ps;
    update();
  }
  multimap<float, exptInfo> exptOrder();
  map<int, bool> activityStates();

  private :
    multimap<float, exptInfo> eMap;
  vector<drawPoint> drawPoints;  // assign during drawing.. 
  int pointSize;                 // the size of points to be drawn..
  int labelWidth;
  int hmargin;                   // the horizontal margin
  int vmargin;                   // the vertical margin.
  int displayInfo;               // displayInfo about this one.. 
  QPixmap* pixmap;               // having this one around facilitates doing copy .. but.. probably there's other ways. 
  QPopupMenu* menu;
  drawPoint* movingPoint;        // if a point is moving.. then this is it.
  set<drawPoint*> movingPoints;  // we want to be able to move several points at once.. 
  int lastY;                     // how much to move a point by.. 
  int lastX;
  int firstX, firstY;            // need separete.. 
  vector<QPoint> areaPoints;     // for selecting an area.. 
  
  void paintEvent(QPaintEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  
  private slots :
    void resetPoints();
  void allPointsActive();
  void allChips();
  void noPointsActive();
  void checkSelected();
  

};         // then later add all the slots for all the other stuff..


#endif
