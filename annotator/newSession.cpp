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
#include <qwidget.h>
#include <vector>
#include <qlineedit.h>
#include <qtextedit.h>
#include <iostream>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qregexp.h>

using namespace std;

NewSession::NewSession(QWidget* parent, const char* name)
  : QWidget(parent, name)
{

  descriptionArea = new QTextEdit(this, "description");
  titleLine = new QLineEdit(this, "titleLine");
  keyWordLine = new QLineEdit(this, "keywordline");

  QLabel* dLabel = new QLabel("Description", this, "descriptionLabel");
  QLabel* tLabel = new QLabel("Title", this, "titleLabel");
  QLabel* kLabel = new QLabel("Keywords", this, "keyLabel");

  QPushButton* commitButton = new QPushButton("&Commit", this, "commit");
  QPushButton* cancelButton = new QPushButton("Cancel", this, "cancel");
  QPushButton* clearButton = new QPushButton("Clear", this, "clear");
  
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commit()) );
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()) );  // just hide it !
  connect(clearButton, SIGNAL(clicked()), this, SLOT(clearText()) );

  // ok.. some layout..
  QGridLayout* grid = new QGridLayout(this, 3, 3, 1, 5);
  grid->setColStretch(1, 4);
  grid->setColStretch(2, 0);
  grid->setColStretch(0, 0);

  grid->addColSpacing(2, 60);
  
  grid->addWidget(tLabel, 0, 0, AlignRight);
  grid->addWidget(dLabel, 1, 0, AlignRight|AlignTop);
  grid->addWidget(kLabel, 2, 0, AlignRight|AlignTop);

  grid->addWidget(titleLine, 0, 1);
  grid->addWidget(descriptionArea, 1, 1);
  grid->addWidget(keyWordLine, 2, 1);

  QVBoxLayout* vBox = new QVBoxLayout(2);
  grid->addMultiCell(vBox, 0, 2, 2, 2);
  vBox->addWidget(commitButton);
  vBox->addWidget(cancelButton);
  vBox->addStretch();
  vBox->addWidget(clearButton);
  resize(350, 50);
}

NewSession::~NewSession(){
  cout << "deleting new session widget " << endl;
}

void NewSession::replaceChars(QString& note){
  note.replace(QRegExp("<"), "&lt");
  note.replace(QRegExp(">"), "&gt");
  note.replace(QRegExp("\\|"), "&pipe");
  note.replace(QRegExp("'"), "&apo");
}

void NewSession::commit(){
  // collect the words from the various areas..
  vector<string> words(0);
  QString t = titleLine->text();
  QString d = descriptionArea->text();
  QString k = keyWordLine->text();
  // replace occurences of <, >, ' | 
  replaceChars(t);
  replaceChars(d);
  replaceChars(k);

  words.push_back(t.latin1());
  words.push_back(d.latin1());
  //words.push_back(titleLine->text().latin1());
  //  words.push_back(descriptionArea->text().latin1());
  string keyLine = k.latin1();
  //  string keyLine = keyWordLine->text().latin1();
  // ok lets push .
  int first = keyLine.find_first_not_of(" \t\n");
  int next;
  while(first != keyLine.npos){
    next = keyLine.find_first_of(" \t\n", first);
    if(next == keyLine.npos){
      next = keyLine.size();
    }
    words.push_back(keyLine.substr(first, next-first));
    first = keyLine.find_first_not_of(" \t\n", next);
  }
  //  words.push_back(keyLine);
  //  words.push_back(keyWordLine->text().latin1());
  // vector<string> not QString, because of the find_first_not_of( function in string, which
  // makes splitting on whitespace much easier.. 

  // maybe clear everything ??
  // don't clear until later.. 
  for(int i=0; i < words.size(); i++) { cout << endl << words[i] << endl; }
  cout << "Emitting words alleluliaaah" << endl;
  emit createSession(words);
}

void NewSession::clearText(){
  titleLine->clear();
  descriptionArea->clear();
  keyWordLine->clear();
}


  
  
