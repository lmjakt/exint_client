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

#include "traceViewer.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <iostream>

using namespace std;

TraceViewer::TraceViewer(std::vector<tracePoint> pts, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  QPushButton* showButton = new QPushButton("Show", this, "showButton");
  QPushButton* deleteButton = new QPushButton("Delete", this, "deleteButton");

  connect(showButton, SIGNAL(clicked()), this, SLOT(showDrawing()) );
  connect(deleteButton, SIGNAL(clicked()), this, SIGNAL(deleteMe()) );

  QHBoxLayout* vbox = new QHBoxLayout(this);
  vbox->addWidget(deleteButton);
  vbox->addWidget(showButton);

  drawer = new TraceDrawer(pts);
  drawer->resize(400, 400);
  drawer->show();
}

TraceViewer::~TraceViewer(){
  delete drawer;
}

void TraceViewer::showDrawing(){
  drawer->raise();
  drawer->show();
}


