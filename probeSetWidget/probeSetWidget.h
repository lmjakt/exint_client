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

#ifndef PROBESETWIDGET_H
#define PROBESETWIDGET_H

#include "../probe_set/probe_set.h"
#include "../smallToggle/smallToggle.h"
#include <qwidget.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <vector>

using namespace std;

class ProbeSetWidget : public QWidget
{
  Q_OBJECT

    public :
    ProbeSetWidget(probe_set pset, probe_data data, QWidget* parent=0, const char* name=0);
  void setProbeSet(probe_set pset, probe_data data, bool checked);
  bool meanPlotOn();      // returns whether or not the button is checked.. 
  ~ProbeSetWidget();
  probe_set mySet;
  probe_data myData;
  QPushButton* save;

  public slots:
    void resetMe();        // toggle buttons off, -remove from mean plot.. 
  
  private slots:
    void plotMean(bool);        // either plot the mean, or remove the mean.. 
  void plotSet();
  void rememberMe();        // add to saved list.. 

 signals:
  void plotMe(probe_set*, probe_data*);             // switch both the plotting function and the data display function 
  void plotMyMean(ProbeSetWidget*, bool);   // plot the values in vector<float> using experimental indices in vector<int>
  void pleaseRememberMe(ProbeSetWidget*);  // pass along to the function that remembers itself.. 

  private :
  SmallToggle* meanButton;
  QLineEdit* affyId;
  QLineEdit* ugGene;

};

#endif
