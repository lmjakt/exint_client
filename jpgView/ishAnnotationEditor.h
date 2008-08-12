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

#ifndef ISHANNOTATIONEDITOR_H
#define ISHANNOTATIONEDITOR_H

#include "../dataStructs.h"
#include "textWidget.h"
#include <qwidget.h>
#include <qtable.h>
#include <qlayout.h>
#include "ishTextEntry.h"
#include "ishFloatEntry.h"
#include <map>
#include <vector>

using namespace std;

class IshAnnotationEditor : public QWidget {
  Q_OBJECT

    public :
    IshAnnotationEditor(map<int, userInformation>* uInfo, int* id, QWidget* parent=0, const char* name=0);     // later I should add something so that it knows something about the current id..
  
  void setData(ishProbeData data);        // set the data to the thingy..
  // need some things for handling incoming data to.. later.. 

  public slots :
    void ishTextHandled(ishTextInsert data);
  void ishFloatHandled(ishFloatInsert data);
  void ishClassHandled(ishFloatInsert data);      // these functions take care of success or error notification.. 
  void receiveTextFields(vector<QString> fields);
  void receiveFloatFields(vector<QString> fields);
  void receiveClasses(vector<QString> classes);

  private :
    // some user information..
    map<int, userInformation>* userInfo;
  int* myId;    

    ishProbeData currentData;            // usually good to keep track of things..
  QTable* staticData;                    // for the not editable stuff.
  //QTable* textAnnotation;                // for the text annotation..
  QTable* floatAnnotation;               // if there is any float annotation..
  QTable* classification;                // what kind of expression is this.. and who says so.. not such simple stuff, but there you go.
  
  IshTextEntry* textInserter;             // for inserting new text.. 
  IshFloatEntry* floatInserter;           // numerical annotation...
  IshFloatEntry* classificationInserter;  // classification inserter.. -- need policy for dealing with stuff... 
  
  vector<TextWidget*> textWidgets;        // contains the textWidgets.. 
  QVBoxLayout* textBox;                   // somewhere to put them.. 

  private slots :
    void textAnnote(ishTextInsert data);
  void floatAnnote(ishFloatInsert data);
  void classAnnote(ishFloatInsert data);         // catch the signals from the inserters and specify the probe id.. 
  //void textUpdate(ishTextInsert data);
  // should be able to edit the float and classifications, but that is not that urgent at the moment.. 
  void staticChanged(int r, int c);                  // value in static table changed.. 
  
  signals :
    void pleaseInsertText(ishTextInsert);
  void pleaseUpdateText(ish_annotation);
  void pleaseInsertFloat(ishFloatInsert);
  void pleaseInsertClassification(ishFloatInsert);   // just pass on from the children. 
  void setName(int, QString);                   //  probe id, name ,, set the name for this bastard.. hoo yeahh.. 
  void getTextFields();
  void getFloatFields();
  void getClasses();      // please get the classes if we don't have them already.. 

  // could do with some rollbacks as well, but leaving those for now..
};

#endif

    
