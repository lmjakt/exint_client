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

#ifndef INDEXREADER_H
#define INDEXREADER_H

#include <vector>
#include <qsocket.h>
#include <qobject.h>

using namespace std;

class IndexReader : public QObject
{
  Q_OBJECT
    // a class that reads in a probe index from a reader..
    // very simple implementation, but requires the ability to remember where it 
    // it.. that's all really
    public:
  IndexReader(QObject* parent=0, const char* name=0);
  ~IndexReader();

  //  bool startReading(QSocket* socket, vector<int>& externalIndex);
  bool read(QSocket* socket, vector<int>& externalIndex);
  void clear();

 private:
  vector<int> index;
  int currentPosition;
  bool vectorResized;

 signals:
  void bigError();

};

#endif

