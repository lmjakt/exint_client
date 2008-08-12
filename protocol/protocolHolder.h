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

#ifndef PROTOCOL_HOLDER
#define PROTOCOL_HOLDER

#include "../dataStructs.h"
#include "protocolViewer.h"
#include "protocolClipboard.h"
#include <qscrollview.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qpushbutton.h>

class ProtocolHolder : public QWidget
{
  Q_OBJECT
    public :
    ProtocolHolder(int myId, map<int, userInformation>* uInfo, ProtocolClipboard* clpboard, QWidget* parent=0, const char* name=0);
  ~ProtocolHolder();

  void setProtocol(Protocol* prot);
  void makeNewProtocol(int parentId);          // assign the protocol, with id, and some stuff.. 
  Protocol* theProtocol(){
    return(protocol);
  }

  private :
    QLineEdit* nameLine;
  QTextEdit* descriptionArea;
  QLabel* creatorLabel;
  QLabel* timeLabel;
  QScrollView* scroll;
  ProtocolViewer* viewer;
  Protocol* protocol;    // for descriptions, name, user, create date, and others.. 
  ProtocolClipboard* clipboard;
  int userId;
  string userName;
  int bottomDistance;
  map<int, userInformation>* userData;
  // need some buttons so that I can change them..
  QPushButton* pasteStep;
  QPushButton* pasteEnd;
  QPushButton* pasteTop;

  QPushButton* commitButton;

  private slots :
    void childGrew();
  void contentsMoved(int x, int y);
  void commitToDB();
  void activatePaste();

  protected :
    void resizeEvent(QResizeEvent* e);

  signals :
    void commitProtocolToDB(Protocol*);
};

#endif
