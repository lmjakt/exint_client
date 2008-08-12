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

#ifndef REGIONWIDGET_H
#define REGIONWIDGET_H

#include "../client.h"
#include <qwidget.h>
#include <qrect.h>
#include <qpopupmenu.h>
#include <vector>

using namespace std;

struct psBox{
  psBox();
  psBox(QRect b, int i);
  QRect box;
  int psIndex;
};

class RegionWidget : public QWidget
{
  Q_OBJECT

    public :
    RegionWidget(QWidget* parent=0, const char* name=0);
  
  public slots :
    void setData(int me, int mall, int mm, chromRegion* cr);
  void setMaxExp(int me);
  void setMinAll(int mall);
  void setMinMatch(int mm);
  void setSelectRegion(int ss, int se);

  private slots :
    void loadProbeSets();     // emit a vector index type signal, and propagate through the window. Could do more directly, but I like this way. 
  void getEnsemblGenes();

  private :
    void paintEvent(QPaintEvent* e);

  chromRegion* region;
  bool hasData;
  vector<psBox> boxes;
  //vector<egBox> egBoxes;  // ensembl genes.. 
  QPopupMenu* actionMenu;
  // some rectangles to show and manipulate the current selection.. 
  QRect wholeRegion;
  QRect subRegion;   // the selected one..
  QRect leftHandle;  // the left handle
  QRect rightHandle;  // and the right handle.. -- should be all that's necessary.. 


  int lm, rm, tm, bm;      // margins, left, right, top, bottom-- may need to know what these are at times..
  int lastX, lastY;        // so I can check selections.. -may not need, but what the hell. 
  bool selectingRegion;    // if the user is in the process of selecting a region. i.e. moving the mouse
  bool changingSubRegion;
  bool leftLimitChanging;
  bool rightLimitChanging;
  int firstX, firstY;      // also needed for the above.. 
  int maxExpect, minAllength, minMatch;    // values that determine whether or not we draw. (expect is in log terms., hence an int).

  /// and if I select a region, I will need to have somewhere to store the coordinates for that region. Fortunately this is not that difficult.
  int selectBegin, selectEnd;

  /// and some labels and stuff for selecting regions and things.. 
/*   QLabel* chromLabel; */
/*   QLineEdit* selectStart; */
/*   QLineEdit* selectEnd;  */

  protected :
    virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);      // so I can select things.. 
  virtual void mouseReleaseEvent(QMouseEvent* e);
  virtual void mouseDoubleClickEvent(QMouseEvent* e);

  signals :
    void pleaseLoadGenes(vector<int>, QString);
  void selectRegionChanged(int, int, int, int);     // min, max, selectstart, selectend.. 
  void pleaseGetEnsemblGenes(int, string, int, int);  // request id, chromsome, start, end.. 
};

#endif
