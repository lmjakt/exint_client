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

#include "twoDimDistPlotter.h"
#include <iostream>
#include <qpainter.h>
#include <qcolor.h>
#include <qtextstream.h>
#include <qcursor.h>

using namespace std;

TwoDimDistPlotter::TwoDimDistPlotter(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  setBackgroundMode(NoBackground);    // but really should have something to put it there.. hmm
  setMouseTracking(true);
  resize(300, 300);
  pixmap = new QPixmap(width(), height());
  data = 0;
  showStats = false;
  showCrossHair = false;
  sigma = 0;
  selectRect.setRect(-1, -1, -1, -1);   // which will be offscreen anyway.. 
  selecting = false;
  // and don't think I need to do anything else for that..
  menu = new QPopupMenu(this, "menu");
  menu->insertItem("Toggle Stats", this, SLOT(toggleStats()) );
  menu->insertItem("Toggle Crosshair", this, SLOT(toggleCrossHair()) );
}

TwoDimDistPlotter::~TwoDimDistPlotter(){
  delete pixmap;
}

void TwoDimDistPlotter::setData(distMap* d, int sgma, bool newData){
  // if I already have some data, let's delete that..
  sigma = sgma;
  QString caption;
  cout << "before setting caption.. " << endl;
  caption.sprintf("2D Distribution (%3d)", sigma);
  cout << "string made .. " << endl;
  setCaption(caption);
  cout << "caption is : " << caption << endl;
  cout << "caption set " << endl;

  if(data && newData){
    delete data;
  }
  data = d;
  // lets set the text for the thingy..
  statString = "";
  QTextStream ts(&statString, 2);   // still not sure how to do this properly.. 
  //  QTextStream ts(&statString, QIODevice::IO_WriteOnly);
  ts << "Probe Sets : " << data->index.size() << endl
     << "Min X      : " << data->minX << endl
     << "Max X      : " << data->maxX << endl
     << "Min Y      : " << data->minY << endl
     << "Max Y      : " << data->maxY << endl;

  // maybe that is all we need to do.. naaah, let's set up the pixmap..
  pixmap->resize(size());
  pixmap->fill(QColor(0, 0, 0));
  if(!data){
    return;
  }

  QPainter p(pixmap);   
  // and now for the tricky bit..
  p.setPen(Qt::NoPen);
  double distRange = data->maxDistValue - data->minDistValue;
  for(int i=0; i < data->on; i++){
    for(int j=0; j < data->in; j++){
      int r = 0;
      int g = 0;
      int b = 0;
      // first work out the color.. ranging between min and max.
      // divide into 768 levels (0 - 769)
      double m = 1535.0;   // expanded colour range..
      //      double m = 767.0;
      // cout << "maxDist value is : " << data->maxDistValue << "  current value is " << data->values[i][j] << endl;
      int l = (int)(m * (data->values[i][j]-data->minDistValue) / distRange);  // should be ok.
      if(l < 256){
	b = l;
      }
      if(l > 255 && l < 512){
	b = 511 - l;
	g = l - 256;
      }
      if(l > 511 && l < 768){
	g = 767 - l;
	r = l - 512;
      }
      if(l > 767 && l < 1024){
	r = 255;
	b = l - 768;
      }
      if(l > 1023 && l < 1280){
	r = 255;
	b = 1279 - l;
	g = l - 1024;
      }
      if(l > 1279){   // upper bound is 1535;;
	r = 255;
	g = 255;
	b = l - 1278;
      }

//       if(l > 511){
// 	r = 255;
// 	g = 255;
// 	b = l - 512;
//       }
//       if(l > 255 && l < 512){
// 	r = 255;
// 	g = l - 256;
//       }
//       if(l < 256){
// 	r = l;
//       }
      // and then we need to work out what the x and y position should be. 
      int x = (width() * i)/data->on;
      int y = height() - (height() * j)/data->in;   // so high y values indicate high positions.. think that's the way anyway..
      // and how about the width of the rectangle to draw.. 
      int w = 1 + width()/data->on;
      int h = 1 + height()/data->on;   // that way we'll draw ones which are too big.. 
      p.setBrush(QColor(r, g, b));
      p.drawRect(x, y, w, h);
      // ok, this is probably going to be a very slow way of drawing it as we have to draw
      // 10, 000 rectangles,, - which I can imagine will take up a lot of cycles..
      // there is probably a quicker way of drawing this.. 
    }
  }
  update();    // make sure that we actually bit blth the thing..
}

void TwoDimDistPlotter::paintEvent(QPaintEvent* e){
  //  cout << "two dim dist plotter repaint event .. " << endl;
  QPainter* p = new QPainter(this);
  p->drawPixmap(0, 0, *pixmap);
  //  bitBlt(this, 0, 0, pixmap, 0, 0);
  p->setPen(QPen(QColor(175, 175, 175), 1));
  if(showStats){
//     cout << "show stats is true.. " << endl;
//     cout << "stat string is : " << statString.latin1() << endl;
//     cout << " and width is " << width() << endl;
    //  QRect rect = p.boundingRect(QRect(0, 0, width(), height()), 
    p->drawText(-10, 10, width(), height(), Qt::AlignRight|Qt::AlignTop, statString);
  }
  if(showCrossHair){
    // just draw a cross in the middle..
    p->drawLine(0, height()/2, width(), height()/2);
    p->drawLine(width()/2, 0, width()/2, height());
  }
  // let's draw the rectangle if its reasonalbe..
  p->setBrush(Qt::NoBrush);
  p->drawRect(selectRect);
  delete p;
}

void TwoDimDistPlotter::resizeEvent(QResizeEvent* e){
  setData(data, sigma, false);   // will force the thing to redraw..
}

void TwoDimDistPlotter::toggleStats(){
  showStats = !showStats;
  cout << "Toggling showStats its now : " << showStats << endl;
  repaint();
}

void TwoDimDistPlotter::toggleCrossHair(){
  showCrossHair = !showCrossHair;
  repaint();
}

bool vInside(QRect& r, int y){
  return((y <= r.bottom() && y >= r.top()));
}

bool hInside(QRect& r, int x){
  return((x >= r.left() && x <= r.right()));
}

void TwoDimDistPlotter::checkEdges(boolSet& edge, int x, int y){
  // basically check if there is an intersection between things.. 
  edge.reset();
  int t = 5;  // threshold... 
  if(abs(x - selectRect.x()) < t && vInside(selectRect, y)){ edge.left = true; }
  if(abs(x - selectRect.right()) < t && vInside(selectRect, y)){ edge.right = true; }
  if(abs(y - selectRect.top()) < t && hInside(selectRect, x)){ edge.top = true; }
  if(abs(y - selectRect.bottom()) < t && hInside(selectRect, x)){ edge.bot = true; }
  if(!edge.somethingTrue()){
    if(selectRect.contains(QPoint(x, y))){
      edge.inside = true;
    }
  }
}

void TwoDimDistPlotter::mousePressEvent(QMouseEvent* e){
  if(e->button() == Qt::RightButton){
    menu->popup(mapToGlobal(e->pos()));
  }
  if(e->button() == Qt::LeftButton){
    // first check if we have selected anything.. 
    checkEdges(edgeSelected, e->x(), e->y());
    if(edgeSelected.somethingTrue()){
      //      cout << " Something selected " << endl;
    }else{
      selecting = true;
      selectRect.setRect(e->x(), e->y(), 0, 0);
    }
  }
  lastX = e->x();
  lastY = e->y();
}

void TwoDimDistPlotter::mouseMoveEvent(QMouseEvent* e){
  //cout << "mouse move event " << endl;
  boolSet edges;
  checkEdges(edges, e->x(), e->y());
  // if this is true, then change cursor to something appropriate..
  if(!edgeSelected.somethingTrue()){
    if(edges.somethingTrue() || selectRect.contains(e->pos())){
      //  if(edges.somethingTrue() && !edgeSelected.somethingTrue()){
      //      cout << "edges something is true, we are going to try and set a cursor.. " << endl; 
      if(edges.top || edges.bot){ setCursor(QCursor(Qt::SizeVerCursor)); }
      if(edges.left || edges.right){ setCursor(QCursor(Qt::SizeHorCursor)); }
      if((edges.left && edges.top) || (edges.bot && edges.right)){ setCursor(QCursor(Qt::SizeFDiagCursor)); }
      if((edges.left && edges.bot) || (edges.top && edges.right)){ setCursor(QCursor(Qt::SizeBDiagCursor)); }
      if(edges.inside){ setCursor(QCursor(Qt::SizeAllCursor)); }
      //      if(selectRect.contains(e->pos())){ setCursor(QCursor(Qt::SizeAllCursor)); }
    }else{
      if(cursor().shape() != Qt::ArrowCursor){
	setCursor(Qt::ArrowCursor);   // seems bad to set this all the time, but presumably this function is smart enough to change 
      }
    }
  }
  //if(changed){ repaint(); }

  if(edgeSelected.somethingTrue()){
    if(edgeSelected.right){
      selectRect.setWidth(e->x() - selectRect.left());
    }
    if(edgeSelected.left){
      selectRect.setRect(e->x(), selectRect.y(), selectRect.width() + (selectRect.x() - e->x()), selectRect.height());
    }
    if(edgeSelected.bot){
      selectRect.setHeight(e->y() - selectRect.top());
    }
    if(edgeSelected.top){
      selectRect.setRect(selectRect.x(), e->y(), selectRect.width(), selectRect.height() + (selectRect.y() - e->y()) );
    }
    if(edgeSelected.inside){
      selectRect.moveBy(e->x() - lastX, e->y() - lastY);
    }
    repaint();
  }
  if(selecting){
    selectRect.setWidth(e->x() - selectRect.left());
    selectRect.setHeight(e->y() - selectRect.top());
    cout << "select Rect left : " << selectRect.left() << " right : " << selectRect.right() << "  top " << selectRect.top() << "  bottom : " << selectRect.bottom() << endl;
    repaint();
  }
  lastX = e->x();
  lastY = e->y();
}

void TwoDimDistPlotter::mouseReleaseEvent(QMouseEvent* e){
  selecting = false;
  edgeSelected.reset();
}
