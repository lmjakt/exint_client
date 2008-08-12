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

#ifndef COMMENTADDER_H
#define COMMENTADDER_H

#include <qwidget.h>
#include <qtextview.h>
#include <qstring.h>
#include <qcombobox.h>
#include "../dataStructs.h"
#include <vector>
#include <map>

class CommentAdder : public QWidget
{
  Q_OBJECT
    public :
    CommentAdder(std::vector<ishAnnotationField> flds, QWidget* parent=0, const char* name=0);

  int currentIndex();    // return the thingy selected..
  QString comment();
  
  public slots :
    void setIshAnnotationFields(std::vector<ishAnnotationField> flds);
  void clear();
  
  private :
    std::map<int, ishAnnotationField> fieldIndex;      // the int is the index in the qcombobox.. 
  std::vector<ishAnnotationField> fields;
  QTextView* commentArea;
  QComboBox* fieldChooser;

  signals :
    void makeField(QString);
};

#endif
