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

#include "smallToggle.h"
#include "../images.h"
#include <qimage.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qstring.h>
#include <stdlib.h>
//#include <iostream>
//#include <qt.h>

using namespace std;

// wrapper function for the images... -- why don't I just stick this in the header I don't know.. 
QImage getImage(const char* name){
  return qembed_findImage(name);
}

SmallToggle::SmallToggle(int n, QWidget* parent, const char* name) :
  QToolButton(parent, name)
{
  id = n;
  setToggleButton(true);
  //cout << "QT_VERSION : " << QT_VERSION << endl;

//   QString activePixMap("active_2_25.xpm");
//   QString inactivePixMap("inactive_25.xpm");

//   //#ifdef Q_OS_LINUX
//   char* path = getenv("CLIENT_DIR");
//   //  cout << "smallToglle cpp  path is : " << path << endl;
//   if(path){   // i.e. not null
//     activePixMap.prepend("/");
//     activePixMap.prepend(path);
//     inactivePixMap.prepend("/");
//     inactivePixMap.prepend(path);
//   }
  //#endif
    
  // QT_VERSION is defined in qglobal.h
  //  QT3 needs the threee following lines.. -so I suppose I should have some #ifdef here, but I'm not sure what.
// #if QT_VERSION > 300
//   QIconSet icon;
//   icon.setPixmap(QPixmap(getImage("active_2_25")), QIconSet::Automatic, QIconSet::Normal, QIconSet::On);
//   icon.setPixmap(QPixmap(getImage("inactive_25")), QIconSet::Automatic, QIconSet::Normal, QIconSet::Off);
//   //  icon.setPixmap(QPixmap(activePixMap), QIconSet::Automatic, QIconSet::Normal, QIconSet::On);
//   //icon.setPixmap(QPixmap(inactivePixMap), QIconSet::Automatic, QIconSet::Normal, QIconSet::Off);
//   setIconSet(icon);
// #endif

// #if QT_VERSION == 231  
//  // QT 2 likes the following. (I've got QT3 on the PC, and QT2 here, well I've been linking against it
//   setOnIconSet(QIconSet(QPixmap(getImage("active_2_25"))));
//   setOffIconSet(QIconSet(QPixmap(getImage("inactive_25"))));  
//   //  setOnIconSet(QIconSet(QPixmap(activePixMap)));
//   //setOffIconSet(QIconSet(QPixmap(inactivePixMap)));  
//   /// Try to remember !!
// #endif

  setFixedSize(20, 20);   // small!!
  connect(this, SIGNAL(clicked()), this, SLOT(emitId()) );
}

void SmallToggle::emitId(){
  emit idClicked(id);
  //  emit toggled(isOn());
}

void SmallToggle::setActivity(bool b){
  ToggleState active;
  if(b){
    active = QButton::On;
  }else{
    active = QButton::Off;
  }
  if(active != state()){
    blockSignals(true);
    setState(active);  // damn ugly,, 
    blockSignals(false);
  }
  // and if that calls the signal, then we are screwed..
}
