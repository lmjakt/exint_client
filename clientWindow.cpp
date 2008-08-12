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

#include "clientWindow.h"
#include "client.h"
#include "experimentChooser/experimentChooser.h"
#include "plotWindow.h"
#include "dataView.h"
#include "probe_set/probe_set.h"
#include "exptinfo.h"
#include "comparisonWindow/comparisonwindow.h"
#include "listWindow/listWindow.h"
#include "probeSetWidget/probeSetWidget.h"
#include "meanPlotWindow/meanPlotWindow.h"
#include "logindialog/logindialog.h"
#include "passwdchange/passwdchange.h"
#include "statPlotter/statWindow.h"
#include "annotator/annotator.h"
#include "annotator/sessionManager/sessionWidget.h"
#include "sequence/sequenceRequest.h"
#include "clusterWindow/clusterWindow.h"
#include "genomicRegion/regionWindow.h"
#include "indexHistory/historyWidget.h"
#include "dataStructs.h"
#include "chips/chipChooser.h"
#include "chips/chipSelection.h"
#include <qdir.h>
#include <qvbox.h>
#include <qtextview.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qpalette.h>
#include <vector>
#include <list>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtextstream.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qfontdialog.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qaccel.h>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>   // for getenv.. 

///// REMOVE LATER ////
//#include "protocol/protocolStepWidget.h"
//#include "protocol/protocolViewer.h"
//#include "protocol/protocolHolder.h"
#include "protocol/protocolManager.h"
//////////////////////



using namespace std;

writeRequest::writeRequest(set<int> i, string dn, ofstream* o){
  dirName = dn;
  indicesToWrite = i;
  out = o;
}

writeRequest::~writeRequest(){
  cout << "closing out " << endl;
  out->close();
  delete out;
};

ClientWindow::ClientWindow(QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  historySize = 50;        // we can change later on.. !!
  setPalette(QPalette(QColor(110, 197, 209), QColor(184, 175, 198)));
  currentProbeSetId = 0;     // which is an impossible probe set.. they start at 1, useful.. 
  setCaption("Main Window");
  // and set an icon for the window.. 
  QString iconPix = "helix_64.xpm";
  QString iconFile;
  const char* path = getenv("CLIENT_DIR");
  if(path){
    iconFile += path;
    iconFile += "/";
  }
  iconFile += iconPix;
  cout << "iconFile = " << iconFile << endl;
  setIcon(QPixmap(iconFile));

  QString passwd;
  QString portString = "8108";
  int port;
  serverName = ("172.21.2.99");
  // the above are hardcoded defaults, which is not good. First check for the presence of a file containing
  // defaults. 
  
  // and if we hav e path, (already checked above ..)
  QString filePath("prefs/settings");
  if(path){
    filePath.prepend("/");
    filePath.prepend(path);
  }   // which is terribly ugly, but there you go. 
  // read file if possible,, just use simple parsing.. 
  ifstream in(filePath.latin1());
  string key, value;
  cout << "Tried to open " << filePath << endl;
  if(in){   // read key value pairs..
    cout << "and it worked " << endl;
    while(in >> key){
      if(in >> value){
	// and then set values accordingly.. in a somewhat obtuse manner..
	// can't use a case on this, so lots of if statements, ugly, but..
	if(key == "port"){ portString = value.c_str();	}
	if(key == "serverAddress") { serverName = value.c_str(); }
      }
    }
  }
  // but then let these be overriden by the user directly as should be the case.. 

  LoginDialog* login = new LoginDialog(&userName, &passwd, &serverName, &portString, this, "logindialog");
  login->setPalette(palette());
  int returnValue = login->exec();
  cout << "Login returned " << returnValue << endl;
  cout << "user name   : " << userName << endl
       << "server name : " << serverName << endl
       << "port        : " << portString << endl;
  vector<int> userKeys = hashPasswd(passwd);
  if(!returnValue){
    cout << "Action Cancelled, enjoy the break" << endl;
    exit(0);
  }

  bool ok;
  port = portString.toInt(&ok);
  if(!ok){
    cerr << "Coulnd't get a port number from string : " << portString << endl;
    exit(1);
  }
  
  client = new Client(userName, serverName, userKeys, port);
  connect(client, SIGNAL(statusMessage(QString)), this, SLOT(writeMessage(QString)) );
  connect(client, SIGNAL(newPSet(probe_set)), this, SLOT(plotValues(probe_set)) );
  connect(client, SIGNAL(newIndex(int)), this, SLOT(changeIndexRange(int)) );
  connect(client, SIGNAL(probeDataChanged()), this, SLOT(writeHistory()) );

//   QString sizeString;
//   sizeString.setNum(client->indexSize());
//   sizeString.prepend("Current Index size:\t");
//   indexSize = new QLineEdit(sizeString, this, "indexSize");
//   indexSize->setReadOnly(true);

  messages = new QTextView();
  //  messages->setMaximumHeight(45);

  plotWindow = new PlotWindow(&client->selectedExperiments, &markedExperiments);    // topLevel window. 
  connect(plotWindow, SIGNAL(showExperimentDetails(int)), this, SLOT(showExperimentDetails(int)) );
  connect(plotWindow, SIGNAL(toggleSampleInfoWidget()), this, SLOT(toggleSampleInfoWidget()) );
  //  cout << "Plot Window created " << endl;
  plotWindow->resize(600, 300);
  //cout << "Resized plot window : " << endl;
  //plotWindow->show();
  //cout << "calling show on plot window : " << endl;
  //cout << "making mean plot window : " << endl;
  meanPlotWindow = new MeanPlotWindow(&client->selectedExperiments, &client->expInfo);
  meanPlotWindow->resize(600, 300);
  cout << "made mean plot window : " << endl;

  sampleInfoWidget = new SampleInfoWidget();
  sampleInfoWidget->resize(400, 200);

  lineWidth = new LSpinBox(0, 10, 1, this, "Line Width");
  //  lineWidth = new QSpinBox(0, 10, 1, this);
  connect(lineWidth, SIGNAL(valueChanged(int)), plotWindow, SLOT(setPenWidth(int)) );
  connect(lineWidth, SIGNAL(valueChanged(int)), meanPlotWindow, SLOT(setPenWidth(int)) );
  lineWidth->setValue(5);

  getProbe = new LSpinBox(0, 10000, 1, this, "Probe");
  //  getProbe = new QSpinBox(0, 10000, 1, this);
  connect(getProbe, SIGNAL(valueChanged(int)), this, SLOT(getProbeSet(int)) );

  getRegion = new LSpinBox(0, 0, 1, this, "Region");    // no range to start with.. 
  //  getRegion = new QSpinBox(0, 0, 1, this);    // no range to start with.. 
  connect(getRegion, SIGNAL(valueChanged(int)), client, SLOT(getRegion(int)) );
  connect(client, SIGNAL(newRegionIndex(int)), this, SLOT(setRegionRange(int)) );

  // a thing for index histories...
  HistoryWidget* indexHistory = new HistoryWidget(client->indexHistory());
  connect(client, SIGNAL(indexHistoryChanged()), indexHistory, SLOT(newIndex()) );
  connect(indexHistory, SIGNAL(loadProbes(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );
  connect(indexHistory, SIGNAL(restoreIndex(vector<int>, int, QString)), this, SLOT(restoreIndex(vector<int>, int, QString)) );
  //  indexHistory->show();
  indexHistory->resize(350, 50);
  QPushButton* indexHistoryButton = new QPushButton("Index History", this, "indexHistoryButton");
  connect(indexHistoryButton, SIGNAL(clicked()), indexHistory, SLOT(show()) );

  //QPushButton* reConnect = new QPushButton("reopen connection", this, "reConnect");
  //connect(reConnect, SIGNAL(clicked()), client, SLOT(reOpenConnection()) );

  dbQuerier = new DBQuerier(this);           
  connect(dbQuerier, SIGNAL(doLookup(QString)), client, SLOT(doDBLookup(QString)) );  
  connect(dbQuerier, SIGNAL(doGenLookup(QString)), client, SLOT(doGenLookup(QString)) );
  connect(client, SIGNAL(newDBChoices(vector<string>)), dbQuerier, SLOT(setChoices(vector<string>)) );
  connect(client, SIGNAL(newRegionChoices(vector<string>)), dbQuerier, SLOT(setRegionChoices(vector<string>)) );

  dataView = new DataView(client);     // top level widget..
  connect(client, SIGNAL(newPData(probe_data*)), this, SLOT(newProbeData(probe_data*)) );
  //  connect(client, SIGNAL(newPData(probe_data*)), dataView, SLOT(setText(probe_data*)) );
  connect(dataView, SIGNAL(updateSessionAnnotation(int, vector<QString>)), client, SLOT(updateSessionDescription(int, vector<QString>)) );
  connect(dataView, SIGNAL(updateAnnotation(int, QString)), client, SLOT(updateAnnotationDescription(int, QString)) );
  connect(dataView, SIGNAL(pleaseLoadTheseGenes(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );
  connect(dataView, SIGNAL(collectSequences(SequenceRequest*)), client, SLOT(collectSequences(SequenceRequest*)) );
  connect(dataView, SIGNAL(pleaseLoadThumbnails(int)), client, SLOT(getIshThumbnails(int)) );
  connect(dataView, SIGNAL(pleaseLoadImages(int)), client, SLOT(getIshImages(int)) );
 
  /// damn, I'm mixing things up,,, .. too much here.. should make dataView->localAlignment private and pass the buck.. instead.
  // as I'm doing above for the collectSequence and images stuff.. 
  connect(client, SIGNAL(newCurrentRegions(int, int)), dataView->localAlignment, SLOT(newData(int, int)) );
  connect(dataView->localAlignment, SIGNAL(newRegionSize(int)), client, SLOT(setRegionSize(int)) );
  connect(dataView->localAlignment, SIGNAL(loadProbeSetsPlease(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );
  //dataView->show();

  connect(client, SIGNAL(newRegion(int, int)), dataView->localAlignment, SLOT(newData(int, int)) );
  //
  //connect(dataView->currentRegion, SIGNAL(loadProbeSetsPlease(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );

  experimentChooser = new ExperimentChooser(); 
  markedExperiments.resize(0);  // shouldn't have anything.. at the moment. this is probably unnecessary.. 

  connect(client, SIGNAL(exptInfoChanged()), this, SLOT(changeExperimentInformation()) );
  connect(experimentChooser, SIGNAL(newExptSelection(vector<int>)), this, SLOT(newExptSelection(vector<int>)) );
  connect(experimentChooser, SIGNAL(newMarkSelection(vector<int>)), this, SLOT(newMarkSelection(vector<int>)) );

  // and something for changing the password with..
  passwordWindow = new PasswdChange();     // floating window..
  passwordWindow->setPalette(palette());
  connect(passwordWindow, SIGNAL(changePasswd(QString, QString)), this, SLOT(changePassword(QString, QString)) );
  connect(client, SIGNAL(passWordError()), passwordWindow, SLOT(passWordError()) );
  connect(client, SIGNAL(passWordChanged(bool)), passwordWindow, SLOT(passwordChanged(bool)));
  connect(passwordWindow, SIGNAL(createNewUser(QString, QString, QString)), this, SLOT(createNewUser(QString, QString, QString)) );

  //passwordWindow->show();   // lets have a look..
  QPushButton* passwdShow = new QPushButton("passwd", this, "change_passwd");
  connect(passwdShow, SIGNAL(clicked()), passwordWindow, SLOT(show()) );
  
  statwindow = new StatWindow(&client->vitalStats);
  QPushButton* showStats = new QPushButton("Show Stats", this, "showStats");
  connect(showStats, SIGNAL(clicked()), statwindow, SLOT(show()) );
  connect(client, SIGNAL(newVitalStats()), statwindow, SLOT(newStats()) );
  connect(statwindow, SIGNAL(setIndex(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );

  annotator = new Annotator(this, "annotator");         // nothing in the constructor for now..
  connect(annotator, SIGNAL(createSession(vector<string>)), client, SLOT(createSession(vector<string>)) );
  connect(client, SIGNAL(sessionCreated(int)), annotator, SLOT(sessionCreated(int)) );
  connect(annotator, SIGNAL(newAnnotation(int, int, QString)), this, SLOT(newAnnotation(int, int, QString)) );
  connect(annotator, SIGNAL(updateSessions()), client, SLOT(updateSessions()) );
  connect(client, SIGNAL(readingSessionWidgets()), annotator, SLOT(clearSessions()) );
  connect(client, SIGNAL(newSessionWidget(SessionWidget*)), annotator, SLOT(addSession(SessionWidget*)) );
  connect(annotator, SIGNAL(newIndex(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );
  connect(annotator, SIGNAL(updateSessionDescription(int, vector<QString>)), client, SLOT(updateSessionDescription(int, vector<QString>)) );
  connect(annotator, SIGNAL(copyToCurrentSession(int, vector<int>, QString)), client, SLOT(newAnnotation(int, vector<int>, QString)) );

  clusterWindow = new ClusterWindow(&client->clusterSets);   // top level..
  clusterWindow->setPalette(palette());
  connect(clusterWindow, SIGNAL(doCluster(int, bool, bool, bool)), client, SLOT(doKCluster(int, bool, bool, bool)) );
  connect(client, SIGNAL(newClusterSet(clusterSet*)), clusterWindow, SLOT(newClusterSet(clusterSet*)) );
  connect(clusterWindow, SIGNAL(pleaseLoadTheseGenes(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );
  QPushButton* clusterButton = new QPushButton("Clusters", this, "clusterButton");
  connect(clusterButton, SIGNAL(clicked()), clusterWindow, SLOT(show()) );
  //  clusterWindow->show();                 // just so we can have a look we'll change it later. 

  regionWindow = new RegionWindow(&client->chromosomalRegions);
  regionWindow->setPalette(palette());
  connect(regionWindow, SIGNAL(getRegion(string, int, int)), client, SLOT(getGenomicRegion(string, int, int)) );
  connect(regionWindow, SIGNAL(pleaseLoadGenes(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );
  connect(regionWindow, SIGNAL(pleaseGetEnsemblGenes(int, string, int, int)), client, SLOT(getGenomicRegionEnsemblGenes(int, string, int, int)) );
  connect(client, SIGNAL(newChromRegion(chromRegion*)), regionWindow, SLOT(receiveChromosome(chromRegion*)) );
  connect(client, SIGNAL(newEnsemblGeneRegion()), regionWindow, SLOT(updateWidget()) );     // ok.. should be OK.. 
  // and a button to make it visible..
  QPushButton* regionButton = new QPushButton("Gen Region", this, "regionButton");
  connect(regionButton, SIGNAL(clicked()), regionWindow, SLOT(show()) );


  // and let's make an image viever
  thumbnailViewer = new InSituView(&(client->currentIshRegions), &(client->userInfo), &(client->userId));       //// this should not be the current Regions, but some other set, but for testing purpose leave here.
  //thumbnailViewer->setPalette(palette()); -- doesn't really work very well with the palette I have been using.. 
  connect(client, SIGNAL(newIshThumbnail(ishImageData)), thumbnailViewer, SLOT(addImage(ishImageData)) );
  connect(client, SIGNAL(newIshImage(ishImageData)), thumbnailViewer, SLOT(addFullImage(ishImageData)) );
  connect(thumbnailViewer, SIGNAL(getImage(int)), client, SLOT(getIshImage(int)) );
  connect(thumbnailViewer, SIGNAL(whoIsThis(int)), client, SLOT(getRegionForIshProbe(int)) );
  connect(thumbnailViewer, SIGNAL(pleaseAssociateIshProbeWithGene(int, int)), client, SLOT(associateIshProbeWithGene(int, int)) );
  connect(thumbnailViewer, SIGNAL(loadProbeSetsPlease(vector<int>, QString)), this, SLOT(changeIndex(vector<int>, QString)) );
  connect(client, SIGNAL(newCurrentIshRegions(int, int)), thumbnailViewer, SLOT(newData(int, int)) );    /// which again is the wrong signal..
  connect(client, SIGNAL(newIshProbeData(ishProbeData)), thumbnailViewer, SLOT(newIshProbeData(ishProbeData) ));


  connect(thumbnailViewer, SIGNAL(insertIshTextAnnotation(ishTextInsert)), client, SLOT(insertIshTextAnnotation(ishTextInsert)) );
  connect(thumbnailViewer, SIGNAL(updateIshTextAnnotation(ish_annotation)), client, SLOT(updateIshTextAnnotation(ish_annotation)) );
  connect(thumbnailViewer, SIGNAL(insertIshFloatAnnotation(ishFloatInsert)), client, SLOT(insertIshFloatAnnotation(ishFloatInsert)) );
  connect(thumbnailViewer, SIGNAL(insertIshClassification(ishFloatInsert)), client, SLOT(insertIshClassification(ishFloatInsert)) );
  connect(thumbnailViewer, SIGNAL(setIshProbeName(int, QString)), client, SLOT(setIshProbeName(int, QString)) );

  connect(client, SIGNAL(newStatus(StatusMessage)), thumbnailViewer, SLOT(newStatus(StatusMessage)) );
  
  connect(client, SIGNAL(ishTextAnnotationHandled(ishTextInsert)), thumbnailViewer, SLOT(ishTextAnnotationHandled(ishTextInsert)) );
  connect(client, SIGNAL(ishFloatAnnotationHandled(ishFloatInsert)), thumbnailViewer, SLOT(ishFloatAnnotationHandled(ishFloatInsert)) );
  connect(client, SIGNAL(ishClassificationHandled(ishFloatInsert)), thumbnailViewer, SLOT(ishClassificationHandled(ishFloatInsert)) );

  connect(thumbnailViewer, SIGNAL(getTextFields()), client, SLOT(getIshTextFields()) );
  connect(thumbnailViewer, SIGNAL(getFloatFields()), client, SLOT(getIshFloatFields()) );
  connect(thumbnailViewer, SIGNAL(getClasses()), client, SLOT(getIshClasses()) );

  connect(thumbnailViewer, SIGNAL(getCollection(int)), this, SLOT(requestProtocolCollection(int)) );
  connect(thumbnailViewer, SIGNAL(commitProtocolToDB(Protocol*)), client, SLOT(commitProtocolToDB(Protocol*)) );
  connect(thumbnailViewer->exptBrowser->eMaker, SIGNAL(getProtocol(int)), client, SLOT(getProtocol(int)) );
  connect(thumbnailViewer->exptBrowser->eMaker, SIGNAL(commitExperimentToDB(Experiment, int)), 
	  client, SLOT(commitExperimentToDB(Experiment, int)) );
  connect(client, SIGNAL(newExperiment(int, Experiment)), thumbnailViewer->exptBrowser, SLOT(addExperiment(int, Experiment)) );
  connect(thumbnailViewer->exptBrowser, SIGNAL(updateExperiments()), client, SLOT(updateExperiments()) );
  connect(client, SIGNAL(newExperiments(vector<Experiment>)), thumbnailViewer->exptBrowser, SLOT(setExperiments(vector<Experiment>)) );
  connect(thumbnailViewer->exptBrowser, SIGNAL(loadProtocol(int)), client, SLOT(getProtocol(int)) );

  connect(thumbnailViewer, SIGNAL(updateTissues()), client, SLOT(requestTissues()) );
  connect(thumbnailViewer, SIGNAL(updateFields()), client, SLOT(requestIshAnnotationFields()) );
  connect(client, SIGNAL(newTissues(vector<tissue>)), thumbnailViewer->imageAdder, SLOT(setTissues(vector<tissue>)) );
  connect(client, SIGNAL(newIshAnnotationFields(vector<ishAnnotationField>)), thumbnailViewer->imageAdder, SLOT(setIshAnnotationFields(vector<ishAnnotationField>)) );
  
  connect(thumbnailViewer->imageAdder, SIGNAL(makeField(QString)), client, SLOT(makeIshAnnotationField(QString)) );
  connect(thumbnailViewer->imageAdder, SIGNAL(makeTissue(QString, float)), client, SLOT(makeIshTissue(QString, float)) );
  connect(thumbnailViewer->imageAdder, SIGNAL(commitImageToDB(ishImageSet)), client, SLOT(commitImageToDB(ishImageSet)) );

  // unfortunately we actually do need to split the protocol when it arrives as well,, -as the in situ view won't receive
  // any new protocols at the moment.. - This is a bit of a troublesome situation.. have to think about a better way of dealing
  // with this kind of problem in the future.. 

  connect(client, SIGNAL(ishTextFields(vector<QString>)), thumbnailViewer, SLOT(receiveTextFields(vector<QString>)) );
  connect(client, SIGNAL(ishFloatFields(vector<QString>)), thumbnailViewer, SLOT(receiveFloatFields(vector<QString>)) );
  connect(client, SIGNAL(ishClasses(vector<QString>)), thumbnailViewer, SLOT(receiveClasses(vector<QString>)) );
  
  cout << "About to make chipchooser and stuff.. " << endl;
  // something for choosing chips for the query rather than always defaulting to all the chips..
  ChipChooser* chipChooser = new ChipChooser(client->chipData, this, "chipChooser");
  connect(client->chipData, SIGNAL(toggleButtons(map<int, bool>)), chipChooser, SLOT(toggleButtons(map<int, bool>)) );
  connect(client->chipData, SIGNAL(newChipInfo(map<int, bool>)), chipChooser, SLOT(newChipInfo(map<int, bool>)) );
  // and call a client function to get the chipInfo.. hmm. 
  //client->getChipInformation();
  // can't do the above, as we don't know if the connection has been made. On the other hand, the client doesn't know if 
  // all the stuff has been set up to handle the messages. so we have a bad situation. 

  //// --- I should also be connecting the signals asking for new regions and this stuff, but that is for later.. as we don't have the appropriate support
  ///      in the client yet.. 
  ////  -- as we don't have a button yet..
  //  thumbnailViewer->show();    // OK, add button later or something.. 

  //QPushButton* showAnnotator = new QPushButton("Annotator", this, "annotator");
  //connect(showAnnotator, SIGNAL(clicked()), annotator, SLOT(show()) );

  //expTable = new ExptTable();

  // buttons, loads of them..
  // first some toggle type buttons for the left side, 
  QPushButton* rawData = new QPushButton("Raw Data", this, "raw");
  rawData->setToggleButton(true);
  rawData->setOn(false);
  connect(rawData, SIGNAL(toggled(bool)), this, SLOT(toggleExpressionWindow(bool)));

  QPushButton* meanData = new QPushButton("Mean Plot", this, "meanData");
  meanData->setToggleButton(true);
  meanData->setOn(false);
  connect(meanData, SIGNAL(toggled(bool)), this, SLOT(toggleMeanPlotWindow(bool)));

  QPushButton* probeData = new QPushButton("Probe Data", this, "probeData");
  probeData->setToggleButton(true);
  probeData->setOn(false);
  connect(probeData, SIGNAL(toggled(bool)), this, SLOT(toggleDataWindow(bool)) );


  QPushButton* experimentChoices= new QPushButton("Expt. Choice", this, "exptchoice");
  experimentChoices->setToggleButton(true);
  experimentChoices->setOn(false);              // connect it later on to something.. 
  connect(experimentChoices, SIGNAL(toggled(bool)), this, SLOT(toggleExperimentWindow(bool)) );

  QPushButton* comparisons = new QPushButton("Profile", this, "comparisons");
  comparisons->setToggleButton(true);
  comparisons->setOn(false);
  connect(comparisons, SIGNAL(toggled(bool)), this, SLOT(toggleComparisonWindow(bool)) );
  
  QPushButton* showHistory = new QPushButton("History", this, "history");
  showHistory->setToggleButton(true);
  showHistory->setOn(false);
  connect(showHistory, SIGNAL(toggled(bool)), this, SLOT(toggleHistoryWindow(bool)) );

  QPushButton* showSaved = new QPushButton("Saved List", this, "showSaved");
  showSaved->setToggleButton(true);
  showSaved->setOn(false);
  connect(showSaved, SIGNAL(toggled(bool)), this, SLOT(toggleSavedWindow(bool)));

  // And some buttons for doing some simple statistical stuff for the right side.
  QPushButton* compare = new QPushButton("Compare", this, "compare");
  connect(compare, SIGNAL(clicked()), this, SLOT(doEuclidSort()) );    // need local slot to check the current index. 

  QPushButton* anovaSort = new QPushButton("&Anova Sort", this, "anovaSort");
  connect(anovaSort, SIGNAL(clicked()), client, SLOT(doAnovaSort()));
  
  QPushButton* anovaSelectSort = new QPushButton("Anova (selected)", this, "anovaSelectedSort");
  connect(anovaSelectSort, SIGNAL(clicked()), client, SLOT(doSelectAnovaSort()) );
  
  QPushButton* statCollection = new QPushButton("Stat Collection", this, "statCollection");
  connect(statCollection, SIGNAL(clicked()), client, SLOT(getStatCollection()) );

  QPushButton* writeHtmlRep = new QPushButton("Write History", this, "writeHtml");
  connect(writeHtmlRep, SIGNAL(clicked()), this, SLOT(writeHtmlReport()) );

  QPushButton* saveState = new QPushButton("Save State", this, "saveState");
  connect(saveState, SIGNAL(clicked()), this, SLOT(saveCurrentState()));

  QPushButton* readSavedState = new QPushButton("Read State", this, "readSavedState");
  connect(readSavedState, SIGNAL(clicked()), this, SLOT(readState()) );

  QPushButton* fontButton = new QPushButton("Font", this, "fontButton");
  connect(fontButton, SIGNAL(clicked()), this, SLOT(selectFont()) );

  ////////// Try making a comparison window..
  //  comparisonWindow = new ComparisonWindow(25);
  comparisonWindow = new ComparisonWindow();
  // this isn't real, this is just so that we have some sort of window in case the button gets pressed.
  // ugly hack if I ever saw one.. 


  /// Play around with listWindow, and see when we crahs..
  listWindow = new ListWindow();
  listWindow->setCaption("History");
  savedListWindow = new ListWindow();
  savedListWindow->setCaption("Memory");

  QPushButton* forgetList = new QPushButton("Clear All", savedListWindow, "clearbutton");
  connect(forgetList, SIGNAL(clicked()), this, SLOT(clearSavedOnes()));
  //  forgetList->setText("Clear All");
  savedListWindow->addButton(forgetList);


  // a button for comparing experiments..

  exptComparisons = new CompareController();
  exptComparisons->setPalette(palette());
  connect(client, SIGNAL(newExperimentalDistances(exptDistanceInfo)), this, SLOT(newExptDistances(exptDistanceInfo)) );
  connect(client, SIGNAL(newExperimentTrace(vector<tracePoint>)), exptComparisons, SLOT(newTrace(vector<tracePoint>)) );
  connect(exptComparisons, SIGNAL(setCoordinates(vector<PointCoordinate>)), this, SLOT(setCoordinates(vector<PointCoordinate>)) );

  QPushButton* compareExperiments = new QPushButton("Compare Expts", this, "compareExperiments");
  connect(compareExperiments, SIGNAL(clicked()), exptComparisons, SLOT(show()) );
  connect(exptComparisons, SIGNAL(doFullCompare()), client, SLOT(compareExperiments()) );
  connect(exptComparisons, SIGNAL(doFlatCompare(float, float)), client, SLOT(flatCompareExperiments(float, float)) );
  connect(exptComparisons, SIGNAL(compareCells(vector<int>, vector<int>)), client, SLOT(compareCells(vector<int>, vector<int>)) );
  connect(exptComparisons, SIGNAL(traceExperiments(float)), client, SLOT(traceExperiments(float)) );

  /////////  Protocol Related Stuff... -- I may move this later on to other classes, but now to begin to test the things..
  QPushButton* getProtocolsButton = new QPushButton("Protocols", this, "getProtocolButton");

  /////////// REMOVE LATER REMOVE,, JUST TEMPORARY.. HONEST TO ... 
  //ProtocolHolder* holder = new ProtocolHolder(client->userId);
  //holder->show();
  
  protManager = new ProtocolManager(&client->userId, &client->userInfo);
  connect(getProtocolsButton, SIGNAL(clicked()), protManager, SLOT(show()) );
  protManager->setPalette(palette());
  connect(client, SIGNAL(newProtocolCollection(int, int, ProtocolCollection*)), 
	  this, SLOT(receiveProtocolCollection(int, int, ProtocolCollection*)) );
  connect(client, SIGNAL(newProtocol(Protocol*)), protManager, SLOT(addProtocol(Protocol*)) );
  connect(protManager, SIGNAL(getCollection(int)), this, SLOT(requestProtocolCollection(int)) );

  connect(protManager, SIGNAL(commitProtocolToDB(Protocol*)), client, SLOT(commitProtocolToDB(Protocol*)) );
  connect(protManager, SIGNAL(getProtocol(int)), client, SLOT(getProtocol(int)) );
  protManager->resize(600, 400);
  //protManager->show();

  exportDialog = new ExportMeanDialog();
  connect(exportDialog, SIGNAL(getMeans(QString, int)), client, SLOT(exportMeans(QString, int)) );
  //exportDialog->resize(300, 150);
  //ProtocolStepWidget* stepWidget = new ProtocolStepWidget();
  //stepWidget->show();
  //ProtocolViewer* protViewer = new ProtocolViewer(client->userId);
  //protViewer->show();

  //////////////////////////////////


  /// add an accelerator class for things.. 
  QAccel* accel = new QAccel(this);
  connect(accel, SIGNAL(activated(int)), this, SLOT(shiftFocus(int)) );
  accel->insertItem(ALT + Key_R, 100);         // R for range,, set focus to the range control
  accel->insertItem(ALT + Key_T, 105);          // T  set focus to thickness control !! 
  accel->insertItem(ALT + Key_U, 110);          // U for user annotation,, getting desparate here..  !! 
  accel->insertItem(ALT + Key_D, 115);          // d for database lookup !! 

  


#ifdef EXPERIMENTAL_CLIENT
  //////////////////////////////// A testButton, remove for real program... should have some ifdefs here or something..
  /// use it for the write Index function.. 
  QPushButton* testButton = new QPushButton("Test Blast", this, "testButton");
  connect(testButton, SIGNAL(clicked()), client, SLOT(testSlot()) );
#endif

  // and another    ---> if I want this, then change the button .. 
#ifdef EXPERIMENTAL_CLIENT
  QPushButton* test2 = new QPushButton("test", this, "test2");
  connect(test2, SIGNAL(clicked()), client, SLOT(getDevsFromMean()) );
#endif

  QPushButton* writeIndexButton = new QPushButton("Write Index", this, "writeIndexButton");
  connect(writeIndexButton, SIGNAL(clicked()), this, SLOT(writeCurrentIndex()) );

  QPushButton* exportMeanButton = new QPushButton("Export Means", this, "exportMeanButton");
  connect(exportMeanButton, SIGNAL(clicked()), this, SLOT(exportMeans()) );

  ////////////////////////// IGNORE INDEX,, ugly hack if I ever saw one..
  ignoreIndex = 0;   // but what does it do?? 


  #ifdef EXPERIMENTAL_CLIENT
  /////////////////////////// READ COMMAND,, FOR KILLING THE SERVER /// 
  QPushButton* readCommandButton = new QPushButton("Read Commands", this, "readCommands");
  connect(readCommandButton, SIGNAL(clicked()), this, SLOT(readCommands()) );

  QPushButton* abuser = new QPushButton("Abuse Server", this, "abuseServer");
  connect(abuser, SIGNAL(clicked()), this, SLOT(abuseTheServer()) );

  //////////////////////////////////////////////////////////////////////////////////
  #endif

  QVBoxLayout* vbox = new QVBoxLayout(this);
  QHBoxLayout* spinners = new QHBoxLayout(1);
  QHBoxLayout* buttonAreas = new QHBoxLayout();
  QVBoxLayout* leftButtons = new QVBoxLayout();
  QVBoxLayout* rightButtons = new QVBoxLayout();
  
  vbox->addLayout(buttonAreas);
  buttonAreas->addLayout(leftButtons);
  buttonAreas->addLayout(rightButtons);
  leftButtons->addWidget(rawData);
  leftButtons->addWidget(meanData);
  leftButtons->addWidget(probeData);
  leftButtons->addWidget(experimentChoices);
  leftButtons->addWidget(comparisons);
  leftButtons->addWidget(clusterButton);
  leftButtons->addWidget(showHistory);
  leftButtons->addWidget(indexHistoryButton);
  leftButtons->addWidget(showSaved);
  leftButtons->addWidget(showStats);
  //leftButtons->addWidget(showAnnotator);
  leftButtons->addWidget(regionButton);
  leftButtons->addStretch();
  leftButtons->addWidget(passwdShow);
  leftButtons->addWidget(getProtocolsButton);
#ifdef EXPERIMENTAL_CLIENT
  leftButtons->addWidget(testButton);
  leftButtons->addWidget(test2);
#endif

  rightButtons->addWidget(compare);
  rightButtons->addWidget(anovaSort);
  rightButtons->addWidget(anovaSelectSort);
  rightButtons->addWidget(statCollection);
  rightButtons->addWidget(compareExperiments);

  rightButtons->addStretch();
  rightButtons->addWidget(saveState);
  rightButtons->addWidget(readSavedState);
  rightButtons->addWidget(writeHtmlRep);
  ///////////////////////////////////////////////////////////////////////////////////////
  /////////////////////  TEST BUTTON !!! 
  rightButtons->addWidget(writeIndexButton);
  rightButtons->addWidget(exportMeanButton);
  rightButtons->addWidget(fontButton);
#ifdef EXPERIMENTAL_CLIENT
  rightButtons->addWidget(readCommandButton);
  rightButtons->addWidget(abuser);
#endif

  vbox->addWidget(annotator);
  //vbox->addWidget(messages);
  //vbox->addWidget(indexSize);
  vbox->addLayout(spinners);
  
  spinners->addWidget(lineWidth);
  spinners->addWidget(getRegion);
  spinners->addWidget(getProbe);
  vbox->addWidget(dbQuerier);
  vbox->addWidget(chipChooser);

//    vbox->addWidget(reConnect);

//    QGridLayout* bGrid = new QGridLayout(this, 1, 5, 2, 1);

//    QVBoxLayout* lb1 = new QVBoxLayout(0);
//    QVBoxLayout* lb2 = new QVBoxLayout(0);
//    QVBoxLayout* an = new QVBoxLayout(0);
//    QVBoxLayout* sb = new QVBoxLayout(0);
//    QVBoxLayout* db = new QVBoxLayout(0);

//    bGrid->addLayout(lb1, 0, 0);
//    bGrid->addLayout(lb2, 0, 1);
//    bGrid->addLayout(an, 0, 2);
//    bGrid->addLayout(sb, 0, 3);
//    bGrid->addLayout(db, 0, 4);
  
//    // and lets add the buttons to lb1, -- window buttons 1,,
//    lb1->addWidget(rawData);
//    lb1->addWidget(meanData);
//    lb1->addWidget(probeData);
//    lb1->addWidget(comparisons);
//    lb1->addWidget(experimentChoices);
//    lb1->addStretch();

//    lb2->addWidget(showHistory);
//    lb2->addWidget(showSaved);
//    lb2->addWidget(showStats);
//    lb2->addStretch();
//    lb2->addWidget(passwdShow);

//    an->addWidget(annotator);
//    //  an->addWidget(messages);
  
//    sb->addWidget(anovaSort);
//    sb->addWidget(compare);
//    sb->addWidget(statCollection);
//    sb->addStretch();
//    sb->addWidget(saveState);
//    sb->addWidget(readSavedState);
//    sb->addWidget(writeHtmlRep);

//    // and now for the difficult bits in db.. the databas thing.. 
//    QHBoxLayout* db_1 = new QHBoxLayout();
//    //QHBoxLayout* db_2 = new QHBoxLayout();
//    db->addWidget(dbQuerier);
//    db->addWidget(indexSize);
//    db->addLayout(db_1);
//    db_1->addWidget(lineWidth);
//    db_1->addWidget(getProbe);
//    db->addStretch();  // stretch at the top.. 
//    db->addWidget(messages);
  
  cout << "at the end of the clientwindow construction" << endl;

  //  comparisonWindow->setPalette(QPalette(QColor(110, 197, 209), QColor(184, 175, 198)));
  comparisonWindow->setPalette(palette());
  dataView->setPalette(palette());
  listWindow->setPalette(palette());
  savedListWindow->setPalette(palette());
  experimentChooser->setPalette(palette());
  annotator->setPalette(palette());
  //  resize(275, 600);
  cout << "Finished the clientWindow construction " << endl;
}


ClientWindow::~ClientWindow(){
  cout << "ClientWindow: deleting itself.. " << endl;
  client->close();
  delete statwindow;
  //delete client;
}

void ClientWindow::writeHistory(){
  //cout << "writeHistory this is where things get sorted out.. " << endl;
  if(client->pSet.index == client->pData.index){
    //// OK, now if we want to write a report with this one, then don't do any of the below, just call some function or 
    //// other and return... 
    if(reportRequests.size()){
      for(set<writeRequest*>::iterator it=reportRequests.begin(); it != reportRequests.end(); it++){
	if((*it)->indicesToWrite.count(client->pSet.index)){
	  //cout << "and this is where we would be calling the writing thingy.. and so on" << endl;
	  ignoreIndex = client->pSet.index;     // so we don't write this one .. 
	  writeCurrentProbeSet(*it);
	  // call some function that writes the thingy. This function will need to know which probeSetIndex to use though I suppose it 
	  // can just assume that it will be using the current thingy.. 
	  // do something
	  return;
	}
      }
    }
    currentProbeSetId = client->pSet.index;
    if(history.size() < historySize){
      history.push_front(new ProbeSetWidget(client->pSet, client->pData));
      history[0]->setPalette(palette());
      connect(history[0], SIGNAL(plotMyMean(ProbeSetWidget*, bool)), meanPlotWindow, SLOT(toggleProbeSet(ProbeSetWidget*, bool)));
      listWindow->addItem(history[0]);
      connect(history[0], SIGNAL(plotMe(probe_set*, probe_data*)), this, SLOT(newSet(probe_set*, probe_data*)));
      connect(history[0], SIGNAL(pleaseRememberMe(ProbeSetWidget*)), this, SLOT(saveProbeSetWidget(ProbeSetWidget*)) );
    }else{
      ////// use the setProbe function to move everything back..
      int i= history.size();
      meanPlotWindow->dontUpdate(true);
      while(i > 1){
	i--;
	history[i]->setProbeSet(history[i-1]->mySet, history[i-1]->myData, history[i-1]->meanPlotOn());
      }
      history[0]->setProbeSet(client->pSet, client->pData, false);
      meanPlotWindow->dontUpdate(false);
      meanPlotWindow->collectData();   // check what the data is and plot it.. 
    }
    // if the index of the last saved one is present in the set indicesToSave, then save the index, and remove it from 
    // indices to save..
    if(indicesToSave.count(history[0]->mySet.index)){
      saveProbeSetWidget(history[0]);
      indicesToSave.erase(history[0]->mySet.index);
    }
  }

//      if(history.size() > historySize){
//        history[history.size()-1]->hide();   // maybe its neater..
//        ProbeSetWidget* toBeDestroyed = history[history.size()-1];
//        history.pop_back();
//        delete toBeDestroyed;
//      }
//    }
  //cout << "\t\t DID Loads of things with ProbeSetWidgets, how is the memory???" << endl;
  //cout << "\t\t History size: " << history.size() << endl;
}

void ClientWindow::pleaseForgetMe(ProbeSetWidget* psw){
  list<ProbeSetWidget*>::iterator lit;
  for(lit = savedOnes.begin(); lit != savedOnes.end(); lit++){
    if((*lit) == psw){
      savedOnes.erase(lit);
      break;
    }
  }
  psw->hide();
  // remember to remove it from the set of savedOnes so that I can save it again..
  savedIndices.erase(psw->mySet.index);
  delete psw;    // how does the parent feel about this I wonder,, ??
}

void ClientWindow::saveProbeSetWidget(ProbeSetWidget* psw){
  cout << "ClientWindow::saveProbeSetWidget" << endl;
  /// I should get myself a set or a list of the saved ones, because at the moment, I don't know exactly 
  /// what will happen but there you go..
  // first check if its already been saved,, 
  if(savedIndices.count(psw->mySet.index)){
    return;
  }
  ProbeSetWidget* copy = new ProbeSetWidget(psw->mySet, psw->myData);   // top level for now..
  savedOnes.push_front(copy);             // and there we go, Now I just need a function to plot and write out everything in a directory.. !
  copy->save->setText("delete");
  // and insert the index into savedOnes,,
  savedIndices.insert(psw->mySet.index);    // and that should be it.. 
  // make the connections, -except for save, which doesn't make any sense for this one..
  connect(copy, SIGNAL(plotMyMean(ProbeSetWidget*, bool)), meanPlotWindow, SLOT(toggleProbeSet(ProbeSetWidget*, bool)));
  connect(copy, SIGNAL(plotMe(probe_set*, probe_data*)), this, SLOT(newSet(probe_set*, probe_data*)));
  connect(copy, SIGNAL(pleaseRememberMe(ProbeSetWidget*)), this, SLOT(pleaseForgetMe(ProbeSetWidget*)) );
  savedListWindow->addItem(copy);
}


void ClientWindow::changeExperimentInformation(){
  cout << "ClientWindow::changeExperimentInformation" << endl;
  dataView->setExperiments(client->expInfo);
  experimentChooser->updateExperiments(client->expInfo);
  comparisonWindow->hide();
  delete comparisonWindow;
  comparisonWindow = new ComparisonWindow(&client->expInfo);
  connect(client->chipData, SIGNAL(equivalentStates(map<int, bool>)), comparisonWindow, SLOT(setActive(map<int, bool>)) );
  connect(experimentChooser, SIGNAL(newExptSelection(vector<int>)), comparisonWindow, SLOT(setActive(vector<int>)) );

  //  comparisonWindow = new ComparisonWindow(client->expInfo.size());
  comparisonWindow->setPalette(palette());
  connect(comparisonWindow, SIGNAL(doRawComparison(vector<float>, vector<int>, bool)), client, SLOT(doRawComparison(vector<float>, vector<int>, bool)) );
  //  connect(comparisonWindow, SIGNAL(doDiff(int, int, bool)), client, SLOT(doBinaryComparison(int, int, bool)) );
  connect(comparisonWindow, SIGNAL(doMeanComparison(vector<float>, vector<int>, bool, bool)), client, SLOT(doMeanComparison(vector<float>, vector<int>, bool, bool)) );
}


void ClientWindow::writeMessage(QString message){
  cout << "ClientWindow::writeMessage" << endl;
  messages->append(message);
  messages->show();     // make top level window.. 
}

void ClientWindow::getProbeSet(int number){
  cout << "ClientWindow::getProbeSet" << endl;
  client->getProbeSet(number);
}


void ClientWindow::setRegionRange(int n){
  int max = n-1;
  if(!n){ max = 0; }
  getRegion->setRange(0, max);
  getRegion->setValue(0);
  client->getRegion(0);
}

void ClientWindow::doEuclidSort(){
  cout << "ClientWindow::doEuclidSort" << endl;
  client->doEuclidSort(getProbe->value());
}


void ClientWindow::plotValues(probe_set pSet){
  cout << "ClientWindow:: calling plotWindow to plot " << endl;
  if(reportRequests.size()){
    for(set<writeRequest*>::iterator it=reportRequests.begin(); it != reportRequests.end(); it++){
      if((*it)->indicesToWrite.count(pSet.index)){
	// don't plot it, just ignore, try to catch when we get the probe set data.. !!
	return;
      }
    }
  }
  plotWindow->plot(&pSet);
}

void ClientWindow::newProbeData(probe_data* pd){
  cout << "clientWindow newProbeData thingy  pd index is " << pd->index << endl;
  if(ignoreIndex == pd->index){
    ignoreIndex = 0;
    return;
  }
  if(reportRequests.size()){
    for(set<writeRequest*>::iterator it=reportRequests.begin(); it != reportRequests.end(); it){
      if((*it)->indicesToWrite.count(pd->index)){
	return;
      }
    }
  }
  dataView->setText(pd);
}

void ClientWindow::newSet(probe_set* p, probe_data* d){
  plotWindow->plot(p);
  dataView->setText(d);
  currentProbeSetId = p->index;
}

void ClientWindow::newAnnotation(int state, int sessionIndex, QString note){
  //cout << "Adding annotation " << endl
  //    << "\tcurrent session : " << sessionIndex << endl
  //    << "\tcurrent gene    : " << currentProbeSetId << endl
  //    << "\tnote state      : " << state << endl
  //    << "note              : " << note << endl;
  // do a switch on the state, and call a function in the client.. !. 
  vector<int> genes(0);
  set<int>::iterator it;
  switch(state){
  case 0:
    // 0 means no association with anything. so in fact we don't do anything
    break;
  case 1:
    // 1 current gene..
    genes.push_back(currentProbeSetId);
    break;
  case 2:
    // Saved Gene set.. 
    for(it=savedIndices.begin(); it != savedIndices.end(); it++){
      genes.push_back(*it);
    }
    break;
  case 3:
    // current association only. Should not be 0, but do error checking on the server side.
    break;
  default :
    cout << "Unknown annotation state, This won't do anything.. " << endl;
  }
  client->newAnnotation(sessionIndex, genes, note);   // better make this function.. !!
  
}

void ClientWindow::changeIndexRange(int n){
  // first set the indexSize.. 
//   QString sizeString;
//   sizeString.setNum(n);
//   sizeString.prepend("Current Index size:\t");
//   indexSize->setText(sizeString);
//   cout << "ClientWindow::changeIndexRange" << endl;
  //  cout << "chaning the range of getProbe to 0 -> " << n-1 << endl;
  n--;                   // the size of the currentIndex is emitted. As we count from 0, this is one more than the maximum value..
  getProbe->setMaxValue(n);
  if(getProbe->value() == 0){
    getProbeSet(0);
  }else{
    getProbe->setValue(0);     // which will send the valueChanged signal which will automatically get the probe set anyway.
  }
}


void ClientWindow::toggleExperimentWindow(bool vis){
  cout << "ClientWindow::toggleExperimentWindow" << endl;
  if(vis){
    experimentChooser->show();
    return;
  }
  experimentChooser->hide();
}


void ClientWindow::toggleDataWindow(bool vis){
  cout << "ClientWindow::toggleDataWindow" << endl;
  if(vis){
    dataView->show();
    return;
  }
  dataView->hide();
}

void ClientWindow::toggleExpressionWindow(bool vis){
  cout << "ClientWindow::toggleExpressionWindow" << endl;
  if(vis){
    plotWindow->show();
    return;
  }
  plotWindow->hide();
}

void ClientWindow::toggleComparisonWindow(bool vis){
  cout << "ClientWindow::toggleComparisonWindow" << endl;
  if(vis){
    comparisonWindow->show();
    return;
  }
  comparisonWindow->hide();
}

void ClientWindow::toggleHistoryWindow(bool vis){
  cout << "ClientWindow::toggleHistoryWindow" << endl;
  if(vis){
    listWindow->show();
    return;
  }
  listWindow->hide();
}

void ClientWindow::toggleMeanPlotWindow(bool vis){
  cout << "ClientWindow::toggleMeanPlotWindow" << endl;
  if(vis){
    meanPlotWindow->show();
    return;
  }
  meanPlotWindow->hide();
}

void ClientWindow::toggleSavedWindow(bool vis){
  cout << "ClientWindow::toggleSavedWindow" << endl;
  if(vis){
    savedListWindow->show();
    return;
  }
  savedListWindow->hide();
}

void ClientWindow::newExptSelection(vector<int> v){
  //cout << "ClientWindow::newExptSelection" << endl;
  client->selectedExperiments = v;
  plotWindow->replot();
  meanPlotWindow->collectData();
  // and tell all the clients that need to know that 
  // the exptSelection has changed..
}

void ClientWindow::newMarkSelection(vector<int> v){
  markedExperiments = v;
  plotWindow->replot();
  meanPlotWindow->collectData();
}


void ClientWindow::writeHtmlReport(){
  cout << "ClientWindow::writeHtmlReport" << endl;
  // creates a directory with images of the plots, along with an html file
  // with the appropriate stuff in it. Html is good, then I don't have to worry about 
  // things like formatting... hmmmm... 
  QDir d;       // should be the present directory,, 
  // make a new one as long as it doesn't exist
  bool dirMade = false;
  //QString dirName;    // need for later application..
  QString dirName;
  while(!dirMade){
    dirName = QFileDialog::getSaveFileName();
    cout << "dirName is " << dirName << endl;
    if(dirName.isNull()){
      return;
    }
    dirMade = d.mkdir(dirName);
  }
//   while(1){
//     dirName.setNum(counter);
//     dirName.prepend("temp");
//     if(!d.exists(dirName)){
//       // create the directory.. 
//       if(d.mkdir(dirName)){
// 	cout << "created the directory" << endl;
//       }else{
// 	cout << "couldn't make the directory, oh bugger" << endl;
//       }
//       break;
//     }
//     cout << "directory already exists: " << dirName << endl;
//     counter++;
//   }
  // ugly, but what the hell what can you do.. could probably just do something else
  // lets make a pixmap and paint to it, then save it. Just try a tester function first
  vector<QString> fileNames(0); 
  vector<QString> rawFileNames(0);
  list<ProbeSetWidget*>::iterator lit;
  for(lit = savedOnes.begin(); lit != savedOnes.end(); lit++){
    //  for(int i=0; i < savedOnes.size(); i++){
    //if(savedOnes.size() > 0){
    QPixmap pm(1200, 300);
    QPixmap pmRaw(1200, 300);
    pm.fill();
    pmRaw.fill();
    QPainter p(&pm);
    QPainter p2(&pmRaw);
    plotWindow->plot(&(*lit)->mySet, &p, false);
    plotWindow->plot(&(*lit)->mySet, &p2, true);
    //    plotWindow->plot(&savedOnes[i]->mySet, &p, false);
    p.end();
    QString fileName((*lit)->myData.afid.c_str());
    fileName.append(".jpg");
    fileNames.push_back(fileName);        // hmmmm.. 
    fileName.prepend("/");
    fileName.prepend(dirName);
    //    QString fileName = dirName;
    //fileName.append("/");
    //fileName.append(
    //    fileName.append("/test.jpg");
    //cout << "tring to write file : " << fileName << endl; 
    pm.save(fileName, "JPEG", 100);
    // then do the normalised one.. 
    fileName = (*lit)->myData.afid.c_str();
    fileName.append("_raw.jpg");
    // and then make the raw pixmap.
    //cout << "just before calling pm.fill " << endl;
    //    pm.fill();
    //plotWindow->plot(&(*lit)->mySet, &p, true);  // I think.. 
    //cout << "after calling plotwindow to plot on my thingy.. " << endl;
    rawFileNames.push_back(fileName);
    fileName.prepend("/");
    fileName.prepend(dirName);
    //cout << "just before calling save for the raw data " << endl;
    pmRaw.save(fileName, "JPEG", 100);
  }
  QString htmlFile = dirName;
  htmlFile.append("/index.html");
  ofstream out(htmlFile.latin1());  // open a file for writing to and write the HTML header.. 
  out << "<html><head><meta HTTP-EQUIV=\"content-type\" CONTENT=\"text/html; charset=ISO-8859-1\"><title>Your Gene Selection</title></head>" << endl;     // makes for neater html.. 
  out << "<body>" << endl << "<h1 align=\"Center\">Your Gene Selection</h1><br><br>" << endl;  
  // we should go through the experimental information or something here.. Like in the thingy.. 
  
  out << "<br><table width=\"90%\" cellpadding=\"2\" cellspacing=\"2\" border=\"2\"><tr><td colspan=3><font align=center><h2>Experiments</h2></td></tr>" << endl;
  out << "<tr><td></td><td><font size=+1>Short</td><td align=center><font size=+1>Long Description</td></font></tr>" << endl;
  map<float, exptInfo>::iterator it;
  for(it = client->expInfo.begin(); it != client->expInfo.end(); it++){
    out << "<tr><td>" << (*it).second.dbaseIndex << "</td><td>" << (*it).second.shortName << "</td><td>" << (*it).second.description << "</td></tr>" << endl;
  }
  out << "</table><br>" << endl;

  int i=0;
  for(lit = savedOnes.begin(); lit != savedOnes.end(); lit++){
    //  for(int i=0; i < fileNames.size(); i++){
    out << "<table cellpadding=\"2\" cellspacing=\"2\" border=\"2\" width=\"90%\">" << endl;
    out << "<tr><td colspan=3><img src=\"" << fileNames[i] << "\" alt=\"Expression Man!\"></td></tr>" << endl;
    out << "<tr><td colspan=3><img src=\"" << rawFileNames[i] << "\" alt=\"Expression Man!\"></td></tr>" << endl;
    out << "<tr><td colspan=3 align=center>Affymetrix Annotation</td></tr>" << endl;
    out << "<tr><td colspan=1>Affy Id</td><td colspan=2>" << (*lit)->myData.afid << "</td></tr>" << endl;
    out << "<tr><td colspan=1>Description</td><td colspan=2>" << (*lit)->myData.afdes << "</td></tr>" << endl;
    out << "<tr><td colspan=1>TIGR</td><td colspan=2>" << (*lit)->myData.tigrDescription << "</td></tr>" << endl; 
    out << "<tr><td colspan=3 align=center>UniGene Matches</td></tr>" << endl;
    out << "<tr><td colspan=1>Gene</td><td colspan=1>Title</td><td colspan=1>Index</td></tr>" << endl;
    for(int j=0; j < (*lit)->myData.ugData.size(); j++){
      out << "<tr><td colspan=1>" << (*lit)->myData.ugData[j].gene << "</td>"
	  << "<td colspan=1>" << (*lit)->myData.ugData[j].title << "</td>"
	  << "<td colspan=1>" << (*lit)->myData.ugData[j].index << "</td>" << endl;
    }
    out << "</table><br>" << endl;
    i++;
    //    cout << "filenames i " << fileNames[i] << endl;
  }
  out << "</html></body>" << endl;
}

void ClientWindow::writeCurrentIndex(){
  // generate a writeRequest with a load of indices in it, and an index file containing regular html..
  // no worries mate, it's alright..
  
  // first let's get ourselves a set of indices..
  // but let's not do this if the size of the indices is more than a 1000..
  if(client->indexSize() > 1000){
    cout << "Don't be bloody stupid, if you really want to do this, have a word with your friendly BOFH" << endl;
    return;
  }
  set<int> nums;
  vector<int> currentIndex = client->curIndex();
  for(int i=0; i < currentIndex.size(); i++){ nums.insert(currentIndex[i]); }
  // and then let's see if we can make a directory, just do as before..
  QDir d;
  bool dirMade = false;
  //QString dirName;    // need for later application..
  QString dirName;
  while(!dirMade){
    dirName = QFileDialog::getSaveFileName();
    cout << "dirName is " << dirName << endl;
    if(dirName.isNull()){
      return;
    }
    dirMade = d.mkdir(dirName);
  }
  ///////////////  and again, I'm doing ugly conversions between strings and qstrings.. ayaah.
  QString htmlFile = dirName;
  htmlFile.append("/index.html");
  ofstream* out = new ofstream(htmlFile.latin1());   // and check the file is OK..
  if(!out){
    cerr << "couldn't open that file name bybybeee" << endl;
    return;
  }
  *out << "<html><head><meta HTTP-EQUIV=\"content-type\" CONTENT=\"text/html; charset=ISO-8859-1\"><title>Your Gene Selection</title></head>" << endl;     // makes for neater html.. 
  *out << "<body>" << endl << "<h1 align=\"Center\">Your Gene Selection</h1><br><br>" << endl;  
  // we should go through the experimental information or something here.. Like in the thingy.. 
  *out << "<br><table width=\"90%\" cellpadding=\"2\" cellspacing=\"2\" border=\"2\"><tr><td colspan=3><font align=center><h2>Experiments</h2></td></tr>" << endl;
  *out << "<tr><td></td><td><font size=+1>Short</td><td align=center><font size=+1>Long Description</td></font></tr>" << endl;
  map<float, exptInfo>::iterator it;
  for(it = client->expInfo.begin(); it != client->expInfo.end(); it++){
    *out << "<tr><td>" << (*it).second.dbaseIndex << "</td><td>" << (*it).second.shortName << "</td><td>" << (*it).second.description << "</td></tr>" << endl;
  }
  *out << "</table><br>" << endl;
  /// and then we create the new writeRequest, stick into the set of things and go through the current index looking for stuff, and hope that 
  /// everything works out just fine..
  writeRequest* request = new writeRequest(nums, dirName.latin1(), out);
  reportRequests.insert(request);
  // and then...
  for(int i=0; i < client->indexSize(); i++){
    getProbeSet(i);             // a bit funny and a bit arbitrary, and ofcourse depends on the clientIndex still being the same..
  }
  // this might really cause a bit of a memory problem if currentIndex is large, and we go through everything.. oh well, never mind..
  // we just leave it here, and catch and clean up in another function somewhere.. cheers and goodnight..
}

void ClientWindow::writeCurrentProbeSet(writeRequest* wr){
  // first make sure that the current index thing is in the wr->
  if(!wr->indicesToWrite.count(client->pSet.index)){
    cerr << "not in indices to Write, that's not good, do something else... " << endl;
    return;
  }
  // html output section. Determines size of plots amongst other things.. 
  QPixmap pm(1200, 300);
  QPixmap pmRaw(1200, 300);
  pm.fill();
  pmRaw.fill();
  QPainter p(&pm);
  QPainter p2(&pmRaw);
  cout << "write Current probe set, and pixmap is large .. " << endl;
  plotWindow->plot(&client->pSet, &p, false);
  plotWindow->plot(&client->pSet, &p2, true);
  QString f1(client->pData.afid.c_str());
  f1.append(".jpg");
  QString f2 = f1;
  QString f1Name = f1;
  QString f2Name = f2;
  f2Name.prepend("raw_");
  f2.prepend("/raw_");
  f1.prepend("/");
  f1.prepend(wr->dirName.c_str());   // ugly as hell, but there you go..
  f2.prepend(wr->dirName.c_str());   // ugly as hell, but there you go..
  // ok let's make the files...
  cout << "trying to save file something or other " << f1 <<  "   and " << f2 << endl;
  pm.save(f1, "JPEG", 100);
  pmRaw.save(f2, "JPEG", 100);
  // then let's write to the out in wr..
  *(wr->out) << "<table cellpadding=\"2\" cellspacing=\"2\" border=\"2\" width=\"90%\">" << endl
	     << "<tr><td colspan=3><img src=\"" << f1Name.latin1() << "\" alt=\"Expression Man!\"></td></tr>" << endl
	     << "<tr><td colspan=3><img src=\"" << f2Name.latin1() << "\" alt=\"Expression Man!\"></td></tr>" << endl
	     << "<tr><td colspan=3 align=center>Affymetrix Annotation</td></tr>" << endl
	     << "<tr><td colspan=1>Affy Id</td><td colspan=2>" << client->pData.afid << "</td></tr>" << endl
	     << "<tr><td colspan=1>Description</td><td colspan=2>" << client->pData.afdes << "</td></tr>" << endl
	     << "<tr><td colspan=1>TIGR</td><td colspan=2>" << client->pData.tigrDescription << "</td></tr>" << endl
	     << "<tr><td colspan=3 align=center>UniGene Matches</td></tr>" << endl
	     << "<tr><td colspan=1>Gene</td><td colspan=1>Title</td><td colspan=1>Index</td></tr>" << endl;
  for(int j=0; j < client->pData.ugData.size(); j++){
    *(wr->out) << "<tr><td colspan=1>" << client->pData.ugData[j].gene << "</td>"
	       << "<td colspan=1>" << client->pData.ugData[j].title << "</td>"
	       << "<td colspan=1>" << client->pData.ugData[j].index << "</td>" << endl;
  }
  *(wr->out) << "</table><br>" << endl;
  //    cout << "filenames i " << fileNames[i] << endl;
  /// and now importantly..
  wr->indicesToWrite.erase(client->pSet.index);
  // and if size is now 0,, remove wr from the thingy and so on.. this will probably kill the thingy..
  cout << "writing probe set to thingy.. size of wr is " << wr->indicesToWrite.size() << endl;
  if(!wr->indicesToWrite.size()){
    *(wr->out) << "</html></body>" << endl;
    reportRequests.erase(wr);
    delete wr;      // which should close the out thing and leave us OK.. 
  }
}

void ClientWindow::exportMeans(){
    // show the exportDialog ...
    if(exportDialog->isVisible()){
	exportDialog->raise();
    }else{
	exportDialog->show();
    }
    // which is all we do, later we make a slot to which it is connected.. 

    // first get a file Name..
//    QString fileName = QFileDialog::getSaveFileName();
//    if(fileName.isNull()){
//	return;
//    }
    // and just call client to request the thingy..
//    client->exportMeans(fileName);  
}

void ClientWindow::selectFont(){
  qApp->setFont( QFontDialog::getFont( 0, qApp->font() ), true );
}
  
void ClientWindow::newExptDistances(exptDistanceInfo data){
  cout << "client window : distances size is : " << data.values.size() 
       << "client window : experiments size is : " << data.experiments.size() << endl;

  exptComparisons->newDistances(data);    /////// need to fix this.. 
  exptComparisons->raise();
  exptComparisons->show();   //
  //  DistanceViewer* dv = new DistanceViewer(data.experiments, data.values);
  //dv->setPalette(palette());
  //dv->show();    // problem is the thread will start too quickly.. 
  //distanceViewers.insert(dv);
  //dv->start();   // allow user to control.. 
}

void ClientWindow::saveCurrentState(){
  // 
  // write the current probe set, the current index, and the currently saved probe sets to a file
  // that can be read and opened without too much of a problem... Not so exciting..

  // use QFileDialog to get a file name.. 
  QString outfile = QFileDialog::getSaveFileName();
  cout << "save Current State, -the file name is " << outfile << endl;
  if(outfile.isNull()){
    return;
  }
  ofstream out(outfile.latin1());
  if(!out){
    writeMessage("Couldn't open file");
    return;
  }
  /// write some sort of file identifier..
  out << "state_file_groovy_isnt_it" << endl;
  vector<int> currentIndex = client->curIndex();
  for(int i=0; i < currentIndex.size(); i++){
    out << currentIndex[i] << ",";
  }
  out << endl;
  // client->pSet.index is the current one,, so write that out first,, 
  out << getProbe->value() << endl;
  //  out << client->pSet.index << endl;
  list<ProbeSetWidget*>::iterator lit;
  for(lit = savedOnes.begin(); lit != savedOnes.end(); lit++){
    //  for(int i=0; i < savedOnes.size(); i++){
    out << (*lit)->mySet.index << ",";
  }
  out << endl;
  out.close();
  return;
  // lets just hope its ok.. 
}

vector<int> split_line(string& line, string& pat){
  // as above, but uses the pattern for the splitting.. 
  vector<int> numbers;
  //  string subWord;
  numbers.reserve(line.size()/3);
  int first = line.find_first_not_of(pat);
  int next;
  while(first != line.npos){
    next = line.find_first_of(pat, first);
    if(next == line.npos){
      // i.e at the end of the line. as first must be true there is something still around..
      next = line.size();
    }
    numbers.push_back(atoi(line.substr(first, next-first).c_str()));
    //words.push_back(line.substr(first, next-first));
    first = line.find_first_not_of(pat, next);
  }
  return(numbers);
}

void ClientWindow::changeIndex(vector<int> v, QString descriptor){
  cout << "change Index function " << endl;
  client->setIndex(v, descriptor);
  //client->currentIndex = v;
  changeIndexRange(v.size());
}

void ClientWindow::restoreIndex(vector<int> v, int i, QString descriptor){
  client->setIndex(v, descriptor);
  changeIndexRange(v.size());    // unfortunately will set to 0, which will retrieve a value, this is a bit messy, but isn't that a big deal.. 
  if(i < v.size()){
    getProbe->setValue(i);
  }
}

void ClientWindow::readState(){
  // read a file of numbers and see how it goes... 
  QString infile = QFileDialog::getOpenFileName();
  if(infile.isNull()){
    return;
  }
  ifstream in(infile.latin1());
  if(!in){
    writeMessage("Couldn't open file");
    return;
  }
  string delimit(",");
  string line;
  vector<int> index;
  int current;
  vector<int> savedIndex;
  // first line is the silly identifier, return if incorrect.. 
  if(in >> line){
    if(line != string("state_file_groovy_isnt_it")){
      writeMessage("Looks like wrong kind of file");
      writeMessage("should begin with: state_file_groovy_isnt_it");
      return;
    }
    //    cout << "Silly identifier: " << line << endl;
  }else{ return; }

  // next the current index
  if(in >> line){
    //    cout << "index Line\t" << line << endl;
    index = split_line(line, delimit);
  }else{ return; }
  
  // and next the current index choice.. 
  if(in >> line){
    //cout << line << endl;
    vector<int> temp = split_line(line, delimit);
    if(temp.size()){
      current = temp[0];
    }else{
      current = index[0];
      //      return;
    }
  }//else{ return; }
  // and next the line of saved indices..
  if(in >> line){
    //cout << line << endl;
    savedIndex = split_line(line, delimit);
  }//else{ return; }
  
  for(int i=0; i < index.size(); i++) { cout << i << "\t" << index[i] << endl; }
  for(int i=0; i < savedIndex.size(); i++) { cout << i << "\t" << savedIndex[i] << endl; }
  cout << "And the current choice is : " << current << endl;
  /// first insert the values of the saved ones into the indicesToSave, then 
  /// change the currentIndex to this vector, then go through all of these, and they
  /// should be automatically inserted into the saved list. Then, change the index to the current
  /// index, and set the value to where we were before..
  client->setIndex(savedIndex, "State File Saved");
  //  client->currentIndex = savedIndex;
  changeIndexRange(savedIndex.size());
  for(int i=0; i < savedIndex.size(); i++){
    indicesToSave.insert(savedIndex[i]);
    /// and call the getProbeSet for value i..
    getProbeSet(i);
    /// which should automatically do everything.. 
  }

  // and lets change the index,, not too difficult..
  client->setIndex(index, "State File Index");
  //  client->currentIndex = index;
  changeIndexRange(index.size());
  if(current < index.size()){
    getProbe->setValue(current);
  }else{
    getProbe->setValue(0);
  }
  // which should be enough.. 
}

void ClientWindow::clearSavedOnes(){
  // First pop up a dialog to confirm this.. bit painful this but here goes.. 
  QMessageBox mb( "Client",
		  "Are you sure you want to delete ALL saved probe sets?.\n",
		  QMessageBox::NoIcon,
		  QMessageBox::Yes | QMessageBox::Default,
		  QMessageBox::No | QMessageBox::Escape,
		  QMessageBox::NoButton);
  mb.setButtonText( QMessageBox::Yes, "Delete" );
  mb.setButtonText( QMessageBox::No, "Cancel" );
  list<ProbeSetWidget*>::iterator lit;
  switch( mb.exec() ) {
  case QMessageBox::Yes:
    // go through the list and delete whoa...
    while(savedOnes.size() > 0){
      lit = savedOnes.begin();
      //    for(lit = savedOnes.begin(); lit != savedOnes.end(); lit++){
      ProbeSetWidget* temp = *lit;
      savedOnes.erase(lit);
      savedIndices.erase(temp->mySet.index);
      temp->hide();
      delete temp;
    }
    break;
  case QMessageBox::No:
    // do Nothing.. 
    break;
  }
}

void ClientWindow::requestProtocolCollection(int requestId){
  const QObject* requester = sender();
  if(requester == (QObject*)protManager){
    client->getProtocolCollection(requestId, 1);
    return;
  }
  // if the sender is the dataview,, then no. 2
  if(requester == (QObject*)thumbnailViewer){
    client->getProtocolCollection(requestId, 2);
  }
}

void ClientWindow::receiveProtocolCollection(int requester, int requestId, ProtocolCollection* collection){
  //cout << "Receiving new protocol collection .. requestId is : " << requestId << "  and requester is " << requester << endl;;
  cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
  protManager->setProtocolCollection(requestId, collection);
  cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
  thumbnailViewer->exptBrowser->eMaker->setProtocolCollection(requestId, collection);  // hmmmm.. 
//   if(requester == 1){
//     return;
//   }
//   if(requester == 2){
//     cout << "called upon thumbnail viewer,, ebrwoser, emaker .. to do the dirty deed. " << endl;
//     return;
//   }
  /// and otherwise do other things..
}

void ClientWindow::setCoordinates(vector<PointCoordinate> pts){
  plotWindow->setCoordinates(pts);
  cout << "Client Window ..  SET COORDINATES..  should do something to cause some trouble somewhere.. " << endl;
}

void ClientWindow::showExperimentDetails(int n){
    exptInfo eInfo = client->getExperimentInfo(n);
//    sampleInfoWidget->show();
    sampleInfoWidget->setInfo(eInfo);
    ostringstream oss;
    oss << "dbase Index: " << eInfo.dbaseIndex << "\n"
	<< "group Index: " << eInfo.exptGroup << "\n"
	<< "      Index: " << eInfo.index << "\n"
	<< eInfo.shortName << "\n"
	<< eInfo.description;
    annotator->commentArea->setText(QString(oss.str().c_str()));
}

void ClientWindow::toggleSampleInfoWidget(){
    if(sampleInfoWidget->isHidden()){
	sampleInfoWidget->show();
	return;
    }
    sampleInfoWidget->hide();
}

void ClientWindow::changePassword(QString oldp, QString newp){
  vector<int> oldh = hashPasswd(oldp);
  vector<int> newh = hashPasswd(newp);
  // and call on client to change the passwords..
  client->changePassword(oldh, newh); 
  // which is all we need to do here really..
}

void ClientWindow::createNewUser(QString oldp, QString newp, QString uName){
  vector<int> oldh = hashPasswd(oldp);
  vector<int> newh = hashPasswd(newp);
  client->createNewUser(oldh, newh, uName);
}

vector<int> ClientWindow::hashPasswd(QString passwd){
  // the purpose of this is not to provide water tight security for my server.. 
  // this in any case is probably very difficult anyway, and since I don't have 
  // any cryptography books,, it's a little difficult.

  // The point of this exercise is to transform the password into something which has 
  // lost sufficient information such that it should be impossible to guess the password
  // based on the output of this algorithm. This means that the resulting encrypted password
  // .. oh well never mind, I'm tired..
  // I'm just going to convert the string into a series of numbers based on some simple rules,,

  vector<int> nums(passwd.length());
  //int numSums = 0;
  //int sqSums = 0;
  //int ratSums = 0;
  vector<int> returns(3);
  for(int i=0; i < passwd.length(); i++){
    //char c = passwd.at(i)
    nums[i] = (int)passwd.at(i).latin1();    // I think I can do this..
  }
  int e = nums.size();
  for(int i=0; i < nums.size(); i++){
    int v = nums[i]-85;
    e--;
    returns[0] += ((nums[i]) * ((nums[i] % 30)-15));  // bigger range,, .. gives less information about the length of the string.
    returns[0] += (nums[i] - nums[e]);             // to avoid numSums being a factor of 15. 
    returns[1] += (v*v) + (nums[e] * nums[i]); 
    returns[2] += ((nums[e] - nums[i]) * (nums[e] * v));

    //  numSums += 
//      numSums += 
//      sqSums +=    
//      ratSums +=  
    //    cout << "i : " << i << "\t" << nums[i] << endl;
  }
  cout << "Sum : \t" << returns[0] << endl;
  cout << "sqSums: \t" << returns[1] << endl;
  cout << "ratSums: \t" << returns[2] << endl;

  return(returns);
}

void ClientWindow::shiftFocus(int id){
  switch(id){
  case 100 :
    getProbe->setFocus();
    break;
  case 105 :
    lineWidth->setFocus();
    break;
  case 110 :
    annotator->commentArea->setFocus();
    break;
  case 115 :
    dbQuerier->inLine->setFocus();
    break;
  default :
    cout << "Unknown acceleration code " << endl;
  }
}

void ClientWindow::readCommands(){
  QString infile = QFileDialog::getOpenFileName();
  if(infile.isNull()){
    return;
  }
  client->readCommandFile(infile);
}

void ClientWindow::abuseTheServer(){
  QString infile = QFileDialog::getOpenFileName();
  if(infile.isNull()){
    return;
  }
  client->abuseTheServer(infile);
}
