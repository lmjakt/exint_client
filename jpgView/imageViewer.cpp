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

#include "imageViewer.h"
#include "thumbView.h"
#include "../dataStructs.h"
#include <qwidget.h>
#include <qcstring.h>
#include <qpainter.h>
#include <qsizepolicy.h>
#include <map>
#include <iostream>

using namespace std;

//labelRect::~labelRect(){
  //cout << "address of label is : " << label << endl;
  //if(label){
    //delete label;
    //label = 0;
//}
//  cout << "and now the addres is : " << label << endl;
//}

ImageViewer::ImageViewer(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  // don't know if I want to do anything in particular here.. I think the situation may by 
  // under control
  w = 256;
  h = 175;
  vmargin = 10;
  hmargin = 10;
  wantSize = QSize(500, 700);
  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));   // not that it's going to make any bloody difference.. 
  //  resize(500, 700);   // arbitrary resize..
  movingImage = 0;
  currentSelection = 0;
  selectedColor = QColor(130, 165, 165);
  normalColor = QColor(200, 200, 200);
  menu = new QPopupMenu(this, "menu");
  menu->insertItem("User Annotation", this, SIGNAL(toggleUserAnnotation()) );
}

void ImageViewer::addFullImage(ishImageData data){
  ThumbView* image = new ThumbView(data);    // toplevel window
  connect(image, SIGNAL(whoAmI(int)), this, SIGNAL(whoIsThis(int)) );
  connect(image, SIGNAL(deleteMe(int)), this, SLOT(removeImage(int)) );
  image->setImage(data.data);
  image->scale(0.5);       // it's a bit big otherwise,, but,
  image->resize(image->viewSize());
  fullImages.insert(make_pair(data.imageIndex, image));
  image->show();
}

void ImageViewer::addImage(ishImageData data, QString labelText){
  /// first check if we have the image or not, if the case then return without doing anything,, otherwise 
  /// put into the set.. currentImages..
  if(currentImages.count(data.imageIndex)){
    return;
  }
  currentImages.insert(data.imageIndex);
  cout << "constructing a thingy.. ok lall.... " << endl;
  cout << "making a image with label " << labelText << endl;
  ThumbView* image = new ThumbView(data, this);  
  connect(image, SIGNAL(whoAmI(int)), this, SIGNAL(whoIsThis(int)) );
  connect(image, SIGNAL(getImage(int)), this, SLOT(getFullImage(int)) );
  connect(image, SIGNAL(deleteMe(int)), this, SLOT(removeImages(int)) );
  cout << " just before making a handle" << endl;
  QRect handle;
  QLabel* label = new QLabel(labelText, this, "imagelabel");
  label->setPaletteBackgroundColor(normalColor);
  cout << "constructred the label" << endl;
  imageHandles.insert(make_pair(image, labelRect(handle, label)));
  cout << "inserted the thingy into the map" << endl;
  //  imageLabels.insert(make_pair(image, label));
  image->setImage(data.data);
  // and insert into the map..
  // let's set it as an arbitrary size of 256 * 200 for now..
  //int w = 256;
  //int h = 175;
  //int vmargin = 10;
  //int hmargin = 10;
  int colWidth = w + 2*vmargin;
  int rowHeight = h + 2*hmargin;   // give us s
  image->resize(w, h);
  images.insert(make_pair(data.ishProbeIndex, image));
  /////////////////////////////////////////////////////////
  // and now for the tricky bit. I still haven't found any good way of 
  // using the different QLayouts in a dyanmic way in such a way that I can easily move them around and other stuff..
  // so I'm going to use some stupid heuristic to arrange the images...
  

  // hmmmm,,, all images are going to have dimensions w and h,, so it's merely a matter of working out what the best thingy is..
  int columnNo = width()/colWidth ? width()/colWidth : 1;   // if width()/w evaluates to more than 0 use it, otherwise use 1.
  int rowNo = images.size()/columnNo + 1;
  wantSize = QSize(columnNo * colWidth, rowNo * rowHeight);
  //resize(wantSize);
  int count = 0;
  map<int, ThumbView*>::iterator it;
  map<ThumbView*, labelRect>::iterator qit;
  //map<ThumbView*, QLabel*>::iterator lit;
  for(it = images.begin(); it != images.end(); it++){
    int col = count % columnNo;
    int row = count / columnNo; 
    count++;
    cout << "trying to move the image to col " << col << "  and row : " << row << endl
	 << "by setting geomtry to : " << col*colWidth + hmargin << ", " << row*rowHeight+hmargin << endl; 
    //    (*it).second->setGeometry(col*colWidth + hmargin, row*rowHeight+hmargin, w, h);
    (*it).second->move(col*colWidth + hmargin, row*rowHeight+hmargin);
    (*it).second->show();   /// shouldn't really be necessary ?? 
    qit = imageHandles.find((*it).second);
    (*qit).second.rect.setRect(col*colWidth + hmargin, (row+1)*rowHeight-hmargin, w, vmargin);
    //    lit = imageLabels.find((*it).second);
    (*qit).second.label->setGeometry((*qit).second.rect);    // should be OK!! 
    (*qit).second.label->show();     // ?? 
    
  }
  if(!isVisible()){
    show();
  }
}

QSize ImageViewer::goodSize(QSize avSpace){
  if(!images.size()){
    return(avSpace);
  }
  int colWidth = w + 2*vmargin;
  int rowHeight = h + 2*hmargin;   
  //// use parent width for the column no calculation instead.. that way we can get better fits.. 
  int pWidth = width();
  if(parent()){
    pWidth = ((QWidget*)parent())->width();
  }
  int columnNo = pWidth/colWidth ? pWidth/colWidth : 1;   // if width()/w evaluates to more than 0 use it, otherwise use 1.
  int rowNo = images.size()/columnNo + 1;
  int needWidth = columnNo * colWidth;
  int needHeight = rowNo * rowHeight;
  needWidth = needWidth > avSpace.width() ? needWidth : avSpace.width();
  needHeight = needHeight > avSpace.height() ? needHeight : avSpace.height();
  return(QSize(needWidth, needHeight));
}


QSize ImageViewer::sizeHint() const {
  return(wantSize);
}

void ImageViewer::mousePressEvent(QMouseEvent* e){
  map<ThumbView*, labelRect>::iterator qit;
  for(qit = imageHandles.begin(); qit != imageHandles.end(); qit++){
    if((*qit).second.rect.contains(e->pos())){
      movingImage = (*qit).first;
      break;
    }
  }
  // then we want to move this one.. -- by the difference in the points..  lastX = pressX = e->x();
  lastY = pressY = e->y();
  lastX = pressX = e->x();
  lastPoint = e->pos();
  if(e->button() == Qt::RightButton){
    menu->popup(mapToGlobal(e->pos()));
  }
}

void ImageViewer::mouseMoveEvent(QMouseEvent* e){
  // check if we are moving any images
  if(movingImage){
    map<ThumbView*, labelRect>::iterator qit = imageHandles.find(movingImage);
    if(qit == imageHandles.end()){
      lastPoint = e->pos();
      lastX = e->x();
      lastY = e->y();
      return;
    }
    //cout << "we've hit a qrect, so will try to move something or other.." << endl;
    int dx = e->x() - lastX;
    int dy = e->y() - lastY;
    //cout << "dx : " << dx << "  dy : " << dy << endl;
    (*qit).second.rect.moveBy(dx, dy);
    (*qit).second.label->setGeometry((*qit).second.rect);
    QPoint pos = (*qit).first->pos();
    (*qit).first->move(pos.x() + dx, pos.y()+dy);
    // and that 's it really.
    update();
  }
  lastPoint = e->pos();
  lastX = e->x();
  lastY = e->y();
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* e){
  movingImage = 0;
}


void ImageViewer::resizeEvent(QResizeEvent* e){
  //  int w = 256;
  //int h = 175;
  //int vmargin = 10;
  //int hmargin = 10;
  int colWidth = w + 2*vmargin;
  int rowHeight = h + 2*hmargin;   // give us s
  int count = 0;
  int parentWidth = width();      // default, assume no parent.. 
  if(parent()){
    parentWidth = ((QWidget*)parent())->width();      // this might be dangerous, -- say if there is no parent.. 
  }
  int columnNo = parentWidth/colWidth ? parentWidth/colWidth : 1;   // divide by 0 ?? 
  cout << "the width of my parent is " << ((QWidget*)parent())->width() << endl;
  cout << "and my width is " << width() << endl;

  map<int, ThumbView*>::iterator it;
  map<ThumbView*, labelRect>::iterator qit;
  for(it = images.begin(); it != images.end(); it++){
    int col = count % columnNo;
    int row = count / columnNo; 
    count++;
    cout << "trying to move the image to col " << col << "  and row : " << row << endl
	 << "by setting geomtry to : " << col*colWidth + hmargin << ", " << row*rowHeight+hmargin << endl; 
    //    (*it).second->setGeometry(col*colWidth + hmargin, row*rowHeight+hmargin, w, h);
    (*it).second->move(col*colWidth + hmargin, row*rowHeight+hmargin);
    qit = imageHandles.find((*it).second);
    (*qit).second.rect.setRect(col*colWidth + hmargin, (row+1)*rowHeight-hmargin, w, vmargin);
    (*qit).second.label->setGeometry((*qit).second.rect);
    //(*it).second->show();   /// shouldn't really be necessary ?? 
  }
}

void ImageViewer::setLabel(int index, QString label){
  // first see if we can find a widget..
  pair< multimap<int, ThumbView*>::iterator, multimap<int, ThumbView*>::iterator > range = images.equal_range(index);
  multimap<int, ThumbView*>::iterator it;     //
  for(it = range.first; it != range.second; it++){
    map<ThumbView*, labelRect>::iterator qit = imageHandles.find((*it).second);
    if(qit != imageHandles.end()){
      (*qit).second.label->setText(label);
    }
  }
}

void ImageViewer::getFullImage(int index){
  map<int, ThumbView*>::iterator it = fullImages.find(index);
  if(it == fullImages.end()){
    emit getImage(index);
    return;
  }
  // if we are here, just see if we can make it toplevel.. hmmm ..
  // we could do a hide() , followed by a show(), but this is alittle bit ugly.. 
  //(*it).second->showMinimized();
  //(*it).second->update();
  (*it).second->raise();
  (*it).second->show();
  //(*it).second->update();
}

void ImageViewer::selectedChanged(int index){
  // we have to find the images associated with the old index...
  pair< multimap<int, ThumbView*>::iterator, multimap<int, ThumbView*>::iterator > range = images.equal_range(currentSelection);
  multimap<int, ThumbView*>::iterator it;     //
  for(it = range.first; it != range.second; it++){
    map<ThumbView*, labelRect>::iterator qit = imageHandles.find((*it).second);
    if(qit != imageHandles.end()){
      (*qit).second.label->setPaletteBackgroundColor(normalColor);
    }
  }
  range = images.equal_range(index);
  for(it = range.first; it != range.second; it++){
    map<ThumbView*, labelRect>::iterator qit = imageHandles.find((*it).second);
    if(qit != imageHandles.end()){
      (*qit).second.label->setPaletteBackgroundColor(selectedColor);
    }
  }
  currentSelection = index;
}

void ImageViewer::removeImage(int index){
  cout << "in removeImage function, what's going on" << endl;
  /// this only removes one image,, and is connected to by the full sized images..
  map<int, ThumbView*>::iterator it = fullImages.find(index);
  if(it == fullImages.end()){
    return;
  }
  ThumbView* image = (*it).second;
  fullImages.erase(it);
  delete image;
}

void ImageViewer::removeImages(int index){
  // this it turns out is a little more delicate than I had expected. There are two problems..
  // 1. the index emitted by thumbview is the image index, not the probe index, and we don't have a map to deal with this
  // 2. we must make sure to remove the relevant image handle for which we need a descructor.. -- this is a bit ugly
  // 
  // as usual fudge it and use some ugly code rather than rewriting the maps. (I am thinking of changing it so that the user can 
  // reorder the images, but that will take some more work,, -and which would involve changing the maps again..
  ThumbView* image = (ThumbView*)sender();  // cast to ThumbView.. 
  cout << "trying to remove an image with address : " << image << endl;
       
  if(image->imageIndex() == index){
    map<ThumbView*, labelRect>::iterator lit = imageHandles.find(image);
    if(lit != imageHandles.end()){
     QLabel* label = (*lit).second.label;
     cout << "about to delete a label with address : " << label << endl;
     label->hide();
     delete label;
    }                          // now this is awfully ugly.. and necessitated by lazy design of the labelRect.. -- which I dont' really need. 
    cout << "just before calling erase on the imageHandles map, maybe I should use an iterator here " << endl;
    imageHandles.erase(image);
    int probeIndex = image->probeIndex();
    cout << "the probe index for the image is : " << probeIndex << endl;
    pair< multimap<int, ThumbView*>::iterator, multimap<int, ThumbView*>::iterator > range = images.equal_range(probeIndex);
    multimap<int, ThumbView*>::iterator it;     //
    for(it = range.first; it != range.second; it++){
      cout << "\tgoing through the images to check things out" << endl;
      if((*it).second->imageIndex() == index){
	cout << "calling erase on the map " << endl;
	images.erase(it);
	cout << "after calling erase on the map" << endl;
	break;           // this should not be necessary, but I'm getting a crash with Borland here.. 
      }
    }
    cout << "just before calling delete on the image : " << endl;
    delete image;   
    // and let's see if we can delete the full sized image..
    map<int, ThumbView*>::iterator tit = fullImages.find(index);
    if(tit != fullImages.end()){
      ThumbView* fImage = (*tit).second;
      fullImages.erase(tit);
      delete fImage;
    }
  }
  //// whoah that's some ugly code, but never mind.. just make sure to remove the index from the currentImages..
  currentImages.erase(index);
}

void ImageViewer::moveWidget(int dx, int dy){
    dx=dy;  // to remove the warning.. 
  //  QWidget* image = (QWidget*)sender();
  //image->move(image->x() + dx, image->y()+dy);
}

// void ImageViewer::paintEvent(QPaintEvent* e){
//   cout << "ImageViewer paint event " << endl;
//   map<ThumbView*, labelRect>::iterator qit;
//   QPainter p(this);     // don't bother to paint to thingy..
//   p.setBrush(QColor(125, 125, 125));
//   p.setPen(Qt::NoPen);
//   for(qit = imageHandles.begin(); qit != imageHandles.end(); qit++){
//     //p.drawRect((*qit).second.rect);
//   }
// }
  
