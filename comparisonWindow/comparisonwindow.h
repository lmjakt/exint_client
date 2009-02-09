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

#ifndef COMPARISONWINDOW_H
#define COMPARISONWINDOW_H

#include "compInputWindow.h"
//#include "../smallToggle/smallToggle.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <vector>
#include <map>
//#include <qcanvas.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qpainter.h>
#include "../exptinfo.h"



class ComparisonWindow: public QWidget
{
  Q_OBJECT
    public:
  ComparisonWindow(map<float, exptInfo>* em, QWidget *parent=0, const char *name=0);
  ComparisonWindow(QWidget *parent=0, const char *name=0);
  ~ComparisonWindow();
  map<float, exptInfo>* eMap;       // stuff 

  CompInputWindow* inputWindow;     // replace all the canvas stuff with this.. 

  int xo;
  int yo;
  int w;
  int h;
  int ym;

  vector<QRadioButton*> selector;

  public slots:
  void rawComparison();                // collects the data from the canvas and emits a rawComparison(vecttor<float>, vector<int>) signal.. 
  void meanComparison();
  void setActive(map<int, bool> cmap);  // set the active maps.. 
  void setActive(vector<int> samples, int id);  // set the active samples.. 

 signals:
  //void doDiff(int, int, bool);
  //void do_pairs(int, int);             // plot pairwise the things from the in pairPlotter
  void doRawComparison(vector<float>, vector<int>, bool);  // values and indices both needed for the comparison.. 
  void doMeanComparison(vector<float>, vector<int>, bool, bool);

  private :
    QCheckBox* getDist;
  QSpinBox* ptSize;
};



#endif
