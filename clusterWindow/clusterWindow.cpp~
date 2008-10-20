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

#include "clusterWindow.h"
#include "../smallToggle/smallToggle.h"
#include "clusterListItem.h"
#include "clusterPlotter.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <iostream>
#include <vector>

using namespace std;

ClusterWindow::ClusterWindow(set<clusterSet*>* cs, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  // set up the relevant things.. 
  clusterSets = cs;   // ok that is great.. let's include 

  int labelIndent = 5;
  QLabel* lNorm = new QLabel("Local Normalisation", this, "lNorm");
  QLabel* iNorm = new QLabel("Individual Normalisation", this, "iNorm");
  QLabel* mNorm = new QLabel("Mean Normalisation", this, "mNorm");
  QLabel* cNum  = new QLabel("Cluster Number", this, "cNum");
  lNorm->setIndent(labelIndent);  
  iNorm->setIndent(labelIndent);
  mNorm->setIndent(labelIndent);
  cNum->setIndent(labelIndent);
  // alignment, or let's just see how it handles it.
  localNorm = new SmallToggle(1, this);
  individualNorm = new SmallToggle(2, this);
  meanNorm = new SmallToggle(3, this);
  localNorm->setFixedSize(25, 25);
  individualNorm->setFixedSize(25, 25);
  meanNorm->setFixedSize(25, 25);
  // and set them all to be on..
  localNorm->setOn(true);
  individualNorm->setOn(true);
  meanNorm->setOn(true);
  // and that should be fine for the buttons...
  
  clusterNumber = new QSpinBox(this, "clusterNumber");
  clusterNumber->setMinValue(1);      // though this doesn't actually make much sense.. 
  // clusterNumber->setMaxValue(30);  // is there a reasonable way of doing this??
  QPushButton* clusterButton = new QPushButton("Do Clustering", this, "clusterButton");
  connect(clusterButton, SIGNAL(clicked()), this, SLOT(startClustering()) );

  // and a QListView for storing some graphical representation of the clusters that we may receive..
  clusterList = new QListView(this, "clusterList");
  clusterList->addColumn("Cluster Set");
  //clusterList->setSelectionMode(QListView::Extended);
  connect(clusterList, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(clusterSelectionChanged(QListViewItem*)) );
  //  clusterList->addColumn("cluster");

  // and make a plotter and hide it for now..
  meanPlotter = new ClusterPlotter();
  connect(meanPlotter, SIGNAL(pleaseLoadTheseGenes(vector<int>, QString)), this, SIGNAL(pleaseLoadTheseGenes(vector<int>, QString)) );
  //  meanPlotter = new MeanExpressionPlotter();
  meanPlotter->resize(500, 300);

  // and do the layout..
  QGridLayout* grid = new QGridLayout(this, 6, 2, 5, 3, "grid");
  
  grid->addWidget(lNorm, 0, 0);
  grid->addWidget(localNorm, 0, 1, AlignRight);
  grid->addWidget(iNorm, 1, 0);
  grid->addWidget(individualNorm, 1, 1, AlignRight);
  grid->addWidget(mNorm, 2, 0);
  grid->addWidget(meanNorm, 2, 1, AlignRight);
  grid->addWidget(cNum, 3, 0);
  grid->addWidget(clusterNumber, 3, 1);
  grid->addWidget(clusterButton, 4, 0);
  grid->addMultiCellWidget(clusterList, 5, 5, 0, 1);
}

void ClusterWindow::startClustering(){
  cout << "emitting " << clusterNumber->value() << "\t" << localNorm->isOn() << endl;
  emit doCluster(clusterNumber->value(), localNorm->isOn(), individualNorm->isOn(), meanNorm->isOn());
}

void ClusterWindow::newClusterSet(clusterSet* newSet){
  cout << "ok connection ok, client has received a new  cluster Set and I should update something so the user can have a look at it" << endl;
  if(!clusterSets->count(newSet)){
    return;
  }
  QString label;
  //  label.setNum(clusterSets->size());
  //label.prepend("Set ");
  label.sprintf("Set %d (%d)", clusterSets->size(), newSet->indices.size());
  ClusterListItem* item = new ClusterListItem(newSet, clusterSets->size(), clusterList);
  //  QListViewItem* item = new QListViewItem(clusterList, label);
  for(int i=0; i < newSet->indices.size(); i++){
    if(newSet->indices[i].size()){
      ClusterListItem* clusterItem = new ClusterListItem(newSet, i, item);
    }
  }
  
}
  
void ClusterWindow::clusterSelectionChanged(QListViewItem* item){
  cout << "Selection Changed rtid is     " << item->rtti() << endl;
  cout << "Item's selection state is now " << item->isSelected() << endl;
  if(item->rtti() == 234576 && item->isSelected()){
    ClusterListItem* temp = (ClusterListItem*)item;
    if(!meanPlotter->isVisible()){
      meanPlotter->show();
    }
    /// danger signal, I should check here.. that it actually exists.. 
    clusterSet* cSet = temp->cluster();
    int cNo = temp->number();
    meanPlotter->paintCluster(cSet, cNo);
    //    if(cNo < cSet->points.size() && cNo < cSet->expts.size()){
    //  if(cSet->points[cNo].size()){
    //	meanPlotter->paintLines(cSet->points[cNo], cSet->expts[cNo], cSet->expInfo);
    //  }
    //} 
  }
  if(item->rtti() == 234577 && item->isSelected()){
    ClusterListItem* temp = (ClusterListItem*)item;
    clusterSet* cSet = temp->cluster();
    meanPlotter->paintClusterSet(cSet);
    //    if(cSet->centers.size()){
    // meanPlotter->paintLines(cSet->centers, cSet->centExpts, cSet->expInfo);
    //}
  }
}
