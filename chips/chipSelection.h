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

#ifndef CHIPSELECTION_H
#define CHIPSELECTION_H

#include <qobject.h>
#include "../dataStructs.h"
#include <map>

using namespace std;

class ChipSelection : public QObject
{
  Q_OBJECT

    public :
    ChipSelection();      // set things later on..
  ~ChipSelection();
  
  void setChipInfo(map<int, chipInfo> info);
  void toggleChip(int n);   // toggle a chip with identifier n..

  signals :
    void toggleButtons(map<int, bool>);  // go through and set the buttons for the things..
  void toggleExperiments(map<int, bool>);  // toggle experiments, this has to use equivalents rather than the chips themselves.. ok.. 
  void newChipInfo(map<int, bool>);      // create the buttons if they are not there.. displayers and others can do whatever.. 
  void setClientChips(vector<int>);
  void equivalentStates(map<int, bool>);

  private :
    map<int, chipInfo> chips;    // maybe that's all I need.
  map<int, bool> states;         // the states for each thingy.. 
};

#endif
