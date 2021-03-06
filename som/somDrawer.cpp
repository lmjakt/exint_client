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

#include "somDrawer.h"
#include <qwidget.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <values.h>

using namespace std;

SomDrawer::SomDrawer(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  setBackgroundMode(NoBackground);
  smap.mapcd.resize(0);
  
  maxX = minX = maxY = minY = 1;
  maxDev = 2.5;               // default value.. 
  margin = 25;
  nodeDiameter = 6;   // won't look so good, but 6 * 100 = 600,, hmm,   
  pointDiameter = 18;
  distDrawer = new DistChooser("Distances", 255);
  distDrawer->resize(300, 200);
  distDrawer->show();
}

SomDrawer::~SomDrawer(){
  cout << "Destryong som drawer " << endl;
}

void SomDrawer::setData(mapDescription md){
  // the meat of the thing..
  // the md constructor takes care of checking for internal consistency.. -- just make sure not to 
  // alter the map... 
  // but do check that it has some values..
  //  cout << "somDrawer setting data : " << endl;
  if(!md.mapcd.size()){
    cerr << "map Description is empty size " << md.mapcd.size() << endl;
    return;
  }
  vector<float> eDistances;    // for all of the euclidean distances so I can set the distChooser.. hmm, 
  smap = md;
  fwd.resize(smap.mapcd.size());
  down.resize(smap.mapcd.size());
  // we now have to determine maxX, minX, and minY for the appropriate dimensions..
  minX = maxX = smap.mapcd[0][smap.mc1];
  minY = maxY = smap.mapcd[0][smap.mc2];
  int w = smap.mw;
  int h = smap.mh;
  int cp, fp, dp;    // positions.. 
  maxEuclidean = minEuclidean = euclidean(smap.mapcd[0], smap.mapcd[1]);
  float euclideanSum = 0;
  float euclideanSqSum = 0;   
  int euclidCount = 0;    // I'm bound to get it wrong if I do the calculation instead.. 
  //cout << "som drawer about to work out euclidean mins and max" << endl;
  //cout << "beginning of things, maxEuclid : " << maxEuclidean << "  min is " << minEuclidean << endl;
  for(int i=0; i < h-1; i++){
    for(int j=0; j < w-1; j++){
      cp = i*w + j;
      fp = cp + 1;
      dp = (i+1) * w + j;    // the positions that we want to take distances from..-- 
      //cout << "doing euclideans : i " << i << "  j: " << j << endl;
      fwd[cp] = euclidean(smap.mapcd[cp], smap.mapcd[fp]);
      down[cp] = euclidean(smap.mapcd[cp], smap.mapcd[dp]);
      euclideanSum += fwd[cp] + down[cp];
      euclideanSqSum += ((fwd[cp]*fwd[cp]) + (down[cp]*down[cp]));
      euclidCount += 2;
      eDistances.push_back(fwd[cp]);
      eDistances.push_back(down[cp]);
      if(fwd[cp] > maxEuclidean){ maxEuclidean = fwd[cp]; }
      if(down[cp] > maxEuclidean){ maxEuclidean = down[cp]; }
      if(fwd[cp] < minEuclidean){ minEuclidean = fwd[cp]; }
      if(down[cp] < minEuclidean){ minEuclidean = down[cp]; }
    }
    //cout << "done columns, , now need to check the last column" << endl;
    // the last position only has a down, set fwd to 0.
    cp = i*w + w-1;
    dp = (i+1) * w + w-1;
    fwd[cp] = 0;
    //cout << "cp is " << cp << "dp is : " << dp << endl
    // << "size of vector : " << smap.mapcd[cp].size() << "  and " << smap.mapcd[dp].size() << endl;
    down[cp] = euclidean(smap.mapcd[cp], smap.mapcd[dp]);
    eDistances.push_back(down[cp]);
    euclidCount++;
    euclideanSum += down[cp];
    euclideanSqSum += (down[cp] * down[cp]);
    if(down[cp] > maxEuclidean){ maxEuclidean = down[cp]; }
    if(down[cp] < minEuclidean){ minEuclidean = down[cp]; }
  }
  // and then do the forward arrows for the bottom row... arghh, talk about ugly code,, ehhhh!!!
  for(int j=0; j < w-1; j++){
    cp = (h-1)*w +j;
    fp = cp + 1;
    fwd[cp] = euclidean(smap.mapcd[cp], smap.mapcd[fp]);
    eDistances.push_back(fwd[cp]);
    down[cp] = 0;
    euclidCount++;
    euclideanSum += fwd[cp];
    euclideanSqSum += (fwd[cp] * fwd[cp]);
    if(fwd[cp] > maxEuclidean){ maxEuclidean = fwd[cp]; }
    if(fwd[cp] < minEuclidean){ minEuclidean = fwd[cp]; }
  }
  float SS = euclideanSqSum - ( (euclideanSum * euclideanSum)/euclidCount);
  euclidStd = sqrt(SS / (float)(euclidCount-1));
  euclidMean = euclideanSum / (float)euclidCount;  // ok..
  
  cout << "\t\tSS is " << SS << "   euclidStd is : " << euclidStd << endl;

  // draw the distribution..
  distDrawer->setData(eDistances);
  //  cout << "last row done: maxEuclidean : " << maxEuclidean << "  minEuclidean : " << minEuclidean << endl;
  //// maybe I don't use the below, so eventually I can comment out, but for now.. hmm. 
  for(int i=0; i < smap.mapcd.size(); i++){
    if(maxX < smap.mapcd[i][smap.mc1]){ maxX = smap.mapcd[i][smap.mc1]; }
    if(minX > smap.mapcd[i][smap.mc1]){ minX = smap.mapcd[i][smap.mc1]; }
    if(maxY < smap.mapcd[i][smap.mc2]){ maxY = smap.mapcd[i][smap.mc2]; }
    if(maxY < smap.mapcd[i][smap.mc2]){ minY = smap.mapcd[i][smap.mc2]; }
  }
  // and I think that is pretty much all I have to do.. now just go ahead and draw the thing..
  update();
}

void SomDrawer::paintEvent(QPaintEvent* e){
  //cout << "beginning of SomDrawer paintEvent " << endl;
  QPixmap pix(width(), height());
  pix.fill(QColor(0, 0, 0));
  

  if(smap.mapcd.size()){
    float xMult = ((float)(width() - 2 * margin))/(maxX - minX);
    float yMult = ((float)(height() - 2 * margin))/(maxY - minY);
    
    QPainter p(&pix);
    
    // draw a grid first of all
    // later decide what we'll use.
    QPen gridPen(QColor(255, 255, 255), 1);
    p.setPen(gridPen);
    int x1, x2, y1, y2;
    int mc1 = smap.mc1;
    int mc2 = smap.mc2;
    int w = smap.mw;       // for brevity.. 
    int h = smap.mh;
    int b;     // the blue component.. 
    float erange = maxEuclidean - minEuclidean;
    if(erange == 0){
      erange = maxEuclidean;
    }
    float logRange = log(erange);
    float logMin = log(minEuclidean);   // let's hope it's not 0.. 
    //  cout << "entering the loop, smap.mh is : " << smap.mh << endl;
    for(int i=0; i < (smap.mh -1); i++){   // the height,,
      // draw one vertical line from the first component to the second one.. 
      //     x1 = (int)((smap.mapcd[i * smap.mw][mc1]-minX) * xMult) + margin;
      //     x2 = (int)((smap.mapcd[(i+1) * smap.mw][mc1]-minX) * xMult) + margin;
      //     y1 = (int)((smap.mapcd[i * smap.mw][mc2]-minY) * yMult) + margin;
      //     y2 = (int)((smap.mapcd[(i+1) * smap.mw][mc2]-minY) * yMult) + margin;
      
      // draw without taking into account the real positions..
      x1 = x2 = margin;
      y1 = margin + (i * (height() - 2*margin))/(h-1);
      y2 = margin + ((i+1) * (height() - 2*margin))/(h-1);
      //b =   (int) 255 * (int)((log(down[i*w]) - logMin)/logRange);
      //b =   (int) 255 * (int)((down[i*w] - minEuclidean)/erange);
      //cout << "minEuclidean :" << minEuclidean << "  range : " << erange 
      //	 << "   down[" << i*w << "] " << down[i*w] << endl;
      //cout << "   and that makes for a blue colour with : " << b << endl
      //	 << "((down[i*w] - minEuclidean)/erange :"  << (down[i*w] - minEuclidean)/erange << endl;
      int r = 255 - b;
      p.setPen(color(down[i*w]));
      //    p.setPen(QPen(QColor(r, r, b), 1));
      p.drawLine(x1, y1, x1, y2);
      // the draw one back, and one down for the rest of the row..
      for(int j=1; j < smap.mw; j++){
	// first one line down..
	//     x1 = (int)((smap.mapcd[i * smap.mw + j][mc1]-minX) * xMult) + margin;
	//       x2 = (int)((smap.mapcd[(i+1) * smap.mw + j][mc1]-minX) * xMult) + margin;
	//       y1 = (int)((smap.mapcd[i * smap.mw + j][mc2]-minY) * yMult) + margin;
	//       y2 = (int)((smap.mapcd[(i+1) * smap.mw + j][mc2]-minY) * yMult) + margin;
	
	x1 = margin + (j * (width() - 2*margin))/(w-1);
	x2 = margin + ((j-1) * (width() - 2*margin))/(w-1);
	//y1 = margin + (i * (height() - 2*margin))/(h-1);
	//y2 = margin + ((i+1) * (height() - 2*margin))/(h-1);
	//b = (int) 255 * ((log(down[i*w + j]) - logMin)/logRange);
	//b = (int) 255 * ((down[i*w + j] - minEuclidean)/erange);
	//cout << "minEuclidean :" << minEuclidean << "  range : " << erange 
	// << "   down[" << i*w+j << "] " << down[i*w+j] << endl;
	//cout << "   and that makes for a blue colour with : " << b << endl
	//	   << "((down[i*w] - minEuclidean)/erange :"  << (down[i*w+j] - minEuclidean)/erange << endl;
	p.setPen(color(down[i*w+j]));
	//      p.setPen(QPen(QColor(255-b, 255-b, b), 1));      
	p.drawLine(x1, y1, x1, y2);
	
	//b = (int) 255 * ((fwd[i*w + j-1] - minEuclidean)/erange);
	//cout << "minEuclidean :" << minEuclidean << "  range : " << erange 
	// << "   fwd[" << i*w+j << "] " << fwd[i*w+j] << endl;
	//cout << "   and that makes for a blue colour with : " << b << endl
	//   << "((down[i*w] - minEuclidean)/erange :"  << (down[i*w+j] - minEuclidean)/erange << endl;
	p.setPen(color(fwd[i*w+j-1]));
	//      p.setPen(QPen(QColor(255-b, 255-b, b), 1));      
	p.drawLine(x1, y1, x2, y1);
	// and then draw one line back to the last one..
	//       x1 = (int)((smap.mapcd[i * smap.mw + (j-1)][mc1]-minX) * xMult) + margin;
	//       x2 = (int)((smap.mapcd[i * smap.mw + j][mc1]-minX) * xMult) + margin;
	//       y1 = (int)((smap.mapcd[i * smap.mw + (j-1)][mc2]-minY) * yMult) + margin;
	//       y2 = (int)((smap.mapcd[i * smap.mw + j][mc2]-minY) * yMult) + margin;
	//p.drawLine(x1, y1, x2, y2);
      }
    }
    // at this point we still have to draw the bottom line.. 
    y1 = height()-margin;    // shortcut..
    for(int j=0; j < (smap.mw-1); j++){
      //     int r = smap.mh-1;
      //     x1 = (int)((smap.mapcd[r * smap.mw + j][mc1]-minX) * xMult) + margin;
      //     x2 = (int)((smap.mapcd[r * smap.mw + (j+1)][mc1]-minX) * xMult) + margin;
      //     y1 = (int)((smap.mapcd[r * smap.mw + j][mc2]-minY) * yMult) + margin;
      //     y2 = (int)((smap.mapcd[r * smap.mw + (j+1)][mc2]-minY) * yMult) + margin;
      
      x1 = margin + (j * (width() - 2*margin))/(w-1);
      x2 = margin + ((j+1) * (width() - 2*margin))/(w-1);
      
      p.setPen(color(fwd[(h-1) * w + j]));
      //    int b = (int) 255 * ((fwd[(h-1) * w + j] - minEuclidean)/erange);
      //p.setPen(QPen(QColor(255-b, 255-b, b), 1));         
      p.drawLine(x1, y1, x2, y1);
    }
    //cout << "smap membership size is : " << smap.membership.size() << endl;
    for(int i=0; i < smap.membership.size(); i++){
      if(smap.membership[i] == -1){
	cout << "membership is equal to -1" << endl;
	continue;
      }
      int xp = smap.membership[i] % w;
      int yp = smap.membership[i] / w;
      int x = margin + (xp * (width() - 2 * margin))/(w-1) - pointDiameter/2;
      int y = margin + (yp * (height() - 2 * margin))/(h-1) - pointDiameter/2;
      p.setBrush(QColor(255, 0, 0));
      //cout << "trying to draw member at : " << x << "  and y : " << y << endl;
      p.drawEllipse(x, y, pointDiameter, pointDiameter);
      // and let's draw the number on top..
      QString num;
      p.setPen(QPen(QColor(255, 255, 255), 1));
      num.setNum(smap.index[i]);
      p.drawText(x, y, pointDiameter, pointDiameter, Qt::AlignCenter, num);
    }
  }
  // at which point the grid of the map will have been drawn in some sort of way.. 
  // later we can have a look at the mapping, it's not so difficult, but I'm curious how this will look first..
  bitBlt(this, 0, 0, &pix, 0, 0);
  // hoo hoo yeaahh.
}

float SomDrawer::euclidean(vector<float>& v1, vector<float>& v2){
  float d = 0;
  for(int i=0; i < v1.size(); i++){
    d += ((v1[i] - v2[i]) * (v1[i] - v2[i]));
  }
  //if(d == 0){
  //  cerr << "euclidean distance of 0, we don't really believe this" << endl;
  //  exit(1);
  //}
  //  return(sqrt(d));
  if(d > 0){
    return(log(sqrt(d)));
  }
  return(log(MINFLOAT));
  //  return(sqrt(sqrt(d)));  // to flatten the curve.. 
}

QPen SomDrawer::color(float d){
  // given a distance d return a suitable color component.. 
  
  // use mean and standard deviation..
  // -- with 0 -> 255 for the blue component between -3 -> +3 std deviations from the mean..
  //  float maxDev = 2.5;
  int b =   (int)(255 * ((maxDev + (d - euclidMean)/euclidStd)/(maxDev*2)));
  //cout << "euclid Mean is : " << euclidMean << "  and std is " << euclidStd << endl
  //     << "distance is : " << d << "  giving a colour b  of : " << b << endl;

  //cout << "color thingy b : " << b << endl;
  if(b > 255) { b = 255; }
  if(b < 0) { b = 0; }
  return(QPen(QColor(255-b, 0, b), 1));
}
  
void SomDrawer::setMaxDev(float f){
  maxDev = f;
}
