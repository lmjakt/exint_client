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

#include "genAlignView.h"
#include "../probe_set/probe_set.h"
#include "../sequence/sequenceRequest.h"
#include <qwidget.h>
#include <qscrollview.h>
#include <qframe.h>
#include <qlayout.h>
#include <qcolor.h>
#include <iostream>     // just in case...

using namespace std;         // which I probably don't need at the moment but there you go..

GenAlignView::GenAlignView(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  // set up the scroll view and things.. 
  // the question is, do I need a qlayout for the qscrollview, or is it ok without. 
  // well we'll just have to wait and see.
  setBackgroundColor(QColor(0, 0, 0));
  scroll = new QScrollView(this, "scroll");
  //scroll->setBackgroundColor(QColor(0, 0, 0));

  //  frame = new QFrame(scroll->viewport());
  //frame->setBackgroundColor(QColor(0, 0, 0));
  //scroll->addChild(frame);

  alignWidget = new GenAlignWidget(scroll->viewport());
  //connect(alignWidget, SIGNAL(moveMe(int, int)), this, SLOT(moveMe(int, int)) );
  connect(alignWidget, SIGNAL(collectSequences(SequenceRequest*)), this, SIGNAL(collectSequences(SequenceRequest*)) );
  //  connect(alignWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)) );
  scroll->addChild(alignWidget);
  //  QVBoxLayout* innerBox = new QVBoxLayout(scroll->viewport());
  //innerBox->addWidget(alignWidget);
  //innerBox->addStretch();

  //alignWidget = new GenAlignWidget(frame, "alignWidget");
  QVBoxLayout* vBox = new QVBoxLayout(this);
  vBox->addWidget(scroll);
  //  vBox->addStretch();
  // and that should be pretty much everything needed.
}

void GenAlignView::setData(probe_data* pdata){
  alignWidget->setData(pdata);   // and resize it..   which means two paint events. hmm ugly
  //alignWidget->resize(scroll->viewport()->width(), alignWidget->sizeWish());
  if(alignWidget->sizeWish() > scroll->viewport()->height()){
    alignWidget->resize(scroll->viewport()->width(), alignWidget->sizeWish());
  }else{
    //scroll->viewport()->resize(size()); // ??
    scroll->resizeContents(width(), height());
    alignWidget->resize(size());
    //alignWidget->update();
  }  // frame->resize(scroll->viewport()->width(), alignWidget->sizeWish());
  alignWidget->repaint();
}

void GenAlignView::resizeEvent(QResizeEvent* e){
  // resize frame -- ? do I need to or not??
  //frame->resize(scroll->viewport()->width(), alignWidget->sizeWish());
  if(alignWidget->sizeWish() > scroll->viewport()->height()){
    alignWidget->resize(scroll->viewport()->width(), alignWidget->sizeWish());
  }else{
    //scroll->viewport()->resize(size()); // ??
    scroll->resizeContents(width(), height());
    alignWidget->resize(size());
  }
  //  alignWidget->resize(frame->width(), alignWidget->sizeWish());
  //alignWidget->update();
}

// void GenAlignView::mousePress(QMouseEvent* e){
//   //cout << "mousePressEvent, updating lastX and lastY" << endl;
//   lastX = e->x();
//   lastY = e->y();
// }

// void GenAlignView::mouseMove(QMouseEvent* e){
//   //cout << "mouseMove event, updating scrolling ? " << endl;
//   scroll->scrollBy(lastX - e->x(), lastY- e->y());
//   lastX = e->x();
//   lastY = e->y();
// }

void GenAlignView::moveMe(int x, int y){
  scroll->scrollBy(x, y);
}
