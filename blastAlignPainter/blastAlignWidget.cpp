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

#include "blastalignpainter.h"
#include "blastAlignWidget.h"
#include "../probe_set/probe_set.h"
#include <qwidget.h>
#include <qpainter.h>
#include <qcolor.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

using namespace std;

BlastAlignWidget::BlastAlignWidget(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  // actually in the constructor we don't really need to do anything.. 
  //matches.resize(0);
  myParent = parent;
  painter = new BlastAlignPainter();
  setPaletteBackgroundColor(QColor(0, 0, 0));
  hasData = false;
  cout << "at the end of the constructor.. for the blast align widget.. " << endl;
  // just make sure that the alignments are resized.. 
}

void BlastAlignWidget::setData(probe_data* d){
  data = d;
  hasData = true;
  update();
}

void BlastAlignWidget::paintEvent(QPaintEvent* e){
  //  resize(myParent->width(), 600);
  resize(myParent->size());
  cout << "\tWIDTH : " << width() << endl;
  cout << "\tHEIGHT: " << height() << endl; 
  //  if(data->ensemblMatches.size() == 0){
  if(!hasData){
    return;
  }
  QPainter* p = new QPainter(this);
  // but first lets paint some annotation on the widget..
  double rowHeight = 20;
  int indent = 150;
  int textMargin = 50;

  p->translate(0, rowHeight);
  p->setPen(QColor(255, 255, 255));
  p->setFont(QFont("Arial", 9));
  // and lets write some informatin about the probe set.. 
  QString text;
  text = "Affymetrix Id";
  p->drawText(textMargin, 0, text);
  text = data->afid.c_str();
  p->drawText(textMargin+indent, 0, text);
  p->translate(0, rowHeight);
  if(data->ensemblMatches.size() > 0){
    text = "Probe Set Length";
    p->drawText(textMargin, 0, text);
    text.setNum(data->ensemblMatches[0].af_length);
    p->drawText(textMargin+indent, 0, text);
  }
  p->translate(0, rowHeight);
  for(int i=0; i < data->ensemblMatches.size(); i++){
    p->translate(0, rowHeight);
    multimap<string, string>::iterator it;
    p->setPen(QColor(255, 255, 255));
    p->setFont(QFont("Arial", 9));
    for(it = data->ensemblMatches[i].description.begin(); it != data->ensemblMatches[i].description.end(); it++){
      text = (*it).first.c_str();
      p->drawText(textMargin, 0, text);
      text = (*it).second.c_str();
      p->drawText(textMargin + indent, 0, text);
      p->translate(0, rowHeight);
    }

    //  QString text("Hello there, how do you like this");
    //QString text(matches[0].matchId.c_str());
    text = "Ensembl Id";
    p->drawText(textMargin, 0, text);
    text = data->ensemblMatches[i].matchId.c_str();
    p->drawText(textMargin+indent, 0, text);
  
    // just paint on itself.. what can you do..
    painter->paintAlignments(&data->ensemblMatches[i].alignments, p, 10, 10, width(), height(), data->ensemblMatches[i].af_length, data->ensemblMatches[i].length);
    p->translate(0, 20);
  }
  p->end();
  delete p;
}

//  
