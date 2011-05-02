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

#include "experimentMaker.h"
#include <qlabel.h>
#include <map>
#include <qmessagebox.h>
#include <iostream>

using namespace std;      

ExperimentMaker::ExperimentMaker(const int* myId, map<int, userInformation>* uInfo, QWidget* parent, const char* name)
  : ProtocolManager(myId, uInfo, parent, name)
{
  // set up the things..
  setCaption("Experiment Maker");
  requestId = (int)(long)this;
  
  descriptionView->setReadOnly(true);
  
  experimentDescription = new QTextEdit(this);
  QLabel* desLabel = new QLabel("Experiment Description", this, "desLabel");
  
  timeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
  initialTime = timeEdit->dateTime();
  timeEdit->setAutoAdvance(true);
 
  QPushButton* commitButton = new QPushButton("Commit Experiment", this, "commitButton");
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commitExperiment()) );
 
  // and put into the thingy..
  rbox->addStretch();
  rbox->addWidget(desLabel);
  rbox->addWidget(experimentDescription);
  rbox->addWidget(timeEdit);
  
  QHBoxLayout* comBox = new QHBoxLayout();
  rbox->addLayout(comBox);
  comBox->addStretch();
  comBox->addWidget(commitButton);

  //  desLabel->show();
  //experimentDescription->show();
  //timeEdit->show();
}

void ExperimentMaker::commitExperiment(){
  cout << "Do commit the experiment or something... " << endl;
  // check that we have a selected protocol :
  QString errorString;
  QListViewItem* item = listView->selectedItem();
  if(!item){
    errorString = "You need to select a protocol before you can create the experiment\n";
    cerr << "No protocol Selected, can't do this really " << endl;
  }
  // check if the time has been changed.. -- the experiment really isn't likely to have been carried out
  // at the entry time is it now.. 
  if(timeEdit->dateTime() == initialTime){
    errorString += "You need to set the time of the experiment in order to create the experiment\n";
    cerr << "You didn't set the time did you,, " << endl;
  }
  // check if the description has been modified or not.. 
  if(!experimentDescription->isModified()){
    errorString += "You need to give some sort of description of the experiment\n";
    cerr << "You didn't give any kind of description for the experiment.. Do you really want to go ahead with this.. "
	 << endl;
  }
  if(errorString.length()){
    QMessageBox::information(this, "Experiment Maker", errorString, "Cancel", 0);
    return;
  }

  // if we are here, I suppose things are ok.. we need to make an experiment and send it on..
  // hmm... 
  // first find the protocol that we need..
  map<QListViewItem*, Protocol*>::iterator it = protocolMap.find(item);
  if(it == protocolMap.end()){
    cerr << "Couldn't find a protocol associated with this thingy,, bugger " << endl;
    return;
  }
  map<int, userInformation>::iterator uit = userData->find(*userId);
  if(uit == userData->end()){
    cerr << "Couldn't get a user information for user id : " << *userId << endl;
  }
  Experiment experiment(-1, *userId, (*uit).second.userName, timeEdit->dateTime(), timeEdit->dateTime(), 
			(*it).second->id, (*it).second->name, (*it).second->description, experimentDescription->text().latin1());
  // and emit it.. 
  requestId++;
  emit commitExperimentToDB(experiment, requestId);
}

void ExperimentMaker::experimentMade(int reqid){           // this is terrible stuff.. but there you go..
  if(reqid == requestId){
    hide();
  }
}
