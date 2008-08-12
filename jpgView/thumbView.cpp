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

#include "thumbView.h"
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <iostream>

using namespace std;

ThumbView::ThumbView(ishImageData d, QWidget* parent, const char* name)
  : JpgView(parent, name)
{
  data = d;
  imageMenu = new QPopupMenu(this, "imageMenu");
  imageMenu->insertItem("Full Size", this, SLOT(requestImage()));   // some serious problems with setting the focus such that the appropriate widget gets the signal.. 
  imageMenu->insertItem("Save to JPEG", this, SLOT(saveImage()));   // for the keboard accelerators.. -may have to do far more complicated things.. 
  imageMenu->insertItem("Copy", this, SLOT(copyImage()));
  imageMenu->insertItem("Delete Me", this, SLOT(removeMe()));
  //imageMenu->insertItem("Full Size", this, SLOT(requestImage()), CTRL+Key_I, 1);
  //imageMenu->insertItem("Save to JPEG", this, SLOT(saveImage()), CTRL+Key_S, 2);
  //imageMenu->insertItem("Copy", this, SLOT(copyImage()), CTRL+Key_C, 3);
  //  imageMenu->insertItem("Delete Me", this, SLOT(removeMe()), CTRL+Key_D, 4);
  setFocusPolicy(QWidget::StrongFocus);
 // for now, do nothing..
}

void ThumbView::mousePressEvent(QMouseEvent* e){
  if(e->button() == Qt::RightButton){
    imageMenu->popup(mapToGlobal(e->pos()));
  }
  x = lastX = e->x();
  y = lastY = e->y();
  lastgx = e->globalPos().x();
  lastgy = e->globalPos().y();
}

void ThumbView::mouseReleaseEvent(QMouseEvent* e){
  //  moving = false;
  if(printZoomFactor){
    printZoomFactor = false;
    scale(virtualScale);
  }
  if(e->x() == x && e->y() == y && e->state() == Qt::LeftButton){
    //cout << "am emitting whoAmI " << endl;
    emit whoAmI(data.ishProbeIndex);
  }
}

void ThumbView::mouseMoveEvent(QMouseEvent* e){
  if(e->state() == Qt::LeftButton){
    xo += ((float)(lastX - e->x()))/(float)(scaledImage.width());
    yo += ((float)(lastY - e->y()))/(float)(scaledImage.height());
    // top the offsets
    xo = xo < 0 ? 0 : xo;
    yo = yo < 0 ? 0 : yo;
    xo = xo > 1 ? 1 : xo;
    yo = yo > 1 ? 1 : yo;
    update();
    //cout << "x " << e->x() << "  y " << e->y() << "  xo " << xo << "  yo " << yo << endl;
  }
  if(e->state() == (Qt::LeftButton + Qt::ControlButton)){
    // change Virtual Scale, and print out.. 
    virtualScale += (float)(lastY - e->y())/(float)200;
    // make sure not below 0..
    virtualScale = virtualScale > 0.1 ? virtualScale : 0.1;
    // generate a paintevent and see if we can call the paintevent method.. 
    int w = 75;
    int h = 45;   // should be enough space for writing the the zoom factor in..
    printZoomFactor = true;
    repaint(x, y, w, h, false);
    //    QPaintEvent* pe = new QPaintEvent(QRegion(x, y, w, h), false);
    //paintEvent(pe);
  }
  if(e->state() == (Qt::LeftButton + Qt::ShiftButton)){
    //moving = true;
    // we want to move by x and y...
    int dx = e->globalPos().x() - lastgx;
    int dy = e->globalPos().y() - lastgy;
    QPoint p = mapToGlobal(pos());
    int x = p.x();
    int y = p.y();
    QPoint d = mapFromGlobal(QPoint(x+dx, y+dy));
    move(d);
    //emit moveMe(dx, dy);
    //    move(e->pos());
    //update();
  }
  lastX = e->x();
  lastY = e->y();
  lastgx = e->globalPos().x();
  lastgy = e->globalPos().y();
}

void ThumbView::requestImage(){
  //cout << "Should be requesting an image, but don't yet know the index,, have to work this out" << endl;
  cout << "Requesting an image with index : " << data.imageIndex << endl;
  emit getImage(data.imageIndex);
}

void ThumbView::saveImage(){
  QString fileName = QFileDialog::getSaveFileName();
  if(fileName.isNull()){
    return;
  }
  if(!scaledImage.save(fileName, "JPEG", 100)){
    cerr << "Coulnd't save file to " << fileName << endl;
  }
}
void ThumbView::removeMe(){
  emit deleteMe(data.imageIndex);
}

void ThumbView::copyImage(){
  QClipboard *cb = QApplication::clipboard();
  cb->setImage(scaledImage);
}

void ThumbView::enterEvent(QEvent* e){
  //  cout << "setting focus" << endl;
  //grabKeyboard();
  //setFocus();
  //if(isFocusEnabled()){
  //  cout << "Focus is enabled for the widget" << endl;
  //}
}

void ThumbView::leaveEvent(QEvent* e){
  //cout << "clearing focus" << endl;
  //releaseKeyboard();
  //clearFocus();
}

ThumbView::~ThumbView(){
  delete image; // ??
}
