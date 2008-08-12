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

#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <qcstring.h>
#include <qstring.h>
#include <qobject.h>
#include <qdatetime.h>
#include <string>
#include <map>
#include <vector>
#include <set>

using namespace std;

struct drawPoint {    // a point for drawing things..
  QRect rect;
  int dbIndex;
  float index;        // these are just set when we draw.. the actual values we get from elsewhere.. 
  int yo;             // y offset.. for calculating position in the drawing thing.. 
  int xo;             // and an x offset.. 
  bool active;
    bool hidden;     // don't draw.. the dots.. 
  drawPoint(){
    rect.setRect(0, 0, 0, 0);
    dbIndex = 0;
    index = 0;
    yo = 0;
    xo = 0;
    active = false;
    hidden = false;
  }
};

struct exptPoints {     // this is to allow it to plug into old code more easily.. not great, kind of ugly, but might save lots of time.. 
  vector<float> values;
  vector<int> indices;   // the db indices..
  bool isOK(){
    return(values.size() == indices.size());
  }
  exptPoints(){}        // empty constructor. should be OK.. 
};

struct chipInfo{
  int index;
  string id;
  string description;
  set<int> equivs;    // chip equivalents, so that chip 6 can represent 4 and 5.. sort of..
  chipInfo(){
    index = -1;
    description = "null";
  }
  chipInfo(int i, string Id, string d){
    index = i;
    id = Id;
    description = d;
  }
  chipInfo(int i, string Id, string d, set<int> e){
    index = i;
    id = Id;
    description = d;
    equivs = e;
  }
  // and a function to check..  which is a very rudimentary check. 
  bool defined(int i){
    return((bool)equivs.count(i));
  }
};


struct StatusMessage {
  unsigned int id;  // some identifier for a requester..
  int no;           // some internal identifier for a requester
  bool ok;          // if it worked ok..
  vector<QString> errorMessages;  // error Messages..
  vector<int> errorCodes;        // not sure which one is better, but anyway, these are all going to be ok
  StatusMessage(bool o=true){
    id = 0;
    no = 0;
    ok = o;
  }
  StatusMessage(unsigned int i, bool o=true){
    id = i;
    no = 0;
    ok = o;
  }
  StatusMessage(unsigned int i, int n, bool o=true){
    id = i;
    no = 0;
    ok = o;
  }
  StatusMessage(unsigned int i, int n, vector<QString> em, bool o=true){
    id = i;
    no = 0;
    ok = o;
    errorMessages = em;
  }
  StatusMessage(unsigned int i, int n, vector<QString> em, vector<int> ec, bool o=true){
    id = i;
    no = 0;
    ok = o;
    errorMessages = em;
    errorCodes = ec;
  }
};

// point coordinates are used to map cell types to point in n dimensional space.
// Generally these would be in 2 dimensional space, to allow easy visualisation, but this could also be done in 
// more dimensions for other purposes (map type comparisons for fuzzier comparisons. !! 

struct PointCoordinate {
  int index;     // the experimental index.. if not defined, then do something else.. 
  float* coords;
  int dimNo;
  //  vector<float> coords;       // the coordinates, -- use a vector, so we can use different dimensional space
  PointCoordinate(){
    // do nothing..
  }
  PointCoordinate(int i, float* crds, int n){
    index = i;
    coords = crds;
    dimNo = n;
  }
};


struct IndexSet {              // bad name, -- but contains a set of probe set indices and some string as to where it came from
  public :
  IndexSet(vector<int> ind, QString des){
    Index = ind;
    Description = des;
    LastRequest = 0;
  }
  IndexSet(){}
  ~IndexSet(){}
  
  void setLast(int i){
    if(i < Index.size()){
      LastRequest = i;
    }
  }

  int size(){
    return(Index.size());
  }
  vector<int> index(){
    return(Index);
  }
  int lastRequest(){
    return(LastRequest);
  }
  QString description(){
    return(Description);
  }
  // combine functions..
  vector<int> And(vector<int> b);
  vector<int> Or(vector<int> b);   /// hmmm. this will be a bit wasteful, but it will at least allow for the preservation of some order.. 
  vector<int> Not(vector<int> b);  // return a vector which doesn't contain stuff from b.. 

  private :
  vector<int> Index;
  int LastRequest;
  QString Description;
};

struct ProtocolStep {
  int id;
  int parentId;
  int creatorId;
  string creatorName;
  string description;
  QDateTime creationTime;
  ProtocolStep* parent;                 // these last two might be useful at a later date, but I'm unlikely to use them at the moment.. 
  vector<ProtocolStep*> daughters;      
  ProtocolStep(){
    id = parentId = 0;
    parent = 0;
  }
  ProtocolStep(int i, int pi, uint time, int ci, string cname, string des);     // other things are worked out at later stages.. 
  ~ProtocolStep();
};

struct Protocol {
  int id;
  int parentId;
  QDateTime creationTime;
  int creatorId;
  string creatorName;
  string name;
  string description;
  vector<ProtocolStep*> steps;
  vector<Protocol*> daughters;
  Protocol* parent;
  void* pointer;      // for the user .. useful for going up and down trees and things.. !! 
  int requestId;      // if this is sent as a result of an insert.. request id is not a good thing, ,but can't think of a better word for it at the moment.. 
  // and some constructors..
  Protocol(){
    id = parentId = 0;
    parent = 0;
    pointer = 0;
    currentDaughter = 0;
    requestId = 0;
  }
  Protocol(int i, int pi, uint time, int cid, string cname, string pname, string pdes, int reqId = 0);
  ~Protocol();         // deletes the steps, but doesn't look at its own parents.. BE VERY CAREFUL WITH THIS.. 
  /// some functions that help parse trees...
  int currentDaughter;
  Protocol* nextDaughter();
};
  
struct ProtocolCollection {
  map<int, Protocol*> protocols;           // first : protocolId, second : position in the protocols.. 
  Protocol* rootProtocol;                  // the first protocol.. 
  bool valid;                              // if any problem then not valid,, 
  int refcount;                            // how many people are looking at us.. --
                                           // note that the user has to implement this.. 
  /// need creator and destructors.............
  /// let the client do the parsing.. otherwise too specialised... 
  ProtocolCollection(map<int, Protocol*> prots);           // sets up the relationships between the different protocols.. -and checks that these make sense.. 
  ~ProtocolCollection();        // destroy all of the pointers.. 
};

struct Experiment {
  int id;   // -1 = not set yet. 
  int userId;
  string userName;
  QDateTime experimentTime;
  QDateTime entryTime;      // the time entered into the database.. 
  int protocol;             // to view the protocol, use a protocol viewer,, -it just needs to know the db index of the protocol..
  string protocolName;
  string protocolDescription;   // get these when loading experiments.. 
  string comment;           // extra information that the user might want to input into the system..
  //   some creators..
  Experiment(){
    id = -1;
  }
  Experiment(int i, int uid, string uname, QDateTime expttime, QDateTime etime, int prot, string pName, string pDes, string com){
    id = i;
    userId = uid;
    userName = uname;
    experimentTime = expttime;
    entryTime = etime;
    protocol = prot;
    comment = com;
    protocolName = pName;
    protocolDescription = pDes;
  }
};

// I don't really need anything else,, as I can just deal with vectors of these structs.. nothing really fanciful.
struct tracePoint {
  int id;
  float distance;
  float x;
  float y;
  tracePoint(int i, float d, float X, float Y){
    id = i;
    distance = d;
    x = X;
    y = Y;
  }
  tracePoint(){
    id =0;
    distance = x = y = 0;
  }
};

struct exptDistanceInfo {
  bool isFlat;
  int geneNo;
  float sigma;
  float order;
  vector<int> experiments;
  vector<vector<float> > values;
  exptDistanceInfo(vector<int> e, vector<vector<float> > v, bool isflat=false){
    experiments = e;
    values = v;
    isFlat = isflat;
    geneNo = 0;
    sigma = 0;
    order = 0;
  }
  
};

struct ishImageData {
  /// it strikes me that I should probably include a unique index for the image,, -which we can get from the database
  /// this would then allow us to refrain from creating lots of duplicate images in some sort of reasonable way,, and 
  /// it would also allow us the information required to get a specific image.. 
  /// or a thumbnail.. and other useful thngs.. 
  ishImageData(){
    ishProbeIndex = 0;
  }
  ishImageData(QByteArray d, int ishIndex, int imIndex){
    ishProbeIndex = ishIndex;
    imageIndex = imIndex;
    data = d;
  }
  ~ishImageData();   // just to see where and when it get's destroyed.. 
  QByteArray data;
  int ishProbeIndex;   // the ishProbeIndex..
  int imageIndex;      // the index of the image.. -- but we don't know if we are a thumbnail or a full sizeed image.. hmm 
};

struct ishFloatInsert {
  int id;
  int errorNo;
  QString errorString;
  bool fieldNameNew;
  QString fieldName;
  float value;       // which in a classification would indicate a confidence value, and whatever for other types.. 
  int ishProbeId;
  
  ishFloatInsert(QString fn, float v, bool n=false){
    ishProbeId = 0;
    id = (int)this;
    errorNo = 0;
    errorString = "";
    fieldName = fn;
    value = v;
    fieldNameNew = n;
  }
  ishFloatInsert(int i, int ishId, int eNo, QString es, QString fn, float v, bool n=false){
    ishProbeId = ishId;
    id = i;
    errorNo = eNo;
    errorString = es;
    fieldName = fn;
    value = v;
    fieldNameNew = n;
  }
  ~ishFloatInsert(){}
};

struct ishTextInsert {
  // contains this and that.. 
  int id;            // should be a unique id of some sort.. I suggest using the memory address of something.. 
  int errorNo;     // not used in sending, but set by the server to indicate failure to insert.. -- can use int to indicate many things. 
  QString errorString;  // if we get one set to the database error.. -- plus perhaps some other interesting note. 
  bool fieldNameNew;  // true if the field name is new.. 
  QString fieldName;  // 
  QString annotation; // 
  int ishProbeId;     // --- how could I forget.. 
  
  ishTextInsert(QString fn, QString a, bool n=false){    // the annotation widget may not need to know the ishprobeid, ..
    id = (int)this;
    errorNo = 0;
    errorString = "";
    fieldName = fn;
    annotation = a;
    fieldNameNew = n;
    ishProbeId = 0;
  }
  ishTextInsert(int i, int ishId, int eNo, QString es, QString fn, QString a, bool n=false){
    ishProbeId = ishId;
    id = i;
    errorNo = eNo;
    errorString = es;
    fieldName = fn;
    annotation = a;
    fieldNameNew = n;
  }

};

struct ish_annotation {
  int userId;
  int annotationId;  // if we receive from the server, we will need some unique id to allow modifications.. 
  int probeId;       // seems we do need this after all.. bugger that ehh.. 
  string userName;   // it's redundant I know, but will probably make life more convenient.
  QString annotation;
  QString fieldName;   // it is useful to have even if at times a little redundant. 
  float value;        // -- define either the float or the annotation string..
  bool numerical;     // -- did we define the value or the string.. 
  ish_annotation(int annId, int uid, string usr, QString note, QString fName, int pid){
    userId = uid;
    userName = usr;
    annotation = note;
    numerical = false;
    annotationId = annId;
    fieldName = fName;
    probeId = pid;
  }
  ish_annotation(int annId, int uid, string usr, float v, QString fName, int pid){
    userId = uid;
    userName = usr;
    value = v;
    numerical = true;
    annotationId = annId;
    fieldName = fName;
    probeId = pid;
  }
  ish_annotation(){
    userId = 0;
    userName = "null";
    annotation = "null";
    value = 0;
    numerical = false;
    annotationId = 0;
    fieldName = "null";
    probeId = 0;
  }
  ~ish_annotation(){};    //empty destructor.. 
};

struct ishProbeData {
  int probeId;
  string consensusSequence;
  string antisensePromoter;
  int afIdAssociation;     // the associated afId..
  string vectorName;
  int designLength;
  string probeName;        // maybe a blank string unless it has been defined. Can be changed by privileged users.. 
  string probeIdentifier;
  int ensemblGuess;        // a guess for an ensembl gene.. --- I don't actually have this yet.. 
  /// some maps for annotation stuff..
  multimap<string, ish_annotation> textAnnotation;
  multimap<string, ish_annotation> numberAnnotation;    // anything that would measured by a number.. 
  multimap<string, ish_annotation> classification;      // -- does it fall into class A? -- and with what degree of confidence.
  
  ishProbeData(){
    probeId = 0;
  }
  ishProbeData(int pId, string cons, string asp, int afid, string v, int dll, string pName, string p_ident, int eGuess){
    probeId = pId;
    consensusSequence = cons;
    antisensePromoter = asp;
    afIdAssociation = afid;
    vectorName = v;
    designLength = dll;
    probeName = pName;
    probeIdentifier = p_ident;
    ensemblGuess = eGuess; 
  }
};

struct userInformation{
  int index;
  string userName;
  string fullName;
  string labName;
  userInformation();
  userInformation(int i, string un, string fn, string lbn);
};

struct tissue {
  int tissueIndex;
  string tissueName;
  float age;
  tissue(){}
  tissue(int ti, string t, float a){
    tissueIndex = ti;
    tissueName = t;
    age = a;
  }
};

struct ishAnnotationField {
  int index;
  QString fieldName;
  ishAnnotationField(){};
  ishAnnotationField(int i, QString s){
    index = i;
    fieldName = s;
  }
};

struct Comment {
  int index;   // the database index..
  QString comment;  // the actual comment..
  Comment(int i, QString c){
    index = i;
    comment = c;
  }
  Comment(){
    index = -1;
    comment = "";
  }
};

struct ishImageSet {             // a temporary struct,, just to allow the emission of a whole load of stuff in the first place
  QImage* image;                 // this is pointer which might get deleted, so only use once..
  int probeId;
  QString promoter;
  int experimentId;
  int tissueId;
  QString fileName;
  vector<Comment> comments;       // as we can have many comments..
  unsigned int identifier;        // so the server can tell me if this worked or not.. 

  //int commentField;
  //QString comment;
  
  ishImageSet(QImage* i, int pi, QString prom, int expid, int tid, QString fname, int cField, QString cmmnt, unsigned int id){
    image = i;
    probeId = pi;
    promoter = prom;
    experimentId = expid;
    tissueId = tid;
    fileName = fname;
    comments.push_back(Comment(cField, cmmnt));
    identifier = id;
  }
  ishImageSet(QImage* i, int pi, QString prom, int expid, int tid, QString fname, vector<Comment> cmmnts, unsigned int id){
    image = i;
    probeId = pi;
    promoter = prom;
    experimentId = expid;
    tissueId = tid;
    fileName = fname;
    comments = cmmnts;
    identifier = id;
  }
};

#endif
