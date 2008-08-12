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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "sessionWidget.h"
#include <qwidget.h>
#include <qlayout.h>
#include <qframe.h>
#include <qsize.h>
#include <qscrollview.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <vector>
#include <map>

using namespace std;

class SessionManager : public QWidget
{
  Q_OBJECT
    
    public :
    SessionManager(QWidget* parent=0, const char* name=0);
  
  public slots :
    void addSessionWidget(SessionWidget* sw);
  void clearSessionWidgets();   // clear old ones, and get some new memory.. 

  QSize sizeHint();

  private slots :
    void displayData();      // user sender() to work out how..
  void includeData(bool include);  // again use sender to work out how..
  void selectGenes();     // take the genes from the things and select
  void updateDescriptions();   // essentially commit changes to the thingy.. 
  void setDisableColor(QPushButton* button, bool disabled);
  void cleanText(QString& note);
  void dirtyText(QString& note);
  void copyIncludedGenesToCurrentSession();
  void loadAndAppend();        // just take the current section, load and append.. 

  private :
    set<SessionWidget*> includedSessions;   // ones that are included..
  set<SessionWidget*> sessions;             // all the sessions. May not need this. but there you go..
  map<int, int> includedGeneIds;                 // so I know if something is included or not...
  int currentlyDisplayedSession;                  // the currently Displayed session... 

  QFrame* frame;      // for the scrollview..  -- reparent to this frame,, which contains the vBox !! .
  QVBoxLayout* vBox;  // for the scrollview, to allow the adding of stuff. 
  QScrollView* scroll;
  int preferredWidth;

  QTextEdit* descriptionField;
  QLineEdit* titleField;
  QLineEdit* keywordField;
  QLineEdit* selectedGeneNo;       // no. of genes selected.. 
  //  QLabel* selectedGeneNo;       // no. of genes selected.. 

  // some buttons that need to dynamically disabled or enabled.. 
  QPushButton* commitChanges;
  QPushButton* copyToSession;
  QPushButton* appendSession;
  QPushButton* loadGenes;

  signals :
    void updateSessions();     // connects all the way to client.. who in fact will be sending the session widgets in this direction.. 
  void newIndex(vector<int>, QString);
  void changeDescriptions(int, vector<QString>);  // int -the session Index. QString, -title, description, keywords.. 
  void copyToCurrentSession(vector<int>);
  void appendToSession(int, QString, QString, set<QString>);          // basically load the session and start editing and adding stuff. !!! .. 
};

#endif

