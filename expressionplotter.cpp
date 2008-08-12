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

#include <qwidget.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <vector>
#include <math.h>
#include <iostream>
#include <qapplication.h>
#include <qclipboard.h>
#include <qfontdialog.h>
#include "expressionplotter.h"

using namespace std;

ExpressionPlotter::ExpressionPlotter(PlotType pt, QWidget *parent, const char* name)
  : QWidget(parent, name)
{
  //  setCaption("Raw Expression");
  cout << "at the beginning of Expression Plotter" << endl;
  plotType = pt;
//  setMouseTracking(true);
  currentExperiment = -1;
  pixmap = new QPixmap(size());
  resize(300, 300);
  setBackgroundMode(NoBackground);
  //  setPalette(QPalette(QColor(250, 250,200)));
  setColours();
  penWidth = 2;
  hasData = false;
  cout << "at the end of Expression Plotter constructor" << endl;
  // let's have a menu for doing things...
  threshold = 1.0;

  // set the font for this widget..
//  setFont(qApp->font());
  setFont(QFont(qApp->font().family(), qApp->font().pointSize() -2));
  menu = new QPopupMenu(this, "menu");
  menu->insertItem("Copy", this, SLOT(copyImage()) );
  menu->insertItem("Toggle sample info", this, SIGNAL(toggleSampleInfoWidget()) );
  menu->insertItem("Probe Correlations", this, SIGNAL(showPoints()) ); 
  menu->insertItem("Set Font", this, SLOT(selectFont()) );
  menu->insertItem("Export Postscript", this, SLOT(exportPS()) );
  menu->insertItem("Toggle Surface Plot", this, SIGNAL(toggleSurfacePlot()) );
}

void ExpressionPlotter::paintEvent(QPaintEvent *e){
  if(hasData){
    paintLines();
    return;
  }
  //  pixmap = 0;
}

void ExpressionPlotter::paintLines(vector< vector<float> >& v, vector<int>& exIndex, vector<int>& marks, vector<float>& devs){
  values = v;
  cout << "Expression Plotter : values size " << values.size() << endl;
  exptIndex = exIndex;
  markedOnes = marks;
  cout << "before setting minmax.. " << endl;
  setMinMax(v);
  cout << "min max set " << endl;
  devsFromMean = devs;
  hasData = true;
  paintLines();
}

void ExpressionPlotter::paintLines(){
  // private function, only called by paintEvent,,
  cout << "paint lines,, " << endl;
  xo = 20;
  yo = 20;
  rxo = 50;
  int w = width()-2*xo-rxo;
  int h = height()-2*yo;
  //if(pixmap){
  //  delete pixmap;
  //  pixmap = 0;
  //}
  if(!width() || !height()){
      return;
  }
  pixmap->resize(size());
  //  pixmap = new QPixmap(width(), height());
  //  QPixmap pix(width(), height());
  if(pixmap->isNull()){
      cerr << "pixmap is null can't paint, try again" << endl;
      return;
  }
  pixmap->fill(QColor(250, 250, 200));
  //pixmap->fill(this, QPoint(0, 0));
  QPainter* p = new QPainter(pixmap);
  cout << "calling paintLines .. " << endl;
  p->begin(pixmap);
  paintLines(p, w, h);
  p->end();
  p->begin(this);
  bitBlt(this, 0, 0, pixmap, 0, 0, width(), height());
  //  p->drawPixmap(0, 0, *pixmap);   // change to bitblt,, 
  delete p;
  cout << "end of plot lines thingy, now what ? " << endl;
}
  

void ExpressionPlotter::paintLines(QPainter* p, int w, int h){
  cout << "Expression Plotter paint lines p, w, h" << endl;
  if(!hasData){
    return;
  }
  int ticknumber = 5;
  p->setPen(QPen(QColor(200, 0, 0), penWidth));
  int x1, x2, y1, y2, y;
  float range = maxV - minV;
  float fh = (float)h;
  cout << "values size is : " << values.size() << endl;
  if(!values.size()){
    return;
  }
  if(!values[0].size()){
    return;
  }
  cout << "I should be here .. " << endl;
  if(exptIndex.size() != values[0].size()){
    emit plotterStatusMessage(QString("exptIndex size is different from values[0] size. Can't really draw!!"));
    return;
  }

  // draw some vertical lines.. and some labels..
  //  p->setFont(QFont("helvetica", 8));
  //cout << "Application font is : " << qApp->font().family() << "\t" << qApp->font().pointSize() << endl;
//  int fontsize = font().pointSize()-1;
  //  int fontsize = qApp->font().pointSize()-1;
//  p->setFont(QFont(font().family(), fontsize));
  p->setFont(font());
  cout << "Set the painter to have a font of " << font().family() << " : "  << font().pointSize() << endl;
  QString tick_label;

  // First draw any marked lines that need to be drawn, so that the marks are in the background. 
  p->setPen(QPen(QColor(255, 0, 0), 4, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
  for(uint i=0; i < markedOnes.size(); i++){
    int x = rxo + xo + (markedOnes[i]*w)/(exptIndex.size()-1);
    p->drawLine(x, yo+h, x, yo);
  }

  p->setPen(QPen(QColor(0, 0, 0), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));

  tick_label.setNum(exptIndex.back());
  QRect brect = p->boundingRect(0, 0, 50, 50, Qt::AlignLeft|Qt::AlignBottom, tick_label);
  cout << "BOUNDING RECTANGLE HAS WIDTH : " << brect.width() << " height : " << brect.height() << " for a number " << exptIndex.back() << endl;
  cout << " and the space for the thingy is : " << ( (w - (rxo + 2 * xo)) / (exptIndex.size() - 1) ) << endl;

  bool drawXLabels = (brect.width() - 4)  < ( (w - (rxo + 2 * xo)) / (exptIndex.size() - 1) );

  tick_label.setNum(exptIndex[0]);
  if(drawXLabels)
      p->drawText(rxo + xo - 10, yo+h+4, 20, 10, AlignCenter, tick_label);
  for(uint i=1; i < exptIndex.size(); i++){
    int x = rxo + xo + (i*w)/(exptIndex.size()-1);
    p->setPen(QPen(QColor(150, 150, 150), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
    p->drawLine(x, yo+h, x, yo);
    tick_label.setNum(exptIndex[i]);
    p->setPen(QPen(QColor(0, 0, 0), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
    if(drawXLabels)
	p->drawText(x-10, yo+h+4, 20, 10, AlignCenter, tick_label);
  }
  p->setPen(QPen(QColor(0, 0, 0), penWidth, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));

  // lets draw an x -axis..
  y = yo+h - (int)((0-minV)*fh/range);
  p->drawLine(rxo + xo, y, rxo+xo+w, y);
  // and a y -axis
  p->drawLine(rxo+xo, yo+h, rxo+xo, yo);  // should be.. 
  // some labels and ticks..

  float tick_value;
  for(int i=0; i <= ticknumber; i++){
    tick_value = i*(range/ticknumber);
    tick_label.sprintf("%1.1e", tick_value+minV);
    y = yo+h - (int)((tick_value)*fh/range);
    p->drawText(0, y-8, rxo+xo-2, 25, AlignRight, tick_label);
    p->drawLine(rxo+xo, y, rxo+xo+10, y);
  }

  //cout << "and now in the plot function.. : " << values.size() << endl;
  for(uint i=0; i < values.size(); i++){
    // if devsFromMean is larger than threshold, then go to next one..
    if(devsFromMean.size() > i){
      if(devsFromMean[i] > threshold){
	//	cout << " Too high i: " << i << "  devsFrom Mean : " << devsFromMean[i] << "   threshold : " << threshold << endl;
	continue;
      }
    }
    //cout << " i: " << i << "  devsFrom Mean : " << devsFromMean[i] << "   threshold : " << threshold << endl;
    int mx = values[i].size()-1;
    p->setPen(QPen(*colours[i % colours.size()], penWidth, PenStyle(SolidLine), PenCapStyle(RoundCap), PenJoinStyle(MiterJoin)));
    //    p.setPen(QPen(*colours[i % colours.size()], penWidth));
    for(uint j=0; j < values[i].size()-1; j++){
      x1 = rxo+xo + (j*w)/mx;
      x2 = rxo+xo + ((j+1)*w)/mx;
      y1 = yo+h - (int)((values[i][j]-minV)*fh/range);
      y2 = yo+h - (int)((values[i][j+1]-minV)*fh/range);
      p->drawLine(x1, y1, x2, y2);
    }
  }
  // lets draw a thin white lint on top of the x-axis
  p->setPen(QPen(QColor(255, 255, 255), 0));
  y = yo+h - (int)((0-minV)*fh/range);  
  p->drawLine(rxo+xo, y, rxo+xo+w, y);
  //  p.end();
  //p.begin(this);
  //p.drawPixmap(0, 0, pix);
}

void ExpressionPlotter::paintLines(QPainter* p, vector< vector<float> >& v, vector<int>& exIndex){
  // first find out the w and h from the QPaintDevice.. -assume that this is a QPixmap.. 
  QPixmap* pd = (QPixmap*)p->device();
  //  int w = width()-2*xo-rxo;
  //int h = height()-2*yo;
  int w = pd->width()-2*xo-rxo;
  int h = pd->height()-2*yo;    // should work, but I suppose it will crash if it's wrong.. 

  cout << endl << "\t\tPaintLines : w " << w << "\th " << h << endl;
  int ticknumber = 5;

  p->setPen(QPen(QColor(200, 0, 0), penWidth));
  int x1, x2, y1, y2, y;
  // need to find the max and min values in v,, hmm. maybe I can't use the built in function.. as this will screw things up
  float lmaxV = v[0][0];
  float lminV = v[0][0];               // l for local.. 
  for(uint i=0; i < v.size(); i++){
    for(uint j=0; j < v[i].size(); j++){
      if(v[i][j] < lminV) { lminV = v[i][j]; }
      if(v[i][j] > lmaxV) { lmaxV = v[i][j]; }
    }
  }

  float range = lmaxV - lminV;
  float fh = (float)h;

  if(exIndex.size() != v[0].size()){
    emit plotterStatusMessage(QString("exptIndex size is different from values[0] size. Can't really draw!!"));
    return;
  }

  // draw some vertical lines.. and some labels..
//  p->setFont(QFont("Arial", 7));
  //  p->setFont(QFont("helvetica", 8));
  p->setFont(font());
  QString tick_label;
  //p.setPen(QPen(QColor(150, 150, 150), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
  p->setPen(QPen(QColor(0, 0, 0), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
  // first check how much space is required to print the highest value..

  tick_label.setNum(exIndex[0]);
  p->drawText(rxo + xo - 10, yo+h+4, 20, 10, AlignCenter, tick_label);

  for(uint i=1; i < exIndex.size(); i++){
    int x = rxo + xo + (i*w)/(exIndex.size()-1);
    p->setPen(QPen(QColor(150, 150, 150), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
    p->drawLine(x, yo+h, x, yo);
    tick_label.setNum(exIndex[i]);
    p->setPen(QPen(QColor(0, 0, 0), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
    p->drawText(x-10, yo+h+4, 20, 10, AlignCenter, tick_label);
  }
  p->setPen(QPen(QColor(0, 0, 0), penWidth, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));

  // lets draw an x -axis..
  y = yo+h - (int)((0-lminV)*fh/range);
  cout << "\tx-axis y value : " << y << endl;
  p->drawLine(rxo + xo, y, rxo+xo+w, y);
  // and a y -axis
  p->drawLine(rxo+xo, yo+h, rxo+xo, yo);  // should be.. 
  // some labels and ticks..

  float tick_value;
  for(int i=0; i <= ticknumber; i++){
    tick_value = i*(range/ticknumber);
    tick_label.sprintf("%1.1e", tick_value+lminV);
    y = yo+h - (int)((tick_value)*fh/range);
    p->drawText(xo-2, y-8, rxo, 16, AlignRight, tick_label);
    p->drawLine(rxo+xo, y, rxo+xo+10, y);
  }

  for(uint i=0; i < v.size(); i++){
    int mx = v[i].size()-1;
    p->setPen(QPen(*colours[i % colours.size()], penWidth, PenStyle(SolidLine), PenCapStyle(RoundCap), PenJoinStyle(MiterJoin)));
    //    p.setPen(QPen(*colours[i % colours.size()], penWidth));
    for(uint j=0; j < v[i].size()-1; j++){
      x1 = rxo+xo + (j*w)/mx;
      x2 = rxo+xo + ((j+1)*w)/mx;
      y1 = yo+h - (int)((v[i][j]-lminV)*fh/range);
      y2 = yo+h - (int)((v[i][j+1]-lminV)*fh/range);
      p->drawLine(x1, y1, x2, y2);
    }
  }
  // lets draw a thin white lint on top of the x-axis
  p->setPen(QPen(QColor(255, 255, 255), 0));
  y = yo+h - (int)((0-lminV)*fh/range);  
  p->drawLine(rxo+xo, y, rxo+xo+w, y);
  //  p.end();
  //p.begin(this);
  //p.drawPixmap(0, 0, pix);
}

void ExpressionPlotter::setMinMax(vector< vector<float> >& v){
  if(!v.size()){
    minV = 0;
    maxV = 1;       // avoids any divide by 0 problems, .. if I do the wrong thing
    return;
  }
  if(!v[0].size()){
    minV = 0;
    maxV = 1;
    return;
  }
  maxV = v[0][0];
  //minV = v[0][0];
  minV = 0;
  for(uint i=0; i < v.size(); i++){
    for(uint j=0; j < v[i].size(); j++){
      if(v[i][j] < minV) { minV = v[i][j]; }
      if(v[i][j] > maxV) { maxV = v[i][j]; }
    }
  }
}

void ExpressionPlotter::setColours(){
  int base=55;
  for(int i=colours.size(); i < 100; i++){
    colours.push_back(new QColor());
    colours[i]->setHsv(((i-1)*base+154) % 360, 255, 220);
  }
}

void ExpressionPlotter::setPenWidth(int width){
  penWidth = width;
  update();
}

void ExpressionPlotter::copyImage(){
  if(!pixmap){
    return;
  }
  QClipboard *cb = QApplication::clipboard();
  //  QPixmap pmap;
  //pmap = *pixmap;
  cb->setPixmap(*pixmap, QClipboard::Clipboard);
}

void ExpressionPlotter::mousePressEvent(QMouseEvent* e){
  if(e->button() == Qt::RightButton){
    menu->popup(mapToGlobal(e->pos()));
  }
  if(e->button() == Qt::LeftButton){
      showExpDetails(e->x(), e->y());
  }
}

void ExpressionPlotter::mouseMoveEvent(QMouseEvent* e){
    // but don't doo this with mouseTracking .. as it will be pretty horrible.. 
    if(e->state() & Qt::LeftButton)
	showExpDetails(e->x(), e->y());
}

void ExpressionPlotter::showExpDetails(int x, int y){
    if(x > xo + rxo && x < width() - xo && y > yo && y < height() - yo){
	// let's see if we can work out which experiment we are looking at ..
	float w = float( width() - (2 * xo + rxo) );
	float xp = float(exptIndex.size() - 1) *  float(x - (xo + rxo)) / w;
	uint expI = (uint)floor(xp + 0.5); // since windows doesn't appear to have lroundf
//	uint expI = (uint)lroundf(xp);
 	if(expI < exptIndex.size()){
	    if(exptIndex[expI] != currentExperiment){
		currentExperiment = exptIndex[expI];
		emit showExperimentDetails(currentExperiment);
	    }
	}
    }
}

void ExpressionPlotter::setProbePairThreshold(float t){
  threshold = t;
  paintLines();
}

void ExpressionPlotter::selectFont(){
    QFont f = QFontDialog::getFont( 0, font() );
    emit selectedNewFont(f);

    cout << "emitted selectedNewFont " << f.family() << " : " << f.pointSize() << endl;
    
//    emit selectedNewFont(QFontDialog::getFont( 0, font()));
//  setFont( QFontDialog::getFont( 0, font() ) );
//  update();
}

void ExpressionPlotter::exportPS(){
    cout << "calling emit exportEPS and hoping taht something will happen" << endl;
    emit exportEPS(plotType);
}

