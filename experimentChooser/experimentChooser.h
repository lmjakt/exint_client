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

#ifndef EXPERIMENTCHOOSER_H
#define EXPERIMENTCHOOSER_H

#include "../exptinfo.h"
#include "../experimentWidget/experimentWidget.h"
#include "orderChooser/orderWindow.h"
#include "sampleMemoryWidget.h"
#include <qwidget.h>
#include <qlayout.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <vector>
#include <map>

using namespace std;

class ExperimentChooser : public QWidget
{
  Q_OBJECT

    public :
    ExperimentChooser(QWidget* parent=0, const char* name=0);

  public slots :
    void updateExperiments(multimap<float, exptInfo> info);
  void updateExperiments(map<float, exptInfo> info);   // a wrapper that turns the map into a multimap.. 
  void selectionChanging(int, bool);
  void markSelectionChanging(int, bool);

  private slots :
    void updateExperimentOrder(multimap<float, exptInfo> em);
  void setActivityStates(map<int, bool> states);
  void excludeIfNoChip(int chip);   
  void setAllActive();
  void rememberSelection();
  void readFromFile();
  void setFromMemory(std::set<int> active, std::set<int> marks);

  signals :
    void newExptSelection(vector<int>);
  void newMarkSelection(vector<int>);

 private:
  vector<ExperimentWidget*> eWidgets;     // so I can manually delete at update things
  QFrame* frame;
  QVBoxLayout* grid;
  QHBoxLayout* hbox;
  OrderWindow* orderChooser;
  SampleMemoryWidget* sampleMemory;
  QLineEdit* sampleSetLabel;
  bool updateOrderChooser;
  set<int> chips;
  vector<QPushButton*> excludeButtons;   // this is not going to look pretty, but,,, 
  QButtonGroup* chipGroup;           // ahh, that's a bit ugly.. 
};

#endif                    // later add on signals and stuff so I can create things.. 

