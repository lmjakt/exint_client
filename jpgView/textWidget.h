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

#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <qwidget.h>
#include <qtextedit.h>
#include "../dataStructs.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <string>

using namespace std;

class TextWidget : public QWidget {
  Q_OBJECT

    public :
    //TextWidget(QWidget* parent=0, const char* name=0);    // if I want to modify this one so that I can use it for data entry.. it's logical
  TextWidget(ish_annotation data, string fName, bool isReadOnly, QWidget* parent=0, const char* name=0);
  
  void setData(ish_annotation data, string fName, bool isReadOnly);      // set the data of present ones.. 
  QSize sizeHint() const;

  private slots :
    //void dataChanged();
  void updateAnnotation();

  signals :
    void updateData(ish_annotation);    // construct a new ish_annotation and ask for it to be updated..

  private :
    QPushButton* updateButton;          // so I can disable and enable it.. 
  ish_annotation currentAnnotation;     // so it is possible to do things..
  QTextEdit* textArea; 
  QString fieldName;
  QLabel* userLabel;
  QLabel* fieldLabel;
};

#endif

    
