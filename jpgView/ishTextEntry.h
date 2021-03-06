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

#ifndef ISHTEXTENTRY_H
#define ISHTEXTENTRY_H

#include "../dataStructs.h"       // for the insert
#include <qwidget.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qtimer.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <map>
#include <vector>
#include <set>

using namespace std;        // apparently a really bad thing to do which I should stop doing.. 

class IshTextEntry : public QWidget
{
  Q_OBJECT
    public :
    IshTextEntry(vector<QString> fieldNames, QWidget* parent=0, const char* name=0);
  void insertNotification(ishTextInsert data);   // returns the data inserted.. -- or something like that.. 
  void setFields(vector<QString> fieldNames);
  void newFieldNotification(QString fieldName);  // if new field is input into database we should be told about it.. so we don't crate duplicates. 
  bool haveFields(){
    return(fieldsSet);
  }
  QSize sizeHint() const;

  private :
    bool fieldsSet;
  QTextEdit* textField;
  QComboBox* fieldChooser;
  QTimer* insertTimer;              // start on sending insert statement.. stop when notified of success, -on timeout report failure to user
                                    // disable insert button as well..
  QPushButton* commitButton;        // commit to database,, - start timer, disable itself until timed out or notified of success.. 
  set<ishTextInsert*> pendingInserts; // insert created and emitted, but no notification of insertion received.. 

  map<int, QString> fields;                    // fields received from database as having been ok..
  map<int, QString> newFields;                 // new fields input by user, but not input into database.. 
  map<QString, int> fieldLookup;
  map<QString, int> newFieldLookup;            // if the server sends a string as a new field,, - I need to find if I have it .. in my current set
                                               // or in the pending set... -if it is in the pending set, then just move them around..
                                               // if I don't have it then insert into list and into the fields, and so on.. -- 
                                               // not very nice as too many things to look for.. 

  private slots :
    void textChanged(const QString& text);
  void activated(const QString& text);   // just to check for now
  void commitData();                     // connect to the button.. 
  void timedOut();                       // we've been waiting for the reply for some time and are not sure what to do.. 

  signals :
    void pleaseInsert(ishTextInsert);
  void pleaseRollback(ishTextInsert);    // do I really want to do this ?? 
};

#endif

