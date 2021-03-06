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

#include "pointPlotter.h"
#include <qpainter.h>
#include <iostream>
#include <qstring.h>
#include <qcolor.h>

using namespace std;

PointPlotter::PointPlotter(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  values.resize(0);    // probably don't need, but it's good to do so..
  useZero = false;
  pixmap = new QPixmap(size());    // maybe not necessary, but..
  min = max = 0;
  setBackgroundMode(NoBackground);
  // if nothing else..
  threshold = 1;
  drawThreshold = false;
  thresholdChanging = false;

}

void PointPlotter::paintEvent(QPaintEvent* e){
  bitBlt(this, 0, 0, pixmap, 0, 0, width(), height());
}

void PointPlotter::plotPoints(vector<float> v, bool use_zero){
  useZero = use_zero;
  values = v;
  
  if(!v.size()){
    plotPoints();   // anyway, will give use something black.. 
    return;
  }
  max = v[0];
  if(useZero){
    min = 0;
  }else{
    min = max;
  }
  for(int i=0; i < values.size(); i++){
    if(v[i] > max){ max = v[i]; }
    if(v[i] < min){ min = v[i]; }
  }
  plotPoints();
}

void PointPlotter::plotPoints(){
  pixmap->resize(size());
  pixmap->fill(QColor(0, 0, 0));
  if(!values.size()){
    repaint();   // force a thingy..
    return;
  }
  QPainter p(pixmap);   // paint on the pixmap..
  // plot points on a vertical line.. going from min value to max value bottom to top..
  int tm, bm, lm, rm;    // the margins on either side of the plot..
  int indent = 10;
  tm = bm = rm = 30;   // what the hell.. 
  lm = 50;     // need space for the labels.
  int h = height() - tm - bm;
  int w = width() - lm - rm;
  int x = lm + indent;      // just indent it a few pixels.. 
  int mw=8;               // marker width.. 
  float range = max - min;
  p.setPen(QPen(QColor(255, 255, 255), 1));
  p.setBrush(QColor(0, 0, 255));          // a blue brush.. 
  for(int i=0; i < values.size(); i++){
    int y = tm + h - mw/2 - (int)( ((float)h * (values[i]-min))/range  );
    p.drawEllipse(x, y, mw, mw);
  }
  // and let's draw some axis.. -- 
  p.drawLine(lm, tm, lm, tm+h);
  int y = tm + h;
  if(min <= 0){    // then we like to indicate this.. 
    y = tm + h - (int)( ((float)h * (0-min))/range  );
  }
  p.drawLine(lm, y, lm + w, y);
  // and let's put some labels on it.. -- let's have a label every 40 pixels or so..
  int labelHeight = 40; 
  int ln = h/labelHeight;
  QFont fnt = font();    // 
  if(fnt.pointSize() == -1){
    //cout << "setting pixel size to : " << fnt.pixelSize() << endl;
    fnt.setPixelSize(fnt.pixelSize() - 4);
  }else{
    fnt.setPointSize(fnt.pointSize() - 2);
    //cout << "setting point size to : " << fnt.pointSize() - 2 << endl;
  }
  p.setFont(fnt);
  for(int i=0; i < ln; i++){
    // and work out what label to put on it.. in the range of min to max..
    float value = min + (float(i)*range / float(ln-1));
    int y = tm + h - (int)( ((float)h * (value-min))/range  );    // wich of course is a long way round of doing it..
    p.drawLine(lm, y, lm + indent/2, y);
    // and let's draw some text..
    QString number;
    number.sprintf("%1.1e", value);
    p.drawText(0, y - labelHeight/2, lm-3, labelHeight, Qt::AlignRight|Qt::AlignVCenter, number);
  } 
  // if the threshold is changing we need to recalculate it from the change in position..
  if(thresholdChanging){
    //cout << "threshold changing from " << threshold << "  to : ";
    threshold -=  (range * float(thisY - lastY)/float(h));
    //cout << threshold << endl;
  }
  if(drawThreshold){   // well we better draw it..
    int y = tm + h - (int)( ((float)h * (threshold-min))/range  );
    // and if silly, don't draw it..
    if(y <= h + tm){
      QString number;
      number.sprintf("%1.3e", threshold);
      p.drawLine(lm, y, lm + w, y);
      p.drawText(lm, y - labelHeight, w - 3, labelHeight-2, Qt::AlignRight|Qt::AlignBottom, number); 
    }
  }

  repaint();
}

void PointPlotter::resizeEvent(QResizeEvent* e){
  plotPoints();    // just replot the points..
}

void PointPlotter::mousePressEvent(QMouseEvent* e){
  if(e->button() == Qt::RightButton){
    drawThreshold = !drawThreshold;
    if(drawThreshold){           // lets make sure that this threshold gets used ..
      emit setThreshold(threshold);
    }
    //    cout << "calling repaint to get the thing to paint again. draw Threshold is " << endl;
    plotPoints();
    return;
  }
  if(e->button() == Qt::LeftButton && drawThreshold){
    lastY = e->y();
    thresholdChanging = true;
  }
}

void PointPlotter::mouseMoveEvent(QMouseEvent* e){
  if(thresholdChanging){
    // adjust the threshold, and repaint...
    thisY = e->y();
    //cout << " mouse move event threshold changing hopefully " << endl;
    plotPoints();
  }
  lastY = thisY;
}

void PointPlotter::mouseReleaseEvent(QMouseEvent* e){
  thresholdChanging = false;
  emit setThreshold(threshold);
}

