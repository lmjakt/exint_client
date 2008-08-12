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

#include "textWidget.h"
#include "../dataStructs.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qstring.h>
#include <iostream>

using namespace std;

TextWidget::TextWidget(ish_annotation data,  string fName, bool isReadOnly, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred); 
  currentAnnotation = data;
  fieldName = fName.c_str();
  textArea = new QTextEdit(this, "textArea");
  textArea->setText(data.annotation);
  textArea->setReadOnly(isReadOnly);
  
  int labelIndent = 5;
  userLabel = new QLabel(data.userName.c_str(), this, "userLabel");
  fieldLabel = new QLabel(fieldName, this, "fieldLabel");   // ugly, I should change the ish_annotation struct.. 
  userLabel->setPaletteBackgroundColor(QColor(255, 255, 255));
  fieldLabel->setPaletteBackgroundColor(QColor(255, 255, 255));
  userLabel->setIndent(labelIndent);
  fieldLabel->setIndent(labelIndent);
  
  updateButton = new QPushButton("Update", this, "updateButton");
  updateButton->setDisabled(true);
  connect(textArea, SIGNAL(modificationChanged(bool)), updateButton, SLOT(setEnabled(bool)) );      
  connect(updateButton, SIGNAL(clicked()), this, SLOT(updateAnnotation()) );

  // ok all we really need now is a reasonably layout..
  QGridLayout* grid = new QGridLayout(this, 2, 3, 0);
  grid->addMultiCellWidget(textArea, 0, 0, 0, 2);
  grid->addWidget(userLabel, 1, 0);
  grid->addWidget(fieldLabel, 1, 1);
  grid->addWidget(updateButton, 1, 2);
}

void TextWidget::setData(ish_annotation data, string fName, bool isReadOnly){
  textArea->setText(data.annotation);
  textArea->setReadOnly(isReadOnly);
  userLabel->setText(data.userName.c_str());
  fieldLabel->setText(fName.c_str());
  updateButton->setDisabled(true);
  // and that's it..
  cout << "the sizehint of the text area is : " << textArea->sizeHint().width() << " x " << textArea->sizeHint().height() << endl;
  
}

void TextWidget::updateAnnotation(){
  // should only be called if a modification has been made.. 
  cout << "this is TextWidget calling updateAnnotation want to change :" << endl 
       << currentAnnotation.annotation << endl << "to : " << endl
       << textArea->text() << endl
       << "And the id of the annotation is : " << currentAnnotation.annotationId << endl;
  currentAnnotation.annotation = textArea->text();
  //  emit updateData(ishTextInsert(fieldName, textArea->text(), false));
  emit updateData(currentAnnotation);
  updateButton->setDisabled(true);
}

QSize TextWidget::sizeHint() const {
  return(QSize(125, 75));
}
