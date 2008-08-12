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

#ifndef KEYWINDOW_H
#define KEYWINDOW_H

#include <qwidget.h>
#include <qcolor.h>
#include <qstring.h>
#include <vector>

using namespace std;

class KeyWindow : public QWidget
{
  Q_OBJECT
    public : 
    KeyWindow(vector<QColor*> c, QWidget* parent=0, const char* name=0);
  
  void paintKeys(vector<QString> k);
  void clearAll();

  public slots:
    void setPenWidth(int n);
  
 private:
  vector<QColor*> colours;
  vector<QString> keys;
  int penWidth;

  void paintEvent(QPaintEvent* e);
  void paintKeys();      // overloaded for repaints.. 
};

#endif
