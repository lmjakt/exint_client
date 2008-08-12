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

#ifndef PROTOCOLVIEWER_H
#define PROTOCOLVIEWER_H

#include "protocolStepWidget.h"
#include "protocolClipboard.h"
#include <qscrollview.h>
#include <qlayout.h>
#include <qframe.h>
#include <vector>

using namespace std;

class ProtocolViewer : public QWidget
{
  Q_OBJECT
    public :
    ProtocolViewer(int myId, ProtocolClipboard* clpboard, QWidget* parent=0, const char* name=0);
  ~ProtocolViewer();

  void setProtocol(Protocol* prot);       // sets the protocol,, use local copy.. so we can edit and later emit.. for inclusion
  QSizePolicy sizePolicy() const;
  QSize sizeHint() const;

  public slots :
  void addStep();          // call below with the size of the thingy.. 
  void addStep(int pos);   // add an empty step.. down the bottom.. 
  void addStepAfterCurrent();  // if a step has is current add a new step at this point..
  void prependStep();          // add a step at the beginning..
  void cutStep();              // cut but remember it.. -- question is where to paste it into.. 
  void copyStep();             // similar but..
  void pasteStepAfterCurrent();   // --- hmmmm, bugger, should have written the things so that I don't have to do this.. oh well.
  void pasteBeginning();
  void pasteEnd();

  void commitToDB(string userName);           // make a new Protocol,, and update it.. 
  //// I don't want to confuse with protocol collection,, hence the copy .. 
  private :
    vector<ProtocolStepWidget*> steps;   // the steps.. -- get order from internal.. chain structure. 
  ProtocolStepWidget* firstStep;         // special case.. 
  Protocol* protocol;     // our protocol..

  //ProtocolStepWidget* copiedStep;    // the cut Step.. 

  int userId;
  ProtocolClipboard* clipboard;
  // Layout items required.. 
  //QFrame* frame;       // I don't actually know if I need this.. but.. 
  //QWidget* port;         // the viewport, I hope this doesn't change.. 
  //QScrollView* view;
  QGridLayout* grid;   // somewhere to put all of those thingies.. 

  protected :
    void resizeEvent(QResizeEvent* e);
  
  /// maybe that is enough for now.. -setProtocol just sets up the layout.. 
  private slots :
  void arrangeSteps(int pos);  // arrange the steps after the step point in the order of the thingy.. 
  void childChanged(ProtocolStepWidget*);

  signals :
    //void keepVisible(QPoint);   // hmm 
    void IGrew();
  void copiedAStep();    // when I copy a step.. 
};

#endif

  
