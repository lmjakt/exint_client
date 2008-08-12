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

#ifndef GLPLOTTER_H
#define GLPLOTTER_H

#include <qgl.h>
#include <vector>

using namespace std;

class GLPlotter: public QGLWidget
{
  Q_OBJECT

    public:
  GLPlotter(QWidget* parent, const char* name);
  ~GLPlotter();

  public slots:
  void setData(vector< vector<float> >& v, vector<int>& exIndex, vector<int>& marks);
  void setXRotation(int degrees);
  void setYRotation(int degrees);
  void setZRotation(int degress);   // activate later when its working.. 
  //void setScale(float x, float y, float z);
  void setxScale(int x);
  void setyScale(int y);
  void setzScale(int z);

  void xMove(int x);
  void yMove(int y);
  void zMove(int z);
  
  
 protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);
  void paintEvent(QPaintEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mousePressEvent(QMouseEvent* e);

 private:
  GLuint model;
  GLuint sphere;
  GLfloat xRot, yRot, zRot, scale;
  GLfloat xScale, yScale, zScale;
  GLfloat minV, maxV;
  vector<vector<float> > values;
  vector<int> experimentIndex;
  vector<int> markedExperiments;    
  GLfloat colorComponent;    // changes with mouse movement.. 
  GLfloat xOffset, yOffset, zOffset;
  int lastX, lastY;      // for use with the mouse movement
  int mouseButton;       // which mousebutton was pressed.. 
};

#endif
