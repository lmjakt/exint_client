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

#ifndef TWODIMDISTLAUNCHER_H
#define TWODIMDISTLAUNCHER_H

#include "../probe_set/probe_set.h"    // for vital statistics, oh please move this definition one of these days..
#include <qwidget.h>
#include "../stat/twoDimDist.h"
#include "twoDimDistPlotter.h"
#include <vector>
#include <qstring.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qmutex.h>    // these two are necessary in order to check on the progress of the mapping.. 
#include <qprogressbar.h>
#include <qspinbox.h>
#include <set>

// note this doesn't really take care of itself, and certainly doesn't do the OO object 
// encapsulation bit. Use only in the below class as anything outside this will be troublesome

struct statistic {
  std::vector<float> values;
  QString name;
  QLabel* nameLabel;
  QRadioButton* xButton;
  QRadioButton* yButton;
  statistic();
  statistic(vector<float> v, string nme);
  ~statistic();
  void show();  // call show on the things.. 
  void insert(QGridLayout* g, int row);  // insert into a layout gridlayout.. 
};

class TwoDimDistLauncher : public QWidget {
  Q_OBJECT
    
    public :
    TwoDimDistLauncher(const vitalStatistics* s, QWidget* parent=0, const char* name=0);
  ~TwoDimDistLauncher(){
    for(uint i=0; i < stats.size(); i++){
      delete stats[i];
    }
    if(distMapper){
      delete distMapper;
    }
    // also delete the plotters..
    while(plotters.size()){
      delete *plotters.begin();    // segmentation fault ?? 
    }
  }
  
  public slots :
    void newStats();         // gets passed when the vitalStatistics change,, at which point we need to do something..

  private slots :
    void xSelected();
  void ySelected();   // connect to the radiobuttons to make sure the appropriate ones are selected.. 
  void mapDistribution();
  void checkProgress();   // check the progress of the thingy..
  void displayMap();      // not sure how to do this yet, as we have to build a display mapper, but we can check later on. 

  private :
    const vitalStatistics* vitalStats;
    std::vector<statistic*> stats;
  vector<int> indices;    // set these when appropriate.
  QGridLayout* grid;      // for the newly created statistics and stuff..

  // and some other stuff..
  QPushButton* doButton;
  QTimer* progTimer;
  QProgressBar* progBar;
  QSpinBox* sigmaBox;   // set the sigma value (0-100 as a percentage of the thingy.. );
  QCheckBox* logXBox;
  QCheckBox* logYBox;

  QLineEdit* minX;
  QLineEdit* maxX;
  QLineEdit* minY;
  QLineEdit* maxY;
  // This int should be protected by the progMutex. It may not be that important as we are not going to be changing it, just reading, but better to make sure
  int progInt;     // send the address of this int to the thingy, and check. 
  QMutex progMutex;       // use these three to check when the TwoDimDist thingy is done mapping the distribution... 
  TwoDimDist* distMapper;  // set this to 0 when not in use, and create a new one when asked.. -disable the doButton when progressing.. 
  // and something to display the plot with..
  //TwoDimDistPlotter* plotter;  
  std::set<TwoDimDistPlotter*> plotters;    // so I can keep a whole load of them and compare..
  int currentSigma;                         // the sigma used for the current mapping.. 
};

#endif
