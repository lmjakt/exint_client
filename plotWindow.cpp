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

#include "plotWindow.h"
#include "probe_set/probe_set.h"
#include "stat/stat.h"
#include "glPlotter/glPlotter.h"
#include <qwidget.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qslider.h>
#include <qprinter.h>
#include <qpainter.h>
#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace std;

PlotWindow::PlotWindow(vector<int>* es, vector<int>* me, QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  setCaption("Raw Data");
  hasData = false;
  exptSelection = es;        // vector in client,, long way away, be careful, !!!.. 
  markedExperiments = me;
  probePairThreshold = 1;      // default.. 
  QSplitter* split = new QSplitter(this, "split");
  raw = new ExpressionPlotter(ExpressionPlotter::RAW, split, "raw");
  raw->setProbePairThreshold(probePairThreshold);
  cout << "made raw : " << endl;
  //norm = new ExpressionPlotter(split, "norm");
  //norm->setProbePairThreshold(probePairThreshold);
  cout << "made norm : " << endl;
  model = new ExpressionPlotter(ExpressionPlotter::MODEL, split, "model");
  model->setProbePairThreshold(probePairThreshold);
  cout << "made model : " << endl;
  //glRaw = new GLPlotter(split, "glPlotter");
  //cout << "made glplottter " << endl;
  
  connect(raw, SIGNAL(toggleSurfacePlot()), this, SLOT(toggleSurfacePlot()) );
  //connect(norm, SIGNAL(toggleSurfacePlot()), this, SLOT(toggleSurfacePlot()) );
  connect(model, SIGNAL(toggleSurfacePlot()), this, SLOT(toggleSurfacePlot()) );

  connect(raw, SIGNAL(selectedNewFont(QFont)), this, SLOT(setFonts(QFont)) );
  connect(model, SIGNAL(selectedNewFont(QFont)), this, SLOT(setFonts(QFont)) );

  connect(raw, SIGNAL(exportEPS(ExpressionPlotter::PlotType)), this, SLOT(showExportDialog(ExpressionPlotter::PlotType)) );
  connect(model, SIGNAL(exportEPS(ExpressionPlotter::PlotType)), this, SLOT(showExportDialog(ExpressionPlotter::PlotType)) );

  connect(raw, SIGNAL(showExperimentDetails(int)), this, SIGNAL(showExperimentDetails(int)) );
  connect(model, SIGNAL(showExperimentDetails(int)), this, SIGNAL(showExperimentDetails(int)) );

  connect(raw, SIGNAL(toggleSampleInfoWidget()), this, SIGNAL(toggleSampleInfoWidget()) );
  connect(model, SIGNAL(toggleSampleInfoWidget()), this, SIGNAL(toggleSampleInfoWidget()) );

  posPlot = new TwoDPlot();      // make a top level window.
  //  posPlot->show();               // later make this an option in the menu.. 
  pointPlotter = new PointPlotter();
  pointPlotter->setProbeThreshold(probePairThreshold);
  connect(pointPlotter, SIGNAL(setThreshold(float)), this, SLOT(setProbePairThreshold(float)) );
  pointPlotter->resize(200, 300);
  connect(raw, SIGNAL(showPoints()), pointPlotter, SLOT(show()) );
  //connect(norm, SIGNAL(showPoints()), pointPlotter, SLOT(show()) );
  connect(model, SIGNAL(showPoints()), pointPlotter, SLOT(show()) );

  //  pointPlotter->show();           // fix this later to be menu dependent.. 
  //  pointPlotter->resize(200, 300);

  // make some bits and pieces to allow the postscript exporting of postscript plots ..
  set<ExpressionPlotter::PlotType> pTypes;
  pTypes.insert(ExpressionPlotter::RAW);
  pTypes.insert(ExpressionPlotter::MODEL);
  // and a PSDialog
  exportDialog = new PSDialog(pTypes);
//  exportDetails = exportDialog->expWidget();
//  ((QDialog*)exportDialog)->setModal(false);  // Borland doesn't like me to setModal ? not sure why but it defaults to false so should be ok
  connect(exportDialog, SIGNAL(exportPS(QString, ExpressionPlotter::PlotType, int, int)),
	  this, SLOT(exportPS(QString, ExpressionPlotter::PlotType, int, int)) );

  QVBoxLayout* vBox = new QVBoxLayout(this);
  vBox->addWidget(split);

}

void PlotWindow::setCoordinates(vector<PointCoordinate> pts){
  cout << "This is plot window calling set coordinates for the 2dplot thingy .. " << endl;
  posPlot->setCoordinates(pts);
}

void PlotWindow::plot(probe_set* pset){
  // expression plotter takes a reference to a vector< vector<float> > and vector<int> 
  // of expression indices. All we have to do is extract these from the places and
  // things
  //vector< vector<float> > normValues(p->probes.size());
  //  cout << "PlotWindow ,, calling plot() " << endl;
  p = *pset;
  //cout << "PlotWindow,, plot(probe_set* pset function p.exptIndex " << p.exptIndex.size() << endl; 
  hasData = true;
  plot();
}

void PlotWindow::plot(probe_set* pset, QPainter* p, bool rawData){
  // Do not set p to pset,, but rather dereference throughout,,
  vector< vector<float> > rawValues(pset->probes.size());        // we first have to assign these from the exptSelection.. 
  vector<int> experiments;    // because we may not be able to use all of the experiments.. 
  vector<int> experimentIds;  // got a bit confused.. 

  map<int, int>::iterator it;
  for(uint i=0; i < exptSelection->size(); i++){     // do for current experiment selection, makes sense,, 
    it = pset->exptLookup.find((*exptSelection)[i]);
    if(it != pset->exptLookup.end()){
      int n = (*it).second;     // n being the value we need to take from the current index..
      experiments.push_back(n);
      experimentIds.push_back((*exptSelection)[i]);
      for(uint j=0; j < pset->probes.size(); j++){
	rawValues[j].push_back(pset->probes[j][n]);
      }
    }
  }
  //cout << "\tplot() function size of experiments: " << experiments.size() << endl;
  
  if(experiments.size() < 2) { return; }
  if(!rawData){
    // normalise using the zScore fuction from stat ...
    vector< vector<float> > normValues = rawValues;
    model2Normalise(normValues);
    model->paintLines(p, normValues, experimentIds);
  }else{
    raw->paintLines(p, rawValues, experimentIds);     // note these are by reference, and so if the underlying data changes, this might cause a bit of a problem
  }
}

void PlotWindow::plot(){
  vector< vector<float> > rawValues(p.probes.size());        // we first have to assign these from the exptSelection.. 
  vector<int> experiments;    // because we may not be able to use all of the experiments.. 
  vector<int> experimentIds;  // got a bit confused.. 
  vector<int> mExperiments;  // for the marked ones..
  

  map<int, int>::iterator it;
  for(uint i=0; i < exptSelection->size(); i++){
    it = p.exptLookup.find((*exptSelection)[i]);
    if(it != p.exptLookup.end()){
      int n = (*it).second;     // n being the value we need to take from the current index..
      for(uint j=0; j < markedExperiments->size(); j++){
	if((*markedExperiments)[j] == ((*exptSelection)[i])){
	  mExperiments.push_back(experimentIds.size()); // the position of the marked thingy.. 
	}
      }
      experiments.push_back(n);
      experimentIds.push_back((*exptSelection)[i]);
      for(uint j=0; j < p.probes.size(); j++){
	rawValues[j].push_back(p.probes[j][n]);
      }
    }
  }
  // check size of experiments..
  if(experiments.size() < 2){
    return;
  }

  //cout << "\tplot() function size of experiments: " << experiments.size() << endl;
  //vector< vector<float> > normValues = rawValues;
  vector< vector<float> > modelValues = rawValues;

  //cout << "just before zThreshold thingy .. " << endl;
  //  if(experiments.size() < 2) { return; }

  // normalise using the zScore fuction from stat ...
  //zThreshold(normValues);     // -- Funky function adds extra things to, probably is very slow.. 
  cout << "after zthreshold" << endl;
  //  for(int i=0; i < normValues.size(); i++){
  //  zScore(normValues[i]);     // by ref. modifies..
  //}
  // normalise the raw Values..
  cout << "before model normalise " << endl;
  model2Normalise(modelValues);   // just one call !! .. 
  cout << "after model normalise " << endl;
  vector<float> devFromMean = devsFromMean(modelValues);   // just to check..   -- the other one has lots of extra.. 
  //cout << "after devs from mean " << endl;
  //  for(int i=0; i < devFromMean.size(); i++){
  //  cout << "devs from mean profile : " << devFromMean[i] << endl;
  //}
  //cout << "values.size : " << normValues.size() << "  devFromMean.size : " << devFromMean.size() << endl;
  pointPlotter->plotPoints(devFromMean, true);
  cout << "after point plotter plot points .. " << endl;
  // then pass the plot commands to raw and norm..
  //  raw->paintLines(p->probes, p->exptIndex);     // note these are by reference, and so if the underlying data changes, this might cause a bit of a problem
  //  norm->paintLines(normValues, p->exptIndex);
  raw->paintLines(rawValues, experimentIds, mExperiments, devFromMean);     // note these are by reference, and so if the underlying data changes, this might cause a bit of a problem

//  norm->paintLines(normValues, experimentIds, mExperiments, devFromMean);

  model->paintLines(modelValues,experimentIds, mExperiments, devFromMean);
  cout << "all expression plotters done " << endl;
  // done!!

//  glRaw->setData(normValues, experimentIds, mExperiments);
//  cout << "glRaw done .. " << endl;

  posPlot->plot(modelValues, experimentIds, (*markedExperiments));
  cout << "position plot done .. " << endl;
}

void PlotWindow::setPenWidth(int n){
  raw->setPenWidth(n);
  //norm->setPenWidth(n);
  model->setPenWidth(n);
}

void PlotWindow::replot(){
  if(hasData){
    //cout << "memory address of currentProbeSet: " << currentProbeSet << endl;
    plot();
  }
}

void PlotWindow::toggleSurfacePlot(){
  posPlot->setShown(!posPlot->isVisible());
}

void PlotWindow::setProbePairThreshold(float t){
  // go through the different expression plotters and set the threshold..
  probePairThreshold = t;
  raw->setProbePairThreshold(t);  
  //norm->setProbePairThreshold(t);  
  model->setProbePairThreshold(t);
}

void PlotWindow::setFonts(QFont f){
    raw->setFont(f);
    model->setFont(f);

    cout << "Setting fonts to " << f.family() << "  :  " << f.pointSize() << endl;

    raw->update();
    model->update();
}

void PlotWindow::showExportDialog(ExpressionPlotter::PlotType type){
    cout << "PlotWindow::exportPlot trying to do some stuff " << endl;

    exportDialog->prepareExport(type);
    exportDialog->show();
    exportDialog->raise();
}

void PlotWindow::exportPS(QString fname, ExpressionPlotter::PlotType type, int w, int h){
    cout << "PlotWindow::exportPS with parameters " << fname.latin1() << " : " << type << "  w: " << "  h: " << h << endl;
    if(fname.isNull()){
	cout << "but it looks like fname is Null " << endl;
	return;
    }
    // and now we need to prepare a QPrinter and then make a thingy with it..
    
    QPrinter printer;
//    QPrinter printer(QPrinter::HighResolution);   // which sets it to screen resolution
    printer.setColorMode(QPrinter::Color);
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageSize(QPrinter::B0);
    printer.setOutputFileName(fname);
    QPainter p(&printer);
    // then simply make a QPainter and paint to that one..
    switch(type){
	case ExpressionPlotter::RAW :
	    raw->paintLines(&p, w, h);
	    break;
	case ExpressionPlotter::MODEL :
	    model->paintLines(&p, w, h);
	    break;
	default :
	    cerr << "PlotWindow::exportPlot unknown Plot Type : " << type << endl;
    }
    p.end();
    // we might need to end the plotter or something .. but I'm not sure.. 
}
