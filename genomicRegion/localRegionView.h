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

#ifndef LOCALREGIONVIEW_H
#define LOCALREGIONVIEW_H

#include "localRegionWidget.h"
#include "../client.h"
#include "../sequence/sequenceRequest.h"
#include <qwidget.h>
#include <qscrollview.h>
#include <qspinbox.h>
#include <qtextview.h>
#include <qtextbrowser.h>
#include <vector>

class LocalRegionView : public QWidget 
{
  Q_OBJECT

    public :
    LocalRegionView(vector<chromRegion>* r, QWidget* parent=0, const char* name=0, bool isIshish=false);
  ~LocalRegionView(){
    delete helpView;
  }
  
  public slots :
    void newData(int i, int g);
  void setPSet(int p);     // set the probe set association thingy.. 
  void setSource(vector<chromRegion>* r);    // switch the source to something else.. 
  
  private :
    LocalRegionWidget* alignment;         // for a specific probe set
  //LocalRegionWidget* regionAlignmnet;     // for a specific genomic region.. -It's a little more complicated.. 
  QScrollView* scroll;
  QSpinBox* regionSize;
  QSpinBox* windowSize;
  QSpinBox* maxExpect;
  QSpinBox* minAlLength;
  QSpinBox* minMatch;   // % as it makes more sense.. 

  QTextView* dataTable;
  QTextBrowser* helpView;
  vector<chromRegion>* regions;
  vector<string> fieldNames;

  int currentPSet;
  int currentGene;
  
  private slots :
    void regionSizeChanged(int i);
  void setData(int g);
  void displayTranscriptInfo(string source, string id);
  void setHelp();

  signals :
    void newRegionSize(int);
  void loadProbeSetsPlease(vector<int>, QString);
  void collectSequences(SequenceRequest*);
  void pleaseLoadImages(int);
  void pleaseLoadThumbnails(int);
  void associateIshProbeWithGene(int);
  void addImagesFor(int);

  protected :
    virtual void resizeEvent(QResizeEvent* e);
  
};

#endif

  
