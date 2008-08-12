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

#include "statWidget.h"
#include "statWindow.h"
#include "distChooser.h"
#include "../probe_set/probe_set.h"
#include <qwidget.h>
#include <string>
#include <vector>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <iostream>
#include <fstream>
#include <qstring.h>
#include <qfiledialog.h>

using namespace std;

StatWindow::StatWindow(vitalStatistics* s, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  stats = s;
  // at this poing stats may not include any data,, so do nothing.
  // except set up the layout..
  statwidget = new StatWidget(this, "plot");
  dists.push_back(new DistChooser("empty", 100, this, "empty"));
  twoDimLauncher = new TwoDimDistLauncher(stats);
  QPushButton* twoDimButton = new QPushButton("2 Dim Plot", this, "twoDimButton");
  connect(twoDimButton, SIGNAL(clicked()), twoDimLauncher, SLOT(showNormal()) );

  QPushButton* fileButton = new QPushButton("Read File", this, "fileButton");
  connect(fileButton, SIGNAL(clicked()), this, SLOT(readStatsFromFile()) );
  // twoDimLauncher->show();
  //  dist1 = new DistChooser("Anova Score", 100, this, "anova");
//    dist1->setLog(true);
//    dist2 = new DistChooser("Euclidean", 100, this, "euclid");
//    dist2->setLog(true);
//    dist3 = new DistChooser("Variation Coefficient", 100, this, "variation");
//    dist3->setLog(true);
//    connect(dist1, SIGNAL(newRanges(float, float)), this, SLOT(newT(float, float)) );
//    connect(dist2, SIGNAL(newRanges(float, float)), this, SLOT(newT(float, float)) );
//    connect(dist3, SIGNAL(newRanges(float, float)), this, SLOT(newT(float, float)) );
  QPushButton* loadButton = new QPushButton("load genes", this, "loadButton");
  connect(loadButton, SIGNAL(clicked()), this, SLOT(loadGenes()) );
  activeLabel = new QLabel("selected : 0", this, "activeLabel");
  activeLabel->setIndent(5);
  
  QGridLayout* grid = new QGridLayout(this, 3, 1, 0, 5);  // 5 == space between.. 
  grid->setRowStretch(0, 3);
  grid->setRowStretch(1, 1);  //hmm. maybe .. 

  hbox = new QHBoxLayout();
  QHBoxLayout* buttonBox = new QHBoxLayout();
  
  grid->addWidget(statwidget, 0, 0);
  grid->addLayout(hbox, 1, 0);
  hbox->addWidget(dists[0]);
  grid->addLayout(buttonBox, 2, 0);
  buttonBox->addWidget(twoDimButton);
  buttonBox->addWidget(loadButton);
  buttonBox->addWidget(activeLabel);
  buttonBox->addWidget(fileButton);
  //hbox->addWidget(dist2);
  //hbox->addWidget(dist3);
  resize(500, 500);
}

StatWindow::~StatWindow(){
  // cout<< "Deleting StatWindow " << endl;
}

void StatWindow::newStats(){
  // ok, let's try this one out..
  twoDimLauncher->newStats();
  for(int i=0; i < dists.size(); i++){
    dists[i]->hide();
    distMap.erase(dists[i]);
    delete dists[i];
  } // delete the current dists, then make some new ones.. 
  ///////////////////////////////////////////////
  //        BIG NOTE,, REMEMEBER TO keep the dists vector in the same order as the stats->stats vector,, if this 
  //                   changes we will probably get lots of crashes due to bad pointers and the like.. 
  dists.resize(0);
  for(int i=0; i < stats->statNames.size(); i++){
    dists.push_back(new DistChooser(stats->statNames[i], 100, this, stats->statNames[i].c_str()));
    dists[i]->setData(stats->index, stats->stats[i]);
    distMap[dists[i]] = i;      // not the fastest way, but it's ok.. just sets the data.. 
    connect(dists[i], SIGNAL(newRanges(float, float)), this, SLOT(newT(float, float)) );
    connect(dists[i], SIGNAL(axisWish(int)), this, SLOT(changeAxis(int)) );
    connect(dists[i], SIGNAL(setLogValues(bool)), this, SLOT(setLogAxis(bool)) );
    dists[i]->show();  
    hbox->addWidget(dists[i]);
  }
  ///////////// need to change the below.. !! argh,, what I wouldn't give for a quiet life.. 
  //vector< vector<float>* > pointData(3); 
  emptyData.resize(stats->stats[0].size());       // which I think should do fine.. 
  active.resize(stats->stats[0].size());
  activityVotes.resize(stats->stats[0].size());  // and they should all be empty to begin with.. 
  for(int i=0; i < emptyData.size(); i++) { 
    emptyData[i] = 0; 
    active[i] = true;
    // and empty the activityVotes.. ?? 
    while(activityVotes[i].size()){
      activityVotes[i].erase(activityVotes[i].begin());    // which seems to be the best way of erasing?? 
    }
  } // there has to be a better way.?? 
  for(int i=0; i < 3; i++){  // should be the first three.. h..
    if(i < stats->stats.size()){
      axisMap[i+1] = &stats->stats[i];
      distAxisMap[i+1] = i;
      dists[i]->setAxis(i+1);
      //pointData[i] = &stats->stats[i];        // actually not a necessary copy. we could use something else.. 
    }else{
      axisMap[i+1] = &emptyData;
      //      pointData[i] = &emptyData;
    }
  }
  //  for(int i=stats->stats.size(); i < 3; i++){
  //  pointData[i] = emptyData;
  //}
  // at the beginning we need to create vector<bool> active or something.. but we need to have this in somewhere. 
  data.setData(axisMap[1], axisMap[2], axisMap[3], &active);          /// hmmmmmmm hmnmmm,m,m  x, y, z.. 
  //  data.SetData(*pointData[0], *pointData[1], *pointData[2]);    // hmm. 
  //  vector<float> x(stats->stats[0].size());
//    vector<float> y(stats->stats[1].size());
//    vector<float> z(stats->stats[2].size());            // kludge it for now.. and for a bit longer 
//    for(int i=0; i < stats->index.size(); i++){
//      x[i] = stats->stats[0][i];
//      y[i] = stats->stats[1][i];
//      z[i] = stats->stats[2][i];
//    }  //// that is a real waste of time, I really need to sort this data struct out. 
//    data.setData(x, y, z);   // sets thresholds and stuff to include everything..
  data.xName = "Anova";
  data.yName = "Euclidean";
  data.zName = "Variation";
  statwidget->setModel(data);
  QString num;
  num.setNum(active.size());
  num.prepend("selected : ");
  activeLabel->setText(num);
  //  dist1->setLog(true);
  //  dists[0]->setData(x); 
  //dists[1]->setData(y);
  //dists[2]->setData(z);

}

void StatWindow::changeAxis(int a){
  DistChooser* dc = (DistChooser*)sender();
  map<DistChooser*, int>::iterator mit = distMap.find(dc);
  // cout<< "Distchooser derived from stats->stat[" << (*mit).second << "] wants to change axis to : " << a << endl;
  // cout<< "This Distchooser currently holds axis " << dc->currentAxis() << endl;
  axisMap[dc->currentAxis()] = &emptyData;
  distAxisMap.erase(dc->currentAxis());       // remove the axis currently held by this one, if it does hold one 
  map<int, int>::iterator dit = distAxisMap.find(a);
  if(dit != distAxisMap.end()){
    //cout << "this axis is currently held by distchooser number : " << (*dit).second << endl;
    dists[(*dit).second]->setAxis(0);         // i.e. undo it.. 
    
  }else{
    //cout << "This axis is currently free and no problems hello " << endl;
  }
  // and update the map.. 
  dc->setAxis(a);
  distAxisMap[a] = (*mit).second;
  axisMap[a] = &stats->stats[(*mit).second];
  // unfortunately I know have to make sure that the appropriate logValues is set for the thing.. so I
  // have to switch on the and then set the appropriate value depending on the value in the distChooser..
  switch(a){
  case 1:
    data.xLog = dc->isLog();
    break;
  case 2:
    data.yLog = dc->isLog();
    break;
  case 3:
    data.zLog = dc->isLog();
    break;
  default :
    cerr << "change Axis : no axis " << a << endl;
  }
  //cout << "And distAxisMap[" << a << "] is now " << distAxisMap[a] << endl;
  //cout << "sizes axisMap1 " << axisMap[1]->size() << "\t2 : " << axisMap[2]->size() << "\t3 : " << axisMap[3]->size() << endl;
  data.setData(axisMap[1], axisMap[2], axisMap[3], &active);
  // cout << endl << "dc Current Axis : " << dc->currentAxis() << endl
  //     << "tmax " << dc->tMax() << "\ttmin " << dc->tMin() << endl;
  //  dit = distAxisMap.find(1);
//   if(dit != distAxisMap.end()){
//     data.tmaxX = dists[(*dit).second]->tMax();
//     data.tminX = dists[(*dit).second]->tMin();
//   }
//   dit = distAxisMap.find(2);
//   if(dit != distAxisMap.end()){
//     data.tmaxY = dists[(*dit).second]->tMax();
//     data.tminY = dists[(*dit).second]->tMin();
//   }
//   dit = distAxisMap.find(3);
//   if(dit != distAxisMap.end()){
//     data.tmaxZ = dists[(*dit).second]->tMax();
//     data.tminZ = dists[(*dit).second]->tMin();
//   }
//    switch(dc->currentAxis()){
//    case 1:
//      data.tmaxX = dc->tMax();
//      data.tminX = dc->tMin();
//      break;
//    case 2:
//      data.tmaxY = dc->tMax();
//      data.tminY = dc->tMin();
//      break;
//    case 3:
//      data.tmaxZ = dc->tMax();
//      data.tminZ = dc->tMin();
//      break;
//    default:
//      cout << "no change " << endl;
//      // do nothing;;
//    }
//  cout << "calling statwidget set model with data.. " << endl;
  statwidget->setModel(data);     // hooo gengaaa.. 
}

void StatWindow::setLogAxis(bool b){
  DistChooser* distSender = (DistChooser*)sender();
  int a = distSender->currentAxis();
  switch(a){
  case 1:
    data.xLog = b;
    break;
  case 2:
    data.yLog = b;
    break;
  case 3:
    data.zLog = b;
    break;
  default :
    cerr << "Unknown axis : " << a << endl;
  }
  // no need to update.. the model, as it will be updated anyway.. by the threshold change..
}

void StatWindow::newT(float min, float max){
  DistChooser* distSender = (DistChooser*)sender();
  // ok, I now actually have to go through all of the data points for this particular
  // variable, and then I have to work out what's going on.. 
  // first find out which statistic from the distMap..

  map<DistChooser*, int>::iterator it = distMap.find(distSender);
  if(it == distMap.end()){
    cerr << "couldn't find an int for the distchooser* " << endl;
    return;
  }
  if((*it).second >= stats->stats.size() || (*it).second < 0){
    cerr << "int from distMap too large,, no good. lets" << endl;
    return;
  }
  int s = (*it).second;
  // ok so I have an int, and I have some other stuff..
  if(active.size() != stats->stats[s].size()){
    cerr << "active size is not the same as the stats->stats.size " << endl;
    return;
  }
  // and now let's just check..
  activeNo = 0;
  for(int i=0; i < stats->stats[s].size(); i++){
    if(stats->stats[s][i] > max || stats->stats[s][i] < min){
      activityVotes[i].insert(distSender);
    }else{
      activityVotes[i].erase(distSender);
    }
    active[i] = !(activityVotes[i].size());  // i.e. not active if any activityVotes which should probably be called something else..
    if(active[i]){
      activeNo++;
    }
  }
  // and at this point we should just set the model again..
  statwidget->setModel(data);  // ok !! 
  cout << "number of Active probe sets is : " << activeNo << endl;
  QString num;
  num.setNum(activeNo);
  num.prepend("selected : ");
  activeLabel->setText(num);
//    cout << "the senders name is: " << distSender->name() << endl;
//    if(string("anova") == distSender->name()){
//      // anova maps to the x axis, now obviously this isn't particularly useful.. but..
//      // we should set up some sort of map and then use this.. 
//      data.tmaxX = max;
//      data.tminX = min;
//      statwidget->setModel(data);   // redraws.. 
//      cout << "REdrawing with new stuff.. " << endl;
//    }
//    if(string("euclidean") == distSender->name()){
//      // anova maps to the x axis, now obviously this isn't particularly useful.. but..
//      // we should set up some sort of map and then use this.. 
//      data.tmaxY = max;
//      data.tminY = min;
//      statwidget->setModel(data);   // redraws.. 
//      cout << "REdrawing with new stuff.. " << endl;
//    }
//    if(string("variation") == distSender->name()){
//      // anova maps to the x axis, now obviously this isn't particularly useful.. but..
//      // we should set up some sort of map and then use this.. 
//      data.tmaxZ = max;
//      data.tminZ = min;
//      statwidget->setModel(data);   // redraws.. 
//      cout << "REdrawing with new stuff.. " << endl;
  // }
}

void StatWindow::loadGenes(){
  vector<int> index;
  index.reserve(active.size());
  for(int i=0; i < active.size(); i++){
    if(active[i]){
      cout << "pushing back the index value : " << stats->index[i] << endl;
      index.push_back(stats->index[i]+1);
    }
  }
  emit setIndex(index, "Statistical");
}

void StatWindow::readStatsFromFile(){
  QString infile = QFileDialog::getOpenFileName();
  if(infile.isNull()){
    return;
  }
  ifstream in(infile.latin1());
  if(!in){
    cerr << "Couldn't open file" << endl;
    return;
  }
  string word;
  vector<string> fields;
  vector<string> statNames;
  vector<int> index;
  while(in >> word){
    cout << word << endl;
    if(word == "|"){
      break;
    }
    if(fields.size()){       // i.e. don't put in the probe thing as a field size .. 
      statNames.push_back(word);
    }
    fields.push_back(word);
  }
  vector<vector<float> > statValues(statNames.size());
  while(1){
    vector<float> values;
    int i;
    if(!(in >> i)){
      cerr << "Coulnd't get index " << endl;
      break;
    }
    while(values.size() < statValues.size()){
      float v;
      if(!(in >> v)){
	cerr << "Error reading float" << endl;
      }
      values.push_back(v);
    }
    if(values.size() == statValues.size()){
      index.push_back(i);
      for(int j=0; j < statValues.size(); j++){
	statValues[j].push_back(values[j]);
      }
    }else{
      cerr << "Values size is not the same as index size, that sucks.. " << endl;
    }
    // and lets print out the stuff
    cout << i;
    for(int i=0; i < values.size(); i++){
      cout << "\t" << values[i];
    }
    cout << endl;
  }
  cout << "size of stats is : " << statValues.size() << endl;
  cout << "size of fields is : " << fields.size() << endl;
  vitalStatistics tempStats;
  tempStats.index = index;
  tempStats.stats = statValues;
  tempStats.statNames = statNames;
  // and do a sly one..
  *stats = tempStats;     // can I?
  newStats();
}
