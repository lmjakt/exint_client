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
#include <qapplication.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <vector>
#include <map>
#include <set>
#include <math.h>
#include <iostream>
#include <set>
#include <qclipboard.h>
#include <qfontdialog.h>
#include "meanExpressionPlotter.h"
#include "../exptinfo.h"

using namespace std;

MeanExpressionPlotter::MeanExpressionPlotter(QWidget *parent, const char* name)
  : QWidget(parent, name)
{
  cout << "at the beginning of Expression Plotter" << endl;
  resize(300, 300);
  setBackgroundMode(NoBackground);
  setPalette(QPalette(QColor(250, 250,200)));
  setColours();
  penWidth = 2;
  hasData = false;

  pixmap = new QPixmap(size());

  menu = new QPopupMenu(this, "menu");
  menu->insertItem("Copy", this, SLOT(copyImage()) );
  menu->insertItem("Set Font", this, SLOT(selectFont()) );
  setFont(qApp->font().family(), qApp->font().pointSize() - 2);
  cout << "at the end of Expression Plotter constructor" << endl;
}

void MeanExpressionPlotter::paintEvent(QPaintEvent *e){
  //  if(hasData){
    paintLines();
    //}
  //repaint();
  //  update();
}

void MeanExpressionPlotter::paintLines(vector< vector<float> >& v, vector< vector<int> >& exIndex, map<float, exptInfo>* em){
    values = v;
    exptIndex = exIndex;
    eMap = em;
    setMinMax(v);
    hasData = true;
    paintLines();
}

void MeanExpressionPlotter::paintLines(){
  /////////////// unfortunately it seems we have to do this every time to deal with a bug in Borland
  //////////////  commercial compilers.. well there you go..
    
    pixmap->resize(width(), height());
//  QPixmap pix(width(), height());
    //  pix.fill(this, QPoint(0, 0));
    pixmap->fill(QColor(250, 250,200));
    QPainter p(pixmap);
    
    if(hasData){
	set<int> temp;
	for(int i=0; i < exptIndex.size(); i++){
	    for(int j=0; j < exptIndex[i].size(); j++){
		temp.insert(exptIndex[i][j]);
	    }
	}
	map<int, int> allExperiments;
	set<int>::iterator sit;
	map<float, exptInfo>::iterator mit;
	int counter = 0;
	for(mit = (*eMap).begin(); mit != (*eMap).end(); mit++){
	    //  for(sit = temp.begin(); sit != temp.end(); sit++){
	    if(temp.count((*mit).second.dbaseIndex) > 0){
		allExperiments.insert(make_pair((*mit).second.dbaseIndex, counter));
		counter++;
	    }
	}
	
	int xo=20;
	int yo=20;
	int rxo=50;
	int w = width()-2*xo-rxo;
	int h = height()-2*yo;
	int ticknumber = 5;
	
	
	p.setPen(QPen(QColor(200, 0, 0), penWidth));
	int x1, x2, y1, y2, y;
	float range = maxV - minV;
	float fh = (float)h;
	
	if(exptIndex.size() != values.size()){
	    emit plotterStatusMessage(QString("exptIndex size is different from values[0] size. Can't really draw!!"));
	    return;
	}
	
	// draw some vertical lines.. and some labels..
	int pointsize = qApp->font().pointSize() -1 ;
	p.setFont(font());
	QString tick_label;
	p.setPen(QPen(QColor(150, 150, 150), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
	map<int, int>::iterator it;
	for(it=allExperiments.begin(); it != allExperiments.end(); it++){
	    int x = rxo + xo + ((*it).second * w)/(allExperiments.size()-1);
	    p.setPen(QPen(QColor(150, 150, 150), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
	    p.drawLine(x, yo+h, x, yo);
	    tick_label.setNum((*it).first);
	    p.setPen(QPen(QColor(0, 0, 0), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
	    p.drawText(x-10, yo+h+4, 20, 10, AlignCenter, tick_label);
	}

	p.setPen(QPen(QColor(0, 0, 0), penWidth, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
	
	// lets draw an x -axis..
	y = yo+h - (int)((0-minV)*fh/range);
	p.drawLine(rxo + xo, y, rxo+xo+w, y);
	// and a y -axis
	p.drawLine(rxo+xo, yo+h, rxo+xo, yo);  // should be.. 
	// some labels and ticks..
	
	float tick_value;
	for(int i=0; i <= ticknumber; i++){
	    tick_value = i*(range/ticknumber);
	    tick_label.sprintf("%1.1e", tick_value+minV);
	    y = yo+h - (int)((tick_value)*fh/range);
	    p.drawText(xo-2, y-8, rxo, 16, AlignRight|AlignVCenter, tick_label);
	    p.drawLine(rxo+xo, y, rxo+xo+10, y);
	}
	for(int i=0; i < values.size(); i++){
	    int mx = allExperiments.size()-1;
	    //    int mx = values[i].size()-1;
	    p.setPen(QPen(*colours[i % colours.size()], penWidth, PenStyle(SolidLine), PenCapStyle(RoundCap), PenJoinStyle(MiterJoin)));
	    //    p.setPen(QPen(*colours[i % colours.size()], penWidth));
	    if(values[i].size()){
		for(int j=0; j < values[i].size()-1; j++){
		    x1 = rxo+xo + (allExperiments[exptIndex[i][j]] * w)/mx;        //LEAP OF FAITH.. !!!!!! NOT GOOD.
		    //x1 = rxo+xo + (j*w)/mx;
		    //x2 = rxo+xo + ((j+1)*w)/mx;
		    x2 = rxo+xo + (allExperiments[exptIndex[i][j+1]] * w)/mx;
		    y1 = yo+h - (int)((values[i][j]-minV)*fh/range);
		    y2 = yo+h - (int)((values[i][j+1]-minV)*fh/range);
		    p.drawLine(x1, y1, x2, y2);
		}
	    }
	}
	// lets draw a thin white lint on top of the x-axis
	p.setPen(QPen(QColor(255, 255, 255), 0));
	y = yo+h - (int)((0-minV)*fh/range);  
	p.drawLine(rxo+xo, y, rxo+xo+w, y);
    }    
//    p.end();
    QPainter p2(this);
//    p.begin(this);
    p2.drawPixmap(0, 0, *pixmap);
}

void MeanExpressionPlotter::mousePressEvent(QMouseEvent* e){
  if(e->button() == Qt::RightButton){
    menu->popup(mapToGlobal(e->pos()));
  }
}

void MeanExpressionPlotter::setMinMax(vector< vector<float> >& v){
    // default
    minV = 0.0;
    maxV = 1.0;
    if(!v.size())
	return;
    // assign the initial value
    for(int i=0; i < v.size(); ++i){
	if(!v[i].size())
	    continue;
	maxV = v[i][0];
	minV = v[i][0];
	break;
    }
    for(int i=0; i < v.size(); i++){
	for(int j=0; j < v[i].size(); j++){
	    if(v[i][j] < minV) { minV = v[i][j]; }
	    if(v[i][j] > maxV) { maxV = v[i][j]; }
	}
    }
}

void MeanExpressionPlotter::setColours(){
  int hBase=65;
  int vBase=10;
  int h;
  int v;
  for(int i=colours.size(); i < 100; i++){
    colours.push_back(new QColor());
    h = (i*hBase) % 360;
    v = (((i+1)*vBase) % 55)+180;      // keep it high.. 
    //    cout << "colours: i " << i << "\tH: " << h << "\tV: " << v << endl;
    colours[i]->setHsv(h, 255, v);
  }
}

void MeanExpressionPlotter::setPenWidth(int width){
  penWidth = width;
  update();
}

void MeanExpressionPlotter::clearAll(){
  // clear everything, -i.e. just erase everything.
  hasData = false;
  update();
}

vector<QColor*> MeanExpressionPlotter::colourTable(){
  return(colours);
}

void MeanExpressionPlotter::copyImage(){
    QClipboard *cb = QApplication::clipboard();
    cb->setPixmap(*pixmap, QClipboard::Clipboard);
}

void MeanExpressionPlotter::selectFont(){
    QFont f = QFontDialog::getFont( 0, font() );
    emit selectedNewFont(f);
}
