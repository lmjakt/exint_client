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

#ifndef ISHANNOTATIONERRORMESSAGE_H
#define ISHANNOTATIONERRORMESSAGE_H

#include "../dataStructs.h"
#include <qdialog.h>
#include <qstring.h>

class IshAnnotationErrorMessage : public QDialog
{

  Q_OBJECT

    public :
    
    IshAnnotationErrorMessage(ishTextInsert ti, QWidget* parent=0, const char* name=0);
  IshAnnotationErrorMessage(ishFloatInsert ti, bool classification, QWidget* parent=0, const char* name=0);
  // two constructors, which can be used with different types of error. Make a new one each time and delete 
  // after exec() returns;
  // try again is set to 
  private slots :
    void giveUp();   // calls done(0);
  void tryAgain();   // calls done(1)
};

#endif
