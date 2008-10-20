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

#ifndef LOCALREGIONWIDGET_H
#define LOCALREGIONWIDGET_H

#include "../client.h"
#include <qwidget.h>
#include <qrect.h>
#include <vector>
#include <qsize.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qpixmap.h>
#include <string>
#include <iostream>
#include "../sequence/sequenceRequest.h"

using namespace std;

// first a couple of rectangles that we can place all over the drawing,, -so we can link a click on the map to one of these guys. 
// we will need some sort of an index as well that we can use to check what's what, but we can do that later.

struct Thresholds {
  int* maxExpect;
  int* minLength;
  float* minMatch;
  Thresholds(int* me, int* ml, float* mm){
    maxExpect = me;
    minLength = ml;
    minMatch = mm;
  }
  Thresholds(){
    maxExpect = minLength = 0;
    minMatch = 0;
  }
};

struct action {
  bool redraw;
  bool reposition;
  action(bool d, bool p){
    redraw = d;
    reposition = p;
  }
};

class FeatureBox {
  public :
    enum FeatureType {
      BASE, PSET, EGENE, TRANSCRIPT, REGION, DNA, ISHBOX, SLIDER, TYPE_SELECTOR
    };
  
  
  virtual ~FeatureBox()=0;          // can I have a virtual destructor. ?? 
  FeatureBox();
  FeatureBox(QRect r, int i);       // needs to have some specific index ..
  virtual FeatureType rtid(){               // for realtime identification.. 
    return(BASE);
  }
  QRect& rect(){              // return a pointer so we don't have to copy.. 
    return(box);
  }
  void setRect(QRect r){
    box = r;
  }
  virtual void drawBox(QPainter* p, bool keepState=false);   // we don't need to know.. where
  virtual void shift(int dy);               // move the box..  -- returns distance shifted,, -and allows for some silly expressions.. 
  virtual void checkBox();                  // if box has no width.. then make it a bit bigger.. 
  virtual bool intersects(QRect& r){        // does it intersect.. 
    return(r.intersects(box));
  }
  bool contains(QPoint& p){
    return(box.contains(p));
  }
  virtual action mouseClick(QMouseEvent* e);
  virtual action mouseMove(QMouseEvent* e);
  virtual action mouseRelease(QMouseEvent* e);
  
  protected :
    QRect box;
  int index;
  int id;
  // bool selected;
};

class PSBox : public FeatureBox {
  public :
    ~PSBox();
  //PSBox();
  PSBox(QRect b, int i, probeSetMatch* m, Thresholds trs);         // do I really need to call these ?? -- need to set the id.. 
  void drawBox(QPainter* p, bool keepState=false);    // override.. 
  FeatureType rtid(){
    return(PSET);
  }
  probeSetMatch* match;
  Thresholds thresholds;   // 
  bool active;     // if it will be drawn or not.. 
};

class IshBox : public FeatureBox {
  public :
    IshBox(QRect bx, int i, ishProbeMatchSet& mathcSet, float range, int yinit, int ymargin, int b, int e, int w, int lm);  // complicated.. 
  ~IshBox();
  void drawBox(QPainter* p, bool keepState=false);
  void shift(int dy);
  FeatureType rtid(){
    return(ISHBOX);
  }
  vector<QRect> matches;    // individual matching sequences, These are still a complete mess, but I have a program to sort them out with..
  ishProbeMatchSet* match;
};

class EGBox : public FeatureBox {                           // this will actually contain a series of boxes,, -one which covers everything, and one which covers individual exons.. 
  public :
    EGBox();                            // needed to set the id.. 
  ~EGBox();
  EGBox(QRect b, ensemblTranscript* t, int g=0);
  //  gBox(ensemblTranscript* t);
  FeatureType rtid(){
    return(EGENE);
  }
  action mouseClick(QMouseEvent* e);
  void drawBox(QPainter* p, bool keepState=false);
  void shift(int dy); 
  void checkBox();
  ensemblTranscript* transcript;
  
  private :
    //  const ensemblTranscript* transcript;
    vector<QRect> codeExons;
  vector<QRect> exons;    // expensive, but.. draw exons first, then put codeExons on top of those.. 
  bool isCurrent;
  //  int type;   // gene, transcript, exon, codeExon.. -- but coding exon does not have an index, just a string id. -hmm trouble. for now ignore and call 0. 
  // gene 1, transcript 2, exon 3, cd Exon 0,, -as I don't have any id's for it.. 
};

class TranscriptBox : public FeatureBox {
  public :
    TranscriptBox(QRect b, Transcript& trans);    // and work everything out from that.... 
  
  void drawBox(QPainter* p, bool keepState=false);
  void shift(int dy);
  FeatureType rtid(){
    return(TRANSCRIPT);
  }
  
  Transcript* transcript;
  private :
    vector<QRect> exons;
  string source;
  string transcriptId;
};

struct DNABox : public FeatureBox {        // for selecting a region of DNA for saving at a later state..
  ~DNABox();
  DNABox(){
    index = -1;
    id = 20004;
    start = stop = 0;
    deleteMe = false;
    showNumbers = false;
  }
  DNABox(QRect r, int st, int sp){
    box = r;
    start = st;
    stop = sp;
    firstX = firstY = 0;
    lastX = 0;
    deleteMe = false;
    showNumbers = false;
  }
  friend bool operator <(const DNABox& a, const DNABox& b){
    return(&a < &b);
  }
  FeatureType rtid(){
    return(DNA);
  }
  action mouseClick(QMouseEvent* e);
  action mouseMove(QMouseEvent* e);
  action mouseRelease(QMouseEvent* e);

  void drawBox(QPainter* p, bool keepState=false);
  int start;    // should be private with accessors, but I'm tired, and seems likely that forcing function calls everywhere might be a bad idea. 
  int stop;
  int lastX;    // to handle mouse moves.. necessary to remember last one.. 
  int firstX, firstY;    // if no mouse movement, try to get rid of myself.. 
  bool deleteMe;
  bool showNumbers;      // if show numbers.. it's useful.. 
};

class RegionBox : public FeatureBox {
  public :
    ~RegionBox();
    RegionBox(){
    index = 0;
    id = 20003;
    selected = (bool*)&index;   // hmm, but don't ever use this --- shouldn't die, but.. hmm
    selection = 0;
    lm = w = wstart = wend = 0;
  }
  RegionBox(QRect r, int st, int rIndex, bool* sel, int lmargin, int width, int ws, int we, int sh, set<DNABox*>* selRegions=0){  // the box, the strand, and the region Index.. -- set the location of the boxes inside the drawing function.. 
    selection = selRegions;
    strand = st;
    index = rIndex;
    id = 20003;
    box = r;
    selected = sel;
    lm = lmargin;
    w = width;
    wstart = ws;
    wend = we;
    selHeight = sh;
  }
  FeatureType rtid(){
    return(REGION);
  }
  int strnd(){
    return(strand);
  }
  void drawBox(QPainter* p, bool keepState=false);
  bool* selected;   // kept by the relevant construct.. 
  action mouseClick(QMouseEvent* e);

  private :
    int strand;
  int lm, w, wstart, wend;    // parameters of the current window. These are needed to correctly place DNA boxes and give their appropriate locations..
  int selHeight;              // how high to make a selection
  set<DNABox*>* selection;    // so that I can make new DNA boxes when clicked on.. 
};


class DNASlider : public FeatureBox {
  public :
    DNASlider(QRect bx, int wb, int we){      /// who takes care of the resising of the box.. 
    innerBox = bx;
    box = QRect(innerBox.left()-2, innerBox.top(), innerBox.width() + 4, innerBox.height()); 
    index = -1;
    windowBegin = wb;
    windowEnd = we;
  }
  void drawBox(QPainter* p, bool keepState=false);
  FeatureType rtid(){
    return(SLIDER);
  }
  void shift(int dy);

  private :
    int windowBegin, windowEnd;
  QRect innerBox;
};

class TypeSelector : public FeatureBox {
  public :
    TypeSelector(QRect bx, string tpe, set<string>* itypes, bool sel=false){
    type = tpe;
    includedTypes = itypes;
    selected = sel;     // get's toggled by clicking..-- but we should override.. 
    box = bx;
  }
  void drawBox(QPainter* p, bool keepState=false);
  FeatureType rtid(){
    return(TYPE_SELECTOR);
  }
  action mouseClick(QMouseEvent* e);    // make sure things are set appropriately.. 
  private :
    set<string>* includedTypes;
  string type;
  bool selected;
};

class RegionDrawing {       // contains a load of boxes, and some values regarding the max and min positions of that box.. and a box that surrounds. it.. 
  public :
    RegionDrawing(set<string>* ttypes, set<string>* itypes, Thresholds trs){       // doesn't really need very much..
    tm = rm = bm = lm = 40;
    boxes.resize(0);
    region = 0;
    includedTypes = itypes;
    transcriptTypes = ttypes;
    thresholds = trs;
    setVariables();
  }
  RegionDrawing(int leftMargin, int rightMargin, int topMargin, int botMargin, set<string>* ttypes, set<string>* itypes, Thresholds trs){
    tm = leftMargin;
    rm = rightMargin;
    tm = topMargin;
    bm = botMargin;
    region = 0;
    boxes.resize(0);
    thresholds = trs;
    setVariables();
    includedTypes = itypes;
    transcriptTypes = ttypes;
  }
  ~RegionDrawing(){
    //cout << "Region Drawer destructor boxes size is : " << boxes.size() << endl;
    for(uint i=0; i < boxes.size(); i++){
      //cout << " ------ deleting i : " << i << endl;
      delete boxes[i];
    }
    boxes.resize(0);    // probably has no effect.. but .. 
  }
  void setPSet(int p){
    currentProbe = p;
    rePosition();
  }
  //void rePosition(chromRegion* cr, int width, int g, int p);    // w being the width. p being the probe set id.. g being the gene id..
  void rePosition(chromRegion* cr, int b, int e, int width, int g, int p);    // w being the width. p being the probe set id.. g being the gene id..
  void rePosition(int db, int de);    // db and de are delta pixels,, i.e. when moving the slider thingy.. 
  void rePosition(int w);
  void rePosition();
  void shift(int dy);      // move in the y axis,, -requires moving all of the boxes within it to the appropriate location..
  void draw(QPainter* p);  // call draw on all of the boxes... 
  void startSelect(int x, int y);    // widget coordinates.. 
  void moveSelect(int x, int y);
  void endSelect(int x, int y);
  int maxy();
  int miny();
  set<int> selectedProbeSets();
  
  bool contains(QPoint p){
    return(box.contains(p));
  }

  FeatureBox* findBox(QPoint p);        // returns 0 if not contained or if no boxes there..
  set<genomicRequest> genomeRequests();  // return a set of genomic requests.. 

  private :
    chromRegion* region;
  QRect box;
  QRect slidePole;             // something for the slider to slide on.. 
  QRect selectBox;
  bool selecting;              // for selections.. 
  int selX, selY;              // origin of selection
  vector<FeatureBox*> boxes;   // make these using new, so that we don't have to copy things too much.. 
  set<DNABox*> selectedRegions;  // DNA Regions that have been selected, these are added automatically by the RegionBox.. 
  set<string>* transcriptTypes;  // Available transcript types... 
  set<string>*  includedTypes;    // types that we don't want to draw... 
  Thresholds thresholds;          // a struct of pointers to ints containing thresholds... 
  int tm, bm, lm, rm;
  int geneHeight, ishHeight, probeHeight;   // heights of different kinds of marker things.. 
  int slideHeight;
  int featureMargin;
  int DNAWidth;
  int selectionHeight;
  int maxY, minY;                          // gets set during the repositioning.. 
  int windowWidth, currentGene, currentProbe;
  //  int begin, end, windowWidth, currentGene, currentProbe;
  int strandPos;      // ugly, there because of bad design.. 

  bool drawDNA;
  bool topStrand;
  bool botStrand;      // whether or not to draw stuff. 

  void positionSelections(int ypos);     // somehow or other it seems we need to do this.. 
  int addEnsGene(ensemblGene& gene, float range, int yinit, int up, int b, int e, int w, int g);
  int addIshProbe(ishProbeMatchSet& match, float range, int yinit, int up, int b, int e, int w);
  int addTranscript(Transcript& trans, float range, int y, int b, int e, int w, uint tb);   // tb is the place where the transcript boxes begin.. 
  void setVariables();
  set<int> displayedProbeSets(vector<probeSetMatch>* matches);

};
  

class LocalRegionWidget : public QWidget
{
  Q_OBJECT

    public :
    LocalRegionWidget(vector<chromRegion>* r, int ws, QWidget* parent=0, const char* name=0, bool isIshish=false);
  void newSize(int w, int h, bool forceResize=false);

  QSize sizeHint() const;

  public slots :
    void newData(int i, int g);     // redraw if visible, otherwise, do nothing. 
  void setSelectRegion(int ss, int se);   // change the region which is actually drawn,, -maybe should be private slot.. 
  void setSource(vector<chromRegion>* r);
  void setPSet(int p){
    currentProbeSet = p;
    for(uint i=0; i < drawings.size(); i++){
      drawings[i]->setPSet(p);
    }
    newSize(width(), height(), true);    
  }
  void setWindowSize(int i){
    windowSize = i;
  }
  void setMaxExpect(int i){
    maxExpect = i;
    reDraw = true;
    update();
  }
  void setMinLength(int i){
    minLength = i;
    reDraw = true;
    update();
  }
  void setMinMatch(int i){
    minMatch = (float)i/(float)100;
    reDraw = true;
    update();
  }

  private :
    void paintEvent(QPaintEvent* e);
  void drawRect(QRect r, QPainter* p);
  void shiftDrawings();
  void shiftDrawings(int w);
  
  bool reDraw;
  vector<chromRegion>* regions;
  vector<RegionDrawing*> drawings;   // collections of drawings.. 
  RegionDrawing* windowChanging;     // if the window is changing.. 
  RegionDrawing* selectingRegion;   // maintain as 0 if we are selecting a region.. 
  FeatureBox* selectedBox;          // if there is a selected box.. 
  RegionDrawing* selectedRegion;    // if we need to pass a call for repositioning.. 
  set<DNABox*> DNABoxes;     // for selecting regions..  -- use pointers -- keep these for a while.. 
  set<string> transcriptTypes;   // to allow selection of which transcript types are displayed... 
  set<string> includedTypes;    
  int typeHeight;
  int typeMargin; // just for drawing.. 
  map<string, TypeSelector*> typeSelectors;   // for selecting the types with..  the actual boxes, draw at the end.. 

  //vector<int> selectBegin;
  //vector<int> selectEnd;             // these need to be different for each region as we need to be able to selectively to things to them..

  QPopupMenu* popMenu; 
  QPopupMenu* ishImageMenu;           // what to do with ishImages man.. 
  QPixmap* pixmap;                    // for drawing on so that we know.. 
  int currentlySelectedIshProbe;     // so I know which images to load .. 

  /// alignment criteria.. 
  int maxExpect;         // the exponent of the thingy..
  int minLength;
  float minMatch;        // the ratio.. 

  // some values used in drawing..
  int lm, rm, tm, bm, fm, regM;          // margins.. 
  int minY, maxY;                        // maximum and minimum y values of the boxes and stuff.  -- maybe I don't need this.. 

  int windowSize;                        // the requested window size..  (.i.e part of the region)
  int currentProbeSet;                   // the currentProbeSet.. !!!   
  int guessGene;                         // the dbIndex of the bestGuess gene.. from probeSet.. it sucks.. 
  QSize recommendedSize;
  // then we should have some functions that pick up mouse interactions and stuff, now we'll just try to draw it..
  int pressX, pressY, lastX, lastY;
  vector<int> dnaPos;   // the y positions of the dna lines. use to indicate whether or not we draw a selection box.. !!
  QRect regionSelecter;
  int selectionId;      // i.e. which region are we selecting.. anyway.. 
  bool sliderMoving;
  int movingSlider;   // again which one is being selected.. 
  bool inSlideRegion;
  bool DNABoxChanging;
  DNABox* changingBox;   // the changing box.. 
  int currentlySelectedGene;  // or at least the last one we clicked on.. 
  //int mouseState;
  
  private slots :
    void getSequences();
  void displaySequences();
  void makeSequenceRequest(bool saveFile);
  void loadProbeSets();
  void nameProbe();
  void associateWithGene();
  void addImages();
  void getThumbnails();
  void getImages();    // big ones.. full size... 
  void copyFigure();

  protected :
    //virtual void resizeEvent(QResizeEvent* e);
    void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);

  signals :
    void displayInfo(int);    // display info about this gene.. 
  void displayTranscriptInfo(string, string);
  void loadProbeSetsPlease(vector<int>, QString);  // load these probe sets.. 
  void collectSequences(SequenceRequest*);
  void pleaseLoadImages(int);
  void pleaseLoadThumbnails(int);    // these two are for in situ images for a given probe...have to rewrite the server soon, but let's connect things.. 
  void associateIshProbeWithGene(int);  // unfortunately this widget doesn't know which ishProbe, that is for the receiver to work out.
  void addImagesFor(int);               // add images for the currently selected probe... 
};

#endif


