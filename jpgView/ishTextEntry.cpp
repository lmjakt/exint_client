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

#include "ishTextEntry.h"
#include "../dataStructs.h"
#include "ishAnnotationErrorMessage.h"
#include <qwidget.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qsize.h>
#include <iostream>

using namespace std;

IshTextEntry::IshTextEntry(vector<QString> fieldNames, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  cout << "creating an IshTextEntry object, fieldNames size is " << fieldNames.size() << endl;

  //  if(!fieldNames.size()){
  //  fieldNames.push_back("number one");
  //  fieldNames.push_back("number two");
  //}
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  //setMinimumHeight(90);
  fieldsSet = false;
  textField = new QTextEdit(this, "textField");
  fieldChooser = new QComboBox(true, this, "fieldChooser");
  fieldChooser->setDuplicatesEnabled(false);
  fieldChooser->insertItem("----------------");    // hence 0 is not valid.. if 0, do nothing.. but perhaps complain loudly.. 
  for(uint i=0; i < fieldNames.size(); i++){
    int in = fieldChooser->count();
    fieldChooser->insertItem(fieldNames[i], in);
    // and set up the maps accordingly.. 
    fields.insert(make_pair(in, fieldNames[i]));
    fieldLookup.insert(make_pair(fieldNames[i], in));
  }    
  connect(fieldChooser, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)) );
  connect(fieldChooser, SIGNAL(activated(const QString&)), this, SLOT(activated(const QString&)) );

  commitButton = new QPushButton("Commit to DB", this, "commitButton");
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commitData()) );

  insertTimer = new QTimer(this, "insertTimer");
  connect(insertTimer, SIGNAL(timeout()), this, SLOT(timedOut()) );

  QVBoxLayout* vbox = new QVBoxLayout(this);
  vbox->addWidget(textField);
  QHBoxLayout* hbox = new QHBoxLayout();
  vbox->addLayout(hbox);
  hbox->addWidget(commitButton);  
  hbox->addWidget(fieldChooser);
  hbox->setStretchFactor(commitButton, 0);
  hbox->setStretchFactor(fieldChooser, 1);
}

QSize IshTextEntry::sizeHint() const
{
  return(QSize(width(), textField->sizeHint().height() + fieldChooser->height()));
}

void IshTextEntry::setFields(vector<QString> fieldNames){
  cout << "Setting fields in the text inserter ,, how's this " << endl;
  for(uint i=0; i < fieldNames.size(); i++){
    int in = fieldChooser->count();
    fieldChooser->insertItem(fieldNames[i], in);
    // and set up the maps accordingly.. 
    fields.insert(make_pair(in, fieldNames[i]));
    fieldLookup.insert(make_pair(fieldNames[i], in));
  }
  fieldsSet = true;
}

void IshTextEntry::newFieldNotification(QString fieldName){
  // insert the field name into the menu, with an appropriate index, and
  // then update the maps.. -- if we already have it do other stuff... ok..
}

void IshTextEntry::textChanged(const QString& text){
  cout << "text changed to : " << text << endl;
}

void IshTextEntry::activated(const QString& text){
  cout << "activated with string : " << text << endl;
  cout << "the number of the current item is : " << fieldChooser->currentItem() << endl; 
  if(!fields.count(fieldChooser->currentItem()) && !newFields.count(fieldChooser->currentItem()) ){
    // then we've made a new one..
    cout << "made a new thingy.. will insert into the thingy.. " << endl;
    // actually we should check that this isn't a duplicate,, because duplicates are likely to just cause trouble..
    // do later though..
    if(!fieldLookup.count(text) && !newFieldLookup.count(text)){    // this isn't strictly speaking necessary as I've disabled duplicates above
      newFields.insert(make_pair(fieldChooser->currentItem(), text));
      newFieldLookup.insert(make_pair(text, fieldChooser->currentItem()));   /// hmm, really should be unique.. 
    }else{
      // we really should remove the current one.. --
      fieldChooser->removeItem(fieldChooser->currentItem());
    }
  }
}

void IshTextEntry::commitData(){
  // read the fields and work out how to do stuff..
  if(!fieldChooser->currentItem()){
    cerr << "no field choosen, try again" << endl;
    return;
  }
  QString text = textField->text();
  if(!text.length()){
    cerr << "no annotation,, try something else" << endl;
    return;
  }
  bool newField = false;
  if(newFields.count(fieldChooser->currentItem())){
    newField = true;
  }
  ishTextInsert* insert = new ishTextInsert(fieldChooser->currentText(), text, newField);
  // and emit the signal with the thing dereferenced..
  pendingInserts.insert(insert);     // it's a pointer so I don't need to know if it's inserted.. 
  cout << "pendingInserts is now : " << pendingInserts.size() << "  big" << endl;
  insertTimer->start(2000, true);    // 2 seconds.. 
  //  commitButton->setDisabled(true);
  emit pleaseInsert(*insert);
  // after this returns, clear the field..
  textField->clear();
  fieldChooser->setCurrentItem(0);
}

void IshTextEntry::insertNotification(ishTextInsert data){
  // check if we are waiting for this one,  /// SERIOUS ERROR ON 64 BIT SYSTEM.. //// 
  set<ishTextInsert*>::iterator it = pendingInserts.find((ishTextInsert*)data.id);
  if(it == pendingInserts.end()){
    cout << "no such pending insert here, go away.. " << endl;
    return;
  }
  if(data.errorNo){
    IshAnnotationErrorMessage* message = new IshAnnotationErrorMessage(data, this, "error");
    if(message->exec()){
      // do something
      cout << "the foolish user wants to try again" << endl;
    }else{
      ishTextInsert* temp = (*it);
      pendingInserts.erase(it);
      delete temp;
    }
    return;
  }
  ishTextInsert* temp = (*it);
  pendingInserts.erase(it);
  delete temp;
  insertTimer->stop();    // only if we are waiting for this one... 
  //commitButton->setDisabled(false);
}

void IshTextEntry::timedOut(){
  cerr << "Insert timed out,, what to do now.. I just don't know anymore.." << endl;
  //commitButton->setDisabled(false);
  // what to do now,, -we could try to send a callback function of some sort, or we could just try again.
  // trying again though is not really a good idea -- we could ask the user for some input here... -- it is probably
  // better than trying to do other stuff..
}

  
