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

#include "orderWidget.h"
#include <qwidget.h>        // shouldn't be necessary to inlcude very much at the moment..
#include <qapplication.h>
#include <qstring.h>
#include <qpainter.h>
#include <iostream>
#include <qregion.h>
#include <qpointarray.h>

using namespace std;

OrderWidget::OrderWidget(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  //  eMap = em;    // at this stage this may be empty, so there is little point in use changing this..
  movingPoint = 0; 
  displayInfo = -1;
  pointSize = 12;
  labelWidth = 20;
  hmargin = 25;
  vmargin = 25;
  pixmap = new QPixmap(size());       // which ofcourse will be empty, but never mind.. we can resize it when we need. 
  setBackgroundMode(NoBackground);    // for memory intensive, but flicker free redraw.. 
  
  menu = new QPopupMenu(this, "menu");
  menu->insertItem("Reset Points", this, SLOT(resetPoints()) );
  menu->insertItem("All Active", this, SLOT(allPointsActive()) );
  //  menu->insertItem("All Chips", this, SLOT(allChips()) );  //not so useful at the moment.. 
  menu->insertItem("All Inactive", this, SLOT(noPointsActive()) );
}

void OrderWidget::setExperiments(multimap<float, exptInfo> em){
  cout << "OrderWidget setExperiments " << endl;
  eMap = em;
  // and empty moving points..
  movingPoints.clear();
}

exptPoints OrderWidget::currentPoints(){
  exptPoints pts;
  for(int i=0; i < drawPoints.size(); i++){
    if(drawPoints[i].active){
      pts.values.push_back(-(float)drawPoints[i].yo);
      pts.indices.push_back(drawPoints[i].dbIndex);
    }
  }
  return(pts);
}

void OrderWidget::paintEvent(QPaintEvent* e){
  // first resize the pixmap..
  pixmap->resize(size());
  pixmap->fill(QColor(255, 255, 255));
  // then make sure the drawPoints and the exptPoints are sufficiently large to hold the data..
  if(!eMap.size()){
    cerr << "no contents in emap, leave empty" << endl;
    return;
  }
  
  map<float, exptInfo>::iterator it;
  if(drawPoints.size() != eMap.size()){
    drawPoints.resize(eMap.size());       // if this is the case then something changed, so we should probably put all the points on the 0 axis..
    int i = 0;
    for(it = eMap.begin(); it != eMap.end(); it++){
      drawPoints[i].dbIndex = (*it).second.dbaseIndex;
      drawPoints[i].index = (*it).second.index;
      //drawPoints[i].active = true;  defaults to false.. which is ok for this application.
      drawPoints[i].yo = 0;
      i++;
    }
  }
  // then very simply we just draw the axis, and then the lines..
  QPainter p(pixmap);
  p.setPen(QPen(QColor(0, 0, 0), 2));
  p.drawLine(hmargin, height()/2, width()-hmargin, height()/2);
  p.drawLine(hmargin, vmargin, hmargin, height()-vmargin);
  
  // and then draw a blob at each position for a drawPoint..
  p.setPen(NoPen);
  p.setBrush(QColor(0, 0, 255));     // bright blue man.. let's have something milder later on... 
  for(int i=0; i < drawPoints.size(); i++){
    int x = hmargin - pointSize/2 + ( (i+1) * (width() - 2*hmargin)) / (eMap.size()) + drawPoints[i].xo;
    int y = height()/2 - pointSize/2 + drawPoints[i].yo;      
    if(drawPoints[i].active){
      p.setBrush(QColor(0, 0, 255));
    }else{
      p.setBrush(QColor(100, 100, 100));
    }
    drawPoints[i].rect.setRect(x, y, pointSize, pointSize);      
    p.drawEllipse(drawPoints[i].rect);
  }
  // draw numbers -- separate loop so we don't have to set the pen each time..
  p.setPen(QPen(QColor(200, 0, 50), 1));
  int pointsize = qApp->font().pointSize()-1;
  p.setFont(QFont("Helvetica", pointsize));
  QString label;
  QRect tRect;
  for(int i=0; i < drawPoints.size(); i++){
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
    it = eMap.find(drawPoints[displayInfo].index);
    if(it != eMap.end()){
      p.drawText(hmargin, height()-vmargin/2, (*it).second.shortName.c_str());
    }
  } 
  // we should ofcourse also be drawing numbers and stuff, but lets work that out later on,,,
  bitBlt(this, 0, 0, pixmap, 0, 0, width(), height());
}

void OrderWidget::mouseMoveEvent(QMouseEvent* e){
  //cout << "pos : " << e->x() << ", " << e->y() << endl;
  if(movingPoint){
    //    movingPoint->yo += (e->y() - lastY);
    // movingPoint->xo += (e->x() - lastX);
    for(vector<drawPoint>::iterator it=drawPoints.begin(); it != drawPoints.end(); it++){
      if((*it).active){
	(*it).yo += (e->y() - lastY);
	(*it).xo += (e->x() - lastX);
      }
    }
    update();
  }else{
    // if no moving point then we will draw a line to select things.. 
    // do this by drawing on top of everything rather than by calling update each time..
    if(areaPoints.size()){
      QPainter p(this);
      p.setPen(QPen(QColor(0, 150, 200), 1));
      //p.drawPoint(e->pos());
      p.drawLine(lastX, lastY, e->x(), e->y());
      areaPoints.push_back(e->pos());
    }
  }
  lastY = e->y();
  lastX = e->x();
}

void OrderWidget::checkSelected(){
  if(areaPoints.size() < 10){  // not so likely to be anything..
    return;
  }
  QPointArray pts(areaPoints.size());
  for(int i=0; i < areaPoints.size(); i++){
    pts.setPoint(i, areaPoints[i]);
  }
  // make a QRegion with the points..
  QRegion r(pts);
  for(int i=0; i < drawPoints.size(); i++){
    // get the center of the drawPoint..
    int x = drawPoints[i].rect.x() + drawPoints[i].rect.width()/2;
    int y = drawPoints[i].rect.y() + drawPoints[i].rect.height()/2;
    if(r.contains(QPoint(x, y))){
      drawPoints[i].active = !drawPoints[i].active;
    }
  }
}

void OrderWidget::mousePressEvent(QMouseEvent* e){
  // check if we have an overlap with any of the rectangles..
  //  int dispInfo = -1;
  bool foundDot = false;
  for(int i=0; i < drawPoints.size(); i++){
    if(drawPoints[i].rect.contains(e->pos())){
      //if(e->button() == Qt::LeftButton && drawPoints[i].active){            // the burden of history ..
      if(e->button() == Qt::LeftButton){
	movingPoint = &drawPoints[i];
	drawPoints[i].active = true;
      }
      if(e->button() == Qt::RightButton){
	drawPoints[i].active = !drawPoints[i].active;
	if(movingPoints.count(&drawPoints[i])){
	  movingPoints.erase(&drawPoints[i]);
	}else{
	  movingPoints.insert(&drawPoints[i]);
	}
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
    if(e->button() == Qt::LeftButton){
      areaPoints.push_back(e->pos());
    }
  }
  firstY = lastY = e->y();
  firstX = lastX = e->x();
  update();
  //  cout << "pos : " << e->x() << ", " << e->y() << endl;
}

void OrderWidget::mouseReleaseEvent(QMouseEvent* e){
  //cout << "pos : " << e->x() << ", " << e->y() << endl;
  if(e->x() != firstX || e->y() != firstY){
    if(movingPoint){
      for(vector<drawPoint>::iterator it=drawPoints.begin(); it != drawPoints.end(); it++){
	(*it).active=false;
      }
    }
  }
  movingPoint = 0;
  checkSelected();
  update();
  areaPoints.resize(0);
}

void OrderWidget::resetPoints(){
  for(int i=0; i < drawPoints.size(); i++){
    drawPoints[i].yo = 0;
    drawPoints[i].xo = 0;
  }
  update();
}
  
void OrderWidget::allPointsActive(){
  for(int i=0; i < drawPoints.size(); i++){
    drawPoints[i].active = true;
  }
  update();
}

void OrderWidget::allChips(){
  map<float, exptInfo>::iterator it;
  for(int i=0; i < drawPoints.size(); i++){
    it = eMap.find(drawPoints[i].index);
    if(it != eMap.end()){
      bool show = true;
      map<int, bool>::iterator bit;
      for(bit = (*it).second.chips.begin(); bit != (*it).second.chips.end(); bit++){
	if(!(*bit).second){
	  show = false;
	}
      }
      drawPoints[i].active = show;
    }
  }
  update();
}

void OrderWidget::setActive(map<int, bool> cmap){    // int has to refer to chip equivalents, otherwise this won't work.
  cout << "this is setActive, did we get here.. " << endl;
  map<int, bool>::iterator bit;
  map<float, exptInfo>::iterator it;
  map<int, bool>::iterator bit2;
  for(uint i=0; i < drawPoints.size(); i++){
    it = eMap.find(drawPoints[i].index);
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
    drawPoints[i].active = show;
  }
  update();
}

void OrderWidget::noPointsActive(){
  for(int i=0; i < drawPoints.size(); i++){
    drawPoints[i].active = false;
  }
  update();
}

multimap<float, exptInfo> OrderWidget::exptOrder(){
  // basically sort the drawPoint thingy by their x position..
  // and then give out those experiment numbers.. which should 
  // correspond to the correct order..
  // doesn't work.. make a multimap first and read off the numbers from this..
  multimap<float, float> indexOrder;
  for(int i=0; i < drawPoints.size(); i++){
    // then work out its x positions..
    int x = hmargin - pointSize/2 + ( (i+1) * (width() - 2*hmargin)) / (eMap.size()) + drawPoints[i].xo;
    // ofcourse it might be good to store this, in the point, but .. but this wil do for now..
    indexOrder.insert(make_pair((float)x, drawPoints[i].index));   // I'm guessin this is the real index..
  }
  // and make a vector of the indices..
  multimap<float, exptInfo> expts;   // copy from eMap, but don't change eMap.. 
  multimap<float, exptInfo>::iterator mit;
  for(multimap<float, float>::iterator it=indexOrder.begin(); it != indexOrder.end(); it++){
    mit = eMap.find((*it).second);   // 
    if(mit != eMap.end()){
      /// note the original multimap, has to be unique.. but subsequent ones don't have this issue 
      // but for this reason it is essential that we do not allow the experiment chooser to set the 
      // the order for this widget.. so we will need to block stuff.. during the setting..
      expts.insert(make_pair((*it).first, (*mit).second)); 
    }
    
  }
  return(expts);
}

map<int, bool> OrderWidget::activityStates(){
  // make a map of what's active or not..
  map<int, bool> states;
  for(vector<drawPoint>::iterator it=drawPoints.begin(); it != drawPoints.end(); it++){
    states.insert(make_pair((*it).dbIndex, (*it).active));
  }
  return(states);

}
