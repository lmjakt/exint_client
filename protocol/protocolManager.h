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

#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

#include "../dataStructs.h"
#include "protocolHolder.h"
#include "protocolClipboard.h"
#include <qwidget.h>
#include <vector>
#include <set>
#include <map>
#include <qlistview.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qlayout.h> 
#include <qpopupmenu.h>

using namespace std;

class ProtocolManager : public QWidget
{
  Q_OBJECT
    public :
    ProtocolManager(const int* myId, map<int, userInformation>* uInfo, QWidget* parent=0, const char* name=0);
  
  public slots :
    void setProtocolCollection(int reqid, ProtocolCollection* coll);  // replaces the current collection if the request id's match..
  void addProtocol(Protocol* prot);            // creates a viewer and adds a protocol to it.
  
  protected :             // now we can inherit and do bad stuff to all of these things.. 
    QListView* listView;
  ProtocolCollection* collection;
  int requestId;                                // the last requestId,, the only one that matters.. !!
  map<QListViewItem*, Protocol*> protocolMap;   // the protocols in the collection..
  set<ProtocolHolder*> protocols;            // includes the viewer and takes care of the protocol.. -we don't touch it from here on.
  map<int, bool> openItem;                    // try to remember whether or not items are open or closed.. -set when we request a new collection.. and 
                                              // queried after we create the widgets.. 
  const int* userId;
  map<int, userInformation>* userData;
  Protocol* selectedProtocol;
  ProtocolClipboard clipboard;                  // pass a ref to the daughters so they know about this.. one 

  QTextEdit* descriptionView;
  QLabel* creatorLabel;
  QLabel* timeLabel;
  QPopupMenu* loadMenu;
  QPopupMenu* updateMenu;

  // we will also need a whole load of things for setting new protocols and things like that, but it can wait..
  private slots :
    void requestCollection();
  void requestProtocol();
  void newProtocol();
  void commitProtocol(Protocol* protocol);
  void selectionChanged(QListViewItem* item);
  void loadProtocol();
  void popMenu(QListViewItem* item, const QPoint& point, int i);

  signals :
    void getCollection(int);    
  void getProtocol(int);
  void commitProtocolToDB(Protocol*);
  
  protected :
    QVBoxLayout* rbox;

};

#endif

  
