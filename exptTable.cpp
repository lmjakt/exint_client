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

#include "experimentChooser.h"
#include "exptTable.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

ExptTable::ExptTable(QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  cout << "in the empty constructor" << endl;
}

ExptTable::~ExptTable(){
}
  
ExptTable::ExptTable(map<float, exptInfo> info, QWidget* parent, const char* name) :
  QWidget(parent, name)
{
 cout << "in Experiment Choices setting choices" << endl;
  exptMap = info;   
  int counter = 0;
  /// we need to determine the number of chips. As this should be the same for all it might be OK.
  map<float, exptInfo>::iterator it;
  it = exptMap.begin();
  int chipNo = (*it).second.chips.size();
  
  cout << "chipNo determined to be : " << chipNo << endl;
  
  QVBoxLayout* vBox = new QVBoxLayout(this); 
  QGridLayout* grid = new QGridLayout(exptMap.size()+2, chipNo+4, 5, 0);
  cout << "gridlayout constructed" << endl;
  vBox->addLayout(grid);
  cout << "gridlayout added to vbox " << endl;
  int padding = 10;
  grid->addColSpacing(0, padding);
  grid->addColSpacing(2, padding);
  grid->setColStretch(chipNo+3, 1);
  //box->addLayout(vbox);
  grid->setSpacing(0);
  vBox->addStretch();

  QPushButton* accept = new QPushButton("accept", this, "accept");
  connect(accept, SIGNAL(clicked()), this, SLOT(acceptChoices()) );
  vBox->addWidget(accept);

  QLabel* exptDes = new QLabel(QString("Experiment Description"), this);
  grid->addMultiCellWidget(exptDes, 0, 1, 1, 1, Qt::AlignCenter); 
  QLabel* chip = new QLabel(QString("Chip"), this);
  grid->addMultiCellWidget(chip, 0, 0, 3, 4, Qt::AlignCenter);
  for(int i=0; i < chipNo; i++){
    QString num;
    num.setNum(i+1);
    QLabel* cnum = new QLabel(num, this);
    grid->addWidget(cnum, 1, i+3);
  }

  for(it = exptMap.begin(); it != exptMap.end(); it++){
    //cout << "going through the epximerental map" << endl;
    QLabel* label = new QLabel(QString((*it).second.description.c_str()), this);
    label->setFont(QFont("helvetica", 9));
    label->setAlignment(AlignBottom);
    grid->addWidget(label, counter+2, 1);
    /// Need to change this to use an iterator..
    map<int, bool>:: iterator cit;       // for chip iterator !!
    int chipCounter = 0;
    for(cit = (*it).second.chips.begin(); cit != (*it).second.chips.end(); cit++){
      //      QToolButton* box = new QToolButton(this);
      SmallToggle* box = new SmallToggle((*it).second.dbaseIndex, this);
      (*it).second.chipButtons.push_back(box);         // in this way I can refer to it as box for the initialisation.. 
      box->setToggleButton(true);
      //box->setOnIconSet(QIconSet(QPixmap("active_2_25.xpm")));
      //box->setOffIconSet(QIconSet(QPixmap("inactive_25.xpm")));
      box->setFixedSize(20, 20);
      //cout << "experiment: " << (*it).first << "\tchip: " << (*cit).first << "\ton: " << (*cit).second << endl;
      if(!(*cit).second){
	box->setDisabled(true);
      }else{
	box->setOn(true);
      }
      grid->addWidget(box, counter+2, chipCounter+3);
      chipCounter++;
    }
    counter++;
  }
  setPalette(QPalette(QColor(200, 200, 255)));
}

void ExptTable::acceptChoices(){
  cout << "Am in the expt table accepting choices, allellulaill" << endl;
}
