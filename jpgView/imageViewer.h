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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "thumbView.h"
#include <qwidget.h>
#include <qrect.h>
#include <qlabel.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <map>
#include <set>
#include "../dataStructs.h"
#include "qpoint.h"

using namespace std;

struct labelRect {
  QRect rect;
  QLabel* label;
  labelRect(){
    label = 0;
  }
  labelRect(QRect r, QLabel* l){
    rect = r;
    label = l;
  }
  //  ~labelRect();
};

class ImageViewer : public QWidget
{
  Q_OBJECT
    public :
    ImageViewer(QWidget* parent=0, const char* name=0);
  QSize sizeHint() const;
  QSize goodSize(QSize avSpace);    // given the available space, what size would you like to be?? ?? 

  public slots :
    void addImage(ishImageData data, QString label);    // data that we can construct a jpgView from.. always assuming Jpgs here.. that's bad..
  void addFullImage(ishImageData data);     // a full sized image.. make top level thingy.. or something.. not sure how to destroy it though.. 
  void setLabel(int index, QString labelText);  // set the label of images using a specific probe.. or something like that.. 
  void selectedChanged(int index);              // when the information display changes probe or something.. 

  private slots :
    void getFullImage(int imageIndex);          // catch the signal from the thumbview, and check if we already have it.. otherwise send signal
    void removeImages(int);         // signal from itself.. but how to find it.. hmm. could keep an index, but implement that later .. 
    void removeImage(int);          // just remove the full sized image.. 
  void resizeEvent(QResizeEvent* e);
  void moveWidget(int dx, int dy);     

  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  //  void showUserAnnotation();
  //void paintEvent(QPaintEvent* e);
 
  private :
    QPopupMenu* menu;                  // just for calling the annotator.. clumsy,, but there you go.. 
    multimap<int, ThumbView*> images;  // the int is the ishprobe index, and the view. the image..
  map<ThumbView*, labelRect> imageHandles; // some handles for moving the rects with, perhaps.. 
  //map<ThumbView*, QLabel*> imageLabels; // a label,, unfortunately we will have to rely on the parent for part of the text as we should not keep probe data here.
  map<int, ThumbView*> fullImages;            // just full images..   -- obviously shouldnt' be a thumbview, but fix it later.. with a derived class.  -- the int is the identifier (row not the actual oid) for the image.. -so we know if we've got it or not..  
  set<int> currentImages;             // keeps tab of the images currently displayed.. so we don't redisplay images already somewhere else.. 

  int pressX, pressY;                // initial mouse press position..
  int lastX, lastY;                  // last positions.. 
  int w, h, vmargin, hmargin;        // set up in constructor..  -- width's and heights of the thumbnails.. 
  QPoint lastPoint;                  // hmm, maybe that is a bit redundant.. but.. 
  ThumbView* movingImage;              // 0 if not moving, then we can use it as a boolean as well.. 
  QSize wantSize;                      // size I want to be.. 
  int currentSelection;               // the currently selecte ishProbe.. i.e. when we click the thingy.. 
  QColor selectedColor;
  QColor normalColor;

  signals :
    void getImage(int);   // get's full sized image.. -- 
  void whoIsThis(int);
  void toggleUserAnnotation();
};

#endif
