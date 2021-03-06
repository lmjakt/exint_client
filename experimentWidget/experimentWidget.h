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

#ifndef EXPERIMENTWIDGET_H
#define EXPERIMENTWIDGET_H

#include <qwidget.h>
#include <qstring.h>
#include "../exptinfo.h"
#include "../smallToggle/smallToggle.h"

using namespace std;

class ExperimentWidget : public QWidget
{
  Q_OBJECT

    public:
  ExperimentWidget(exptInfo e, QWidget* parent=0, const char* name=0);
  bool active;
  bool markMe;
  exptInfo eInfo;     // its pretty small, so I might as well store a copy of it here. It's not a really big deal.
  SmallToggle* includeButton;
  SmallToggle* markButton;

 signals:
  void includeMe(float, bool);  // the float key,,
  void includeMyIndex(int, bool);    // the integer, giving the dbaseIndex, maybe its more important..
  void markMyIndex(int, bool);       // mark me when plotting things.. 

  public slots:
    void includeButtonToggled(bool);   // send one of the two signals out to the ether...
  void markButtonToggled(bool);        // mark me.. !! 
  void excludeIfNoChip(int chip);    
  void setState(bool state);

 private:

};

#endif
  
