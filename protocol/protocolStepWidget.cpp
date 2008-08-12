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

#include "protocolStepWidget.h"
#include <qlayout.h>
#include <qwidget.h>
#include <iostream>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <stdlib.h>

using namespace std;

ProtocolStepWidget::ProtocolStepWidget(QWidget* parent, const char* name) :
  QWidget(parent, name)
{

  //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  laterStep = 0;

  textArea = new QTextEdit(this, "textArea");
  //  textArea->setFocusProxy(this);
  //  setFocusPolicy(QWidget::WheelFocus);
  textArea->setVScrollBarMode(QScrollView::AlwaysOff);
  connect(textArea, SIGNAL(textChanged()), this, SLOT(textChanged()) );

  //QPushButton* addBefore = new QPushButton(this);
  //QPushButton* addAfter = new QPushButton(this);
  // see if we have a path for the icons..
  QString beforePixmap = "addUp_25.xpm";
  QString afterPixmap = "addDown_25.xpm";
    
  char* path = getenv("CLIENT_DIR");
  if(path){
    beforePixmap.prepend("/");
    beforePixmap.prepend(path);
    afterPixmap.prepend("/");
    afterPixmap.prepend(path);
  }
  // and set the icons.. 
  //addBefore->setPixmap(QPixmap(beforePixmap));
  //addAfter->setPixmap(QPixmap(afterPixmap));
  //addBefore->setFixedSize(25, 25);
  //addAfter->setFixedSize(25, 25);   // may need to use setFixed size.. 
  

  stepLabel = new QLabel("?", this, "stepLabel");
  stepLabel->setIndent(5);
  stepLabel->setMinimumWidth(25);
  
  QHBoxLayout* hbox = new QHBoxLayout(this);
  QVBoxLayout* leftBox = new QVBoxLayout();
  //QVBoxLayout* rightBox = new QVBoxLayout();
  hbox->addLayout(leftBox);
  
  leftBox->addWidget(stepLabel);
  leftBox->addStretch();
  //leftBox->addWidget(addAfter);
  hbox->addWidget(textArea);
  //hbox->addLayout(rightBox);
  //rightBox->addWidget(addBefore);
  //rightBox->addStretch();
  minHeight = 40;
  setMinimumHeight(minHeight);
} 

void ProtocolStepWidget::setStep(ProtocolStep step, int pos){    // make a local copy.. maybe somewhat safer.. as we may change
  pStep = step;
  QString ps;
  ps.setNum(pos);
  stepLabel->setText(ps);
  textArea->setText(step.description.c_str());
}

ProtocolStep ProtocolStepWidget::theStep(){
  // just return a copy of the step..
  return(pStep);
}

ProtocolStep ProtocolStepWidget::theStep(int userId, string userName){
  // work out what the step should be.. and return a copy of it ..
  if(pStep.id){    // i.e. it was set..
    if(!textArea->isModified()){
      return(pStep);
    }
    pStep.parentId = pStep.id;
    pStep.id = 0;
    pStep.creatorId = userId;
    pStep.creatorName = userName;
    pStep.description = textArea->text().latin1();
    return(pStep);
  }
  pStep.id = 0;
  pStep.parentId = 1;
  pStep.creatorId = userId;
  pStep.creatorName = userName;
  pStep.description = textArea->text().latin1();
  return(pStep);
}

void ProtocolStepWidget::setPosition(int p){
  QString pos;
  pos.setNum(p);
  stepLabel->setText(pos);
}

QSize ProtocolStepWidget::sizeHint() const
{
  int h = textArea->contentsHeight();
  int w = width();  // whatever we are..
  if(h < minHeight){ h = minHeight; }
  //cout << "Size hint : h " << h << endl; 
  return(QSize(w, h));
}

QSizePolicy ProtocolStepWidget::sizePolicy() const
{
  return(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
}

void ProtocolStepWidget::textChanged(){
  //cout << "textArea contents height : " << textArea->contentsHeight() << endl
  //     << " and my own height ..    : " << height() << endl;
  if(textArea->contentsHeight() > height()){
    //cout << "resizing the protocol thingy widget area " << endl;
    //resize(width(), textArea->contentsHeight());
    //updateGeometry();   // maybe this is enough.. 
    emit sizeHintChanged(this);
    return;
    /// Probably I can't do this directly, but I will need to send a signal to the parent that get's to decide what to do.. 
  }
  if(textArea->contentsHeight() < height() && textArea->contentsHeight() >= minHeight){
    //cout << "changing minimum height in the step widget " << endl;
    //setMinimumHeight(textArea->contentsHeight());
    //updateGeometry();
    emit sizeHintChanged(this);
  }
}

void ProtocolStepWidget::focusInEvent(QFocusEvent* e){
  cout << "ProtocolStepWidget received focus.. " << endl;
  //textArea->setFocusProxy(0);
  textArea->setFocus();
}
