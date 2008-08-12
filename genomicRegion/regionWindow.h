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

#ifndef REGIONWINDOW_H
#define REGIONWINDOW_H

#include "../client.h"
//#include "regionWidget.h"            // try to replace with local region view.. 
#include <qwidget.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qvalidator.h>
#include <string>
#include <set>
#include <vector>

using namespace std;

class RegionWindow : public QWidget
{
  Q_OBJECT
    public :
    RegionWindow(set<chromRegion*>* cr, QWidget* parent=0, const char* name=0);
  
  private :
    set<chromRegion*>* regions;
  QListView* regionList;
  QLineEdit* chromLine;   // for choosing a chromosome..
  //  RegionWidget* regionWidget;

  QSpinBox* maxExpect;
  QSpinBox* minAllength;
  QSpinBox* minMatch;   // as a real number

  QSpinBox* selectStart;
  QSpinBox* selectEnd;
  //QIntValidator* startValidator;
  //QIntValidator* endValidator;

  private slots :
    void getChromosome();    // no range for the moment, just get it..
  void selectionChanged(QListViewItem* item);
  void selectRegionChanged(int cs, int ce, int ss, int se);
  void changingRegion(int i);

  public slots :
    void receiveChromosome(chromRegion* cr);
  void updateWidget();

  signals :
    void getRegion(string chromosome, int start, int end);
  void pleaseLoadGenes(vector<int>, QString);
  void pleaseGetEnsemblGenes(int, string, int, int);
};

#endif
