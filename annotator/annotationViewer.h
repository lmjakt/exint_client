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

#ifndef ANNOTATIONVIEWER_H
#define ANNOTATIONVIEWER_H

#include "annotationWidget/annotationWidget.h"
#include "../probe_set/probe_set.h"
#include "../client.h"
#include <qwidget.h>
#include <qframe.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qlabel.h>
#include <vector>
#include <map>

using namespace std;

class AnnotationViewer : public QWidget
{
  Q_OBJECT
    
    public :
    AnnotationViewer(Client* c, QWidget* parent=0, const char* name=0);     // constrcutor
  
  void updateWidgets(probe_data* pdata);       // delete all the widgets, and then make some new ones.
  void updateSessionWidgets(probe_data* pdata);
  void addLabel(QString labelString);
  void clearWidgets();           // delete the annotationWidgets
  
  private slots :
    void includeGenes(bool include);  // include the genes.. 
  void loadIncludedGenes();

  private :
  vector<AnnotationWidget*> widgets;  // the annotation widgets.
  vector<QLabel*> labels;             // generalised, we can add labels at places.. 
  QFrame* frame;
  QScrollView* scroll;
  QLabel* includedGeneCount;  // how many genes included.. 
  QPushButton* loadIncluded;  // load the included buttons.. just emit some sort of signal
  QVBoxLayout* vBox;        // needed in the updateWidgets function..
  Client* client;
  map<int, int> includedGenes;  // genes included by the actions of thingies.. 
  

  protected :
    virtual void resizeEvent(QResizeEvent* e);

  signals :
    void loadTheseGenes(vector<int>, QString);
  void updateAnnotation(int index, QString description);
  void updateSessionAnnotation(int index, vector<QString>);
};

#endif            // have to make sure this one doesn't leak memory.. 
