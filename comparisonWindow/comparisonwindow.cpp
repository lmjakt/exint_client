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

#include "comparisonwindow.h"
#include "../smallToggle/smallToggle.h"
#include "../stat/stat.h"
#include "../exptinfo.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qstring.h>
#include <qcanvas.h>
#include <qgroupbox.h>
#include <iostream>



ComparisonWindow::ComparisonWindow(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  // do nothing..

}

ComparisonWindow::ComparisonWindow(map<float, exptInfo>* em, QWidget* parent, const char *name)
  : QWidget(parent, name)
{
  setCaption("Comparison");
  eMap = em;
  //  int exp_number = eMap->size();
  //int iCanvasWidth = exp_number * 20;
  //int iCanvasHeight = 200;

  inputWindow = new CompInputWindow(eMap, this, "inputWindow");
  inputWindow->setMinimumWidth(700);
  //  inputWindow->show();

  // And some buttons for different Actions.
  QPushButton* rawCompare = new QPushButton("Raw Compare", this, "rawCompare");
  connect(rawCompare, SIGNAL(clicked()), this, SLOT(rawComparison()) );
  QPushButton* meanCompare = new QPushButton("Mean Compare", this, "meanCompare");
  connect(meanCompare, SIGNAL(clicked()), this, SLOT(meanComparison()));
 
  QButtonGroup* selectors = new QButtonGroup(2, Vertical, QString("Data Set"), this);
  getDist = new QCheckBox("Distribution", this, "getDist");
  ptSize = new QSpinBox(2, 20, 2, this, "ptSize");
  connect(ptSize, SIGNAL(valueChanged(int)), inputWindow, SLOT(setPointSize(int)) );
  ptSize->setValue(12);

  selector.push_back(new QRadioButton(QString("raw"), selectors));
  selector.push_back(new QRadioButton(QString("z-score"), selectors));
  selector[1]->setChecked(true);



  QGridLayout* grid = new QGridLayout(this, 1, 2, 5);
  grid->setColStretch(0, 0);
  grid->setColStretch(1, 1);

  QVBoxLayout* topLeft = new QVBoxLayout();
  grid->addWidget(inputWindow, 0, 1);

  grid->addLayout(topLeft, 0, 0);
  topLeft->addWidget(rawCompare);
  topLeft->addWidget(meanCompare);
  topLeft->addWidget(selectors);
  topLeft->addWidget(getDist);
  topLeft->addWidget(ptSize);
  topLeft->addStretch();

}






ComparisonWindow::~ComparisonWindow(){
}




void ComparisonWindow::meanComparison(){

  exptPoints pts = inputWindow->currentPoints();
  zScore(pts.values);   // better algorithm.
  // determine the normed value..
  bool normed = selector[1]->isChecked();    // radio buttons.. so it's ok.. 
  emit doMeanComparison(pts.values, pts.indices, normed, getDist->isChecked());
}

void ComparisonWindow::rawComparison(){

  exptPoints pts = inputWindow->currentPoints();
  

  zScore(pts.values);   // better algorithm.
  emit doRawComparison(pts.values, pts.indices, getDist->isChecked());
}

void ComparisonWindow::setActive(map<int, bool> cmap){   // chooses experiments on the basis of which chips are selected. 
  cout << "comparison window set active .. " << endl;
  inputWindow->setActive(cmap);
}

void ComparisonWindow::setActive(vector<int> samples, int id){
    inputWindow->setActive(samples);
}
