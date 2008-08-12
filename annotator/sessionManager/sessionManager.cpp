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

#include "sessionManager.h"
#include <qwidget.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <set>
#include <qscrollview.h>
#include <qframe.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qpalette.h>
#include <qsize.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qregexp.h>
#include <qfont.h>
#include <vector>
#include <iostream>

using namespace std;

SessionManager::SessionManager(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  currentlyDisplayedSession = 0;
  QGridLayout* grid = new QGridLayout(this, 4, 2, 2, 2);
  preferredWidth = 200;
  grid->setColStretch(0, 1);
  grid->setColStretch(1, 1);

  scroll = new QScrollView(this);
  frame = new QFrame(scroll->viewport());
  scroll->addChild(frame);

  vBox = new QVBoxLayout(frame, 0, 2);
  vBox->setDirection(QBoxLayout::Up);

  descriptionField = new QTextEdit(this, "description");
  titleField = new QLineEdit(this, "title");
  keywordField = new QLineEdit(this, "keyword");

  selectedGeneNo = new QLineEdit("0  Genes Included", this, "selectedGeneNo");
  //  selectedGeneNo = new QLabel("0  Genes Included", this, "selectedGeneNo");

  commitChanges = new QPushButton("Commit Changes", this, "commitChanges");
  connect(commitChanges, SIGNAL(clicked()), this, SLOT(updateDescriptions()) );
  // commitChanges->setFlat(true);        // enabled if the currently displayed info is owned by the user.. hmm. let's see how we can do
  setDisableColor(commitChanges, true);

  QPushButton* updateSessions = new QPushButton("Update", this, "update");
  connect(updateSessions, SIGNAL(clicked()), this, SIGNAL(updateSessions()) );

  loadGenes = new QPushButton("Load Genes", this, "LoadGenes");
  connect(loadGenes, SIGNAL(clicked()), this, SLOT(selectGenes()) );
  setDisableColor(loadGenes, true);

  copyToSession = new QPushButton("Copy to Current Session", this, "copyToSession");
  connect(copyToSession, SIGNAL(clicked()), this, SLOT(copyIncludedGenesToCurrentSession()) );
  //copyToSession->setFlat(true);         // disabled if nothing selected,, otherwise enabled.. 
  setDisableColor(copyToSession, true);
  appendSession = new QPushButton("Append", this, "appendSession");
  connect(appendSession, SIGNAL(clicked()), this, SLOT(loadAndAppend()) );
  //appendSession->setFlat(true);         // enabled only if one is selected, and readOnly is false.. 
  setDisableColor(appendSession, true);

  grid->addMultiCellWidget(scroll, 0, 1, 1, 1);
  grid->addWidget(selectedGeneNo, 2, 1); 
  //  grid->addWidget(selectedGeneNo, 2, 1, AlignRight); 

  grid->addWidget(titleField, 0, 0);
  grid->addWidget(descriptionField, 1, 0);

  QHBoxLayout* leftBox = new QHBoxLayout();
  grid->addMultiCell(leftBox, 3, 3, 0, 1);
  leftBox->addWidget(commitChanges);
  
  // QHBoxLayout* rightBox = new QHBoxLayout();
  //grid->addLayout(rightBox, 3, 1);

  leftBox->addWidget(loadGenes);
  leftBox->addSpacing(1);
  leftBox->addWidget(copyToSession);
  leftBox->addSpacing(1);
  leftBox->addWidget(appendSession);
  leftBox->addSpacing(10);
  
  leftBox->addWidget(updateSessions);

//    rightBox->addWidget(loadGenes);
//    rightBox->addWidget(copyToSession);
//    rightBox->addWidget(appendSession);
//    rightBox->addSpacing(10);
  
//    rightBox->addWidget(updateSessions);
  
  grid->addWidget(keywordField, 2, 0);
  resize(450, 200);
}

void SessionManager::addSessionWidget(SessionWidget* sw){
  sw->reparent(frame, QPoint(0, 0));
  vBox->addWidget(sw);
  sessions.insert(sw);
  sw->show();
  QSize fsize = sw->sizeHint();
  if(fsize.width() > preferredWidth){
    preferredWidth = fsize.width();
  }
  connect(sw, SIGNAL(displayMe()), this, SLOT(displayData()) );
  connect(sw, SIGNAL(includeMe(bool)), this, SLOT(includeData(bool)) );
  //  cout << "recommended width = " << fsize.width() << endl;
  //scroll->viewport()->resize(fsize.width()-18, height());
  //scroll->update();
  //resize(preferredWidth+20, height());
}

void SessionManager::displayData(){
  // do some display stuff, extract data and put in later..
  QString text;
  SessionWidget* sw = (SessionWidget*)sender();
  text = sw->sessionTitle();
  dirtyText(text);
  titleField->setText(text);
  //  titleField->setText(sw->sessionTitle());
  text = sw->sessionDescription();
  dirtyText(text);
  descriptionField->setText(text);
  //  descriptionField->setText(sw->sessionDescription());
  currentlyDisplayedSession = sw->sId();

//  set<QString>::iterator it;
//  QString keywords;
//  for(it = sw->sessionKeyWords().begin(); it != sw->sessionKeyWords().end(); it++){
//    keywords += (*it);
//    keywords += ", ";
//  }
//  dirtyText(keywords);
//  keywordField->setText(keywords);
  if(sw->canEdit()){
    //    commitChanges->setFlat(false);
    setDisableColor(commitChanges, false);
    titleField->setReadOnly(false);
    descriptionField->setReadOnly(false);
    keywordField->setReadOnly(false);
  }else{
    //commitChanges->setFlat(true);
    setDisableColor(commitChanges, true);
    titleField->setReadOnly(true);
    descriptionField->setReadOnly(true);
    keywordField->setReadOnly(true);
  }
}

void SessionManager::includeData(bool include){
  //cout << "including genes.. ho yeah.." << endl;
  SessionWidget* sw = (SessionWidget*)sender();
  set<int>::iterator it;
  map<int, int>::iterator mit;
  if(include){
    //cout << "including gene " << endl;
    includedSessions.insert(sw);
    for(it = sw->genes.begin(); it != sw->genes.end(); it++){
      //cout << "\t: " << (*it) << endl;
      mit = includedGeneIds.find(*it);
      if(mit != includedGeneIds.end()){
	(*mit).second++;
      }else{
	includedGeneIds.insert(make_pair(*it, 1));
      }
      //      includedGeneIds.insert(*it);
    }
  }else{
    includedSessions.erase(sw);
    for(it = sw->genes.begin(); it != sw->genes.end(); it++){
      mit = includedGeneIds.find(*it);
      if((*mit).second > 1){
	(*mit).second--;
      }else{
	includedGeneIds.erase(*it);
      }
      //includedGeneIds.erase(*it);  // which should just decrement the count??? 
    }
  }
  //for(mit=includedGeneIds.begin(); mit != includedGeneIds.end(); mit++){
  //  cout << "\t:" << (*mit).first << "\t: " << (*mit).second << endl;
  //}
  QString number;
  number.setNum(includedGeneIds.size());
  number += "  Genes Included";
  setDisableColor(loadGenes, (includedGeneIds.size() < 1));
  if(includedSessions.size() == 1){
    setDisableColor(appendSession, !(*includedSessions.begin())->canEdit());
  }else{
    setDisableColor(appendSession, true);
  }
  setDisableColor(copyToSession, (includedSessions.size() < 1) );
  selectedGeneNo->setText(number);
  // and then do some update on the thingy I suppose.. 
}

void SessionManager::clearSessionWidgets(){
  // delete all the session widgets.. --
  // first just erase from includedSessions.
  set<SessionWidget*>::iterator it;
  //while(includedSessions.size()){
    // it = includedSessions.begin();
     //SessionWidget* tw = (*it);
     //includedSessions.erase(tw);
     //cout << "erasing included sessions " << endl;
     //}
  it = includedSessions.begin();
  while(it != includedSessions.end()){
     SessionWidget* tw = (*it);
     it++;
     includedSessions.erase(tw);
     }
  //for(it = includedSessions.begin(); it != includedSessions.end(); it++){
  //  cout << "Erasing from included sessions " << endl;
  //  SessionWidget* tw = (*it);
  //  includedSessions.erase(tw);
  //}
  // then delete and erase.. hmm. 

  set<SessionWidget*>::iterator sit;
  sit = sessions.begin();
  while(sit != sessions.end()){
      SessionWidget* tw = (*sit);
      sit++;
      sessions.erase(tw);
      delete tw;
      //cout << "New erase function.. *sit is " << *sit << endl;
      }

  //for(sit = sessions.begin(); sit != sessions.end(); sit++){
    // can't remember if I can do this, but lets have a go..
    //cout << "Erasing from all sessions. well maybe. " << endl;
    //SessionWidget* tw = (*sit);
    //(*it)->hide();
    //sessions.erase(tw);
    //delete tw;
    //delete(*it);
  //}

  map<int, int>::iterator mit;
  while(includedGeneIds.size() > 0){
     includedGeneIds.erase((*includedGeneIds.begin()).first);
  }

  //for(mit = includedGeneIds.begin(); mit != includedGeneIds.end(); mit++){
  //  includedGeneIds.erase((*mit).first);
  //}
  selectedGeneNo->setText("0  Genes Selected");
  //  preferredWidth = 200;
  setDisableColor(commitChanges, true);
  setDisableColor(copyToSession, true);
  setDisableColor(appendSession, true);
  setDisableColor(loadGenes, true);
}
  
void SessionManager::selectGenes(){
  vector<int> geneSelection;
  map<int, int>::iterator it;
  for(it = includedGeneIds.begin(); it != includedGeneIds.end(); it++){
    geneSelection.push_back((*it).first);
  }
  emit newIndex(geneSelection, "User Session");
}

QSize SessionManager::sizeHint(){
  return(QSize(preferredWidth+20, 300));
}

void SessionManager::setDisableColor(QPushButton* button, bool disabled){
  if(!disabled){
    button->setPaletteBackgroundColor(palette().color(QPalette::Active, QColorGroup::Button));
  }else{
    button->setPaletteBackgroundColor(QColor(100, 150, 150));
  }
  button->setDisabled(disabled);
}

void SessionManager::cleanText(QString& note){
  note.replace(QRegExp("<"), "&lt");
  note.replace(QRegExp(">"), "&gt");
  note.replace(QRegExp("\\|"), "&pipe");
  note.replace(QRegExp("'"), "&apo");
}

void SessionManager::dirtyText(QString& note){
  note.replace(QRegExp("&lt"), "<");
  note.replace(QRegExp("&gt"), ">");
  note.replace(QRegExp("&pipe"), "|");
  note.replace(QRegExp("&apo"), "'");
}

void SessionManager::updateDescriptions(){
  QString t = titleField->text();
  QString d = descriptionField->text();
  QString k = keywordField->text();
  // again, clean up these guys..
  cleanText(t);
  cleanText(d);
  cleanText(k);
  vector<QString> words;
  words.push_back(t);
  words.push_back(d);
  // and now split k on space or something like that using some 
  // qt specific functions. We could use string, but not much point since
  // we are already using the graphical stuff..
  QStringList sList = QStringList::split(QRegExp("\\s+|,"), k);
  for(QStringList::iterator it=sList.begin(); it != sList.end(); it++){
    words.push_back(*it);
  }
  emit changeDescriptions(currentlyDisplayedSession, words);
  for(int i=0; i < words.size(); i++){
    cout << i << " : " << words[i] << endl;
  }
  // and hope for the best.. -- maybe run an update later on.. -we can check.. !! 
}

void SessionManager::copyIncludedGenesToCurrentSession(){
  map<int, int>::iterator it;
  vector<int> cGenes;
  cGenes.reserve(includedGeneIds.size());
  for(it = includedGeneIds.begin(); it != includedGeneIds.end(); it++){
    cGenes.push_back((*it).first);    // second is the count !! 
  }
  emit copyToCurrentSession(cGenes);
}

void SessionManager::loadAndAppend(){
  if(includedSessions.size() != 1) { return; }
  SessionWidget* sw = *includedSessions.begin();
  emit appendToSession(sw->sId(), sw->sessionTitle(), sw->sessionDescription(), sw->sessionKeyWords());
  cout << "emitting appendToSession" << endl;
}
  
