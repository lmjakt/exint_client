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

#ifndef ISHFLOATENTRY_H
#define ISHFLOATENTRY_H

#include "../dataStructs.h"
#include <qwidget.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <map>
#include <vector>
#include <set>

using namespace std;

class IshFloatEntry : public QWidget
{
  Q_OBJECT
    public :
    IshFloatEntry(vector<QString> fieldNames, QWidget* parent=0, const char* name=0);
  void insertNotification(ishFloatInsert data, bool isClass=false);
  void setFields(vector<QString> fieldNames);     // for setting initially.. 
  void newFieldNotification(QString fieldName);
  bool haveFields(){
    return fieldsSet;
  }
  
  private :
    bool fieldsSet;
    QLineEdit* inLine;
  QComboBox* fieldChooser;
  QTimer* insertTimer;
  QPushButton* commitButton;

  set<ishFloatInsert*> pendingInserts;
  
  map<int, QString> fields;
  map<int, QString> newFields;
  map<QString, int> fieldLookup;
  map<QString, int> newFieldLookup;

  private slots :
    void activated(const QString& text);
  void commitData();
  void timedOut();

  signals :
    void pleaseInsert(ishFloatInsert);
  void pleaseRollBack(ishTextInsert);     // maybe I never use this.. not sure.. 

};

#endif

