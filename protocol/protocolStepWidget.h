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

#ifndef PROTOCOLSTEPWIDGET_H
#define PROTOCOLSTEPWIDGET_H

#include <qwidget.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qsize.h>
#include "../dataStructs.h"

class ProtocolStepWidget : public QWidget
{     
  Q_OBJECT

    public :
    ProtocolStepWidget(QWidget* parent=0, const char* name=0);
  
  void setStep(ProtocolStep step, int pos); // we'll change this later..  
  void setPosition(int p);              // the position occupied by the step.. 
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool isActive(){
    return(textArea->hasFocus());
  }
  bool isModified(){
    return(textArea->isModified());
  }
  void setAfter(ProtocolStepWidget* wid){
    laterStep = wid;
  }
  string description(){
    return(textArea->text().latin1());
  }
  ProtocolStepWidget* next(){
    return(laterStep);
  }
  ProtocolStep theStep(int userId, string userName);   
  ProtocolStep theStep();   


  private :
    QTextEdit* textArea;   // the description area
  QLabel* stepLabel;       // the number of the step. (later one we can include things like creator and creator date and so forth,.. but not now..
  ProtocolStepWidget* laterStep;
  int minHeight;
  void focusInEvent(QFocusEvent* e);
  ProtocolStep pStep;    
  
  private slots :
    void textChanged();    // check if the size is changed.. if that's the case signal someone to do something about it. Might be ugly.. but maybe try.
  
  signals :
    void sizeHintChanged(ProtocolStepWidget*);
};

#endif

