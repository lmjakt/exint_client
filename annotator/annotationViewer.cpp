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

#include "annotationViewer.h"
#include "../probe_set/probe_set.h"
#include <qwidget.h>
#include <qsizepolicy.h>
#include <qscrollview.h>
#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qlabel.h>
#include <vector>
#include <iostream>

using namespace std;

AnnotationViewer::AnnotationViewer(Client* c, QWidget* parent, const char* name)
  : QWidget(parent, name)
{

  client = c;
  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  int rowHeight = 25; // the bottom row.. 

  scroll = new QScrollView(this);
  frame = new QFrame(scroll->viewport());
  //  frame->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum));
  scroll->addChild(frame);
  vBox = new QVBoxLayout(frame, 0, 3);    // ok.. 

  //  QPushButton* voidButton = new QPushButton("void button", this, "voidButton");
  loadIncluded = new QPushButton("Load Set", this, "loadIncludedButton");
  loadIncluded->setDisabled(true);
  loadIncluded->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
  loadIncluded->setFixedHeight(rowHeight);
  connect(loadIncluded, SIGNAL(clicked()), this, SLOT(loadIncludedGenes()) );

  includedGeneCount = new QLabel("Set size : 0", this, "includedGeneCount");
  includedGeneCount->setIndent(4);
  includedGeneCount->setFixedHeight(rowHeight);

  QGridLayout* grid = new QGridLayout(this, 2, 1, 0, 0);      // a bottom row for some buttons maybe..
  grid->addWidget(scroll, 0, 0);
  QHBoxLayout* hBox = new QHBoxLayout();
  grid->addLayout(hBox, 1, 0);
  //  grid->addWidget(voidButton, 1, 0);
  hBox->addWidget(loadIncluded);
  hBox->addWidget(includedGeneCount);
}

void AnnotationViewer::updateWidgets(probe_data* pdata){
  //  clearWidgets();
  //cout << "cleared Widgets, and am about to make some new ones" << endl;
  int myId = client->userId;
  for(uint i=0; i < pdata->annotations.size(); i++){
    /// ok we'll need to find out what our identity is later, but for now assume we can edit.. 
    /// and we'll need to connect some signals and stuff here later on..
    // use the client pointer to determine what user id I have.. and so forth..
    //cout << "myId             : " << myId << endl
    //	 << "annotation owner : " << pdata->annotations[i].owner << endl;
    bool canEdit = (myId == pdata->annotations[i].owner);
    AnnotationWidget* temp = new AnnotationWidget(pdata->annotations[i], client, canEdit, frame);
    vBox->addWidget(temp);     // this might be deadly for performance, I'll just have to check and see if it works..
    temp->show();
    temp->setFixedWidth(scroll->viewport()->width());
    temp->setFixedHeight(temp->sizeHint().height());
    connect(temp, SIGNAL(includeMe(bool)), this, SLOT(includeGenes(bool)) );
    connect(temp, SIGNAL(updateAnnotation(int, QString)), this, SIGNAL(updateAnnotation(int, QString)) );
    widgets.push_back(temp);
    //cout << "made new widget and what happended" << endl;
  }
}

void AnnotationViewer::updateSessionWidgets(probe_data* pdata){
  //clearWidgets();
  //cout << "cleared Widgets, and am about to make some new ones" << endl;
  int myId = client->userId;
  for(uint i=0; i < pdata->sessions.size(); i++){
    /// ok we'll need to find out what our identity is later, but for now assume we can edit.. 
    /// and we'll need to connect some signals and stuff here later on..
    // use the client pointer to determine what user id I have.. and so forth..
    //    cout << "myId             : " << myId << endl
    //	 << "annotation owner : " << pdata->sessions[i].owner << endl;
    bool canEdit = (myId == pdata->sessions[i].owner);
    AnnotationWidget* temp = new AnnotationWidget(pdata->sessions[i], client, canEdit, frame);
    vBox->addWidget(temp);     // this might be deadly for performance, I'll just have to check and see if it works..
    temp->show();
    temp->setFixedWidth(scroll->viewport()->width());
    temp->setFixedHeight(temp->sizeHint().height());
    connect(temp, SIGNAL(includeMe(bool)), this, SLOT(includeGenes(bool)) );
    connect(temp, SIGNAL(updateSessionAnnotation(int, vector<QString>)), this, SIGNAL(updateSessionAnnotation(int, vector<QString>)) );
    widgets.push_back(temp);
    // cout << "made new widget and what happended" << endl;
  }
}

void AnnotationViewer::addLabel(QString labelString){
  QLabel* tempLabel = new QLabel(labelString, frame);
  //QLabel* tempLabel = new QLabel(labelString, frame, "label", Qt::AlignCenter);
  tempLabel->setBackgroundColor(QColor(226, 174, 174));
  tempLabel->setFont(QFont("Conga", 15, QFont::Bold));
  tempLabel->setAlignment(Qt::AlignCenter);
  tempLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum));
  //  tempLabel->setFont(QFont("Helmet", 15, QFont::Bold));
  vBox->addWidget(tempLabel);
  scroll->resizeContents(width(), height());
  //scroll->resize(width(), frame->height());
  //frame->resize(width(), height());
  tempLabel->setFixedWidth(width());
  //  tempLabel->resize(width(), tempLabel->height());
  tempLabel->show();
  labels.push_back(tempLabel);
  //cout << "My current size is " << width() << ", " << height() << endl
  //    << "Label size is      " << tempLabel->width() << ", " << tempLabel->height() << endl
  //    << "Viewport size is   " << scroll->viewport()->width() << ", " << scroll->viewport()->height() << endl;
}
  
    
void AnnotationViewer::clearWidgets(){
  for(uint i=0; i < widgets.size(); i++){
    widgets[i]->hide();
    delete widgets[i];
  }
  widgets.resize(0);
  for(uint i=0; i < labels.size(); i++){
    labels[i]->hide();
    delete labels[i];
  }
  labels.resize(0);
  // and lets clear the includedGenes map
  map<int, int>::iterator it;
  while(includedGenes.size() > 0){
    includedGenes.erase((*includedGenes.begin()).first);
  }
  // and set the label appropriately, and disable the button ... 
  includedGeneCount->setText("Set size : 0");
  loadIncluded->setDisabled(true);
}

void AnnotationViewer::resizeEvent(QResizeEvent* e){
  // need to resize all the widgets. 
  frame->resize(scroll->viewport()->width(), frame->height());
  for(uint i=0; i < widgets.size(); i++){
    widgets[i]->setFixedWidth(scroll->viewport()->width());
    widgets[i]->setFixedHeight(widgets[i]->sizeHint().height());
  }
  for(uint i=0; i < labels.size(); i++){
    labels[i]->setFixedWidth(scroll->viewport()->width());
  }
  //update();
}

void AnnotationViewer::includeGenes(bool include){
  AnnotationWidget* aw = (AnnotationWidget*)sender();
  map<int, int>::iterator it;
  if(include){
    for(uint i=0; i < aw->annotation.otherMembers.size(); i++){
      it = includedGenes.find(aw->annotation.otherMembers[i]);
      if(it == includedGenes.end()){
	includedGenes.insert(make_pair(aw->annotation.otherMembers[i], 1));
      }else{
	(*it).second++;
      }
    }
  }else{
    for(uint i=0; i < aw->annotation.otherMembers.size(); i++){
      it = includedGenes.find(aw->annotation.otherMembers[i]);
      if((*it).second > 1){
	(*it).second--;
      }else{
	includedGenes.erase((*it).first);
      }
    }
  }
    // at this point we should check disable/able load button..
  // more than one, -it doesn't make much sense to load the current one
  loadIncluded->setDisabled(includedGenes.size() < 2);
  QString setSize;
  setSize.sprintf("Set size : %d", (int)includedGenes.size());
  includedGeneCount->setText(setSize);
  //cout << "Number of Genes Included : " << includedGenes.size() << endl;
}

void AnnotationViewer::loadIncludedGenes(){
  //cout << "loadIncludedGenes function " << endl;
  vector<int> g;
  g.reserve(includedGenes.size());
  map<int, int>::iterator it;
  for(it = includedGenes.begin(); it != includedGenes.end(); it++){
    g.push_back((*it).first);
  }
  //cout << "emitting loadTheseGenes size of index is " << g.size() << endl;
  emit loadTheseGenes(g, "User Annotation");
}
