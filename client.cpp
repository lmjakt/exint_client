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

#include "client.h"
#include "probe_set/probe_set.h"
#include "bytevec/bytevec.h"
#include "exptinfo.h"        // just has the declaration of the exptInfo struct. 
#include "annotator/sessionManager/sessionWidget.h"        // so I can make sessionWidgets !!. 
#include "sequence/sequenceRequest.h"
#include "stat/stat.h"
#include <string>
#include <qstring.h>
#include <qsocket.h>
#include <qtextstream.h>
#include <iostream>
#include <sstream>
#include <deque>
#include <qregexp.h>
#include <qfiledialog.h>
#include <qtimer.h>
#include <qimage.h>
#include <qbuffer.h>
#include <qcstring.h>
#include <qfile.h>
#include <string.h>

#ifdef Q_WS_WIN
#include <winsock.h>
#else
#include <netinet/in.h>
#include <unistd.h>
#endif

using namespace std;

ensemblGene chromRegion::getGene(int gIndex){
  ensemblGene gene;
  bool done = false;
  vector<ensemblGene>::iterator it;
  for(it = fEnsGenes.begin(); it != fEnsGenes.end(); it++){
    if((*it).dbIndex == gIndex){
      gene = (*it);
      done = true;
      break;
    }
  }
  if(!done){
    for(it = rEnsGenes.begin(); it != rEnsGenes.end(); it++){
      if((*it).dbIndex == gIndex){
	gene = (*it);
	done = true;
	break;
      }
    }
  }
  return(gene);   // if no gene, dbIndex is equal to 0,, and can be used as a boolean as we do not allow dbindex of 0.. 
}

Transcript chromRegion::getTranscript(string source, string id){
  Transcript transcript;
  pair<multimap<string, Transcript>::iterator, multimap<string, Transcript>::iterator > range;
  multimap<string, Transcript>::iterator it;
  range = fTranscripts.equal_range(source);
  for(it = range.first; it != range.second; it++){
    if((*it).second.id == id){
      transcript = (*it).second;
      break;
    }
  }
  if(!transcript.length){        // empty transcript has 0 length..
    range = rTranscripts.equal_range(source);
    for(it = range.first; it != range.second; it++){
      if((*it).second.id == id){
	transcript = (*it).second;
	break;
      }
    }
  }
  // whatever the case we still have to return the transcript..
  return(transcript);
}
  
    

ishImageData::~ishImageData(){
  //cout << "am destryoing an ishImageData with an probe indes of : " << ishProbeIndex << endl;
}

userInformation::userInformation(){
  // empty..
}

userInformation::userInformation(int i, string un, string fn, string lbn){
  index = i;
  userName = un;
  fullName = fn;
  labName = lbn;
}

void Transcript::addExon(Exon ex){
  if(ex.cstart < cstart) { cstart = ex.cstart; }
  if(ex.cend > cend){ cend = ex.cend; }
  exons.push_back(ex);
}

ensemblGene::ensemblGene(){
  dbIndex = 0;
  selected = false;
  // and ... 
}

ensemblExon::ensemblExon(){
  // do nothing..
}

ensemblExon::ensemblExon(string i, int st, int sp, int cst, int csp, int sd){
  id = i;
  start = st;
  stop = sp;
  codeStart = cst;
  codeStop = csp;
  strand = sd;
}

ensemblTranscript::ensemblTranscript(int tIndex, string i, int st, int sp, int sd, int gi){
  index = tIndex;
  id = i;
  start = st;
  stop = sp;
  strand = sd;
  geneIndex = gi;
  exons.resize(0);
  rSelected = false;
  pSelected = false;
}

void ensemblTranscript::addExon(ensemblExon ex){
  if(ex.start < start){ start = ex.start; }
  if(ex.stop > stop){ stop = ex.stop; }
  exons.push_back(ex);
}

//ensemblGene::ensemblGene(int dbi, string ensid, string extid, string des, string chr, int st, int sp, bool s){
ensemblGene::ensemblGene(int dbi, string ensid, string chr, int st, int sp, bool s){
  dbIndex = dbi;
  ensemblId = ensid;
  //  externalId = extid;
  // description = des;
  chromosome = chr;
  start = st;
  stop = sp;
  sense = s;
}

probeSetMatch::probeSetMatch(){
  cStart = cEnd = afIndex = afStart = afEnd = afLength = alLength= match = 0;
  expect = 1;
  selected = false;
}

probeSetMatch::probeSetMatch(int cs, int ce, int afi, string chr, int afs, int afe, int afl, int all, int m, int s, double exp){
  cStart = cs;
  cEnd = ce;
  afIndex = afi;
  //afid = aid;
  chromosome = chr;
  afStart = afs;
  afEnd = afe;
  afLength = afl;
  alLength = all;
  //  af_n_count = afn;
  match = m;
  strand = s;
  expect = exp;
  selected = false;
}

ishProbeMatchSet::ishProbeMatchSet(int pi, int minpos, int maxpos, int plength, float scr, int strnd, string chrome, int assemblyid){
  //ishProbeMatchSet::ishProbeMatchSet(ishProbeMatch ispm){
  //// note that this sort of solves the problem of two independent probes designed against overlapping regions,,
  //// but that the situation where we have matches to repetitive sequences within the same region will still show
  //// results that look apparently good, but which may not be so great.. (though in general if the probes are designed against
  //// predicted exon structures the problem may not be so great...
  probeIndex = pi;
  probeLength = plength;
  //  probeStart = ispm.p;
  //probeEnd = ispm.pEnd;
  cStart = minpos;
  cEnd = maxpos;
  strand = strnd;       // if the matches are to both sides of the strand I will be buggered... oh well, perhaps not if I do this the painful way.. 
  chromosome = chrome;
  //  ishMatches.push_back(ispm);
  score = scr;
  assemblyId = assemblyid;
  selected = false;
}

void ishProbeMatchSet::addMatch(ishProbeMatch ispm){
//   if(ispm.probeIndex != probeIndex){
//     return;
//     cerr << "trying to insert probe Index : " << ispm.probeIndex << "  into set with probeIndex " << probeIndex << endl;
//   }
//   if(ispm.pStart < probeStart) { probeStart = ispm.pStart; }
//   if(ispm.pEnd > probeEnd) { probeEnd = ispm.pEnd; }
//   if(ispm.cStart < cStart) { cStart = ispm.cStart; }
//   if(ispm.cEnd > cEnd){ cEnd = ispm.cEnd; }
  ishMatches.push_back(ispm);
}

// chromRegion::chromRegion(){
//   chromosome = "0";
//   chromLength = chromStart = chromEnd = 0;
//   fPMatches.resize(0);
//   selectBegin = selectEnd = 0;
// }

//// debug stuff for Richard Boyce. This probably won't make a difference though.. 
chromRegion::chromRegion(){
  chromosome = "0";
  chromLength = 1000000;
  chromStart =  1000;
  chromEnd = 10000;
  fPMatches.resize(0);
  selectBegin = selectEnd = 0;
}
  
chromRegion::chromRegion(string s, int l, int st, int e, vector<probeSetMatch> matches){
  chromosome = s;
  chromLength = l;
  chromStart = st;
  chromEnd = e;
  fPMatches = matches;           /// kludge which needs fixing at some stage.. 
  maxForwardFeatures = 0;
  maxReverseFeatures = 0;
  hasForwardIshProbe = false;
  hasReverseIshProbe = false;
  selectBegin = selectEnd = 0;
}

void chromRegion::addEnsGene(ensemblGene eg){
  if(eg.sense){
    fEnsGenes.push_back(eg);
    return;
  }
  rEnsGenes.push_back(eg);
}

void chromRegion::addPMatch(probeSetMatch psm){
  if(psm.strand == 1){
    fPMatches.push_back(psm);
    return;
  }
  rPMatches.push_back(psm);
}

void chromRegion::addIshMatch(ishProbeMatchSet ispm){
  //  map<int, ishProbeMatchSet>::iterator it;
  map<int, ishProbeMatchSet>* ptr = 0;
  if(ispm.strand == 1){
    ptr = &fIshMatches;
  }else{
    ptr = &rIshMatches;
  }
  ptr->insert(make_pair(ispm.probeIndex, ispm));
//   it = ptr->find(ispm.probeIndex);
//   if(it == ptr->end()){
//     ptr->insert(make_pair(ispm.probeIndex, ishProbeMatchSet(ispm)));
//   }else{
//     (*it).second.addMatch(ispm);
//   }
}

void chromRegion::addTranscript(Transcript trans){
  if(trans.strand == 1){
    fTranscripts.insert(make_pair(trans.source, trans));
    return;
  }
  rTranscripts.insert(make_pair(trans.source, trans));
}

// chromRegion::chromRegion(string s, int st, int e, int l, int maxF, int maxR, vector<probeSetMatch> matches, vector<ensemblGene> genes, map<int, ishProbeMatchSet> ishm){
//   chromosome = s;
//   chromLength = l;
//   chromStart = st;
//   chromEnd = e;
//   maxForwardFeatures = maxF;
//   maxReverseFeatures = maxR;
//   pMatches = matches;
//   ensGenes = genes;
//   ishMatches = ishm;
//   // check if we have any reverse matches or not..
//   hasForwardIshProbe = false;
//   hasReverseIshProbe = false;
//   for(map<int, ishProbeMatchSet>::iterator it=ishm.begin(); it != ishm.end(); it++){
//     for(int i=0; i < (*it).second.ishMatches.size(); i++){
//       if((*it).second.ishMatches[i].strand == 1){
// 	hasForwardIshProbe = true;
//       }else{
// 	hasReverseIshProbe = true;
//       }
//     }
//   }
//   /// ugly, but there you go.. 
// }

chromRegion::chromRegion(string chr, int st, int e, int cl){
  chromosome = chr;
  chromLength = cl;
  chromStart = st;
  chromEnd = e;
  selectBegin = selectEnd = 0;
}


clusterSet::clusterSet(){
  expInfo = 0;
}

clusterSet::clusterSet(vector< vector<int> > i, vector< vector< vector<float> > > p, vector< vector< vector<int> > > e, vector< vector<float> > c, vector< vector<int> > ce, map<float, exptInfo>* me, bool lN){
  indices = i;
  points = p;
  expts = e;
  centers = c;
  centExpts = ce;
  localNorm = lN;
  expInfo = me;
}
  
clusterSet::~clusterSet(){
  //cout << "deleting clusterSet " << endl;
}




Client::Client(QString uName, QString serverName, vector<int> uKeys, int port){
  indexHistorySize = 20;   // should be enough.. 
  lastRequested = 0;
  abusive = false;
  userName = uName;
  userKeys = uKeys;
  currentRegionAssociation = currentIshRegionAssociation = 0;    // default.. 
  genRegions.resize(1);                                          // this is ugly, but.. 

  currentRegions.resize(1);   // Try this for Richard Boyce ? 

//  currentRegions.resize(0);   // there must be a default allocator.. 

  ///// chip information handler..
  chipData = new ChipSelection();
  connect(chipData, SIGNAL(setClientChips(vector<int>)), this, SLOT(setClientChips(vector<int>)) );
  //////////////////////////////////// COMMAND RECORDING THINGIES /////////////////
  commandRecord = 0;
  recordCommands = false;
  #ifdef EXPERIMENTAL_CLIENT
  recordCommands = true;       // change to allow recording.. later perhaps make this a little different..
  if(recordCommands){           // choose a filename and open an ofstreamm.. 
    QString recordFile = QFileDialog::getSaveFileName();
    if(recordFile.isNull()){
      recordCommands = false;
    }else{
      commandRecord = new ofstream(recordFile.latin1());
      if(!commandRecord){
	delete commandRecord;
	commandRecord = 0;
	recordCommands = false;
      }
    }
  }
  #endif
  //  use the static one below until we can set do appropriate errorchecking.. 
 
  // make a window for display sequences..
  seqDisplay = new SeqWidget();
  seqDisplay->resize(900, 400);

  // initialiase a socket to the localhost and send the 
  // message in message, then receive the response in response..
  // and emit the signal with the stuff read.. -- actually can probably 
  // do all of this in the constructor, so probably don't need the slots..
  idata = new ByteVec(200000);   // 36000 * 4 plus some extra.. lots of space, but should be better. 
  open = false;
  reOpen = false;
  readState = 0;
  hostAddress = serverName;
  socket = new QSocket(this);
  connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()) );
  connect(socket, SIGNAL(connected()), this, SLOT(connected()) ); // necessary as I can't really do  anything until connected.. 
  connect(socket, SIGNAL(connectionClosed()), this, SLOT(connectionClosed()) );

  socket->connectToHost(hostAddress, port);         // this is the 2.5 server -- using old version of the database.. !! 

  regionSize = 200000;
  currentIndex.resize(1000);
  for(int i=0; i < currentIndex.size(); i++) { currentIndex[i] = i+1; }
  emit newIndex(currentIndex.size());
  //// this is kind of bad, but we may change later... 
}
  
Client::~Client(){
  cout << "Client destructor: " << endl;
  if(commandRecord){
    commandRecord->close();
    delete commandRecord;
  }
  delete t;
  delete chipData;   // probably lots of other things which should be deleted as well.. 
  socket->close();
  delete socket;
}

void Client::reOpenConnection(){
  // if the data Stream gets corrupted, there's no way we can keep parsing it.. (as I'm using a binary format!!
  // hence we need to close the socket and reopen it without reading anything.. i.e. immediate closure!!
  //cout << "closing socket" << endl;
  close();
  socket->close();
  delete t;
  open = false;
  reOpen = true;
  socket->connectToHost(hostAddress, 8080);
  //cout << "\tjust reopened the connection to thingy " << endl;
}

void Client::openConnection(){
  cout << "delayed Closed finished, am reopening" << endl;
  if(reOpen){
    socket->connectToHost(hostAddress, 8080);
    reOpen = false;
  }
}

void Client::setIndex(vector<int> ind, QString term){
  currentIndex = ind;
  IndexHistory.push_front(IndexSet(ind, term));  // but -- this is the wrong last request.. !! 
  if(IndexHistory.size() > 1){
    IndexHistory[1].setLast(lastRequested);
  }
  if(IndexHistory.size() > indexHistorySize){
    IndexHistory.pop_back();
  }
  emit indexHistoryChanged();            // - only for a specific receiver, perhaps.. 
  //cout << "setIndex : The term is : " << term << endl;
}

void Client::writeInt(int i){
  int ni = htonl(i);
  socket->writeBlock((const char*)&ni, sizeof(ni));   // doesn't bother to check if it works.. maybe later ..
}

void Client::writeBlock(const char* data, uint s){        // write the block..
  int remaining = s;
  int tryCounter = 0;
  int pos = 0;
  while(remaining){
    tryCounter++;
    int written = socket->writeBlock(data+pos, remaining);
    if(written < 0){
      if(tryCounter > 250){
	cerr << "Couldn't write block after trying 250 times, bugger " << endl;
	return;
      }
      continue;
    }
    remaining -= written;
    pos += written;
  }
}

void Client::connected(){
  open = true;
  cout << "Connected successfully to server: " << ((QSocket*)sender())->peerAddress().toString() << endl;
  // OK, send the length of the user Name, then the username, then the three keys,,
  int length = userName.length();
  int nlength = htonl(length);
  socket->writeBlock((const char*)&nlength, 4);
  for(int i=0; i < length; i++){
    char c = userName.at(i).latin1();
    socket->writeBlock(&c, 1);
  }
  for(int i=0; i < 3; i++){
    int key = htonl(userKeys[i]);
    //cout << "writing Key : " << userKeys[i] << "\tnetwork order: " <<  key << endl;
    socket->writeBlock((const char*)&key, 4);
  }
  t = new QTextStream((QSocket*)sender());  // for use in later functions.. 
  socket->flush();
  // and lets get the file information..
  getFileInformation();
  getDBChoices();
  getUserInformation();
  getChipInformation();
  getProbeSet(0);
  socket->flush();
  //cout << "at the end of connected" << endl;
}

void Client::sendMessage(QString request){
  if(open){
    *t << request;
  }else{
    emit statusMessage("connection has been lost");
  }
  if(recordCommands){
    *commandRecord << request << endl;
  }
}

void Client::createNewUser(vector<int> oldh, vector<int> newh, QString uName){
  if(open){
    *t << "<createNewUser>" << userName << ":"
       << oldh[0] << ":" << oldh[1] << ":" << oldh[2] << ":"
       << uName << ":"
       << newh[0] << ":" << newh[1] << ":" << newh[2] << ":>";
    socket->flush();
  }else{
    emit statusMessage("connection is closed");
  }
  if(recordCommands){
    *commandRecord << "<createNewUser>" << userName << ":"
		   << oldh[0] << ":" << oldh[1] << ":" << oldh[2] << ":"
		   << uName << ":"
		   << newh[0] << ":" << newh[1] << ":" << newh[2] << ":>" << endl;
  }
}

void Client::changePassword(vector<int> oldh, vector<int> newh){      // old and new hashes..
  if(open){
    *t << "<changePassword>" << userName << ":"
       << oldh[0] << ":" << oldh[1] << ":" << oldh[2]
       << ":" << newh[0] << ":" << newh[1] << ":" << newh[2] << ":>";
    socket->flush();
  }else{
    emit statusMessage("connection is closed");
  }
  /////////////// don't record this one -- as I might end up not knowing any passwords.. 
  ///////////////  --- only if I don't log in as master user.. which I have to try and do anyway at some point
  ////// -- anyway it's a bit dangerous.. could lead to trouble.. 
}

void Client::createSession(vector<string> words){
  // make sure the words do not contain |, <, or >, as this will really screw things up.. 
  // 
  //for(int i=0; i < words.size(); i++){
    // ahh. its a string. not a qstring. hmm. do this later..
  //}
  if(open){
    *t << "<newSession>";
    for(int i=0; i < words.size(); i++){
      *t << words[i].c_str() << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("connection is closed");
  }
  if(recordCommands){
    *commandRecord  << "<newSession>";
    for(int i=0; i < words.size(); i++){
      *commandRecord << words[i].c_str() << "|";
    }
    *commandRecord << ">" << endl;
  }
}

void Client::newAnnotation(int sIndex, vector<int> aGenes, QString note){
  // before we do this, we should make sure that the QString does not contain any
  // |, or < or > characters, as this will screw everything up. We can probably use the
  // regular expression .. to do this.. 
  note.replace(QRegExp("<"), "&lt");
  note.replace(QRegExp(">"), "&gt");
  note.replace(QRegExp("\\|"), "&pipe");
  note.replace(QRegExp("'"), "&apo");
  
  if(open){
    *t << "<newAnnotation>";
    *t << sIndex << "|";
    *t << note << "|";
    for(int i=0; i < aGenes.size(); i++){
      *t << aGenes[i] << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("connection is closed");
  }
  if(recordCommands){
    *commandRecord << "<newAnnotation>"
		   << sIndex << "|"
		   << note << "|";
    for(int i=0; i < aGenes.size(); i++){
      *commandRecord << aGenes[i] << "|";
    }
    *commandRecord << ">" << endl;
  }
}

void Client::updateSessionDescription(int sessionIndex, vector<QString> words){
  //cout << "updateSessionDescription words size is " << words.size() << endl;
  if(words.size() < 2){
    return;
  }
  if(open){
    *t << "<updateSessionDescription>" << sessionIndex << "|" << words[0] << "|" << words[1] << "|";
    for(int i=2; i < words.size(); i++){
      *t << words[i] << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("Connection is lost, byby");
  }
  if(recordCommands){
    *commandRecord <<  "<updateSessionDescription>" << sessionIndex << "|" << words[0] << "|" << words[1] << "|";
    for(int i=2; i < words.size(); i++){
      *commandRecord << words[i] << "|";
    }
    *commandRecord << ">" << endl;
  }
}

void Client::updateAnnotationDescription(int annotationIndex, QString description){
  if(open){
    *t << "<updateUserAnnotation>" << annotationIndex << "|" << description << "|>";
    socket->flush();
  }else{
    emit statusMessage("Connection is lost, can't update Annotation");
  }
  if(recordCommands){
    *commandRecord << "<updateUserAnnotation>" << annotationIndex << "|" << description << "|>" << endl;
  }
}

void Client::updateSessions(){
  if(open){
    *t << "<sendSessionInfo>>";
    socket->flush();
  }else{
    emit statusMessage("connection has been lost");
  }
  if(recordCommands){
    *commandRecord << "<sendSessionInfo>>" << endl;
  }
}
  
void Client::doDBLookup(QString request){
  // for now dont do any formatting..
  if(open){
    *t << "<dbLookup>" << request << ">";
  }else{
    emit statusMessage("connection has been lost");
    return;
  }
  if(recordCommands){
    *commandRecord << "<dbLookup>" << request << ">" << endl;
  }
  socket->flush();
}

void Client::doGenLookup(QString request){
  // for now dont do any formatting..
  if(open){
    *t << "<genDBLookup>" << request << ">";
  }else{
    emit statusMessage("connection has been lost");
    return;
  }
  socket->flush();
}

void Client::getProbeSet(int n){
  if(n >= currentIndex.size() || n < 0){
    cerr << "get Probe Set, n is larger than current Index : " << endl;
    return;
  }
  if(open){
    *t << "<getProbeSet>" << currentIndex[n] << "|" << regionSize <<  "|>";
    socket->flush();
    lastRequested = n;
  }else{
    emit statusMessage("connection has been lost");
  }
  if(recordCommands){
    *commandRecord << "<getProbeSet>" << currentIndex[n] << "|" << regionSize <<  "|>" << endl;
  }
}

void Client::getRegion(int n){
  cout << "Client get region function n is : " << n << endl;
  int maxExpect = -10;
  int minLength = 35;
  float minMatch = 0.9;        // these should be somewhere else, but we'll work it out later..
  if(open){
    *t << "<sendRegion>" << n << "|"  
       << maxExpect << "|"
       << minLength << "|"
       << minMatch << "|"
       << 3 << "|>";            // number 3 is just another genomic region thingy.. 
    socket->flush();
  }else{
    emit statusMessage("Socket is closed ");
  }
}

void Client::setRegionSize(int i){
  regionSize = i;
  //cout << "just set region size to  " << i  << endl;
}

void Client::exportMeans(QString fileName, int normMethod){
  cout << "fileName is " << fileName << endl;
  // first make sure that the client index is the same as the thingy..
  sendClientIndex();
  if(open){
    *t << "<ExportMeans>";
    *t << fileName << "|";
    *t << normMethod << "|";
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << "|";
    }
    *t << ">";
    socket->flush();
    saveFileNames.insert(fileName.latin1());
  }else{
    emit statusMessage("export Means cannot find a connection");
  }
  return;
}

void Client::parseMeans(){
  cout << "This is parse Means and we should be doing something with a file or something" << endl;
  string fileName = idata->s();
  cout << "And the file name I got is " << fileName << endl;
  set<string>::iterator it = saveFileNames.find(fileName);
  if(it == saveFileNames.end()){
    cerr << "Parse Means got an unknown file Name, return without doing anything.. " << endl
	 << "Received : " << fileName << endl;
    return;
  }
  saveFileNames.erase(fileName);
  ofstream out(fileName.c_str());
  if(!out){
    cerr << "Couldn't open " << fileName << endl;
    return;
  }
  while(1){
    string line = idata->s();
    if(line == "<ExportMeansEnd>"){        // this is very bad as we may get into an infinite loop. .. ahh, what to do.. 
      break;
    }
    out << line << endl;
  }
  out.close();
}

void Client::getFileInformation(){
  // request for fileInformatiom..
  if(open){
    *t << "<getExptInformation>>";
  }else{
    emit statusMessage("connection has been lost");
  }
  if(recordCommands){
    *commandRecord << "<getExptInformation>>" << endl;
  }
}

void Client::getUserInformation(){
  if(open){
    *t << "<getUserInformation>>";
    socket->flush();
  }else{
    emit statusMessage("No connection");
  }
  if(recordCommands){
    *commandRecord << "<getUserInformation>>" << endl;
  }
}

void Client::getChipInformation(){
  cout << "Client getchipinfo..  .. thingy " << endl;
  if(open){
    *t << "<sendChipInfo>>";
    socket->flush();
  }else{
    emit statusMessage("connections has been lost");
  }
}

void Client::getDBChoices(){
  if(open){
    *t << "<getDBChoices>>";
  }else{
    emit statusMessage("connections has been lost");
  }
  if(recordCommands){
    *commandRecord << "<getDBChoices>>" << endl;
  }
}

void Client::doEuclidSort(int n){
    if(open && n < currentIndex.size()){
//	*t << "<euclidCompare>" << currentIndex[n] << ">";
// comment the above line and uncomment the below lines to switch to old style

	*t << "<euclidCompare>" << currentIndex[n] << "|";
	for(uint i=0; i < selectedExperiments.size(); i++){
	    *t << selectedExperiments[i] << "|";
	}
	*t << ">";
	//    cout << "<euclidCompare>" << currentIndn << ">" << endl;

    }else{
	emit statusMessage("connection has been lost");
    }
    if(recordCommands){
	*commandRecord << "<euclidCompare>" << currentIndex[n] << ">" << endl;
    }
}

void Client::doBinaryComparison(int up, int down, bool normed){
  int flushInterval = 250;
  sendClientIndex();
  if(open){
//     *t << "<setClientIndex>";
//     for(int i=0; i < currentIndex.size(); i++){
//       *t << currentIndex[i] << ":";
//       if(i % flushInterval == 0){
// 	socket->flush();
//       }
//     }
//     *t << ">";
    *t << "<doDiff>" << up << ":" << down << ":";
    if(normed){
      *t << 1;
    }else{
      *t << 0;
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("Connection has Been Lost");
  }
  if(recordCommands){
//     *commandRecord << "<setClientIndex>";
//     for(int i=0; i < currentIndex.size(); i++){
//       *commandRecord << currentIndex[i] << ":";
//     }
//     *commandRecord << ">" << endl;
    *commandRecord << "<doDiff>" << up << ":" << down << ":";
    if(normed){
      *commandRecord << 1;
    }else{
      *commandRecord << 0;
    }
    *commandRecord << ">" << endl;
  }
}

void Client::getStatCollection(){
  // first make sure we sync the current selelction.. 
  int flushInterval = 250;
  sendClientIndex();
  if(open){
//     *t << "<setClientIndex>";                                //// NOTE PLEASE TRY to change these to synchronisation efforts at some 
//     for(int i=0; i < currentIndex.size(); i++){              //// Point as this could have us sending 36000*4 bytes down the pipe
//       *t << currentIndex[i] << ":";                          ///  for no real good reason.. 
//       if(i % flushInterval == 0){
// 	socket->flush();
//       }
//     }
    *t << "<getStatCollection>";
    // and go through the current crop of experiments..
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << ":";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("connection Lost");
  }
  if(recordCommands){
    *commandRecord << "<setClientIndex>";                                //// NOTE PLEASE TRY to change these to synchronisation efforts at some 
    for(int i=0; i < currentIndex.size(); i++){              //// Point as this could have us sending 36000*4 bytes down the pipe
      *commandRecord << currentIndex[i] << ":";                          ///  for no real good reason.. 
    }
    *commandRecord << ">" << endl << "<getStatCollection>";
    // and go through the current crop of experiments..
    for(int i=0; i < selectedExperiments.size(); i++){
      *commandRecord << selectedExperiments[i] << ":";
    }
    *commandRecord << ">" << endl;
  }
}

void Client::doMeanComparison(vector<float> values, vector<int> eIndex, bool normed, bool distribution){
  //cout << "in the Do mean comparison function in client.cpp" << endl;
  if(values.size() != eIndex.size()) { return; }
  int flushInterval = 250;
  sendClientIndex();
  if(open){
//     *t << "<setClientIndex>";                                //// NOTE PLEASE TRY to change these to synchronisation efforts at some 
//     for(int i=0; i < currentIndex.size(); i++){              //// Point as this could have us sending 36000*4 bytes down the pipe
//       *t << currentIndex[i] << ":";                          ///  for no real good reason.. 
//       if(i % flushInterval == 0){
// 	socket->flush();
//       }
//     }
    *t << "<doMeanComparison>";
    for(int i=0; i < values.size(); i++){
      *t << values[i] << ":" << eIndex[i] << ":";
    }
    if(normed){
      *t << 1;
    }else{
      *t << 0;
    }
    *t << ":" << (int)distribution  << ":>";
    socket->flush();
  }else{
    emit statusMessage("connection lost");
  }
  if(recordCommands){
    *commandRecord << "<setClientIndex>";                                //// NOTE PLEASE TRY to change these to synchronisation efforts at some 
    for(int i=0; i < currentIndex.size(); i++){              //// Point as this could have us sending 36000*4 bytes down the pipe
      *commandRecord << currentIndex[i] << ":";                          ///  for no real good reason.. 
    }
    *commandRecord << ">" << endl << "<doMeanComparison>";
    for(int i=0; i < values.size(); i++){
      *commandRecord << values[i] << ":" << eIndex[i] << ":";
    }
    if(normed){
      *commandRecord << 1;
    }else{
      *commandRecord << 0;
    }
    *commandRecord << ">" << endl;
  }
  //cout << "at the end of the mean comparison function what's going on" << endl;
} 

void Client::doRawComparison(vector<float> values, vector<int> eIndex, bool distribution){
  // Note this comes straight from Comparison Window, which doesn't know anything about 
  // different experimental indices. At the moment, I'm not sure exactly how to deal with this.
  // --- before I can do this properly I need to set up mechanisms for maintaining a current selection
  // of Experiments that one does the comparisons on.. -- Will do this later...
  //cout << "in Client doRawComparison,, values size: " << values.size() << "\teIndex size: " << eIndex.size() << endl;
  if(values.size() != eIndex.size()) { return; }
  int flushInterval = 250;
  sendClientIndex();
//   if(open){
//     *t << "<setClientIndex>";                                //// NOTE PLEASE TRY to change these to synchronisation efforts at some 
//     for(int i=0; i < currentIndex.size(); i++){              //// Point as this could have us sending 36000*4 bytes down the pipe
//       *t << currentIndex[i] << ":";                          ///  for no real good reason.. 
//       if(i % flushInterval == 0){
// 	socket->flush();
//       }
//     }
//    *t << "><doRawComparison>";
  if(open){
    *t << "<doRawComparison>";
    for(int i=0; i < values.size(); i++){
      *t << values[i] << ":" << eIndex[i] << ":";
    }
    *t << (int)distribution << ":>";
    socket->flush();
  }else{
    emit statusMessage("connection lost");
  }
  if(recordCommands){
    *commandRecord << "<setClientIndex>";                                //// NOTE PLEASE TRY to change these to synchronisation efforts at some 
    for(int i=0; i < currentIndex.size(); i++){              //// Point as this could have us sending 36000*4 bytes down the pipe
      *commandRecord << currentIndex[i] << ":";                          ///  for no real good reason.. 
    }
    *commandRecord << ">" << endl << "<doRawComparison>";
    for(int i=0; i < values.size(); i++){
      *commandRecord << values[i] << ":" << eIndex[i] << ":";
    }
    *commandRecord << ">" << endl;
  }
}

void Client::sendClientIndex(){
  int flushInterval = 250;
  cout << "sendClientIndex" << endl;
  // since we seem to have a problem on Kataoka-san's Intel iMac (at least in emulated) mode
  // with synching of the stream, let's write to a an ostring stream first
  ostringstream message;
  if(open){
      message << "<setClientIndex>";
      //     *t << "<setClientIndex>";
    for(uint i=0; i < currentIndex.size(); i++){
//	cout << i << endl;
	message << currentIndex[i] << ":";
//	*t << currentIndex[i] << ":";
//	if(i % flushInterval == 0){
//	    flush(*t);
//	}
    }
    message << ">";
//    *t << ">";

    ostringstream test;
    test << "<setClientIndex>";
    test << 23 << ":" << 24 << ":" << ">";
    cout << test.str() << endl;
    cout << "length of test is " << test.str().length() << endl;
    const char* test_c = test.str().c_str();
    cout << "and as a c string we get " << test_c << endl;
    cout << "and the length of the c_string is " << strlen(test_c) << endl;

    string smes = message.str();
    const char* mes = smes.c_str();
    uint bleft = smes.length();
    uint bwritten = 0;
    while(bleft > 0){
	long b = socket->writeBlock(mes + bwritten, bleft);
	if(b < 0){
	    cerr << "An error occured trying to write to the socket.. " << endl;
	    break;
	}
	bwritten += (uint)b;
	bleft -= (uint)b;
    }
    socket->flush();
//    *t << message.str().c_str();
    cout << "currentIndex.size is " << currentIndex.size() << endl;
    cout << "calling flush on the socket " << endl;
    flush(*t);
//    socket->flush();
    cout << "returned from flushing the socket " << endl;
  }else{
    emit statusMessage("connection lost");
  }
  if(recordCommands){
    *commandRecord << "<setClientIndex>";
    for(int i=0; i < currentIndex.size(); i++){
      *commandRecord << currentIndex[i] << ":";
    }
    *commandRecord << ">" << endl;
  }
}

void Client::setClientChips(vector<int> v){
  if(open){
    *t << "<setClientChips>";
    for(uint i=0; i < v.size(); i++){
      *t << v[i] << "|";
    }
    *t << ">";
    socket->flush();
  }    
}

void Client::doKCluster(int k, bool localNorm, bool individualNorm, bool meanNorm){
  sendClientIndex();
  if(open){
    *t << "<doKCluster>" << k << "|"
       << (int)localNorm << "|" << (int)individualNorm << "|"
       << (int)meanNorm << "|";
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("connection lost");
  }
  if(recordCommands){
    *commandRecord << "<doKCluster>" << k << "|"
       << (int)localNorm << "|" << (int)individualNorm << "|"
       << (int)meanNorm << "|";
    for(int i=0; i < selectedExperiments.size(); i++){
      *commandRecord << selectedExperiments[i] << "|";
    }
    *commandRecord << ">" << endl;
  }
}

void Client::compareExperiments(){
  sendClientIndex();
  if(open){
    *t << "<compareExperiments>";
    //    *t << "<compareExperiments>";
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("connection not here anymore");
  }
}

void Client::flatCompareExperiments(float sigma, float order){
  sendClientIndex();
  if(open){
    *t << "<flatCompareExperiments>";
    *t << sigma << '|';
    *t << order << '|';
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("lost connection");
  }
}

void Client::traceExperiments(float sm){
  sendClientIndex();
  if(open){
    *t << "<traceExperiments>";
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << "|";
    }
    *t << sm <<  "|>";
    socket->flush();
  }else{
    emit statusMessage("trace Experiments, no connection");
  }
}

void Client::compareCells(vector<int> a, vector<int> b){
  //cout << "I should be asking the thingy to be comparing the experiments here" << endl;
  sendClientIndex();
  if(open){
    *t << "<compareCellGroups>" << a.size() << "|";
    for(int i=0; i < a.size(); i++){
      *t << a[i] << "|";
    }
    *t << b.size() << "|";
    for(int i=0; i < b.size(); i++){
      *t << b[i] << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("lost connection");
  }
}
    
       
  
void Client::doSelectAnovaSort(){
  int flushInterval = 250;
  if(open){
      sendClientIndex();

//     *t << "<setClientIndex>";
//     for(int i=0; i < currentIndex.size(); i++){
//       *t << currentIndex[i] << ":";
//       if(i % flushInterval == 0){
// 	socket->flush();
//       }
//     }
    *t << "<doAnovaSelectSort>";
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << ":";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("connection lost");
  }
  if(recordCommands){
    *commandRecord << "<doAnovaSelectSort>";
    for(int i=0; i < selectedExperiments.size(); i++){
      *commandRecord << selectedExperiments[i] << ":";
    }
    *commandRecord << ">" << endl;
  } 
}

void Client::getDevsFromMean(){
  if(open){
    sendClientIndex();  // just in case we changed it..
    *t << "<DevsFromMean>";
    //cout << "selectedExperiments size is : " << selectedExperiments.size() << endl;
    for(int i=0; i < selectedExperiments.size(); i++){
      *t << selectedExperiments[i] << "|";
    }
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("Get Devs from Mean, connection is closed");
  }
}

void Client::doAnovaSort(){
  // first it's necessary to send the current index to the server.
  int flushInterval = 250;
  if(open){
    sendClientIndex();
//     *t << "<setClientIndex>";
//     for(int i=0; i < currentIndex.size(); i++){
//       *t << currentIndex[i] << ":";
//       if(i % flushInterval == 0){
// 	socket->flush();
//       }
//     }
    *t << "<doAnovaSort>>";
    socket->flush();
  }else{
    emit statusMessage("connection lost");
  }
  if(recordCommands){
    *commandRecord << "<doAnovaSort>>" << endl;
  }
}

void Client::getGenomicRegion(string chr, int start, int end){
  if(open){
    *t << "<sendGenomicRegion>";
    *t << "ps,|" << chr.c_str() << "|" << start << "|" << end << "|>";
    socket->flush();
  }else{
    emit statusMessage("no more connection, bye bye");
  }
  if(recordCommands){
    *commandRecord << "<sendGenomicRegion>"
		   << "ps,|" << chr.c_str() << "|" << start << "|" << end << "|>" << endl;
  }
}

void Client::getGenomicRegionEnsemblGenes(int requestId, string chr, int start, int end){
  if(open){
    *t << "<sendGenomicRegion>";
    *t << "egene,|" << chr.c_str() << "|" << start << "|" << end << "|" << requestId << "|>";
    socket->flush();
  }
  if(recordCommands){
    *commandRecord << "<sendGenomicRegion>"
		   << "egene,|" << chr.c_str() << "|" << start << "|" << end << "|" << requestId << "|>" << endl;
  }
}

////////////////////////////////////////////////////////////////////
///  a test slot .. 

void Client::testSlot(){
  if(open){
    *t << "<doBlast>";
    *t << ">";
    socket->flush();
  }else{
    emit statusMessage("no more connection you fool");
  }
}
  
void Client::clearHeader(){
  readState = 0;
  currentHeader = "";
}


void Client::readyRead(){
  cout << "beginning of the new ready read" << endl;
  cout << "sockets available is : " << socket->bytesAvailable() << endl;
  char c;
  while(socket->bytesAvailable() > 0){                  //looking for new format thingy.. 
    switch(readState){
    case 0:    // looking for a <
//      cout << "\treadState: " << readState << endl;
      if(socket->getch() == '#'){
	readState = 1;
      }
      break;
    case 1:
      // read until we get to a :
      c = socket->getch();
      //     cout << "\treadState: " << readState << "\t:" << c << endl;
      if(c != ':'){
	dataId.append(c);               // NEED to DECLARE this in header file..
      }else{
	readState = 2;
      }
      break;
    case 2:
      c = socket->getch();
      //    cout << "\treadState: " << readState << endl;
      if(c != '#'){
	dataSize.append(c);
      }else{
	readState = 3;   // we got everywhere..
	// and resize dataArray..
	//dataArray.resize(dataSize.toInt());
	//currentPosition = 0;
      }
      break;
    case 3:
      cout << "readState is 3::::: dataId: " << dataId << "\tdataSize: " << dataSize << endl;
      if(!readArray()){
	cout << "readArray returned false" << endl;
	//socket->waitForMore(100);
	if(open){
	  *t << "<pleaseFlush>>";
	}
	
	//getProbeSet(0);    // arghh, not nice.. This is so ugly, but it seesm to work. history will be strange though..
      }else{
	// reset the dataSize and dataId things..
	idata->init();
	//int id = dataId.toInt();
	//acknowledgeReceipt(id);   // this is regardless of whether or not we understood it.. 
	readState = 0;
	dataSize = "";
	dataId = "";
      }
      break;
    default:
      cout << "Unknown ReadState: " << readState << endl;
    }
  }
  //cout << "no more bytes to read.. " << endl;
  //cout << "         or....         " << socket->bytesAvailable() << endl;
  if(abusive){
    doTheAbuse();
  }
}

bool Client::readArray(){
  ////////////////   We better decleare a QByteArray in the header somewhere.. 
  //idata->init();  // set to 0 size..NONONONONONON then if we return after not having finished we are screwed!! 
  int size = dataSize.toInt();
  while(idata->size() < size && socket->bytesAvailable() > 0){
    idata->app(socket->getch());
  }
  cout << "\tidata size: " << idata->size() << endl;


  if(idata->size() < size){
    return(false);
  }

  if(!abusive){
    parseArray();   // it's got to be global anyway for us to get away with this.. so there we go.. 
  }
  // and then whatever the case happens, if we get here, we want to get rid of this data
  // array, and then return..
  //  if(abusive){
  //  doTheAbuse();
  //}
  return(true);
}

exptInfo Client::getExperimentInfo(int n){
    exptInfo eInfo;
    if(!exptLookup.count(n))
	return(eInfo);
    if(!expInfo.count(exptLookup[n]))
	return(eInfo);
    return(expInfo[exptLookup[n]]);
}

void Client::parseArray(){
  if(idata->size() == 0) { return; }
  string header = idata->s();

  cout << "\t\theader: " << header << endl;
  if(header == string("<ProbeSet>")){
    if(parseProbeSet()){
      cout << "parseProbeSet returned true.. good one" << endl;
    }

  }
  if(header == string("<ProbeData>")){
    if(parseProbeData()){
      //cout << "parseProbeData returned true... most excellent oh enlightened one" << endl;
    }else{
      //cout << "ohh bugger what's going on" << endl; 
    }
    // read Data..
  }
  if(header == string("<ProbeIndex>")){
    if(parseIndex()){
      //cout << "got the New index, thank you very much" << endl;
    }else{
      //cout << "parseIndex returned false.. bugger.. " << endl;
    }
    // read Index..
  }
  if(header == string("<ExptInfo>")){
    if(parseExptInfo()){
      //cout << "parseExptInfo returnd true.. great!! " << endl;
    }else{
      //cout << "parseExptInfo returned false, bummer" << endl;
    }
    // read ExptInfo
  }
  if(header == "<ChipInfo>"){
    parseChipInfo();
  }
  if(header == string("<DBChoices>")){
    if(parseDBChoices()){
      //cout << "DBchoices parse OK ... !!! " << endl;
    }else{
      //cout << "BUGGER, couldn't get the dbchoices.. " << endl;
    }
    // read choices..
  }
  if(header == string("<Message>")){
    if(parseMessage()){
      //cout << "message parsed, great" << endl;
    }else{
      //cout << "couldn't parse the message " << endl;
    }
  }
  if(header == "<StatusMessage>"){
    parseStatusMessage();
  }
  if(header == string("<ChangePassWordError>")){
    emit passWordError();
  }
  if(header == string("<passWordChanged>")){
    int check = idata->qi();
    emit passWordChanged((bool)check);
    //cout << "Emitted passWordChanged " << (bool)check << endl;
  }
  if(header == "<statCollection>"){
    if(parseVitalStatistics()){
      //cout << "Parsed a Stat collection,, " << endl;
    }else{
      //cout << "some problem in parsing the stat collection " << endl;
    }
  }
  if(header == "<sessionCreated>"){
    if(parseSessionCreation()){   // the parser does the emiting of the appropriate signals .. 
      //cout << "Session created parsed " << endl;
    }else{
      cout << "problem in parsing session created message.. " << endl;
    }
  }
  if(header == "<sessionDescription>"){
    if(parseSessionDescription()){
      // cout << "SessionDescription Parsed OK" << endl;
    }else{
      cout << "problem in parsing session description" << endl;
    }
  }
  if(header == "<userInformation>"){
    if(parseUserInformation()){
      //cout << "great Got the userInformation" << endl;
    }else{
      cerr << "Couldn't parse the user information , bugger" << endl;
    }
  }
  if(header == "<KClusters>"){
    parseClusters();
  }
  if(header == "<dnaSequence>"){
    parseSequence();
  }
  if(header == "<chromRegionProbeSetMatches>"){
    parseGenomicRegionProbeSetMatches();
  }
  if(header == "<chromRegionEnsemblGenes>"){
    parseGenomicRegionEnsemblGenes();
  }
  if(header == "<chromRegion>"){
    parseGenomicRegion();
  }
  if(header == "<ishThumbNail>"){        /// BAD, -I shouldn't be using a capital N in thumbnail,, but it slipped into the server code..
    parseThumbnail();
  }
  if(header == "<ishFullImage>"){
    parseIshImage();
  }
  if(header == "<sendingRegions>"){
    parseGenomicRegionHeader();    // just set the indices for the current things being sent.. 
  }
  if(header == "<finishedSendingRegions>"){
    parseGenomicRegionTailer();    // just make sure that numbers are ok.. swap temp and current and resize temp to 0.. 
  }
  if(header == "<ishProbeData>"){
    parseIshProbeData();
  }
  if(header == "<ishTextFields>"){
    parseIshTextFields();
  }
  if(header == "<ishFloatFields>"){
    parseIshFloatFields();
  }
  if(header == "<ishClasses>"){
    parseIshClasses();
  }
  if(header == "<ExperimentDistances>"){
    parseExperimentDistances(false);
  }
  if(header == "<FlatExperimentDistances>"){
    parseExperimentDistances(true);
  }
  if(header == "<ExperimentTrace>"){
    parseExperimentTrace();
  }
  if(header == "<ProtocolCollection>"){
    parseProtocolCollection();
  }
  if(header == "<Protocol>"){
    parseProtocol();
  }
  if(header == "<Experiment>"){
    parseExperiment();
    //cout << "Receiving an experiment.. " << endl;
  }
  if(header == "<ExperimentCollection>"){
    parseExperiments();
  }
  if(header == "<Tissues>"){
    parseTissues();
  }
  if(header == "<IshAnnotationFields>"){
    parseIshAnnotationFields();
  }
  if(header == "<clientRegionsChanged>"){
    parseNewRegions();
  }
  if(header == "<ExportMeans>"){
    parseMeans();
  }
  //if(abusive){
  //  doTheAbuse();
  //}
  // which should be enough.. 
  // do nothing for now, until I've worked this one out..
  // in any case..
}

bool Client::parseMessage(){
  string message = idata->s();
  string tailer = idata->s();
  if(tailer == string("<MessageEnd>")){
    emit statusMessage(message.c_str());
    return(true);
  }else{
    emit statusMessage("couldn't parse message");
    return(false);
  }
}

void Client::parseStatusMessage(){
  unsigned int id = idata->ui();
  int no = idata->i();
  int okInt = idata->i();
  //  bool ok = (bool)idata->i();
  bool ok = (bool)okInt;
  int mno = idata->i();
  //  cout << "parse Status message id " << id << "  no " << no << "  ok " << ok << " mno " << mno << endl;
  //exit(1);
  vector<QString> messages;
  for(int i=0; i < mno; i++){
    messages.push_back(idata->s().c_str());
  }
  mno = idata->i();
  vector<int> codes;
  for(int i=0; i < mno; i++){
    codes.push_back(idata->i());
  }
  string tailer = idata->s();
  if(tailer != "<StatusMessageEnd>"){
    cerr << "Some problem in parsing a status message, got " << tailer << "  as a tailer instead of <StatusMessageEnd>" << endl;
    return;
  }
  // if id is equal to 0, noone is going to take this message, so lets just send out one message with all of the things.
  if(id == 0){
    QString longMessage = "Some Problems, please read carefully\n";
    for(int i=0; i < messages.size(); i++){
      longMessage += messages[i];
      longMessage += "\n";
    }
    emit statusMessage(longMessage);
    return;
  }
  // otherwise, let's hope someone takes care of it..
  emit newStatus(StatusMessage(id, no, messages, codes, ok));
}
  

bool Client::parseUserInformation(){
  userId = idata->qi();
  int sz = idata->qi();
  map<int, userInformation> temp;
  for(int i=0; i < sz; i++){
    int id = idata->qi();
    string userName = idata->s();
    string fullName = idata->s();
    string labName = idata->s();
    temp.insert(make_pair(id, userInformation(id, userName, fullName, labName)));
  }
  string tail = idata->s();
  if(tail == "<userInformationEnd>"){
    // everything ok..
    userInfo = temp;
    return(true);
  }
  cerr << "parseUserInformation : wrong tail obtained, returning false " << endl
       << "tail : " << tail << endl;
  return(false);
}


bool Client::parseProbeSet(){
  //cout << "Top of parseProbeSet" << endl;
  int index;
  vector<int> exptIndex;
  vector< vector<float> > values;
  index = idata->qi();
  cout << "index = " << index << endl;
  int exptSize = idata->qi();
  cout << "exptSize = " << exptSize << endl;
  for(int i=0; i < exptSize; i++){
    exptIndex.push_back(idata->qi());
  }
  int probeNo = idata->qi();
  int s;
  cout << "probe No : " << probeNo << endl;
  values.resize(probeNo);
  for(int i=0; i < probeNo; i++){
    s = idata->qi();
    values[i].resize(s);
    cout << "\t" << s;
    for(int j=0; j < s; j++){
      values[i][j] = idata->f();
    }
  }
  cout << endl;
  // and now we have everything,, check that the last word is OK..
  string tailer = idata->s();
  if(tailer == string("<ProbeSetEnd>")){
    // everything should be OK, and we should be able to call the probe_set constructor and
    // emit the probe set in the usual manner, and see if it gets plotted..
    //cout << "got the probe set end marker" << endl;
    //cout << "index : " << index << "\tvalues size : " << values.size() << "\texpt's size : " << exptIndex.size() << endl;
    cout << "about to make a pset .. " << endl;
    pSet = probe_set(index, values, exptIndex);
    emit newPSet(pSet);
    // and resize currentRegions to 0..
    //tempRegions.resize(0);   // all resizing happens at the swapping moment, just before the emission.. 
    //currentRegions.resize(0);
    return(true);
  }
  return(false);
}

bool Client::parseExptInfo(){
  //cout << "\t\t beginning of parse Expt Info" << endl;
  int mapSize = idata->qi();
  for(int i=0; i < mapSize; i++){
    exptInfo* currentInfo = new exptInfo();
    float currentKey = idata->f();
    currentInfo->dbaseIndex = idata->qi();;
    currentInfo->exptGroup = idata->qi();
    currentInfo->index = idata->f();           // this seems to be broken as it is always 0... hmmmmm 
    currentInfo->index = currentKey;
    currentInfo->shortName = idata->s();
    currentInfo->description = idata->s();
    int chipSize = idata->qi();
    int chipId;
    for(int j=0; j < chipSize; j++){
      chipId = idata->qi();
      bool used = idata->b();
      currentInfo->chips.insert(make_pair(chipId, used));
    }
    if(expInfo.count(currentKey) == 0){
      expInfo.insert(make_pair(currentKey, *currentInfo));
      exptLookup.insert(make_pair(currentInfo->dbaseIndex, currentKey));
      delete currentInfo;
    }
  }
  ///////  this is being very optimistic, and could cause crashes, as I don't actually
  ///////  check if its working. I guess I could do a reassignment somwe
  ///////  if it causes problems, I could create a temporary map, and then use an
  ///////  assignment if there's a problem..
  string tailer = idata->s();
  if(tailer == string("<ExptInfoEnd>")){
    // everything is good.. return true..
    emit exptInfoChanged();
    return(true);
  }else{
    cout << "Holy COWS,, exptInfo is corrupted, run for cover, write a decent parser for god's sake" << endl;
    return(false);
  }
}

void Client::parseChipInfo(){
  cout << "parsing a chip info " << endl;
  // we basically need to make a map of things..
  map<int, chipInfo> info;
  int n=idata->qi();
  for(int i=0; i < n; i++){
    int index = idata->qi();
    string id = idata->s();
    string description = idata->s();
    set<int> equivs;
    int m = idata->qi();
    for(int j=0; j < m; j++){
      equivs.insert(idata->qi());
    }
    // and lets make a thingy..
    info.insert(make_pair(index, chipInfo(index, id, description, equivs)));
  }
  // make sure that things are ok..
  string tailer = idata->s();
  if(tailer != "<ChipInfoEnd>"){
    cerr << "Client: parseChipInfo, received inapproprate tailer got : " << tailer << endl;
    return;
  }
  cout << "emitting the chip info size is : " << info.size() << endl;
  // and then for the chipData call the appropriate information..
  chipData->setChipInfo(info);
}
  
bool Client::parseProbeData(){
    
  probe_data tdata; // temporary. probe_data..
  tdata.index = idata->qi();
  tdata.gbid = idata->s();
  tdata.afid = idata->s();
  tdata.ensemblBlastGuess = idata->qi();
  int ugsize = idata->qi();

  tdata.ugData.resize(ugsize);

  for(int i=0; i < ugsize; i++){
    tdata.ugData[i].index = idata->qi();
    tdata.ugData[i].title = idata->s();
    tdata.ugData[i].gene = idata->s();
  }
//   int celSize = idata->qi();
//   tdata.celeraMatches.resize(celSize);
//   for(int i=0; i < celSize; i++){
//     tdata.celeraMatches[i].celeraGene = idata->s();
//     tdata.celeraMatches[i].expectation = idata->f();
//     tdata.celeraMatches[i].match = idata->f();
//     tdata.celeraMatches[i].sf = idata->s();
//     tdata.celeraMatches[i].fn = idata->s();
//     tdata.celeraMatches[i].gs = idata->s();
//     tdata.celeraMatches[i].nd = idata->s();
//   }
  /// and the space for the ensembl matches..

 //  int ensemblSize = idata->qi();
//   for(int i=0; i < ensemblSize; i++){
//     string matchId = idata->s();
//     int eLength = idata->qi();
//     int afLength = idata->qi();
//     int index = tdata.ensemblMatches.size();
//     tdata.ensemblMatches.push_back(blastMatch(matchId, eLength, afLength));
//     int desFields = idata->qi();
//     for(int j=0; j < desFields; j++){
//       string field = idata->s();
//       string des = idata->s();
//       tdata.ensemblMatches[index].description.insert(make_pair(field, des));
//     }
//     int aligns = idata->qi();
//     for(int j=0; j < aligns; j++){
//       int affy_start = idata->qi();
//       int affy_end = idata->qi();
//       int m_start = idata->qi();
//       int m_end = idata->qi();
//       double expect = idata->d();
//       int align_length = idata->qi();
//       int match_length = idata->qi();
//       int af_n_count = idata->qi();
//       tdata.ensemblMatches[index].alignments.push_back(blastAlignment(affy_start, affy_end, m_start, m_end, expect, align_length, match_length, af_n_count));
//     }
//   }
  /// and the space for the ensembl genome blast search results. // to be updated with more interesting stuff
//   int gMatchSize = idata->qi();
  
//   for(int i=0; i < gMatchSize; i++){
//     //cout << "This will crash anyway, but length requested is " << idata->qi() << endl;
//     //cout << "just before reading the string for the chromosome" << endl;
//     string chromosome = idata->s();
//     // cout << "just after reading the string for the chromosome : " << chromosome << endl;
//     int lowerIndex = idata->qi();
//     int lowerS = idata->qi();
//     int lowerE = idata->qi();
//     int lowerStrand = idata->qi();
//     int midIndex = idata->qi();
//     int midS = idata->qi();
//     int midE = idata->qi();
//     int midStrand = idata->qi();
//     int higherIndex = idata->qi();
//     int higherS = idata->qi();
//     int higherE = idata->qi();
//     int higherStrand = idata->qi();
//     int guessGene = idata->qi();
//     int af_length = idata->qi();
//     bool overlap = idata->qi();    // should be OK hmmm... 
//     cout << "making a blast Genome Match with stuff in it.. " << endl
// 	 << "lower start : " << lowerS << "\tlower end : " << lowerE
// 	 << "\tmid start : " << midS << "\tmid end : " << midE
// 	 << "\thigher start : " << higherS << "\thigher end : " << higherE << endl;
//     blastGenomeMatch tmatch(chromosome, lowerIndex, lowerS, lowerE, lowerStrand, midIndex, midS, midE, midStrand, higherIndex, higherS, higherE, higherStrand, guessGene, af_length, overlap);
//     ///////////////// OK lets read some annotation,,
//     while(idata->qi() == -1){                        // read as long as we have a -1,, the read an int, and two strings..
//       int selector = idata->qi();
//       string field = idata->s();
//       string term = idata->s();
//       switch(selector){
//       case 1:
// 	tmatch.lowerD.insert(make_pair(field, term));
// 	break;
//       case 2:
// 	tmatch.midD.insert(make_pair(field, term));
// 	break;
//       case 3:
// 	tmatch.higherD.insert(make_pair(field, term));
// 	break;
//       default:
// 	cout << "unknown selector, what's going on??? " << endl;
//       }
//     }
//     // note that we need to add an empty int in the the thing, as we have already read on in idata.. hmm. ugly, but what the hell.. 
//     int alignmentsSize = idata->qi();
   //   cout << "lower Index: " << lowerIndex << "\tmid index : " << midIndex << "\thigher index : " << higherIndex
//  	 << "\tguess : " << guessGene << "\taf_length : " << af_length << "\toverlap " << overlap
//  	 << "\talignments size : " << alignmentsSize << endl;
//     double minExpect = 1000; // the minimum expectation.. 
//     tmatch.alignments.resize(alignmentsSize);
//     for(int j=0; j < alignmentsSize; j++){
//       //cout << "\t\tj : " << j << endl;
//       int af_start = idata->qi();
//       int af_end = idata->qi();
//       int m_start = idata->qi();
//       int m_end = idata->qi();
//       double expect = idata->d();
//       if(expect < minExpect) { minExpect = expect; }
//       int a_length = idata->qi();
//       int matches = idata->qi();
//       int af_ns = idata->qi();
      
//       tmatch.alignments[j] = blastAlignment(af_start, af_end, m_start, m_end, expect, a_length, matches, af_ns);
//     }
//     //    tmatch.alignments = aligns;
//     tdata.genomeMatches.insert(make_pair(minExpect, tmatch));
//   }

  // lets get some session data stuff... well if thereis any !!!!!.. 
  // First session information.
  int sNo = idata->qi();
  //  cout << "number of Associated sessions " << sNo << endl;
  for(int i=0; i < sNo; i++){
    int index = idata->qi();
    int owner = idata->qi();
    string title = idata->s();
    //cout << "title : " << title << endl;
    string description = idata->s();
    int s = idata->qi();
    vector<string> kw(s);
    for(int j=0; j < s; j++){
      kw[j] = idata->s();
    }
    s = idata->qi();
    vector<int> m(s);
    for(int j=0; j < s; j++){
      m[j] = idata->qi();
    }
    tdata.sessions.push_back(sessionInformation(index, owner, title, description, kw, m));
  }
  int aNo = idata->qi();
  //  cout << "number of Associated annotations " << endl;
  for(int i=0; i < aNo; i++){
    int index = idata->qi();
    int owner = idata->qi();
    string annote = idata->s();
    //    cout << "Annotation : " << annote << endl;
    int s = idata->qi();
    vector<int> m(s);
    for(int j=0; j < s; j++){
      m[j] = idata->qi();
    }
    tdata.annotations.push_back(annotationInformation(index, owner, annote, m));
  }

  tdata.afdes = idata->s();
  tdata.tigrDescription = idata->s();
  int goSize = idata->qi();
  tdata.go.resize(goSize);
  for(int i=0; i < goSize; i++){
    int gogoSize = idata->qi();
    tdata.go[i].resize(gogoSize);
    for(int j=0; j < gogoSize; j++){
      tdata.go[i][j] = idata->s();
    }
  }
  string tailer = idata->s();
  if(tailer == string("<ProbeDataEnd>")){       // evertying is fine..
    pData = tdata;
    emit newPData(&pData);
    emit probeDataChanged();     // probably only need one of these, but will have to check
    //currentRegions = tempRegions;   // copy.. 
    //tempRegions.resize(0);          
    //emit newCurrentRegions(pSet.index, pData.ensemblBlastGuess);    // it's not necessarily true, but it seems reasonably likely!! 
    // the above are now done in a separate function, that is dedicated to this.. 
    return(true);
  }else{
    cerr << "didn't get the right tailer. tailer " << tailer << endl;
    return(false);
  }
  /// hmm, hope this works ok.. hmmm... 
}

bool Client::parseIndex(){
  //cout << "Beginning of Parse Index " << endl;
  vector<int> tIndex;
  int isize = idata->qi();
  tIndex.resize(isize);
  //cout << "size of index: " << isize << endl;
  for(int i=0; i < isize; i++){
    tIndex[i] = idata->qi();
    //cout << tIndex[i] << "   ";
  }
  //cout << endl << "read in a total of " << isize << " ints" << endl;
  string description = idata->s();
  string tailer = idata->s();
  if(tailer == string("<ProbeIndexEnd>")){
    setIndex(tIndex, description.c_str());
    //    currentIndex = tIndex;
    emit newIndex(currentIndex.size());        // we keep this separate as the clientWindow can also call the setIndex function.. 
    return(true);
  }else{
    return(false);
  }
}

bool Client::parseNewRegions(){        // not much just tells us how many regions there are.. 
  int regionNo = idata->qi();
  string tailer = idata->s();
  if(tailer != "<clientRegionsChangedEnd>"){
    cerr << "Parse New Regions got wrong tailer got : " << tailer << endl;
    return(false);
  }
  //cout << "New Region No is : " << regionNo << endl;
  emit newRegionIndex(regionNo);
  // and here we should set some selector to something..
}

bool Client::parseVitalStatistics(){
  vitalStatistics tempStats;
  tempStats.statNames.resize(idata->qi());
  //  vector<string> statNames(idata->qi());       // no checking. man this is so dangerous.. !!
  for(int i=0; i < tempStats.statNames.size(); i++){
    tempStats.statNames[i] = idata->s();
    //cout << "Stat Name : " << tempStats.statNames[i] << endl;
  }
  int statNo = idata->qi();
  tempStats.index.resize(statNo);
  tempStats.stats.resize(tempStats.statNames.size());
  for(int i=0; i < tempStats.stats.size(); i++){
    tempStats.stats[i].resize(statNo);
  }
  //vector<vitalStatistics> tempStats(statNo);
  for(int i=0; i < statNo; i++){
    tempStats.index[i] = idata->qi();
    for(int j=0; j < tempStats.stats.size(); j++){
      tempStats.stats[j][i] = idata->f();
      //cout << "Stat for j " << j << " : " << i << "\t" << tempStats.stats[j][i] << endl;
    }
  }
  //     tempStats[i].anovaScore = idata->f();
  //      tempStats[i].euclideanQuality = idata->f();
  //      tempStats[i].variationCoefficient = idata->f();
  //      //cout << "index : " << tempStats[i].index << "\tanova: " << tempStats[i].anovaScore << "\tvariance : " << tempStats[i].variationCoefficient << endl;
  string tailer = idata->s();
  if(tailer == "<statCollectionEnd>"){
    vitalStats = tempStats;
    emit newVitalStats();
    return(true);
  }
  return(false);
}

bool Client::parseSessionCreation(){
  int sessionId = idata->qi();
  string tailer = idata->s();
  if(tailer == "<sessionCreatedEnd>"){
    emit sessionCreated(sessionId);
    return(true);
  }
  return(false);
}

bool Client::parseSessionDescription(){
  int sessionNo = idata->qi();
  int sessionId;
  bool ownership;
  QString title;
  QString description;
  emit readingSessionWidgets();
  for(int i=0; i < sessionNo; i++){
    set<int> genes;
    set<QString> keywords;
    sessionId = idata->qi();
    ownership = (bool)idata->qi();
    title = idata->s().c_str();
    description = idata->s().c_str();
    int keywordNo = idata->qi();
    for(int j=0; j < keywordNo; j++){
      keywords.insert(idata->s().c_str());
    }
    int geneNo = idata->qi();
    for(int j=0; j < geneNo; j++){
      genes.insert(idata->qi());
    }
    // and now we can make a new thingy, and emit it. 
    SessionWidget* sWidget = new SessionWidget(sessionId, genes, keywords, title, description, ownership);
    emit newSessionWidget(sWidget);
  }
  string tail = idata->s();
  if(tail == "<sessionDescriptionEnd>"){
    return(true);
  }
  return(false);
}
  
bool Client::parseClusters(){
  bool localNorm = (bool)idata->qi();
  int exptNo = idata->qi();
  vector<int> expts(exptNo);
  for(int i=0; i < exptNo; i++){
    expts[i] = idata->qi();
  }
  int clusterNo = idata->qi();
  vector< vector<int> > indices(clusterNo);
  vector< vector< vector<float> > > points(clusterNo);
  vector< vector< vector<int> > > exptIndices(clusterNo);      // which anyway will always be equal to expts. now that is ugly. 
  vector< vector<float> > centers(clusterNo);
  vector< vector<int> > centExpts(clusterNo);                  //
  // ok now lets go through the clusters and see what we get..... 
  for(int i=0; i < clusterNo; i++){
    // first the cluster centers,, remember, exptNo
    centers[i].resize(exptNo);
    for(int j=0; j < exptNo; j++){
      centers[i][j] = idata->f();
    }
    centExpts[i] = expts;
    int clusterSize = idata->qi();
    indices[i].resize(clusterSize);
    points[i].resize(clusterSize);
    exptIndices[i].resize(clusterSize);
    for(int j=0; j < clusterSize; j++){
      indices[i][j] = idata->qi();
      exptIndices[i][j] = expts;  // hmm whoaa..
      points[i][j].resize(exptNo);
      for(int k=0; k < exptNo; k++){
	points[i][j][k] = idata->f();
      }
    }
  }
  string tailer = idata->s();
  if(tailer == "<KClusterEnd>"){
    //cout << "seems that we have read in a KCluster set alright, will now set about actually inserting it into the main structures.. " << endl;
    clusterSet* tempSet = new clusterSet(indices, points, exptIndices, centers, centExpts, &expInfo, localNorm);
    clusterSets.insert(tempSet);
    emit newClusterSet(tempSet);
    return(true);
  }
  //cout << "Parse Clusters the tailer is wrong, doing nothing. tailer is " << tailer << endl;
  return(false);
}

void Client::parseExperimentTrace(){
  cout << "parsing experiment Trace.. " << endl;
  vector<tracePoint> points;
  while(idata->qi() == 1){    // as long as it returns a 1..
    int id = idata->qi();
    float d = idata->f();
    float x = idata->f();
    float y = idata->f();
    points.push_back(tracePoint(id, d, x, y));
  }
  string tailer = idata->s();
  if(tailer == "<ExperimentTraceEnd>"){
    emit newExperimentTrace(points);
    return;
  }
  cerr << "parseExperimentTrace got invalid tailer value is : " << tailer << endl;
}


void Client::parseExperimentDistances(bool isFlat){
  //cout << "This is in the parseExperiment Distances thingy.." << endl;
  int geneNo = idata->qi();
  float sigma, order;
  if(isFlat){
    sigma = idata->f();
    order = idata->f();
  }
  int exptNo = idata->qi();
  vector< vector<float> > distances(exptNo);
  vector<int> expts(exptNo, 0);
  for(int i=0; i < exptNo; i++){
    distances[i].resize(exptNo);
    for(int j=0; j < exptNo; j++){
      distances[i][j] = 0;
    }
  }
  for(int i=0; i < exptNo; i++){
    expts[i] = idata->qi();
    for(int j=i+1; j < exptNo; j++){
      distances[i][j] = idata->f();
      distances[j][i] = distances[i][j];
    }
  }
  // which should be everything I really need to do.. but at this point I should use this information..
  // let's just print it out first..
//   cout << "Experimental distances : " << endl;
//   for(int i=0; i < distances.size(); i++){
//     cout << expts[i];
//     for(int j=0; j < distances.size(); j++){
//       cout << "\t" << distances[i][j];
//     }
//     cout << endl;
//   }
//  cout << "Client : expt size is : " << expts.size() << endl
  //     << "Client : distances size is : " << distances.size() << endl;
  exptDistanceInfo distinfo(expts, distances);
  distinfo.geneNo = geneNo;
  distinfo.isFlat = isFlat;
  if(isFlat){
    distinfo.sigma = sigma;
    distinfo.order = order;
  }
  emit newExperimentalDistances(distinfo); // ok !! just handle in clientWindow and everything should be ok.
  //  DistanceMapper* mapper = new DistanceMapper(expts, distances, 2, &testMutex);
  //cout << "Starting Mapper,, end of the road " << endl;
  //mapper->start();
}

bool Client::parseGenomicRegionProbeSetMatches(){
  string chrom = idata->s();
  int start = idata->qi();
  int end = idata->qi();
  int cSize = idata->qi();
  //cout << "parsing genomic region probe set matches " << endl;
  vector<probeSetMatch> matches;
  while(idata->qi() == 1){
    //cout << "found a probe set match with some parameters of sorts" << endl;
    int cStart = idata->qi();
    int cEnd = idata->qi();
    int afIndex = idata->qi();
    int afStart = idata->qi();
    int afEnd = idata->qi();
    int afLength = idata->qi();
    int alignLength = idata->qi();
    //int af_n_count = idata->qi();
    int af_n_count = 0;
    int match = idata->qi();
    double expect = idata->d();
    // and that then is it..
    //string afid = "null";
    int strand = 1;
    matches.push_back(probeSetMatch(cStart, cEnd, afIndex, chrom, afStart, afEnd, afLength, alignLength, match, strand, expect));
  }
  // idata->qi() has already read in the the thing so let's see if the tailer is ok
  string tail = idata->s();
  if(tail != "<chromRegionProbeSetMatchesEnd>"){
    cout << "tailer is no good, returning false : tailer : " << tail << endl;
    return(false);
  }
  // and now let's leak memory all over the place,,
  chromRegion* region = new chromRegion(chrom, cSize, start, end, matches);
  chromosomalRegions.insert(region);   // seems very dangerous...
  emit newChromRegion(region);
  // cout << "emitting newChromRegion Thingy" << endl;
  return(true);
}

bool Client::parseGenomicRegionEnsemblGenes(){
  return(false);
  ///////// This function should be replaced as it's not particularly useful.. and we want to use different sets of things.. 

//   int requestId = idata->qi();
//   string chromosome = idata->s();
//   int start = idata->qi();
//   int end = idata->qi();
//   int size = idata->qi();   // we may not use this, but what the hell.
  
//   vector<ensemblGene> genes;  // we'll pack into this one.. and just emit it later.. and see if anyone wants it..
//   while(idata->qi() == 1){
//     int dbi = idata->qi();
//     string ensid = idata->s();

//     string extid = "";
//     string des = "";
//     //string extid = idata->s();
//     //string des = idata->s();
    
//     string chr = idata->s();
//     int gStart = idata->qi();
//     int gEnd = idata->qi();
//     bool strand = (idata->qi() == 1);   // how's that??
//     genes.push_back(ensemblGene(dbi, ensid, chr, gStart, gEnd, strand));
//   }
//   string tail = idata->s();
//   if(tail != "<chromRegionEnsemblGenesEnd>"){
//     cout << "tail is no good, what is meant by " << tail << endl;
//     return(false);
//   }
//   set<chromRegion*>::iterator it = chromosomalRegions.find((chromRegion*)requestId);
//   if(it == chromosomalRegions.end()){
//     cout << "couldn't find a chromosomal region with address " << requestId << endl;
//     return(false);
//   }
//   /// rather brutally...
//   if((*it)->chromosome == chromosome && (*it)->chromStart == start && (*it)->chromEnd == end){
//     (*it)->ensGenes = genes;
//     emit newEnsemblGeneRegion();
//     return(true);
//   }
//   cout << "region paramters don't seem to match, and getting out of here, byby,, " << endl;
//   return(false);
}


bool Client::parseGenomicRegionHeader(){
     // just contains two numbers,,, -- one the target, which now is just one or two.. 1 for probe set region, 2 for ish probe region
  int target = idata->qi();
  int association = idata->qi();
  string tailer = idata->s();
  if(tailer != "<sendingRegionsEnd>"){
    cerr << "parseGenomicRegion Header obtained incorrect region tailer, returning false  tailer : " << tailer << endl;
    return(false);
  }
  /// we have some choices.. if the currentRegionAssociation is not 0, it should indicate that we are currently receiving a 
  // a set of regions for another association.. if this is the case, then it is not entirely clear what we should be doing. My 
  // guess it that we should probably resize the tempregions to 0, and start afresh, as this would seem to be the more stable behaviour..
  // i.e. it will not fail if we should have a problem in receiving a thing before.. OK.. 
  if(target == 1 && association > 0){
    currentRegionAssociation = association;
    tempRegions.resize(0);
    return(true);
  }
  if(target == 2 && association > 0){
    currentIshRegionAssociation = association;
    tempIshRegions.resize(0);
    return(true);
  }
  cerr << "obtained strange results in parse genomic region header target : " << target << "  association : " << association << endl;
  return(false);
}

bool Client::parseGenomicRegionTailer(){
  int target = idata->qi();
  int association = idata->qi();
  string tailer = idata->s();
  if(tailer != "<finishedSendingRegionsEnd>"){
    cerr << "parseGenomicRegionTailer obtained wrong tail : " << tailer << endl;
    return(false);
  }
  if(target == 1){
    if(association == currentRegionAssociation && association == pData.index){   
      /// everything is great, let's swap, emit and get out of here..
      currentRegions = tempRegions;
      tempRegions.resize(0);
      emit newCurrentRegions(association, pData.ensemblBlastGuess);
      return(true);
    }
  }
  if(target == 2){
    if(association == currentIshRegionAssociation){
      currentIshRegions = tempIshRegions;
      tempIshRegions.resize(0);
      emit newCurrentIshRegions(association, 0);   // we don't have a blast guess for this yet.. -- maybe we should ..
      return(true);
    }
  }
  return(false);
}
    

bool Client::parseGenomicRegion(){
  // this is a bit strange really, but what the hell. 
  // what we are looking to make is a vector of genomicRegions ,, and if they are correct then we do something else..
  //
  //  --- ahh, actually we make one chromRegion, which we then append to the currentProbeSet.. 
  // the chromRegion vector should be emptied each time we get a new probe Set, but we can look at that later on..
  /// hmm. hmm. hmmm.. 
  //cout << "parsing Genomic Region " << endl;
  int target = idata->qi();
  int association = idata->qi();
  if(target == 1 && association != pData.index){
    //  if(target == 1 && association != pSet.index){
    cerr << "parseGenomicRegion : association is not the same as the current pSet->index, returning false" << endl;
    return(false);
  }
  string chromosome = idata->s();
  int chromStart = idata->qi();
  int chromEnd = idata->qi();
  int chromSize = idata->qi();
  int maxF = 0;
  int maxR = 0;
  // make a temp region..
  chromRegion tempRegion(chromosome, chromStart, chromEnd, chromSize);

  //vector<probeSetMatch> tempMatches(0);
  //vector<ensemblGene> tempGenes(0);
  //map<int, ishProbeMatchSet> tempIshMatches;
  //cout << "read in the first information" << endl;
  //cout << "parsing ensembl genes : " << endl;
  while(idata->qi() == 1){
    cout << "Receiving Ensembl Gene : " << endl;
    int gIndex = idata->qi();
    string ensId = idata->s();
    int gStart = idata->qi();
    int gEnd = idata->qi();
    int gStrand = idata->qi();
    int transcriptNo = idata->qi();
    if(gStrand > 0){
      if(transcriptNo > maxF){ maxF = transcriptNo; }
    }else{
      if(transcriptNo > maxR){ maxR = transcriptNo; }
    }
    ensemblGene tempGene(gIndex, ensId, chromosome, gStart, gEnd, (gStrand > 0));
    multimap<string, string> tmpAnnotation;
    //cout << "just made an ensemblGene" << endl;
    for(int i=0; i < transcriptNo; i++){
      int transcriptIndex = idata->qi();
      string transcriptId = idata->s();
      int transcriptStart = idata->qi();
      int transcriptEnd = idata->qi();
      int exonNo = idata->qi();
      ensemblTranscript tempTranscript(transcriptIndex, transcriptId, transcriptStart, transcriptEnd, gStrand, gIndex);
      for(int j=0; j < exonNo; j++){
	string exonId = idata->s();
	int exonStart = idata->qi();
	int exonEnd = idata->qi();
	int exonCodeStart = idata->qi();
	int exonCodeEnd = idata->qi();
	tempTranscript.addExon(ensemblExon(exonId, exonStart, exonEnd, exonCodeStart, exonCodeEnd, gStrand));
	//	tempTranscript.exons.push_back(ensemblExon(exonId, exonStart, exonEnd, exonCodeStart, exonCodeEnd, gStrand));
	//cout << "\t\tpushing back exons for the gene" << endl;
      }
      tempGene.transcripts.push_back(tempTranscript);
    }
    while(idata->qi() == 2){
      string field = idata->s();
      string term = idata->s();
      //cout << "field : " << field << "\t:" << term << endl;
      tempGene.annotation.insert(make_pair(field, term));
    }
    tempRegion.addEnsGene(tempGene);
    //    tempGenes.push_back(tempGene);
  }
  while(idata->qi() == 3){
    string source = idata->s();
    string id = idata->s();
    string chromosome = idata->s();   // kind of redundant I know, but might be useful.. 
    int cstart = idata->qi();
    int cend = idata->qi();
    int strand = idata->qi();
    int tlength = idata->qi();
    Transcript tempTranscript(source, id, chromosome, cstart, cend, strand, tlength);
    //cout << "\t\t RECEived transcript id : " << id << "\tstart : " << cstart << "\tend : " << cend << endl;
    int exonNo = idata->qi();
    for(int i=0; i < exonNo; i++){
      int start = idata->qi();
      int end = idata->qi();
      int estart = idata->qi();
      int eend = idata->qi();
      //cout << "\t\tRecevied exon start : " << start << "\tend : " << end << endl; 
      tempTranscript.addExon(Exon(start, end, estart, eend));
    }
    tempRegion.addTranscript(tempTranscript);
  }
  // and that should be it... 
  /// and now we go to the probeSet matches, they are a little bit easier after all....
  while(idata->qi() == 1){
    int afIndex = idata->qi();
    int cStart = idata->qi();
    int cEnd = idata->qi();
    int afStart = idata->qi();
    int afEnd = idata->qi();
    int afLength = idata->qi();
    int alignLength = idata->qi();
    int match = idata->qi();
    double expect = idata->d();
    int strand = idata->qi();
    tempRegion.addPMatch(probeSetMatch(cStart, cEnd, afIndex, chromosome, afStart, afEnd, afLength, alignLength, match, strand, expect));
    //    tempMatches.push_back(probeSetMatch(cStart, cEnd, afIndex, chromosome, afStart, afEnd, afLength, alignLength, match, strand, expect));
    //cout << "making a probeSetMatch" << endl;
  }
  // and now for the in situ probe set matches.. they are very similar to the above.. should probably do by polymorphism or something..
  while(idata->qi() == 1){
    int probeIndex = idata->qi();
    int minpos = idata->qi();
    int maxpos = idata->qi();
    int plength = idata->qi();
    float score = idata->qi();
    int strand = idata->qi();
    string chromosome = idata->s();
    int assemblyid = idata->qi();
    int matchNo = idata->qi();
    ishProbeMatchSet tset(probeIndex, minpos, maxpos, plength, score, strand, chromosome, assemblyid);
    for(int i=0; i < matchNo; i++){
      int pstart = idata->qi();
      int pend = idata->qi();
      int cstart = idata->qi();
      int cend = idata->qi();
      float percent = idata->f();
      tset.addMatch(ishProbeMatch(pstart, pend, cstart, cend, percent));
    }
    tempRegion.addIshMatch(tset);
  }
  // hmmmm, should be ok.. perhaps.. 
  //     int probeIndex = idata->qi();
  //     int pStart = idata->qi();
  //     int pEnd = idata->qi();
  //     int pLength = idata->qi();
  //     int alignLength = idata->qi();
  //     int matchLength = idata->qi();
  //     int cStart = idata->qi();
  //     int cEnd = idata->qi();
  //     double expect = idata->d();
  //     int strand = idata->qi();
  //     string chromosome = idata->s();
  //     // check if we've already seen this ish probe,, 
  //     //cout << endl << "ADDING ISH PROBE MATCH TO A CHROMOSMAL REGION START : " << cStart << " END " << cEnd << " and index : " << probeIndex << endl;
  //     tempRegion.addIshMatch(ishProbeMatch(probeIndex, pStart, pEnd, pLength, alignLength, matchLength, cStart, cEnd, expect, strand, chromosome));
  //    map<int, ishProbeMatchSet>::iterator it = tempIshMatches.find(probeIndex);
  //     if(it == tempIshMatches.end()){
  //       tempIshMatches.insert(make_pair(probeIndex, ishProbeMatchSet(ishProbeMatch(probeIndex, pStart, pEnd, pLength, alignLength, matchLength, cStart, cEnd, expect, strand, chromosome))));
  //     }else{
  //       (*it).second.addMatch(ishProbeMatch(probeIndex, pStart, pEnd, pLength, alignLength, matchLength, cStart, cEnd, expect, strand, chromosome));
  //     }
  // 
  string tailer = idata->s();
  if(tailer != "<chromRegionEnd>"){
    cerr << "wrong tailer returning false from parsing genomic region tailer : " << tailer << endl;
    return(false);
  }
  // push_back localRegions..
  //currentRegions.push_back(chromRegion(chromosome, chromStart, chromEnd, chromSize, maxF, maxR, tempMatches, tempGenes));
  if(target == 1){
    tempRegions.push_back(tempRegion);
    //    tempRegions.push_back(chromRegion(chromosome, chromStart, chromEnd, chromSize, maxF, maxR, tempMatches, tempGenes, tempIshMatches));
  }
  if(target == 2){
    //    tempIshRegions.push_back(chromRegion(chromosome, chromStart, chromEnd, chromSize, maxF, maxR, tempMatches, tempGenes, tempIshMatches));
    tempIshRegions.push_back(tempRegion);
  }
  if(target == 3){    // the region viewer,,
    genRegions[0] = tempRegion;
    emit newRegion(0, 0);
  }
  //emit newCurrentRegions(pSet.index);  -- do after probeData has been received instead.. 
  //cout << "pushed back current Regions, size is now : " << currentRegions.size() << endl;
  return(true);
} 

bool Client::parseThumbnail(){
  int ishProbeIndex = idata->qi();
  int imageIndex = idata->qi();
  int dataLength = idata->qi();
  QByteArray data(dataLength);
  for(int i=0; i < dataLength; i++){
    data[i] = idata->c();   // should be a safe method .. if not enough, then sends '0'  -- not much.. could try \0..
  }
  string terminator = idata->s();
  if(terminator == "<ishThumbNailEnd>"){
    emit newIshThumbnail(ishImageData(data, ishProbeIndex, imageIndex));
    //cout << "emitted an ishImageData to the world to see... " << endl;
    return(true);
  }
  cerr << "wrong terminator for the parseThumbnail function: got : " << terminator << endl;
  return(false);
}

bool Client::parseWords(vector<QString>& words, string tail){
  int n = idata->qi();
  words.resize(n);    // obviously an easy way to get the client to fall over..
  for(int i=0; i < n; i++){
    words[i] = idata->s().c_str();
  }
  string tailer = idata->s();
  if(tailer == tail){
    return(true);
  }
  return(false);
}

bool Client::parseProtocolCollection(){
  /// parse the data, and make a protocol collection.. 
  map<int, Protocol*> prots;
  int requesterId = idata->qi();
  int requestId = idata->qi();
  //cout << "request id is " << requestId << endl;
  int protocolNumber = idata->qi();
  ///////// I really should include some sanity checking along the way .. server certainly can crash the client at the moment..
  //cout << "protocol number is : " << protocolNumber << endl;
  for(int i=0; i < protocolNumber; i++){
    int protocolId = idata->qi();
    uint time = (uint)idata->qi();
    int parentId = idata->qi();
    int creatorId = idata->qi();
    string creatorName = idata->s();
    string protocolName = idata->s();
    string protocolDescription = idata->s();
    cout << "protocol id " << protocolId << endl
	 << "time        " << time << endl
	 << "parent id   " << parentId << endl
	 << "creator Id  " << creatorId << endl
	 << "protocolName " << protocolName << endl
	 << "description  " << protocolDescription << endl;
    // make the thingy...
    //cout << "making protocol with id : " << protocolId << " and name " << protocolName << endl;
    //    cout << "Client.cpp making a protocol with a name of " << protocolName << endl;
    Protocol* protocol = new Protocol(protocolId, parentId, time, creatorId, creatorName, protocolName, protocolDescription);
    int stepNo = idata->qi();
    for(int j=0; j < stepNo; j++){
      int stepId = idata->qi();
      int pid = idata->qi();
      int cid = idata->qi();
      string cname = idata->s();
      string sdescription = idata->s();
      uint ctime = (uint)idata->qi();
      protocol->steps.push_back(new ProtocolStep(stepId, pid, ctime, cid, cname, sdescription));
    }
    /// which should work as long as we all play according to the rules.. -- please start using sanity checking at some point..
    prots.insert(make_pair(protocol->id, protocol));
  }
  // and now we just make a protocolCollection.. and emit it to somewhere..... 
  ProtocolCollection* collection = new ProtocolCollection(prots);
  /// check if we have the correct tailer.. 
  string tailer = idata->s();
  if(tailer != "<ProtocolCollectionEnd>"){
    cerr << "parse protocol collection received wrong tailer ,, will delete the collection : got " << tailer << endl;
    delete collection;
    return(false);
  }
  //// Note that destruction of this will have to be taken care of by the receiver, which if there are multiple
  //// receivers could cause havoc. Use with care.. 
  //cout << "emitting the address of a collection hoo yeahh" << endl;
  //cout << "emitting newProtocolCollection" << endl;
  emit newProtocolCollection(requesterId, requestId, collection);
  return(true);
}

      
bool Client::parseProtocol(){
  int protocolId = idata->qi();
  int requestId = 0;
  if(protocolId == -1){
    // not real id, but an identifer..
    requestId = idata->qi();
    protocolId = idata->qi();    // follows on from after the thing.. 
  }
  uint time = (uint)idata->qi();
  int parentId = idata->qi();
  int creatorId = idata->qi();
  string creatorName = idata->s();
  string protocolName = idata->s();
  string protocolDescription = idata->s();
  // make the thingy...
  //cout << "making protocol with id : " << protocolId << " and name " << protocolName << endl;
  Protocol* protocol = new Protocol(protocolId, parentId, time, creatorId, creatorName, protocolName, protocolDescription, requestId);
  int stepNo = idata->qi();
  for(int j=0; j < stepNo; j++){
    int stepId = idata->qi();
    int pid = idata->qi();
    int cid = idata->qi();
    string cname = idata->s();
    string sdescription = idata->s();
    uint ctime = (uint)idata->qi();
    protocol->steps.push_back(new ProtocolStep(stepId, pid, ctime, cid, cname, sdescription));
  }
  string tailer = idata->s();
  if(tailer != "<ProtocolEnd>"){
    cerr << "got the wrong tailer for the protocol, deleting and going home " << endl;
    delete protocol;
    return(false);
  }
  emit newProtocol(protocol);
  return(true); 
}

bool Client::parseExperiment(){
  int requestId = idata->qi();
  int expId = idata->qi();
  int userId = idata->qi();
  string userName = idata->s();
  QDateTime expTime, enTime;
  expTime.setTime_t((uint)idata->qi());
  enTime.setTime_t((uint)idata->qi());
  int protId = idata->qi();
  string protName = idata->s();
  string protDes = idata->s();
  string comment = idata->s();
  // and let's make an experiment and emit it with the request id.. and see how it get's recieved..
  string tailer = idata->s();
  if(tailer != "<ExperimentEnd>"){
    cout << "parseExperiment tailer is wrong, is : " << tailer << endl;
    return(false);
  }
  Experiment expt(expId, userId, userName, expTime, enTime, protId, protName, protDes, comment);
  emit newExperiment(requestId, expt);
  return(true);
  
}

void Client::parseExperiments(){
  int exptNo = idata->qi();
  vector<Experiment> expts(exptNo);     /// DANGEROUS.. -- could crash the client easily.. if we have a misread number.. 
  for(int i=0; i < exptNo; i++){
    int expId = idata->qi();
    int userId = idata->qi();
    string userName = idata->s();
    QDateTime expTime, enTime;
    expTime.setTime_t((uint)idata->qi());
    enTime.setTime_t((uint)idata->qi());
    int protId = idata->qi();
    string protName = idata->s();
    string protDes = idata->s();
    string comment = idata->s();
    expts[i] = Experiment(expId, userId, userName, expTime, enTime, protId, protName, protDes, comment);
  }
  string tailer = idata->s();
  if(tailer != "<ExperimentCollectionEnd>"){
    cerr << "parseExperiments got wrong tailer : " << tailer << endl;
    return;
  }
  emit newExperiments(expts);
}

void Client::parseTissues(){
  int tissueNo = idata->qi();
  vector<tissue> tissues(tissueNo);
  for(int i=0; i < tissueNo; i++){
    int ti = idata->qi();
    string t = idata->s();
    float a = idata->f();
    tissues[i] = tissue(ti, t, a);
  }
  string tailer = idata->s();
  if(tailer != "<TissuesEnd>"){
    cerr << "parseTissues received wrong tailer " << endl;
    return;
  }
  emit newTissues(tissues);
}

void Client::parseIshAnnotationFields(){
  int fieldNo = idata->qi();
  vector<ishAnnotationField> fields(fieldNo);
  for(int i=0; i < fieldNo; i++){
    int index = idata->qi(); 
    QString name = idata->s().c_str();    // ugly,, soo ugly.. 
    fields[i] = ishAnnotationField(index, name);
  }
  string tailer = idata->s();
  if(tailer != "<IshAnnotationFieldsEnd>"){
    cerr << "parseIshAnnotationFields got wrong tailer : " << tailer << endl;
    return;
  }
  emit newIshAnnotationFields(fields);
}
  
bool Client::parseIshTextFields(){
  vector<QString> fields;    // obviously an easy way to get the server to fall over..
  if(!parseWords(fields, "<ishTextFieldsEnd>")){
    cerr << "Couldn't get the ishTextFieldEnd tail  or some other problem " << endl;
    return(false);
  }
  //cout << "am emitting ishTextFields size : " << fields.size() << endl;
  emit ishTextFields(fields);
  return(true);
}

bool Client::parseIshFloatFields(){
  vector<QString> fields;
  if(!parseWords(fields, "<ishFloatFieldsEnd>")){
    cerr << "Couldn't parse ishFloatFields.. " << endl;
    return(false);
  }
  emit ishFloatFields(fields);
  return(true);
}

bool Client::parseIshClasses(){
  vector<QString> classes;
  if(!parseWords(classes, "<ishClassesEnd>")){
    cerr << "Couldn't parse ish classes end" << endl;
    return(false);
  }
  emit ishClasses(classes);
  return(true);
}

bool Client::parseIshProbeData(){
  int index = idata->qi();
  string consSequence = idata->s();
  string antiSenseProm = idata->s();
  int afIdAssociation = idata->qi();
  string vectorName = idata->s();
  int designLength = idata->qi();
  string probeName = idata->s();
  string probeIdentifier = idata->s();
  int ensemblGuess = idata->qi();
  // make an ishProbeData and then check if we have any annotation to go with it.
  ishProbeData data(index, consSequence, antiSenseProm, afIdAssociation, vectorName, designLength, probeName, probeIdentifier, ensemblGuess);
  int n = idata->qi();     // this one could really screw up if the number is completely out of here.. but let's have some faith for now..
  for(int i = 0; i < n; i++){
    QString field = idata->s().c_str();
    int annotationId = idata->qi();
    int userId = idata->qi();
    string userName = idata->s();
    QString note = idata->s().c_str();    // ohh so ugly. 
    dirtyText(note);
    dirtyText(field);
    data.textAnnotation.insert(make_pair(field.latin1(), ish_annotation(annotationId, userId, userName, note, field, index)));
    //cout << "getting text annotation for thingy,, " << note << endl;
  }
  n = idata->qi();
  for(int i=0; i < n; i++){
    QString field = idata->s().c_str();
    int annotationId = idata->qi();
    int userId = idata->qi();
    string userName = idata->s();
    float value = idata->f();       // this does seem rather fragile.. and ugly, and so on.
    dirtyText(field);
    data.numberAnnotation.insert(make_pair(field.latin1(), ish_annotation(annotationId, userId, userName, value, field, index)));
  }
  n = idata->qi();
  for(int i=0; i < n; i++){
    QString field = idata->s().c_str();
    int annotationId = idata->qi();
    int userId = idata->qi();
    string userName = idata->s();
    float value = idata->f();
    dirtyText(field);
    //cout << "parsing an ish classification and the confidence is : " << value << "  for class : " << field  <<  endl;
    data.classification.insert(make_pair(field.latin1(), ish_annotation(annotationId, userId, userName, value, field, index)));
  }
  string tailer = idata->s();
  if(tailer != "<ishProbeDataEnd>"){
    cerr << "parseIshProbeData : wrong tailer got " << tailer << endl;
    return(false);
  }
  // emit some signal and return true..
  //cout << "\n\n\n\t\tParsed an ISHPROBEDATA\n\n\n" << endl;
  emit newIshProbeData(data);     // should be ok, pass it on via clientWindow to inSituView.. 
  return(true);
}
    

bool Client::parseIshImage(){
  //cout << "parsing ish image what's up" << endl;
  int ishProbeIndex = idata->qi();
  int imageIndex = idata->qi();
  int dataLength = idata->qi();
  QByteArray data(dataLength);
  for(int i=0; i < dataLength; i++){
    data[i] = idata->c();
  }
  string terminator = idata->s();
  if(terminator == "<ishFullImageEnd>"){
    //cout << "emmiting newIshImage " << endl;
    emit newIshImage(ishImageData(data, ishProbeIndex, imageIndex));
    return(true);
  }
  cerr << "wrong terminator for parseIshImage got: " << terminator << endl;
  return(false);
}

void Client::parseSequence(){
  int requestId = idata->qi();
  //cout << "request Id is " << requestId << endl;
  map<int, SequenceRequest*>::iterator mit = sequenceRequests.find(requestId);
  if(mit == sequenceRequests.end()){
    cout << "Couldn't find a sequence request with id " << requestId << endl;
    return;
  }
  int sequenceType = idata->qi();
  if(sequenceType == 0){
    string afid = idata->s();
    int afIndex = idata->qi();
    set<int>::iterator it = (*mit).second->probeSetRequests.find(afIndex);
    if(it == (*mit).second->probeSetRequests.end()){
      cout << "Doesn't look like I'm looking for this probe set : " << afid << endl;
      return;
    }
    string sequence = idata->s();
    string terminator = idata->s();
    if(terminator != "<dnaSequenceEnd>"){
      cout << "parseSequence : probe set sequence wrong terminator : " << terminator << endl;
      return;
    }
    string description = string("Sequence for Probe Set : ") + afid;
    (*mit).second->sequences.insert(make_pair(0, dnaSequence(afid, description, sequence)));
    // then remove from the probe Set requests..
    (*mit).second->probeSetRequests.erase(it);
    //    (*mit).second->probeSetRequests.erase(afid);
    //cout << "received and incorporated the sequence for probe set " << afid << endl
    // << "sequence : " << sequence << endl;
    //    cout << "received a probe set sequence, but don't know how to handle yet, returning" << endl;
    //return;
  }
  if(sequenceType == 1){
    //cout << "received a transcript sequence, don't know how to handle " << endl;
    int genIndex = idata->qi();
    // check if we are actually looking for this one..
    set<int>::iterator it = (*mit).second->transcriptRequests.find(genIndex);
    if(it == (*mit).second->transcriptRequests.end()){
      cout << "Doesn't seem like we are looking for this request,, funny that" << endl;
      return;
    }
    int tnum = idata->qi();  // the number of sequences being sent..
    for(int i=0; i < tnum; i++){
      string name = idata->s();
      string chromosome = idata->s();
      int startPos = idata->qi();
      string strand = idata->s();
      string sequence = idata->s();
      // ok, we now have everything, and all we have to do is create a name and description..
      ostringstream description;
      description << "Transcript no: " << i+1 << " for Ensembl transcript index : " << genIndex
		  << "   chromosome : " << chromosome << "  start position : " << startPos << "  strand : " << strand;
      (*mit).second->sequences.insert(make_pair(1, dnaSequence(name, description.str(), sequence)));
      //cout << "Transcript sequence : " << endl << sequence << endl;
    }
    // and the last bit we have to do is to remove the thingy..
    (*mit).second->transcriptRequests.erase(it);
    //    (*mit).second->transcriptRequests.erase(genIndex);
    //cout << "Seems like we stuffed a transcript sequence into the struct, see how this works" << endl; 
  }
  if(sequenceType == 2){
    cout << "received a genomic sequence, let's parse it and see if we can do something nice" << endl;
    string chromosome = idata->s();
    int start = idata->qi();
    int length = idata->qi();
    // to be more efficient, check whether we actually requested this sequence or not
    set<genomicRequest>::iterator it;
    
    for(it = (*mit).second->genomeRequests.begin(); it != (*mit).second->genomeRequests.end(); it++){
      if((*it).chromosome == chromosome && (*it).start == start){
	break;
      }
    }
    if(it == (*mit).second->genomeRequests.end()){   /// think this works. 
      //cout << "Although we have a sequence request with this id, the genomic sequence does not appear wanted" << endl;
      return;
    }
    // so now mit is the SequencRequest,, it is the genomic request.. and so on.. we can now try to do something clever.
    
    string sequence = idata->s();
    // check to make sure the terminator is OK..
    string terminator = idata->s();
    if(terminator != "<dnaSequenceEnd>"){
      //cout << "Wrong terminator got : " << terminator << endl;
      return;
    }
    
    //cout << "Got the end of the terminator sequence, everything OK, now for the tricky bit.. " << endl;
    //cout << "sequence is " << sequence << endl;
    //cout << "chromosome is " << chromosome << endl;
    //cout << "start is " << start << endl;
    //cout << "length is " << length << endl;
    //cout << "and just to confirm that  sequence.size() returns " << sequence.size() << endl;
    // basically what I need to do is to make a sequence object, stick it in the appropriate place, and 
    // then remove the genomic request from the set.. 
    ostringstream geneId;
    geneId << chromosome << "." << start << "-" << start+length;
    ostringstream geneDescription;
    geneDescription << "Genomic sequence from chromosome " << chromosome
		    << "  start bp: " << start << "  end bp: " << start +length;
    (*mit).second->sequences.insert(make_pair(2, dnaSequence(geneId.str(), geneDescription.str(), sequence)));
    cout << "just before calling genome requests to erase the current one" << endl;
    //genomicRequest temp = (*it);
    (*mit).second->genomeRequests.erase(it);
    //    (*mit).second->genomeRequests.erase((*it));
    //cout << "and just after calling genome requests to erase the last one" << endl;
    // and that really should be it for the genome request..
  }
  if(sequenceType == 4){   // a peptide sequence.. we don't actually get very much in terms of things..
    int geneIndex = idata->qi();
    // check if we actually have the things.. 
    set<int>::iterator it = (*mit).second->peptideRequests.find(geneIndex);
    if(it != (*mit).second->peptideRequests.end()){
      int pNum = idata->qi();
      for(int i=0; i < pNum; i++){
	string pName = idata->s();
	int gI = idata->qi();  // kind of redundant.. 
	string sequence = idata->s();
	ostringstream description; 
	description << "Peptide sequence for transcript " << gI << " Ensembl peptide : " << pName;
	(*mit).second->sequences.insert(make_pair(4, dnaSequence(pName, description.str(), sequence)));
      }
      (*mit).second->peptideRequests.erase(it);
    }
  }
  if((*mit).second->probeSetRequests.size() + (*mit).second->transcriptRequests.size() + (*mit).second->genomeRequests.size() + (*mit).second->peptideRequests.size() == 0){
    // everything is happy. lets call it to write the file,,
      if(!(*mit).second->displayOnly){
	  (*mit).second->saveData();
      }else{
	  seqDisplay->setText((*mit).second->seq().c_str());
//	  cout << "Sequences : " << endl << (*mit).second->seq() << endl;
      }
      SequenceRequest* temp = (*mit).second;
      sequenceRequests.erase((*mit).first);
      cout << "erased the sequence Request from the struct" << endl;
      cout << "now try to delete it, let's see what happens. " << endl;
      delete temp;
      cout << "after deletion, maybe I need to write something like a destructor" << endl;
  }
  
}



bool Client::parseDBChoices(){
  int target = idata->qi();          // probe set tables or region choices.. 1, 2 respectively.. 
  int dbsize = idata->qi();
  vector<string> tdb(dbsize);   // temp dbchoices..
  for(int i=0; i < dbsize; i++){
    tdb[i] = idata->s();
  }
  cout << "target is : " << target << endl;
  string tailer = idata->s();
  if(tailer == string("<DBChoicesEnd>")){
    //dbChoices = tdb;
    switch(target){
    case 1:
      emit newDBChoices(tdb);
      break;
    case 2:
      cout << "emitting new Region Chocies " << endl;
      emit newRegionChoices(tdb);
      break;
    default :
      cerr << "Unknown table query choice, target is : " << target << endl;
    }
    return(true);
  }else{
    cerr << "Got wrong tail thingy .. " << tailer << endl;
    return(false);
  }
}
    
void Client::acknowledgeReceipt(int n){
  if(open){
    *t << "<Received>" << n << ">";
  }else{
    emit statusMessage(QString("couldn't acknowledge receipt"));
  }
}

void Client::collectSequences(SequenceRequest* seqr){
  //cout << "Hey, this is the client object, I got a sequence request. groovy baby.. " << endl;
  // let's see what's in the sequence request and then let's see what we can do..
  // first of all 

  if(!open) { return; }
  sequenceRequests.insert(make_pair(seqr->id, seqr));  // hooo yeaaah. 

  //cout << "Sequence Request Id is " << seqr->id << endl;
  set<int>::iterator sit;
  for(sit = seqr->probeSetRequests.begin(); sit != seqr->probeSetRequests.end(); sit++){
    // send the message.. 
    //cout << "And the first probe set wanted is " << (*sit) << endl;
    *t << "<getProbeSetSequence>" << (*sit) << "|" << seqr->id << "|>";
    if(recordCommands){
      *commandRecord << "<getProbeSetSequence>" << (*sit) << "|" << seqr->id << "|>" << endl;
    }
    socket->flush();
  }
  set<int>::iterator it;
  for(it=seqr->transcriptRequests.begin(); it != seqr->transcriptRequests.end(); it++){
    *t << "<getEnsemblTranscript>" << (*it) << "|" << seqr->id  << "|>";
    socket->flush();
    cout << "Want to get transcript for index " << (*it) << endl;
    if(recordCommands){
      *commandRecord << "<getEnsemblTranscript>" << (*it) << "|" << seqr->id  << "|>" << endl;
    }
  }
  for(it = seqr->peptideRequests.begin(); it != seqr->peptideRequests.end(); it++){
    *t << "<getEnsemblPeptide>" << (*it) << "|" << seqr->id << "|>";
    socket->flush();
    cout << "am requesting peptide sequences for gene " << (*it) << endl;
    if(recordCommands){
      *commandRecord << "<getEnsemblPeptide>" << (*it) << "|" << seqr->id << "|>" << endl;
    }
  }
  set<genomicRequest>::iterator git;
  for(git = seqr->genomeRequests.begin(); git != seqr->genomeRequests.end(); git++){
    cout << "Genome Request chromosome : " << (*git).chromosome << endl
	 << "                  start   : " << (*git).start << endl
	 << "                   end    : " << (*git).end << endl;
    // alleluluaillhhh... 
    *t << "<getGenomeSequence>" << (*git).chromosome.c_str() << "|"
       << (*git).start << "|" << (*git).end << "|" << seqr->id << "|>";
    socket->flush();
    if(recordCommands){
      *commandRecord << "<getGenomeSequence>" << (*git).chromosome.c_str() << "|"
		     << (*git).start << "|" << (*git).end << "|" << seqr->id << "|>" << endl;
    }
  }
  
  //  delete seqr;
  // so the obvious memory leak, I really need to make sure that I don't run into stupid problems here.
  // have to make sure that the server informs me if there is a problem getting the sequence back, at the moment
  // nothing is happenning, which ,, pleaseremember.. 
  
  // and that is all for now, let's just see if I can collect the different things, then I need to know
  // how to handle incoming sequences. hoahhha, haoooah, and rewrite the server to handle the id's and stuff 
  // like that...
}

void Client::getIshThumbnails(int index){
  // request the server to send some thumbnails for probe set index index...
  // very simple...
  if(!open){
    cerr << "Connection Not Open" << endl;
    return;
  }
  *t << "<getIshThumbnails>" << index << ">";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getIshThumbnails>" << index << ">" << endl;
  }
}

void Client::getIshImages(int index){
  // request the server to send some images for probe set index index...
  // very simple...
  if(!open){
    cerr << "Connection Not Open" << endl;
    return;
  }
  *t << "<getIshImages>" << index << ">";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getIshImages>" << index << ">" << endl;
  }
}

void Client::getIshImage(int index){
  if(!open){
    cerr << "Connection Not Open" << endl;
    return;
  }
  cout << "Client requesting image with an index of : " << index << endl;
  *t << "<getIshImage>" << index << ">";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getIshImage>" << index << ">" << endl;
  }
}

void Client::getRegionForIshProbe(int index){
  cout << "am requesting the genomic region for ish probe index : " << index << endl;
  if(!open){
    cerr << "No open connection " << endl;
    return;
  }
  *t << "<getRegionForIshProbe>" << index << ">";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getRegionForIshProbe>" << index << ">" << endl;
  }
}

void Client::getIshTextFields(){
  if(!open){
    cerr << "No open connection " << endl;
    return;
  }
  *t << "<getIshTextFields>>";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getIshTextFields>>" << endl;
  }
}

void Client::getIshFloatFields(){
  if(!open){
    cerr << "No open connection " << endl;
    return;
  }
  *t << "<getIshFloatFields>>";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getIshFloatFields>>" << endl;
  }
}

void Client::getIshClasses(){
  if(!open){
    cerr << "No open connection " << endl;
    return;
  }
  *t << "<getIshClasses>>";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getIshClasses>>" << endl;
  }
}

void Client::getProtocol(int pid){
  if(!open){
    cerr << "no open connection" << endl;
    return;
  }
  *t << "<getProtocol>" << pid << ">";
  socket->flush();
}

void Client::getProtocolCollection(int requestId, int requester){
  if(!open){
    cerr << "No open Connection" << endl;
    return;
  }
  *t << "<getProtocols>" << requester << "|" 
     << requestId << "|>";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<getProtocols>" << requestId << ">";
  }
}

void Client::commitProtocolToDB(Protocol* protocol){
  cout << "Client committing protocol to DB" << endl;
  if(!open){
    cerr << "No open connection " << endl;
    return;
  }
  // make sure to clean text..
  QString cleanName = protocol->name.c_str();
  QString cleanDescription = protocol->description.c_str();
  cleanText2(cleanName, "\\|");
  cleanText2(cleanDescription, "\\|");
  *t << "<commitProtocolToDB>" << (int)protocol << "|"         // so that we can have some sort of identifier.. for acknowledgment purposes.. 
     << protocol->parentId << "|"
     << cleanName << "|" << cleanDescription << "|"
     << protocol->steps.size() << "|";
  for(int i=0; i < protocol->steps.size(); i++){
    cleanDescription = protocol->steps[i]->description.c_str();
    cleanText2(cleanDescription, "\\|");
    *t << protocol->steps[i]->id << "|"
       << protocol->steps[i]->parentId << "|"
       << cleanDescription << "|"
       << protocol->steps[i]->creatorId << "|";
  }
  *t << ">";
  socket->flush();
  /////////  not recording commands it's a bit too bloody long.. 
}

void Client::commitExperimentToDB(Experiment expt, int requestId){
  cout << "Client, committing experiment to DB .. hoo hhooo yeah.. " << endl;
  // =maybe we should have some way of knowing if this worked.. prb
  // -- hmm, which is why addresses are nice, but there you go, what can you do.. -- we'll think of something..
  if(!open){
    cerr << "No open connection " << endl;
    return;
  }
  // firs need to clean things..
  QString cleanComment = expt.comment.c_str();
  cleanText(cleanComment, "\\|");
  *t << "<commitExperimentToDB>" << requestId << "|"
     << expt.userId << "|"
     << expt.experimentTime.toTime_t() << "|"
     << expt.protocol << "|"
     << expt.comment.c_str() << "|>";
  socket->flush();
}

void Client::updateExperiments(){
  cout << "requesting an update for the experiments : " << endl;
  if(!open){
    cerr << "No open connection" << endl;
    return;
  }
  *t << "<SendExperiments>>";
  socket->flush();
}

void Client::requestTissues(){
  if(!open){
    cerr << "No open connection" << endl;
    return;
  }
  *t << "<SendTissues>>";
  socket->flush();
}

void Client::requestIshAnnotationFields(){
  if(!open){
    cerr << "No open connection" << endl;
    return;
  }
  *t << "<SendIshAnnotationFields>>";
  socket->flush();
}


void Client::makeIshTissue(QString name, float age){
  if(!open){
    cerr << "No open Connection " << endl;
    return;
  }
  cleanText2(name, "\\|");
  *t << "<MakeIshTissue>" << name << "|" << age << "|>";
  socket->flush();
}

void Client::makeIshAnnotationField(QString field){
  if(!open){
    cerr << "No open Connection" << endl;
    return;
  }
  *t << "<MakeIshAnnotationField>" << field << ">";
  socket->flush();
}

void Client::associateIshProbeWithGene(int pIndex, int gIndex){
  if(!open){
    cerr << "No open connection " << endl;
    return;
  }
  *t << "<associateIshProbeWithEnsemblGene>" << pIndex << "|" << gIndex << "|" << ">";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<associateIshProbeWithEnsemblGene>" << pIndex << "|" << gIndex << "|" << ">" << endl;
  }
}

void Client::cleanText2(QString& text, QString separator){
  text.replace(QRegExp(separator), "&sep");
  text.replace(QRegExp(">"), "&gt");
  text.replace(QRegExp("<"), "&lt");
}

void Client::cleanText(QString& text, QString separator){
  text.replace(QRegExp(separator), "&sep");
  text.replace(QRegExp(">"), "&gt");
  text.replace(QRegExp("<"), "&lt");
  text.replace(QRegExp("'"), "&apo");
}

void Client::dirtyText(QString& text){
  text.replace(QRegExp("&gt"), ">");
  text.replace(QRegExp("&lt"), "<");
  text.replace(QRegExp("&apo"), "'");
}
  

void Client::setIshProbeName(int id, QString name){
  if(!open){
    cerr << "Socket not open" << endl;
    return;
  }
  cleanText(name, "\\|");
  *t << "<setIshProbeName>" << id << "|" << name << "|>";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<setIshProbeName>" << id << "|" << name << "|>" << endl;
  }
}

void Client::insertIshTextAnnotation(ishTextInsert data){
  // do something..
  if(!open){
    cerr << "Socket not open" << endl;
    data.errorNo = 1;
    data.errorString = "Socket Not Open";
    emit ishTextAnnotationHandled(data);
    return;
  }
  if(!data.ishProbeId){
    cerr << "probeId is 0 illegal value " << endl;
    data.errorNo = 2;
    data.errorString = "No In Situ probe currently selected";
    emit ishTextAnnotationHandled(data);
    return;
  }
  // first get rid of any pipes in the message or the thing,, -as we will use those for the separator..
  cleanText(data.fieldName, "\\|");      // -- maybe a bad choice for a separator,, -as its used in regular expressions.. hmm 
  cleanText(data.annotation, "\\|");     // replaces the | with a &sep, and removes apostrophies and others.. 
  ////  ofocurse I should also use the escape text function in the connection object, but that's for later.. 
  
  *t << "<insertIshTextAnnotation>" << data.id 
     << "|" 
     << data.ishProbeId 
     << "|" 
     << (int)data.fieldNameNew << "|"
     << data.fieldName << "|" << data.annotation << "|>";
  socket->flush();
  cout << "client, insert ish text annotation, field name  " << data.fieldName << endl
       <<  "                                   annotation  " << data.annotation << endl
       << " and this is a new field ? " << data.fieldNameNew << endl
       << " and the ish probe id is   " << data.ishProbeId << endl
       << " and the id of the ishTextInsert is : " << data.id << endl;
  if(recordCommands){
    *commandRecord << "<insertIshTextAnnotation>" << data.id 
		   << "|" 
		   << data.ishProbeId 
		   << "|" 
		   << (int)data.fieldNameNew << "|"
		   << data.fieldName << "|" << data.annotation << "|>" << endl;
  }
}

void Client::updateIshTextAnnotation(ish_annotation data){
  cout << "Am in the updateIshText Annotation thingy will ask the server to do something funky... " << endl;
  if(!open){
    cerr << "connection not open" << endl;
    return;
  }
  *t << "<updateIshProbeTextAnnotation>" << data.annotationId << "|" << data.probeId << "|" << data.annotation << "|>";
  socket->flush();
  if(recordCommands){
    *commandRecord << "<updateIshProbeTextAnnotation>" << data.annotationId << "|" << data.probeId << "|" << data.annotation << "|>" << endl;
  }
}

void Client::insertIshFloatAnnotation(ishFloatInsert data){
  // do something else..
  if(!open){
    cerr << "connection not open " << endl;
    data.errorNo = 1;
    data.errorString = "Socket not open";
    emit ishFloatAnnotationHandled(data);
    return;
  }
  if(!data.ishProbeId){
    cerr << "ish probe id is 0, not allowed " << endl;      /// actually we need to tell the insitu view about this.. - so need some
    data.errorNo = 2;
    data.errorString = "No In Situ probe currently selected";
    emit ishFloatAnnotationHandled(data);
    return;
  }
  cleanText(data.fieldName, "|");
  *t << "<insertIshFloatAnnotation>" << data.id << "|" << data.ishProbeId << "|" << (int)data.fieldNameNew << "|"
     << data.fieldName << "|" << data.value << "|>";
  socket->flush();

  cout << "in insertIshFloatAnnotation, field name is : " << data.fieldName << endl
       << "                          and the value is : " << data.value << endl
       << "                          and the probe id is : " << data.ishProbeId << endl
       << "                          and the id of the insert is " << data.id << endl;
  //// note that we have to switch to escaping dangerous characters in the server rather than the client.. 
  /// have no idea why I didn't do that before. Should change everything.. ahhayy.. 
  if(recordCommands){
    *commandRecord << "<insertIshFloatAnnotation>" << data.id << "|" << data.ishProbeId << "|" << (int)data.fieldNameNew << "|"
		   << data.fieldName << "|" << data.value << "|>" << endl;
  }
}

void Client::insertIshClassification(ishFloatInsert data){
  // do something. 
  if(!open){
    cerr << "socket closed, byby " << endl;
    data.errorNo = 1;
    data.errorString = "Socket closed";
    emit ishClassificationHandled(data);
    return;
  }
  if(!data.ishProbeId){
    cerr << "ishprobe id is 0, not a valid id,, " << endl;
    data.errorNo = 2;
    data.errorString = "No In Situ probe currently selected";
    emit ishClassificationHandled(data);
    return;
  }
  *t << "<insertIshClassification>" << data.id << "|" << data.ishProbeId << "|" << (int)data.fieldNameNew << "|"
     << data.fieldName << "|" << data.value << "|>";
  socket->flush();
  cout << "in insert ish classification .. class is " << data.fieldName << endl
       << "                    and confidence is : " << data.value << endl
       << "        and the probe id is           : " << data.ishProbeId << endl
       << "        and the data of the id is     : " << data.id << endl;
  if(recordCommands){
    *commandRecord << "<insertIshClassification>" << data.id << "|" << data.ishProbeId << "|" << (int)data.fieldNameNew << "|"
		   << data.fieldName << "|" << data.value << "|>" << endl;
  }
}

void Client::commitImageToDB(ishImageSet data){
  // We have to do many things here.. 
  // one is create a scaled image and then we have to write the images
  // to some kind of buffer.. -like a QBuffer,, then we have to use the QBuffer
  // to write the data to socket along with identifiers as to how we do this.. 
 
  // First make a thumbNail image.. 
  int zw = 250;      // with jpg compression makes for about 4k.. if 5 pics for each, then about 20k down the line.. should be OK.. 
  float zoomFactor = ((float)zw)/((float)data.image->width());
  int zh = (int)(((float)data.image->height())*zoomFactor);
  QImage thumbnail = data.image->smoothScale(zw, zh, QImage::ScaleMin);
  /// OK, now let's write two of these to a QBuffer.. and see how that goes.. (this is lots of memory.. ).
  QImageIO thumbIO, imageIO;
  imageIO.setImage(*data.image);
  thumbIO.setImage(thumbnail);
  imageIO.setFormat("JPEG");
  thumbIO.setFormat("JPEG");
  imageIO.setQuality(100);
  thumbIO.setQuality(100);
  QBuffer thumbBuffer;
  QBuffer imageBuffer;
  thumbBuffer.open(IO_ReadWrite);
  imageBuffer.open(IO_ReadWrite);
  imageIO.setIODevice((QIODevice*)&imageBuffer);
  thumbIO.setIODevice((QIODevice*)&thumbBuffer);    // ahhh... and then write to the buffers.....
  
  if(!imageIO.write()){
    cerr << "Couldn't write the thumb IO to the buffer.. " << endl;
    return;
  }
  if(!thumbIO.write()){
    cerr << "Couldn't write the image IO to the buffer .." << endl;
    return;
  }
  /// It is probably better to move the above code into the image adder part, and then just emit something
  /// containing the relevant char[] or QByteArrays.. ---copying QByteArrays shouldn't be a problem.. 
  
  // at this point we have the images in the buffers let's just take a lok at what they look like..
  QByteArray imageData = imageBuffer.buffer();
  QByteArray thumbData = thumbBuffer.buffer();
  cout << "size of Image data is : " << imageData.size() << endl
       << "size of thumb data is : " << thumbData.size() << endl;
  // let's try just writing the block.. to the socket, should confuse the server, but it should be oK.
  if(!open){
    cerr << "Socket closed bugger " << endl;
    return;
  }
  cleanText2(data.fileName, "\\|");
  //cleanText2(data.comment, "\\|");
  cleanText2(data.promoter, "\\|");    // replace at the server level with the appropriate pgsql function
  *t << "<CommitIshImageToDB>" 
     << data.probeId << "|"
     << data.promoter << "|"
     << data.experimentId << "|"
     << data.tissueId << "|"
     << data.fileName << "|"
     << data.comments.size() << "|";
  cout << "Data comments size is : " << data.comments.size() << endl;
  for(int i=0; i < data.comments.size(); i++){
    *t << data.comments[i].index << "|"
       << data.comments[i].comment << "|";
  }
  *t << imageData.size() << "|" << data.identifier << "|>";
  socket->flush();
  writeBlock(imageData.data(), imageData.size());
  writeInt(0);
  writeInt(thumbData.size() / 2);   
  writeInt(thumbData.size());  // three numbers that have to follow this,, just to make sure that we don't get the
                               // the wrong number for the size.. 
  writeBlock(thumbData.data(), thumbData.size());
  cout << "managed to write a whole load of stuff to the database.. " << endl;
  // and let's update the things.. ok ..
  getIshThumbnails(data.probeId);     // which will force an update.. it's good.. 
}

void Client::connectionClosed(){
  open = false;
  emit statusMessage("connection has been closed");
}

void Client::readCommandFile(QString file){
  ifstream in(file.latin1());
  if(!in){
    cerr << "couldn't open command file " << endl;
    return;
  }
  string line;
  while(in >> line){
    *t << line.c_str();
    socket->flush();
  }
  /// and maybe it is just that simple.. 
}  

char* copyString(string line){
  char* bytes = new char[line.length()];
  for(int i=0; i < line.length(); i++){
    bytes[i] = line[i];     // ??
  }
  return(bytes);
}

void Client::abuseTheServer(QString file){
  abuseCounter = 0;
  abusive = true;
  cout << "Beginning of abuseTheServer hold your breath.. " << endl;
  ifstream in(file.latin1());
  if(!in){
    cerr << "couldn't open command file " << endl;
    return;
  }
  //vector<string> lines;
  string line;
  commands.resize(0);
  messages.resize(0);
  //  vector<string> commands;
  //vector<string> messages;
  ///////// kind of messy, but I want to be able to do a random choice of command and then a random message combination..
  QRegExp matcher("^(<[^>]+>)([^>]*>)");
  while(in >> line){
    //lines.push_back(line);
    int pos = matcher.search(line.c_str());
    if(pos != -1){
      //      cout << "command : " << matcher.cap(1) << endl << "message : " << matcher.cap(2) << endl;
      commands.push_back(matcher.cap(1).latin1());
      messages.push_back(matcher.cap(2).latin1());
    }
  }
  in.close();
  //abusive = !abusive;
  doTheAbuse();
}

void Client::doTheAbuse(){
  /// now we will read through these lines, and for each line we decide to either
  // 1. send it to the server as it is )
  // 2. mutate it by a random frequency..
  // 3. ignore it and send a random array of bytes of a random length (within certain limits)..
  // 
  // and we will repeat this a number of times... dependant on some variable.. (later on, take a value from somewhere)..
  int commandNo = 4;    // small number to begin with..
  int maxLength = 50000;  // not to bad.. -- but likely to be really quite confusing..
  int mlength;      // dependant on the choice of abuse.. 
  int clength;
  int tempLength;
  int expandLength;
  char* messageBytes;
  char* commandBytes;   // so we can independently change these around.. 
  char* tempBytes;      // use as copy buffer if necessary.. 
  float mutFrequency;
  int maxCount = 100000;
  int counter=0;
  // choose commands at random, and then randomise in a random manner.. 
  //  QTimer* timer = new QTimer(this);  
  //connect(timer, SIGNAL(timeout()), this, SLOT

  for(int i=0; i < commandNo; i++){
    
    //for(int i=0; i < commandNo; i++){ 
    int decider = rand() % 5;    // 0, 1, 2
    int command = rand() % commands.size();   // a random command.. 
    int message = command;
    if(rand() % 2){                             // i.e. if odd number,, i.e. 50 % of the time..
      int message = rand() % messages.size();   // i.e. half the time make a random mixture of command and message.. and then mutate.. 
    }
    cout << "decider is Now : " << decider << endl;
    switch(decider){
    case 0:                   // 
      commandBytes = copyString(commands[command]);
      clength = commands[command].length();
      messageBytes = copyString(messages[message]);   // i.e. the appropriate message.. 
      mlength = messages[message].length();
      break;
    case 1:
      commandBytes = copyString(commands[command]);
      clength = commands[command].length();
      messageBytes = copyString(messages[message]);
      mlength = messages[message].length();
      mutFrequency = (float)rand() / (float)RAND_MAX;
      mutateBytes(messageBytes, mlength, mutFrequency);
      break;
    case 2:
      commandBytes = copyString(commands[command]);
      clength = commands[command].length();
      mlength = 1 + (rand() % maxLength);
      messageBytes = randomBytes(mlength);
      break;
    case 3:
      commandBytes = copyString(commands[command]);
      clength = commands[command].length();
      messageBytes = copyString(messages[message]);
      mlength = messages[message].length();
      mutFrequency = (float)rand() / (float)RAND_MAX;
      softMutateBytes(messageBytes, mlength, 5, mutFrequency);
    case 4:
      commandBytes = copyString(commands[command]);
      clength = commands[command].length();
      //// get a temporary buffer for the expand bytes function.
      tempBytes = copyString(messages[message]);
      tempLength = messages[message].length();
      expandLength = rand() % (tempLength);   // but what if it's 0.. hmm. 
      mlength = tempLength + expandLength;
      messageBytes = new char[mlength];
      expandPositions(tempBytes, messageBytes, tempLength, mlength);
      delete tempBytes;   // (am I allowed.. )
      break;
    default :
      cerr << "??????????????????????????????????????" << endl;
    }
    cout << "\tcommand sent is " << commands[command] << endl;
    cout << "\tdecider is      " << decider << endl;
    socket->writeBlock(commandBytes, clength);
    socket->writeBlock(messageBytes, mlength);
    //      socket->writeBlock(bytes, length);
    socket->flush();
    delete messageBytes;
    delete commandBytes;
    
    abuseCounter++;
    cout << endl << "\t\t\t COUNTER: " << abuseCounter << endl << endl;
    if(abuseCounter > maxCount){
      abusive = false;
    }
    //if(counter % 10 == 0){
    //  sleep(1);
    //}
    //    readyRead();  // check for some response.. but may make little differe nce as 
  }
  // sleep(1);
  cout << "end of abuseTheSocket whooa, how did that go.. " << endl; 
}

void Client::close(){
  cout << "in the client close function" << endl;
  //*t << "<closingConnection>>";
  //socket->flush();
  socket->close();      // why didn't I try this before,, - well not necessary as it get's  closed anyway.. 
}

/*
void Client::deleteExpChooser(ExperimentChooser* address){
  delete address;
}
*/
  

