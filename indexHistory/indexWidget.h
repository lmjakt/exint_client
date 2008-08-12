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

#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "../dataStructs.h"
#include "../smallToggle/smallToggle.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>

class IndexWidget : public QWidget
{
  Q_OBJECT

    public :
    IndexWidget(QWidget* parent=0, const char* name=0);
  
  void setIndex(IndexSet is, int no);            // copy the set,, hmm, 
  void setIndex(IndexSet is, bool inc, bool exc, int no);   
  bool Include(){
    return(include->isOn());
  }
  bool Exclude(){
    return(exclude->isOn());
  }

  IndexSet indexSet;                  // the data that we might want to use -- so should have
  //// bad to put it here, but then I can easily call functions in the IndexSet.. -ok, so that
  //// is probably considered a big sin, but what can i do.. 
  private slots :
    void toggled(bool on);
  void loadIndex();

  private :
    QLabel* noLabel;    // just the number.. 
  QLabel* sizeLabel;
  QLineEdit* descriptionLabel;
  SmallToggle* include;
  SmallToggle* exclude;
  
  signals :
    void switchToggled();  
  void restoreIndex(vector<int>, int, QString);
};

#endif
  
