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
#include "../exptinfo.h"
#include "../experimentWidget/experimentWidget.h"
#include <qwidget.h>
#include <qscrollview.h>
#include <qlayout.h>
#include <qframe.h>
#include <qsize.h>
#include <qlabel.h>
//#include <qscrollbar.h>
#include <vector>
#include <iostream>
#include <qpushbutton.h>

using namespace std;

ExperimentChooser::ExperimentChooser(QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  updateOrderChooser = true;

  QVBoxLayout* base = new QVBoxLayout(this);
  setCaption("Samples");

  QScrollView* scroll = new QScrollView(this);
  frame = new QFrame(scroll->viewport());
  scroll->addChild(frame);

  grid = new QVBoxLayout(frame);
  base->addWidget(scroll);
  resize(300, 300);

  orderChooser = new OrderWindow();
  connect(orderChooser, SIGNAL(setExptOrder(multimap<float, exptInfo>)), this, SLOT(updateExperimentOrder(multimap<float, exptInfo>)) );
  connect(orderChooser, SIGNAL(changeActivityStates(map<int, bool>)), this, SLOT(setActivityStates(map<int, bool>)) );

  sampleMemory = new SampleMemoryWidget();
  sampleMemory->resize(300, 200);
  connect(sampleMemory, SIGNAL(displayThese(std::set<int>, std::set<int>)), this, SLOT(setFromMemory(std::set<int>, std::set<int>)) );
  // and a button to add stuff to the memory widget
  QPushButton* rememberButton = new QPushButton("Remember", this, "rememberButton");
  connect(rememberButton, SIGNAL(clicked()), this, SLOT(rememberSelection()) );
  sampleSetLabel = new QLineEdit(this, "sampleSetLabel");

  QPushButton* readFileButton = new QPushButton("From file", this, "readFileButton");
  connect(readFileButton, SIGNAL(clicked()), this, SLOT(readFromFile()) );

  QPushButton* showMemoryButton = new QPushButton("Memory", this, "showMemoryButton");
  connect(showMemoryButton, SIGNAL(clicked()), sampleMemory, SLOT(show()) );

  chipGroup = new QButtonGroup();
  connect(chipGroup, SIGNAL(clicked(int)), this, SLOT(excludeIfNoChip(int)) );
//  chipGroup = new QButtonGroup("Mandatory", this, "chipGroups");
  QPushButton* allActiveButton = new QPushButton("All Active", this, "allActiveButton");
  QLabel* chipLabel = new QLabel("By Chip", this, "chipLabel");
  connect(allActiveButton, SIGNAL(clicked()), this, SLOT(setAllActive()) );
  hbox = new QHBoxLayout(base);

  hbox->addWidget(chipLabel);
  hbox->addStretch();
  hbox->addWidget(allActiveButton);

  QHBoxLayout* sampleMemBox = new QHBoxLayout(base);
  sampleMemBox->addWidget(showMemoryButton);
  sampleMemBox->addWidget(readFileButton);
  sampleMemBox->addWidget(rememberButton);
  sampleMemBox->addWidget(sampleSetLabel);

  // and a button,,
  QPushButton* orderButton = new QPushButton("Set Order", this, "orderButton");
  connect(orderButton, SIGNAL(clicked()), orderChooser, SLOT(show()) );
  base->addWidget(orderButton);
  
}

void ExperimentChooser::updateExperiments(map<float, exptInfo> info){
  multimap<float, exptInfo> emap;
  for(map<float, exptInfo>::iterator it=info.begin(); it != info.end(); it++){
    emap.insert((*it));  // which is the pair..
  }
  updateExperiments(emap);
}

void ExperimentChooser::updateExperiments(multimap<float, exptInfo> info){
  // first go through, and delete all the current choices, 
  // first calling hide() to, hopefully get a nice response,,
  // then go through and create new experimentWidgets with the exptInfo.. 
  // structs..
  bool wasVisible = isVisible();
  if(wasVisible) { hide(); }
  for(uint i=0; i < eWidgets.size(); i++){
    eWidgets[i]->hide();
    delete eWidgets[i];
  }
  eWidgets.resize(0);
  eWidgets.reserve(info.size());
  chips.erase(chips.begin(), chips.end());
  for(uint i=0; i < excludeButtons.size(); i++){
      excludeButtons[i]->hide();
      delete(excludeButtons[i]);
  }
  excludeButtons.resize(0);
  multimap<float, exptInfo>::iterator it;
  vector<int> tempSelection;             // signal to update the vector.. 
  for(it = info.begin(); it != info.end(); it++){
      cout << "Experiment : " << (*it).second.shortName << "\t" << (*it).second.dbaseIndex << "\t" << (*it).second.index << endl;
    eWidgets.push_back(new ExperimentWidget((*it).second, frame));
    grid->addWidget(eWidgets[eWidgets.size()-1]);
    connect(eWidgets[eWidgets.size()-1], SIGNAL(includeMyIndex(int, bool)), this, SLOT(selectionChanging(int, bool)) );
    connect(eWidgets[eWidgets.size()-1], SIGNAL(markMyIndex(int, bool)), this, SLOT(markSelectionChanging(int, bool)) );
    tempSelection.push_back((*it).second.dbaseIndex);
    map<int, bool>::iterator bit;
    for(bit = (*it).second.chips.begin(); bit != (*it).second.chips.end(); bit++){
	chips.insert((*bit).first);
	cout << "\t" << (*bit).first << ":" << (*bit).second;
    }
    cout << endl;
    // do some connects later on..
  }
  // Emit the index, and hope that it connects to the vector in client.. 
  emit newExptSelection(tempSelection);    // later -- reconstruct and emit this each time it changes.. 
  QSize fSize = frame->sizeHint();
  cout << "frame height: " << fSize.height() << endl 
       << "frame width : " << fSize.width() << endl;
  cout << "my Parents address is : " << this->parent() << endl;
  resize(fSize.width()+18, 300);

  // and then lets make some buttons and insert them in the place I like.. 
  set<int>::iterator sit;
  for(sit = chips.begin(); sit != chips.end(); sit++){
      QString num;
      num.setNum((*sit));
      QPushButton* button = new QPushButton(num, this);
      button->setFixedWidth(25);
      hbox->addWidget(button);
      button->show();
      chipGroup->insert(button, (*sit));
      excludeButtons.push_back(button);
      // and connect .. to the relevant thingy.. 
     
  }
      
  if(wasVisible) { show(); }
  
  // and update the orderWindow..
  if(updateOrderChooser){
    orderChooser->setExperiments(info);
  }
  
}

void ExperimentChooser::updateExperimentOrder(multimap<float, exptInfo> em){
  updateOrderChooser = false;
  updateExperiments(em);
  updateOrderChooser = true;
}

void ExperimentChooser::excludeIfNoChip(int chip){
    for(uint i=0; i < eWidgets.size(); i++){
	eWidgets[i]->excludeIfNoChip(chip);
    }
    selectionChanging(0, false);   // ignores the values..
}

void ExperimentChooser::setAllActive(){ 
    for(uint i=0; i < eWidgets.size(); i++){
	eWidgets[i]->setState(true);
    }
    selectionChanging(0, false);
}   

void ExperimentChooser::rememberSelection(){
    set<int> active;
    set<int> marks;
    for(vector<ExperimentWidget*>::iterator it=eWidgets.begin(); it != eWidgets.end(); it++){
	if((*it)->active)
	    active.insert((*it)->eInfo.dbaseIndex);
	if((*it)->markMe)
	    marks.insert((*it)->eInfo.dbaseIndex);
    }
    cout << "rememberSelection active " << active.size() << "  marks " << marks.size() << endl;
    QString labText = sampleSetLabel->text();
    if(labText.length()){
	sampleMemory->rememberStates(active, marks, labText);
    }else{
	sampleMemory->rememberStates(active, marks);
    }
    sampleMemory->show();
}

void ExperimentChooser::readFromFile(){
    sampleMemory->readSetFromFile();
    sampleMemory->show();
}

void ExperimentChooser::setFromMemory(std::set<int> active, std::set<int> marks){
      for(vector<ExperimentWidget*>::iterator it=eWidgets.begin(); it != eWidgets.end(); it++){
	  int dbi = (*it)->eInfo.dbaseIndex;
	  (*it)->blockSignals(true);
	  (*it)->includeButtonToggled( active.count(dbi) );
	  (*it)->markButtonToggled( marks.count(dbi) );
	  (*it)->blockSignals(false);
      }
      selectionChanging(0, false);
      markSelectionChanging(0, false);
}

void ExperimentChooser::setActivityStates(map<int, bool> states){
  map<int, bool>::iterator bit;
  for(vector<ExperimentWidget*>::iterator it=eWidgets.begin(); it != eWidgets.end(); it++){
    bit = states.find((*it)->eInfo.dbaseIndex);
    if(bit == states.end()){
      continue;
    }
    if((*bit).second != (*it)->active){
      (*it)->includeButtonToggled((*bit).second);
    }
  }
}

/// Not sure why, but we don't seem to use the n or the boolean below.. ?
void ExperimentChooser::selectionChanging(int n, bool b){  // should be able to use this in a better manner.. 
  vector<int> selection;
  for(uint i=0; i < eWidgets.size(); i++){
    if(eWidgets[i]->active){
      selection.push_back(eWidgets[i]->eInfo.dbaseIndex);
    }
  }
  cout << endl << "size of newExptSelection  " << selection.size() << " " << n << "  " << b << endl;
  emit newExptSelection(selection);     // hmm, probably OK for now. I shouldn't have had that bun thing, has stopped my brain from functioning
}

void ExperimentChooser::markSelectionChanging(int, bool){
  vector<int> selection;
  for(uint i=0; i < eWidgets.size(); i++){
    if(eWidgets[i]->markMe){
      selection.push_back(eWidgets[i]->eInfo.dbaseIndex);
    }
  }
  emit newMarkSelection(selection);    
}
