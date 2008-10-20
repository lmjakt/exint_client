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

#include "ishFloatEntry.h"
#include "../dataStructs.h"
#include "ishAnnotationErrorMessage.h"
#include <qwidget.h>
#include <qcombobox.h>
#include <qvalidator.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <vector>
#include <map>
#include <set>
#include <iostream>

using namespace std;

IshFloatEntry::IshFloatEntry(vector<QString> fieldNames, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  //  if(!fieldNames.size()){
  //  fieldNames.push_back("number one");
  //  fieldNames.push_back("number two");
  //}
  fieldsSet = false;
  inLine = new QLineEdit(this, "inLine");
  QDoubleValidator* validator = new QDoubleValidator(this, "validator");
  inLine->setValidator(validator); 
  fieldChooser = new QComboBox(true, this, "fieldChooser");
  fieldChooser->setDuplicatesEnabled(false);
  fieldChooser->insertItem("----------------", 0);
  for(int i=0; i < fieldNames.size(); i++){
    fieldChooser->insertItem(fieldNames[i], i+1);
    fields.insert(make_pair(i+1, fieldNames[i]));
    fieldLookup.insert(make_pair(fieldNames[i], i+1));
  }
  connect(fieldChooser, SIGNAL(activated(const QString&)), this, SLOT(activated(const QString&)) );
  
  commitButton = new QPushButton("Commit to DB", this, "commitButton");
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commitData()) );
  
  insertTimer = new QTimer(this, "insertTimer");
  connect(insertTimer, SIGNAL(timeout()), this, SLOT(timedOut()) );

  QHBoxLayout* hbox = new QHBoxLayout(this);
  hbox->addWidget(commitButton);
  hbox->addWidget(fieldChooser);
  hbox->addWidget(inLine);
  // set up some stretch factors..
  hbox->setStretchFactor(commitButton, 0);
  hbox->setStretchFactor(fieldChooser, 1);
  hbox->setStretchFactor(inLine, 1);
  
  //resize(300, 200);
}

void IshFloatEntry::setFields(vector<QString> fieldNames){
  // fairly simply stuff.. 
  for(int i=0; i < fieldNames.size(); i++){
    int in = fieldChooser->count();
    fieldChooser->insertItem(fieldNames[i], in);
    // and set up the maps accordingly.. 
    fields.insert(make_pair(in, fieldNames[i]));
    fieldLookup.insert(make_pair(fieldNames[i], in));
  }
  fieldsSet = true;
}


void IshFloatEntry::newFieldNotification(QString fieldName){
  /// do something for god's sake.
}

void IshFloatEntry::activated(const QString& text){
  if(!fields.count(fieldChooser->currentItem()) && !newFields.count(fieldChooser->currentItem())){
    // then we've made new one and need to insert into the new maps..
    newFields.insert(make_pair(fieldChooser->currentItem(), text));
    newFieldLookup.insert(make_pair(text, fieldChooser->currentItem()));   // makes a copy ?? should do.. otherwise trouble.. 
  }
  // no need to check if we already have one like this, as I've disabled duplicates in the thingy..
  
}

void IshFloatEntry::commitData(){
  if(!fieldChooser->currentItem()){
    cerr << "no field chosen" << endl;
    return;
  }
  // don't worry about the details of the below until we've checked how the validator works..
  bool ok;
  float value = inLine->text().toFloat(&ok);
  if(!ok){
    cerr << "no value specified... " << endl;
    return;
  }
  bool newField = false;
  if(newFields.count(fieldChooser->currentItem())){
    newField = true;
  }
  ishFloatInsert* insert = new ishFloatInsert(fieldChooser->currentText(), value, newField);
  pendingInserts.insert(insert);
  insertTimer->start(2000, true);
  //commitButton->setDisabled(true);
  emit pleaseInsert(*insert);
  inLine->clear();
  fieldChooser->setCurrentItem(0);
}


void IshFloatEntry::insertNotification(ishFloatInsert data, bool isClass){
  // do something.. -- I want to know if it is a classification or not.. 
  set<ishFloatInsert*>::iterator it = pendingInserts.find((ishFloatInsert*)data.id);
  if(it == pendingInserts.end()){
    cout << "no such pending floatinsert here, bybyb " << endl;
    return;
  }
  if(data.errorNo){
    IshAnnotationErrorMessage* message = new IshAnnotationErrorMessage(data, isClass, this, "error");
    if(message->exec()){
      // do something
      cout << "the foolish user wants to try again" << endl;
    }else{
      ishFloatInsert* temp = (*it);
      pendingInserts.erase(it);
      delete temp;
    }
    return;
  }
  ishFloatInsert* temp = (*it);
  pendingInserts.erase(it);
  delete temp;
  insertTimer->stop();
  //commitButton->setDisabled(false);
}

void IshFloatEntry::timedOut(){
  cerr << "insert timer timed out, what's going on" << endl;
  //commitButton->setDisabled(false);
}
