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

#include "../dataStructs.h"
#include "compInputWindow.h"
#include <qwidget.h>        // shouldn't be necessary to inlcude very much at the moment..
#include <qapplication.h>
#include <qstring.h>
#include <qpainter.h>
#include <iostream>

using namespace std;

CompInputWindow::CompInputWindow(map<float, exptInfo>* em, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  eMap = em;    // at this stage this may be empty, so there is little point in use changing this..
  movingPoint = 0; 
  displayInfo = -1;
  pointSize = 12;
  labelWidth = 20;
  hmargin = 25;
  vmargin = 25;
  pixmap = new QPixmap(size());       // which ofcourse will be empty, but never mind.. we can resize it when we need. 
  setBackgroundMode(NoBackground);    // for memory intensive, but flicker free redraw.. 
  hiddenNo = 0;
  selectionLock = false;
  menu = new QPopupMenu(this, "menu");
  menu->insertItem("Reset Points", this, SLOT(resetPoints()) );
  menu->insertItem("All Active", this, SLOT(allPointsActive()) );
  //  menu->insertItem("All Chips", this, SLOT(allChips()) );  //not so useful at the moment.. 
  menu->insertItem("All Inactive", this, SLOT(noPointsActive()) );
  menu->insertItem("Toggle selection lock", this, SLOT(toggleSelectionLock()) );
}

exptPoints CompInputWindow::currentPoints(){
  exptPoints pts;
  for(int i=0; i < drawPoints.size(); i++){
    if(drawPoints[i].active && !drawPoints[i].hidden){
      pts.values.push_back(-(float)drawPoints[i].yo);
      pts.indices.push_back(drawPoints[i].dbIndex);
    }
  }
  return(pts);
}

void CompInputWindow::paintEvent(QPaintEvent* e){
  // first resize the pixmap..
  pixmap->resize(size());
  pixmap->fill(QColor(255, 255, 255));
  // then make sure the drawPoints and the exptPoints are sufficiently large to hold the data..
  map<float, exptInfo>::iterator it;
  if(drawPoints.size() != eMap->size()){
    drawPoints.resize(eMap->size());       // if this is the case then something changed, so we should probably put all the points on the 0 axis..
    int i = 0;
    for(it = eMap->begin(); it != eMap->end(); it++){
      drawPoints[i].dbIndex = (*it).second.dbaseIndex;
      drawPoints[i].index = (*it).second.index;
      drawPoints[i].active = true;
      drawPoints[i].yo = 0;
      i++;
    }
  }

  // then very simply we just draw the axis, and then the lines..
  QPainter p(pixmap);
  p.setPen(QPen(QColor(0, 0, 0), 2));
  p.drawLine(hmargin, height()/2, width()-hmargin, height()/2);
  p.drawLine(hmargin, vmargin, hmargin, height()-vmargin);
  
  // draw some lines connecting points together.. 
  p.setPen(QPen(QColor(0, 255, 255), 1));
  int first = 0;
  int second = 1;
  
  int drawPointNo = drawPoints.size() - hiddenNo;

  int drawPos1 = 0;
  int drawPos2;
  while(1){
    if(first >= drawPoints.size()){
      break;
    }
    if(drawPoints[first].hidden){
	first++;
	continue;
    }
    if(!drawPoints[first].active){
      first++;
      drawPos1++;
      continue;
    }
    // so if I'm here, first is ok,, find second..
    second = first + 1;
    drawPos2 = drawPos1 + 1;
    //cout << "first is : " << first << " second is : " << second << endl;
    while(1){
      if(second >= drawPoints.size()){
	second = -1;
	break;        // but need to identify the failure somehow..
      }
      if(drawPoints[second].hidden){
	  second++;
	  continue;
      }
      if(!drawPoints[second].active){
	second++;
	drawPos2++;
	continue;
      }else{
	break;
      }
    }
    if(second == -1){
      break;
    }
    // and if I'm here, I should have two ok points that I can draw lines between, but it's probably going to be a bit ugly..
    int x1, x2, y1, y2;
    x1 = hmargin + ( (drawPos1+1) * (width() - 2*hmargin)) / drawPointNo;
    x2 = hmargin + ( (drawPos2+1) * (width() - 2*hmargin)) / drawPointNo;
//    x1 = hmargin + ( (first+1) * (width() - 2*hmargin)) / (eMap->size());
//    x2 = hmargin + ( (second+1) * (width() - 2*hmargin)) / (eMap->size());
    y1 = height()/2 + drawPoints[first].yo;
    y2 = height()/2 + drawPoints[second].yo;
    p.drawLine(x1, y1, x2, y2);    
    first = second;
    drawPos1 = drawPos2;
  }

  // and then draw a blob at each position for a drawPoint..
  p.setPen(NoPen);
  p.setBrush(QColor(0, 0, 255));     // bright blue man.. let's have something milder later on... 
  int xi = 0;
  for(int i=0; i < drawPoints.size(); i++){
      if(drawPoints[i].hidden){
	  drawPoints[i].rect.setRect(-100, -100, pointSize, pointSize);
	  continue;
      }
      int x = hmargin - pointSize/2 + ( (xi+1) * (width() - 2*hmargin)) / drawPointNo;
//      int x = hmargin - pointSize/2 + ( (i+1) * (width() - 2*hmargin)) / (eMap->size());
      int y = height()/2 - pointSize/2 + drawPoints[i].yo;      
      if(drawPoints[i].active){
	  p.setBrush(QColor(0, 0, 255));
      }else{
	  p.setBrush(QColor(100, 100, 100));
      }
      drawPoints[i].rect.setRect(x, y, pointSize, pointSize);      
      p.drawEllipse(drawPoints[i].rect);
      xi++;
  }
  // draw numbers -- separate loop so we don't have to set the pen each time..
  p.setPen(QPen(QColor(200, 0, 50), 1));
//  int pointsize = qApp->font().pointSize()-1;
  QFont appFont = qApp->font();
  p.setFont(QFont(appFont.family(), appFont.pointSize() - 1));
//  p.setFont(QFont("Helvetica", pointsize));
  QString label;
  QRect tRect;
  for(int i=0; i < drawPoints.size(); i++){
      if(drawPoints[i].hidden){
	  continue;
      }
      label.setNum(drawPoints[i].dbIndex);
      tRect.setRect(drawPoints[i].rect.x() - (labelWidth-pointSize)/2, drawPoints[i].rect.y()+pointSize+3, labelWidth, labelWidth);
      if(displayInfo == i){
	  p.setPen(QPen(QColor(50, 0, 150), 1));
	  p.drawText(tRect, Qt::AlignCenter, label);
	  p.setPen(QPen(QColor(200, 0, 50), 1));
      }else{
	  p.drawText(tRect, Qt::AlignCenter, label);
      }
  }
  
  if(displayInfo > -1 && displayInfo < drawPoints.size()){
    it = eMap->find(drawPoints[displayInfo].index);
    if(it != eMap->end()){
      p.drawText(hmargin, height()-vmargin/2, (*it).second.shortName.c_str());
    }
  }
  if(selectionLock){
      p.setPen(QPen(QColor(200, 0, 50), 1));
      label = "Selection Locked";
  }else{
      p.setPen(QPen(QColor(0, 200, 50), 1));
      label = "Selection Unlocked";
  }
  p.drawText(0, 0, width(), height(), Qt::AlignRight|Qt::AlignBottom, label);
  // we should ofcourse also be drawing numbers and stuff, but lets work that out later on,,,
  bitBlt(this, 0, 0, pixmap, 0, 0, width(), height());
}

void CompInputWindow::mouseMoveEvent(QMouseEvent* e){
  //cout << "pos : " << e->x() << ", " << e->y() << endl;
  if(movingPoint){
    movingPoint->yo += (e->y() - lastY);
  }
  lastY = e->y();
  update();
}

void CompInputWindow::mousePressEvent(QMouseEvent* e){
  // check if we have an overlap with any of the rectangles..
  //  int dispInfo = -1;
  bool foundDot = false;
  for(int i=0; i < drawPoints.size(); i++){
    if(drawPoints[i].rect.contains(e->pos())){
      if(e->button() == Qt::LeftButton && drawPoints[i].active){
	movingPoint = &drawPoints[i];
      }
      if(e->button() == Qt::RightButton){
	drawPoints[i].active = !drawPoints[i].active;
      }
      displayInfo = i;
      foundDot = true;
      break;
    }
  }
  if(!foundDot){
    if(e->button() == Qt::RightButton){
      menu->popup(mapToGlobal(e->pos()));
    }
  }
  lastY = e->y();
  update();
  //  cout << "pos : " << e->x() << ", " << e->y() << endl;
}

void CompInputWindow::mouseReleaseEvent(QMouseEvent* e){
  //cout << "pos : " << e->x() << ", " << e->y() << endl;
  movingPoint = 0;
}

void CompInputWindow::resetPoints(){
  for(int i=0; i < drawPoints.size(); i++){
    drawPoints[i].yo = 0;
  }
  update();
}
  
void CompInputWindow::allPointsActive(){
  for(int i=0; i < drawPoints.size(); i++){
    drawPoints[i].active = true;
  }
  update();
}

void CompInputWindow::allChips(){
    if(selectionLock){
	return;
    }
    map<float, exptInfo>::iterator it;
    hiddenNo = 0;
    for(int i=0; i < drawPoints.size(); i++){
	it = eMap->find(drawPoints[i].index);
	if(it != eMap->end()){
	    bool show = true;
	    map<int, bool>::iterator bit;
	    for(bit = (*it).second.chips.begin(); bit != (*it).second.chips.end(); bit++){
		if(!(*bit).second){
		    show = false;
		}
	    }
	    drawPoints[i].hidden = !show;
	    if(!show){
		hiddenNo++;
	    }
    }
    }
    update();
}

void CompInputWindow::setActive(map<int, bool> cmap){    // int has to refer to chip equivalents, otherwise this won't work.
    if(selectionLock){
	return;
    }
    cout << "this is setActive, did we get here.. " << endl;
    map<int, bool>::iterator bit;
    map<float, exptInfo>::iterator it;
    map<int, bool>::iterator bit2;
    hiddenNo = 0;
    for(uint i=0; i < drawPoints.size(); i++){
	it = eMap->find(drawPoints[i].index);
	bool show = true;
	cout << "draw points : " << i << "\tindex: " << drawPoints[i].index << endl;
	// ok this is a bit ugly and could probably be written in a much nicer way.. but there you go..
	for(bit=(*it).second.chips.begin(); bit != (*it).second.chips.end(); bit++){
	    cout << "\t\t" << (*bit).first << "\t";
	    bit2 = cmap.find((*bit).first);
	    if(bit2 == cmap.end()){
		cout << "no entry\t";
		show = false;
	    }else{
		if((*bit2).second && !(*bit).second){
		    cout << "bit2 is true but bit.second is false, therefore false";
		    show = false;
		}
	    }
	    cout << endl;
	}
	drawPoints[i].hidden = !show;
	if(!show){
	hiddenNo++;
	}
    }
    update();
}

void CompInputWindow::setActive(vector<int> samples){
    // unfortunately this is the wrong way around..
    // ok, let's do this in a long way around..
    if(selectionLock){
	return;
    }
    set<int> active;
//    for(uint i=0; i < samples.size(); i++){
//	active.insert(samples[i]);
//    }
    active.insert(samples.begin(), samples.end());
    // I think I can do this..
    hiddenNo = 0;
    for(uint i=0; i < drawPoints.size(); i++){
	if(!active.count(drawPoints[i].dbIndex)){
//	    cout << i << "\t" << drawPoints[i].index << " not found in active set" << endl;
	    drawPoints[i].hidden = true;
	    hiddenNo++;
	    continue;
	}
	drawPoints[i].hidden = false;
    }
    update();
}


void CompInputWindow::noPointsActive(){
  for(int i=0; i < drawPoints.size(); i++){
    drawPoints[i].active = false;
  }
  update();
}

void CompInputWindow::toggleSelectionLock(){
    selectionLock = !selectionLock;
    update();
}
