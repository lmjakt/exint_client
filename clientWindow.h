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

#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include "client.h"
//#include "expressionplotter.h"
#include "exptinfo.h"    // for the exptInfo struct.. 
#include "plotWindow.h"
#include "sampleInfo/sampleInfoWidget.h";
#include "probe_set/probe_set.h"
#include "dbQuerier.h"
#include "dataView.h"
//#include "jpgView/imageViewer.h"
#include "jpgView/inSituView.h"
#include "experimentChooser/experimentChooser.h"
 //#include "experimentChooser.h"    remove all references to this
//#include "exptTable.h"             and this one and do something a bit more in line with the list windows. 
#include "comparisonWindow/comparisonwindow.h"
#include "listWindow/listWindow.h"
#include "probeSetWidget/probeSetWidget.h"
#include "meanPlotWindow/meanPlotWindow.h"
#include "passwdchange/passwdchange.h"
#include "statPlotter/statWindow.h"
#include "annotator/annotator.h"
#include "clusterWindow/clusterWindow.h"
#include "genomicRegion/regionWindow.h"
#include "distanceMapper/compareController.h"
#include "protocol/protocolManager.h"
#include "customWidgets/lSpinBox.h"
#include "dialogs/exportMeanDialog.h"
#include <qvbox.h>
#include <qtextview.h>
#include <qstring.h>
#include <qlineedit.h>
#include <vector>
#include <list>
#include <fstream>
#include <qwidget.h>
#include <qspinbox.h>

using namespace std;

struct writeRequest{
  set<int> indicesToWrite;
  string dirName;
  ofstream* out;    // for the html file...
  writeRequest(set<int> i, string dn, ofstream* o);   // easier for now.. not so OO but hell who cares. 
  ~writeRequest();
};


class ClientWindow : public QWidget
{
  Q_OBJECT

    public:
  ClientWindow(QWidget* parent=0, const char* name=0);
  ~ClientWindow();
  vector<int> markedExperiments;           // maybe it needs to be public I'm not sure.. 

  private slots:
    void writeMessage(QString);
  //void sendMessage();
  //void plotMean(probe_set);
  void plotValues(probe_set);
  void newSet(probe_set*, probe_data*);
  void newProbeData(probe_data* pd);
  void saveProbeSetWidget(ProbeSetWidget* psw);
  void pleaseForgetMe(ProbeSetWidget*);
  void setRegionRange(int n);
  void clearSavedOnes();    // clear all of the saved ones.. 
  void getProbeSet(int);
  void changeIndexRange(int);
  void changeIndex(vector<int> v, QString descriptor);
  void restoreIndex(vector<int> v, int i, QString descriptor);    // sets the position as something other than 0.. 
  // void testing();       // used for updating experimental data,, as the signals slots can't seem to cope
  void changeExperimentInformation();
  void newExptSelection(vector<int>);
  void newMarkSelection(vector<int>);
  void doEuclidSort();
  void changePassword(QString oldp, QString newp);
  void createNewUser(QString oldp, QString newp, QString uName);   // should really have more information, like laboratory, and so one. but never mind. 

  void toggleExpressionWindow(bool);
  void toggleMeanPlotWindow(bool);
  void toggleDataWindow(bool);
  void toggleExperimentWindow(bool);
  void toggleComparisonWindow(bool);
  void toggleHistoryWindow(bool);
  void toggleSavedWindow(bool);

  void newAnnotation(int state, int sessionIndex, QString note);
  
  void writeHistory();         // good one that.. !! 
  void writeHtmlReport();      // and how about that one.. Maybe one day, I can make it real pretty !! 
  void saveCurrentState();     // write the current Index, and saved things to a file.. 
  void readState();            // arghh, now how to do this properly.. hmm.. 
  vector<int> hashPasswd(QString passwd);           // some sort of simple algorithm to hash the password.. 

  void shiftFocus(int id);        // shift the focus depeding on the id give.. 
  void writeCurrentIndex();       // create a writeRequest, and write an html report.. as before. very similar to some other things
  void writeCurrentProbeSet(writeRequest* wr);   // write current probe set to the thing in outfile.. 
  void exportMeans();                            // request the server to export the means for the currently selected set of experiments and indices.. 

  void selectFont();                             // select a font for the application.. 

  //// mapping experimental distances to a n-d surface..
  void newExptDistances(exptDistanceInfo data);
  

  ////////////// READ COMMANDS : DESIGNED FOR STRESSING THE SERVER IN ORDER TO FIND ERRORS.. ////
  void readCommands();
  void abuseTheServer();  
  void requestProtocolCollection(int requestId);    // work out who is doing the requesting.. 
  void receiveProtocolCollection(int requester, int requestId, ProtocolCollection* collection);

  //// set space coordinates for different cell types, to allow for 2 or more dimensional distribution of points..
  void setCoordinates(vector<PointCoordinate> pts);

  void showExperimentDetails(int n);
  void toggleSampleInfoWidget();

  private:
  QTextView* messages;
  PlotWindow* plotWindow;
  MeanPlotWindow* meanPlotWindow;
  SampleInfoWidget* sampleInfoWidget;
  //QLineEdit* indexSize; 
  //QLineEdit* inputWindow;
  Client* client;                         // for access to public data sets.. not very good I know, but there you go.. 

  LSpinBox* getRegion;                    // this is ugly, but what the hell... what can we do.. 
  //  QSpinBox* getRegion;                    // this is ugly, but what the hell... what can we do.. 
  LSpinBox* getProbe;
  LSpinBox* lineWidth;
  //QSpinBox* getProbe;
  //QSpinBox* lineWidth;
  DBQuerier* dbQuerier;  
  DataView* dataView;
  ExperimentChooser* experimentChooser;
  //ExptTable* expTable;
  ComparisonWindow* comparisonWindow;
  PasswdChange* passwordWindow;          // for changing the password. I will need client implemenatations though.. 
  StatWindow* statwindow;                // for displaying stats.. !! 
  Annotator* annotator;                  // for user annotation !! 
  ClusterWindow* clusterWindow;          // to start clustering. Later may also hold the results of clustering. 
  RegionWindow* regionWindow;            // a window for displaying and selecting genomic regions.. 
  InSituView* thumbnailViewer;
  //  ImageViewer* thumbnailViewer;
  // For Comparing experiments with each other.. i.e. cell types.. mostly..
  CompareController* exptComparisons;

  
  deque<ProbeSetWidget*> history;        // our history as told by things.. 
  list<ProbeSetWidget*> savedOnes;      // the ones we saved. Actual copies of the things.. wasteful, but simple
  set<int> savedIndices;                 // the indices that have already been saved so we don't don't waste space
  set<int> indicesToSave;                // if we read state from somewhere.. !. 
  int historySize;                       // so we know when to forget the things we used to know.. 
  int currentProbeSetId;                 // try to keep this synchronised. id as in the db index, !
  set<writeRequest*> reportRequests;       // try to write reports for these genes, or something.. 


  ListWindow* listWindow;                // in fact at the moment this contains the history list.. 
  ListWindow* savedListWindow;                // interesting ones that are good..

  // Protocol Stuff.. 
  ProtocolManager* protManager;

  ExportMeanDialog* exportDialog;

  //set<DistanceViewer*> distanceViewers;    // create a new one each time as they run their own threads.. .. 
  // Some user variables..
  QString userName;
  QString serverName;        // 
  //  vector<int> userKeys;      // the passwd hashed. -- maybe should not be kept in memory.. 
  
  // list of experiments To Mark..
  ///// and something really ugly... --
  int ignoreIndex;          // if this index defined, then don't display the data for this one.. or something
};

#endif
