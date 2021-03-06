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

#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "exptinfo.h"         /// bad..
#include "probe_set/probe_set.h"
#include "client.h"           // so I can access the userInformation.. !! messy, would be better to do another way..
#include "sequence/sequenceRequest.h"
#include <map>
#include "annotator/annotationViewer.h"
#include <qwidget.h>
#include <qtextview.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include "blastAlignPainter/blastAlignWidget.h"
#include "blastAlignPainter/GenAlignWidget.h"
#include "blastAlignPainter/genAlignView.h"
#include "genomicRegion/localRegionView.h"

using namespace std;

class DataView: public QWidget
{
  Q_OBJECT
    public:
  DataView(Client* c, QWidget* parent=0, const char* name=0);
  ~DataView();

  LocalRegionView* localAlignment;            // public so that clientWindow can directly call functions. Not good, but I'm lazy here.
  //LocalRegionView* currentRegion;             // if we have a region selected from the genome, as opposed to selecting other stuff.

  public slots:
    void setText(probe_data* data);          // set the text..
  void setExperiments(map<float, exptInfo>& expInfo);

  private slots:
    void hideShow();
  void toggleVis(QWidget*);
  void switchSource();    // switch source for the alignment.. 
  //  void toggleVis(QTextView*);

 private:
  int tableWidth;
  int exptWidth;

  Client* client;      // the client object.. send the userInformation stuff from here.. !! to the annotationViewer.. 

  QTextView* affyView;
  QTextView* uniView;
  QTextEdit* experiments;      // short descriptions of the experiments.
  AnnotationViewer* annotation;     // user based annotation.. 
  /// and we need to declare some buttons
  QPushButton* affy;
  QPushButton* uni;
  QPushButton* expts;
  QPushButton* genomeAlignments;
  //QPushButton* regionButton;      // hmm 
  QPushButton* switchButton;      // switch the data source for the alignment thingy.. should do this differently, but what the hell 

  QPushButton* userAnnotation;

  signals :
    void updateSessionAnnotation(int, vector<QString>);
  void updateAnnotation(int, QString);
  void pleaseLoadTheseGenes(vector<int>, QString);    // change the set to these genes. ohh please.. 
  void collectSequences(SequenceRequest*);
  void pleaseLoadImages(int);
  void pleaseLoadThumbnails(int);      // for in situ images... 
};

#endif        // I think that is probably all I need for now..

