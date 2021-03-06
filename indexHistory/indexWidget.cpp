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

#include "indexWidget.h"
#include "../smallToggle/smallToggle.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qstring.h>

using namespace std;

IndexWidget::IndexWidget(QWidget* parent, const char* name)
  : QWidget(parent, name)
{

  noLabel = new QLabel(this, "noLabel");
  noLabel->setFixedWidth(30);
  noLabel->setIndent(5);
  sizeLabel = new QLabel(this, "sizeLabel");
  sizeLabel->setFixedWidth(50);
  sizeLabel->setIndent(5);
  sizeLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  descriptionLabel = new QLineEdit(this, "descriptionLabel");
  descriptionLabel->setLineWidth(1);
  descriptionLabel->setMargin(2);
  descriptionLabel->setFixedHeight(25);

  QPushButton* loadButton = new QPushButton("load", this, "loadButton");
  loadButton->setFixedHeight(25);
  loadButton->setFixedWidth(40);          // this isn't very flexible, but it allows me to add some labels and stuff.. more easily
  connect(loadButton, SIGNAL(clicked()), this, SLOT(loadIndex()) );
  include = new SmallToggle(1, this, "include");
  //  include->setText("I");
  exclude = new SmallToggle(1, this, "exclude");

  connect(include, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)) );
  connect(exclude, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)) );
  
  QHBoxLayout* box = new QHBoxLayout(this);
  box->addWidget(noLabel);
  box->addWidget(loadButton);
  box->addWidget(sizeLabel);
  box->addWidget(descriptionLabel);
  box->addWidget(include);
  box->addWidget(exclude);
  setFixedHeight(25);
}

void IndexWidget::setIndex(IndexSet is, bool inc, bool exc, int no){
  indexSet = is;
  QString numString;
  numString.setNum(indexSet.size());
  sizeLabel->setText(numString);
  numString.setNum(no);
  noLabel->setText(numString);
  descriptionLabel->setText(indexSet.description());
  if(inc == exc){
    include->setOn(false);
    exclude->setOn(false);        // not good, but there you go.. 
  }else{
    include->setOn(inc);
    exclude->setOn(exc);
  }
}


void IndexWidget::setIndex(IndexSet is, int no){
  indexSet = is;
  QString numString;
  numString.setNum(indexSet.size());
  sizeLabel->setText(numString);
  numString.setNum(no);
  noLabel->setText(numString);
  descriptionLabel->setText(indexSet.description());
  include->setOn(false);
  exclude->setOn(false);        // not good, but there you go.. 
}

void IndexWidget::toggled(bool on){
  emit switchToggled();
  ///const QObject* button = sender();
  // only actually need to do something if we are trying to turn a button on..
  if(!on){
    return;
  }
  if(sender() == (QObject*)include){
    exclude->setOn(false);
    //    cout << "sender is include button " << endl;
  }else{
    include->setOn(false);
    //    cout << "sender is exclude button " << endl;
  }
}

void IndexWidget::loadIndex(){
  emit restoreIndex(indexSet.index(), indexSet.lastRequest(), indexSet.description());
}
