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

#include "localRegionWidget.h"
#include "../client.h"
#include "../sequence/sequenceRequest.h"
#include <qwidget.h>
#include <qrect.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qfont.h>
#include <qstring.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qfont.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

FeatureBox::FeatureBox(){
  index = -1;
  id = 0;
  //  selected = false;
}

FeatureBox::FeatureBox(QRect r, int i){
  index = i;
  id = 20000;
  //selected = false;
  box = r;
}

FeatureBox::~FeatureBox(){
  //cout << "deleting a Feature Box: " << endl;
}

void FeatureBox::checkBox(){
 if(!box.width()){
    box.setRect(box.left(), box.top(), 1, box.height());
  }
}

void FeatureBox::shift(int dy){
  box.moveBy(0, dy);
}

void FeatureBox::drawBox(QPainter* p, bool keepState){
  if(!keepState){
    p->save();     // save the state and over turn .. 
    p->setPen(QPen(QColor(255, 255, 150), 1));
    p->setBrush(QColor(0, 0, 255));     // blue and yellow  !!
  }
  p->drawRect(box);
  if(!keepState){
    p->restore();
  }
}

action FeatureBox::mouseClick(QMouseEvent* e){
  return(action(false, false));
}

action FeatureBox::mouseMove(QMouseEvent* e){
  //cout << "Feature Box mouse Move : " << endl;
  return(action(false, false));
}

action FeatureBox::mouseRelease(QMouseEvent* e){
  return(action(false, false));
}



// PSBox::PSBox(){
//   index = -1;
//   id = 20001;
//   match = 0;    /// don't use this, as it's profoundly dangerous.. 
// }

PSBox::PSBox(QRect b, int i, probeSetMatch* m, Thresholds trs){
  box = b;
  index = i;
  //  selected = false;
  id = 20001;
  match = m;
  thresholds = trs;
}

PSBox::~PSBox(){
  //  cout << "Deleting probe set box " << endl;
}

void PSBox::drawBox(QPainter* p, bool keepState){
  //  cout << "Drawing PSET box top " << box.top() << "\tleft " << box.left() << "\twidth " << box.width() << endl;
  // first determine if we want to draw this or not.. 
  int expLog = -500;
  if(match->expect){    // larger than 0.
    int expLog = (int)log10(match->expect);
  }
  if(expLog <= *thresholds.maxExpect && *thresholds.minLength <= match->alLength && (float)match->match/(float)match->alLength > *thresholds.minMatch){
    active = true;
  }else{
    active = false;
    return;
  }
  int r, g;
  r = (1024 * match->match)/ match->alLength - 769;
  if(r < 0){ r = 0; }
  g = 255 - r;
  if(!keepState){
    p->save();
    p->setPen(Qt::NoPen);   // hmm 
    p->setBrush(QColor(r, g, 0));    // this should be dependent on some factor, but later..
  }
  p->drawRect(box);
  if(!keepState){
    p->restore();
  }
}

IshBox::IshBox(QRect bx, int i, ishProbeMatchSet& matchSet, float range, int yinit, int ymargin, int b, int e, int w, int lm){
  box = bx;
  index = i;
  //selected = false;
  id = 20005;
  match = &matchSet;
  int h = box.height();
  // and now for the complicated thing.. 
  int y;
  if(matchSet.strand == 1){
    y = yinit - ymargin - h;
    //cout << "\tdecreasing y " << endl;
  }else{
    y = yinit + ymargin;
  }
  vector<ishProbeMatch>::iterator it;
  for(it = matchSet.ishMatches.begin(); it != matchSet.ishMatches.end(); it++){
    //cout << "ISH BOXXXXXXXXXXXXXXXX STRAND is " << (*it).strand << endl;
    //cout << "\t\tyinit in this one is : " << yinit << endl;
   //  if((*it).strand == 1){
//       y = yinit - ymargin - h;
//       //cout << "\tdecreasing y " << endl;
//     }else{
//       y = yinit + ymargin;
//       //cout << "decreasing y " << endl;
//     }
    //cout << "AND now y is : " << y << endl;
    int x1 = lm + w * (((float)((*it).cStart - b))/range);
    int mw = w * (((float)((*it).cEnd - (*it).cStart))/range);
    if(!mw){ mw = 1; }
    matches.push_back(QRect(x1, y, mw, h));
  }
}

void IshBox::shift(int dy){
  for(int i=0; i < matches.size(); i++){
    matches[i].moveBy(0, dy);
  }
  box.moveBy(0, dy);
}

IshBox::~IshBox(){
  //cout << "deleting ISH Box " << endl;
}

void IshBox::drawBox(QPainter* p, bool keepState){
  //  cout << "\t\t\tISH box being drawn, what's going on here top : " << box.top() << "  left : " << box.left() << " and width : " << box.width()  << endl;
  p->save();
  p->setPen(Qt::NoPen);
  p->setBrush(QColor(255, 0, 255));
  for(int i=0; i < matches.size(); i++){
    p->drawRect(matches[i]);
  }
  p->setPen(QPen(QColor(255, 255, 255), 1));   // hmm 
  p->setBrush(Qt::NoBrush);    // 
  p->drawRect(box);
  p->restore();
}

EGBox::EGBox(){
  //  transcript = 0;
  transcript = 0;
  id = 20002;
  index = -1;
  isCurrent=false;
  //
}

EGBox::EGBox(QRect b, ensemblTranscript* t, int g){
  id = 20002;
  box = b;
  index = -1;     // don't use this for this one,, well, maybe, work it out later.. 
  //transcript = t;  // hmmm.. hmmmm 
  transcript = t;
  isCurrent = false;
  if(g == transcript->geneIndex){    // override.. 
    isCurrent = true;
  }
  // and set up the QRect boxes ... for the exons..
  int bl = box.left();
  int bw = box.width();
  int top = box.top();
  int h = box.height();
  int tl = t->stop - t->start;
  //cout << "defining exons : " << endl;
  for(int i=0; i < t->exons.size(); i++){
    //cout << "\texon " << i << endl;
    //cout << "bw : " << bw << "\texon start : " << t->exons[i].start << "\tstop : " << t->exons[i].stop << "\t length : " << tl << endl;
    int left = bl + (bw*(t->exons[i].start - t->start))/tl;
    int w = (bw*(t->exons[i].stop - t->exons[i].start))/tl;
    //cout << "\tdefined paramters : " << endl;
    exons.push_back(QRect(left, top, w, h));
    if(t->exons[i].codeStart && t->exons[i].codeStop){
      //cout << "\t\tCoding Region Exon " << endl;
      left = bl + (bw*(t->exons[i].codeStart - t->start))/tl;  
      w = (bw*(t->exons[i].codeStop - t->exons[i].codeStart))/tl;
      codeExons.push_back(QRect(left, top, w, h));
    }
  }
}

EGBox::~EGBox(){
  //cout << "Deleting EGBox " << endl;
}

void EGBox::shift(int dy){
  for(int i=0; i < exons.size(); i++){
    exons[i].moveBy(0, dy);
  }
  for(int i=0; i < codeExons.size(); i++){
    codeExons[i].moveBy(0, dy);
  }
  box.moveBy(0, dy);
}

void EGBox::drawBox(QPainter* p, bool keepState){
  //cout << "Drawing Ensembl Transcript : left " << box.left() << "\ttop : " << box.top() << endl;
  p->save();
  p->setPen(Qt::NoPen);
  p->setBrush(QColor(147, 45, 120));
  for(int i=0; i < exons.size(); i++){
    p->drawRect(exons[i]);
  }
  p->setBrush(QColor(255, 255, 0));
  for(int i=0; i < codeExons.size(); i++){
    p->drawRect(codeExons[i]);
  }

  if(transcript->rSelected){
     p->setBrush(QColor(15, 200, 15));
     p->setRasterOp(Qt::XorROP);
     p->drawRect(box);
     p->setRasterOp(Qt::CopyROP);
  }
  if(transcript->pSelected){
     p->setBrush(QColor(15, 15, 219));
     p->setRasterOp(Qt::XorROP);
     p->drawRect(box);
     p->setRasterOp(Qt::CopyROP);
  }
  p->setPen(QPen(QColor(255, 255, 255), 1));
  if(isCurrent){
    p->setPen(QPen(QColor(255, 0, 0), 1));
  }
  p->setBrush(Qt::NoBrush);
  p->drawRect(box);
  p->restore();
}

action EGBox::mouseClick(QMouseEvent* e){
  switch(e->button()){
  case Qt::LeftButton :
    //cout << "We should somehow tell the parent to look at this information " << endl;
    // add another parameter for the action perhaps.. 
    break;
  case Qt::RightButton :
    transcript->rSelected = !transcript->rSelected;
    break;
  case Qt::MidButton :
    transcript->pSelected = !transcript->pSelected;
    break;
  default :
    cerr << "Unknown button " << endl;
  }
  return(action(true, false));
}
  
RegionBox::~RegionBox(){
  //cout << "deleting a Region Box " << endl;
}

void EGBox::checkBox(){
  if(!box.width()){
    box.setRect(box.left(), box.top(), 1, box.height());
  }
  for(int i=0; i < exons.size(); i++){
    if(!exons[i].width()){
      exons[i].setRect(exons[i].left(), exons[i].top(), 1, exons[i].height());
    }
  }
  for(int i=0; i < codeExons.size(); i++){
    if(!codeExons[i].width()){
      codeExons[i].setRect(codeExons[i].left(), codeExons[i].top(), 1, codeExons[i].height());
    }
  }
}

TranscriptBox::TranscriptBox(QRect b, Transcript& trans){
  index = 0;
  id = 20006;     // ??
  source = trans.source;
  transcriptId = trans.id;
  transcript = &trans;
  box = b;
  int bl = box.left();
  int bw = box.width();
  int top = box.top();
  int h = box.height();
  int tl = trans.cend - trans.cstart;
  //cout << "defining exons : " << endl;
  for(int i=0; i < trans.exons.size(); i++){
    int left = bl + (bw*(trans.exons[i].cstart - trans.cstart))/tl;
    int w = (bw*(trans.exons[i].cend - trans.exons[i].cstart))/tl;
    //cout << "\tAdding an Exon : cstart " << trans.exons[i].cstart << "\tto " << trans.exons[i].cend << " left : " << left << "\tright : " << left + w << endl; 
    //cout << "\tdefined paramters : " << endl;
    if(!w){ w = 1; }
    exons.push_back(QRect(left, top, w, h));
  }
}

void TranscriptBox::shift(int dy){
  box.moveBy(0, dy);
  for(int i=0; i < exons.size(); i++){
    exons[i].moveBy(0, dy);
  }
}

void TranscriptBox::drawBox(QPainter* p, bool keepState){
  //cout << "Drawing Transcript : " << box.left() << "  to : " << box.right() << endl;
  p->save();
  p->setPen(Qt::NoPen);
  p->setBrush(QColor(0, 0, 255));
  for(int i=0; i < exons.size(); i++){
    p->drawRect(exons[i]);
  }
  p->setPen(QPen(QColor(200, 200, 0), 1));
  p->setBrush(Qt::NoBrush);
  p->drawRect(box);
  p->restore();
}

void RegionBox::drawBox(QPainter* p, bool keepState){
  // ignore keepState..
  //cout << "regionbox drawing strand : " << strand << endl; 
  //cout << "box top is : " << box.top() << "  left : " << box.left() << "  width : " << box.width() << "  height : " << box.height() << endl;
  //cout << "AND SELECTED IS : " << selected << endl;
  if(!(*selected)){
    //cout << "\t\treturning : " << endl;
    return;
  }
  p->save();
  p->setPen(Qt::NoPen);
  switch(strand){
  case -1 :
    p->setBrush(QColor(86, 56, 108));
    break;
  case 1 :
    p->setBrush(QColor(19, 57, 108));
    break;
  case 0 :
    p->setBrush(QColor(255, 255, 255));
    break;
  default :
    p->setBrush(QColor(0, 200, 200));     // eh.... 
  }
  p->drawRect(box);
  p->restore();
}

action RegionBox::mouseClick(QMouseEvent* e){
  if(!strand){        // strand is 0 always draw..
    if(!selection){                     // i.e. we don't have access to the set of things in here.. hmmm
      return(action(false, false));
    }
    // we want to make a new DNA box. -- we need to work out some new parameters for it, as in where to put it and so on..
    float scale = ((float)(wend-wstart))/((float)w);
    int midPos = wstart + (e->x() - lm) * scale;    // will give warnings in the compile but should be ok.. 
    int pixRange = 50;     // arbitrary 100 pixel start..
    int bpRange = pixRange * scale;
    //cout << "Calling The DNABox constructor : " << endl
    //	 << "wstart : " << wstart << "\twend : " << wend << "\t w : " << w << "\tscale : " << scale << "\tbprange : " << bpRange << "\t midPos : " << midPos << endl;
    QRect selBox(e->x() - pixRange, box.top() + (box.height() - selHeight)/2, pixRange*2, selHeight);
    DNABox* dnaBox = new DNABox(selBox, midPos-bpRange, midPos+bpRange);
    selection->insert(dnaBox);
    return(action(true, false));
  }
  *selected = !(*selected);
  return(action(true, false));
}

void DNABox::drawBox(QPainter* p, bool keepState){
  // ignore keep state...
  p->save();
  p->setPen(QPen(QColor(201, 44, 193), 2));
  p->drawLine(box.left(), box.top(), box.left(), box.bottom());
  p->drawLine(box.right(), box.top(), box.right(), box.bottom());
  int middle = box.top() + box.height()/2;
  p->drawLine(box.left(), middle, box.right(), middle);
  if(showNumbers){
    QFont currentFont = p->font();
    p->setFont(QFont("Arial", currentFont.pointSize()-1));
    p->setPen(QPen(QColor(255, 255, 255), 1));
    QString num;
    num.setNum(start);
    int twidth = 100;
    int theight = 20;   // just the box.. 
    p->drawText(box.left(), box.top() - theight - 2, twidth, theight, Qt::AlignLeft|Qt::AlignBottom, num);
    num.setNum(stop);
    p->drawText(box.right()-twidth, box.bottom() + 2, twidth, theight, Qt::AlignRight|Qt::AlignTop, num);
  }
  p->restore();
}

action DNABox::mouseClick(QMouseEvent* e){
  lastX = e->x();
  firstX = e->x();
  firstY = e->y();     // so I can detect no movement.. 
  return(action(false, false));
}

action DNABox::mouseMove(QMouseEvent* e){
  //cout << "DNA Box mouse Move : " << endl;
  //cout << "lastX " << lastX << "  x " << e->x() << endl;
  int db = 0; 
  int de= 0;   // delta begin, and delta end..
  int dpb = 0;
  int dpe = 0;  // delta pixel begin and end..
  if(e->state() == Qt::LeftButton){
    dpb = e->x() - lastX;
  }
  if(e->state() == Qt::RightButton){
    dpe = e->x() - lastX;
  }
  if(e->state() == Qt::MidButton || e->state() == Qt::LeftButton + Qt::ControlButton){
    dpe = e->x() - lastX;
    dpb = e->x() - lastX;
  }
  // work out the changes to the 
  float scale = (float)(stop-start) / (float)(box.right() - box.left());
  db = dpb * scale;
  de = dpe * scale;
  // and update values..  // though we should check..
//   cout << "db = " << db << "\tde " << de << endl;
//   cout << "dpb = " << dpb << "\tdpe " << dpe << endl;
//   cout << "box right is : " << box.right() << endl
//        << "box left  is : " << box.left() << endl
//        << "box width is : " << box.width() << endl;
  


  int x = box.left() + dpb;
  int boxWidth = box.width() - dpb + dpe;
  if(boxWidth > 30){
    start = start + db;
    stop = stop + de;
    //cout << "box right is : " << box.right() << "\tboxWidth : " << boxWidth << endl
    //	 << "box left  is : " << box.left() << "\tx : " << x << endl;
    //int y = box.top();
    //int h = box.height();
    box.setRect(x, box.top(), boxWidth, box.height());
    lastX = e->x();
    return(action(true, false));
  }
  //cout << "Returning without anythying,, " << endl;
  return(action(false, false));   // nothing changed..
}

action DNABox::mouseRelease(QMouseEvent* e){
  // do nothing for now,, later check if no movement try do get rid of myself, somehow..
  //cout << "Mouse Release function of DNA box firstX : " << firstX << "  firstY = " << firstY << "  x : " << e->x() << "   y : " << e->y() << endl; 
  if(e->x() == firstX && e->y() == firstY){
    if(e->state() == Qt::LeftButton){
      deleteMe = true;
      //cout << "\tdelete me is true : " << endl;
    }
    if(e->state() == Qt::RightButton){
      showNumbers = !showNumbers;
    }
    return(action(true, false));
  }
  return(action(false, false));
}
		
    


//gBox::gBox(ensemblTranscript* t){
//transcript = t;
//  rSelected = false;
//  pSelected = false;
//}

DNABox::~DNABox(){
  //cout << "deleting a DNABox " << endl;
}

void DNASlider::drawBox(QPainter* p, bool keepState){
  //cout << "Drawing DNA BOX from " << box.left() << "  to : " << box.right() << "  at level " << box.top() << endl;
  p->save();
  p->setPen(Qt::NoPen);
  p->setBrush(QColor(240, 0, 0));
  p->drawRect(box);
  p->setBrush(QColor(136, 164, 201));
  p->drawRect(innerBox);
  // we should also draw in the numbers in the appropriate form here, but as I don't know the font,, we can wait.. do later..
  QFont currentFont = p->font();
  p->setFont(QFont("Arial", currentFont.pointSize()-1));
  p->setPen(QPen(QColor(255, 255, 255), 1));
  QString num;
  num.setNum(windowBegin);
  int twidth = 100;
  int theight = 20;   // just the box.. 
  p->drawText(innerBox.left(), innerBox.top() - theight - 2, twidth, theight, Qt::AlignLeft|Qt::AlignBottom, num);
  num.setNum(windowEnd);
  p->drawText(innerBox.right()-twidth, innerBox.bottom() + 2, twidth, theight, Qt::AlignRight|Qt::AlignTop, num);
  p->restore();
}

void DNASlider::shift(int dy){
  box.moveBy(0, dy);
  innerBox.moveBy(0, dy);
}

void TypeSelector::drawBox(QPainter* p, bool keepState){
  p->save();
  p->setBrush(Qt::NoBrush);
  p->setPen(QPen(QColor(225, 0, 0), 1));
  p->drawRect(box);
  if(selected){
    p->drawLine(box.left(), box.top(), box.right(), box.bottom());
    p->drawLine(box.left(), box.bottom(), box.right(), box.top());
  }
  QFont currentFont = p->font();
  p->setFont(QFont("Arial", currentFont.pointSize()-1));
  p->setPen(QPen(QColor(255, 255, 255), 1));
  p->drawText(box.right() + 2, box.bottom(), type.c_str());    // no so many, so just draw at the bottom of the thingy.. hmm, this is not quite right is it.. but..
  p->restore();
}

action TypeSelector::mouseClick(QMouseEvent* e){
  selected = !selected;
  if(selected){
    includedTypes->insert(type);
  }else{
    includedTypes->erase(type);    // hope this is ok.. on Windows as well.. 
  }
  return(action(true, false));
}

int RegionDrawing::miny(){
  return(minY);
}

int RegionDrawing::maxy(){
  return(maxY);
}

void RegionDrawing::setVariables(){
  geneHeight = 10;
  ishHeight = 8;
  probeHeight = 6;
  featureMargin = 3;
  DNAWidth = 2;
  slideHeight = 8;
  selectionHeight = 2 * probeHeight + DNAWidth;
  windowWidth = currentGene = currentProbe = 0;
  drawDNA = true;
  topStrand = false;
  botStrand = false;
  selecting = false;
}    

void RegionDrawing::startSelect(int x, int y){
  selecting = true;
  selX = x;
  selY = y;
  selectBox.setRect(x, y, 0, 0);
}

void RegionDrawing::moveSelect(int x, int y){
  selectBox.setRect(selX, selY, x-selX, y-selY);
}

void RegionDrawing::endSelect(int x, int y){
  selecting = false;
  // and work out how to change begin and end, and reposition.. or something like that...
  int minDist = 30;    // don't do unless distance betseen start and stop is larger than some value
  if(abs(x-selX) < 30){
    return;
  }
  float w = (float)(windowWidth - lm - rm);
  float b = ((float)(selX - lm))/w;
  float e = ((float)(x - lm))/w;
  float range = ((float)(region->selectEnd - region->selectBegin));
  //cout << "w : " << w << "\tb : " << b << "\te : " << e << "\tbegin : " << begin << "\tend : " << end << endl;
  if(b > e){
    float tb = b;
    b = e;
    e = tb;
  }
  region->selectEnd = region->selectBegin + e * ((float)range);
  region->selectBegin = region->selectBegin + b * ((float)range);
  //cout << "\t\tbegin : " << begin << "\tend : " << end << endl;
  rePosition();           // ugly maths, but should work.. 
}

void RegionDrawing::rePosition(int db, int de){               // delta begin, and delta end respectively..
  // de and db are in pixels, we have to relate this to base pairs.. but should be relatively easy..
  float w = (float)(windowWidth - lm - rm);     // this is the width of the slide pole representing everything..
  float range = (float)(region->chromEnd - region->chromStart);
  int db_nuc = db * (int)(range / w);
  int de_nuc = de * (int)(range / w);
  /// no checking, let's just force it... through..
  //cout << "rePosition db : " << db << "\tde : " << de << "\tw : " << "\tdb_nuc : " << db_nuc << "\tde_nuc : " << de_nuc << endl;

  int tempbegin = region->selectBegin + db_nuc;
  int tempend = region->selectEnd + de_nuc;
  if(tempbegin < tempend && tempbegin >= region->chromStart && tempend <= region->chromEnd){
    region->selectBegin = tempbegin;
    region->selectEnd = tempend;
  }
  //cout << "\tbegin : " << begin << "\tend : " << endl;
  rePosition();
}

void RegionDrawing::rePosition(int w){
  rePosition(region, region->selectBegin, region->selectEnd, w, currentGene, currentProbe);
}

void RegionDrawing::rePosition(){
  rePosition(region, region->selectBegin, region->selectEnd, windowWidth, currentGene, currentProbe);
}

void RegionDrawing::rePosition(chromRegion* cr, int b, int e, int width, int g, int p){             // hmm, is it possible for this to crash.. ??
  // first delete all of the old regions.. 
  // p is the current probe, g is the current gene thing.. need to know in order to draw with red outline.. 
  //cout << "Region Drawing, calling rePosition " << endl;
  //cout << "Boxes size is : " << boxes.size() << endl;
  //  cout << "Calling reposition with begin : " << b << "   and end : " << e << endl;
  if(region != cr){        // new region, better delete selections..
    while(selectedRegions.size()){
      DNABox* ptr = (*selectedRegions.begin());
      delete ptr;
      selectedRegions.erase(selectedRegions.begin());
    }
  }
  region = cr;
  //begin = b;
  //end = e;
  region->selectBegin = b;
  region->selectEnd = e;
  windowWidth = width;
  currentGene = g;
  currentProbe = p;
  for(int i=0; i < boxes.size(); i++){
    //cout << "deleting box # " << i << endl;
    delete boxes[i];
  }
  boxes.resize(0);
  //cout << "boxes deleted and resized " << endl;
  int w = width - lm - rm;
  float range = (float)(e -b);           // this is the select begin and select end.. 
  minY = 0;                          // this ok, as we will have some negative values.. 
  maxY = 0;             
  int ypos = 0;

  // First Forward Genes.. draw from the bottom  to the top, assume genes don't overlap.. 
  for(int i=0; i < cr->fEnsGenes.size(); i++){
    addEnsGene(cr->fEnsGenes[i], range, ypos, -1, b, e, w, g);
  }
  // let's do forward matches to fantom transcripts.. 
  ypos += featureMargin;
  multimap<string, Transcript>::iterator tit;
  uint transcriptBegin = boxes.size();
  for(tit = cr->fTranscripts.begin(); tit != cr->fTranscripts.end(); tit++){
    transcriptTypes->insert((*tit).first);    // should be the transcript type.. 
    int tempy = addTranscript((*tit).second, range, ypos, b, e, w, transcriptBegin);
    //if(tempy > ypos){ ypos = tempy; }
    
  }
  ypos = maxY;
  //ypos += geneHeight + featureMargin;
  // Then let's do Ish probes. Anyway, these will start at 0, and we'll move down from there..
  // for the moment we'll draw all of these at the 0 line.. and see how it goes.. 
  ypos += featureMargin;
  map<int, ishProbeMatchSet>::iterator it;
  for(it = cr->fIshMatches.begin(); it != cr->fIshMatches.end(); it++){
    int y = addIshProbe((*it).second, range, ypos, -1, b, e, w);
  }
  
  // and Now let's start darying things properly.. we need to start incrementing ypos..
  ypos += featureMargin + probeHeight + ishHeight;
  RegionBox* topStrandBox = new RegionBox(QRect(lm, ypos, w, probeHeight), 1, 1, &topStrand, lm, w, b, e, selectionHeight);   // the second 1 is obsolete and should be changed.
  RegionBox* botStrandBox = new RegionBox(QRect(lm, ypos + probeHeight, w, probeHeight), -1, 1, &botStrand, lm, w, b, e, selectionHeight); 
  RegionBox* DNABox = new RegionBox(QRect(lm, ypos + probeHeight - DNAWidth/2, w, DNAWidth), 0, 1, &drawDNA, lm, w, b, e, selectionHeight,  &selectedRegions);  // strand is 0, draw a thingy number. 
  //DNABox->setSelected(true);     // well.. 
  strandPos = ypos + probeHeight - DNAWidth/2;    // need for drawing the number... .. 
  // let's go through any selected boxes that we might have here..
  positionSelections(strandPos);
//   set<DNABox*>::iterator pit;
//   for(pit=selectedRegions.begin(); pit != selectedRegions.end(); pit++){
//     int x1 = lm + w * (((float)((*pit)->start - b))/range);
//     int w = w * (((float)((*pit)->end - (*pit)->start))/range);
//     int y = ypos + (DNAWidth - selectionHeight)/2;
//     (*pit)->setRect(QRect(x1, y, w, selectionHeight));
//   }


  //cout << "done the RegionBoxes ypos is : " << ypos << endl;
  // the DNA line (strand 0 ) should be drawn after the two regions, and then the other stuff is drawn..
  // soo simply.. 
  boxes.push_back(topStrandBox);
  boxes.push_back(botStrandBox);
  boxes.push_back(DNABox);            // which should give the appropriate thiny.. 

  // and let's draw the forward probe Set matches..
  for(int i=0; i < cr->fPMatches.size(); i++){
    int x1 = lm + w * (((float)(cr->fPMatches[i].cStart - b))/range);
    int x2 = lm + w * (((float)(cr->fPMatches[i].cEnd - b))/range);
    int height = cr->fPMatches[i].afIndex == p ? 2*probeHeight : probeHeight;
    PSBox* psbox = new PSBox(QRect(x1, ypos-(height-probeHeight), x2-x1, height), cr->fPMatches[i].afIndex, &cr->fPMatches[i], thresholds);
    psbox->checkBox();
    boxes.push_back(psbox);
  }
  // and then the reverse probe Set Matches.. but now easier..
  ypos += probeHeight;
  for(int i=0; i < cr->rPMatches.size(); i++){
    int x1 = lm + w * (((float)(cr->rPMatches[i].cStart - b))/range);
    int x2 = lm + w * (((float)(cr->rPMatches[i].cEnd - b))/range);
    int height = cr->rPMatches[i].afIndex == p ? 2*probeHeight : probeHeight;
    PSBox* psbox = new PSBox(QRect(x1, ypos, x2-x1, height), cr->rPMatches[i].afIndex, &cr->rPMatches[i], thresholds);
    psbox->checkBox();
    boxes.push_back(psbox);
  }
  ypos += featureMargin + 2*probeHeight;
  // and then the reverse ish matches .. 
  for(it = cr->rIshMatches.begin(); it != cr->rIshMatches.end(); it++){
    int y = addIshProbe((*it).second, range, ypos, 1, b, e, w); 
  }
  ypos += featureMargin + ishHeight;
  maxY = ypos;       // in case there's nothing below..
  transcriptBegin = boxes.size();
  for(tit = cr->rTranscripts.begin(); tit != cr->rTranscripts.end(); tit++){
    int tempy = addTranscript((*tit).second, range, ypos, b, e, w, transcriptBegin);
    //if(tempy > ypos){ ypos = tempy; }
  }
  ypos = maxY;
  //  ypos += featureMargin + geneHeight;
 
  // and then the tricky stuff.. 
  for(int i=0; i < cr->rEnsGenes.size(); i++){
    int y = addEnsGene(cr->rEnsGenes[i], range, ypos, 1, b, e, w, g);   // but maybe I don't need the y.. !! 
  }
  // let's define a box for the sliding region..
  ypos = maxY;
  ypos += featureMargin + 35;

  slidePole.setRect(lm, ypos, w, DNAWidth);
  // and make a SLIDER for it..
  float cRange = float(cr->chromEnd - cr->chromStart);
  int x1 = lm + w * (((float)(b - cr->chromStart))/cRange);
  int l = w * (((float)(e - b))/cRange);
  //cout << "b : " << b << "\te: " << e << "\tchromStart : " << cr->chromStart << "\tchromRange : " << cRange << endl;

  DNASlider* slider = new DNASlider(QRect(x1, ypos - (slideHeight - DNAWidth)/2, l, geneHeight), b, e);
  boxes.push_back(slider);

  ypos += 10;
  if(ypos > maxY){ maxY = ypos; }
  
  //// ok we still haven't done anything about selected regions, but perhaps we can do that later. It's alright...
  maxY += bm;
  minY -= tm;
  box.setRect(0, minY, width, maxY-minY);

  //  set<DNABox*>::iterator spit;

  //  cout << "max y is now : " << maxY << endl;
  //cout << "min y is now : " << minY << endl;
  //cout << "from function  miny : " << miny() << endl;
  //cout << "from function  maxy : " << maxy() << endl;
  /// We still haven't specifed the amazing sliding thingy,, but what can you do eh.. that's for later when we've ironed out the 
  /// bugs.. and other stuff.. --- also need to take into account things like minimum expectation and other stuff.. in a more organised way.
  /// which probably means sticking pointers in the constructs,, -I've got no way of checking this at the moment.. 

}

void RegionDrawing::positionSelections(int ypos){
  //cout << "Changing the position selections.. " << endl;
  set<DNABox*>::iterator pit;
  float w = (float)(windowWidth - lm - rm);
  float range = (float)(region->selectEnd - region->selectBegin);
  for(pit=selectedRegions.begin(); pit != selectedRegions.end(); pit++){
    int x1 = lm + (int)(w * (((float)((*pit)->start - region->selectBegin))/range));
    int wid = (int)(w * (((float)((*pit)->stop - (*pit)->start))/range));
    int y = ypos + (DNAWidth - selectionHeight)/2;
    //cout << "Selection Start : " << (*pit)->start << "\tEnd : " << (*pit)->stop << endl
    //	 << "Window    Start : " << begin << "\tEnd : " << end << endl;
    //cout << "Moving rectange to : " << x1 << ", " << y << "\t Width : " << wid << "\t height : " << selectionHeight << endl;
    (*pit)->setRect(QRect(x1, y, wid, selectionHeight));
  }
}

int RegionDrawing::addEnsGene(ensemblGene& gene, float range, int yinit, int up, int b, int e, int w, int g){
  // cout << "Adding EnseGene : b : " << b << "  e : " << e << "  yinit : " << yinit << endl;
  if(up < 0){
    yinit = yinit + up * geneHeight;    // which in fact is a subtraction.
  }
  int y = yinit;
  for(int i=0; i < gene.transcripts.size(); i++){
    //cout << "\tTranscript : " << i << endl;
    // ok. let's just make one box for now which contains the gene. This will take care of itself anyway..
    ensemblTranscript* etrans = &gene.transcripts[i];
    y = yinit + up * i * (geneHeight + featureMargin);
    int x1 = lm + w * (((float)(etrans->start - b))/range);
    int x2 = lm + w * (((float)(etrans->stop - b))/range);
    //cout << "\t\t\ttrans start : " << etrans->start << "  stop : " << etrans->stop << endl;
    //cout << "\t\t\ty: " << y << "\tx : " << x1 << "\t x2 " << x2 << endl;
    EGBox* geneBox = new EGBox(QRect(x1, y, x2-x1, geneHeight), etrans, g);
    geneBox->checkBox();
    boxes.push_back(geneBox);
    if(y < minY){ minY = y; }
    if(y > maxY){ maxY = y; }
  }
  //y += (geneHeight + featureMargin);
  //if(y < minY){ minY = y; }
  //if(y > maxY){ maxY = y; }
  return(y);
}

int RegionDrawing::addTranscript(Transcript& trans, float range, int y, int b, int e, int w, uint tb){
  if(!includedTypes->count(trans.source)){
    return(y);
  }
  int x1 = lm + w * (((float)(trans.cstart - b))/range);
  int mw = w * (((float)(trans.cend - trans.cstart))/range);
  //cout << "Adding A transcript id : " << trans.id << " cstart :" << trans.cstart << "\tcend : " << trans.cend  << "\tx1 : " << x1 << "\tx2 " << x1 +mw << endl;
  TranscriptBox* tbox = new TranscriptBox(QRect(x1, y, mw, geneHeight), trans);
  bool intersect = true;
  while(intersect){
    intersect = false;
    for(uint i=tb; i < boxes.size(); i++){
      if(boxes[i]->intersects(tbox->rect())){
	intersect = true;
	break;
      }
    }
    if(intersect){
      tbox->shift(featureMargin + geneHeight);
    }
  }
  y = tbox->rect().top();
  boxes.push_back(tbox);
  y += featureMargin + geneHeight;   // make some space.. 
  if(y < minY){ minY = y; }
  if(y > maxY){ maxY = y; }
  return(y);
}

int RegionDrawing::addIshProbe(ishProbeMatchSet& match, float range, int yinit, int up, int b, int e, int w){
  int x1 = lm + w * (((float)(match.cStart - b))/range);
  int x2 = lm + w * (((float)(match.cEnd - b))/range);
  int dnaY;
  int yMargin = featureMargin + (2 * probeHeight);
  //cout << "UPUPUP IS EQUAL TO : " << up << endl;
  if(up == -1){     // top strand.. strange I know, but it's true..
    dnaY = yinit + featureMargin + ishHeight + 2 * probeHeight;
  }else{
    dnaY = yinit - (2 * probeHeight + featureMargin);
  }
  //cout << "Yiniti is " << yinit << "\t and dnaY is " << dnaY << endl;
  IshBox* ishBox = new IshBox(QRect(x1, yinit, x2-x1, ishHeight), match.probeIndex, match, range, dnaY, yMargin, b, e, w, lm);   // Needs more for the boxes as well..
  ishBox->checkBox();
  //  cout << "\tx1 : " << x1 << "\tx2 : " << x2 << endl;
  boxes.push_back(ishBox); 
  return(yinit);
}

void RegionDrawing::shift(int dy){
  for(int i=0; i < boxes.size(); i++){
    boxes[i]->shift(dy);
  }
  // and 
  for(set<DNABox*>::iterator it = selectedRegions.begin(); it != selectedRegions.end(); it++){
    (*it)->shift(dy);
  }
  box.moveBy(0, dy);
  slidePole.moveBy(0, dy);
  minY += dy;
  maxY += dy;
  strandPos += dy;
}
			     
void RegionDrawing::draw(QPainter* p){
  p->save();
  p->setBrush(QColor(255, 255, 255));
  p->setPen(Qt::NoPen);
  p->drawRect(slidePole);
  for(int i=0; i < boxes.size(); i++){
    //cout << "\t\t\tdrawing box # " << i << endl;
    boxes[i]->drawBox(p);
  }
  // let's draw the RegionBoxes if there are any..
  for(set<DNABox*>::iterator it = selectedRegions.begin(); it != selectedRegions.end(); it++){
    if(!(*it)->deleteMe){
      (*it)->drawBox(p);
    }else{
      DNABox* ptr = *it;
      selectedRegions.erase(it);
      delete ptr;
    }
  }

  p->save();
  p->setPen(Qt::NoPen);
  p->setBrush(QColor(0, 0, 0));   // let's remove stuff..
  QRect r(0, minY, lm, maxY-minY);
  p->drawRect(r);
  r.setRect(box.width()-rm, minY, rm, maxY-minY);
  p->drawRect(r);

  //lets draw the thing.. 
  p->setPen(QPen(QColor(255, 255, 255), 1));
  QString chrom(region->chromosome.c_str());
  int theight = 30;
  int twidth = 30;
  p->drawText(lm-3-twidth, strandPos + (DNAWidth - theight)/2, twidth, theight, Qt::AlignRight|Qt::AlignVCenter, chrom);
  

  // and finally let's draw a selecting box if I'm selecting..
  if(selecting){
    p->setPen(QPen(QColor(255, 255, 255), 1));
    p->setBrush(Qt::NoBrush);
    p->drawRect(selectBox);
  }
  p->restore();
}

FeatureBox* RegionDrawing::findBox(QPoint p){
  // use reverse order.. to allow probe set matches to be identified.. -- as these overlap with the
  // the region boxes but must be drawn after.. they are always after the DNA boxes.. 
  set<DNABox*>::iterator it;
  for(it = selectedRegions.begin(); it != selectedRegions.end(); it++){
    if((*it)->contains(p)){
      return(*it);
    }
  }
  for(int i=boxes.size()-1; i >=0; i--){
    if(boxes[i]->contains(p)){
      return(boxes[i]);
    }
  }
  return(0);
}

set<int> RegionDrawing::displayedProbeSets(vector<probeSetMatch>* matches){
  set<int> index;
  for(vector<probeSetMatch>::iterator it = matches->begin(); it != matches->end(); it++){
    if((*it).cStart < region->selectEnd && (*it).cEnd > region->selectBegin){
      // check if things ok.. 
      int expLog = -500;
      if((*it).expect){    // larger than 0.
	int expLog = (int)log10((*it).expect);
      }
      if(expLog <= *thresholds.maxExpect && *thresholds.minLength <= (*it).alLength && (float)(*it).match/(float)(*it).alLength > *thresholds.minMatch){
	index.insert((*it).afIndex);
      }
    }
  }
  return(index);
}

set<int> RegionDrawing::selectedProbeSets(){
  // check values of botStrand and topStrand..
  set<int> selected;
  if(topStrand){
    set<int> sel = displayedProbeSets(&region->fPMatches);
    selected.insert(sel.begin(), sel.end());
  }
  if(botStrand){
    set<int> sel = displayedProbeSets(&region->rPMatches);
    selected.insert(sel.begin(), sel.end());
  }
  return(selected);
}
      
set<genomicRequest> RegionDrawing::genomeRequests(){
  set<genomicRequest> requests;
  for(set<DNABox*>::iterator it = selectedRegions.begin(); it != selectedRegions.end(); it++){
    requests.insert(genomicRequest(region->chromosome, (*it)->start, (*it)->stop));
  }
  return(requests);
}
      

LocalRegionWidget::LocalRegionWidget(vector<chromRegion>* r, int ws, QWidget* parent, const char* name, bool isIshish)
  : QWidget(parent, name)
{
  regions = r;
  windowSize = ws; 
  reDraw = true;
  
  lm = 35;
  rm = 35;
  tm = 40;
  bm = 30;

  fm = 14;     // vertical feature margin
  regM = 80;   // vertical region Margin.. 
  typeHeight = 10;
  typeMargin = 3;

  windowChanging = 0;
  selectingRegion = 0;
  selectedBox = 0;
  selectedRegion = 0;
  pixmap = new QPixmap(size());
  currentProbeSet = 0;
  guessGene = 0;
  currentlySelectedGene = 0;

//   selectBegin.resize(regions->size());
//   selectEnd.resize(regions->size());
//   for(int i=0; i < regions->size(); i++){
//     selectBegin[i] = (*regions)[i].chromStart;
//     selectEnd[i] = (*regions)[i].chromEnd;
//   }
  recommendedSize = QSize(400, 100);  // -- doesn't really matter..
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));

  //  setPaletteBackgroundColor(QColor(0, 0, 0));
  setBackgroundMode(NoBackground);
  //cout << "Construction of localRegionWidget done " << endl;
  selectingRegion = 0;
  sliderMoving = false;
  inSlideRegion = false;
  
  popMenu = new QPopupMenu(this);
  popMenu->insertItem("Save Sequences", this, SLOT(getSequences()), CTRL+Key_S, 1);
  popMenu->insertItem("Display Sequences", this, SLOT(displaySequences()), CTRL+Key_D, 2);
  popMenu->insertItem("Load ProbeSets", this, SLOT(loadProbeSets()), CTRL+Key_L, 3);
  
  popMenu->insertItem("Copy", this, SLOT(copyFigure()));
  ishImageMenu = new QPopupMenu(this);
  if(isIshish){
    ishImageMenu->insertItem("Name Probe", this, SLOT(nameProbe()), CTRL+Key_N, 4);
    ishImageMenu->insertItem("Associate with Gene", this, SLOT(associateWithGene()), CTRL+Key_G, 5);
    ishImageMenu->insertItem("Add Image", this, SLOT(addImages()) );
  }else{
    ishImageMenu->insertItem("Load Thumbnails", this, SLOT(getThumbnails()), CTRL+Key_T, 1);
    //ishImageMenu->insertItem("Load Images (big!)", this, SLOT(getImages()), CTRL+Key_I, 2);
  }
  currentlySelectedIshProbe = 0;
}

void LocalRegionWidget::setSource(vector<chromRegion>* r){
  regions = r;
  //  update();
}

void LocalRegionWidget::shiftDrawings(){
  shiftDrawings(width());
}

void LocalRegionWidget::shiftDrawings(int w){        // need to give a width.. 
  int top = 0;
  for(int i=0; i < drawings.size(); i++){
    // set miny to be 0.. 
    // cout << "Shifting a drawing by : " << top - drawings[i]->miny() << endl;
    drawings[i]->shift(top - drawings[i]->miny());
    top = drawings[i]->maxy();    // the new max y..
  }
  int h = height();
  if(drawings.size()){
    if(h < (drawings.back()->maxy() + transcriptTypes.size() * (typeHeight + typeMargin)) ){   // make space for the drawings.. 
      h = drawings.back()->maxy() + transcriptTypes.size() * (typeHeight + typeMargin);
    }
  }
  recommendedSize = QSize(w, h);
}

void LocalRegionWidget::newSize(int w, int h, bool forceUpdate){
  // we need to go throught the date again and reposition the boxes.. 
  //cout << "New Size calling things... " << endl;
  for(int i=0; i < drawings.size() && i < regions->size(); i++){
    drawings[i]->rePosition(w);
    //    drawings[i]->rePosition(&(*regions)[i], selectBegin[i], selectEnd[i], w, currentlySelectedGene, currentProbeSet);
  }
  shiftDrawings(w);
  //cout << "Drawings, shifted, do something else.. " << endl;
  reDraw = true;
  if(forceUpdate){
    resize(recommendedSize);
    update();
  }
  //update();   // ?? don't update here, because we expect to get an update through a resize.. 
  // as this should be called before any resize events, we do not want to call an update at this point.. 
}

void LocalRegionWidget::newData(int i, int g){
  //cout << "newData in localregionwidget" << endl;
  currentlySelectedGene = g;      // no guarantees that this will actually be the thing.. that's going on.. 
  selectingRegion = 0;
  sliderMoving = false;
  DNABoxChanging = false;
  changingBox = 0;
  //cout << "receiving a newData signal i is " << i << endl;
  currentProbeSet = i;
  guessGene = g;
  //cout << "and the guess gene is g " << g << endl; 
  //selectBegin.resize(regions->size());
  //selectEnd.resize(regions->size());
  //  dnaPos.resize(regions->size());
  //slideRegions.resize(regions->size());
  //sliders.resize(regions->size());
  //  regionBoxes.resize(0);    // one for each strand.. 
  for(int i=0; i < drawings.size(); i++){
    delete drawings[i];
  }
  drawings.resize(0);
  //  drawings.resize(regions->size());
  while(DNABoxes.size()){ 
    set<DNABox*>::iterator it = DNABoxes.begin();
    DNABox* pointer = *it;
    DNABoxes.erase(it); 
    delete pointer;
  }
  maxY = 0;
  minY = 0;        /// make boxes in negative space if appropriate.. then 
  int currentY = 0;
  int w = width()-lm-rm;
  for(int i=0; i < regions->size(); i++){
    int range = (*regions)[i].chromEnd - (*regions)[i].chromStart;
    if(!range){
      continue;
    }
    // cout << "regions select Begin : " << (*regions)[i].selectBegin << "  chromstart : " << (*regions)[i].chromStart << endl;
    if(!(*regions)[i].selectBegin){      // set to 0,, on initialiser, so if not set, then set range, otherwise leave alone.. 
      if(windowSize > range){
	(*regions)[i].selectBegin = (*regions)[i].chromStart;
	(*regions)[i].selectEnd = (*regions)[i].chromEnd;
	//      selectBegin[i] = (*regions)[i].chromStart;
	//selectEnd[i] = (*regions)[i].chromEnd;
      }else{
	(*regions)[i].selectBegin = ((*regions)[i].chromStart + (*regions)[i].chromEnd - windowSize)/2;
	(*regions)[i].selectEnd = ((*regions)[i].chromStart + (*regions)[i].chromEnd + windowSize)/2;
	//      selectBegin[i] = ((*regions)[i].chromStart + (*regions)[i].chromEnd - windowSize)/2;
	//selectEnd[i] = ((*regions)[i].chromStart + (*regions)[i].chromEnd + windowSize)/2;
      }
    }
    //cout << "Calling Reposition on drawing # " << i << endl;
    drawings.push_back(new RegionDrawing(&transcriptTypes, &includedTypes, Thresholds(&maxExpect, &minLength, &minMatch)));          // 
    // drawings.push_back(new RegionDrawing(lm, rm, tm, bm));          // 
    drawings[i]->rePosition(&(*regions)[i], (*regions)[i].selectBegin, (*regions)[i].selectEnd, width(),  currentlySelectedGene, currentProbeSet);
    //    drawings[i]->rePosition(&(*regions)[i], selectBegin[i], selectEnd[i], width(),  currentlySelectedGene, currentProbeSet);
    //cout << "MinY is now : " << drawings[i]->miny() << "\tmaxY : " << drawings[i]->maxy() << endl;
  }
  cout << "and made the drawing.. " << endl;
  // go through and shift the drawings.. 
  int top = 0;
  for(int i=0; i < drawings.size(); i++){
    // set miny to be 0.. 
    //cout << "Shifting a drawing by : " << top - drawings[i]->miny() << endl;
    drawings[i]->shift(top - drawings[i]->miny());
    top = drawings[i]->maxy();    // the new max y..
  }
  if(drawings.size()){
    recommendedSize = QSize(width(), drawings.back()->maxy() + transcriptTypes.size() * (typeHeight + typeMargin));
  }else{
    recommendedSize = QSize(width(), height());
  }
  reDraw = true;
  cout << "end of set data .. " << endl;
}

QSize LocalRegionWidget::sizeHint() const {
  cout << "returning recommended size " << endl;
  return(recommendedSize);
}

void LocalRegionWidget::setSelectRegion(int ss, int se){
  // for now do nothing.. maybe we will use it later..
}

void LocalRegionWidget::paintEvent(QPaintEvent* e){
  //cout << "painting local region widget with name : " << name() << "  and parents name : " << parent()->name() << endl
  //    << "   and great grandparent " << parent()->parent()->parent()->name() << endl;
  // the first thing we need to do is to work out how much space we need, -- i.e. 
  // the amount of vertical space required to draw the thing...
  // naah, that's done in the new data thing, which should be called before there is any new data.
  // arrghh. We will assume at the moment that we have enough space..
  //cout << "localRegionWidget paintEvent : " << endl;
  //psboxes.resize(0);
  //ishProbeBoxes.resize(0);
  //gboxes.resize(0);
  //int transcriptCounter = 0;

  if(reDraw){
    QFont currentFont = font();
    //cout << "resizing pixmap to : " << width() << " x " << height() << endl;
    pixmap->resize(size());
    pixmap->fill(QColor(0, 0, 0));
    
    int dnaWidth = 2;
    QPen dnaPen(QColor(255, 255, 255), dnaWidth);
    int psHeight = 6;
    int geneHeight = 9;
    int transcriptHeight = 9;
    int w = width()-lm-rm;
    
    int currentHeight = 2+fm+tm;    //  // needed for the penwidth..  
    // first let's draw the genes,, 
    QPainter* p = new QPainter(pixmap);
    //  cout << "painting for the region " << endl;
    for(int i=0; i < drawings.size(); i++){
      drawings[i]->draw(p);
    }
    if(selectingRegion){
      p->setBrush(Qt::NoBrush);
      p->setPen(QPen(QColor(255, 255, 255), 0));
      p->drawRect(regionSelecter);
    }
    // and let's see if we have some transcript types to draw...
    int ypos = 0;
    if(drawings.size()){
      ypos = drawings.back()->maxy();
    }
    for(set<string>::iterator it = transcriptTypes.begin(); it != transcriptTypes.end(); it++){
      // see if we need to make a new transcriptBox..
      map<string, TypeSelector*>::iterator mit = typeSelectors.find(*it);
      if(mit == typeSelectors.end()){
	TypeSelector* sel = new TypeSelector(QRect(lm, ypos, typeHeight, typeHeight), (*it), &includedTypes, false);
	sel->drawBox(p);
	typeSelectors.insert(make_pair((*it), sel));
      }else{
	(*mit).second->setRect(QRect(lm, ypos, typeHeight, typeHeight));
	(*mit).second->drawBox(p);
      }
      ypos += (typeHeight + typeMargin);
    }
    delete p;
  }
  reDraw = false;
  bitBlt(this, 0, 0, pixmap, 0, 0);
  //cout << "finished painting local region alignment " << endl;
}

void LocalRegionWidget::mousePressEvent(QMouseEvent* e){
  //cout << "Received a mouse event. Check where that mouse event is and perform the appropriate action.." << endl;
  FeatureBox* box = 0;
  RegionDrawing* drawing = 0;
  lastX = e->x();
  lastY = e->y();
  QPoint point = e->pos();   
  for(int i=0; i < drawings.size(); i++){
    if(drawings[i]->contains(e->pos())){
      drawing = drawings[i];
      box = drawings[i]->findBox(e->pos());
      break;
    }
  }
  if(!drawing){
    // check the click is in a selector box...
    for(map<string, TypeSelector*>::iterator it=typeSelectors.begin(); it != typeSelectors.end(); it++){
      if((*it).second->contains(point)){
	//cout << "Clicked on a Type Selector do something " << endl;
	(*it).second->mouseClick(e);    // returns an action struct, but basically we don't care..
	newSize(width(), height(), true);    // causes a reposition, a shift and an update.. so we can return here..
	return;
      }
    }
    return;
  }
  selectedRegion = drawing;
  if(!box){
    // if button is right button, then let's take control...
    if(e->button() == Qt::RightButton){
      popMenu->popup(mapToGlobal(e->pos()));
    }
    if(e->button() == Qt::LeftButton){
      selectingRegion = drawing;    // let's call a function in this one.. to notify... 
      drawing->startSelect(e->x(), e->y());
    }
    return;
  }

  ////////////////// DIRTY HACK... CAN'T USE SELECTED BOX IF WE HAVE REPOSITIONINGS AS THE 
  /////////////////  POINTERS CAN GET CHANGED... HMMM, THIS IS TROUBLE SOME.. -WE CAN USE FOR THE DNA BOXES,, ONLY 
  /////////////////  AS THESE ONLY CHANGE WHEN WE GET NEW DATA.. BUT THIS IS NO LONG TERM SOLUTION.. 
  if(box->rtid() == FeatureBox::DNA){
    selectedBox = box;
  }

  ///////////////////////// DIRTY HACK END.... ////////////////////

  //cout << "Calling Action : " << endl;
  action act = box->mouseClick(e);
  if(act.reposition){
    // we need some sort of function that does this nicely.. without the risk of 
    // memory segmentations..
    reDraw = true;
    //cout << "act.reposition is true,, do something :" << endl;
  }
  if(act.redraw){
    //cout << "calling update " << endl;
    reDraw = true;
    update();
  }
  // so now we can just go ahead and to a rtid dirty switch..
  // This is ugly, ideally we should be able to rely on the mouse press function, but it is difficult at times to 
  // do that.. unless we provide a pointer to our own structure,, that could be used to access our functions ....
  // in a call back style thing.. oh, well, never mind.. 
  switch(box->rtid()){
  case FeatureBox::BASE : 
    //cout << "Base Feature Clicked on.. " << endl;
    break;
  case FeatureBox::PSET :
    //cout << "Probe Set clicked on " << endl;
    break;
  case FeatureBox::EGENE :
    //cout << "Ensembl Transcript clicked on " << endl;
    if(e->button() == Qt::LeftButton){              // it would be better to handle this using a virtual function, but I can't think of a clean way at the moment
      currentlySelectedGene = ((EGBox*)box)->transcript->geneIndex;
      emit displayInfo(((EGBox*)box)->transcript->geneIndex);
    }
    break;
  case FeatureBox::TRANSCRIPT :
    if(e->button() == Qt::LeftButton){
      emit displayTranscriptInfo(((TranscriptBox*)box)->transcript->source, ((TranscriptBox*)box)->transcript->id);    // this seems a bit silly as we could just emit a pointer instead.. which shouldn't make the whole thing any more unstable.. hmm
    }
    //cout << "Transcript clicked on " << endl;
    break;
  case FeatureBox::REGION :
    //cout << "Region clicked on " << endl;
    break;
  case FeatureBox::DNA :
    //cout << "DNA thingy clicked on,, but we don't have one yet.. " << endl;
    break;
  case FeatureBox::ISHBOX :
    //cout << "ISHbox thingy clicked on,, doo suru.. " << endl;
    if(e->button() == Qt::RightButton){
      currentlySelectedIshProbe = ((IshBox*)box)->match->probeIndex;
      ishImageMenu->popup(mapToGlobal(e->pos()));
    }
    break;
  case FeatureBox::SLIDER :
    //cout << "SLider pressed on,, do something for god's sake.. ohhhh.. " << endl;
    windowChanging = drawing;
    break;
  default :
    cerr << "Unknown type pressed on, what's up.. " << endl;
  }
//   cout << "press x " << e->x() << "\ty " << e->y() << endl;
//   // check if press is inside any of the slider boxes..
//   for(int i=0; i < sliders.size(); i++){
//     //cout << "going through the sliders " << endl;
//     if(slideRegions[i].contains(e->pos())){ 
//       cout << "\t\t\tSETTING INSLIDEREGION to TRUE" << endl;
//       inSlideRegion = true; 
//     }
//     if(sliders[i].contains(e->pos())){
//       sliderMoving = true;
//       movingSlider = i;
//       break;
//       //cout << "and the moving slider is number " << i << endl;
//     }
//   }
//   DNABoxChanging = false;  // the default
//   changingBox = 0;         // just to make sure. 
//   for(set<DNABox*>::iterator it=DNABoxes.begin(); it != DNABoxes.end(); it++){
//     cout << "going through the DNABoxes, to see if I have any overlaps.. " << endl;
//     if((*it)->box.contains(e->pos())){
//       changingBox = *it;
//       DNABoxChanging = true;
//       cout << "got an overlap witha DNABox,, och, have set a whole load of things " << endl; 
//       break;
//     }
//   }
//   lastX = pressX = e->x();
//   lastY = pressY = e->y();
}

void LocalRegionWidget::mouseMoveEvent(QMouseEvent* e){
  // this is only used for changing the size of the window through different ways..
  if(selectedBox){
    //cout << "calling mouseMove on selectedBox,, " << endl;
    action act = selectedBox->mouseMove(e);
    //cout << "action returned : " << endl;
    if(act.reposition){
      //cout << "reposition is true, but we are doing nothing " << endl;
      // still haven't worked this one out.. doesn't make sense.. necessarily as we don't know which region this is from
    }
    if(act.redraw){
      reDraw = true;
      update();
    }
  }
  if(windowChanging){
    //cout << "Window changing is true.. and so we do something  : " << endl;
    reDraw = true;
    int db, de;
    db = de = 0;
    //cout << "MOUSE MOVE EVENT " << lastX << "\t" << e->x() << endl;
    if(e->state() == Qt::LeftButton){
      db = e->x() - lastX;
      //cout << "\tleft button " << endl;
    }
    if(e->state() == Qt::RightButton){
      de = e->x() - lastX;
      //cout << "\tright button " << endl;
    }
    if(e->state() == Qt::MidButton || e->state() == Qt::LeftButton + Qt::ControlButton){
      db = e->x() - lastX;
      de = e->x() - lastX;
      //cout << "\tmid button " << endl;
    }
    // hmm, that's a bit clumsy, but..
    windowChanging->rePosition(db, de);
    shiftDrawings();          // ugly, but.. it is possible in the future that the drawings will change size.. 
    update();
  }
  if(selectingRegion){
    selectingRegion->moveSelect(e->x(), e->y());
    reDraw = true;
    update();
  }
  /// don't forget  ..
  lastX = e->x();
  lastY = e->y();

  //cout << "move x " << e->x() << "\ty " << e->y() << endl;
//   bool change = false;
//   if(e->state() == Qt::LeftButton){
//     if(!selectingRegion && !sliderMoving && !inSlideRegion && !DNABoxChanging){   // check if we should be..
//       for(int i=0; i < dnaPos.size(); i++){
// 	if(abs(pressY - dnaPos[i]) < 50 && abs(pressY- dnaPos[i]) > 3){
// 	  selectingRegion = true;
// 	  selectionId = i;
// 	}
//       }
//     }
//     if(selectingRegion){
//       regionSelecter.setRect(pressX, pressY, e->x()-pressX, e->y()-pressY);
//       change = true;
//     }
//   }
//   if(sliderMoving){
//     //cout << "slider is moving" << endl;
//     if(movingSlider < regions->size()){
//       //cout << "and the array size seems to be ok " << endl;
//       int i = movingSlider;
//       float cRange = (float)((*regions)[i].chromEnd - (*regions)[i].chromStart);
//       float delta = (float)(e->x() - lastX)/(float)(width()-lm-rm);   // the percentage change..
//       int bpdelta = (int)(delta * cRange);
//       //cout << "delta is " << delta << "\tand bpdelta is " << bpdelta << endl;
//       int newBegin = selectBegin[i] + bpdelta;
//       int newEnd = selectEnd[i] + bpdelta;
//       //cout << "newBegin is " << newBegin << endl;
//       //cout << "newEnd is " << newEnd << endl;
//       //cout << "selectBegin (old) is " << selectBegin[i] << endl;
//       //cout << "chrom range begin is " << (*regions)[i].chromStart << endl;
//       if(newBegin >= (*regions)[i].chromStart && newEnd <= (*regions)[i].chromEnd && newBegin < newEnd){
// 	if(e->state() == Qt::MidButton || e->state() == Qt::LeftButton + Qt::ControlButton){
// 	  selectBegin[i] = newBegin;
// 	  selectEnd[i] = newEnd;
// 	  change = true;
// 	}
//       }
//       if(newBegin >= (*regions)[i].chromStart && e->state() == Qt::LeftButton){
// 	selectBegin[i] = newBegin;
// 	change = true;
// 	//cout << "setting selectBegin [i] to newBegin " << endl;
//       }
//       if(newEnd <= (*regions)[i].chromEnd && e->state() == Qt::RightButton){
// 	selectEnd[i] = newEnd;
// 	change = true;
//       }
//     }
//   }
//   if(DNABoxChanging && changingBox){
//     //    cout << "calculating movement for DNABox" << endl;
//     int dx = e->x() - lastX;
//     float dratio = (float)dx/(float)(width()-lm-rm);
//     int index = changingBox->regionIndex;
//     float chromRange = (float)(selectEnd[index]-selectBegin[index]);
//     int bpDelta = (int)(dratio * chromRange);
//     //cout << "dx : " << dx << "\tdratio : " << dratio << "\tbpDelta " << bpDelta << endl;
//     change = true;
//     if(e->state() == Qt::MidButton || e->state() == Qt::LeftButton + Qt::ControlButton){
//       // change both the beginning and the ending.. 
//       changingBox->start += bpDelta;
//       changingBox->stop += bpDelta;
//     }
//     if(e->state() == Qt::LeftButton){
//       changingBox->start += bpDelta;
      
//     }
//     if(e->state() == Qt::RightButton){
//       changingBox->stop += bpDelta;
//     }
//   }
//   if(change){
//     repaint();
//   }
//   lastX = e->x();
//   lastY = e->y();
}

void LocalRegionWidget::mouseReleaseEvent(QMouseEvent* e){
  if(selectingRegion){
    selectingRegion->endSelect(e->x(), e->y());
    reDraw = true;
    shiftDrawings();
    update();
  }
  if(selectedBox){
    //cout << "Calling mouseRelease on selectedBox : " << endl;
    action act = selectedBox->mouseRelease(e);
    if(act.reposition){
      // do something..
    }
    if(act.redraw){
      reDraw = true;
      update();
    }
  }
  selectedBox = 0;
  windowChanging = 0;
  selectingRegion = 0;
  selectedRegion = 0;
//   cout << "release  x " << e->x() << "\ty " << e->y() << endl;
//   bool change = false;
//   sliderMoving = false;
//   inSlideRegion = false;
//   DNABoxChanging = false;
//   changingBox = 0;
//   if(selectingRegion){
//     selectingRegion = false;
//     change = true;
//     if(abs(e->x()-pressX) > 30){
//       // and let's zoom baby.. 
//       float w = (float)width()-lm-rm;
//       //    float w = (float)width()-lm-rm;
//       int low = (e->x() > pressX) ? pressX-lm : e->x()-lm;
//       int high = (e->x() > pressX) ? e->x()-lm : pressX-lm;
//       low = low < 0 ? 0 : low;
//       high = high > (width()-lm-rm) ? (width()-lm-rm) : high;
//       /// kludgy feeling there, but what can you do..
//       if(selectionId < selectBegin.size()){
// 	// let's calculate some new values... again, we'll have to float it and see what happens..
// 	float range = (float)(selectEnd[selectionId] - selectBegin[selectionId]);
// 	selectEnd[selectionId] = selectBegin[selectionId] + ((float)high/w) * range;
// 	selectBegin[selectionId] = selectBegin[selectionId] + ((float)low/w) * range;
//       }
//     }
//   }
//   if(e->x() == pressX && e->y() == pressY){
//     // i.e. a click, without any movement, regardless of time.. 
//     // go through the transcript rectangles and see if the selection is set or not
//     for(int i=0; i < gboxes.size(); i++){
//       //cout << "going through the gboxes, and am wondering, .. " << endl;
//       //cout << "box left " << gboxes[i].box.x() << "  top : " << gboxes[i].box.y() << endl;
//       //QPoint p(e->x()-lm, e->y()-tm);
//       if(gboxes[i].box.contains(e->pos())){
// 	if(e->state() == Qt::RightButton){         // select the RNA.. 
// 	gboxes[i].rSelected = !gboxes[i].rSelected;
// 	change = true;
// 	}
// 	if(e->state() == Qt::MidButton || e->state() == Qt::LeftButton + Qt::ControlButton){
// 	  gboxes[i].pSelected = !gboxes[i].pSelected;
// 	  change = true;
// 	}
// 	if(e->state() == Qt::LeftButton){
// 	  currentlySelectedGene = gboxes[i].transcript->geneIndex;
// 	  emit displayInfo(gboxes[i].transcript->geneIndex);
// 	}
// 	//cout << "toggled man" << endl;
// 	//change = true;
//       }
//     }
//     for(int i=0; i < regionBoxes.size(); i++){
//       if(regionBoxes[i].box.contains(e->pos())){
// 	if(e->state() == Qt::LeftButton){
// 	  regionBoxes[i].selected = !regionBoxes[i].selected;
// 	  change =true;
// 	}
// 	if(e->state() == Qt::RightButton){
// 	  // set up a region for selecting genomic DNA, which is centered on the mouse click. 
// 	  // need some sort of set of struct for holding the relevant information... hmm. 
// 	  // first go through our current set of boxes.. and see if there are any, if so, remove..
// 	  bool done = false;
// 	  for(set<DNABox*>::iterator it=DNABoxes.begin(); it != DNABoxes.end(); it++){
// 	    if((*it)->box.contains(e->pos())){
// 	      DNABox* pointer = *it;
// 	      DNABoxes.erase(it);
// 	      delete pointer;
// 	      change = true;
// 	      done = true;  // i.e. don't draw a new one.. 
// 	    }
// 	  }
// 	  if(!done){
// 	    // work out a resonable range to select... -- let's take 30 pixels either side of the current position
// 	    int extra = 30;
// 	    int index = i/2;
// 	    int startPixel = e->x() > lm+extra ? e->x()-extra-lm : 0;
// 	    int stopPixel = e->x() < width()-rm-extra ? e->x()+extra-lm : width()-rm-lm;
// 	    float range = (float)(selectEnd[index] - selectBegin[index]); 
// 	    float w = (float)(width()-lm-rm);
// 	    int start = selectBegin[index] + ((float)startPixel/w) * range;
// 	    int stop = selectBegin[index] + ((float)stopPixel/w) * range;
// 	    DNABoxes.insert(new DNABox(index, start, stop));
// 	    change = true;
// 	  }
// 	}
//       }
//     }
//     //// and also check the 
//     ///  ISHBOXES.. if we have a match within an ishbox, we migth want to consider popping up a menu,, and then returning.. 
//     ///  --- i.e. a menu asking if we want to load images  -- just the thumbnails or the full size images.. 
//     ///  ..........................
//     for(int i=0; i < ishProbeBoxes.size(); i++){
//       if(ishProbeBoxes[i].box.contains(e->pos())){
// 	// if left button display information regarding the blast match..
// 	if(e->state() == Qt::LeftButton){
// 	  // display stuff..
// 	  cout << "display some information about an in situ probe blast match.. " << endl;
// 	  return;
// 	}
// 	if(e->state() == Qt::RightButton){
// 	  // call some function that pops up a menu, and sets some global variable or something.. like that
// 	  // need a global variable somehwere probably.. 
// 	  cout << "would really rather like to load the in situ images for : " << ishProbeBoxes[i].psIndex << endl;
// 	  currentlySelectedIshProbe = ishProbeBoxes[i].psIndex;
// 	  ishImageMenu->popup(mapToGlobal(e->pos()));   // ho yeahhh...
// 	  return;
// 	}
//       }
//     }
//     if(!change && e->state() == Qt::RightButton){
//       popMenu->popup(mapToGlobal(e->pos()));
//     }
//   }
//   if(change){
//     repaint();
//   }
}

void LocalRegionWidget::drawRect(QRect r, QPainter* p){
  p->drawRect(QRect(r.x(), r.y(), r.width()+1, r.height()));
}

void LocalRegionWidget::getSequences(){
    makeSequenceRequest(true);
}

void LocalRegionWidget::displaySequences(){
    makeSequenceRequest(false);
}

void LocalRegionWidget::makeSequenceRequest(bool saveFile){
  // construct a sequence request and emit it, or something like that..
  // but now let's just print stuff to the console..
  //cout << "getSequences called, hold on there, I haven't constructed this one yet " << endl;
    QString outfile;
    if(saveFile){
	outfile = QFileDialog::getSaveFileName();
	if(outfile.isNull()){
	    cerr << "didn't get not file name,, " << endl;
	    return;
	}
    }else{
	outfile = "null";
    }
    
    set<int> probeSets;  // leave empty for now as I still don't have a way of selecting probe sets individually
    // by way of popular request, put the current probe set id into this.. 
    if(currentProbeSet){     // i.e. it's not 0
	probeSets.insert(currentProbeSet);    // not ideal as the user doesn't have a choice,, 
    }
    set<int> transcriptRequests;
    set<int> peptideRequests;
    set<genomicRequest> genRequests;
    for(vector<chromRegion>::iterator it=regions->begin(); it != regions->end(); it++){
	vector<ensemblGene>::iterator eit;
	vector<ensemblTranscript>::iterator tit;
	for(eit = (*it).fEnsGenes.begin(); eit != (*it).fEnsGenes.end(); eit++){
	    for(tit = (*eit).transcripts.begin(); tit != (*eit).transcripts.end(); tit++){
		if((*tit).rSelected){
		    transcriptRequests.insert((*tit).index);
		}
		if((*tit).pSelected){
		    peptideRequests.insert((*tit).index);
		}
	    }
	}
	for(eit = (*it).rEnsGenes.begin(); eit != (*it).rEnsGenes.end(); eit++){
	    for(tit = (*eit).transcripts.begin(); tit != (*eit).transcripts.end(); tit++){
		if((*tit).rSelected){
		    transcriptRequests.insert((*tit).index);
		}
		if((*tit).pSelected){
		    peptideRequests.insert((*tit).index);
		}
	    }
	}
	// and let's get the genomic regions..
    }
    for(vector<RegionDrawing*>::iterator it = drawings.begin(); it != drawings.end(); it++){
	set<genomicRequest> requests = (*it)->genomeRequests();
	genRequests.insert(requests.begin(), requests.end());
    }
    
//   // go through the gBoxes, and see what's selected or not..
//   for(int i=0; i < gboxes.size(); i++){
//     if(gboxes[i].rSelected){ transcriptRequests.insert(gboxes[i].transcript->index); }
//     if(gboxes[i].pSelected){ peptideRequests.insert(gboxes[i].transcript->index); }
//   }
//   // and then check out the DNABoxes..
//   for(set<DNABox*>::iterator it = DNABoxes.begin(); it != DNABoxes.end(); it++){
//     genRequests.insert(genomicRequest((*regions)[(*it)->regionIndex].chromosome, (*it)->start, (*it)->stop));
//   }
    SequenceRequest* seqRequest = new SequenceRequest(probeSets, transcriptRequests, peptideRequests, genRequests, outfile.latin1(), !saveFile);
    emit collectSequences(seqRequest);
}

void LocalRegionWidget::loadProbeSets(){
  set<int> selected;
  for(int i=0; i < drawings.size(); i++){
    set<int> sel = drawings[i]->selectedProbeSets();
    selected.insert(sel.begin(), sel.end());
  }
  vector<int> probes(selected.begin(), selected.end());
  emit loadProbeSetsPlease(probes, "Genomic Region");
  // should send a signal to load some probesets, but for now, just leave it out alright..
 //  cout << "loadProbeSets called, but to be honest, I haven't constructed this one yet eitherr" << endl;
//   set<int> index;
//   vector<int> vindex;
//   for(int i=0; i < regions->size(); i++){
//     if(regionBoxes[i*2].selected || regionBoxes[i*2+1].selected){
//       // then we go through the probe sets..
//       for(vector<probeSetMatch>::iterator it=(*regions)[i].pMatches.begin(); it != (*regions)[i].pMatches.end(); it++){
// 	if( (((*it).strand > 0) && regionBoxes[2*i].selected) || (((*it).strand < 0) && regionBoxes[2*i+1].selected) ){
// 	  // and if it overlaps OK..
// 	  if((*it).cStart < selectEnd[i] && (*it).cEnd > selectBegin[i]){
// 	    float logExp = -500;
// 	    if((*it).expect > 0){ logExp = log10((*it).expect); }
// 	    if((int)logExp <= maxExpect && minLength <= (*it).alLength && (float)(*it).match/(float)(*it).alLength > minMatch){
// 	      if(!index.count((*it).afIndex)){
// 		vindex.push_back((*it).afIndex);
// 		index.insert((*it).afIndex);
// 	      }
// 	    }
// 	  }
// 	}
//       }
//     }
//   }
//
}

void LocalRegionWidget::getThumbnails(){
  if(currentlySelectedIshProbe){
    // emit some signal..
    emit pleaseLoadThumbnails(currentlySelectedIshProbe);
    //cout << "I should be loading thumbnails for " << currentlySelectedIshProbe << endl;
    //
    currentlySelectedIshProbe = 0;
  }
}

void LocalRegionWidget::getImages(){
  if(currentlySelectedIshProbe){
    // emit some signal... 
    emit pleaseLoadImages(currentlySelectedIshProbe);
    //cout << "I should be loading images for " << currentlySelectedIshProbe << endl;
    //
    currentlySelectedIshProbe = 0;
  }
}
    
void LocalRegionWidget::nameProbe(){
  // do something to set the name for the current probe. -- note that this can only be done by privileged users.
}

void LocalRegionWidget::associateWithGene(){
  // do something to association the current in situ probe with some wonderful ensembl gene... just use manual annotation since the numbers are not
  // so large..
  //cout << "emitting associateIshProbeWithGene " << currentlySelectedGene << endl;
  emit associateIshProbeWithGene(currentlySelectedGene);
}

void LocalRegionWidget::addImages(){
  /// emit some signal that get's caught by the thingy..
  if(currentlySelectedIshProbe){
    emit addImagesFor(currentlySelectedIshProbe);
  }
}

void LocalRegionWidget::copyFigure(){
  QClipboard *cb = QApplication::clipboard();
  cb->setPixmap(*pixmap, QClipboard::Clipboard);
}

// void LocalRegionWidget::resizeEvent(QResizeEvent* e){
//   cout << "receiving a resize event, " << endl;
// }

