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

#include "clusterPlotter.h"
#include "../client.h"
#include "../meanPlotWindow/meanExpressionPlotter.h"
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

ClusterPlotter::ClusterPlotter(QWidget* parent, const char* name)
  : MeanExpressionPlotter(parent, name)
{
  actionMenu = new QPopupMenu(this);
  actionMenu->insertItem("Load Genes", this, SLOT(loadGenes()), CTRL+Key_L, 1);
  actionMenu->insertItem("Save Clusters", this, SLOT(saveClusters()), CTRL+Key_S, 2);

  clusterNumber = -1;
  theCluster = 0;
}

void ClusterPlotter::paintCluster(clusterSet* cs, int index){
  if(index < (int)cs->points.size() && index >= 0){
    if(cs->points[index].size()){
      penWidth = 2;
      paintLines(cs->points[index], cs->expts[index], cs->expInfo);
      clusterNumber = index;
      theCluster = cs;
    }
  }
}

void ClusterPlotter::paintClusterSet(clusterSet* cs){
  if(cs->centers.size()){
    penWidth = 4;
    paintLines(cs->centers, cs->centExpts, cs->expInfo);
    clusterNumber = -1;
    theCluster = cs;
  }
}

void ClusterPlotter::mousePressEvent(QMouseEvent* e){
  if(e->state() == Qt::ControlButton || e->button() == 2){
    if(clusterNumber != -1){
      actionMenu->popup(mapToGlobal(e->pos()));
    }
  }
}

void ClusterPlotter::loadGenes(){
  cout << "Cluster Plotter loadGenes private slot called, what a wonderful world it issss" << endl;
  if(clusterNumber < 0 || theCluster == 0){
    return;
  }
  if(clusterNumber < (int)theCluster->indices.size()){
    QString description;
    description.setNum(clusterNumber);
    description.prepend("Cluster no. ");
    emit pleaseLoadTheseGenes(theCluster->indices[clusterNumber], description);
  }
}

void ClusterPlotter::saveClusters(){
  QString fileName = QFileDialog::getSaveFileName();
  if(fileName.isNull()){
    return;
  }
  ofstream out(fileName.latin1());
  if(!out){
    cout << "Couldn't open file" << endl;
    return;
  }
  // File identifier
  out << "groovy_cluster_set_file" << endl;
  // and lets write some numbers.. 
  out << theCluster->indices.size() << " clusters" << endl;
  for(uint i=0; i < theCluster->indices.size(); i++){
    out << i << "\t";
    for(uint j=0; j < theCluster->indices[i].size(); j++){
      out << theCluster->indices[i][j] << " ";
    }
    out << endl;
  }
  out.close();
}
