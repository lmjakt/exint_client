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

#include "2dplot.h"
#include "../stat/kernelFunction.h"
#include <qpainter.h>
#include <iostream>
#include <math.h>

using namespace std;

TwoDPlot::TwoDPlot(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  setBackgroundMode(NoBackground);
  drawBackground = false;
  resize(300, 300);
  pixmap = new QPixmap(size());
  //  background = new QPixmap(size());
  maxV = minV = 0;                // if the real minimum value is more than 0, then leave minV as 0.
  maxX = minX = minY = maxY = 0;  // but this time, the mins have to be the real mins..
  probeNo = 0;                    // just incrememnt it when things get better.. 
  menu = new QPopupMenu(this, "menu");
  menu->insertItem("Toggle background", this, SLOT(toggleBackground()) );
}

void TwoDPlot::plot(vector<vector<float> >& v, vector<int>& exIndex, vector<int>& marks){
  // the first dimension of the colours vector is based on the experiments, -but whatever the
  // case we are going to keep all the posisitions here, so that it is obvious when we have data, and 
  // when we don't. OK la..  -- however, we only have expression data for the thingies, so that we can do something with them

  cout << "TwoDPlot plot function : " << endl;
  values = v;
  exptIndex = exIndex;
  markedOnes = marks;
  
  if(!isVisible()){
    // if I'm not visible, don't bother.. 
    return;
  }


  // first empty the colours for each set..
  map<int, twoDPos>::iterator it;
  for(it = pos.begin(); it != pos.end(); it++){
    (*it).second.colors.resize(0);   // i.e. remove values.. use push to add values.. if we can find them.. 
    (*it).second.v.resize(0);
    (*it).second.marked = false;
    (*it).second.sum = 0;
  }
  // and now go through the experimental values for each probe set. 
  
  float min, max;
  min = max = 0;
  for(int i=0; i < v.size(); i++){
    for(int j=0; j < v[i].size(); j++){
      if(v[i][j] > max){ max = v[i][j];}
      if(v[i][j] < min){ min = v[i][j];}
    }
  }
  for(int i=0; i < v.size(); i++){
    if(v[i].size() != exIndex.size()){
      cerr << "TwoDPlot plot function BUGGERED as a result of exIndex vector not being the same size as thingy no: " << i << endl;
      return;
    }
    //min = max = 0;    // If everything is positive, well, we'll want to know that.. 
    probeNo = v.size();  // ok. no worries.. that's easy..
    /// use a globally defined min and max, -- as the data has been thingied already.. 
//     for(int j=0; j < exIndex.size(); j++){
//       // Ok basically map from min to max. As we do this for each probe pair, abberrant values shouldn't give us too much trouble..
//       if(v[i][j] < min){ min = v[i][j]; }
//       if(v[i][j] > max){ max = v[i][j]; }
//     }
    // and then the tricky bit, go through each one and add to the appropriate experiment.
    // This sucks as we will use a map iterator to find the appropriate point.. -- usint one defined earlier..
    for(int j=0; j < exIndex.size(); j++){
      it = pos.find(exIndex[j]);
      if(it != pos.end()){
	int r, g, b;
	// if we have a positive value, lets use the red channel, if a negative value let's use a green channel, which is 
	// scaled according to the maximum value. -- we can change this later, but if we have absolute data, then we want an 
	// obvious difference between the two.. 
	cout << "v[i][j] is : " << v[i][j] << "  and max : " << max <<  endl;
	if(v[i][j] > 0){
	  int l = (int) ((v[i][j]/max) * (float)767);   // i.e. just sum of the three .. 
	  cout << "\t\t so l is " << l << endl;
	  r = g = b = 0;   // default..
	  if(l >= 512){
	    b = l - 512;
	    g = 255;
	    r = 255;
	  }
	  if(l >= 256 && l < 512){
	    g = l - 256;
	    r = 255;
	  }
	  if(l > 0 && l < 256){
	    r = l;
	  }
	}else{
	  //r = (int) ((v[i][j]/max) * (float)255);
	  //g = 0;
	  //b = r;
	  g = (int) ((v[i][j]/max) * (float)-255);
	  if(g > 255){ g = 255; }   // just in case.. 
	  b = r = 0;
	}
	//cout << "and resulting rgb is " << r << "  " << g << "  " << b << endl;
	(*it).second.colors.push_back(QColor(r, g, b));
	(*it).second.v.push_back(v[i][j]);
	(*it).second.sum += (double)v[i][j];
      }
    }
  }
  // now it would seem that all of the things would have all of the data , but we don't know what's marked or not..
  for(int i=0; i < marks.size(); i++){
    it = pos.find(marks[i]);
    if(it != pos.end()){
      (*it).second.marked = true;
    }
  }
  // at which point it seems that we may be ready to do some drawing..
  // perhaps using the plot function or something... -- which will make the pixmap, 
  plot();
}

void TwoDPlot::plot(){
  // if we are here, all we have to do is make the pixmap.. then we update to call the thingy..
  pixmap->resize(size());
  pixmap->fill(QColor(0, 0, 0));    // hey, a black background..
  // and then we just need to go through the things..
  // work out the size of the square to use..
  double root = sqrt((double)probeNo);
  int rt = (int)root;
  if(root - (double)rt > 0){
    rt++;
  }
  QPainter* p = new QPainter(pixmap);
  if(drawBackground){
    // don't bother, this takes too much time to be worth it. If I can think of some quicker way to do it, then
    // maybe it will be useful.. 
    //plotBackground(p);
  }
  map<int, twoDPos>::iterator it;
  for(it = pos.begin(); it != pos.end(); it++){
    // lets just draw individualt pixels within a bounded rectangle.. 
    // the positions have already been calculated, so we don't have to do very much..
    if((*it).second.marked){
      p->setPen(QPen(QColor(255, 0, 0), 1));
    }else{
      p->setPen(QPen(QColor(255, 255, 255), 1));
    }
    p->setBrush(Qt::NoBrush);
    int m = 3;  // size multiplier.. 
    p->drawRect((*it).second.x, (*it).second.y, rt*m+2, rt*m+2);   // draw a rectangle in a square shape..
    vector<QColor>::iterator cit;
    int n = 0;
    cout << " plotting with colours size of vector is : " << (*it).second.colors.size() << endl;
    p->setPen(Qt::NoPen);
    for(cit = (*it).second.colors.begin(); cit != (*it).second.colors.end(); cit++){
      int xp = 1 + m*(n % rt);
      int yp = 1 + m*(n / rt);
      cout << "\t\tn : " << n << "  xp : " << xp << "  yp : " << yp << endl;
      n++;
      //p->setPen(QPen(*cit, 1));
      p->setBrush(*cit);
      p->drawRect((*it).second.x + xp, (*it).second.y + yp, m, m);
      //      p->drawPoint((*it).second.x + xp, (*it).second.y + yp);
    }
  }
  // and we should be done, all we have to do is bit blt it and delete the painter.. should be ok..
  delete p;
  repaint();
  //  update();   // this will bit blt the thingy..
}

void TwoDPlot::plotBackground(QPainter* p){
  // assume that painter is set to paint on the pixmap, and everything is sorted.. 
  cout << "Plot Background is here for you .. " << endl;
  bool simple = true;        // if simple, then just base the density somewhere on the thingy.
  // make sigma to be equal to the diagonal distance..
  double w = (double)width();
  double h = (double)height();
  double sigma = sqrt(w*w + h*h);
  //  sigma = 2 * sigma;
  // and we can make it some proportion of this later on..
  // declare a 2 d array of values to keep the intensities in..
  double** v = new double*[width()];     // maybe there should be a simpler way, but I don't know it.. 
  double max = 0;
  double min = 0;     // assume 0 value anyway...
  
  double a[2];
  double b[2];    // positions of the things. Let the kernel work out the distance using these..
  kernelFunction kf;
  // do lots of things.
  for(int i=0; i < width(); i ++){
    v[i] = new double[height()];
    for(int j=0; j < height(); j++){
      v[i][j] = 0;
      // then go through the things and work out the distance from them.. 
      map<int, twoDPos>::iterator it;
      for(it = pos.begin(); it != pos.end(); it++){
	double d = (double)( ((*it).second.x - i)*((*it).second.x - i) + (((*it).second.y - j)*((*it).second.y - j)) );
	double k = kf.radial(sigma, d);
	v[i][j] += (k * (*it).second.sum);       // assume a weight of one for everything..
      }
      if(v[i][j] > max) { max = v[i][j]; }
      if(v[i][j] < min) { min = v[i][j]; }
    }
  }
  // then go through all the values and paint pixels. Max value = full blue value... min value = 0.. 
  double m = 511;
  for(int i=0; i < width(); i++){
    for(int j=0; j < height(); j++){
      if(v[i][j] > 0){
	int l = (int) (m * (v[i][j] / max));
	int b, g;
	if(l > 255){
	  g = l - 255;
	  b = 255;
	}else{
	  g = 0;
	  b = l;
	}
  	p->setPen(QPen(QColor(0, g, b), 1));
	p->drawPoint(i, j);
      }
    }
  }
  // don't forget to delete values..
  delete []v;   // 
}

void TwoDPlot::paintEvent(QPaintEvent* e){
  // it would be better to only pain the bit that I need, but don't remember how to do that.
  bitBlt(this, 0, 0, pixmap, 0, 0, width(), height());
}

void TwoDPlot::resizeEvent(QResizeEvent* e){
  // adjust the positions, and call plot..
  adjustPositions();
  plot();
}

void TwoDPlot::adjustPositions(){
  // the minX, the maxY and all the others should be set when the coordinates are sorted out.. this
  // is one of the important things to keep in mind..
  map<int, twoDPos>::iterator it;
  int x, y;
  float xrange = maxX - minX;
  float yrange = maxY - minY;
  int margin = 20;
  int w = width() - margin*2;
  int h = height() - margin*2;
  for(it = pos.begin(); it != pos.end(); it++){
    x = margin + (int)( (float)w *  ( (*it).second.fx - minX)/xrange );
    y = margin + (int)( (float)h *  ( (*it).second.fy - minY)/yrange );
    //cout << "Setting the integral values x: " << x << " y: " << y << endl;
    (*it).second.x = x;
    (*it).second.y = y;
  }
  // let's call plot and update to make sure that we get a redraw..
}

// and the important one.. creating the thingy in the first place..
void TwoDPlot::setCoordinates(vector<PointCoordinate> pc){
  // first of all we'll need to empty the set that we've got..
  pos.erase(pos.begin(), pos.end());
  if(pc.size()){
    maxX = minX = pc[0].coords[0];
    maxY = minY = pc[0].coords[1];
  }
  for(int i=0; i < pc.size(); i++){
    // size of the coords has to be at least something or other..
    if(pc[i].dimNo < 2){
      cerr << "Point coordinates is too small, we are offically buggered,, i is : " << i << "  and size is : " << pc[i].dimNo << endl;
    }else{
      //cout << "Making pair with x : " << pc[i].coords[0] << "  y : " << pc[i].coords[1] << endl;
      pos.insert(make_pair(pc[i].index, twoDPos(pc[i].coords[0], pc[i].coords[1])));
      if(maxX < pc[i].coords[0]){ maxX = pc[i].coords[0]; }
      if(minX > pc[i].coords[0]){ minX = pc[i].coords[0]; }
      if(maxY < pc[i].coords[1]){ maxY = pc[i].coords[1]; }
      if(minY > pc[i].coords[1]){ minY = pc[i].coords[1]; }
    }
  }
  // and then reposition them, just in case..
  adjustPositions();
  // and in case we have some data lets replot..
  plot(values, exptIndex, markedOnes);
}

void TwoDPlot::toggleBackground(){
  drawBackground = !drawBackground;
  plot();
}
  
void TwoDPlot::mousePressEvent(QMouseEvent* e){
  if(e->button() == Qt::RightButton){
    menu->popup(mapToGlobal(e->pos()));
  }
}

void TwoDPlot::showEvent(QShowEvent* e){
  if(!e->spontaneous()){         // i.e. if it's an event from inside the application.. rather than window movement..
    plot(values, exptIndex, markedOnes);
  }
}
