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

#include "GenAlignWidget.h"
#include <qwidget.h>
#include <qpainter.h>
#include <vector>
#include <map>
#include "../probe_set/probe_set.h"
#include "../sequence/sequenceRequest.h"
#include <string>
#include <sstream>
#include <qstring.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpointarray.h>
#include <qrect.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
//#include <qkeysequence.h>
#include <math.h>
#include <iostream>

using namespace std;

alignBox::alignBox(QRect r, const blastAlignment* al, const blastGenomeMatch* bm){
  box = r;
  alignment = al;
  match = bm;
}

alignBox::alignBox(){
  alignment = 0;
  match = 0;
}

sequenceSelect::sequenceSelect(){
  above = 1000;
  below = 1000;
  alignment = 0;
  match = 0;
  box = 0;
  aboveDelta = 0;
  belowDelta = 0;
}

void sequenceSelect::clear(){
  above = 1000;
  below = 1000;
  alignment = 0;
  match = 0;
  box = 0;
  aboveDelta = 0;
  belowDelta = 0;
}

void sequenceSelect::setAlignment(const blastAlignment* al, const blastGenomeMatch* m, const alignBox* b){
  above = 1000;
  below = 1000;
  alignment = al;
  match = m;
  box = b;
  aboveDelta = 0;
  belowDelta = 0;
}

geneSelectBox::geneSelectBox(){
  geneIndex = 0;
}

geneSelectBox::geneSelectBox(QRect r, int index){
  geneIndex = index;
  box = r;
}

GenAlignWidget::GenAlignWidget(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  // and lets initialiase
  hasData = false;
  setBackgroundMode(NoBackground);      // paint with QPixmap for less flicker !!
  rowHeight = 15;
  topSpace = 30;
  matchSpace = 30;
  labelHeight = 12;
  desiredHeight = topSpace; // so there's something.

  saveMenu = new QPopupMenu(this);
  //  saveMenu->insertItem("Save Sequences", 1, 1);
  saveMenu->insertItem("Save Sequences", this, SLOT(getSequences()), CTRL+Key_S, 1);
}

GenAlignWidget::~GenAlignWidget(){
  // doo nothing, be happy..
}

void GenAlignWidget::setData(probe_data* d){
  data = d;
  hasData = true;
  /// maybe calculate some variables before we call update() or repaint();
  // because anyway, we have to iterate through, so lets see how it goes.. 
  multimap<double, blastGenomeMatch>::iterator it;
  int alignmentNo = 0;   // the number of alignments, and hence labels needed. 
  for(it = data->genomeMatches.begin(); it != data->genomeMatches.end(); it++){
    alignmentNo += (*it).second.alignments.size();          // approximation, will overestimate amount of space needed..
  }
  // space required should be, - top space + (number of matches * matchSpace) + (number of matches * 3 * rowHeight) + 
  //                                         (number of alingments * labelHeight);
  int sz = data->genomeMatches.size();
  desiredHeight = topSpace + (sz * (matchSpace + 30)) + (3 * sz * rowHeight) + (alignmentNo * labelHeight);
  //cout << "Number of matches    : " << sz << endl
  //     << "number of alignments : " << alignmentNo << endl
  //     << "total height wanted  : " << desiredHeight << endl;
  // and that should be Ok.. 
  //update();    // require manual thing, so resizing is OK. 
  selectedSequence.clear();
  // and clear the selectedGenes set
  while(selectedGenes.size()){
    selectedGenes.erase(selectedGenes.begin());
  }
  while(selectedPeptides.size()){
    selectedPeptides.erase(selectedPeptides.begin());
  }
  //  saveMenu->setItemEnabled(1, false);  -- well we can always just get the probe set sequence. !! .
}

void GenAlignWidget::drawArrow(int xs, int xe, int y, int w, QPainter* p, QColor c, bool forward){
  int aw = (int)((float)w*1.25);
  int al = (int)((float)w*3);

  int tx;
  //cout << "Drawing arrow xs : " << xs << "\txe : " << xe << "\tstrand : " << strand << endl;
  if(xe < xs){
    int ts = xs;
    xs = xe;
    xe = ts;
  }
  //  cout << "\tDrawing arrow xs : " << xs << "\txe : " << xe << "\tforward : " << forward << endl;
  
  QPointArray head;
  //  if(xe > xs){
  if(forward){
    head.putPoints(0, 4, 0, 0, -al/3, -aw, al, 0, -al/3, aw);  // forward arrow
  }else{
    head.putPoints(0, 4, 0, 0, al/3, -aw, -al, 0, al/3, aw);
  }
  
  p->setPen(QPen(c, w));  
  if(forward){     // again if forward arrow
    p->drawLine(xs, y, xe-al, y);
    p->translate((double)xe-al, (double)y);
    tx = xe-al;
  }else{
    p->drawLine(xs+al, y, xe, y);
    p->translate((double)xs+al, (double)y);
    tx = xs+al;
  }
  // draw Head.
  p->setPen(QPen(c, 0));
  p->setBrush(c);
  p->drawPolygon(head);
  p->translate(-tx, 0);    // put back to where we came from, and return..
}

void GenAlignWidget::drawRange(int xs, int xe, int y, int w, QPainter* p, QColor c){
  p->setPen(QPen(c, 1));
  p->drawLine(xs, y-w, xs, y+w);
  p->drawLine(xe, y-w, xe, y+w);
  p->drawLine(xs, y, xe, y);
}

void GenAlignWidget::drawCheckBox(int x, int y, int w, bool checked, int index, QPainter* p, bool isPeptide){
  QPen oldPen = p->pen();
  QBrush oldBrush = p->brush();
  QColor penColor;
  if(isPeptide){
    penColor = QColor(220, 22, 220);
  }else{
    penColor = QColor(198, 247, 22);
  }
  p->setBrush(NoBrush);
  p->setPen(QPen(penColor, 1));
  QRect rect(x, y, w, w);
  p->drawRect(rect);
  
  if(isPeptide){
    peptideBoxes.push_back(geneSelectBox(p->xForm(rect), index));  // hhoooo yeahh. 
  }else{
    geneBoxes.push_back(geneSelectBox(p->xForm(rect), index));  // hhoooo yeahh. 
  }
  // stuff to do if checked. 
//   p->drawArc(x-w/2, y+w/2, w, w, 0, 90*16);
//   p->drawArc(x+w/2, y, w, w*2, 180*16, -85*16);
//   p->setPen(QPen(penColor, 2));
//   p->drawArc(x+w/2, y, w, w*2, 180*16, -45*16);
  if(checked){
    p->setBrush(penColor);
    p->drawPie(x+2, y+2, w-4, w-4, 0, 360*16);
  }

  p->setPen(oldPen);
  p->setBrush(oldBrush);
}

void GenAlignWidget::paintEvent(QPaintEvent* e){
  QPixmap pix(width(), height());
  //double rowHeight = 15;
  int indent = 20;
  int textMargin = 0;
  pix.fill(QColor(0, 0, 0));
  
  int seqMargin = 10;      // extra bits to put either side of the two genes, just to make it look nicer.. 
  int xm = 50;             // xmargin. keep it symmetrical.. so it's ok.. 
  //double topSpace = 30;    // top space for some title or something leave it blank for now.. 
  //double matchSpace = 30;  // some space between matches.. 
  int seqW = width() - (2*xm + 2*seqMargin);   // the actual width for the gene positions. !! 
  int seqO = xm+seqMargin;   // just for calculations.. 
  

  QPen dnaPen(QColor(255, 255, 255), 1);      // a thin white line
  QColor geneColor(180, 0, 180);              // a nice purple for the genes, ue draw arrow function !!. 
  int geneWidth = 5;                          // the width of the genes.. (thickness more like). 
  int alignWidth = 3;
  
  if(hasData){
    QPainter* p = new QPainter(&pix);      // don't forget to delete the painter.. !!
    p->setFont(QFont("Arial", 9));
    multimap<double, blastGenomeMatch>::iterator it =data->genomeMatches.begin();
    QString Af_label = data->afid.c_str();
    if(it != data->genomeMatches.end()){
      QString tempNum;
      tempNum.setNum((*it).second.af_length);
      Af_label.append("  (");
      Af_label.append(tempNum);
      Af_label.append(" bp)");
    }
    alignmentBoxes.resize(0);      // should be ok.
    geneBoxes.resize(0);
    peptideBoxes.resize(0);
    // write some header stuff,
    p->setPen(dnaPen);
    p->drawText(0, 6, width(), rowHeight, AlignCenter, Af_label);
    //    p->drawText(0, 6, width(), rowHeight, AlignCenter, data->afid.c_str());
    p->translate(0, topSpace);
    for(it = data->genomeMatches.begin(); it != data->genomeMatches.end(); it++){
      // work out the length of the section we are dealing with. then work out the numbers
      // to use for the scales.. 
      blastGenomeMatch* gm = &(*it).second;          // for convenience.. 
      int smallest, highest;    
      if(gm->higherEnd > gm->midEnd && gm->higherEnd > gm->lowerEnd){
	highest = gm->higherEnd;
      }else{
	if(gm->midEnd > gm->lowerEnd){
	  highest = gm->midEnd;
	}else{
	  highest = gm->lowerEnd;
	}
      }
      smallest = gm->lowerStart;      // always true anyway.. 
      int seqRange = highest-smallest;
      int ratio = seqRange/seqW;           // the bp/available space.. just divide by this number to get the thingy.. 
      QString smallestPosition;
      QString highestPosition;   // use sprintf to format..
      //      smallestPosition.sprintf("%1.3e", smallest);
      //highestPosition.sprintf("%1.3e", highest);
      smallestPosition.setNum(smallest);
      highestPosition.setNum(highest);
      //      cout << "smallest is " << smallest << "  formatted " << smallestPosition << endl;
      //cout << "highest is  " << highest  << "  formatted " << highestPosition << endl;
      /// very ugly, but there you go,, beautification next week.. 
      // first draw a line using the thing
      p->setPen(dnaPen);
      p->drawLine(xm, 0, width()-xm, 0);
      p->drawText(xm-20, -8, 20, 16, AlignLeft|AlignVCenter, gm->chromosome.c_str());
      p->setFont(QFont("Courier", 8));
      p->drawText(xm, -17, 100, 12, AlignLeft|AlignTop, smallestPosition);
      p->drawText(width()-xm-100, -17, 100, 12, AlignRight|AlignTop, highestPosition);
      
      // and then lets draw the arrows,, from gm->lowerStart, gmLower end, and so on.. 
      // calculating on the fly..
  //      cout << "smallest : " << smallest << "\tlower start: " << gm->lowerStart
//  	   << "\tlower end: " << gm->lowerEnd << "\tmid start : " << gm->midStart << "\tmid end : " << gm->midEnd
//  	   << "\thigher start : " << gm->higherStart << "\thigher end : " << gm->higherEnd << endl;
      p->translate((double)seqO, 0);   // just take it back to 0 after drawing the arrows.. and alignments..
      bool forward;
      // its always true for these because we use the strand.. and then swap them around.. 
      if(gm->lowerStrand > 0) { forward = true; }else{ forward = false; }
      drawArrow((gm->lowerStart - smallest)/ratio, (gm->lowerEnd - smallest)/ratio, 0, geneWidth, p, geneColor, forward);
      if(gm->midStrand > 0) { forward = true; }else{ forward = false; }
      drawArrow((gm->midStart - smallest)/ratio, (gm->midEnd - smallest)/ratio, 0, geneWidth, p, geneColor, forward);
      if(gm->higherStrand > 0) { forward = true; }else{ forward = false; }
      drawArrow((gm->higherStart - smallest)/ratio, (gm->higherEnd - smallest)/ratio, 0, geneWidth, p, geneColor, forward);
      
      // and then we should draw the alignments..
      //double labelHeight = 12;           // rowHeight = 15 .. 
      double extraSpace = labelHeight;      // if we need extra space for labels.. 
      vector<QRect> rects(0);        // vector of rectangles. I don't think I need to worry about deleting.. 
      for(vector<blastAlignment>::iterator bit = gm->alignments.begin(); bit != gm->alignments.end(); bit++){
	//ostringstream labelStream;  // think that's what we call it.. 
	int labelSpace = 55;  // but in fact 60, as we take +/- this length.. !
	float logExp = 0; //log10((*it).expectation);
	int r;
	int g;
	if((*bit).expectation > 0){
	  logExp = log10((*bit).expectation);
	  r = -((int)logExp);
	  r += 100;      // add on a bit eh.. 
	  if(r > 255) { r = 255; }
	  g = 255 + (int)logExp;  // it should always be negative.. 
	  if(g < 0) { g = 0; }
	}else{
	  r = 255;
	  g = 0;
	}
	
	QColor matchColor(r, g, 0);
	//QColor temp(0, 200, 0);  // green..
	if((*bit).mStart < (*bit).mEnd) { forward = true; }else{ forward = false; }
	drawArrow( ((*bit).mStart - smallest)/ratio, ((*bit).mEnd-smallest)/ratio, 0, alignWidth, p, matchColor, forward);

	// if the arrow is selected let's draw a range in using the thingy... 
	//cout << "Address of current alignment is        : " << &(*bit) << endl
	//     << "Address of selected Sequence Alignment : " << selectedSequence.alignment << endl;
	QString selectSize = "0";  // just in case we get confused, .. 
	if(selectedSequence.alignment == &(*bit)){
	  int s, e;
	  // first adjust the above and below using the aboveDelta and belowDelta..
	  selectedSequence.above += (selectedSequence.aboveDelta * ratio);
	  selectedSequence.below += (selectedSequence.belowDelta * ratio);
	  // just make sure both are above 0,,
	  if(selectedSequence.above < 0){ selectedSequence.above = 0;}
	  if(selectedSequence.below < 0){ selectedSequence.below = 0;}
	  // reset the deltas..
	  selectedSequence.aboveDelta = 0;
	  selectedSequence.belowDelta = 0;
	  if(forward){
	    s = (*bit).mStart - selectedSequence.below;
	    e = (*bit).mEnd + selectedSequence.above;
	  }else{
	    s = (*bit).mEnd - selectedSequence.below;
	    e = (*bit).mStart + selectedSequence.above;
	  }
	  drawRange( (s-smallest)/ratio, (e-smallest)/ratio, 0, 10, p, matchColor);
	  selectSize.setNum(e-s);
	  //	  QRect localRect = p->xFormDev(selectedSequence.box->box);
	  //localRect.moveBy(localRect.width() + 5, 0);
	  //p->drawText(localRect, AlignCenter, selectSize);
	  //	  p->drawText(localRect.right(), localRect.bottom(), selectSize);
	}

	//// and lets think about a label.. 
	QString label;
	label.sprintf("%3d-%-3d %3d/%-3d", (*bit).affyStart, (*bit).affyEnd, (*bit).matches, (*bit).alignmentLength);
	//labelStream << (*bit).affyStart << "-" << (*bit).affyEnd << "  " <<  (*bit).matches << "/" << (*bit).alignmentLength;
	//QRect rect( (((*bit).mStart - smallest)/ratio), labelHeight, labelSpace*2, rowHeight);
	QRect rect( (((*bit).mStart - smallest)/ratio)-labelSpace, labelHeight, labelSpace*2, rowHeight);
	//for(int i=0; i < alignmentBoxes.size(); i++){
	for(int i=0; i < rects.size(); i++){
	  //if(rect.intersects(alignmentBoxes[i].box)){
	  if(rect.intersects(rects[i])){
	    rect.moveBy(0, (int)labelHeight);
	    //extraSpace += labelHeight;
	  }
	}
	if(rect.y() > extraSpace) { extraSpace = rect.y(); }
	p->setFont(QFont("Courier", 8));
	//p->drawText(rect, AlignLeft, label);
	//	p->drawText(rect, AlignLeft, labelStream.str().c_str());
	p->drawText(rect, AlignCenter, label);
	if(selectedSequence.alignment == &(*bit)){        // then we should also draw the selected size length..
	  QRect tempRect = rect;
	  tempRect.moveBy(rect.width()+5, 0);
	  p->drawText(tempRect, AlignVCenter|AlignLeft, selectSize);
	}
	rects.push_back(rect);
	alignmentBoxes.push_back(alignBox(p->xForm(rect), &(*bit), &(*it).second));
				 //	rects.push_back(rect);
      }
      p->setFont(QFont("Arial", 8));
      //      cout << "and extra space is " << extraSpace << endl; 
      QString description;
      string field("Gene description");
      // multimap<string, string>::iterator sit = gm->lowerD.find(field);
//       p->setPen(QPen(QColor(255, 255, 255), 0));
      
//       int boxWidth = rowHeight - 4;
//       p->translate(0, 30 + extraSpace);     // for some text..
//       QString num("1.");
//       description = (*sit).second.c_str();
//       p->drawText(textMargin, 0, num);
//       p->drawText(textMargin + indent, 0, description);
//       // cout << "selected Genes count for lower Index is " << selectedGenes.count(gm->lowerIndex) << endl;
//       drawCheckBox(textMargin-rowHeight-3, -boxWidth, boxWidth, selectedGenes.count(gm->lowerIndex), gm->lowerIndex, p);
//       drawCheckBox(textMargin-rowHeight-5-boxWidth, -boxWidth, boxWidth, selectedPeptides.count(gm->lowerIndex), gm->lowerIndex, p, true);
//       drawCheckBox(textMargin-rowHeight-5-boxWidth, -boxWidth, boxWidth, selectedPeptides.count(gm->lowerIndex), gm->lowerIndex, p, true);
//       p->translate(0, rowHeight);     // for some text..
      
//       sit = gm->midD.find(field);
//       num = "2.";
//       description = (*sit).second.c_str();
//       p->drawText(textMargin, 0, num);
//       p->drawText(textMargin + indent, 0, description);
//       drawCheckBox(textMargin-rowHeight-3, -boxWidth, boxWidth, selectedGenes.count(gm->midIndex), gm->midIndex, p);
//       drawCheckBox(textMargin-rowHeight-5-boxWidth, -boxWidth, boxWidth, selectedPeptides.count(gm->midIndex), gm->midIndex, p, true);
//       p->translate(0, rowHeight);     // for some text..
      
//       sit = gm->higherD.find(field);
//       num = "3.";
//       description = (*sit).second.c_str();
//       p->drawText(textMargin, 0, num);
//       p->drawText(textMargin + indent, 0, description);
//       drawCheckBox(textMargin-rowHeight-3, -boxWidth, boxWidth, selectedGenes.count(gm->higherIndex), gm->higherIndex, p);
//       drawCheckBox(textMargin-rowHeight-5-boxWidth, -boxWidth, boxWidth, selectedPeptides.count(gm->higherIndex), gm->higherIndex, p, true);
//       p->translate(0, rowHeight);
      
//       p->translate(-(double)seqO, matchSpace);
    }
    delete p;
  }
  bitBlt(this, 0, 0, &pix, 0, 0);
  // and that's that for now !!! alleluliah.. 
}

void GenAlignWidget::mousePressEvent(QMouseEvent* e){
  lastX = e->x();
  lastY = e->y();
  //  emit mousePress(e);
  // check if the position overlaps with any of the alignent boxes. I don't know any good way of doing this,
  // so lets just go through the list..
  //cout << "mouse press x: " << lastX << "\ty: " << lastY << endl;
  if(e->button() == 1){
    for(int i=0; i < alignmentBoxes.size(); i++){
      //cout << "alignmentBox x " << alignmentBoxes[i].box.x() << "\ty : " << alignmentBoxes[i].box.y() << endl;
      if(alignmentBoxes[i].box.contains(e->pos())){
	// we found a match.. 
	if(alignmentBoxes[i].alignment != selectedSequence.alignment){
	  selectedSequence.setAlignment(alignmentBoxes[i].alignment, alignmentBoxes[i].match, &alignmentBoxes[i]);
	  repaint();
	}else{
	  selectedSequence.clear();
	  repaint();
	}
	break;
      }
    }
    for(int i=0; i < geneBoxes.size(); i++){
      // cout << "gene Box # " << i << endl;
      if(geneBoxes[i].box.contains(e->pos())){
	// we found a match..
	if(selectedGenes.count(geneBoxes[i].geneIndex)){
	  selectedGenes.erase(geneBoxes[i].geneIndex);
	}else{
	  selectedGenes.insert(geneBoxes[i].geneIndex);
	}
	repaint();
	break;
      }
    }
    for(int i=0; i < peptideBoxes.size(); i++){
      if(peptideBoxes[i].box.contains(e->pos())){
	if(selectedPeptides.count(peptideBoxes[i].geneIndex)){
	  selectedPeptides.erase(peptideBoxes[i].geneIndex);
	}else{
	  selectedPeptides.insert(peptideBoxes[i].geneIndex);
	}
	repaint();
	break;
      }
    }
  }
  if(e->state() == Qt::ControlButton || e->button() == 4){
    //  if(e->button() == 4 || e->button() == 513){
    saveMenu->popup(mapToGlobal(e->pos()));
  }
  //  saveMenu->setItemEnabled(1, (selectedSequence.alignment || selectedGenes.size()));
  //  cout << "selected Genes size is " << selectedGenes.size() << endl;

}

void GenAlignWidget::mouseMoveEvent(QMouseEvent* e){
  //  emit moveMe(lastX - e->x(), lastY - e->y());
  //  emit mouseMove(e);
  if(selectedSequence.alignment){   // i.e. one is defined with a non 0 value for the memory address
    //cout << "moving mouse, button state is " << e->button() << endl;
    if(e->state() == 1){   // left button
      selectedSequence.belowDelta += (lastX - e->x());
    }
    if(e->state() == 2){   // right button
      selectedSequence.aboveDelta += (e->x() - lastX);
    }
  }
  lastX = e->x();
  lastY = e->y();
  repaint();
}

void GenAlignWidget::getSequences(){
  // first of all lets get a fileName. We shouldn't be able to get here if it 
  // wasn't for the fact that some sequence has already been selected. Still,
  // you know, it could happen.

  // OK how do we get a fileName??
  // very simple
  QString outfile = QFileDialog::getSaveFileName();
  cout << "Got a file name and it is " << outfile << endl;
  
  if(outfile.isNull()){
    return;
  }
  // then make a Sequence Request.. !!!
  set<int> probeSets;
  probeSets.insert(data->index);
  cout << "inserted the name of the probe set " << data->afid << endl;
  // we can take the transcripts straight from the selectedGenes set !! convenient..
  set<genomicRequest> genRequest;
  cout << "selectedSequence alignment is " << selectedSequence.alignment << endl;
  if(selectedSequence.alignment){
    genRequest.insert(genomicRequest(selectedSequence.match->chromosome, selectedSequence.alignment->mStart - selectedSequence.below, selectedSequence.alignment->mEnd + selectedSequence.above));
    // whoarrrh, what a mouthfule.. It actually has a problem, It does rather assume that mStart is always smaller then mEnd.. which may not be true.
  }
  //  set<genomicRequest>::iterator git = genRequest.begin();
  //cout << "Made a genome request : chromosome : " << (*git).chromosome << endl
  //    << "                        start      : " << (*git).start << endl
  //    << "                        end        : " << (*git).end << endl;
  SequenceRequest* seqRequest = new SequenceRequest(probeSets, selectedGenes, selectedPeptides, genRequest, outfile.latin1());  // hhooaaa
  ///// note this is a memory leak if this isn't taken care of by the receiver of the request.. so please remember this.. a byte is just a byte..
  emit collectSequences(seqRequest);
}
