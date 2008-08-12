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

#include "twoDimDistLauncher.h"
#include <qpushbutton.h>
#include <qvalidator.h>
#include <math.h>
#include <iostream>

using namespace std;

statistic::statistic(){
  nameLabel = 0;
  xButton = 0;
  yButton = 0;
}

statistic::statistic(vector<float> v, string nme){
  name = nme.c_str();
  values = v;
  nameLabel = 0;
  xButton = 0;
  yButton = 0;
}

statistic::~statistic(){
  cout << "statistic destructor " << endl;
  if(nameLabel){
   delete nameLabel;
  }
  if(xButton){
    delete xButton;
  }
  if(yButton){
    delete yButton;
  }
}

void statistic::show(){
  if(xButton){
    xButton->show();
  }
  if(yButton){
    yButton->show();
  }
  if(nameLabel){
    nameLabel->show();
  }
}

void statistic::insert(QGridLayout* g, int row){
  if(nameLabel){
    g->addWidget(nameLabel, row, 0);
  }
  if(xButton){
    g->addWidget(xButton, row, 1);
  }
  if(yButton){
    g->addWidget(yButton, row, 2);
  }
}

TwoDimDistLauncher::TwoDimDistLauncher(const vitalStatistics* s, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  cout << "Twod launcher .. beginning.. " << endl;
  setCaption("2 Dimensional Distribution");
  vitalStats = s;
  // don't bother checking these.. at the moment..
  cout << "am about to make do button " << endl;
  doButton = new QPushButton("Map Stats", this, "doButton");
  connect(doButton, SIGNAL(clicked()), this, SLOT(mapDistribution()) );
  QLabel* statNameLabel = new QLabel("Statistic Name", this, "statNameLabel");
  QLabel* xLabel = new QLabel("X", this, "xLabel");
  QLabel* yLabel = new QLabel("Y", this, "yLabel");
  
  // inputs for mins and maxes if we want to set separately..
  minX = new QLineEdit(this, "minX");
  maxX = new QLineEdit(this, "maxX");
  minY = new QLineEdit(this, "minY");
  maxY = new QLineEdit(this, "maxY");
  // and lets set up some validators for these
  QDoubleValidator* minXValid = new QDoubleValidator(minX);
  QDoubleValidator* maxXValid = new QDoubleValidator(maxX);
  QDoubleValidator* minYValid = new QDoubleValidator(minY);
  QDoubleValidator* maxYValid = new QDoubleValidator(maxY);
  minX->setValidator(minXValid);
  maxX->setValidator(maxXValid);
  minY->setValidator(minYValid);
  maxY->setValidator(maxYValid);

  progBar = new QProgressBar(100, this, "progBar");
  cout << "made progress bar.. " << endl;


  logXBox = new QCheckBox(this, "logBox");
  logYBox = new QCheckBox(this, "logBox");
  //logBox->setChecked(true);
  QLabel* logLabel = new QLabel("Log", this, "logLabel");
  sigmaBox = new QSpinBox(1, 100, 1, this, "sigmaBox");
  sigmaBox->setSuffix("%");
  sigmaBox->setValue(5);
  QLabel* sigmaLabel = new QLabel("Sigma", this, "sigmaLabel");
  // some layout stuff..
  QVBoxLayout* vbox = new QVBoxLayout(this, 3, 3);
  grid = new QGridLayout(vbox, 1, 3, 3);   // change spacing and stuff later on.. 
  grid->setColStretch(0, 1);
  grid->addWidget(statNameLabel, 0, 0);
  grid->addWidget(xLabel, 0, 1);
  grid->addWidget(yLabel, 0, 2);
  
  // and horizontal layout..
  QHBoxLayout* hbox1 = new QHBoxLayout(vbox);
  hbox1->addWidget(sigmaLabel);
  hbox1->addStretch();
  hbox1->addWidget(sigmaBox);
  
  QHBoxLayout* hbox2 = new QHBoxLayout(vbox);
  hbox2->addWidget(logLabel);
  hbox2->addStretch();
  hbox2->addWidget(logXBox);
  hbox2->addWidget(logYBox);

  // forget about the nice layout..
  vbox->addWidget(minX);
  vbox->addWidget(maxX);
  vbox->addWidget(minY);
  vbox->addWidget(maxY);

  QHBoxLayout* hbox = new QHBoxLayout(vbox);
  hbox->addWidget(doButton);
  hbox->addWidget(progBar);

  // set distMapper to 0, so I can check..
  distMapper = 0;
  progTimer = new QTimer(this, "progTimer");
  connect(progTimer, SIGNAL(timeout()), this, SLOT(checkProgress()) );

  // and let's make a plotter..
  //plotter = new TwoDimDistPlotter();   // toplevel window.. 
  
  // which should be pretty much everything.. 
  cout << "the end of the constructor " << endl;
}

void TwoDimDistLauncher::newStats(){
  // ok, first lets make a new vector of things.. and then we can do
  if(vitalStats->statNames.size() != vitalStats->stats.size()){
    cerr << "TwoDimDistLauncher, vital stats misformed, sizes not equal " << endl;
    return;
  }
  cout << "making the tempindex thing" << endl;
  vector<int> tempIndex(vitalStats->index.size());
  for(int i=0; i < vitalStats->index.size(); i++){
    tempIndex[i] = vitalStats->index[i];
  }
  cout << "checking sizes " << endl;
  // now go through and make sure that all the stats are of the right size..
  for(int i=0; i < vitalStats->stats.size(); i++){
    if(vitalStats->stats[i].size() != tempIndex.size()){
      cerr << "TwoDimDistLauncher stats size is different from index size, give up.. " << endl;
      return;
    }
  }
  // ok, now we can be reasonably happy with stuff, let's go through and remove the widgets from the 
  // the layout, and then resize the vector, and hence deleting stuff.
  cout << "removing from the grid.. " << endl;
  for(int i=0; i < stats.size(); i++){
    grid->remove(stats[i]->xButton);
    grid->remove(stats[i]->yButton);
    grid->remove(stats[i]->nameLabel);
    delete stats[i];
  }
  cout << "removed done, calling resize " << endl;
  stats.resize(0);
  cout << "resize done " << endl;
  // which should call delete on everything..
  // and now let's go through stats and make new ones, and put those into the grid, and call show on them.. 
  cout << "making new stats .. " << endl;
  for(int i=0; i < vitalStats->statNames.size(); i++){
    cout << "\ti : " << i << endl;
    stats.push_back(new statistic(vitalStats->stats[i], vitalStats->statNames[i])); // which should take care of everything..
    cout << "\t\tpushed back" << endl;
    stats[i]->nameLabel = new QLabel(stats[i]->name, this, "nameLabel");
    stats[i]->xButton = new QRadioButton(this, "xButton");
    stats[i]->yButton = new QRadioButton(this, "yButton");
    // we have to connect the radiobuttons..
    cout << "\t\tmade" << endl;
    connect(stats[i]->xButton, SIGNAL(clicked()), this, SLOT(xSelected()) );
    connect(stats[i]->yButton, SIGNAL(clicked()), this, SLOT(ySelected()) );
    cout << "\t\tconnected " << endl;
  }
  // and let's insert and call show on themm
  
  indices = tempIndex;  // ok.. 
  cout << "inserting " << endl;
  for(int i=0; i < stats.size(); i++){
    cout << "\t" << i << endl;
    stats[i]->insert(grid, i+1);
    cout << "\t\tshow.. " << endl;
    stats[i]->show();
  }
  cout << "end of new stats.. " << endl;
  // and that should be everything..
}

void TwoDimDistLauncher::xSelected(){
  QRadioButton* button = (QRadioButton*)sender();
  cout << "X button pressed and it is now : " << button->isOn() << endl;
  if(button->isOn()){     // we have to make sure that no other buttons are on in the x thingy
    for(int i=0; i < stats.size(); i++){
      if(stats[i]->xButton){
	if(stats[i]->xButton->isOn() && stats[i]->xButton != button){
	  stats[i]->xButton->toggle();
	}
      }
    }
  }
}

void TwoDimDistLauncher::ySelected(){
  QRadioButton* button = (QRadioButton*)sender();
  cout << "Y bytton pressend and it is now " << button->isOn() << endl;
  if(button->isOn()){     // we have to make sure that no other buttons are on in the x thingy
    for(int i=0; i < stats.size(); i++){
      if(stats[i]->yButton){
	if(stats[i]->yButton->isOn() && stats[i]->yButton != button){
	  stats[i]->yButton->toggle();
	}
      }
    }
  }

}

void TwoDimDistLauncher::mapDistribution(){
  cout << "Map distribution called, check it out.. " << endl;
  // first make two float distributions..
  if(!indices.size()){
    cerr << "no index man, nothing to map" << endl;
  }
  double* a = new double[indices.size()];
  double* b = new double[indices.size()];
  int* indx = new int[indices.size()];
  bool x = false;
  bool y = false;
  bool doXLog = logXBox->isChecked();
  bool doYLog = logYBox->isChecked();
  cout << "called a and b to 0 " << endl;
  double minx, maxx, miny, maxy;
  minx = maxx = miny = maxy = 0.0;
  bool ok;
  if(minX->text().length() && maxX->text().length() && minY->text().length() && maxY->text().length()){
    minx = minX->text().toDouble(&ok);
    maxx = maxX->text().toDouble(&ok);
    miny = minY->text().toDouble(&ok);
    maxy = maxY->text().toDouble(&ok);
  }
  // they all have validators so we shouldn't have to worry about setting them.. 

  for(int i=0; i < indices.size(); i++){
    indx[i] = indices[i];
  }
  for(int i=0; i < stats.size(); i++){
    if(stats[i]->xButton->isOn()){
      x = true;
      for(int j=0; j < stats[i]->values.size(); j++){
	if(doXLog){
	  a[j] = log((double)stats[i]->values[j]);
	}else{
	  a[j] = (double)stats[i]->values[j];
	}
      }
    }
    if(stats[i]->yButton->isOn()){
      y = true;
      for(int j=0; j < stats[i]->values.size(); j++){
	if(doYLog){
	  b[j] = log((double)stats[i]->values[j]);
	}else{
	  b[j] = (double)stats[i]->values[j];
	}
      }
    }
  }
  if(x && y){
    // lets make the thingy and do something with it.. 
    distMapper = new TwoDimDist(a, b, indices.size(), indx, &progMutex, &progInt, sigmaBox->value(), 100, minx, maxx, miny, maxy);
    currentSigma = sigmaBox->value();
    cout << "starting dist mapper " << endl;
    distMapper->start();
    cout << "distmapper started " << endl;
    progTimer->start(50);
    cout << "prog timer started " << endl;
    doButton->setEnabled(false);
    cout << "and about to return " << endl;
    return;
  }
  cout << "You didn't select an x and a y thing to plot did you now.. " << endl;
  if(a){
    delete []a;
  }
  if(b){
    delete []b;
  }
}


void TwoDimDistLauncher::checkProgress(){
  //  cout << "This is check progress..  " << endl;
  int value;
  progMutex.lock();
  value = progInt;
  progMutex.unlock();      // these locks are probably unnecessary, but better safe than sorry.
  //cout << "and the value is .. " << value << endl;
  if(value <= 100){
    progBar->setProgress(value);
  }
  if(value == 100){
    cout << "value is 100, so am going to do some stuff " << endl;
    doButton->setEnabled(true);
    progTimer->stop();
    cout << "calling display Map " << endl;
    displayMap();   // display Map takes care of distMapper, extracts the values it wants and deletes it, and other things..
  }
}

void TwoDimDistLauncher::displayMap(){
  cout << "Display the map and stuff.. " << endl;
  progBar->setProgress(0);
  // make some class for displaying the map and give this the appropriate information..
  distMapper->wait();
  // before anything .. 
  TwoDimDistPlotter* plotter = new TwoDimDistPlotter();
  plotters.insert(plotter);
  plotter->setData(distMapper->distribution(), currentSigma, true);
  plotter->show();

  delete distMapper;
  //cout << "distMapper deleted .. " << endl;
  distMapper = 0;
  progInt = 0;
}


