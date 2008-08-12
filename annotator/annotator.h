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

#ifndef ANNOTATOR_H
#define ANNOTATOR_H

#include "newSession.h"
#include "sessionManager/sessionManager.h"
#include <qwidget.h>
#include <qpalette.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qsize.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <qstring.h>

using namespace std;

class Annotator : public QWidget
{
  Q_OBJECT

    public :
    Annotator(QWidget* parent=0, const char* name=0);
  ~Annotator();
  QSize sizeHint();
  QTextEdit* commentArea;   // so I can set the focus to it with a shortcut.. 


  public slots :
    void sessionCreated(int sessionId);     // from client.. from connectionobject.. 
  void clearSessions();
  void addSession(SessionWidget* sw);

  private slots :
    void commit();        // commit the current data.. 
  void switchAssociation();   // association to current gene, current selection, or current session.. 
  void switchSessionAssociation(); 
  void paletteChange(const QPalette& oldPalette);
  void makeNewSession(vector<string> words);
  void copyGenesToCurrentSession(vector<int> cGenes);
  void setCurrentSession(int sId, QString ti, QString des, set<QString> kw);

  private :
    QGridLayout* grid;
    map<QPushButton*, QString> switchButtons;
  map<QPushButton*, int> associationStates;
  QPushButton* onButton;
  QPushButton* currentSession;   // need this as it's separate.. 
  QLabel* sessionTitle;
  //QLabel* keyWords;
  QLabel* sessionAssociation;       // whether it's associated with a session or comment.. 
  QLabel* geneAssociation;
  NewSession* sessionCreator;
  SessionManager* sessionManager;

  // and some variables..
  int currentSessionId;
  string currentSessionTitle;
  string currentSessionDescription;
  vector<string> currentKeywords;

  // and if it changes..
  string nextSessionTitle;
  string nextSessionDescription;
  vector<string> nextKeywords;

  signals :
    void createSession(vector<string>);
  void newAnnotation(int, int, QString);      // association selector, session index, annotation. 
  void updateSessions();                      // get the client to ask the server for some new sessions.. (let the server do the thing).
  void newIndex(vector<int>, QString);                 // set the index to the current !!. 
  void updateSessionDescription(int, vector<QString>);
  void copyToCurrentSession(int, vector<int>, QString);       // copy indices in vector<int> to current session.. ,, hmm, no way to check for duplicates.. maybe.. 
                                                              // the QString is a little cheeky, but it allows me to plug straigth into the client newAnnotation function.. 
};

#endif
