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

#include "clientWindow.h"
#include "qfont.h"
#include <qapplication.h>
#include <qstylefactory.h>
//#include <qcdestyle.h>
#include <qpalette.h>
#include <qcolor.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){
  /// seed the rand function
  time_t time_count;
  time(&time_count);
  srand((int)time_count);   // ?? 
  QApplication app(argc, argv);
  int ptsize = app.font().pointSize();
  cout << "Set pointsize to : " << ptsize << endl
       << "and will set family to : " << app.font().family() << endl;
  if(ptsize < 1){
    // no special font set, set to Arial, and size 9.. 
    app.setFont(QFont("Arial", 9));
  }else{
    app.setFont(QFont(app.font().family(), ptsize));
  }

//  app.setStyle(new QCDEStyle());
  app.setStyle(QStyleFactory::create("cde"));
  app.setPalette(QPalette(QColor(110, 197, 209), QColor(184, 175, 198)));
  ClientWindow messages;
  app.setMainWidget(&messages);
  messages.show();
  return app.exec();
}
