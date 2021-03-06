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

#include "experimentWidget.h"
#include "../exptinfo.h"
#include <qwidget.h>
#include <qapplication.h>
#include <qlineedit.h>
//#include "../smallToggle/smallToggle.h"
#include <qstring.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qfont.h>
#include <map>
#include <iostream>

using namespace std;

ExperimentWidget::ExperimentWidget(exptInfo e, QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  eInfo = e;
  QString indexNo;
  indexNo.setNum(eInfo.dbaseIndex);
  QLabel* index = new QLabel(this);
  index->setText(indexNo);
  QFont fnt = qApp->font();
  index->setFont(fnt);
  //  index->setFont(QFont("helvetica", 8));
  index->setMinimumWidth(20);
  active = true;

  QLabel* shortName = new QLabel(this);
  //  QLineEdit* shortName = new QLineEdit(this);
  shortName->setText(QString(eInfo.shortName.c_str()));
  shortName->setFont(fnt);
  //shortName->setFont(QFont("helvetica", 8));

  includeButton = new SmallToggle(1, this);
  includeButton->setOn(true);
  connect(includeButton, SIGNAL(toggled(bool)), this, SLOT(includeButtonToggled(bool)));
  
  markButton = new SmallToggle(1, this);
  markButton->setOn(false);
  markMe = false;
  connect(markButton, SIGNAL(toggled(bool)), this, SLOT(markButtonToggled(bool)) );

  QHBoxLayout* box = new QHBoxLayout(this);

  box->insertSpacing(0, 5);
  box->addWidget(index);
  box->insertSpacing(2, 4);
  box->addWidget(shortName);
  box->insertStretch(4, 1);
  box->addWidget(includeButton);
  box->addWidget(markButton);

  map<int, bool>::iterator it;
  for(it = eInfo.chips.begin(); it != eInfo.chips.end(); it++){
    QWidget* flag = new QWidget(this, "flag");
    flag->setFixedWidth(15);
    if((*it).second){
      flag->setBackgroundColor(QColor(180, 0, 180));   // really red like
    }else{
      flag->setBackgroundColor(QColor(250, 250, 200)); // kind of a weak grey..
    }
    box->addWidget(flag);
  }
}

void ExperimentWidget::includeButtonToggled(bool inc){
  //make sure that the correct info is set for the toggle button.. 
  // this takes a toggleState... as a thing..
  //cout << "emitting include Me .. " << endl;
  active = inc;
  includeButton->setActivity(inc);
  emit includeMe(eInfo.index, inc);
  emit includeMyIndex(eInfo.dbaseIndex, inc);
}

void ExperimentWidget::excludeIfNoChip(int chip){
    map<int, bool>::iterator it = eInfo.chips.find(chip);
    if(it == eInfo.chips.end()){
	cerr << "This experiment does not know about chip : " << endl;
	return;
    }
    if((*it).second){   // we have data, don't care..
	return;
    }
    includeButton->setActivity(false);      // this does not result in a change of state being registered.. 
    active=false;
}

void ExperimentWidget::setState(bool state){
    includeButton->setActivity(state);
    active = state;
}

void ExperimentWidget::markButtonToggled(bool mark){
    markMe = mark;
    markButton->setActivity(mark);
    emit markMyIndex(eInfo.dbaseIndex, mark);
}
