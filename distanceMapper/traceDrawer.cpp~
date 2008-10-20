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

#include "traceDrawer.h"
#include <iostream>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpainter.h>

using namespace std;

TraceDrawer::TraceDrawer(vector<tracePoint> pts, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  setBackgroundMode(NoBackground);
  points = pts;
  if(points.size()){
    maxy = miny = points[0].y;
    maxx = minx = points[0].x;
    for(uint i=0; i < points.size(); i++){
      if(maxy < points[i].y){ maxy = points[i].y;}
      if(miny > points[i].y){ miny = points[i].y;}
      if(maxx < points[i].x){ maxx = points[i].x;}
      if(minx > points[i].x){ minx = points[i].x;}
    }
  }
  // which should be fine, though we'll have to be careful with stuff later on..
}

TraceDrawer::~TraceDrawer(){
  // ?
}

void TraceDrawer::paintEvent(QPaintEvent* e){
  // do this in a stupid ugly way of sorts..
  QPixmap pix(width(), height());
  pix.fill(QColor(0, 0, 0));
  int margin = 20;
  float xrange = maxx - minx;
  float yrange = maxy - miny;
  int diameter = 16;

  if(points.size()){
    float xMult = ( (float)(width() - 2 * margin))/xrange;
    float yMult = ( (float)(height() - 2 * margin))/yrange;
    cout << "xmultiplier : " << xMult << "\tymultiplier: " << yMult << endl;
    QPainter p(&pix);
    QString numString;
    // first draw the connections.. 
    p.setPen(QPen(QColor(255, 255, 255), 1));
    for(int i=1; i < points.size(); i++){
      int x1 = (int)((points[i-1].x - minx) * xMult) + margin;
      int y1 = (int)((points[i-1].y - miny) * yMult) + margin;
      int x2 = (int)((points[i].x - minx) * xMult) + margin;
      int y2 = (int)((points[i].y - miny) * yMult) + margin;
      p.drawLine(x1, y1, x2, y2);
    }
    for(int i=0; i < points.size(); i++){
      int x = (int)((points[i].x - minx) * xMult) + margin;
      int y = (int)((points[i].y - miny) * yMult) + margin;
      p.setPen(NoPen);
      p.setBrush(QColor(20, 20, 240));
      p.drawEllipse(x, y, diameter, diameter);
      // and lets draw the name..
      p.setPen(QPen(QColor(255, 255, 255), 1));
      numString.setNum(points[i].id);
      int extra = 15;
      p.drawText(x-extra, y, diameter+extra*2, diameter, Qt::AlignCenter, numString);
    }
    // and .. 
    bitBlt(this, 0, 0, &pix, 0, 0);
  }
}

