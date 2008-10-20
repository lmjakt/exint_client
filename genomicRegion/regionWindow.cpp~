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

#include "regionWindow.h"
#include "regionListItem.h"
#include "../client.h"
#include <qwidget.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
//#include <qvalidator.h>
#include <iostream>
#include <string>
#include <set>
#include <vector>

using namespace std;

RegionWindow::RegionWindow(set<chromRegion*>* cr, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  regions = cr;

  // and then some elements
  chromLine = new QLineEdit(this, "chromLine");
  QPushButton* getChrom = new QPushButton("Chromosome", this, "getChrom");
  connect(getChrom, SIGNAL(clicked()), this, SLOT(getChromosome()) );
  connect(chromLine, SIGNAL(returnPressed()), this, SLOT(getChromosome()) );
  
  regionList = new QListView(this, "regionList");
  regionList->addColumn("Chromosome");
  regionList->addColumn("Matches");
  regionList->setAllColumnsShowFocus(true);
  connect(regionList, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(selectionChanged(QListViewItem*)) );
  
  maxExpect = new QSpinBox(-500, 0, 1, this, "minExpect");
  maxExpect->setValue(0);    
  minAllength = new QSpinBox(0, 1000, 1, this, "minAllength");
  minMatch = new QSpinBox(0, 1000, 1, this, "minMatch");

  // a couple of QLineEdits for showing and entering the selection thingy.. 
  selectStart = new QSpinBox(0, 0, 1, this);
  selectEnd = new QSpinBox(0, 0, 1, this);
  connect(selectStart, SIGNAL(valueChanged(int)), this, SLOT(changingRegion(int)) );
  connect(selectEnd, SIGNAL(valueChanged(int)), this, SLOT(changingRegion(int)) );

  //  selectStart->setBackgroundColor(QColor(0, 0, 0));
  //selectStart->setForegroundColor(QColor(255, 255, 255));
 

  //startValidator = new QIntValidator(1, 1, this);
  //endValidator = new QIntValidator(1, 1, this);
  //selectStart->setValidator(startValidator);
  //selectEnd->setValidator(endValidator);

  //regionWidget = new RegionWidget();
  /// NEED to replace or rewrite this .. try to replace with local Region View.. 

//   regionWidget = new RegionWidget(this, "regionWidget");
//   connect(regionWidget, SIGNAL(pleaseLoadGenes(vector<int>, QString)), this, SIGNAL(pleaseLoadGenes(vector<int>, QString)) );
//   connect(regionWidget, SIGNAL(selectRegionChanged(int, int, int, int)), this, SLOT(selectRegionChanged(int, int, int, int)) );
//   connect(regionWidget, SIGNAL(pleaseGetEnsemblGenes(int, string, int, int)), this, SIGNAL(pleaseGetEnsemblGenes(int, string, int, int)) );
//   connect(maxExpect, SIGNAL(valueChanged(int)), regionWidget, SLOT(setMaxExp(int)));
//   connect(minAllength, SIGNAL(valueChanged(int)), regionWidget, SLOT(setMinAll(int)) );
//   connect(minMatch, SIGNAL(valueChanged(int)), regionWidget, SLOT(setMinMatch(int)) );
  
//   //regionWidget->show();
//   regionWidget->resize(500, 100);

  // and let's have a simple layout..
  
  QGridLayout* grid = new QGridLayout(this, 4, 4, 5, 3, "grid");
  
  grid->setColStretch(3, 1);
  grid->addWidget(getChrom, 0, 0);
  grid->addMultiCellWidget(chromLine, 0, 0, 1, 2);
  
  grid->addWidget(maxExpect, 1, 0);
  grid->addWidget(minAllength, 1, 1);
  grid->addWidget(minMatch, 1, 2);

  QHBoxLayout* hbox = new QHBoxLayout();
  grid->addMultiCellLayout(hbox, 2, 2, 0, 2);
  hbox->addWidget(selectStart);
  hbox->addWidget(selectEnd);
  
  grid->addMultiCellWidget(regionList, 3, 3, 0, 2);

  QVBoxLayout* vbox = new QVBoxLayout();
  grid->addMultiCellLayout(vbox, 0, 3, 3, 3);
  //  grid->addMultiCellWidget(regionWidget, 0, 1, 3, 3);
  
  //vbox->addWidget(regionWidget);
  //  QWidget* spaceItem = new QWidget(this);
  //spaceItem->setEraseColor(QColor(0, 0, 0));
  //hbox->addWidget(spaceItem);
  //  hbox->addStretch();
  //hbox->setStretchFactor(spaceItem, 1);

  resize(600, 100);
}

void RegionWindow::getChromosome(){
  QString chr = chromLine->text();
  if(!chr.length()){
    cout << "string is null returning" << endl;
    return;
  }
  emit getRegion(chr.latin1(), -1, -1);
}

void RegionWindow::receiveChromosome(chromRegion* cr){
  // check to make sure it is in the regions set..
  if(!regions->count(cr)){
    cout << "no such region known by client" << endl;
    return;
  }
  RegionListItem* item = new RegionListItem(cr, regions->size(), regionList);
  //regionWidget->setData(maxExpect->value(), minAllength->value(), minMatch->value(), cr);
}

void RegionWindow::selectionChanged(QListViewItem* item){
  if(item->rtti() == 234579){
    //regionWidget->setData(maxExpect->value(), minAllength->value(), minMatch->value(), ((RegionListItem*)item)->chRegion());
  }
}

void RegionWindow::selectRegionChanged(int cs, int ce, int ss, int se){
  selectStart->blockSignals(true);
  selectEnd->blockSignals(true);
  selectStart->setRange(cs, se);
  selectEnd->setRange(ss, ce);
  selectStart->setValue(ss);
  selectEnd->setValue(se);
  selectStart->blockSignals(false);
  selectEnd->blockSignals(false);
}
  
void RegionWindow::changingRegion(int i){
  selectStart->setMaxValue(selectEnd->value());
  selectEnd->setMinValue(selectStart->value());
  //regionWidget->setSelectRegion(selectStart->value(), selectEnd->value());
}

void RegionWindow::updateWidget(){
  //regionWidget->update();
}
// and that is all i need for now I think.. 
