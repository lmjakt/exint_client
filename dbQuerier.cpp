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

#include "dbQuerier.h"
#include <vector>
#include <string>
#include <qstring.h>
#include <qtextstream.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <iostream>

using namespace std;

DBQuerier::DBQuerier(QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  QPushButton* doIt = new QPushButton("Query", this);
  QPushButton* doGen = new QPushButton("Gen Query", this);
  menu = new QComboBox(this);
  regionMenu = new QComboBox(this);
  inLine = new QLineEdit(this);
  
  connect(doIt, SIGNAL(clicked()), this, SLOT(doQuery()));
  connect(inLine, SIGNAL(returnPressed()), this, SLOT(doQuery()));
  
  connect(doGen, SIGNAL(clicked()), this, SLOT(doGen()) );
  
  QVBoxLayout* vbox = new QVBoxLayout(this);
  QGridLayout* hbox = new QGridLayout();
  
  vbox->addWidget(inLine);
  vbox->addLayout(hbox);
  hbox->addWidget(doIt, 0, 0);
  hbox->addWidget(menu, 0, 1);
  hbox->addWidget(doGen, 1, 0);
  hbox->addWidget(regionMenu, 1, 1);
  hbox->setColStretch(1, 2);
  hbox->setColStretch(0, 1);
}

void DBQuerier::setChoices(vector<string> choices){
  menu->clear();
  for(int i=0; i < choices.size(); i++){
    menu->insertItem(QString(choices[i].c_str()));
  }
}

void DBQuerier::setRegionChoices(vector<string> choices){
  cout << "setRegion Choices in dbqueriring thingy.. " << endl;
  regionMenu->clear();
  for(int i=0; i < choices.size(); i++){
    regionMenu->insertItem(choices[i].c_str());
  }
}

void DBQuerier::doQuery(){
  QString query;
  query.setNum(menu->currentItem());
  query.prepend("#");
  query.prepend(inLine->text());
  // if the query exists..
  if(query.length() > 2){
    emit doLookup(query);
  }
}
  
void DBQuerier::doGen(){
  QString query;    // 
  QTextStream ts(query, 2);
  if(!inLine->text().length()){
    return;
  }
  int margin = 50000;
  int maxExpect = -10;
  int minLength = 30;
  float minMatch = 0.9;    
  ts << inLine->text() << "#" << regionMenu->currentItem()  << "#" << margin << "#" << margin << "#"
	<< maxExpect << "#" << minLength << "#" << minMatch << "#";   // split string likes this to be the case.. 
  emit doGenLookup(query);
}

