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

#include "listWindow.h"
#include "../probeSetWidget/probeSetWidget.h"
#include <qpoint.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qlabel.h>
#include <qstring.h>
#include <qframe.h>
#include <qsize.h>
#include <qpushbutton.h>
#include <deque>
#include <iostream>

using namespace std;

ListWindow::ListWindow(QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  // set up the QGridLayout, and create a vector of Labels,, just containing the number,, 
  // 
  //  cout << "at the beginning of listwindow" << endl;

  base = new QVBoxLayout(this);

  QScrollView* scroll = new QScrollView(this);
  frame = new QFrame(scroll->viewport());
  scroll->addChild(frame);

  grid = new QVBoxLayout(frame);
  grid->setDirection(QBoxLayout::Up);
  //cout << "set up grid and stuff, but haven't done much else" << endl;
  base->addWidget(scroll);
  resize(390, 300);
}

void ListWindow::addItem(ProbeSetWidget* pset){
  //cout << "\t\tADDING ProbeSetWidget.. arggh,,, , I HOPE !! WHAT'S GOING ON" << endl;
  pset->reparent(frame, QPoint(0, 0));
  grid->addWidget(pset);       // put at front!!!;
  pset->show();
  QSize fSize = frame->sizeHint();
  resize(fSize.width()+18, 300);
}

void ListWindow::addButton(QPushButton* button){
  button->reparent(this, QPoint(0, 0));
  base->addWidget(button);
  button->show();
}
