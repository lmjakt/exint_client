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

#include "stressPlotter.h"
#include <qpainter.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <iostream>

using namespace std;

StressPlotter::StressPlotter(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    // don't do very much..
    setBackgroundMode(NoBackground);
    setCaption("Stress Plotter");
    maxValue = 0;
    minValue = 0;
    resize(300, 300);
}

void StressPlotter::setData(vector<float> stress){
    values = stress;
    if(values.size())
	minValue = values[0];
    for(uint i=0; i < values.size(); ++i){
	maxValue = maxValue > values[i] ? maxValue : values[i];
	minValue = minValue < values[i] ? minValue : values[i];
    }
    update();
}

void StressPlotter::paintEvent(QPaintEvent* e){
    int w = width();
    int h = height();
    float r = maxValue - minValue;
    r = (r == 0) ? maxValue : r;
    // draw some kind of background..
    QPixmap pix(w, h);
    pix.fill(QColor(0, 0, 0));
    QPainter p(&pix);
    p.setPen(QPen(QColor(255, 255, 255), 1));
    p.setBrush(Qt::NoBrush);
    for(uint i=0; i < values.size(); ++i){
	if(!values[i])
	    continue;
	int x = (w * i) / values.size();
	int y =  h - (float(h) *  values[i] / maxValue);
	p.drawEllipse(x, y, 4, 4);
//	if(values[i])
//	    cout << "stress : " << values[i] << " p: " << i << "  --> " << y << ", " << x << endl;
	
    }
    bitBlt(this, 0, 0, &pix, 0, 0);
}

