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

#ifndef ANNOTATIONWIDGET_H
#define ANNOTATIONWIDGET_H

#include <qwidget.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qsize.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "../../probe_set/probe_set.h"   // for the definition of annotationInformation..
#include "../../client.h"                // to give the link to the userTable.. 

using namespace std;

class AnnotationWidget : public QWidget
{
  Q_OBJECT

    public :
    AnnotationWidget(annotationInformation an, Client* client, bool ce, QWidget* parent=0, const char* name=0);
  AnnotationWidget(sessionInformation sn, Client* client, bool ce, QWidget* parent=0, const char* name=0);
  
  sessionInformation annotation;      // maybe it should be private, but this is less work. 
  virtual QSize sizeHint();
  int type(){
    return(widgetType);
  }

  private :
    int widgetType;     // session or annotation.. 
  bool canEdit;
    
    QPushButton* updateButton;
  QTextEdit* annotationArea;
  QLineEdit* titleLine;
  QLineEdit* keywordLine;  // if a session edit. thingy.. 

  private slots :
    void modificationChanged(bool c);
  void lineChanged(const QString& word);
  void setDisabled(QPushButton* button, bool disabled);
  void cleanText(QString& note);
  void dirtyText(QString& note);
  void updateInformation();     // process the information in the thingy.. 

  signals :
    void updateMe();
  void includeMe(bool);
  void updateAnnotation(int, QString);     // the index, and the description.. (cleaned. !!)
  void updateSessionAnnotation(int, vector<QString>);

};

#endif
