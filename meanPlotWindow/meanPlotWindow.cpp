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

#include "meanPlotWindow.h"
#include "../probeSetWidget/probeSetWidget.h"
#include "meanExpressionPlotter.h"
#include "keyWindow.h"
#include "../stat/stat.h"
#include "../exptinfo.h"
#include <qwidget.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

MeanPlotWindow::MeanPlotWindow(vector<int>* ve, map<float, exptInfo>* em, QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  setCaption("Mean Expression");
  exptSelection = ve;
  eMap = em; 
  dontUpdatePlot = false; 
  QSplitter* split = new QSplitter(this, "split");
  
  raw = new MeanExpressionPlotter(split, "raw");
  norm = new MeanExpressionPlotter(split, "norm");
  keyWindow = new KeyWindow(raw->colourTable(), split, "keys");
  keyWindow->setPalette(norm->palette());

  connect(raw, SIGNAL(selectedNewFont(QFont)), this, SLOT(setFonts(QFont)) );
  connect(norm, SIGNAL(selectedNewFont(QFont)), this, SLOT(setFonts(QFont)) );

  QVBoxLayout* vbox = new QVBoxLayout(this);
  vbox->addWidget(split);
}

void MeanPlotWindow::toggleProbeSet(ProbeSetWidget* pset, bool insert){
  //cout << "MeanPlotWindow: toggle Probe Set: probesetwidget: " << pset << "  insert: " << insert << endl;
  if(insert){
    addProbeSet(pset);
  }else{
    removeProbeSet(pset);
  }
}

void MeanPlotWindow::addProbeSet(ProbeSetWidget* pset){
  cout << "this is in the addProbeSet function.. " << endl; 
  if(pSets.count(pset) > 0){ 
    cout << " Am returning, because this has already been inserted, so there's no point in doing again" << endl;
    return; 
  }    // don't insert it if its already there. 
  pSets.insert(pset);
  cout << "did insert into pset,, what happened? " << endl; 
  if(!dontUpdatePlot){
    collectData();
  }
  ////// and go through pSet, and normalise.. 
}

void MeanPlotWindow::collectData(){
  // set the appropriate values in rawMeans, normMeans, exptIndices, for all the current genes..
  // go through all of the genes, and create the appropriate things..
  if(pSets.size() < 1) { return; }
  if((*exptSelection).size() < 2) { return; }
  set<ProbeSetWidget*>::iterator it;
  map<int, int>::iterator eit;
  rawMeans.resize(0);
  normMeans.resize(0);
  exptIndices.resize(0);
  keys.resize(0);

  for(it = pSets.begin(); it != pSets.end(); it++){
    QString key;
    for(int i=0; i < (*it)->myData.ugData.size(); i++){
	if((*it)->myData.ugData[i].gene.length() && (*it)->myData.ugData[i].gene != "undeffed"){
	    key.append((*it)->myData.ugData[i].gene.c_str());
	    key.append(" ");
	}
    }
    key.append((*it)->myData.afid.c_str());
    keys.push_back(key);
    vector<float> raw;    // temp vector
    vector< vector<float> > rawSelection((*it)->mySet.probes.size()); 
    vector<int> tempIndices;
    for(int i=0; i < (*exptSelection).size(); i++){
      //cout << "\t i: " << (*exptSelection)[i] << endl;
      eit = (*it)->mySet.exptLookup.find((*exptSelection)[i]);
      if(eit != (*it)->mySet.exptLookup.end()){
	raw.push_back((*it)->mySet.mean[(*eit).second]);
	tempIndices.push_back((*exptSelection)[i]);
	for(int j=0; j < (*it)->mySet.probes.size(); j++){
	  rawSelection[j].push_back((*it)->mySet.probes[j][(*eit).second]);
	}
      }
    }
    //////// Now we have collected all we need for one gene. push back, rawMeans, exptIndices
    //////// and do the zScore for the norm.. 
    rawMeans.push_back(raw);
    exptIndices.push_back(tempIndices);
    vector<float> tempMeans(raw.size(), 0);
    for(int i=0; i < rawSelection.size(); i++){
      zScore(rawSelection[i]);
      for(int j=0; j < rawSelection[i].size(); j++){
	tempMeans[j] += rawSelection[i][j];
      }
    }
    for(int i=0; i < tempMeans.size(); i++) { tempMeans[i] = tempMeans[i]/rawSelection.size(); }
    normMeans.push_back(tempMeans);
  }
  /////////////// pheww,, done.. now just send to the plotter..
  drawStuff();
}

void MeanPlotWindow::removeProbeSet(ProbeSetWidget* pset){
  if(pSets.count(pset) > 0){
    pSets.erase(pset);
  }
  if(pSets.size() > 0){
    if(!dontUpdatePlot){
      collectData();
    }
  }else{
    raw->clearAll();
    norm->clearAll();
    keyWindow->clearAll();
  }
}

void MeanPlotWindow::drawStuff(){
  // roll up the data into a reasonable data format and send it to the plotter...
  // ho yeahh.

  raw->paintLines(rawMeans, exptIndices, eMap);
  norm->paintLines(normMeans, exptIndices, eMap);
  keyWindow->paintKeys(keys);
}
  

void MeanPlotWindow::setPenWidth(int n){
  raw->setPenWidth(n);
  norm->setPenWidth(n);
  keyWindow->setPenWidth(n);
  // do nothing for now, but we can do this later..
}

void MeanPlotWindow::dontUpdate(bool synced){
  dontUpdatePlot = synced;
  if(!synced){
    collectData();
  }
}

void MeanPlotWindow::setFonts(QFont f){
    raw->setFont(f);
    norm->setFont(f);
    
    raw->update();
    norm->update();
}
