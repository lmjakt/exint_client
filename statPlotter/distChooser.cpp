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

#include "distChooser.h"
#include <vector>
#include <qwidget.h>
#include <qapplication.h>
#include <math.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpointarray.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <fstream>
#include <iostream>

using namespace std;

DistChooser::DistChooser(string sName, int ds, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  cout << "constructing distchooser " << endl;
  divs = ds;
  statisticName = sName;
  rangesChanged = false;
  showText = false;
  axisState = 0;       // default, no axis used.. 
  minTValue = maxTValue = 0;
  counts.resize(divs);          // initialises the vectors to a count of 0 if I remember correctly. 
  logCounts.resize(divs);
  for(int i=0; i < divs; i++){
    counts[i] = 0;
    logCounts[i] = 0;
  }
  values.resize(0);       // just make sure.. !
  lastX = 0;
  lastY = 0;              // just in case something goes horribly wrong.
  setBackgroundMode(NoBackground);
  //  setPaletteBackgroundColor(QColor(201, 206, 221));       // a real light blue almost white. maybe it's terrible, but I feel like trying. 
  logValues = false;
  lm = 40;
  rm = 20;
  tm = 10;
  bm = 30;
  menu = new QPopupMenu(this, "menu");
  menu->insertItem("Save Values", this, SLOT(saveValues()) );

  cout << "done constructing distchooser " << endl;
}

DistChooser::~DistChooser(){
  cout << "deleting dist chooser " << endl;
  // do nothing 
}

void DistChooser::setData(vector<int> ind, vector<float> vf){
  indices = ind;
  values = vf;
  if(values.size() == 0){ return; }
  min = values[0];
  max = values[0];
  for(int i=0; i < values.size(); i++){
      if(min > values[i]) { min = values[i]; }
    if(max < values[i]) { max = values[i]; }
  }
  lowT = 0;
  highT = 1;          // set the thresholds so that everything is sorted.. 
  setData();
}

void DistChooser::setData(){
  //cout << "Setting the data for dist Chooser " << endl;
  //cout << "Setting data for " << statisticName << "\tvalues size " << values.size() << endl;
  if(values.size() == 0){ return; }
  counts.resize(divs);
  logCounts.resize(divs);
  for(int i=0; i < divs; i++){
    counts[i] = 0;
    logCounts[i] = 0;
  }
  float range, lmax, lmin, lrange;
  range = max - min;
  if(min > 0){
    //    float range = min - max;
    lmax = log(max);
    lmin = log(min);
    lrange = log(max) - log(min);        // ??? hmmm. is this right ?? 
  }else{
    //update();     // everything is 0, I wonder what will happen.. 
    //return;
  }
  
  // then do the count dividing the space between the two areas.. 
  for(int i=0; i < values.size(); i++){
    counts[(int)(((values[i] - min)/range)*(divs-1))]++;
    //cout << statisticName << "\ti : " << i << "\tvalue : " << values[i] << "\tindex : " << (int)(((values[i] - min)/range)*(divs-1)) << endl;
    if(min > 0){
      logCounts[(int)(((log(values[i]) - lmin)/lrange)*(divs-1))]++;
      //cout << "Distribution function lvalue : " << log(values[i]) << "\tindex: " << (int)(((log(values[i]) - lmin)/lrange)*(divs-1)) << "\tmin : " << lmin << "\trange : " << lrange << endl;
    }
  }
  maxCount = counts[0];
  logMaxCount = logCounts[0];
  for(int i=0; i < divs; i++){
    if(maxCount < counts[i]) { maxCount = counts[i]; }
    if(logMaxCount < logCounts[i]) { logMaxCount = logCounts[i]; }
  }


  // in fact, that is all we need to do. 
  // cout << endl << endl << "THIS IS JUST at the end of setData, aand height is " << height() << endl << endl;
  update();        // which causes the thing to plot.. 
}

void DistChooser::setLog(bool useLog){
  logValues = useLog;
}

void DistChooser::paintEvent(QPaintEvent* e){
  //cout << "painting the distribution " << endl;
  //     << "maxCount is " << maxCount << endl;
  // well paint the bugger..
  int pointsize = qApp->font().pointSize()-2;
  QPixmap pix(width(), height());
  pix.fill(QColor(201, 206, 221));    // could change this.. ! 

  // some graph margins, so we have space for an axis and other things..
//   int lm = 40;    // space for ticks and labels
//   int rm = 20;    // just for looks
//   int tm = 10;    // just for looks
//   int bm = 30;    // neeed some numbers and ticks

  // the above are now defined in the constructor so I can read them from other functions.. 
  
  int w = width() - (rm + lm);
  int h = height() - (tm + bm);

  int ys = height()-bm;      // convenience,, rather than using something else.. 


  QPainter* p = new QPainter(&pix);
  //  p->setPen(QColor(0, 0, 0));
  int lowRange = (int)(((float)divs)*lowT);
  int highRange = (int)(((float)divs)*highT);
  int penWidth = (int)(((float)w)/((float)divs));
  penWidth++;     // so overlap.
  QPen lowPen(QColor(150, 150, 200), penWidth);
  QPen midPen(QColor(200, 0, 0), penWidth);
  QPen highPen(QColor(150, 200, 150), penWidth);
  p->setPen(lowPen);
  int selectedNo = 0;
  int tooLowNo = 0;
  int tooHighNo = 0;
  for(int i=0; i < divs; i++){
    int x = lm + (int)( (float)w*((float)i/(float)divs));
    int y = ys;      // the default (i.e. 0 value)
    if(logValues){
      if(logMaxCount){
	y = ys - (int)((float)h * ((float)logCounts[i]/(float)logMaxCount));
	//	y = height() - (int)((float)height()* ((float)logCounts[i]/(float)logMaxCount));
      }
    }else{
      if(maxCount){
	y = ys - (int)((float)h * ((float)counts[i]/(float)maxCount));
	//	y = height() - (int)((float)height()* ((float)counts[i]/(float)maxCount));
      }
    }
    if(i < lowRange){
      p->setPen(lowPen);
      if(logValues){
	tooLowNo += logCounts[i];
      }else{
	tooLowNo += counts[i];
      }
    }
    if(i >= lowRange){
      if(i < highRange){
	p->setPen(midPen);
	if(logValues){
	  selectedNo += logCounts[i];
	}else{
	  selectedNo += counts[i];
	}
      }else{
	p->setPen(highPen);
	if(logValues){
	  tooHighNo += logCounts[i];
	}else{
	  tooHighNo += counts[i];
	}
      }
    }
    p->drawLine(x, ys, x, y);
  }
  // lets draw two thin lines at the two lowT and highT values..
  p->setPen(QPen(QColor(50, 50, 50), 0));  // thin dark grey line.. 
  int lowX = ((int) (lowT * (float)w)) + lm;
  int highX = ((int) (highT * (float)w)) + lm;
  p->drawLine(lowX, ys, lowX, tm);
  p->drawLine(highX, ys, highX, tm);
  // and how about an axis and some ticks and ticklabels..
  p->setPen(QPen(QColor(0, 0, 0), 1));
  p->drawLine(lm, ys, lm+w, ys);    // x axis
  p->drawLine(lm, ys, lm, tm);
  
  int xTickno = width() / 60;
  int yTickno = height() / 30;
  //cout << "x ticks : " << xTickno << "\ty ticks : " << yTickno << endl; 
  //  int xTickno = 5;
  //int yTickno = 5;    // later adjust these dependent on the dimensions of the widget
  QString tickLabel;  // for the numbers
  int tickLength = 5;
  int tickCount;   // the count.. 
  p->setFont(QFont("Helvetica", pointsize-1));
  if(yTickno > 0){
    for(int i=0; i <= yTickno; i++){
      int y = ys - (i * h)/yTickno;
      p->drawLine(lm, y, lm-tickLength, y);
      if(logValues){
	tickCount = (i * logMaxCount)/yTickno;
      }else{
	tickCount = (i * maxCount)/yTickno;
      }
      tickLabel.setNum(tickCount);
      //    tickLabel.sprintf("%1.1e", (float)tickCount);
      //cout << tickLabel << "\t: and the count " << tickCount << endl;
      //cout << "and M_E defined in math.h is " << M_E << endl;
      p->drawText(0, y - 5, lm-2-tickLength, 16, AlignRight, tickLabel);
    }
  }
  // and now for the x axis ticks. This is much trickier as we have to calculate back from the various
  // ranges and minimum values and so on.. what are the buckets and so forth.. may not make much sense..
  double tickValue;
  if(xTickno > 0){
    for(int i=0; i <= xTickno; i++){
      //  cout << " x tick calculation.. hmm. i : " << i << endl;
      int x = lm + (i * w)/xTickno;
      p->drawLine(x, ys, x, ys+tickLength);
      // and now work out the value
      if(logValues){
	// the range is divided between log(min) and log(max) -in equal lengths. x * i /
	tickValue = ((float)((float)i/(float)xTickno) * (log(max) - log(min))) + log(min);      // this now is the log value we can pow that by M_E,, 
	tickValue = pow(M_E, tickValue);
      }else{
	tickValue = ((float)((float)i/(float)xTickno) * (max - min)) + min;      // this now is the log value we can pow that by M_E,, 
      }
      tickLabel.sprintf("%1.1e", tickValue);
      //cout << "\t\tend of x calculation " << endl;
      p->drawText(x-20, ys + tickLength + 2, 40, 16, AlignHCenter, tickLabel);
    }
  }
  //cout << "Max : " << max << "\tlog Max : " << log(max) << "\tMin : " << min << "\tlog(Min) : " << log(min) << endl;
  // lets draw some statistics in the corner. Reserve 100 pixels from the righthand side and draw there
  // just use the tickLabel QString for the labels..
  int textSpace = 115;
  int rowHeight = 11;
  int topSpace = 8;
  p->setFont(QFont("Helvetica", pointsize-1));
  tickLabel = statisticName.c_str();
  if(logValues){
    tickLabel.prepend("Log ");
  }
  p->drawText(0, topSpace, width(), rowHeight, AlignHCenter, tickLabel);
  topSpace += rowHeight;
  //  cout << "Drawing again,, and showText is " << showText << endl;
  if(logValues){
    minTValue = pow((float)M_E, (lowT * (log(max) - log(min))) + log(min));
    maxTValue = pow((float)M_E, (highT * (log(max) - log(min))) + log(min));
  }else{
    minTValue = lowT * (max - min) + min;
    maxTValue = highT * (max - min) + min;
  }    
  if(showText){
    p->setFont(QFont("Helvetica", pointsize-1));
    tickLabel.sprintf("%1.2e", max);
    tickLabel.prepend("Max Value: ");
    topSpace += rowHeight;
    p->drawText(width()-textSpace, topSpace, textSpace, rowHeight, AlignLeft, tickLabel);
    tickLabel.sprintf("%1.2e", min);
    tickLabel.prepend("Min Value: ");
    topSpace += rowHeight;
    p->drawText(width()-textSpace, topSpace, textSpace, rowHeight, AlignLeft, tickLabel);
    //float minTValue, maxTValue;
    tickLabel.sprintf("%1.2e", minTValue);
    tickLabel.prepend("Low  T:   ");
    topSpace += rowHeight;
    p->drawText(width()-textSpace, topSpace, textSpace, rowHeight, AlignLeft, tickLabel);
    tickLabel.sprintf("%1.2e", maxTValue);
    tickLabel.prepend("High T:   ");
    topSpace += rowHeight;
    p->drawText(width()-textSpace, topSpace, textSpace, rowHeight, AlignLeft, tickLabel);
    tickLabel.setNum(tooLowNo);
    tickLabel.prepend("below :   ");
    topSpace += rowHeight;
    p->drawText(width()-textSpace, topSpace, textSpace, rowHeight, AlignLeft, tickLabel);
    tickLabel.setNum(tooHighNo);
    tickLabel.prepend("above :   ");
    topSpace += rowHeight;
    p->drawText(width()-textSpace, topSpace, textSpace, rowHeight, AlignLeft, tickLabel);
    tickLabel.setNum(selectedNo);
    tickLabel.prepend("Selected :");
    topSpace += rowHeight;
    p->drawText(width()-textSpace, topSpace, textSpace, rowHeight, AlignLeft, tickLabel);
  //cout << "lowT : " << lowT << "\thighT : " << highT << endl;
  //cout << "total num: " << tooLowNo + tooHighNo + selectedNo << endl;
  }
  /// whoa this is getting a little long.. oh well never mind.. OK, draw some little boxes we can use as buttons..
  // in the right hand side margin..
  textArea = QRect(w+lm+3, tm, rm-6, rm-6);
  int bw = rm-6;
  int bh = rm-6;
  int bs = rm-4;         // button offset,, 
  int bo = 40;       // buttongroup offset.. !! 
  xArea = QRect(w+lm+3, tm + bo, bw, bh);
  bo += bs;
  yArea = QRect(w+lm+3, tm +bo, bw, bh);
  bo += bs;
  zArea = QRect(w+lm+3, tm +bo, bw, bh);
  

  if(showText){
    p->setBrush(QColor(180, 180, 180));
  }else{
    p->setBrush(QColor(200, 125, 125));
  }
  p->setPen(QPen(QColor(150, 150, 200), 0));
  p->drawRect(textArea);
  //  p->drawRect(w+lm+3, tm, rm-6, rm-6);
  p->setFont(QFont("Times", pointsize-1));
  p->setPen(QPen(QColor(200, 200, 200), 0));
  //  p->setPen(QPen(QColor(75, 5, 5), 0));
  //p->drawText(w+lm+6, tm+(rm-8), "T");
  p->drawText(textArea, Qt::AlignCenter, "T");
  
  // draw X, y and z..
  p->setBrush(QColor(180, 180, 180));
  p->setPen(QPen(QColor(150, 150, 200), 0));
  p->drawRect(xArea);
  p->drawRect(yArea);
  p->drawRect(zArea);
  p->setBrush(QColor(200, 125, 125));
  switch(axisState){
  case 0:
    break;     // do nothing
  case 1:
    p->drawRect(xArea);
    break;
  case 2:
    p->drawRect(yArea);
    break;
  case 3:
    p->drawRect(zArea);
    break;
  default :
    cout << "unknown axis State " << endl;
  }
  p->setFont(QFont("Helvetica", pointsize-1));
  p->setPen(QPen(QColor(0, 0, 0), 0));

  p->drawText(xArea, AlignCenter, "X");
  p->drawText(yArea, AlignCenter, "Y");
  p->drawText(zArea, AlignCenter, "Z");

 //   p->drawText(xArea.x() + 3, xArea.y()+(bh-2), "X");
//    p->drawText(yArea.x() + 3, yArea.y()+(bh-2), "Y");
//    p->drawText(zArea.x() + 3, zArea.y()+(bh-2), "Z");

  delete p;
  bitBlt(this, 0, 0, &pix, 0, 0);  
  //cout << "and really that's it !! for now anyway.. ho yeahh.. " << endl;
  //  cout << "Finished painting " << endl;
}

void DistChooser::mousePressEvent(QMouseEvent* e){
  // hello do nothing for now..
  lastX = e->x();
  lastY = e->y();
  mouseMoved = false;
  bool axisChange = false;
  int axisWanted;
  if(textArea.contains(e->pos())){
    showText = !showText;
    update();
    return;
  }
  if(xArea.contains(e->pos())){
    if(axisState == 1){
      axisWanted = 0;
    }else{
      axisWanted = 1;
    }
    axisChange = true;
    //    update();
  }
  if(yArea.contains(e->pos())){
    if(axisState == 2){
      axisWanted = 0;
    }else{
      axisWanted = 2;
    }
    axisChange = true;
    //    update();
  }
  if(zArea.contains(e->pos())){
    if(axisState == 3){
      axisWanted = 0;
    }else{
      axisWanted = 3;
    }
    axisChange = true;
    //    update();
  }
  if(axisChange){
    emit axisWish(axisWanted);
    update();
  }
  cout << "Mouse Click event : lastX " << lastX << "\tlastY : " << lastY << endl;
}

void DistChooser::mouseDoubleClickEvent(QMouseEvent* e){
  logValues = !logValues;
  //setData();   // for mins and maxes ?? 
  repaint();         // this will effectively change the thresholds and the easiest way is for me just to change the.. 
  emit setLogValues(logValues);  // hmm..... 
  emit newRanges(minTValue, maxTValue);
  
}

void DistChooser::mouseMoveEvent(QMouseEvent* e){
  // hello there
  mouseMoved = true;
  bool dataChanged = false;
  bool redraw = false;
  float oldHigh, oldLow;
  switch(e->state()){
  case 1:       
    // left mouse button change minThreshold ??
    lowT += ((float)(e->x() - lastX))/ ((float)(width()-lm-rm));
    if(lowT < 0) { lowT = 0; }
    if(lowT > 1) { lowT = 1; }
    if(lowT > highT) { lowT = highT; }
    redraw = true;
    rangesChanged = true;
    //    emit newRanges(minTValue, maxTValue);          // not fast enough to do this yet.. maybe if I can optimise all of the functions.  
//      if(lowT < 0 || lowT > highT || lowT > 1.0){
//        redraw = false;
//      }else{
//        redraw = true;
//      }
    break;
  case 2:
    /// right mouse button change max Threshold (as this is on the right hand side)
    highT += ((float)(e->x() - lastX))/ ((float)(width()-lm-rm));
    if(highT > 1) { highT = 1; }
    if(highT < lowT) { highT = lowT; }
    if(highT < 0) { highT = 0; }       // redundant, as it shouldn't happen if we follow the other rules.. 
    redraw = true;
    rangesChanged = true;
  //    if(highT < 0 || lowT > highT || highT > 1.0){
//        redraw = false;
//      }else{
//        redraw = true;
//      }
    break;
  case 4:
    oldLow = lowT;
    oldHigh = highT;
    lowT += ((float)(e->x() - lastX))/ ((float)(width()-lm-rm));
    highT += ((float)(e->x() - lastX))/ ((float)(width()-lm-rm));
    if(highT < 0 || lowT > highT || highT > 1.0 || lowT < 0 || lowT > 1){
      lowT = oldLow;
      highT = oldHigh;
      redraw = false;
    }else{
      rangesChanged = true;
      redraw = true;
    }
    break;
  case 513:
    // control left.. change divs .. 
    //cout << "Mouse Move event : lastX " << lastX << "\tlastY : " << lastY << endl;
    divs += (lastY - e->y());
    //cout << "divs " << divs << endl;
    if(divs < 3) { divs = 3; }
    //cout << "divs " << divs << endl << endl;
    dataChanged = true;
    break;
  default :
    // do nothing
    break;
  }
  lastY = e->y();
  lastX = e->x();
  if(dataChanged){
    setData();
  }
  if(redraw){
    update();
  }
}



void DistChooser::mouseReleaseEvent(QMouseEvent* e){
  // send change signal thingy,, (if the mouse has been moved use some boolean or something)
  if(rangesChanged){
    emit newRanges(minTValue, maxTValue);
    rangesChanged = false;
    cout << "Ranges are changing, do some connections for gods sake !! " << endl;
    cout << "min Threshold : " << minTValue << "\tmaxTValue : " << maxTValue << endl;
  }
  if(e->button() == Qt::RightButton && !mouseMoved){
    menu->popup(mapToGlobal(e->pos()));
  }
}

int DistChooser::currentAxis(){
  return(axisState);
}

void DistChooser::setAxis(int as){
  if(as >= 0 && as < 4){        // should really just use an 
    axisState = as;
  }
  repaint();
  //  update();
  //emit newRanges(minTValue, maxTValue);
}

float DistChooser::tMax(){
  return(maxTValue);
}

float DistChooser::tMin(){
  return(minTValue);
}

void DistChooser::saveValues(){
  QString fileName = QFileDialog::getSaveFileName();
  if(fileName.isNull()){
    return;
  }
  ofstream out(fileName.latin1());
  if(!out){
    cerr << "Couldn't open file Name " << fileName.latin1() << endl;
    return;
  }
  if(indices.size() != values.size()){
    cerr << "Indices size is not equal to values size,, some bug somewhere, please fix.. " << endl;
    return;
  }
  for(int i=0; i < indices.size(); i++){
    out << indices[i] << "\t" << values[i] << endl;
  }
  //  close(out);
}
