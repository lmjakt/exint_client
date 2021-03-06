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

#ifndef MEANEXPRESSIONPLOTTER_H
#define MEANEXPRESSIONPLOTTER_H

#include <qwidget.h>
#include <vector>
#include <map>
#include <qpoint.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qpixmap.h>
#include <qfont.h>
#include "../exptinfo.h" 

using namespace std;

class MeanExpressionPlotter: public QWidget
{
  Q_OBJECT
    public:
  MeanExpressionPlotter(QWidget* parent=0, const char *name=0);  // the constructor..
  void paintEvent(QPaintEvent *e);                 // so it updates..
  void paintLines(vector< vector<float> >& v, vector< vector<int> >& exIndex, map<float, exptInfo>* em);
  void clearAll();
  vector<QColor*> colourTable();           // a bit dangerous, as its a vector of pointers, use with care!!! 
  
 private:
  vector< vector<float> > values;                 // contains the values
  vector< vector<int> > exptIndex;                // the experimental indices, so I know how to label the X-axis. 
  map<float, exptInfo>* eMap;                     // argh, ugly, but there you go. 
  //map<int, int> allExperiments;
  vector<QColor*>  colours;                 // a vector of colours.. 
  float maxV;
  float minV;                                     // minimum and maximum values.
  bool hasData;                            // if it has data or not.. 
  QPopupMenu* menu;
  QPixmap* pixmap;

  void paintLines();                                   // called by redraw functions..
  void setMinMax(vector< vector<float> >& v);        // to set min and max values.. ok la.. 
  void setColours();                                  // just called at the beginning set colours. 
  void mousePressEvent(QMouseEvent* e);


  protected :
    int penWidth;
  

 signals:
  void plotterStatusMessage(QString);
  void selectedNewFont(QFont);

  public slots:
    void setPenWidth(int);

  private slots :
      void copyImage();
  void selectFont();

};

#endif
