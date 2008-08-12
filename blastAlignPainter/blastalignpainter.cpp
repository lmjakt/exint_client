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

#include "blastalignpainter.h"
#include "../probe_set/probe_set.h"
#include <qobject.h>
#include <qpainter.h>
#include <qfont.h>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

BlastAlignPainter::BlastAlignPainter(){
  // in fact I don't need to do anything. I might 
  // want to include ownership, so that I get automatically deleted, but I don't really 
  // need it for now..
}

void BlastAlignPainter::paintAlignments(vector<blastAlignment>* balls, QPainter* p, int xo, int yo, int w, int h, int affy_length, int match_length){
  /// draw the matchlength as a line at the top, and the affymetrix probe parts underneath, int the gives space.. 
  /// assume that the background is black!! then we can have lovely purple on black. 
  int numSpace = 40;     // space either side for numbers 
  int thickLineWidth = 8;    // very thick..
  int thickArrowHeadWidth = 10; 
  int thickArrowHeadLength = 25;
  int yMargin = 30;
  int yStep = 30;       // the distance between size.. 
  w = w - 2*xo;
  int ew = w - (2*numSpace);     // effective width. good for calculating things.. 

  QPointArray aHead;
  aHead.putPoints(0, 4, 0, 0,  -thickArrowHeadLength/3, -thickArrowHeadWidth, thickArrowHeadLength, 0, -thickArrowHeadLength/3, +thickArrowHeadWidth); 

  // draw the arrow for the match,,
  // first translate to the appropriate position.. 
  QColor purple(180, 0, 180);
  p->translate((double)xo, (double)yo+yMargin);         // why double?????
  drawArrow(numSpace, w-numSpace, 0, 8, p, purple);
  p->setFont(QFont("Arial", 8));
  p->setPen(QColor(255, 255, 255));
  QString text;
  text.setNum(0);
  p->drawText(0, 4, text);
  text.setNum(match_length);
  p->drawText(w-numSpace+10, 4, text);

  vector<blastAlignment>::iterator it;
  /// and now lets draw the alignments.. ho yeahh...
  for(it = balls->begin(); it != balls->end(); it++){
    //for(int i=0; i < balls->size(); i++){
    p->translate(0, (double)yStep);
    // use the mStart and mEnd to work out where to put it.. simple calculation..
    //float x1 = (float)(*it).mStart/(float)match_length;
    //float x2 = (float)(*it).mEnd/(float)match_length;
    int xs = (int)((float)ew * ((float)(*it).mStart)/((float)match_length));
    int xe = (int)((float)ew * ((float)(*it).mEnd)/((float)match_length));
    xs += numSpace;
    xe += numSpace;
    
    float logExp = 0; //log10((*it).expectation);
    int r;
    int g;
    if((*it).expectation > 0){
      logExp = log10((*it).expectation);
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
    cout << "match length " << match_length << endl;
    cout << "start : " << (*it).mStart << "\txs: " << xs << endl;
    cout << "end   : " << (*it).mEnd  << "\txe: " << xe << endl;
    cout << "and the exponent of " << (*it).expectation << "\tis: " << logExp << endl;
    cout << "r: " << r << "\tg: " << g << endl;
    //cout << "float 1: " << x1 << "\tfloat 2: " << x2 << endl;
    ////// this isn't perfect, -because we are ignoring any effect that gaps will have on the overall length of the alignment.
    ////// however, this isn't likely to be very large, and it is tricky for use to insert those gaps in the line representing th
    /////  whole sequence, so I think that it may be fair to leave it like this.. 
    text.setNum((*it).affyStart);
    p->setPen(QColor(255, 255, 255));
    ostringstream matchString;
    matchString << (*it).matches << "/" << (*it).alignmentLength << "   (" << (*it).affyN << "  N's)";
    if(xs < xe){
      p->drawText(xs-numSpace, 4, text);
      text.setNum((*it).affyEnd);
      p->drawText(xe+10, 4, text);
      text = matchString.str().c_str();
      p->drawText(numSpace, 4, text);
    }else{
      p->drawText(xs+10, 4, text);
      text.setNum((*it).affyEnd);
      p->drawText(xe - numSpace, 4, text);
      text = matchString.str().c_str();
      p->drawText(numSpace, 4, text);
    }
    drawArrow(xs, xe, 0, 4, p, matchColor);
  }

  p->translate((double)-xo, 0);
}

void BlastAlignPainter::drawArrow(int xs, int xe, int y, int w, QPainter* p, QColor c){
  int aw = (int)((float)w*1.25);
  int al = (int)((float)w*3);
  
  int tx;

  QPointArray head;
  if(xe > xs){
    head.putPoints(0, 4, 0, 0, -al/3, -aw, al, 0, -al/3, aw);  // forward arrow
  }else{
    head.putPoints(0, 4, 0, 0, al/3, -aw, -al, 0, al/3, aw);
  }
  p->setPen(QPen(c, w));
  if(xe > xs){     // again if forward arrow
    p->drawLine(xs, y, xe-al, y);
    p->translate((double)xe-al, (double)y);
    tx = xe-al;
  }else{
    p->drawLine(xs, y, xe+al, y);
    p->translate((double)xe+al, (double)y);
    tx = xe+al;
  }
  // draw Head.
  p->setPen(QPen(c, 0));
  p->setBrush(c);
  p->drawPolygon(head);
  p->translate(-tx, 0);    // put back to where we came from, and return..
}
 

