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

#include "historyWidget.h"
#include "../boolParser/boolParser.h"
#include <qfont.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <iostream>

using namespace std;

HistoryWidget::HistoryWidget(deque<IndexSet>* islist, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  listPointer = islist;
  // hmm, could check the list here, but let's not bother, we can just call the thingy later on..
  setCaption("Index History");
  int bsize = qApp->font().pointSize();

  // QLabel* szLabel = new QLabel("size", this, "szLabel");
  //szLabel->setAlignment(Qt::AlignCenter);
  //szLabel->setFixedWidth(50);
  QLabel* incLabel = new QLabel("I", this, "incLabel");
  incLabel->setFixedWidth(25);
  incLabel->setAlignment(Qt::AlignCenter);
  QLabel* excLabel = new QLabel("X", this, "excLabel");
  excLabel->setAlignment(Qt::AlignCenter);
  excLabel->setFixedWidth(25);
  incLabel->setFont(QFont("Times", bsize+3, QFont::Black));
  excLabel->setFont(QFont("Times", bsize+3, QFont::Black));

  QPushButton* combButton = new QPushButton("Load Combination", this, "combButton");
  combButton->setFixedWidth(150);
  connect(combButton, SIGNAL(clicked()), this, SLOT(loadCombination()) );

  QPushButton* evalButton = new QPushButton("&Eval", this, "evalButton");
  connect(evalButton, SIGNAL(clicked()), this, SLOT(evalBoolean()) );
  QPushButton* boolButton = new QPushButton("&Load", this, "boolButton");
  connect(boolButton, SIGNAL(clicked()), this, SLOT(loadBoolean()) );
  boolLine = new QLineEdit(this, "boolLine");
  boolSelect = new QLabel(this, "boolSelect");
  boolSelect->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  boolSelect->setFixedWidth(60);
  boolSelect->setIndent(5);

  selectLabel = new QLabel(this);
  selectLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  selectLabel->setMargin(5);

  QVBoxLayout* bbox = new QVBoxLayout(this);

  box = new QVBoxLayout(bbox);
  //QHBoxLayout* labelRow = new QHBoxLayout(box);
  
  QHBoxLayout* botRow = new QHBoxLayout(bbox);
  QHBoxLayout* boolRow = new QHBoxLayout(bbox);
  botRow->addWidget(combButton);
  botRow->addWidget(selectLabel);
  botRow->addWidget(incLabel);
  botRow->addWidget(excLabel);
  
  boolRow->addWidget(evalButton);
  boolRow->addWidget(boolButton);
  boolRow->addWidget(boolLine);
  boolRow->addWidget(boolSelect);

  //  labelRow->addSpacing(40);
  // labelRow->addWidget(szLabel);
  // labelRow->addStretch();
  // labelRow->addWidget(incLabel);
  //labelRow->addWidget(excLabel);
  // which is pretty much I need to do..
  
 
}

void HistoryWidget::newIndex(){
  // first make sure we have enough widgets to assign..
  for(uint i=indexWidgets.size(); i < listPointer->size(); i++){
    IndexWidget* iw = new IndexWidget(this);
    connect(iw, SIGNAL(switchToggled()), this, SLOT(checkSelection()) );
    indexWidgets.push_back(iw);   // no point naming it..
    box->addWidget(iw);
    iw->show();
    connect(iw, SIGNAL(restoreIndex(vector<int>, int, QString)), this, SIGNAL(restoreIndex(vector<int>, int, QString)) );
  }
  // and go through and assing..
  vector<bool> include(indexWidgets.size());
  vector<bool> exclude(indexWidgets.size());    // ugly, but I can't think of a good way of doing this.. 
  for(uint i=0; i < indexWidgets.size(); i++){
    include[i] = indexWidgets[i]->Include();
    exclude[i] = indexWidgets[i]->Exclude();
  }
  for(uint i=0; i < listPointer->size() && i < indexWidgets.size(); i++){    // double check shouldn't be necessary, but if multithreaded behaviour of sorts..
    if(i == 0){
      indexWidgets[i]->setIndex((*listPointer)[i], i+1);  // hope this isn't too slow..
    }else{
      indexWidgets[i]->setIndex((*listPointer)[i], include[i-1], exclude[i-1], i+1);
    }
  }
  setFixedHeight(indexWidgets.size()*25 + 25 + 25);
  checkSelection();
}
    
QSize HistoryWidget::sizeHint() const {
  return(QSize(250, indexWidgets.size()*25 + 25 + 25));
}

void HistoryWidget::checkSelection(){
  selected.resize(0);
  cout << "check selection selected is : " << selected.size() << endl;
  for(uint i=0; i < indexWidgets.size(); i++){
    if(indexWidgets[i]->Include()){
      selected = indexWidgets[i]->indexSet.Or(selected);
      cout << "\tincluding from " << indexWidgets[i]->indexSet.description() << "  size now : " << selected.size() << endl;
    }
  }
  for(uint i=0; i < indexWidgets.size(); i++){
    if(indexWidgets[i]->Exclude()){
      selected = indexWidgets[i]->indexSet.Not(selected);
      cout << "\texcluding from " << indexWidgets[i]->indexSet.description() << "  size now : " << selected.size() << endl;
    }
  }
  QString text;
  text.setNum(selected.size());
  selectLabel->setText(text);
}

void HistoryWidget::loadCombination(){
  if(!selected.size()){
    return;
  }
  emit loadProbes(selected, "Historical Combination");
}

void HistoryWidget::evalBoolean(){
  if(boolLine->text().length()){
    loadBoolean(boolLine->text(), false);
  }
}

void HistoryWidget::loadBoolean(){
  if(boolLine->text().length()){
    loadBoolean(boolLine->text(), true);
  }
}

void HistoryWidget::loadBoolean(QString text, bool loadIt){
  // first we need to make a vector of vectors.. fairly easy..
  vector<vector<int> > vts;
  for(uint i=0; i < indexWidgets.size(); i++){
    vts.push_back(indexWidgets[i]->indexSet.index());
  }
  BoolParser parser(text, vts);
  if(parser.ok()){
    set<int> index = parser.parseExpression();
    // and make vector and emit or something..
    QString numstring;
    numstring.setNum(index.size());
    boolSelect->setText(numstring);
    // and so on.. 
    if(loadIt){
      vector<int> vindex(index.begin(), index.end());
      emit loadProbes(vindex, "Boolean Combination");
    }
  }
  
}
