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

#include "glPlotter.h"
#ifdef Q_OS_MACX
#include <gl.h>
#else
#include <GL/gl.h>
#endif
//#include <GL/glut.h>
#include <qgl.h>
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

GLPlotter::GLPlotter(QWidget* parent, const char* name)
  : QGLWidget(parent, name)
{
  xRot = yRot = zRot = 0.0;
  scale = 0.25;
  xScale = yScale = zScale = 0.25;
  model = 0;
  xOffset = yOffset = -1;
  zOffset = -1;
  // let's define a sphere model, hmm, if I can !!

}

GLPlotter::~GLPlotter(){
  glDeleteLists( model, 1);
}

void GLPlotter::paintGL(){
  cout << "paintGL " << endl;
  glClear( GL_COLOR_BUFFER_BIT );
  glClear( GL_DEPTH_BUFFER_BIT );
  cout << "cleared buffers " << endl;
  glMatrixMode( GL_PROJECTION );
  cout << "set the mode " << endl;
  if(!values.size()){
      glFlush();
      return;
  }
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 40.0);
  glTranslatef(xOffset, yOffset, zOffset);
  glMatrixMode( GL_MODELVIEW );
  
  glLoadIdentity();
  glTranslatef(1, 1, -4);
  glRotatef( xRot, 1.0, 0.0, 0.0 ); 
  glRotatef( yRot, 0.0, 1.0, 0.0 ); 
  glRotatef( zRot, 0.0, 0.0, 1.0 ); 
  
  glShadeModel(GL_FLAT);
  glLineWidth( 3.0 );
  glPointSize( 10.0 );
  glColor3f(1.0, 1.0, 1.0);

  maxV = values[0][0];
  minV = values[0][0];
  GLfloat green;
  GLfloat blue;

  for(int i=0; i < values.size()-1; i++){
    glColor3f(0.0, 1.0, 0.0);
    cout << "calling glBegin i : " << i << "\t(/" << values.size() - 2 << ")" << endl;
    glBegin( GL_QUAD_STRIP );
    green = (GLfloat)i/(GLfloat)values.size();
    for(int j=0; j < values[i].size()-1; j++){
      cout << "  " << j << "/" << values[i].size() - 2;
      blue = (GLfloat)(j%3 + j%6 + i%3)/(GLfloat)14;
      glColor3f(colorComponent, green, blue);

      GLfloat x = (GLfloat)j - (GLfloat)values[i].size()/(GLfloat)2;
      GLfloat z1 = (GLfloat)i - (GLfloat)values.size()/(GLfloat)2;
      GLfloat z2 = (GLfloat)(i+1) - (GLfloat)values.size()/(GLfloat)2;
      glVertex3f(x, (GLfloat)values[i][j], z1);
      glVertex3f(x, (GLfloat)values[i+1][j], z2);
      
      

      if((GLfloat)values[i][j] > maxV) { maxV = (GLfloat)values[i][j]; }
      if((GLfloat)values[i][j] < minV) { minV = (GLfloat)values[i][j]; }
      if((GLfloat)values[i+1][j] > maxV) { maxV = (GLfloat)values[i+1][j]; }
      if((GLfloat)values[i+1][j] < minV) { minV = (GLfloat)values[i+1][j]; }
    }
    cout << endl << "and let's call glEnd() " << endl;
    glEnd();
    cout << "called glEnd() " << i << endl;
  }

  cout << "maxV : " << maxV << "\t minV " << minV << endl;

  GLfloat pi = 3.141592654;
  GLfloat cosine, sine;
  GLfloat zSine;
  GLfloat m;
  glColor3f(0.8, 0.0, 0.0);
  glTranslatef(0.0, 1.0, 0.0);

  cout << "drawing something else,, " << endl;

  for(int i=0; i < 25; i++){
    cosine = cos(i * 2 * pi / 25.0);
    m = sin(i * 2 * pi / 25.0);
    //    sine = sin(i * 2 * pi / 25.0);
    glBegin(GL_POLYGON);
    for(int j=0; j < 25; j++){
      sine = m * sin(j * 2 * pi / 25.0);
      zSine = m * cos(j * 2 * pi / 25.0);
    glVertex3f(cosine, sine, zSine);
    }
    glEnd();
  }

  cout << "model returned " << endl;
  cout << "calling glFlush() " << endl;
  glFlush();
}

void GLPlotter::paintEvent(QPaintEvent* e){
  //QRect rect = e->rect();
  //  cout << "paint Event: x : " << rect.x() << "\ty : " << rect.y() 
  //     << "\twidth : " << rect.width() << "\theight : " << rect.height() << endl;
  makeCurrent();
  resizeGL(width(), height());
  //  resizeGL(e->rect().width(), e->rect().height());
  //paintGL();
  updateGL();
  // and what else can we do??
}

void GLPlotter::mouseMoveEvent(QMouseEvent* e){
  // I'm not sure exactly how this works, but it seems that e->state() returns 
  // how the button was clicked
  // left = 1, right = 2, mid = 4
  // shifth 256
  // control 512
  // and I'm not sure about others. Anyway, just take the sum of these and you end 
  // up with what happened.. 
  //cout << "Mouse Move Event x : " << e->x() << "\ty : " << e->y() << endl;
  switch(e->state()){
  case 1: 
    yRot += (e->x() - lastX);
    xRot += (e->y() - lastY);
    break;
  case 513:      // left + shift
    zRot += (e->y() - lastY);
    break;
  case 2:
    xOffset += ((GLfloat)(e->x() - lastX))/((GLfloat)(width()/2));
    yOffset += ((GLfloat)(lastY - e->y()))/((GLfloat)(height()/2));   // moves the camera, not the thing
    break;
  case 514:
    zOffset += ( (GLfloat)(e->y() - lastY) )/( (GLfloat)(height()/20));
  case 4:
    colorComponent =  (GLfloat)(e->y()) / (GLfloat)height();
    //cout << "color Component is now : " << colorComponent << endl;
    break;
  default :
    break;
  }
  lastX = e->x();
  lastY = e->y();
  updateGL();
  //cout << "button state is " << e->state() << endl;
}

void GLPlotter::mousePressEvent(QMouseEvent* e){
  switch(e->button()){
  case Qt::LeftButton :
    mouseButton = 1;
    break;
  default :
    mouseButton = 0;
  }
  lastX = e->x();
  lastY = e->y();
}

void GLPlotter::initializeGL(){
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel( GL_FLAT );
  colorComponent = 0.0;  // chang with mouse movement.. 
}

void GLPlotter::resizeGL(int w, int h){
  //cout << "Resizing GL window: w " << w << "\th: " << h << endl;
  glViewport(0, 0, (GLint)w, (GLint)h );
  //cout << "Set the viewport, " << endl;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  //cout << "loaded identity for GL_Projection" << endl;
  glFrustum(-1.0, 1.0, -1.0, 1.0, 2.0, 10.0);
  //cout << "created frustum ,, " << endl;
  glMatrixMode( GL_MODELVIEW );
  //cout << "entering glMatrix Mode Modelview " << endl;
  //  update();
  //cout << "called update(), but I'm not sure,, " << endl;
  //paintGL();
  //swapBuffers();
  //  glFlush();     // ??
  //updateGL();
  //xMove((int)(xOffset*5)+50);
  //xMove((int)(xOffset*5)+55);
}

void GLPlotter::setData(vector< vector<float> >& v, vector<int>& exIndex, vector<int>& marks){
  if(!v.size()){
    return;
  }
  if(!v[0].size()){
    return;
  }
  
  values = v;
  experimentIndex = exIndex;
  markedExperiments = marks;
  
//   makeCurrent();
//   cout << "made current " << endl;
//   glDeleteLists( model, 1);
//   cout << "deleted list " << endl;

//   model = glGenLists( 1 );        // ????
//   glNewList( model, GL_COMPILE );
  
//   cout << "called glNewList" << endl;

//   glShadeModel(GL_FLAT);
//   glLineWidth( 3.0 );
//   glPointSize( 10.0 );
//   glColor3f(1.0, 1.0, 1.0);

//   maxV = v[0][0];
//   minV = v[0][0];
//   GLfloat green;
//   GLfloat blue;

//   for(int i=0; i < v.size()-1; i++){
//     glColor3f(0.0, 1.0, 0.0);
//     glBegin( GL_QUAD_STRIP );
//     green = (GLfloat)i/(GLfloat)v.size();
//     for(int j=0; j < v[i].size()-1; j++){
//       blue = (GLfloat)(j%3 + j%6 + i%3)/(GLfloat)14;
//       glColor3f(colorComponent, green, blue);

//       GLfloat x = (GLfloat)j - (GLfloat)v[i].size()/(GLfloat)2;
//       GLfloat z1 = (GLfloat)i - (GLfloat)v.size()/(GLfloat)2;
//       GLfloat z2 = (GLfloat)(i+1) - (GLfloat)v.size()/(GLfloat)2;
//       glVertex3f(x, (GLfloat)v[i][j], z1);
//       glVertex3f(x, (GLfloat)v[i+1][j], z2);
      
//       if((GLfloat)v[i][j] > maxV) { maxV = (GLfloat)v[i][j]; }
//       if((GLfloat)v[i][j] < minV) { minV = (GLfloat)v[i][j]; }
//       if((GLfloat)v[i+1][j] > maxV) { maxV = (GLfloat)v[i+1][j]; }
//       if((GLfloat)v[i+1][j] < minV) { minV = (GLfloat)v[i+1][j]; }
//     }
//     glEnd();
//     cout << "called glEnd() " << i << endl;
//   }

//   cout << "maxV : " << maxV << "\t minV " << minV << endl;

//   GLfloat pi = 3.141592654;
//   GLfloat cosine, sine;
//   GLfloat zSine;
//   GLfloat m;
//   glColor3f(0.8, 0.0, 0.0);
//   glTranslatef(0.0, 1.0, 0.0);

//   cout << "drawing something else,, " << endl;

//   for(int i=0; i < 25; i++){
//     cosine = cos(i * 2 * pi / 25.0);
//     m = sin(i * 2 * pi / 25.0);
//     //    sine = sin(i * 2 * pi / 25.0);
//     glBegin(GL_POLYGON);
//     for(int j=0; j < 25; j++){
//       sine = m * sin(j * 2 * pi / 25.0);
//       zSine = m * cos(j * 2 * pi / 25.0);
//     glVertex3f(cosine, sine, zSine);
//     }
//     glEnd();
//   }
//   glEndList();
//   // and really that is all of the data.. 
//   // and now we should work out the scaling factors, but do that later on,, lets 
//   // just see what happens if we try to plot this first.. well see if it compiles first !!

  cout << "calling update gl " << endl;

  updateGL();
}

void GLPlotter::xMove(int x){
  // translate range 0-99 ,, to -1 to +1.. 
  xOffset = (GLfloat)(x-50)/(GLfloat)5;
  updateGL();
}

void GLPlotter::yMove(int y){
  // translate range 0-99 ,, to -1 to +1.. 
  yOffset = (GLfloat)(y-50)/(GLfloat)5;
  updateGL();
}

void GLPlotter::zMove(int z){
  // translate range 0-99 ,, to -1 to +1.. 
  zOffset = ((GLfloat)(z-50)/(GLfloat)5);
  updateGL();
}

void GLPlotter::setxScale(int x){
  xScale = (GLfloat)(x)/(GLfloat)500;
  updateGL();
}

void GLPlotter::setyScale(int y){
  yScale = (GLfloat)(y)/(GLfloat)250;
  updateGL();
}

void GLPlotter::setzScale(int z){
  zScale = (GLfloat)(z)/(GLfloat)500;
  updateGL();
}

void GLPlotter::setXRotation(int x){
  //  xRot = (GLfloat)x;
  updateGL();
}

void GLPlotter::setYRotation(int y){
  //yRot = (GLfloat)y;
  updateGL();
}

void GLPlotter::setZRotation(int z){
  zRot = (GLfloat)z;
  updateGL();
}
