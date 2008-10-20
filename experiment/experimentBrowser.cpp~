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

#include "experimentBrowser.h"
#include "experimentMaker.h"
#include <qlayout.h>
#include <iostream>

using namespace std;         //???????

ExperimentItem::ExperimentItem(Experiment expt, QListViewItem* parent)
  : QListViewItem(parent)
{
  experiment = expt;
  initialise();
}

ExperimentItem::ExperimentItem(Experiment expt, QListView* parent)
  : QListViewItem(parent)
{
  experiment = expt;
  initialise();
}

void ExperimentItem::initialise(){
  // set text, rtid and stuff like that..
  rtid = 898743;
  setText(0, experiment.userName.c_str());
  setText(1, experiment.experimentTime.toString("yy MM dd"));   // sorts correctly.. 
  setText(2, experiment.protocolName.c_str());   // alle lulia..
}

ExperimentBrowser::ExperimentBrowser(map<int, userInformation>* uInfo, const int* id, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  setCaption("Experiment Browser");
  userInfo = uInfo;
  myId = id;
  // set up the labels and so forthh..
  experimentView = new QListView(this, "experimentView");
  experimentView->addColumn("User");
  experimentView->addColumn("Date");
  experimentView->addColumn("Protocol");
  experimentView->setAllColumnsShowFocus(true);
  connect(experimentView, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(selectionChanged(QListViewItem*)) );

  exptMenu = new QPopupMenu(this, "exptMenu");
  exptMenu->insertItem("New Experiment", this, SLOT(makeExperiment()) ); 
  exptMenu->insertItem("Update", this, SIGNAL(updateExperiments()) );
  
  protMenu = new QPopupMenu(this, "protMenu");
  protMenu->insertItem("Load Protocol", this, SLOT(requestProtocol()));
  protMenu->insertItem("Select Experiment", this, SLOT(selectExperiment()) );

  connect(experimentView, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), this, 
	  SLOT(popMenu(QListViewItem*, const QPoint&, int)) );
  // andooh.. the QTextEdits ..
  experimentDescription = new QTextEdit(this, "experimentDescription");
  protocolDescription = new QTextEdit(this, "protocolDescription");
  experimentDescription->setReadOnly(true);
  protocolDescription->setReadOnly(true);
  
  // and the labels..

  QLabel* desLabel = new QLabel("Experiment Description", this, "desLabel");
  creatorLabel = new QLabel("Researcher\t: ", this, "creatorLabel");
  exptTimeLabel = new QLabel("Experiment Time\t: ", this, "cTimeLabel");
  entryTimeLabel = new QLabel("Entry Time\t: ", this, "eTimeLabel");
  protocolLabel = new QLabel("Protocol\t\t: ", this, "protocolLabel");
  
  // an ExperimentMaker..
  eMaker = new ExperimentMaker(id, uInfo);  // don't show..  
  eMaker->resize(600, 400);
  connect(eMaker, SIGNAL(getCollection(int)), this, SIGNAL(getCollection(int)) );
  connect(eMaker, SIGNAL(commitProtocolToDB(Protocol*)), this, SIGNAL(commitProtocolToDB(Protocol*)) );

  // and a little bit of a layout..
  QHBoxLayout* hbox = new QHBoxLayout(this, 1, 2);
  
  hbox->addWidget(experimentView);
  QVBoxLayout* rbox = new QVBoxLayout(hbox, 2);   // no need to add to.. I suppose maybe..
  rbox->addWidget(desLabel);
  rbox->addWidget(experimentDescription);
  rbox->addWidget(creatorLabel);
  rbox->addWidget(exptTimeLabel);
  rbox->addWidget(entryTimeLabel);
  rbox->addWidget(protocolLabel);
  rbox->addWidget(protocolDescription);    // we should probably have some static labels to show what things are and so on..
}

void ExperimentBrowser::setExperiments(vector<Experiment> expts){
  // first of all get rid of all of the other labels..
  QListViewItem* item;
  while( (item = experimentView->firstChild()) ){     // should return 0 when no items left..
    delete item;    // maybe I should remove first but lets try..
  }
  // and empty the set..
  set<int>::iterator it;
  while(experimentIds.size()){
    it = experimentIds.begin();
    experimentIds.erase(it);
  }
  for(int i=0; i < expts.size(); i++){
    new ExperimentItem(expts[i], experimentView);   // which should be ok..
  }
  // and in fact that should be all we need to do..
}

void ExperimentBrowser::addExperiment(int reqId, Experiment expt){
  if(!experimentIds.count(expt.id)){
    experimentIds.insert(expt.id);
    ExperimentItem* item = new ExperimentItem(expt, experimentView);
    experimentView->setCurrentItem((QListViewItem*)item);
  }
  // and regardless we should inform the eMaker, who can then decide what to do..
  eMaker->experimentMade(reqId);
  // eMaker->gotNewProtocol;  
}

void ExperimentBrowser::makeExperiment(){
  cout << "I want to make an experiment.. let's make a good widget for that.. " << endl;
  eMaker->show();
}

void ExperimentBrowser::popMenu(QListViewItem* item, const QPoint& point, int c){
  if(!item){
    exptMenu->popup(point);
    return;
  }
  if(item->rtti() != 898743){
    return;
  }
  protMenu->popup(point);
  //  requestProtocol();
  //  Experiment expt = ((ExperimentItem*)item)->expt();
  //emit loadProtocol(expt.id);
}

void ExperimentBrowser::requestProtocol(){
  QListViewItem* item = experimentView->selectedItem();
  if(!item){
    return;
  }
  if(item->rtti() != 898743){
    return;
  }
  Experiment expt = ((ExperimentItem*)item)->expt();
  cout << "requesting protocol with id : " << expt.protocol << endl;
  emit loadProtocol(expt.protocol);
}

void ExperimentBrowser::selectionChanged(QListViewItem* item){
  if(item->rtti() != 898743){
    return;
  }
  Experiment expt = ((ExperimentItem*)item)->expt();
  experimentDescription->setText(expt.comment.c_str());
  protocolDescription->setText(expt.protocolDescription.c_str());
  QString exDate = "Experiment Time\t: ";
  exDate += expt.experimentTime.toString("ddd dd MMM yy hh:mm:ss");
  QString enDate = "Entry Time\t: ";
  enDate += expt.entryTime.toString("ddd dd MMM yy hh:mm:ss");
  QString researcher = "Researcher\t: ";
  researcher += expt.userName.c_str();
  QString prot = "Protocol\t\t: ";
  prot += expt.protocolName.c_str();

  creatorLabel->setText(researcher);
  exptTimeLabel->setText(exDate);
  entryTimeLabel->setText(enDate);
  protocolLabel->setText(prot);
}

void ExperimentBrowser::selectExperiment(){
  ExperimentItem* item = (ExperimentItem*)experimentView->currentItem();
  emit selectedExperiment(item->expt());
}

/// then we need lots of other stuff, but you know.. that's really rather troublesome.. 
