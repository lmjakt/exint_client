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

#include "regionWidget.h"
#include "../client.h"
#include <qwidget.h>
#include <qrect.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qfont.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qlayout.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <iostream>

using namespace std;

psBox::psBox(){
  psIndex = 0;
  box = QRect(0, 0, 0, 0);
}

psBox::psBox(QRect b, int i){
  box = b;
  psIndex = i;
}

RegionWidget::RegionWidget(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  hasData = false;
  region = 0;
  setBackgroundMode(NoBackground);


  lm = 15;
  rm = 15;
  tm = 30;
  bm = 30;
  
  selectingRegion = false;
  changingSubRegion = false;
  rightLimitChanging = false;
  leftLimitChanging = true;

  actionMenu = new QPopupMenu(this);
  actionMenu->insertItem("Load Probe sets", this, SLOT(loadProbeSets()), CTRL+Key_L, 1);
  actionMenu->insertItem("Ensembl Genes", this, SLOT(getEnsemblGenes()), CTRL+Key_G, 2);
  // what to set the height to I am not sure..
  // but let's resize for the hell of it..
//   chromLabel = new QLabel("Chromosome 0  0 bp  start: 0 end 0");
//   QLabel* startLabel("select start", this, "selectStart");
//   QLabel* endLabel("select end", this, "selectEnd");
//   selectStart = new QLineEdit("0", this);
//   selectEnd = new QLineEdit("0", this);

//   QVBoxLayout* vbox = new QVBoxLayout(this);
//   QHBoxLayout* hbox = new QHBoxLayout();
//   vbox->addWidget(
  resize(500, 100);  // or something.. 
}

void RegionWidget::setData(int me, int mall, int mm, chromRegion* cr){
  hasData = true;
  region = cr;
  maxExpect = me;
  minAllength = mall;
  minMatch = mm;
  selectBegin = region->chromStart;
  selectEnd = region->chromEnd;
  boxes.resize(cr->pMatches.size());       // or should I make this equal to the number of alignments ?? 
  //boxes.resize(cr->ensGenes.size());       // probably it is ok.. 
  // if I have any sets of selected genes, I would need to empty them here, but for now.. let's just do some 
  // drawing..
  emit selectRegionChanged(region->chromStart, region->chromEnd, selectBegin, selectEnd);
  update();     // well draw it !! ..
}

void RegionWidget::paintEvent(QPaintEvent* e){
  QPixmap pix(width(), height());
  pix.fill(QColor(0, 0, 0));

  QPen dnaPen(QColor(255, 255, 255), 2);   // for genome range.. 
  int psHeight = 9;   // how high a thingy.. 
  int geneWidth = 9;
  int w = width()-lm-rm;

  if(hasData){
    QPainter* p = new QPainter(&pix);
    p->setPen(dnaPen);
    p->drawLine(lm, tm, width()-rm, tm);   // just a white line anyway..
    // and now just go through the alignments.. and see how they work out.. 
    //float chromRange = (float)(region->chromEnd - region->chromStart);
    float chromRange = (float)(selectEnd - selectBegin);

    p->setPen(QPen(QColor(147, 45, 120), 1));
    p->setBrush(QColor(147, 45, 120));
    for(int i=0; i < region->ensGenes.size(); i++){
      //      cout << "hey I should be a drawing a gene here, but let's just see if this works" << endl;
      if(region->ensGenes[i].start > selectBegin && region->ensGenes[i].stop < selectEnd){
	int x1 = lm + w *(((float)(region->ensGenes[i].start - selectBegin))/chromRange);
	int x2 = lm + w *(((float)(region->ensGenes[i].stop - selectBegin))/chromRange);
	//	int y = region->ensGenes[i].sense ? -geneWidth : 0;
	int y = region->ensGenes[i].sense ? -geneWidth-dnaPen.width()/2 : dnaPen.width()/2;
	y += tm;
	if(x1 == x2){ x2++; }
	QRect box(x1, y, x2-x1, geneWidth);
	p->drawRect(box);
      }
    }
    

    for(int i=0; i < region->pMatches.size(); i++){
      if(region->pMatches[i].cStart > selectBegin && region->pMatches[i].cStart < selectEnd){
	//int y = (region->pMatches[i].cStart < region->pMatches[i].cEnd) ? -psHeight : 0;  // if in the same direction, on top..
	int y = (region->pMatches[i].cStart < region->pMatches[i].cEnd) ? -psHeight-dnaPen.width()/2 : dnaPen.width()/2;  // if in the same direction, on top..
	//int y = (region->pMatches[i].strand) ? -psHeight-dnaPen.width()/2 : dnaPen.width()/2;  // if in the same direction, on top..
	y += tm;    // 
	int x1 = lm + w *(((float)(region->pMatches[i].cStart - selectBegin))/chromRange);
	int x2 = lm + w *(((float)(region->pMatches[i].cEnd - selectBegin))/chromRange);
	//int x1 = lm + w *(((float)(region->pMatches[i].cStart - region->chromStart))/chromRange);
	//      int x2 = lm + w *(((float)(region->pMatches[i].cEnd - region->chromStart))/chromRange);
	if(x1 == x2){
	  x2++;
	}
	boxes[i].box.setRect(x1, y, x2-x1, psHeight);    // should be ok I think
	// find the appropriate colour to be using..
	int r, g;
	float logExp = 0;
	if(region->pMatches[i].expect > 0){
	  logExp = log10(region->pMatches[i].expect);
	  r = 100 - (int)logExp;
	  g = 255 + (int)logExp;
	  if(r > 255){ r = 255; }
	  if(g < 0) { g = 0; }
	}else{
	  r = 255;
	  g = 0;
	  logExp = -500;   // not reasonable at all, but ok hack for the next session.. 
	}
	QColor col(r, g, 0);
	p->setPen(QPen(col, 1));
	p->setBrush(col);
	if(logExp < maxExpect && region->pMatches[i].alLength > minAllength && region->pMatches[i].match > minMatch){
	  p->drawRect(boxes[i].box);
	}
      }
    }
    //// and here lets draw the ensembl genes..

    // let's make some things..
    int selHeight = 10;     // selection heigth. mm.
    int handleWidth = 4;
    int topPos = tm+ bm + 20;      // to give space for some numeric labels..  
    wholeRegion.setRect(lm, topPos+selHeight/2, w, 2);    // anyway, that is always the same.. 
    float wholeRange = (float)(region->chromEnd - region->chromStart);    // chromRange is above,, 
    int x = lm + w *(((float)(selectBegin - region->chromStart))/wholeRange);
    int xe = lm + w *(((float)(selectEnd - region->chromStart))/wholeRange);
    int sw = (int) w * (chromRange/wholeRange);
    if(sw > 2*handleWidth+3){
      subRegion.setRect(x, topPos+1, sw, selHeight);
    }else{
      subRegion.setRect(x, topPos+1, 2*handleWidth+3, selHeight);
    }
    leftHandle.setRect(x, topPos+1, handleWidth, selHeight);
    if(xe-handleWidth > x+handleWidth+3){
      rightHandle.setRect(xe-handleWidth, topPos+1, handleWidth, selHeight);
    }else{
      rightHandle.setRect(x+handleWidth+3, topPos+1, handleWidth, selHeight);
    }

    QColor col(255, 255, 255);
    p->setPen(QPen(col, 1));
    p->setBrush(col);
    p->drawRect(wholeRegion);
    col.setRgb(163, 27, 156);
    p->setPen(QPen(col, 1));
    p->setBrush(col);
    p->drawRect(subRegion);  
    col.setRgb(201, 22, 40);
    p->setPen(QPen(col, 1));
    p->setBrush(col);
    p->drawRect(leftHandle);  
    p->drawRect(rightHandle);  
    
    int labelWidth = 75;
    int labelHeight = 15;
    p->setPen(QPen(QColor(255, 255, 255), 1));
    QString label;
    //p->setFont(QFont("Arial", 8));
    label.setNum(region->chromStart);
    p->drawText(lm, topPos-labelHeight, labelWidth, labelHeight, Qt::AlignLeft, label);
    label.setNum(region->chromEnd);
    p->drawText(width()-rm-labelWidth, topPos-labelHeight, labelWidth, labelHeight, Qt::AlignRight, label);
    QRect l1(x, topPos+selHeight+2, labelWidth, labelHeight);
    QRect l2(xe-labelWidth, topPos+selHeight+2, labelWidth, labelHeight);
    if(l2.intersects(l1)){
      l2.moveBy(0, labelHeight);
    }
    label.setNum(selectBegin);
    p->drawText(l1, Qt::AlignLeft|Qt::AlignBottom, label);
    label.setNum(selectEnd);
    p->drawText(l2, Qt::AlignRight|Qt::AlignBottom, label);

    // and if we are selecting
    if(selectingRegion){
      p->setPen(QPen(QColor(255, 255, 255), 1));
      //      p->setPen(dnaPen);
      p->setBrush(NoBrush);
      p->drawRect(firstX, firstY, lastX-firstX, lastY-firstY);
    }
    delete p;
  }
  bitBlt(this, 0, 0, &pix, 0, 0);
}

void RegionWidget::loadProbeSets(){
  cout << "should be loading probe sets or something " << endl;
  // do this the simple way. load a set with ints, then make a vector from it..
  set<int> indexSet;
  vector<int> indices;
  for(int i=0; i < region->pMatches.size(); i++){
      if(region->pMatches[i].cStart > selectBegin && region->pMatches[i].cStart < selectEnd){
	float logExp = -500;
	if(region->pMatches[i].expect > 0){
	  logExp = log10(region->pMatches[i].expect);
	}
	if(logExp < maxExpect && region->pMatches[i].alLength > minAllength && region->pMatches[i].match > minMatch){
	  if(!indexSet.count(region->pMatches[i].afIndex)){
	    indices.push_back(region->pMatches[i].afIndex);
	    indexSet.insert(region->pMatches[i].afIndex);
	  }
	}
      }
  }    // not particularly clever, very inefficient, but easy to code..
  emit pleaseLoadGenes(indices, "Genomic Region");
}

// I wonder how that is going to look.. eeegh.. 
void RegionWidget::mousePressEvent(QMouseEvent* e){
  firstX = lastX = e->x();
  firstY = lastY = e->y();
  // but that's all for now
  if(e->button() == Qt::LeftButton){            // hmm but what the hell.
    if(subRegion.contains(e->pos())){
      changingSubRegion = true;
      if(leftHandle.contains(e->pos())){
	leftLimitChanging = true;
	//changingSubRegion = false;
      }
      if(rightHandle.contains(e->pos())){
	rightLimitChanging = true;
	//changingSubRegion = false;     // well that now is the ugliest I've seen in a long time.. !!! 
      }
    }else{
      if(e->y() < tm+bm)
	selectingRegion = true;
    }
  }
  if(e->button() == Qt::RightButton){
    actionMenu->popup(e->globalPos());
  }
}

void RegionWidget::mouseMoveEvent(QMouseEvent* e){
  // for tracking put at bottom of event..
  // work out how things are.. 
  if(changingSubRegion){
    int w = width() - lm - rm;
    int deltaX = e->x() - lastX;
    int rightLimit = selectEnd + (int) ((float)(region->chromEnd-region->chromStart)* (float)deltaX/(float)w);
    int leftLimit = selectBegin + (int) ((float)(region->chromEnd-region->chromStart)* (float)deltaX/(float)w);
    if(!leftLimitChanging && !rightLimitChanging && rightLimit <= region->chromEnd && leftLimit >= region->chromStart){
      selectEnd = rightLimit;
      selectBegin = leftLimit;
    }
    if(leftLimitChanging){
      if(leftLimit < selectEnd && leftLimit >= region->chromStart){
	selectBegin = leftLimit;
      }
    }
    if(rightLimitChanging){
      if(rightLimit > selectBegin && rightLimit <= region->chromEnd){
	selectEnd = rightLimit;
      }
    }
  }
  lastX = e->x();
  lastY = e->y();
  emit selectRegionChanged(region->chromStart, region->chromEnd, selectBegin, selectEnd);
  update();
}

void RegionWidget::mouseReleaseEvent(QMouseEvent* e){
  // something..
  // and work out what selectBegin and SelectEnd should be. 
  if(selectingRegion && lastX != firstX){
    if(hasData){
      int first = (firstX < lastX) ? firstX : lastX;
      int last = (lastX > firstX) ? lastX : firstX;  // just so I know
      if(first > lm && last < (width() - rm)){       // forget it if they are both out of range..
	int tempBegin = selectBegin + ((float)(first-lm)*(float)(selectEnd-selectBegin)) / (float)(width()-lm-rm);
	int tempEnd = selectBegin + ((float)(last-lm)*(float)(selectEnd-selectBegin)) / (float)(width()-lm-rm);
	selectBegin = tempBegin; 
	selectEnd = tempEnd;
	emit selectRegionChanged(region->chromStart, region->chromEnd, selectBegin, selectEnd);
      }
    }
  }
  selectingRegion = false;
  changingSubRegion = false;
  leftLimitChanging = false;
  rightLimitChanging = false;
  update();
}

void RegionWidget::mouseDoubleClickEvent(QMouseEvent* e){
  if(hasData){
    selectBegin = region->chromStart;
    selectEnd = region->chromEnd;
    emit selectRegionChanged(region->chromStart, region->chromEnd, selectBegin, selectEnd);
  }
  long test;
  cout << "and the size of a long is " << sizeof(long) << endl;
}

void RegionWidget::setMaxExp(int me){
  maxExpect = me;
  update();
}

void RegionWidget::setMinAll(int mall){
  minAllength = mall;
  update();
}

void RegionWidget::setMinMatch(int mm){
  minMatch = mm;
  update();
}

void RegionWidget::setSelectRegion(int ss, int se){
  if(ss > se){
    return;
  }
  if(ss < region->chromStart || se > region->chromEnd){
    return;
  }
  selectBegin = ss;
  selectEnd = se;
  update();
}

void RegionWidget::getEnsemblGenes(){
  if(hasData){
    emit pleaseGetEnsemblGenes((int)region, region->chromosome, region->chromStart, region->chromEnd);
  }
}
