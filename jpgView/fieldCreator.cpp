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

#include "fieldCreator.h"
#include <iostream>
#include <qlayout.h>
#include <qvalidator.h>

using namespace std;

FieldCreator::FieldCreator(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  tissueLine = new QLineEdit(this, "tissueLine");
  fieldLabel = new QLabel("Tissue Name", this, "tissueLabel");
  fieldLabel->setIndent(5);
  
  ageLine = new QLineEdit(this, "ageLine");
  ageLine->setValidator(new QDoubleValidator(this, "doubleValidator"));
  ageLabel = new QLabel("Age (days)", this, "ageLabel");
  
  QPushButton* cancelButton = new QPushButton("Cancel", this, "cancelButton");
  createButton = new QPushButton("Create Tissue", this, "createButton");
  //  QPushButton* closeButton = new QPushButton("Close", this, "closeButton");

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()) );
  connect(createButton, SIGNAL(clicked()), this, SLOT(create()) );
  //connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()) );

  // and let's make an arrangement of sorts...
  QGridLayout* grid = new QGridLayout(this, 2, 4, 10, 3);
  grid->setColStretch(1, 1);   // the rest are 0.. 
  
  grid->addWidget(fieldLabel, 0, 0);
  grid->addWidget(tissueLine, 0, 1);
  grid->addWidget(ageLabel, 0, 2);
  grid->addWidget(ageLine, 0, 3);
  //grid->addWidget(closeButton, 1, 0);
  grid->addWidget(cancelButton, 1, 0);
  grid->addWidget(createButton, 1, 1);
  resize(350, 55);
}

void FieldCreator::cancel(){
  tissueLine->clear();
  ageLine->clear();
  hide();
}

void FieldCreator::create(){
  if(ageLine->isVisible()){
    if(tissueLine->text().length() && ageLine->text().length()){
      cout << "Tissue " << tissueLine->text() << "\tage: " << ageLine->text() << endl;
      float f = ageLine->text().toFloat();   // should always be OK as we have the validator..
      emit makeTissue(tissueLine->text(), f);
    }
  }else{
    if(tissueLine->text().length()){
      emit makeField(tissueLine->text());
    }
  }
  tissueLine->clear();
  ageLine->clear();
  hide();
  // but leave it open in case one wants to make more comments... OK.. 
}

void FieldCreator::createField(){
  cout << "create Field called " << endl;
  ageLabel->hide();
  ageLine->hide();
  createButton->setText("Create Field");
  fieldLabel->setText("Field Name");
  show();
}

void FieldCreator::createTissue(){
  cout << "create Tissue called " << endl;
  ageLabel->show();
  ageLine->show();
  createButton->setText("Create Tissue");
  fieldLabel->setText("Tissue Name");
  show();
}

