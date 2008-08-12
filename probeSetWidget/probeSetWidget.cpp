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

#include "probeSetWidget.h"
#include "../smallToggle/smallToggle.h"  // my toggle button!
#include "../probe_set/probe_set.h"

#include <qwidget.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qlayout.h>
#include <qlineedit.h>              //
#include <qfont.h>
#include <qstring.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

ProbeSetWidget::ProbeSetWidget(probe_set pset, probe_data data, QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  mySet = pset;
  myData = data;
  
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  int pointsize = qApp->font().pointSize();
  setFont(QFont("Arial", pointsize));
  affyId = new QLineEdit(QString(data.afid.c_str()), this);
  affyId->setReadOnly(true);
  //affyId->setFrame(false);
  affyId->setSizePolicy(sizePolicy);
  ugGene = new QLineEdit(this);    // set later depending on the circumstances..
  //ugGene->setFrame(false);
  ugGene->setSizePolicy(sizePolicy);
  QString ugStuff;
  for(int i=0; i < data.ugData.size(); i++){
    ugStuff.append(data.ugData[i].gene.c_str());
    if(i < data.ugData.size()-1){
      ugStuff.append(", ");
    }
  }             ///////// GOD that is so ugly..
  ugGene->setText(ugStuff);
  QPushButton* plot = new QPushButton("plot", this, "plot");
  plot->setSizePolicy(sizePolicy);
  connect(plot, SIGNAL(clicked()), this, SLOT(plotSet()));
  save = new QPushButton("save", this, "save");
  save->setSizePolicy(sizePolicy);
  connect(save, SIGNAL(clicked()), this, SLOT(rememberMe()));
  
  meanButton = new SmallToggle(1, this, "mean");   // 1 redundant, don't really need..
  //meanButton->resize(save->height(), save->height());
  connect(meanButton, SIGNAL(toggled(bool)), this, SLOT(plotMean(bool)));
  
  // and arrange everything..
  QHBoxLayout* hbox = new QHBoxLayout(this);
  hbox->addWidget(save);
  hbox->addWidget(affyId);
  hbox->addWidget(ugGene);
  hbox->addWidget(plot);
  hbox->addWidget(meanButton);
}

void ProbeSetWidget::setProbeSet(probe_set pset, probe_data data, bool checked){
  //blockSignals(true);
  mySet = pset;
  myData = data;
  affyId->setText(QString(data.afid.c_str()));
  QString ugStuff;
  meanButton->setOn(false);            // to make the implementation cleaner.. hmm. 
  for(int i=0; i < data.ugData.size(); i++){
    ugStuff.append(data.ugData[i].gene.c_str());
    ugStuff.append(" ");
  }
  ugGene->setText(ugStuff);
  meanButton->setOn(checked);
  // and set the checked to checked or not..
  //blockSignals(false);
  emit plotMyMean(this, checked);  // which doesn't actually happen if dontupdate is true, but could.. hmm. 
}

ProbeSetWidget::~ProbeSetWidget(){
  //  cout << "deleting PROBE SET WIDGET, and emitting don't plot me hello!!! " << endl;
  emit plotMyMean(this, false);       // remove from mean plot, so that we can get back to stuff
  ///////////// I think everything else gets deleted anyway.. so there we go..
}

bool ProbeSetWidget::meanPlotOn(){
  return(meanButton->isOn());
}

void ProbeSetWidget::plotMean(bool really){
  //  cout << "THIS is in the probesetwidget,, plotMyMean really: " << this << "  : " << really << endl;
  emit plotMyMean(this, really);
}

void ProbeSetWidget::plotSet(){
  emit plotMe(&mySet, &myData);
}

void ProbeSetWidget::rememberMe(){
  emit pleaseRememberMe(this);
}

void ProbeSetWidget::resetMe(){
  meanButton->setOn(false);
  emit plotMyMean(this, false);
}

