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

#include "statWidget.h"
#ifdef Q_WS_WIN
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <qgl.h>
#include <qpainter.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

point::point(){
  x = 0;
  y = 0;
  z = 0;
}

point::point(GLfloat xp, GLfloat yp, GLfloat zp){
  x = xp;
  y = yp;
  z = zp;
}

coordinates::coordinates(){
  linX.resize(0);
  logX.resize(0);
  linY.resize(0);
  logY.resize(0);
  logZ.resize(0);
  linZ.resize(0);
  //  linear.resize(0);
  //logpoints.resize(0);
  active = 0;  // it's a pointer !
}

coordinates::coordinates(vector<float>* x, vector<float>* y, vector<float>* z, vector<bool>* a){
  //  cout << "at the start of the coordinates constructor" << endl;
  //cout << "a->size is " << a->size();
  //cout << "x->size() is " << x->size();
  active = a;
  int sz;  // the size..
  // sized need to be the same, or nothing will happen
  if(x->size() == y->size() && y->size() == z->size() && z->size() == a->size()){  // ok, 
    sz = x->size();
  }else{
    sz = 0;
  }
  linX.resize(sz);
  logX.resize(sz);
  linY.resize(sz);
  logY.resize(sz);
  linZ.resize(sz);
  logZ.resize(sz);
  
  //    linear.resize(x->size());
  //  logpoints.resize(x->size());
  for(int i=0; i < sz; i++){
    // first the linear points, just a copy and cast. not the greatest, but there you go.
    linX[i] = (GLfloat)(*x)[i];
    linY[i] = (GLfloat)(*y)[i];
    linZ[i] = (GLfloat)(*z)[i];
    // and then lets use the conditional operator..
    logX[i] = linX[i] > 0 ? log(linX[i]) : 0;
    logY[i] = linY[i] > 0 ? log(linY[i]) : 0;
    logZ[i] = linZ[i] > 0 ? log(linZ[i]) : 0;
  }
  //     linear[i] = point((GLfloat)(*x)[i], (GLfloat)(*y)[i], (GLfloat)(*z)[i]);
  //     GLfloat xl, yl, zl;  // for log values..
  //     if((*x)[i] > 0){
  //       xl = (GLfloat)log((*x)[i]);
  //     }else{
  //       xl = 0;
  //     }
  //     if((*y)[i] > 0){
  //       yl = (GLfloat)log((*y)[i]);
  //     }else{
  //       yl = 0;
  //     }
  //     if((*z)[i] > 0){
  //       zl = (GLfloat)log((*z)[i]);
  //     }else{
  //       zl = 0;
  //     }
  //     logpoints[i] = point(xl, yl, zl);
  //   }
}

plotData::plotData(){     // empty constructor
  minX = maxX = minY = maxY = minZ = maxZ = 0;
  xLog = yLog = zLog = false;
  //xLog = yLog = zLog = true;
}

void getMinMax(vector<float>* v, float& min, float& max){
  if(v->size() == 0){ return; }
  min = max = (*v)[0];
  for(int i=1; i < v->size(); i++){
    if(min > (*v)[i]) { min = (*v)[i]; }
    if(max < (*v)[i]) { max = (*v)[i]; }
  }
}

void plotData::setData(vector<float>* xv, vector<float>* yv, vector<float>* zv, vector<bool>* a){
  //  x = xv;
  //y = yv;
  //z = zv;
  cout << "statWidget set data : " << endl;
  cout << "statWidget set data : xv " << xv->size() << " yv " << yv->size() << " zv " << zv->size() << "  a " << a->size() << endl;

  active = a;
  // and now find the mins and maxes for each one.. 
  getMinMax(xv, minX, maxX);
  getMinMax(yv, minY, maxY);
  getMinMax(zv, minZ, maxZ);
  cout << "min maxes have been set.. " << endl;
  // ok, it's great.. now initially, the thresholds equaly the
  // ok awful kludge, but if max = 0, we have some real problems.. so..
  if(maxX == 0) { maxX = 1; }
  if(maxY == 0) { maxY = 1; }
  if(maxZ == 0) { maxZ = 1; }   // for calculating ranges and things. to avoid divide by 0,, 
  cout << " and things have been truncated .. " << endl;
  // make a coordinates struct..
  dp = coordinates(xv, yv, zv, a);
  cout << "and dp has been made .. " << endl;
}
  

// and that's all for now, too many things to set 
// otherwise..

StatWidget::StatWidget(QWidget* parent, const char* name)
  : QGLWidget(parent, name)
{
  xRot = yRot = zRot = 0.0;
  xScale = yScale = zScale = 2.0;   // no good reason.. 
  //model = 5;
  xOffset = yOffset = -1; // maybe
  zOffset = -2;          // maybe
}

StatWidget::~StatWidget(){
  glDeleteLists( model, 1);
}

void StatWidget::initializeGL(){
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel( GL_FLAT );
}

void StatWidget::resizeGL(int w, int h){
  glViewport(0, 0, (GLint)w, (GLint)h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 0.5, 10.0);
  glMatrixMode( GL_MODELVIEW );
  // I can probably remove some of these things
  // if I want to actively move the camera..
}

void StatWidget::paintEvent(QPaintEvent* e){
  makeCurrent();
  resizeGL(width(), height());
  updateGL();
}

void StatWidget::mousePressEvent(QMouseEvent* e){
  lastX = e->x();
  lastY = e->y();
}

void StatWidget::mouseMoveEvent(QMouseEvent* e){
  switch(e->state()){
  case 1:
    // do something
    yRot += (GLfloat)(e->x() - lastX);
    xRot -= (GLfloat)(lastY - e->y());
    break;
  case 513:
    // left button plus control (maybe)
    zOffset += ( (GLfloat)(e->y() - lastY) )/( (GLfloat)(height()/20));
    break;
  case 4:
    // middle button.. 
    zOffset += ( (GLfloat)(e->y() - lastY) )/( (GLfloat)(height()/20));
    break;
  case 2:
    // the right button
    yOffset -= ( (GLfloat)(e->y() - lastY) )/( (GLfloat)(height()/5));
    xOffset += ( (GLfloat)(e->x() - lastX) )/( (GLfloat)(width()/5));
    break;
  default:
    // do nothing
    break;
  }
  // and update
  lastX = e->x();
  lastY = e->y();
  updateGL();
}

void StatWidget::paintGL(){
  glClear( GL_COLOR_BUFFER_BIT );
  glClear( GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glTranslatef(xOffset, yOffset, zOffset);
  glRotatef( xRot, 1.0, 0.0, 0.0 );
  glRotatef( yRot, 0.0, 1.0, 0.0 );
  glRotatef( zRot, 0.0, 0.0, 1.0 );

  glScalef(xScale, yScale, zScale );  // I probably don't need to control this

  //  cout << "calling model, xRot: " << xRot << "\tyRot " << yRot << endl;
  glCallList( model );
  glFlush();
}

void StatWidget::setModel(plotData& pd){
  //cout << "setModel .. " << endl;
  makeCurrent();
  //cout << "model list number is " << model << endl;
  glDeleteLists( model, 1);
  model = glGenLists( 1 );
  //cout << "and now it is : " << model << endl;
  glNewList( model, GL_COMPILE );
  //cout << "and called glNewList " << endl;
  
  glLineWidth( 2.0 );
  glPointSize( 3.0 );    // draw points, !!
  // we have one active color,, and one inactive color.. !
  // use red and green?? 
  //  bool active;         // is the state set to active color of inactive state.
  //  GLfloat x, y, z = 0.0;
  //cout << "getting mins and maxes.. " << endl;
  GLfloat w = (GLfloat)((pd.maxY-pd.minY)/25.0);
  float lmaxX = log(pd.maxX);// - log(pd.minX);
  float lmaxY = log(pd.maxY);// - log(pd.minY);
  float lmaxZ = log(pd.maxZ);// - log(pd.minZ);   // if we use logs.. then most of our values are negative in fact !!.  
  //cout << "and log values calculated log maxes and mins.. " << endl;
  //  float lmaxX = log(pd.maxX) - log(pd.minX);
  //float lmaxY = log(pd.maxY) - log(pd.minY);
  //float lmaxZ = log(pd.maxZ) - log(pd.minZ);   // if we use logs.. then most of our values are negative in fact !!.  
  // set the scaling elements..
  //cout << pd.xLog << "\t" << pd.yLog << "\t" << pd.zLog << endl;
  //cout << pd.maxX << "\t" << pd.maxY << "\t" << pd.maxZ << endl;
  //cout << lmaxX << "\t" << lmaxY << "\t" << lmaxZ << endl;
  xScale = (GLfloat) pd.xLog ? (lmaxX ? 4/lmaxX : 1) : 4/pd.maxX;
  yScale = (GLfloat) pd.yLog ? (lmaxY ? 4/lmaxY : 1) : 4/pd.maxY;
  zScale = (GLfloat) pd.zLog ? (lmaxZ ? 4/lmaxZ : 1) : 4/pd.maxZ;
  //cout << "scales set " << endl;
  // ok not so good, but we need them for the axis. ofcourse, we can replace the set below with smarter versions.. 
  GLfloat maxXp = (GLfloat) pd.xLog ? lmaxX : pd.maxX;
  GLfloat maxYp = (GLfloat) pd.yLog ? lmaxY : pd.maxY;
  GLfloat maxZp = (GLfloat) pd.zLog ? lmaxZ : pd.maxZ;  // the actual maximum points.. 
  // this is a bit messy, but we'll see how it goes.. 
  //cout << "set maxes, mins and scales, probably dead here.. " << endl;

//   if(pd.x->size() != pd.y->size() || pd.y->size() != pd.z->size() || pd.active->size() != pd.z->size()){  // should make this check unnecessary.. 
//     cout << "vectors are of different sizes, that sucks !! by bye " << endl;
//     return;
//   }
  // I know how to do this faster hahaha..
  // but then I would have to abandon the point structure, and stick 6 vectors into the coordinates struct,,
  // and then I could just assign a pointer for the appropriate x, y, and z vectors. The only thing that would have to be checked then is the 
  // active state... hmm, that should be much faster, cleaner, OKOKOK, so I should do that..
  //cout << "pd.dp.linX.size is " << pd.dp.linX.size() << endl;
  vector<GLfloat>* x = pd.xLog ? &pd.dp.logX : &pd.dp.linX;
  vector<GLfloat>* y = pd.yLog ? &pd.dp.logY : &pd.dp.linY;
  vector<GLfloat>* z = pd.zLog ? &pd.dp.logZ : &pd.dp.linZ;
  
  glBegin( GL_POINTS );
  //cout << "and x->size is " << x->size() << endl;
  for(int i=0; i < x->size(); i++){
    if((*pd.dp.active)[i]){
      glColor3f(0.8, 0.0, 0.2);
      //  active = false;
    }else{
      glColor3f(0.0, 1.0, 0.0);
      //active = true;
    }
    glVertex3f((*x)[i], (*y)[i], (*z)[i]);
  }
  glEnd();
  //cout << "made the points.. " << endl;
  glLineWidth(5.0);
  glColor3f(1.0, 1.0, 1.0);
  glBegin( GL_LINES );
  // y axis
  glVertex3f(0, -maxYp, 0);
  glVertex3f(0, maxYp, 0);
  //  glVertex3f(0, -1, 0);
  //glVertex3f(0, 1, 0);
  // x axis
  glColor3f(0, 0, 1.0);
  glVertex3f(-maxXp, 0, 0);
  glVertex3f(maxXp, 0, 0);
  //glVertex3f(-1, 0, 0);
  //  glVertex3f(1, 0, 0);
  // z axis 
  glColor3f(0.7, 0.7, 0);
  glVertex3f(0, 0, maxZp);
  glVertex3f(0, 0, -maxZp);
  //  glVertex3f(0, 0, 1);
  //glVertex3f(0, 0, -1);
  glEnd();
  glEndList();
  //cout << "model size is now  " << pd.x->size() << endl;
  //cout << "and calling update .. " << endl;
  // so all positive numbers are scaled to lie between 0 and 1. Not ideal but easy enough to change.. 
  updateGL();
}
    
void StatWidget::drawCube(GLfloat  x, GLfloat y, GLfloat z, GLfloat w){
  makeCurrent();  // just to make sure..
  // make six polygons.. maybe there's a better way, but.. ?? 
//    glBegin( GL_QUAD_STRIP );
//    glVertex3f(x+w, y-w, z-w);
//    glVertex3f(x+w, y+w, z-w);
//    glVertex3f(x-w, y-w, z-w);
//    glVertex3f(x-w, y+w, z-w);
//    glVertex3f(x-w, y-w, z+w);
//    glVertex3f(x-w, y+w, z+w);
//    glVertex3f(x+w, y-w, z+w);
//    glVertex3f(x+w, y+w, z+w);
//    glVertex3f(x+w, y-w, z-w);
//    glVertex3f(x+w, y+w, z-w);
//    glEnd();
  glBegin( GL_TRIANGLE_FAN );
  glVertex3f(x, y+w, z);
  glVertex3f(x+w, y-w, z-w);
  glVertex3f(x-w, y-w, z-w);
  glVertex3f(x, y-w, z+w);
  // draw the base of the pyramid.. 
  glEnd();
  glBegin( GL_TRIANGLES );
  glVertex3f(x+w, y-w, z-w);
  glVertex3f(x-w, y-w, z-w);
  glVertex3f(x, y-w, z+w);
  glEnd();
  
}

