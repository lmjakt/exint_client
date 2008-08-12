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

#include "inSituView.h"
#include "../dataStructs.h"
#include "../client.h"
#include "../experiment/experimentBrowser.h"
#include "imageViewer.h"
//#include "ishTextEntry.h"
//#include "ishFloatEntry.h"
#include "ishAnnotationEditor.h"
#include <qwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>
#include <iostream>
#include <vector>
#include <qlayout.h>
#include <qvaluelist.h>
//#include <qtable.h>

using namespace std;

InSituView::InSituView(vector<chromRegion>* r, map<int, userInformation>* uInfo, int* id,  QWidget* parent, const char* name)
  : QWidget(parent, name)
{

  userInfo = uInfo;
  myId = id;
  QSplitter* splitter = new QSplitter(Vertical, this);
  // make the local region view and the image viewer.. (in the scroll thing..)...
  scroll = new QScrollView(splitter, "scroll");
  scroll->setVScrollBarMode(QScrollView::AlwaysOn);
  //  splitter->setResizeMode(scroll, QSplitter::Stretch);
  imageViewer = new ImageViewer(scroll->viewport(), "imageViewer");
  scroll->addChild(imageViewer);            // but as imageViewer has no sizeHint,, I'm not sure as to how it will react.. hmm.
  
  //QSplitter* bottomSplit = new QSplitter(Horizontal, splitter);
  regionViewer = new LocalRegionView(r, splitter, "regionViewer", true);
  //annotator = new QTable(10, 2, bottomSplit, "annotation");
  //annotator->setColumnStretchable(0, true);
  //annotator->setColumnStretchable(1, true);

  /// for now just testing some functions...
  annotEditor = new IshAnnotationEditor(userInfo, myId);   // top level.. 
  connect(annotEditor, SIGNAL(pleaseInsertText(ishTextInsert)), this, SLOT(insertText(ishTextInsert)) );
  connect(annotEditor, SIGNAL(pleaseInsertFloat(ishFloatInsert)), this, SLOT(insertFloat(ishFloatInsert)) );
  connect(annotEditor, SIGNAL(pleaseInsertClassification(ishFloatInsert)), this, SLOT(insertClassification(ishFloatInsert)) );
  //  connect(annotEditor, SIGNAL(pleaseUpdateText(ish_annotation)), this, SIGNAL(updateIshTextAnnotation(ish_annotation)) );
  connect(annotEditor, SIGNAL(pleaseUpdateText(ish_annotation)), this, SLOT(updateText(ish_annotation)) );


  connect(annotEditor, SIGNAL(setName(int, QString)), this, SIGNAL(setIshProbeName(int, QString)) );
  connect(annotEditor, SIGNAL(getTextFields()), this, SIGNAL(getTextFields()) );
  connect(annotEditor, SIGNAL(getFloatFields()), this, SIGNAL(getFloatFields()) );
  connect(annotEditor, SIGNAL(getClasses()), this, SIGNAL(getClasses()) );
  
  connect(imageViewer, SIGNAL(toggleUserAnnotation()), this, SLOT(toggleUserAnnotation()) );
  //  annotEditor->show();

  //vector<QString> fieldNames(0);     // no need to put in anytyhing.. 
  //IshTextEntry* textEntry = new IshTextEntry(fieldNames);   // toplevel..
  //textEntry->show();
  //IshFloatEntry* floatEntry = new IshFloatEntry(fieldNames);  
  //floatEntry->show();

  // and that is pretty much everything for the constructor, except we should link the signals from the children..
  connect(imageViewer, SIGNAL(getImage(int)), this, SIGNAL(getImage(int)) );
  connect(imageViewer, SIGNAL(whoIsThis(int)), this, SIGNAL(whoIsThis(int)) );
  connect(regionViewer, SIGNAL(loadProbeSetsPlease(vector<int>, QString)), this, SIGNAL(loadProbeSetsPlease(vector<int>, QString)) );
  connect(regionViewer, SIGNAL(collectSequences(SequenceRequest*)), this, SIGNAL(collectSequences(SequenceRequest*)) );
  connect(regionViewer, SIGNAL(pleaseLoadImages(int)), this, SIGNAL(pleaseLoadImages(int)) );
  connect(regionViewer, SIGNAL(pleaseLoadThumbnails(int)), this, SIGNAL(pleaseLoadThumbnails(int)) );
  connect(regionViewer, SIGNAL(associateIshProbeWithGene(int)), this, SLOT(associateIshProbeWithGene(int)) );
  connect(regionViewer, SIGNAL(addImagesFor(int)), this, SLOT(addImagesFor(int)) );
 // and that should be it for now..

  QVBoxLayout* vbox = new QVBoxLayout(this);
  vbox->addWidget(splitter);    // is it a widget?? 

  QSize goodSize = imageViewer->goodSize(scroll->viewport()->size());
  //imageViewer->resize(goodSize);
  //  QValueList<int> sizes;
  //sizes.append(200);
  //sizes.append(100);
  //splitter->setSizes(sizes);
  
  // a Browser for the experiments..
  exptBrowser = new ExperimentBrowser(userInfo, myId);
  connect(exptBrowser, SIGNAL(getCollection(int)), this, SIGNAL(getCollection(int)) );
  connect(exptBrowser, SIGNAL(commitProtocolToDB(Protocol*)), this, SIGNAL(commitProtocolToDB(Protocol*)) );
  exptBrowser->resize(550, 400);
  //exptBrowser->show();
  
  // and a thing for adding images..
  imageAdder = new ImageAdder();
  imageAdder->resize(450, 600);
  connect(imageAdder, SIGNAL(updateTissues()), this, SIGNAL(updateTissues()) );
  connect(imageAdder, SIGNAL(updateFields()), this, SIGNAL(updateFields()) );
  connect(imageAdder, SIGNAL(getExperiment()), exptBrowser, SLOT(show()) );
  connect(imageAdder, SIGNAL(getExperiment()), exptBrowser, SIGNAL(updateExperiments()) );
  connect(exptBrowser, SIGNAL(selectedExperiment(Experiment)), imageAdder, SLOT(setExperiment(Experiment)) );
  // and we need to have some way of accepting the selected experiment from the experiment chooser.. 
  //  imageAdder->show();

}

void InSituView::toggleUserAnnotation(){
  annotEditor->setShown(!annotEditor->isVisible());
}

void InSituView::insertText(ishTextInsert data){
  emit insertIshTextAnnotation(data);
  // and ask who is this..
  emit whoIsThis(currentlySelectedProbe);
}

void InSituView::updateText(ish_annotation data){
  emit updateIshTextAnnotation(data);
  emit whoIsThis(currentlySelectedProbe);
}

void InSituView::insertFloat(ishFloatInsert data){
  emit insertIshFloatAnnotation(data);
  emit whoIsThis(currentlySelectedProbe);
}

void InSituView::insertClassification(ishFloatInsert data){
  emit insertIshClassification(data);
  emit whoIsThis(currentlySelectedProbe);
}

void InSituView::newData(int i, int g){
  cout << "receiving new data" << endl;
  currentlySelectedProbe = i;        // hmmmmmmmmmmmm. seems fragile.. 
  map<int, ishProbeData>::iterator it = probeData.find(i);
   if(it != probeData.end()){
    g = (*it).second.ensemblGuess;
  }
  regionViewer->newData(i, g);      // hmm?? does what I wonder..
  if(!isVisible()){
    show();
  }
  imageViewer->selectedChanged(i);   // should be OK.. I think?? 
}

void InSituView::addImage(ishImageData data){
  map<int, ishProbeData>::iterator it = probeData.find(data.ishProbeIndex);
  QString label = "NoLabel Man";
  if(it != probeData.end()){
    label = (*it).second.probeName.c_str();
    cout << "\t\t\tSETTING LABEL LABEL !!!!  label to " << (*it).second.probeName.c_str() << endl;
  }
  imageViewer->addImage(data, label);
  QSize goodSize = imageViewer->goodSize(scroll->viewport()->size());
  imageViewer->resize(goodSize);
}

void InSituView::addFullImage(ishImageData data){
  imageViewer->addFullImage(data);
}

void InSituView::newIshProbeData(ishProbeData data){
  if(probeData.count(data.probeId)){
    probeData.erase(data.probeId);        // makes sure that we receive up to date information..
  }
  probeData.insert(make_pair(data.probeId, data));
  annotEditor->setData(data);
  cout << "just called annotEditor to set data" << endl;
}

void InSituView::receiveTextFields(vector<QString> fields){
  cout << "insitu view receive text fiels,, size is " << fields.size() << endl;
  annotEditor->receiveTextFields(fields);
}

void InSituView::receiveFloatFields(vector<QString> fields){
  annotEditor->receiveFloatFields(fields);
}

void InSituView::receiveClasses(vector<QString> classes){
  annotEditor->receiveClasses(classes);
}

void InSituView::resizeEvent(QResizeEvent* e){
  QSize goodSize = imageViewer->goodSize(scroll->viewport()->size());
  imageViewer->resize(goodSize);
  scroll->resizeContents(goodSize.width(), goodSize.height());
  //scroll->resizeContents(goodSize.width(), goodSize.height());
  //scroll->resize(width(), height()/2);
  //regionViewer->resize(width(), height()/2);
}
  
void InSituView::associateIshProbeWithGene(int gIndex){
  emit pleaseAssociateIshProbeWithGene(currentlySelectedProbe, gIndex);
  // and update the view...
  emit whoIsThis(currentlySelectedProbe);
}

void InSituView::ishTextAnnotationHandled(ishTextInsert data){
  // call the appropriate function in the annotator.. -not sure exactly which one this is likely to be..
  cout << "insSituView -- ishtextannotation handled : errorString " << data.errorString << endl;
  annotEditor->ishTextHandled(data);
}

void InSituView::ishFloatAnnotationHandled(ishFloatInsert data){
  // call appropriate function..
  cout << "ishFloatAnnotationHandled errorstring : " << data.errorString << endl;
  annotEditor->ishFloatHandled(data);
}

void InSituView::ishClassificationHandled(ishFloatInsert data){
  // call appropriate function..
  cout << "ishclassification handled errorstring : " << data.errorString << endl;
  annotEditor->ishClassHandled(data);
}

void InSituView::addImagesFor(int index){
  // make sure that the imageAdder has recent fields and other stuff..
  emit updateTissues();
  emit updateFields();
  // then see what we know about this index..
  map<int, ishProbeData>::iterator it = probeData.find(index);
  if(it == probeData.end()){
    cerr << "Coulnd't find a ishprobe data for index : " << index << endl;
    return;
  }
  imageAdder->addImages((*it).second);
}

void InSituView::newStatus(StatusMessage message){
  imageAdder->newStatus(message);
}

/// and for now that is all we need to do.. 
