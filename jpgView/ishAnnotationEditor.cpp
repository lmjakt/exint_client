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

#include "ishAnnotationEditor.h"
#include "ishTextEntry.h"
#include "ishFloatEntry.h"
#include "textWidget.h"
#include "../dataStructs.h"
#include "ishAnnotationErrorMessage.h"
#include <qwidget.h>
#include <qtable.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstring.h>
#include <qlayout.h>
#include <iostream>

using namespace std;

IshAnnotationEditor::IshAnnotationEditor(map<int, userInformation>* uInfo, int* id, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  userInfo = uInfo;
  myId = id;
  cout << "myId is " << *myId << endl;
  staticData = new QTable(this, "staticData");
  connect(staticData, SIGNAL(valueChanged(int, int)), this, SLOT(staticChanged(int, int)) );
  staticData->setMinimumHeight(75);

  //  textAnnotation = new QTable(this, "textAnnotation");
  //textAnnotation->setMinimumHeight(100);
  floatAnnotation = new QTable(this, "floatAnnotation");
  floatAnnotation->setMinimumHeight(50);
  classification = new QTable(this, "classification");
  classification->setMinimumHeight(50);

  textWidgets.resize(0);
  vector<QString> emptyFields(0);     // an empty vector.. 
  
  textInserter = new IshTextEntry(emptyFields, this, "textInserter");
  textInserter->hide();
  QPushButton* newTextButton = new QPushButton("New Text Annotation", this, "newTextButton");
  newTextButton->setToggleButton(true);
  connect(newTextButton, SIGNAL(toggled(bool)), textInserter, SLOT(setShown(bool)) );
  //  textInserter->setMinimumHeight(30);
  //textInserter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  connect(textInserter, SIGNAL(pleaseInsert(ishTextInsert)), this, SLOT(textAnnote(ishTextInsert)) );
  floatInserter = new IshFloatEntry(emptyFields, this, "floatInserter");
  floatInserter->hide();
  QPushButton* newFloatButton = new QPushButton("New Numerical Annotation", this, "newFloatButton");
  newFloatButton->setToggleButton(true);
  connect(newFloatButton, SIGNAL(toggled(bool)), floatInserter, SLOT(setShown(bool)) );
  connect(floatInserter, SIGNAL(pleaseInsert(ishFloatInsert)), this, SLOT(floatAnnote(ishFloatInsert)) );
  classificationInserter = new IshFloatEntry(emptyFields, this, "classificationInserter");
  classificationInserter->hide();
  QPushButton* newClassButton = new QPushButton("New Classification", this, "newClassButton");
  newClassButton->setToggleButton(true);
  connect(newClassButton, SIGNAL(toggled(bool)), classificationInserter, SLOT(setShown(bool)) );
  connect(classificationInserter, SIGNAL(pleaseInsert(ishFloatInsert)), this, SLOT(classAnnote(ishFloatInsert)) );

  /// arghh..... --- how to deal with these bastards.. well, just keep sending the messages to the database.. should be OK.. --

  // and set up the tables in a reasonable manner...
  // the static data is not editable, and not really owned by anyone.. --- hence, needs just two columns..
  // the other tables require three columns each.. -- 
  // field, value, user   -- use user string.. -- and then something.. 
  // columsn, 0 and 2 should be not editable and the value editable only when the user is the current user..
  // -- need to have signals and stuff to update these things with later on.. argghhhhgh.. too complicated stuff, this..
  
  // first remove the row numbers for all.. 
  staticData->setNumCols(2);
  staticData->setNumRows(3);
  staticData->verticalHeader()->hide();
  staticData->setLeftMargin(0);
  staticData->horizontalHeader()->setLabel(0, "Field");
  staticData->horizontalHeader()->setLabel(1, "Annotation");
  staticData->setColumnStretchable(1, true);
  staticData->setColumnReadOnly(0, true);
  staticData->setColumnReadOnly(1, true); 
  //staticData->setRowReadOnly(0, false);
  //staticData->setColumnReadOnly(1, false);
  
    // superuser is all powerful. 
  cout << "set things for static data " << endl;

  //textAnnotation->setNumCols(3);
  //textAnnotation->verticalHeader()->hide();
  // textAnnotation->setLeftMargin(0);
//   textAnnotation->horizontalHeader()->setLabel(0, "User");
//   textAnnotation->horizontalHeader()->setLabel(1, "Field");
//   textAnnotation->horizontalHeader()->setLabel(2, "Annotation");
//   textAnnotation->setColumnStretchable(2, true);
//   textAnnotation->setColumnReadOnly(0, true);
//   textAnnotation->setColumnReadOnly(1, true);
//   cout << "set thing for text thing" << endl;

  floatAnnotation->setNumCols(3);
  floatAnnotation->verticalHeader()->hide();
  floatAnnotation->setLeftMargin(0);
  floatAnnotation->horizontalHeader()->setLabel(0, "User");
  floatAnnotation->horizontalHeader()->setLabel(1, "Field");
  floatAnnotation->horizontalHeader()->setLabel(2, "Value");
  floatAnnotation->setColumnStretchable(2, true);     // so it goes to the end of the table..
  floatAnnotation->setColumnReadOnly(0, true);
  floatAnnotation->setColumnReadOnly(1, true);      // but we really need a validator for the float value.. -- otherwise we would need to put somting else there
  cout << "set things forfloat annotation " << endl; 

  classification->setNumCols(3);
  classification->verticalHeader()->hide();
  classification->setLeftMargin(0);
  classification->horizontalHeader()->setLabel(0, "User");
  classification->horizontalHeader()->setLabel(1, "Class");
  classification->horizontalHeader()->setLabel(2, "Confidence");
  classification->setColumnStretchable(2, true);
  classification->setColumnReadOnly(0, true);
  classification->setColumnReadOnly(1, true);   // again we should have a validator for column 2.. hmm. 
                                                // I could perhaps stick something like a QSpin box in there, but difficult to know how to set it up.
                                                // but probably I have to subclass something to have a better control over this. bummer..
  // ok.. 
  // now just stick the things into a suitable order ..
  cout << "set things for classification " << endl;
  //QHBoxLayout* hbox = new QHBoxLayout(this);
  //QVBoxLayout* rightbox = new QVBoxLayout();
  QVBoxLayout* leftbox = new QVBoxLayout(this);
  //hbox->addLayout(leftbox);
  //hbox->addLayout(rightbox);
  textBox = new QVBoxLayout();
  leftbox->addWidget(staticData);
  QLabel* textLabel = new QLabel("Text Annotation", this, "textLabel");
  QLabel* valueLabel = new QLabel("Numerical Annotation", this, "valueLabel");
  QLabel* classLabel = new QLabel("Classification", this, "classLabel");
  textLabel->setAlignment(Qt::AlignCenter);
  valueLabel->setAlignment(Qt::AlignCenter);
  classLabel->setAlignment(Qt::AlignCenter);
  textLabel->setMinimumHeight(25);
  valueLabel->setMinimumHeight(25);
  classLabel->setMinimumHeight(25);
  //  vbox->addWidget(textAnnotation);
  leftbox->addWidget(textLabel);
  leftbox->addLayout(textBox);      // put annotation here if anything going on.. !
  leftbox->addWidget(classLabel);
  leftbox->addWidget(classification);
  leftbox->addWidget(valueLabel);
  leftbox->addWidget(floatAnnotation);
  leftbox->addWidget(textInserter);
  leftbox->addWidget(classificationInserter);
  leftbox->addWidget(floatInserter);
  QHBoxLayout* buttonRow = new QHBoxLayout();
  leftbox->addLayout(buttonRow);
  buttonRow->addWidget(newTextButton);
  buttonRow->addWidget(newFloatButton);
  buttonRow->addWidget(newClassButton);
  // and that really is everything for now..
}

void IshAnnotationEditor::receiveTextFields(vector<QString> fields){
  textInserter->setFields(fields);
}

void IshAnnotationEditor::receiveFloatFields(vector<QString> fields){
  floatInserter->setFields(fields);
}

void IshAnnotationEditor::receiveClasses(vector<QString> classes){
  classificationInserter->setFields(classes);
}

void IshAnnotationEditor::setData(ishProbeData data){
  // should probably clear all the cells before setting the data, but let's just rung this
  // through and see what it looks like... wont know until it is finished anyway.. 
  // first check if we have some fields..
  if(!textInserter->haveFields() || !floatInserter->haveFields() || !classificationInserter->haveFields()){
    // just get all of them, rather than checking at other times..
    emit getTextFields();
    emit getFloatFields();
    emit getClasses();
  }
  // and hope everything propagates nicely through the systemmm...   hmm. 
  cout << "ISH ANNOTATION EDITOR MY ID IS " << *myId << endl;
  cout << "and column1 is readonly ? : " << staticData->isColumnReadOnly(1) << endl;
  cout << "and row 0 is readonly ? : " << staticData->isRowReadOnly(0) << endl; 
  currentData = data;
  // set the data for each thingy..
  if(*myId == 1){
    staticData->setColumnReadOnly(1, false);
    staticData->setRowReadOnly(0, false);
    staticData->setRowReadOnly(1, true);
    staticData->setRowReadOnly(2, true);
  }

  staticData->setText(0, 0, "name");
  staticData->setText(0, 1, data.probeName.c_str());
  staticData->setText(1, 0, "vector");
  staticData->setText(1, 1, data.vectorName.c_str());
  staticData->setText(2, 0, "Antisense prom");
  staticData->setText(2, 1, data.antisensePromoter.c_str());
  
  /// which would seem to be the only reasonable things to put in, the rest are numerics,, - but I'm suprised I haven't put int eh
  /// string identifier for the probe... --- ok, will do this later.. not to difficult to fix.. 
  // now just go through the different things, and see how things go..
  multimap<string, ish_annotation>::iterator it;
  //  int row = 0;
//   textAnnotation->setNumRows(data.textAnnotation.size());
//   for(it = data.textAnnotation.begin(); it != data.textAnnotation.end(); it++){
//     textAnnotation->setText(row, 0, (*it).second.userName.c_str());
//     textAnnotation->setText(row, 1, (*it).first.c_str());
//     textAnnotation->setText(row, 2, (*it).second.annotation.c_str());
//     textAnnotation->item(row, 2)->setWordWrap(true);
//     textAnnotation->setRowStretchable(row, true);
//     textAnnotation->adjustRow(row);
//     row++;
//   }
  int textCounter = 1;
  cout << "textWidgets size is now.. " << textWidgets.size() << endl;
  for(it = data.textAnnotation.begin(); it != data.textAnnotation.end(); it++){
    if(textCounter > textWidgets.size()){
      cout << "making new textWidget " << endl;
      textWidgets.push_back(new TextWidget((*it).second, (*it).first, ((*it).second.userId != *myId), this));
      connect(textWidgets[textCounter-1], SIGNAL(updateData(ish_annotation)), this, SIGNAL(pleaseUpdateText(ish_annotation)) );
      cout << "and adding it ot the textBox " << endl;
      textBox->addWidget(textWidgets[textCounter-1]);
      cout << "and calling show on it " << endl;
      textWidgets[textCounter-1]->show();
      cout << "done !@! " << endl;
    }else{
      textWidgets[textCounter-1]->setData((*it).second, (*it).first, ((*it).second.userId != *myId));
      textWidgets[textCounter-1]->show();
    }
    cout << "myId is " << *myId << "  and the id of the comment is : " << (*it).second.userId << endl;
    textCounter++;
  }
  // and hide any we don't use..
  for(int i = data.textAnnotation.size(); i < textWidgets.size(); i++){
    textWidgets[i]->hide();
  }

  int row = 0;
  QString value;
  floatAnnotation->setNumRows(data.numberAnnotation.size());
  for(it = data.numberAnnotation.begin(); it != data.numberAnnotation.end(); it++){
    value.setNum((*it).second.value);
    floatAnnotation->setText(row, 0, (*it).second.userName.c_str());
    floatAnnotation->setText(row, 1, (*it).first.c_str());
    floatAnnotation->setText(row, 2, value);
    row++;
  }
  cout << "Float annotation :the size hint suggests : "  << floatAnnotation->sizeHint().height() << endl;
  // but if nothing there, then hide it..
//   if(data.numberAnnotation.size()){
//     floatAnnotation->show();
//   }else{
//     floatAnnotation->hide();
//   }
  row = 0;
  classification->setNumRows(data.classification.size());
  for(it = data.classification.begin(); it != data.classification.end(); it++){
    value.setNum((*it).second.value);
    cout << "setting classification confidence to : " << value << "  derived from a value of : " << (*it).second.value << endl;
    classification->setText(row, 0, (*it).second.userName.c_str());
    classification->setText(row, 1, (*it).first.c_str());
    classification->setText(row, 2, value);
    row++;
  }
  cout << "Classification :the size hint suggests : "  << classification->sizeHint().height() << endl << endl;
//   if(data.classification.size()){
//     classification->show();
//   }else{
//     classification->hide();
//   }
  // and that should be it.. -- except that we need to work out how to deal with changes,
  // and to set which rows are editable and which rows are not depending on identity.. but
  // apart from that, I think we are pretty much ok..
}

// void IshAnnotationEditor::textUpdate(ishTextInsert data){
//   data.ishProbeId = currentData.probeId;
//   data.fieldNameNew = false;
//   emit pleaseUpdateText(data);
// }

void IshAnnotationEditor::textAnnote(ishTextInsert data){
  data.ishProbeId = currentData.probeId;     // hmm, I think that we could crash here.. hmm, it's probably ok..
  emit pleaseInsertText(data);
  // ugly, but let's check the fields as well to make sure things are updated.. 
  if(data.fieldNameNew){
    emit getTextFields();
  }
}

void IshAnnotationEditor::floatAnnote(ishFloatInsert data){
  data.ishProbeId = currentData.probeId;
  emit pleaseInsertFloat(data);
  if(data.fieldNameNew){
    emit getFloatFields();
  }
}

void IshAnnotationEditor::classAnnote(ishFloatInsert data){
  data.ishProbeId = currentData.probeId;
  emit pleaseInsertClassification(data);
  if(data.fieldNameNew){
    emit getClasses();
  }
  
}

void IshAnnotationEditor::ishTextHandled(ishTextInsert data){
  textInserter->insertNotification(data);
  // otherwise, we should send it to the appropriate inserter.. hmm
}

void IshAnnotationEditor::ishFloatHandled(ishFloatInsert data){
  floatInserter->insertNotification(data, false);
  // otherwise, we should send it to the appropriate inserter.. hmm
}

void IshAnnotationEditor::ishClassHandled(ishFloatInsert data){
  // otherwise, we should send it to the appropriate..
  classificationInserter->insertNotification(data, true);  // true is class.. 
}

void IshAnnotationEditor::staticChanged(int r, int c){
  cout << "value in " << r << ", " << c << " changed to " << staticData->text(r, c) << endl;
  if(r == 0 && c == 1){       // that is the name of the gene.. -- which can only be changed by root
    emit setName(currentData.probeId, staticData->text(r, c));   // and see what comes of it.. !! 
  }
}
