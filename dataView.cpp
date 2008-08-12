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

#include "dataView.h"
#include "probe_set/probe_set.h"
#include "blastAlignPainter/blastAlignWidget.h"
#include "blastAlignPainter/GenAlignWidget.h"
#include "sequence/sequenceRequest.h"
#include "genomicRegion/localRegionView.h"
#include "exptinfo.h"        // for the exptInfo declaration. this is bad, fix later on..
#include <qstring.h>
#include <qtextview.h>
#include <qtextedit.h>
#include <qcolor.h>
#include <qlayout.h>
#include <qtextstream.h>
#include <qsplitter.h>
#include <qvaluelist.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qsizepolicy.h>
#include <qframe.h>
#include <sstream>
#include <map>
#include <iostream>

using namespace std;

DataView::DataView(Client* c, QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  
  client = c;
  setCaption("Annotation");
  cout << "At the beginning of dataView constructor" << endl;
  QSplitter* base = new QSplitter(this);
  QSplitter* views = new QSplitter(Vertical, base);
  base->setResizeMode(views, QSplitter::Stretch);
  QString welcomeMessage("<qt><h1>Wonderful Stuff!!</h1><h3>Welcome to the world of Confusing Annotation</h3></qt>");
  
  QSplitter* affyUni = new QSplitter(Horizontal, views);
  affyView = new QTextView(affyUni);
  uniView = new QTextView(affyUni);

  annotation = new AnnotationViewer(client, views, "annotationViewer");
  annotation->hide();
  connect(annotation, SIGNAL(updateSessionAnnotation(int, vector<QString>)), this, SIGNAL(updateSessionAnnotation(int, vector<QString>)) );
  connect(annotation, SIGNAL(updateAnnotation(int, QString)), this, SIGNAL(updateAnnotation(int, QString)) );
  connect(annotation, SIGNAL(loadTheseGenes(vector<int>, QString)), this, SIGNAL(pleaseLoadTheseGenes(vector<int>, QString)) );

  cout << "dataView about to create a localRegionView" << endl;
  localAlignment = new LocalRegionView(&client->currentRegions, views, "localAlignment");
  connect(localAlignment, SIGNAL(collectSequences(SequenceRequest*)), this, SIGNAL(collectSequences(SequenceRequest*)) );
  connect(localAlignment, SIGNAL(pleaseLoadThumbnails(int)), this, SIGNAL(pleaseLoadThumbnails(int)) );
  connect(localAlignment, SIGNAL(pleaseLoadImages(int)), this, SIGNAL(pleaseLoadImages(int)) );
  cout << "dataView about to callshow on the localRegionView";
  localAlignment->show();
  cout << "dataView about to create the experiments" << endl;
  experiments = new QTextEdit(base);      // put it on the right hand side. 
  cout << "dataView experiments created Ok" << endl;
//   currentRegion = new LocalRegionView(&client->genRegions, views, "localAlignment");
//   connect(localAlignment, SIGNAL(collectSequences(SequenceRequest*)), this, SIGNAL(collectSequences(SequenceRequest*)) );
//   connect(localAlignment, SIGNAL(pleaseLoadThumbnails(int)), this, SIGNAL(pleaseLoadThumbnails(int)) );
//   connect(localAlignment, SIGNAL(pleaseLoadImages(int)), this, SIGNAL(pleaseLoadImages(int)) );
//   currentRegion->setShown(false);
  // but don't show it.. 
  

  // Lets set up some buttons for hiding and showing the various types of descriptions.. 
  affy = new QPushButton("Affymetrix", this);
  uni = new QPushButton("UniGene", this);
  expts = new QPushButton("Experiments", this);
  genomeAlignments = new QPushButton("Genomic Alignments", this);
  // regionButton = new QPushButton("Genomic Region", this);
  userAnnotation = new QPushButton("User Annotation", this);

  ////////////// rewrite later, this is just for testing..
  switchButton = new QPushButton("Region", this, "Switcher");
  switchButton->setToggleButton(true);
  connect(switchButton, SIGNAL(clicked()), this, SLOT(switchSource()) );

  // make all of them toggle types, and connect them all to the same slot.. 
  affy->setToggleButton(true);
  uni->setToggleButton(true);
  expts->setToggleButton(true);
  genomeAlignments->setToggleButton(true);
  //regionButton->setToggleButton(true);
  userAnnotation->setToggleButton(true);

  affy->setOn(true);
  uni->setOn(true);
  expts->setOn(true);
  genomeAlignments->setOn(true);         // same as show ??? 
  //regionButton->setOn(false);            // don't view unless necessary.. 
  userAnnotation->setOn(false);

  //connect(regionButton, SIGNAL(toggled(bool)), currentRegion, SLOT(setShown(bool)) );    // the proper way to do it.. 

  connect(affy, SIGNAL(clicked()), this, SLOT(hideShow()));
  connect(uni, SIGNAL(clicked()), this, SLOT(hideShow()));
  connect(expts, SIGNAL(clicked()), this, SLOT(hideShow()));
  connect(genomeAlignments, SIGNAL(clicked()), this, SLOT(hideShow()));
  connect(userAnnotation, SIGNAL(clicked()), this, SLOT(hideShow()) );

  QGridLayout* grid = new QGridLayout(this, 2, 6);

  grid->addWidget(affy, 0, 0);
  grid->addWidget(uni, 0, 1);
  grid->addWidget(genomeAlignments, 0, 2);
  grid->addWidget(switchButton, 0, 3);
  //grid->addWidget(regionButton, 0, 4);
  grid->addWidget(userAnnotation, 0, 4);
  grid->addWidget(expts, 0, 5);


  grid->addMultiCellWidget(base, 1, 1, 0, 5);
  affyView->setText(welcomeMessage);
  tableWidth = 400;
  exptWidth = 150;
  QValueList<int> sizes;
  sizes.append(2*tableWidth+32);
  sizes.append(exptWidth+32);
  base->setSizes(sizes);
  QValueList<int> viewSizes;
  viewSizes.append(120);
  viewSizes.append(180);
  viewSizes.append(180);
  views->setSizes(viewSizes);     // incomplete list.. as not everything shown
  resize(2*tableWidth+exptWidth+70, 300);
  
}

DataView::~DataView(){
  // do nothing;
}

void DataView::switchSource(){
  if(switchButton->isOn()){
    cout << "setting source to genRegions with size " << client->genRegions.size() << endl;
    localAlignment->setSource(&client->genRegions);
  }else{
    cout << "setting source to currentRegions with size " << client->currentRegions.size() << endl;
    localAlignment->setSource(&client->currentRegions);
  }
}

void DataView::setText(probe_data* data){
  // process the text in data and format a text string that displays nicely
  //int tableWidth = 500;
  //QString fontd("<font color=\"#ffffff\">");
  QString fontd("<font color=\"#ffffff\" size=+1>");
  QString fend("</font>");

  QString des;
  QTextStream ts(&des, 2);    // for convenience .. 
  ts << "<qt bgcolor=\"#ffff90\">"; // << fontd;   //<font color=\"white\" size=-1>";            // des for description..
  ts << "<table width=" << tableWidth <<  " bgcolor=\"#0000f0\"><tr><th colspan=2>" << fontd << "Affymetrix Annotation</font></th></tr>" << fend;
  ts << "<tr><td width=15%>" << fontd << "Affy Id" << fend << "</td><td width=85%>" << fontd << data->afid.c_str() << fend << "</td></tr>";
  ts << "<tr><td>" << fontd << "Description" << fend << "</td><td>" << fontd << data->afdes.c_str() << fend << "</td></tr>";
  ts << "<tr><td>" << fontd << "TIGR" << fend << "</td><td>" << fontd << data->tigrDescription.c_str() << fend << "</td></tr>";
  ts << "</table>";
  ts << "</qt>";
  affyView->setText(des);
  QString udes;
  QTextStream us(&udes, 2);
  // and the Unigene Data in a separate table.
  us << "<qt bgcolor=\"#ffff90\">"; //<font color=\"white\" size=-1>";            // des for description..
  us << "<table width=" << tableWidth << " bgcolor=\"#0000f0\"><tr><th colspan=3>" << fontd << "UniGene Matches</font></th></tr>" << fend;
  us << "<tr>" << "<td width=20%>" << fontd << "Gene" << fend << "</td><td width=40%>" << fontd << "Title" << fend << "</td><td width=40%>" << fontd << "Index</td>" << fend << "</tr>";
  for(int i=0; i < data->ugData.size(); i++){
    us << "<tr><td>" << fontd << data->ugData[i].gene.c_str() << fend << "</td><td>" << fontd << data->ugData[i].title.c_str() << "</td><td>" << fontd << data->ugData[i].index << "</td></tr></font>";
  }
  us << "</table></font>";
  uniView->setText(udes);

  // and the Celera Data in a third table.. 
//   QString cdes;
//   QTextStream cs(&cdes, 2);
//   cs << "<qt bgcolor=\"#ffff90\"><font color=\"white\" size=-1>";            // des for description..
//   cs << "<table width=" << tableWidth << " bgcolor=\"#0000f0\" cellspacing=0><tr><th colspan=5>" << fontd << "Celera Gene Matches</font></th></tr>";
//   cs << "<tr><td colspan=3 width=80%>" << fontd << "Celera Gene" << fend << "</td><td width=10%>" << fontd << "Expect" << fend << "</td><td width=10%>" << fontd << "Match" << fend << "</td></tr>";
//   for(int i=0; i < data->celeraMatches.size(); i++){
//     cs << "<tr><td colspan=3>" << fontd << data->celeraMatches[i].celeraGene.c_str() << fend << "</td><td>" << fontd << data->celeraMatches[i].expectation << fend
//        << "</td><td>" << fontd << data->celeraMatches[i].match << fend << "</td></tr></font>";
//     cs << "<tr><td colspan=5><blockquote>" << fontd << "sf:\t" << data->celeraMatches[i].sf.c_str() << fend << "</font></blockquote></td></tr>";
//     cs << "<tr><td colspan=5><blockquote>" << fontd << "fn:\t" << data->celeraMatches[i].sf.c_str()  << fend << "</font></blockquote></td></tr>";
//   }
//   cs << "</table></qt>";
  //  celeraView->setText(cdes);

  // and as we have the pointer to the probe data we can also update the blastAligner widget..
  //blastAligner->setData(data);
  //genAlign->setData(data);
  //  genAlign->setData(data);
  annotation->clearWidgets();
  annotation->addLabel("User Annotation");
  annotation->updateWidgets(data);
  annotation->addLabel("User Session Membership");
  annotation->updateSessionWidgets(data);
  // and I think that it would be nice for the button to show an indication of how many user 
  // annotations /sessions are present. For now, just take the sum of the things.
  QString annotationNo;
  annotationNo.sprintf("User Annotation (%d)", data->sessions.size() + data->annotations.size());
  userAnnotation->setText(annotationNo);
  
  //currentRegion->setPSet(data->index);

}

void DataView::setExperiments(map<float, exptInfo>& expInfo){
  QString des;
  QTextStream ds(&des, 2); 
  QString fontd("<font color=\"#ffffff\">");
  QString fend("</font>");
  ds << "<qt bgcolor=\"#ffff90\"><size=-1>";
  ds << "<table width=" << exptWidth << " bgcolor=\"#0000f0\"><tr><th colspan=2>" << fontd << "Short Experiment Descriptions" << fend << "</th></tr>";
  map<float, exptInfo>::iterator it;
  for(it = expInfo.begin(); it != expInfo.end(); it++){
    ds << "<tr><td>" << fontd << (*it).second.dbaseIndex << fend << "</td><td>" << fontd << (*it).second.shortName.c_str() << fend << "</td></tr>";
  }
  ds << "</table></qt>";
  experiments->setText(des);
}
  

void DataView::hideShow(){
  // bit clumsy, but essentially check the button states with isToggled(), and call show or hide.
  // ugly, but easy..
  if(affy->isOn() != affyView->isVisible()) { toggleVis(affyView); }
  if(uni->isOn() != uniView->isVisible()) { toggleVis(uniView); }
  if(expts->isOn() != experiments->isVisible()) { toggleVis(experiments); }
  if(genomeAlignments->isOn() != localAlignment->isVisible()) { toggleVis(localAlignment); }
  if(userAnnotation->isOn() != annotation->isVisible()) { toggleVis(annotation); }
}

void DataView::toggleVis(QWidget* v){
  if(v->isVisible()){
    v->hide();
    return;
  }
  v->show();
}
