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

#include "newSession.h"
#include "annotator.h"
#include "sessionManager/sessionManager.h"
#include "sessionManager/sessionWidget.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <set>
#include <map>
#include <qstring.h>
#include <qlayout.h>
#include <qsize.h>
#include <iostream>

using namespace std;

Annotator::Annotator(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  // set sizeHint
  //sizeHint = QSize(500, 200);
  currentSessionId = 0;
  currentSessionTitle = "";
  currentSessionDescription = "";
  nextSessionTitle = "";
  nextSessionDescription = "";       // this may be redundant.. 

  sessionCreator = new NewSession();      // no arguments.. 
  connect(sessionCreator, SIGNAL(createSession(vector<string>)), this, SLOT(makeNewSession(vector<string>)));
  QPushButton* showSessionCreator = new QPushButton("New Session", this, "newSession");
  connect(showSessionCreator, SIGNAL(clicked()), sessionCreator, SLOT(show()) );


  sessionManager = new SessionManager();   // maybe it's good.. who knows, maybe we can do something funky..
  sessionManager->setPalette(palette());
  connect(sessionManager, SIGNAL(updateSessions()), this, SIGNAL(updateSessions()) );
  connect(sessionManager, SIGNAL(newIndex(vector<int>, QString)), this, SIGNAL(newIndex(vector<int>, QString)) );
  connect(sessionManager, SIGNAL(changeDescriptions(int, vector<QString>)), this, SIGNAL(updateSessionDescription(int, vector<QString>)) );
  connect(sessionManager, SIGNAL(copyToCurrentSession(vector<int>)), this, SLOT(copyGenesToCurrentSession(vector<int>)) );
  connect(sessionManager, SIGNAL(appendToSession(int, QString, QString, set<QString>)), this, SLOT(setCurrentSession(int, QString, QString, set<QString>)) );
  //QPushButton* getSessions = new QPushButton("Session List", this, "sessionList");
  //connect(getSessions, SIGNAL(clicked()), this, SIGNAL(updateSessions()) );
  // just put an empty sessionWidget into it to see how it looks..
  //SessionWidget* testWidget = new SessionWidget();
  //sessionManager->addSessionWidget(testWidget);
  //SessionWidget* testWidget2 = new SessionWidget();
  //sessionManager->addSessionWidget(testWidget2);
 
  int labelIndent = 3;

  sessionTitle = new QLabel("Session Title: ", this, "sessionTitle");
  sessionTitle->setIndent(labelIndent);
  commentArea = new QTextEdit(this, "commentArea");
  commentArea->setMinimumHeight(40);
  QLabel* currentAssociation = new QLabel("Comment Association :", this, "association");
  currentAssociation->setIndent(labelIndent);
  sessionAssociation = new QLabel("Current Session", this, "sessionAssociation");
  geneAssociation = new QLabel("Current Gene", this, "geneAssociation");
  
  //  sessionAssociation->setIndent(3);
  //geneAssociation->setIndent(3);

  QPushButton* commitButton = new QPushButton("&Commit", this, "commit");
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commit()) );

  QPushButton* loadSession = new QPushButton("&Load Session", this, "loadSession");
  connect(loadSession, SIGNAL(clicked()), sessionManager, SLOT(show()) );

  QPushButton* currentGene = new QPushButton("&Gene", this, "gene");
  QPushButton* currentSaved = new QPushButton("&Saved", this, "saved");
  currentSession = new QPushButton("S&ession", this, "session");
  onButton = currentGene;

  currentGene->setToggleButton(true);
  currentSaved->setToggleButton(true);
  currentSession->setToggleButton(true);

  currentGene->setOn(true);
  currentSaved->setOn(false);
  currentSession->setOn(true); // though there may be no session loaded.. 

  connect(currentGene, SIGNAL(clicked()), this, SLOT(switchAssociation()) );
  connect(currentSaved, SIGNAL(clicked()), this, SLOT(switchAssociation()) );
  connect(currentSession, SIGNAL(clicked()), this, SLOT(switchSessionAssociation()) );

  switchButtons.insert(make_pair(currentGene, QString("GENE")));
  associationStates.insert(make_pair(currentGene, 1));
  switchButtons.insert(make_pair(currentSaved, QString("SAVED SET")));
  associationStates.insert(make_pair(currentSaved, 2));
  switchButtons.insert(make_pair(currentSession, QString("SESSION")));
  associationStates.insert(make_pair(currentSession, 3));


 //   grid = new QGridLayout(this, 7, 4);
//    grid->setColStretch(1, 4);
//    //grid->setColStretch(5, 3);
//    grid->setColStretch(0, 0);
//    grid->setColStretch(2, 0);

//    grid->addColSpacing(0, 4);
//    grid->addColSpacing(3, 5);
//    //grid->addColSpacing(6, 5);

//    //grid->addColSpacing(5, 75);

//    grid->addRowSpacing(0, 5);
//    grid->addRowSpacing(2, 3);
//    grid->addRowSpacing(6, 5);
  
//    grid->addWidget(sessionTitle, 1, 1);
//    grid->addWidget(commentArea, 3, 1);
  
//    //grid->addMultiCellWidget(sessionManager, 1, 5, 5, 5) ;
//    //grid->addWidget(getSessions, 6, 5);

//    QGridLayout* assocGrid = new QGridLayout(2, 2);
//    grid->addLayout(assocGrid, 4, 1);
//    assocGrid->addWidget(currentAssociation, 0, 0);
//    assocGrid->addWidget(sessionAssociation, 0, 1);
//    assocGrid->addWidget(geneAssociation, 1, 1);
  
//    QVBoxLayout* vBox = new QVBoxLayout();
//    grid->addLayout(vBox, 3, 2);
//    //  grid->addLayout(vBox, 0, 1);
//    vBox->addWidget(currentGene);
//    vBox->addWidget(currentSaved);
//    vBox->addWidget(currentSession);
//    vBox->addStretch();
  
//    QHBoxLayout* hBox = new QHBoxLayout();
//    grid->addMultiCell(hBox, 6, 6, 1, 2);
//    hBox->addWidget(showSessionCreator);
//    hBox->addWidget(commitButton);
//    hBox->addWidget(loadSession);
//    hBox->addStretch();
//    resize(500, 50);

  QVBoxLayout* bBox= new QVBoxLayout(this, 0, 0);
  
  bBox->addSpacing(4);
  bBox->addWidget(sessionTitle);
  bBox->addWidget(commentArea);
  
  QHBoxLayout* switchBox = new QHBoxLayout(0);
  bBox->addLayout(switchBox);
  switchBox->addWidget(currentGene);
  switchBox->addWidget(currentSaved);
  switchBox->addWidget(currentSession);
  switchBox->addStretch();
  switchBox->addWidget(commitButton);
  bBox->addSpacing(2);

  QGridLayout* assocGrid = new QGridLayout(2, 2);
  assocGrid->setColStretch(0, 0);
  assocGrid->setColStretch(1, 1);
  bBox->addLayout(assocGrid);
  assocGrid->addWidget(currentAssociation, 0, 0);
  assocGrid->addWidget(sessionAssociation, 0, 1);
  assocGrid->addWidget(geneAssociation, 1, 1);
  bBox->addSpacing(2);

  QHBoxLayout* bottom = new QHBoxLayout(0);
  bBox->addLayout(bottom);
  bottom->addWidget(showSessionCreator);
  bottom->addWidget(loadSession);
  //bottom->addStretch();   // hmmm .. 
  
  sessionCreator->setPalette(palette());
}

Annotator::~Annotator(){
  cout << "destroying annotator " << endl;

}

void Annotator::switchAssociation(){
  QString association;   // should use a variable.. hmm 
  QPushButton* sendButton = (QPushButton*)sender();
  if(sendButton == onButton){        // 
    onButton = 0;
    association = "NO GENE";
    //offButtons.insert(sendButton);
  }else{
    if(onButton){
      onButton->setOn(false);
    }
    onButton=sendButton;
    association = switchButtons[onButton];
    //offButtons.erase(sendButton);
  }
  geneAssociation->setText(association);
}

void Annotator::switchSessionAssociation(){
  if(currentSession->isOn()){
    sessionAssociation->setText("Current Session");
  }else{
    sessionAssociation->setText("NONE");
  }
}

void Annotator::commit(){
  int state = 0;
  if(onButton != 0){
    map<QPushButton*, int>::iterator it = associationStates.find(onButton);
    if(it != associationStates.end()){
      state = (*it).second;
    }
  }
  emit newAnnotation(state, currentSessionId, commentArea->text());

  commentArea->clear();
  //keyWords->clear();
}

QSize Annotator::sizeHint(){
  return(QSize(500, 200));
}

void Annotator::paletteChange(const QPalette& oldPalette){
  sessionCreator->setPalette(palette());
  sessionManager->setPalette(palette());
}

void Annotator::sessionCreated(int sessionId){
  // this signal should only arrive immediately after a session is created, which should only 
  // happen after the new Session has sent a thing.. to the client and the server..
  // essentially clear the newSession object and assign the session id to the current id, and stuff
  currentSessionId = sessionId;
  currentSessionTitle = nextSessionTitle;
  currentSessionDescription = nextSessionDescription;
  currentKeywords = nextKeywords;
  // and do something to update.. 
  // clear the newSession thing !! .. 
  sessionCreator->clearText();
  sessionCreator->hide();
  cout << "Current Session : " << sessionId << endl
       << "        Title   : " << currentSessionTitle << endl
       << "  Description   : " << currentSessionDescription << endl;
  // lets set the text of the Session title.. !
  QString title("Session Title:  ");
  title += currentSessionTitle.c_str();
  sessionTitle->setText(title);
}

void Annotator::makeNewSession(vector<string> words){
  if(words.size() == 0){ return; }
  if(words[0].size() == 0) { return; }
  nextSessionTitle = words[0];
  nextKeywords.resize(0);
  if(words.size() > 1){
    nextSessionDescription = words[1];
    for(int i=2; i < words.size(); i++){
      nextKeywords.push_back(words[i]);
    }
  }else{
    nextSessionDescription = "";
  }
  emit createSession(words);
}

void Annotator::clearSessions(){
  sessionManager->clearSessionWidgets();
}

void Annotator::addSession(SessionWidget* sw){
  sessionManager->addSessionWidget(sw);
  //int w;
  //  if(sessionManager->sizeHint().width() < width()/3){
  //  w = sessionManager->sizeHint().width();
  //}else{
  //  w = width()/3;
  //}
  //grid->addColSpacing(5, w);
  //  grid->addColSpacing(5, sessionManager->sizeHint().width());
  //repaint();
}

void Annotator::copyGenesToCurrentSession(vector<int> cGenes){
  QString empty("");
  if(currentSessionId > 0){
    emit copyToCurrentSession(currentSessionId, cGenes, empty);
    return;
  }
  cout << "No Current Session Active, does not make sense to copy to Current Session " << endl;
}

void Annotator::setCurrentSession(int sId, QString ti, QString des, set<QString> kw){
  // simply set the current session.. 
  cout << "Setting Current Session " << endl;
  currentSessionId = sId;
  sessionTitle->setText(ti);
  currentSessionDescription = des.latin1();
  currentKeywords.resize(0);
  for(set<QString>::iterator it=kw.begin(); it != kw.end(); it++){
    currentKeywords.push_back((*it).latin1());
  }
}
