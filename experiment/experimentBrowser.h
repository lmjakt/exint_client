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

#ifndef EXPERIMENTBROWSER_H
#define EXPERIMENTBROWSER_H

#include "../dataStructs.h"
#include "experimentMaker.h"
#include <qwidget.h>
#include <qlistview.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qpopupmenu.h>
#include <vector>

using namespace std;

class ExperimentItem : public QListViewItem
{
  public :
    ExperimentItem(Experiment expt, QListViewItem* parent);
  ExperimentItem(Experiment expt, QListView* parent);

  //int compare(QListViewItem* i, int col, bool ascending) const;    // use this for the comparison..
  // don't think I need this as the basic text comparison should do well enough.. 

  Experiment expt(){
    return(experiment);           // which is safe, but presumably extremely wasteful in terms of cpu power.. 
  }
  int rtti() const { return rtid; }

  private : 
    void initialise();
    Experiment experiment;
  int rtid;
};
  

class ExperimentBrowser : public QWidget 
{
  Q_OBJECT
    public :
    ExperimentBrowser(map<int, userInformation>* uInfo, const int* id, QWidget* parent=0, const char* name=0);       // I don't think I really need to know anymore..
  
  ExperimentMaker* eMaker;      // it's easy for the grandparent to call a method.. 

  public slots :
  void setExperiments(vector<Experiment> expts);                    // use a wasteful, but safe..
  void addExperiment(int reqId, Experiment expt);                        // add an extra experiment.. 

  private :
    set<int> experimentIds;              // so I don't add more than once.. 
    QListView* experimentView;
  QTextEdit* experimentDescription;
  QTextEdit* protocolDescription;
  QLabel* creatorLabel;
  QLabel* exptTimeLabel;
  QLabel* entryTimeLabel;
  QLabel* protocolLabel;    // the name of the protocol.. 
  map<int, userInformation>* userInfo;
  const int* myId;
  
  QPopupMenu* exptMenu;
  QPopupMenu* protMenu;
  // we probably need some signals and other things.. later on we can work out how to create new experiments.. 
  // this will no doubt involve choosing a protocol, and adding some text, and a date using a qtime edit or something
  // like that.. 
  private slots :
    void makeExperiment();
  void popMenu(QListViewItem* item, const QPoint& point, int c);    // c is the column.. 
  void selectionChanged(QListViewItem* item);
  //  void rightButtonClicked(QListViewItem* item, const QPoint& p, int i); 
  void requestProtocol();
  void selectExperiment();

  signals :
    void getCollection(int);
  void commitProtocolToDB(Protocol*);
  void updateExperiments();
  void loadProtocol(int);      // just need to send it off for thingy.. !
  void selectedExperiment(Experiment);
};

#endif

  
  
