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

#ifndef DBQUERIER_H
#define DBQUERIER_H

#include <qwidget.h>
#include <vector>
#include <string>         // cause I like the string class.. 
#include <qstring.h>
#include <qcombobox.h>
#include <qlineedit.h>

using namespace std;

class DBQuerier : public QWidget
{
  Q_OBJECT

    public:
  DBQuerier(QWidget* parent=0, const char* name=0);
  QLineEdit* inLine;  // so I can set focus to it.. bad practice, but quick and dirty. 
  
  public slots:
    void setChoices(vector<string> choices);
  void setRegionChoices(vector<string> choices);
  
 signals:
  void doLookup(QString);
  void doGenLookup(QString);
  
  private slots:
    void doQuery();
  void doGen();
  
 private:
  QComboBox* menu;            // select table for probe set lookup
  QComboBox* regionMenu;      // select table for genomic region lookup.. 
};

#endif
  
    
