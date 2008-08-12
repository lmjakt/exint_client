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

#ifndef INSITUVIEW_H
#define INSITUVIEW_H

#include "../genomicRegion/localRegionView.h"
#include "imageViewer.h"
#include "../dataStructs.h"
#include "../client.h"            // for the chrom region struct.. should have somewhere else really
#include "../experiment/experimentBrowser.h"
#include "ishAnnotationEditor.h"
#include "imageAdder.h"
//#include <qtable.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <vector>

using namespace std;

class InSituView : public QWidget
{
  Q_OBJECT

    public :
    InSituView(vector<chromRegion>* r, map<int, userInformation>* uInfo, int* id,  QWidget* parent=0, const char* name=0);  // (need another chromregion vector in client.. 

  ExperimentBrowser* exptBrowser;    // make it public so parent can call methods in it.. 
  ImageAdder* imageAdder;            // and we can directly access from the parent as well... terrible practice I know, but there you go.. 

  public slots :
    void newData(int i, int g);
  void addImage(ishImageData data);
  void addFullImage(ishImageData data);      // just calls the functions in the appropriate things..
  void newIshProbeData(ishProbeData data);   

  void receiveTextFields(vector<QString> fields);
  void receiveFloatFields(vector<QString> fields);
  void receiveClasses(vector<QString> classes);

  void ishTextAnnotationHandled(ishTextInsert data);
  void ishFloatAnnotationHandled(ishFloatInsert data);
  void ishClassificationHandled(ishFloatInsert data);

  void newStatus(StatusMessage message);   // propagate appropriately.. 

  private slots :
    void associateIshProbeWithGene(int gIndex);
  void insertText(ishTextInsert data);
  void updateText(ish_annotation data);
  void insertFloat(ishFloatInsert data);
  void insertClassification(ishFloatInsert data);
  void toggleUserAnnotation();
  void addImagesFor(int index);

  signals :
    void getImage(int);               // get the full sized image..
  void whoIsThis(int);                // who is this probe int.. thingy.. please send some relevant information.. 
  ////////////  and pass on the following from localRegionView and pass to the parents.... and then handle appropriately..
  void loadProbeSetsPlease(vector<int>, QString);
  void collectSequences(SequenceRequest*);
  void pleaseLoadImages(int);
  void pleaseLoadThumbnails(int);
  void pleaseAssociateIshProbeWithGene(int, int);    // first is probe, second is gene.. 

  void insertIshTextAnnotation(ishTextInsert);
  void updateIshTextAnnotation(ish_annotation);
  void insertIshFloatAnnotation(ishFloatInsert);
  void insertIshClassification(ishFloatInsert);
  void setIshProbeName(int, QString);        // set the name of the ish probe with this index to .. 

  void getTextFields();
  void getFloatFields();
  void getClasses();

  void getCollection(int);
  void commitProtocolToDB(Protocol*);

  void updateTissues();
  void updateFields();


  private :
    map<int, userInformation>* userInfo;
  int* myId;                          // info about the users..  -- changes when updated.. 

  QScrollView* scroll;             // which absolutely sucks.. 
  ImageViewer* imageViewer;        // the viewer
  LocalRegionView* regionViewer;     // the genomic regions..
  map<int, ishProbeData> probeData;  // data associated with specific probe sets.. -This should be something else.. 
  int currentlySelectedProbe;        // just taken from the new Data slot.. anyway, that is the only that sets data for display.. 
  //  QTable* annotator;    // user Annotation. -- replace with a complete annotation widget later.. 
  ///////// we also need to implement a database query form, so that we can search annotation for probes..
  ////////  -- and something to view that annotation with,,, and something to edit that annotation with as well.
  ////////  -- in addition to which we definetely need some way of providing a similarity index for images.. or 
  ////////  -- some sort of image classification.. but that's for later..
  IshAnnotationEditor* annotEditor;
  protected :
    void resizeEvent(QResizeEvent* e);
};

#endif

