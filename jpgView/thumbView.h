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

#ifndef THUMBVIEW_H
#define THUMBVIEW_H

#include "jpgView.h"
#include "../dataStructs.h"
#include <qwidget.h>
#include <qpopupmenu.h>

using namespace std;

class ThumbView : public JpgView
{
  Q_OBJECT
    public :
    ThumbView(ishImageData d, QWidget* parent=0, const char* name=0);
  ~ThumbView();
  int imageIndex(){
    return(data.imageIndex);
  }
  int probeIndex(){
    return(data.ishProbeIndex);
  }

  private slots :
    void requestImage();
  void saveImage();
  void copyImage();    // copy to clipboard.. 
  void removeMe();  

  private :
    void mousePressEvent(QMouseEvent* e);   // override function.. ??
  void mouseReleaseEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void enterEvent(QEvent* e);              // set to focus when the mouse enters.. 
  void leaveEvent(QEvent* e);              // remove focus.. 

  QPopupMenu* imageMenu;    // things for the image.. 
  ishImageData data;        // ish image data.. maybe a little redundant, but easier this way.. 
  int lastgx, lastgy;       // global x and y.. 

  signals :
    void getImage(int);    // the image index.. 
  void whoAmI(int);        // get some information about myself.. and hope that somehone displays it.. 
  void deleteMe(int);      // delete all images associated with this imageIndex (if I'm a thumbnail, if full size then just the sender.. 

};

#endif
