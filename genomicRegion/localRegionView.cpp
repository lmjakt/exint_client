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

#include "localRegionView.h"
#include "localRegionWidget.h"
#include "../client.h"
#include "../sequence/sequenceRequest.h"
#include <qwidget.h>
#include <qscrollview.h>
#include <qlayout.h>
#include <iostream>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtextstream.h>
#include <qtextedit.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <map>
#include <stdlib.h>

using namespace std;

LocalRegionView::LocalRegionView(vector<chromRegion>* r, QWidget* parent, const char* name, bool isIshish)
  : QWidget(parent, name)
{
  currentPSet = 0;
  currentGene = 0;
  regions = r;
  scroll = new QScrollView(this, "scroll");
  //  scroll->setResizePolicy(QScrollView::Manual);
  scroll->setVScrollBarMode(QScrollView::AlwaysOn);   // ugly,,, 
  scroll->setFrameStyle(QFrame::NoFrame);
  //  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
  int wSize = 100000;   // the initial window size .. 
  alignment = new LocalRegionWidget(r, wSize, scroll->viewport(), "alignment", isIshish);
  connect(alignment, SIGNAL(displayInfo(int)), this, SLOT(setData(int)) );
  connect(alignment, SIGNAL(displayTranscriptInfo(string, string)), this, SLOT(displayTranscriptInfo(string, string)) );
  connect(alignment, SIGNAL(loadProbeSetsPlease(vector<int>, QString)), this, SIGNAL(loadProbeSetsPlease(vector<int>, QString)) );
  connect(alignment, SIGNAL(collectSequences(SequenceRequest*)), this, SIGNAL(collectSequences(SequenceRequest*)) );

  connect(alignment, SIGNAL(pleaseLoadThumbnails(int)), this, SIGNAL(pleaseLoadThumbnails(int)) );
  connect(alignment, SIGNAL(pleaseLoadImages(int)), this, SIGNAL(pleaseLoadImages(int)) );
  connect(alignment, SIGNAL(associateIshProbeWithGene(int)), this, SIGNAL(associateIshProbeWithGene(int)) );
  connect(alignment, SIGNAL(addImagesFor(int)), this, SIGNAL(addImagesFor(int)) );
  //alignment->resize(500, 400);
  scroll->addChild(alignment);
  QVBoxLayout* vBox = new QVBoxLayout(this);
  vBox->addWidget(scroll);
  //  vBox->setStretchFactor(scroll, 1);
  // and let's have some controlling elements..
  int indentsize = 10;
  QLabel* rSizeLabel = new QLabel("RegionSize", this, "rSizeLabel");
  rSizeLabel->setIndent(indentsize);
  regionSize = new QSpinBox(10000, 1000000, 10000, this, "regionSize");
  QLabel* windowStart = new QLabel("Window Size", this, "windowStart");
  windowStart->setIndent(indentsize);
  windowSize = new QSpinBox(0, 200000, 100, this, "windowSize");
  connect(windowSize, SIGNAL(valueChanged(int)), alignment, SLOT(setWindowSize(int)) );
  connect(regionSize, SIGNAL(valueChanged(int)), this, SLOT(regionSizeChanged(int)) );
  regionSize->setValue(200000);
  windowSize->setValue(wSize);

  // some QSpinBoxes that control which probe set matches are displayed..
  // and which things get loaded.
  maxExpect = new QSpinBox(-300, 1, 1, this, "maxExpect");
  minAlLength = new QSpinBox(0, 1000, 1, this, "minAlLength");
  minMatch = new QSpinBox(0, 100, 1, this, "minMatch");

  connect(maxExpect, SIGNAL(valueChanged(int)), alignment, SLOT(setMaxExpect(int)) );
  connect(minAlLength, SIGNAL(valueChanged(int)), alignment, SLOT(setMinLength(int)) );
  connect(minMatch, SIGNAL(valueChanged(int)), alignment, SLOT(setMinMatch(int)) );
  
  maxExpect->setValue(-5);
  minAlLength->setValue(22);
  minMatch->setValue(90);

  QLabel* expectLabel = new QLabel("Max Expect", this, "expectLabel");
  QLabel* alLengthLabel = new QLabel("Min Align Length", this, "alLengthLabel");
  QLabel* matchRatioLabel = new QLabel("Min Match %", this, "matchRatio");
  expectLabel->setIndent(indentsize);
  alLengthLabel->setIndent(indentsize);
  matchRatioLabel->setIndent(indentsize);
  
  //// a table for annotation of some gene,, have to make a guess of some sort..
  dataTable = new QTextView(this);
  dataTable->setFrameStyle(QFrame::NoFrame);
  vBox->addWidget(dataTable);

  // and the fieldNames for which we want to display data.. 
  fieldNames.push_back("Ensembl Gene ID");
  fieldNames.push_back("External Gene ID");
  fieldNames.push_back("Description");
  fieldNames.push_back("Synonyms");
  fieldNames.push_back("Ensembl Family ID");
  fieldNames.push_back("Family Description");
  fieldNames.push_back("InterPro Description");
  fieldNames.push_back("Human Gene External ID");
  fieldNames.push_back("Human Gene Description");
  fieldNames.push_back("Prosite ID");
  fieldNames.push_back("PFAM ID");
  fieldNames.push_back("PRINTS ID");
  fieldNames.push_back("Homolog");

  /// andoooh, a help viewer...
  helpView = new QTextBrowser();
  //helpView->setSource("../doc/localRegionView/LocalRegionView_Help.html");
  QPushButton* helpButton = new QPushButton("Confused?", this, "helpButton");
  connect(helpButton, SIGNAL(clicked()), this, SLOT(setHelp()) );

  QHBoxLayout* controlBox = new QHBoxLayout();
  vBox->addLayout(controlBox);
  controlBox->addWidget(rSizeLabel);
  controlBox->addWidget(regionSize);
  controlBox->addWidget(windowStart);
  controlBox->addWidget(windowSize);
  
  controlBox->addWidget(expectLabel);
  controlBox->addWidget(maxExpect);
  controlBox->addWidget(alLengthLabel);
  controlBox->addWidget(minAlLength);
  controlBox->addWidget(matchRatioLabel);
  controlBox->addWidget(minMatch);
  controlBox->addStretch();
  controlBox->addWidget(helpButton);
  
}

void LocalRegionView::setSource(vector<chromRegion>* r){
  regions = r;    
  alignment->setSource(r);
 
  newData(currentPSet, currentGene);
}

void LocalRegionView::newData(int i, int g){
  currentPSet = i;
  currentGene = g;
  alignment->newData(i, g);
  int desHeight = alignment->sizeHint().height();
  int avheight = height()-dataTable->height()-regionSize->height();
  if(desHeight < avheight){ desHeight = avheight; }
  //cout << "visible width is " << scroll->visibleWidth() << endl;
  alignment->update();
  scroll->resizeContents(scroll->viewport()->width(), desHeight);
  alignment->resize(scroll->viewport()->width(), desHeight);
  //cout << "LocalRegionView New DATA : BEST GUESS GENE : " << g << endl;
  //cout << "visible width is " << scroll->visibleWidth() << endl;
  //////////
  ///////// And let's set the text in the data table.

  setData(g);

}

void LocalRegionView::setPSet(int p){
  alignment->setPSet(p);
}

void LocalRegionView::displayTranscriptInfo(string source, string id){
  Transcript transcript;
  for(uint i=0; i < regions->size(); i++){
    transcript = (*regions)[i].getTranscript(source, id);
    if(transcript.length){
      break;
    }
  }
  if(!transcript.length){
    cerr << "Coulnd't find a transcript with source : " << source << "  and id : " << id << endl;
    return;
  }
  QString data;
  QTextStream ts(&data, 2);     // the 2 is the file mode, i.e. write... 
  QString fontd("<font color=\"#ffffff\" size=+1>");
  QString fend("</font>");
  ts << "<qt>";
  ts << "<table width=90% bgcolor=\"#192828\" cellspacing=0 border=0><tr><th colspan=7 align=center>" << fontd << "Fantom Transcript</th></tr>" << fend;
  // and the content..  
  ts << "<tr><td width=\"20%\">" << fontd << transcript.source.c_str() << fend << "</td><td width=80% colspan=6>"
     << fontd << transcript.id.c_str() << fend << "</td></tr>" << endl;
  ts << "<tr><td width=\"20%\">" << fontd << "Chromosome" << fend << "</td><td width=80% colspan=6>"
     << fontd << transcript.chromosome.c_str() << fend << "</td></tr>" << endl;
  ts << "<tr><td width=\"20%\">" << fontd << "Length" << fend << "</td><td width=80% colspan=6>"
     << fontd << transcript.length << fend << "</td></tr>" << endl;
  
  ts << "<tr><td width=\"20%\">" << fontd << "Exons " << "</td></tr>" << endl; 
  int exCounter = 1;
  for(vector<Exon>::iterator it = transcript.exons.begin(); it != transcript.exons.end(); it++){
    ts << "<tr><td width=\"20%\">" << fontd << exCounter << fend << "</td><td width=8% align=right>"
       << fontd << (*it).tstart << fend << "</td><td width=2% align=center>" 
       << fontd << "-" << fend << "</td><td width=8% align=left>" 
       << fontd << (*it).tend << fend << "</td><td width=15% align=right>"
       << fontd << (*it).cstart << fend  <<"</td><td width=2% align=center>"
       << fontd << "-" << fend << "</td><td width=55% align=left>"
       << fontd << (*it).cend << fend
       << "</tr>" << endl;
    exCounter++;
  }
  ts << "</table></qt>";
  dataTable->setPaper(QBrush(QColor(25, 40, 40)));
  dataTable->setColor(QColor(255, 255, 255));
  dataTable->setText(data);
}
  

void LocalRegionView::setData(int g){
  QString data;
  QString fontd("<font color=\"#ffffff\" size=+1>");
  QString fend("</font>");
  QTextStream ts(&data, 2);     // the 2 is the file mode, i.e. write... 
  ts << "<qt>";
  ts << "<table width=90% bgcolor=\"#192828\" cellspacing=0 border=0><tr><th colspan=2 align=center>" << fontd << "Ensembl Annotation</th></tr>" << fend;
  // first let's find the gene in the regions data structure, a bit troublesome, but there you go.
//  bool done = false;
  ensemblGene myGene;
  //vector<ensemblGene>::iterator pit;
  for(uint i=0; i < regions->size(); i++){
    myGene = (*regions)[i].getGene(g);
    if(myGene.dbIndex){
      break;
    }
  }
//     for(vector<ensemblGene>::iterator it=(*regions)[i].fEnsGenes.begin(); it != (*regions)[i].fEnsGenes.end(); it++){
//       if((*it).dbIndex == g){
// 	myGene = &(*it);
// 	pit = it;
// 	done = true;
// 	break;
//       }
//     }
//     if(!done){
//       for(vector<ensemblGene>::iterator it=(*regions)[i].rEnsGenes.begin(); it != (*regions)[i].rEnsGenes.end(); it++){
// 	if((*it).dbIndex == g){
// 	  myGene = &(*it);
// 	  pit = it;
// 	  done = true;
// 	  break;
// 	}
//       }
//     }
//     if(done){
//       break;
//     }
//   }
  if(myGene.dbIndex){
    // allelulialh, I found my gene, now just find all of the stuff, and do the ds thingy....
    pair< multimap<string, string>::iterator, multimap<string, string>::iterator > range;
    multimap<string, string>::iterator mit;
    for(uint j=0; j < fieldNames.size(); j++){
      range = myGene.annotation.equal_range(fieldNames[j]);
      for(mit = range.first; mit != range.second; mit++){
	ts << "<tr><td width=\"20%\">" << fontd << (*mit).first.c_str() << fend << "</td><td width=80%>"
	   << fontd << (*mit).second.c_str() << fend << "</td></tr>" << endl;
      }
    }
  }
  ts << "</table></qt>";
  dataTable->setPaper(QBrush(QColor(25, 40, 40)));
  dataTable->setColor(QColor(255, 255, 255));
  dataTable->setText(data);
}  

void LocalRegionView::resizeEvent(QResizeEvent* e){
  QSize size = e->size();
//   cout << "the size is width " << size.width() << "\t and the height " << size.height() << endl;
//   cout << "and this guy thinks his widhth is " << width() << " and height " << height() << endl;
//   cout << "and scroll thinks " << scroll->width() << "\t" << scroll->height() << endl;
//   cout << "and viewport width is " << scroll->viewport()->width() << endl;

//   cout << "dataTable wants to be " << dataTable->sizeHint().width() << " x " << dataTable->sizeHint().height() << endl;
//   cout << "                but is " << dataTable->width() << " x " << dataTable->height() << endl;
  int desHeight = alignment->sizeHint().height();
  int avheight = height()-dataTable->height()-windowSize->height();
  if(desHeight < avheight){ desHeight = avheight; }
  //  scroll->resize(size.width(), size.height());
  scroll->resizeContents(scroll->viewport()->width(), desHeight); 
  alignment->newSize(scroll->viewport()->width(), desHeight);   // makes sure boxes get repositioned before the resize event
  alignment->resize(scroll->viewport()->width(), desHeight);
  cout << "and viewport width is " << scroll->viewport()->width() << endl;
}
  
void LocalRegionView::regionSizeChanged(int i){
  windowSize->setMaxValue(regionSize->value());
  emit newRegionSize(i);
  cout << "emitting newRegionSize" << endl;
}

void LocalRegionView::setHelp(){
  cout << "trying to display help, ?? " << endl;
  cout << "current source is : " << helpView->source() << endl;
  // try to set it..
  QString filePath("doc/localRegionView");
  const char* path = getenv("CLIENT_DIR");
  if(path){
    filePath.prepend("/");
    filePath.prepend(path);
  }

  helpView->mimeSourceFactory()->setFilePath(filePath);
  helpView->setSource("LocalRegionView_Help.html");
  //  helpView->setSource("doc/localRegionView/LocalRegionView_Help.html");
  cout << "and the source is now " << helpView->source() << endl;
  helpView->resize(900, 700);
  helpView->show();
}
