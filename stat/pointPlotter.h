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

#ifndef POINTPLOTTER_H
#define POINTPLOTTER_H

#include <qwidget.h>
#include <qpixmap.h>
#include <vector>

class PointPlotter : public QWidget
{
  Q_OBJECT
    public :
    PointPlotter(QWidget* parent=0, const char* name=0);
  
  public slots :
    void plotPoints(std::vector<float> v, bool use_zero=false);   // plot points, use zero as min value even if no negative values.
  void plotPoints();
  void setProbeThreshold(float t){   // bad name,, but .. 
    threshold = t;
  }
  
  private :
    void paintEvent(QPaintEvent* e);                              // do something..
  void resizeEvent(QResizeEvent* e);                               // and something else.. 
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);

  std::vector<float> values;
  bool useZero;
  float min, max;
  QPixmap* pixmap;
  // things for setting and changing thresholds..
  float threshold;
  bool drawThreshold;
  bool thresholdChanging;
  int lastY, thisY;  // hmm. clumsy, but we may need this for repainting.

  signals :
    void setThreshold(float);

};

#endif
