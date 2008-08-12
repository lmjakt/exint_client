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

#ifndef GENALIGNWIDGET_H
#define GENALIGNWIDGET_H

#include <qwidget.h>
#include <qpopupmenu.h>
#include <vector>
#include <qrect.h>
#include <set>
#include "../sequence/sequenceRequest.h"
#include "../probe_set/probe_set.h"

using namespace std;

struct alignBox {
  // contains a QRect, and pointers to the related blastAlignment and 
  // blastGenomeMatch. 
  // have a vector of these created each time we redraw,, or something like that.
  alignBox(QRect r, const blastAlignment* al, const blastGenomeMatch* bm);
  alignBox();
  QRect box;
  const blastAlignment* alignment;
  const blastGenomeMatch* match;
};

struct sequenceSelect {
  // if a sequence is selected with mouse it contains the details of that selection
  // and is looked up during the thing. It contains a pointer to a blastAlignment, plus
  // the blastMatch, plus the range. Defualt is +- 1000 bp of the start and the end. 
  sequenceSelect();
  void clear();
  void setAlignment(const blastAlignment* al, const blastGenomeMatch* m, const alignBox*);
  //void changeAbove(int increment);
  //void changeBelow(int increment);
  int above;
  int below;   // how much up or down.. 
  int aboveDelta;
  int belowDelta;       // a pixel based change from the mouse that can be used to calculate the desired sequence change.
  const blastAlignment* alignment;
  const blastGenomeMatch* match;
  const alignBox* box;
};

struct geneSelectBox {
  geneSelectBox();
  geneSelectBox(QRect r, int index);
  int geneIndex;
  QRect box;
};

class GenAlignWidget : public QWidget
{
  Q_OBJECT

    public :
    GenAlignWidget(QWidget* parent=0, const char* name=0);
  ~GenAlignWidget();
  int sizeWish(){
    return desiredHeight;
  }

  public slots :
    void setData(probe_data* d);     // use this as we can then access a whole load of other things if we should wish to.. 

  private slots :
    void getSequences();   // check the selected Sequences, create a sequenceRequest and emit it into the stratosphere

  private :
    int lastX;
  int lastY;    // for moveMe.. signal.. 
  void paintEvent(QPaintEvent* e);
  probe_data* data;     // keep the pointer for redraws.. 
  void drawArrow(int xs, int xe, int y, int w, QPainter* p, QColor c, bool forward);    // convenience function for drawing an arrow. Nice.. 
  void drawRange(int xs, int xe, int y, int w, QPainter* p, QColor c);
  void drawCheckBox(int x, int y, int w, bool checked, int index, QPainter* p, bool isPeptide=false);
  
  bool hasData;     // just in case.. 
  int desiredHeight;      // how high do I want to be??  -resize to this in paint events..  calculate in set data. 
  double rowHeight;
  double topSpace;
  double matchSpace;      // needed both to calculate the required height and for drawing.. 
  double labelHeight;    
  vector<alignBox> alignmentBoxes;  // keep a track of the numbers, so we can see if the thing has been clicked.
  sequenceSelect selectedSequence;  // sequence selected, -used for sequence retrieval.. 
  vector<geneSelectBox> geneBoxes;  // selected genes. -for obtaining the transcripts. 
  vector<geneSelectBox> peptideBoxes; // for selecting peptide sequences. Similar sort of thing.. hhoo yeaah. 
  set<int> selectedGenes;           // the numbers of the selected genes so we know if something is selected or not. 
  set<int> selectedPeptides;        // uses the gene indices.. 
  QPopupMenu* saveMenu;

  protected :
    virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);

  signals :
    void mousePress(QMouseEvent*);
  void mouseMove(QMouseEvent*);
  void moveMe(int, int);
  void collectSequences(SequenceRequest*);
};

#endif
