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

#ifndef STATWIDGET_H
#define STATWIDGET_H

#include <qgl.h>
#include <vector>
#include <string>


using namespace std;

struct point {
  point();  // default needed for a vector I think.. 
  point(GLfloat xp, GLfloat yp, GLfloat yz);
  GLfloat x;
  GLfloat y;
  GLfloat z;
};

struct coordinates {
  coordinates();
  coordinates(vector<float>* x, vector<float>* y, vector<float>* z, vector<bool>* a);
  vector<GLfloat> linX;
  vector<GLfloat> logX;
  vector<GLfloat> linY;
  vector<GLfloat> logY;
  vector<GLfloat> linZ;
  vector<GLfloat> logZ;   // it's uglier than using the point vectors, but ultimately I should be able to do much faster. 

  //  vector<point> linear;
  //vector<point> logpoints;
  vector<bool>* active;
};

struct plotData {
  plotData();        // empty constructor, maybe I need it..
  void setData(vector<float>* xv, vector<float>* yv, vector<float>* zv, vector<bool>* a);

  coordinates dp;   // data points.. 
  //  vector<float>* x;
  //vector<float>* y;
  //vector<float>* z;     // the actual values
  vector<bool>* active;  // whether the point is active or inactive.

  float minX;
  float maxX;
  float minY;
  float maxY;
  float minZ;
  float maxZ;          // the min's and maxes for different scales. so we can calculate once

  string xName;
  string yName;
  string zName;     // axis identifiers.. 

  bool xLog;
  bool yLog;
  bool zLog;        // log or linear axis.. ?
};

class StatWidget : public QGLWidget
{
  Q_OBJECT

    public :
    StatWidget(QWidget* parent=0, const char* name=0);
  ~StatWidget();

  public slots:
    void setModel(plotData& pd);
  
 protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);
  void paintEvent(QPaintEvent* e);
  void mouseMoveEvent(QMouseEvent* e);  // for rotation and so on..
  void mousePressEvent(QMouseEvent* e); // for setting lastX and lastY

 private:
  GLuint model;   // created from a plotData struct in the setModel thingy. 
  GLfloat xRot, yRot, zRot, xScale, yScale, zScale, xOffset, yOffset, zOffset;
  int lastX, lastY;
  void drawCube(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  //void drawSomeStats(); 

};

#endif
