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

#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include "../dataStructs.h"
#include "indexWidget.h"
#include <vector>
#include <deque>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>

class HistoryWidget : public QWidget 
{
  Q_OBJECT

    public :
    HistoryWidget(deque<IndexSet>* islist, QWidget* parent=0, const char* name=0);
  QSize sizeHint() const;

  public slots :
    void newIndex();     // then just check the thingy..
  
  private :
    deque<IndexSet>* listPointer;
  vector<IndexWidget*> indexWidgets;
  QVBoxLayout* box;        // so I can put new widgets into the layout..
  QLabel* selectLabel;
  QLineEdit* boolLine;     // for inputting boolean queries.. 
  QLabel* boolSelect;
  vector<int> selected;

  private slots :
    void checkSelection();
  void loadCombination();       // loads a combination.. if something there.. 
  void evalBoolean();
  void loadBoolean();
  void loadBoolean(QString text, bool loadIt);

  signals :
    void loadProbes(vector<int>, QString);
  void restoreIndex(vector<int>, int, QString);
  // I probably need to set a size hint and stuff somewhere, but let's wait and see.
};

#endif
