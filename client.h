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

#ifndef SIMPLE_CLIENT_H
#define SIMPLE_CLIENT_H

#include "dataStructs.h"
#include "chips/chipSelection.h"
//#include "jpgView/imageViewer.h"
#include <qobject.h>
#include <qstring.h>
#include <qdatastream.h>
#include <qcstring.h>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <fstream>
#include "probe_set/probe_set.h"
#include "bytevec/bytevec.h"
#include "exptinfo.h"        // for the declaration of the exptInfo struct
#include "annotator/sessionManager/sessionWidget.h"
#include "sequence/sequenceRequest.h"
#include "sequence/seqWidget.h"
//#include <qmutex.h>         /// REMOVE THIS,, later, I DONT NEED this,, 

//#define EXPERIMENTAL_CLIENT

using namespace std;

class QSocket;
class QTextStream;
class QString;


struct Exon {           // a normal exon.. and where it matches in the genome.. sort of.. part of a transcript with more information..
  ~Exon(){}
  Exon(){
    cstart = cend = tstart = tend = 0;
  }
  Exon(int cs, int ce, int ts, int te){
    cstart = cs;
    cend = ce;
    tstart = ts;
    tend = te;
  }
  int cstart;
  int cend;
  int tstart;
  int tend;
};

struct ensemblExon {
  ensemblExon();
  ensemblExon(string i, int st, int sp, int cst, int csp, int sd);
  string id;     // the ensembl identifier, so I can work out unique things..
  int start;
  int stop;
  int codeStart;
  int codeStop;   // if non coding. Doesn't seem like there is an awful lot of things going on for this..
  int strand;     // -1 or 1 ,, -to use ensembl syntax.
};

struct Transcript {
  Transcript(){
    source = id = chromosome = "null";
    cstart = cend = strand = length = 0;
  }
  Transcript(string s, string i, string c, int cs, int ce, int st, int l){
    source = s;
    id = i;
    chromosome = c;
    cstart = cs;
    cend = ce;
    strand = st;
    length = l;
  }
  void addExon(Exon ex);
  string source;
  string id;
  string chromosome;
  int cstart;
  int cend;
  int strand;
  int length;
  vector<Exon> exons;
};
  

struct ensemblTranscript {
  ensemblTranscript(){};
  ensemblTranscript(int tIndex, string i, int st, int sp, int sd, int gi);
  ~ensemblTranscript(){};
  int index;        // the db Index, should be unique perhaps.. (called transcript)
  int geneIndex;
  string id;        // the ensembl ID.
  int start;
  int stop;
  int strand;
  void addExon(ensemblExon ex);
  vector<ensemblExon> exons;    // array of pointers to exons, easier for memory management.. as I don't have to worry about copying actual exons when growing the array.
  bool rSelected;
  bool pSelected;               // a transcript => protein.. 
};

struct ensemblGene {
  ensemblGene();
  ~ensemblGene(){};
  ensemblGene(int dbi, string ensid, string chr, int st, int sp, bool s);
  int dbIndex;
  string ensemblId;
  
  // replace these with a more solid data structure that we can use in a more flexible manner. Later boys. 
  //  string externalId;
  // string description;
  
  string chromosome;
  int start;
  int stop;
  vector<ensemblTranscript> transcripts;
  bool sense;
  multimap<string, string> annotation;
  bool selected;
};

struct probeSetMatch {
  probeSetMatch();
  ~probeSetMatch(){};
  //  probeSetMatch(int cs, int ce, int afi, int afs, int afe, int afl, int all, int afn, int m, double exp);
  probeSetMatch(int cs, int ce, int afi, string chr, int afs, int afe, int afl, int all, int m, int s, double exp);
  int cStart;
  int cEnd;
  int afIndex;
  //string afid;
  string chromosome;    // would seem a good idea?
  int afStart;
  int afEnd;
  int afLength;
  int alLength;
  //int af_n_count;
  int match;
  int strand;
  double expect;
  bool selected;              // for drawing purposes.. usually false. 
};

struct ishProbeMatch {
  ishProbeMatch(){
    //    probeIndex = 0;
    selected = false;
  }
  ishProbeMatch(int pst, int psp, int cst, int csp, float pcnt){
    //probeIndex = pi;
    pStart = pst;
    pEnd = psp;
    //pLength = pl;
    //alignLength = all;
    //match = m;
    cStart = cst;
    cEnd = csp;
    //expectation = e;
    //strand = s;
    //chromosome = c;
    percent = pcnt;
    selected = false;
  }
  //int probeIndex;
  int pStart;
  int pEnd;
  //int pLength;
  //  int alignLength;
  // int match;
  int cStart;
  int cEnd;
  //double expectation;
  float percent;
  //int strand;
  //string chromosome;
  bool selected;
};

struct ishProbeMatchSet {
  ishProbeMatchSet(){
    probeIndex = 0;
    selected = false;
  }
  //ishProbeMatchSet(ishProbeMatch ispm);    // constructor.. 
  ishProbeMatchSet(int pi, int minpos, int maxpos, int plength, float scr, int strand, string chromosome, int assemblyid);    // constructor.. 
  void addMatch(ishProbeMatch ispm);       // add an ispm and check the values.. 
  int probeIndex;
  int probeLength;
  //int probeStart;
  //int probeEnd;    // no sense to put here.. 
  int cStart;
  int cEnd;     /// the beginnings and ends..i.e. maxes and min values.. 
  int strand;
  string chromosome;
  int assemblyId;
  float score;
  vector<ishProbeMatch> ishMatches;   // like below..
  bool selected;
};

struct chromRegion {
    chromRegion();
  //  chromRegion(string s, int st, int e, int l, int maxF, int maxR, vector<probeSetMatch> matches, vector<ensemblGene> genes, map<int, ishProbeMatchSet> ishm); 
  chromRegion(string chr, int st, int e, int cl);
  chromRegion(string s, int l, int st, int e, vector<probeSetMatch> matches);
  void addEnsGene(ensemblGene eg);
  void addPMatch(probeSetMatch psm);
  void addIshMatch(ishProbeMatchSet ispm);   // not sure what the int is but may need it..
  void addTranscript(Transcript trans);             // generic transcript type, contains an identifier of sorts. . 
  ensemblGene getGene(int gIndex);                  // return.. an ensemblGene.. 
  Transcript getTranscript(string source, string id);
  ~chromRegion(){};
  string chromosome;
  int chromLength;
  int chromStart;
  int chromEnd;          // the region we are looking at..
  int maxForwardFeatures;
  int maxReverseFeatures;   // just for drawing... 
  bool hasForwardIshProbe;
  bool hasReverseIshProbe;   // again just for drawin. A little bit clumsy, but there you go.. again it would seem that inheritance might be a good idea.. 
  vector<probeSetMatch> fPMatches;   
  vector<probeSetMatch> rPMatches;
  vector<ensemblGene> fEnsGenes;
  vector<ensemblGene> rEnsGenes;     // but usually we don't set this at the same time. it's for later.. silly, but maybe more flexible.
  map<int, ishProbeMatchSet> fIshMatches;
  map<int, ishProbeMatchSet> rIshMatches;
  multimap<string, Transcript> fTranscripts;
  multimap<string, Transcript> rTranscripts;    // matches to the Fantom sequences.. 
  // and whatever else we can think of putting in there somehwere over the ocean.. 
  // for drawing purposes -- so that we don't have to associate things..
  int selectBegin;
  int selectEnd;

};

struct clusterSet {
  clusterSet();
  clusterSet(vector< vector<int> > i, vector< vector< vector<float> > > p, vector< vector< vector<int> > > e, vector< vector<float> > c, vector< vector<int> > ce, map<float, exptInfo>* me, bool lN);
  ~clusterSet();
  vector< vector<int> > indices;             // the indices of each probe set..
  vector< vector< vector<float> > > points;  // the data points organised by cluster and by probe set
  vector< vector< vector<int> > > expts;   // the experimental points for above. Very wasteful, but convenient for the plotter

  vector< vector<float> > centers;           // the data points for each cluster center..
  vector< vector<int> > centExpts;           // the experimental points for the experimental indices.. 
  bool localNorm;
  // and as we are wasting memory anyway, why not stick in a pointer to the experiment info map..
  map<float, exptInfo>* expInfo;              // just take a pointer for convenience..
};



  
class Client : public QObject
{
  Q_OBJECT

    public:
  Client(QString uName, QString serverName, vector<int> uKeys, int port);
  ~Client();
  map<float, exptInfo> expInfo;  // contains experimental information.. use instead of the nadfile vector..  // may need this to be accessible
  map<int, float> exptLookup;   // dbaseIndex to order specifier.. 
  ChipSelection* chipData;   // some data about chips and their relationships.. -- and some way to relate these.. (Client Window needs to know this one
  vector<int> selectedExperiments;  // -contain the current selection of experiments. use dbindex in exptInfo.. 
  //  vector<vitalStatistics> vitalStats; // stats for the current selection, --hmm sometimes.. for a window up above.. 
  vitalStatistics vitalStats; // stats for the current selection, --hmm sometimes.. for a window up above.. 
  //// vitalStatistics are defined in the probe_set.h file,, and the constructor is there... 
  probe_data pData;               // I may not need to keep these here, but I suppose I can just pass them as referencees.. 
  probe_set pSet;                // for keeping stuff in..

  int userId;
  map<int, userInformation> userInfo;   // all user information.. 
  set<clusterSet*> clusterSets;             // pass it on to the clusterWindow so it can be sorted.. 
  ///// its useful for clientWindow to be able to see this, so its kind of nice to put them in the private,, but 
  ///// would be nice to have them readonly or something, but I'm not sure how to do that.. 
  set<chromRegion*> chromosomalRegions;    // a set of chromosomal region pointers.. !. -- for the chrom region window,, -- which I will eventually get rid of
  /////////  I really should make these private.. and then just provide a pointer.. 
  vector<chromRegion> currentRegions;     // These should be linked to the regions that we get from the things, for redraw, emit new set.. 
  vector<chromRegion> tempRegions;        // while we are changing in between receiving and getting things.. or something like that.. 
  vector<chromRegion> currentIshRegions;
  vector<chromRegion> tempIshRegions;     // for the ish regions.. 
  /// these temps are for growing the thing until the last bit is ok'd... and then swapping over. Maybe for regions we don't need such complicated stuff.
  vector<chromRegion> genRegions;        // Not selected by thingy, but by something else.. 

  vector<int> curIndex(){
    return(currentIndex);
  }
  int indexSize(){
    return(currentIndex.size());
  }
  
  deque<IndexSet>* indexHistory(){
    return(&IndexHistory);              // seems a bit silly, if I give up the pointer, might as well make it public.. hmm 
  }
  
  exptInfo getExperimentInfo(int n);

 signals:
  void newStatus(StatusMessage);        // kind of similar to statusMessage, but more details, and probably better for later.. 
  void statusMessage(QString);
  void newPSet(probe_set);
  void newPData(probe_data*);
  void probeDataChanged();  // like above, but for different purpose. messy but there you go.. 
  void newIndex(int);   // just the size of the index.. but it seems that it might be useful to emit the whole index.. Oh well
  void indexHistoryChanged();
  void newRegionIndex(int n);   // new regions.. 
  void newExpInfo(map<float, exptInfo>);
  void exptInfoChanged();
  void newDBChoices(vector<string>);  
  void newRegionChoices(vector<string>);
  void moreToRead();
  void passWordError();
  void passWordChanged(bool);
  void newVitalStats();      // 
  void sessionCreated(int);
  
  void readingSessionWidgets();
  void newSessionWidget(SessionWidget*); 
  void newClusterSet(clusterSet*);
  void newChromRegion(chromRegion*);
  void newEnsemblGeneRegion();   // just change the internal data structure and it should be OK, then get the thingy to update.. . 
  void newCurrentRegions(int, int);      // it's a bit dodgy as when to call this,, as we can just call it each time the thing grows, which will slow things up..
  void newRegion(int, int);              // for the single region in case we select a region.. 
  void newIshThumbnail(ishImageData);     // enough information to make a thumbnail image,, 
  void newIshImage(ishImageData);
  void newCurrentIshRegions(int, int);    // connects to somewhere.... 
  void newIshProbeData(ishProbeData);     // send data to somewhere.. 
  /// ishAnnotation returns and error notification..
  void ishTextAnnotationHandled(ishTextInsert);
  void ishFloatAnnotationHandled(ishFloatInsert);
  void ishClassificationHandled(ishFloatInsert);   // set's error codes and stuff.. -- hence,, if not sent, because of something, then informs users
  // or something.. like that.. anyway.. -- if no error code than everything went fine.. great.. 
  void ishTextFields(vector<QString>);
  void ishFloatFields(vector<QString>);
  void ishClasses(vector<QString>);
  
  void newExperimentalDistances(exptDistanceInfo);
  void newExperimentTrace(vector<tracePoint>);

  /// Protocols and the like
  void newProtocolCollection(int, int, ProtocolCollection*); 
  void newProtocol(Protocol*);

  void newExperiment(int, Experiment);
  void newExperiments(vector<Experiment>);

  void newTissues(vector<tissue>);   
  void newIshAnnotationFields(vector<ishAnnotationField>);    // should really make some sort of a struct to hold these 

  private slots:
    void readyRead();
  bool readArray();
  void parseArray();
  bool parseProbeSet();                // parse the byte array and create a probe set.. 
  bool parseExptInfo();
  void parseChipInfo();    // parse the chipInfo, and create a 
  bool parseProbeData();
  bool parseIndex();
  bool parseNewRegions();  
  bool parseDBChoices();
  bool parseMessage();
  void parseStatusMessage();
  bool parseVitalStatistics(); //
  bool parseSessionCreation();
  bool parseSessionDescription();   // 
  bool parseClusters();
  void parseExperimentTrace();
  void parseExperimentDistances(bool isFlat);
  

  void parseSequence();             // the message should contain some dnaSequence.. 
  bool parseGenomicRegionProbeSetMatches(); // a bit of a mouthful if I every heard one..
  bool parseGenomicRegionEnsemblGenes();    // but descriptive as the say.. 
  bool parseGenomicRegion();                // one containing everything known to man.. 
  bool parseThumbnail();                    // hmm.. 
  bool parseIshImage();                     // full sized image whooa...h
  bool parseIshProbeData();                 // parse some data and emit some wonderful structures.. 
  bool parseIshTextFields();
  bool parseIshFloatFields();
  bool parseIshClasses();
  bool parseWords(vector<QString>& words, string tail);
  
  //// some things for parsing genomic region headers...
  bool parseGenomicRegionHeader();     // set some variables for the currently loading things..
  bool parseGenomicRegionTailer();     // swap tempRegions with currentRegions (for ish or regular regions).. 

  ///  some things for dealing with protocols.. 
  bool parseProtocolCollection();             // parse a set of protocols and set up a logical structure for these... 
  bool parseProtocol();
  ///  and some things for dealing with experiments.. 
  bool parseExperiment();
  void parseExperiments();   // a colelction of experiments.. 

  void parseTissues();
  void parseIshAnnotationFields();

  void acknowledgeReceipt(int n);     // send an acknowledge statement to the server.. 
  void connected();
  void connectionClosed();
  void openConnection();
  void clearHeader();

  void cleanText2(QString& text, QString separator);  // as below but does not remove ' 
  void cleanText(QString& text, QString separator);    // removes <, > and the separator and replaces with appropriate things (&sep for separator).
  void dirtyText(QString& text);    // opposite of above.. shouldn't be necessary as I should start using the pq function instead.. 

  public slots:
    void setIndex(vector<int> ind, QString term);

    void sendMessage(QString);         // format everything in the clientWindow??
  void doDBLookup(QString);
  void doGenLookup(QString);          // for finding genomic regions using things.. 
  void expandIndexByGenes();
  void getProbeSet(int);
  void getRegion(int);
  void doAnovaSort();                 // sort the current index by anova.. 
  void doSelectAnovaSort();           // use only the experiments in selectedExperiments .. simple message to pass.  
  void doEuclidSort(int n);           // sort by similarity to index n.. (from currentIndex)... 
  void doRawComparison(vector<float> values, vector<int> eIndex, bool distribution);  // do a euclidean sort to this one.. work out what the experimental indices mean.. 
  void doMeanComparison(vector<float> values, vector<int> eIndex, bool normed, bool distribution);
  void doBinaryComparison(int up, int down, bool normed);
  void getDevsFromMean();
  void close();
  void getFileInformation();
  void getDBChoices();
  void getUserInformation();
  bool parseUserInformation();
  void getChipInformation();

  void sendClientIndex();
  void setClientChips(vector<int> v);
  void doKCluster(int k, bool localNorm, bool individualNorm, bool meanNorm);
  void compareExperiments();    // compare the currentley selected experiments on the basis of the currently selected genes
                                // if experiment doesn't have a gene, then that gene is ignored. 
  void flatCompareExperiments(float sigma, float order);
  void traceExperiments(float sm);
  void compareCells(vector<int> a, vector<int> b);          // actually compare the two groups of cells,, -returning the contributing scores for the 
                                                        // currently selected set of genes... --similar ones on one side, and different ones on the other.. 

  void getGenomicRegion(string chr, int start, int end);
  void getGenomicRegionEnsemblGenes(int requestId, string chr, int start, int end);

  void reOpenConnection();
  void changePassword(vector<int> oldh, vector<int> newh);
  void createNewUser(vector<int> oldh, vector<int> newh, QString uName);
  void getStatCollection();        // use the selectedExperiments vector !!.. 
  void createSession(vector<string> words);
  void newAnnotation(int sIndex, vector<int> aGenes, QString note);
  void updateSessions();       // ask the server for a list and details of the sessions available.. 
  void updateSessionDescription(int sessionIndex, vector<QString> words);     // tell the server to change the descriptions for a given set of genes. 
  void updateAnnotationDescription(int annotationIndex, QString description);
  void collectSequences(SequenceRequest* seqr);
  
  void getIshThumbnails(int index);
  void getIshImages(int index);       // this is a request for all images associated with a given probe.. 
  void getIshImage(int index);        // note this is a request for a specific image
  void getRegionForIshProbe(int index);  // get the genomic regions for this specific index.. 
  void associateIshProbeWithGene(int pIndex, int gIndex);
  void insertIshTextAnnotation(ishTextInsert data);
  void updateIshTextAnnotation(ish_annotation data);
  void insertIshFloatAnnotation(ishFloatInsert data);
  void insertIshClassification(ishFloatInsert data);
  void setIshProbeName(int id, QString name);

  void getIshTextFields();
  void getIshFloatFields();
  void getIshClasses();     // request the server to send the appropriate fields to the client.. 

  //// Protocols and stuff;;
  void getProtocolCollection(int requestId, int requester);
  void commitProtocolToDB(Protocol* protocol);   //
  void getProtocol(int pid);

  //// Experiments and Stuff ..
  void commitExperimentToDB(Experiment expt, int requestId);
  void updateExperiments();

  // tissues .. and ishprobeannotationfields..
  void requestTissues();                ////// hmm, this is kind of confusing with the getIshTextFields and so on above.. hmm bugger
  void requestIshAnnotationFields();   // arghh.............. 

  void makeIshTissue(QString name, float age);
  void makeIshAnnotationField(QString field); 

  void commitImageToDB(ishImageSet data);
  ///// a slot for checking new protocols.. 
  void testSlot();
  void setRegionSize(int i);  
  
  ///// Export commands..
  void exportMeans(QString fileName, int normMethod);   // to request the export
  void parseMeans();                    // to receive the data, and write to a file.
                                        // will check in saveFileNames to make sure that the file has been requested.. 


  ///////////////////// read a files containing commands to send to the server, mainly for stressing the client out, but also perhaps for other purposes..
  void readCommandFile(QString file);
  void abuseTheServer(QString file);
  void doTheAbuse();
  int abuseCounter;    // shouldn't be public but placing here so I don't lose track of it.. 

 private:
  /// probably the wrong place to put here,, but nevertheless, I will keep it here for now until I have a good reason for putting it elsewhere
  vector<int> currentIndex;      // constains the indices returned by functions.. 
  int lastRequested;             // the last position requested.. -not ideal, but 
  deque<IndexSet> IndexHistory;  // to allow making stuff.. 
  int indexHistorySize;          // so we can change somewhere.. 
  set<string> saveFileNames;     // if we request a file to save from the server, and it returns, make sure that we get the same name back. 

  //QMutex testMutex;       /// FOR TESTING FUNCTIONS REMOVE AT SOME LATER STAGE PLEASE.. 
  void writeInt(int i);     // just writes an int to the socket.. (binary ..)
  void writeBlock(const char* data, uint s);

  QString message;
  QSocket* socket;
  QString hostAddress;
  QTextStream* t;
  QString response;
  bool open;        // connnection open??  -- change to some sort of real function that actually checks the state of the connection. 
  int readState;    // 0 - read nothing, 1, reading header, 2 reading message,
  QString currentHeader;

  /// the new things to replace all the old things..
  QString dataId;
  QString dataSize;
  ByteVec* idata;          // hmmm, hope nothing else is called idata.. hmm.

  // some user specific data..
  QString userName;
  // host address is already set,, so no need..
  vector<int> userKeys;    // the keys for the user.. 
  
  bool reOpen;                   // do I want to reopen??
  //vector<string> dbChoices;      // for the database options. // hmm probably don't need that here.. 
  map<int, SequenceRequest*> sequenceRequests;
  SeqWidget* seqDisplay;

  int regionSize;    // the region size to request.. from the connectionObject.. !! 
  int currentRegionAssociation;
  int currentIshRegionAssociation;   // probe associated with the regions being sent at the moment..

  //////////////////////////////// RECORDING VARIABLES FOR RECORDING OUTBOUND TRAFFIC, TO ENABLE ABUSING PROGRAMS ////
  bool recordCommands;
  ofstream* commandRecord;
  ////////////////////////////////////// Abuser variables... ... 
  vector<string> commands;
  vector<string> messages; 
  bool abusive;      // am I abusive?? or not.. 
};

#endif


