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

#include "keyWindow.h"
#include <qwidget.h>
#include <qapplication.h>
#include <qcolor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qfont.h>
#include <vector>

using namespace std;

KeyWindow::KeyWindow(vector<QColor*> c, QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  colours = c;
  penWidth = 3;
  /// and that is pretty much all I need to do..
}

void KeyWindow::paintEvent(QPaintEvent* e){
  paintKeys(); // just do everything there..
}

void KeyWindow::paintKeys(vector<QString> k){
  keys = k;
  paintKeys();
}

void KeyWindow::paintKeys(){
  erase();
  if(keys.size() < 1) {
    return;
  }
  int y;
  QPainter p(this);
  int pointsize = qApp->font().pointSize()-1;
  p.setFont(QFont("Helvetica", pointsize));
  // just draw straight on the widget as this should be pretty fast..
  for(int i=0; i < keys.size(); i++){
    y = (20 + i*20);
    p.setPen(QPen(QColor(0, 0, 0), 0, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
    p.drawText(30, y, keys[i]);
    p.setPen(QPen(*colours[i % colours.size()], penWidth, PenStyle(SolidLine), PenCapStyle(SquareCap), PenJoinStyle(MiterJoin)));
    p.drawLine(5, y-4, 25, y-4);
  }
} 

void KeyWindow::setPenWidth(int n){
  penWidth = n;
  update();
}

void KeyWindow::clearAll(){
  keys.resize(0);
  erase();
}
    
    
