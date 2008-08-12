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

#ifndef PROBEDATAREADER_H
#define PROBEDATAREADER_H

#include "probe_set/probe_set.h"
#include <qsocket.h>
#include <qobject.h>


class ProbeDataReader : public QObject
{
  Q_OBJECT
    // a class that reads a probe set from a socket.. 
    // uses the socketReadFunctions for safety and portability.. well sort of
    public:
  ProbeDataReader(QObject* parent=0, const char* name=0);
  
  bool readData(QSocket* socket, probe_data& pdata);   // returns true if complete read.. 
  void clear();

 private:
  int readState;           // where am I?
  int subState;            // within a sub thingy.. trouble that.. 
  int subSubState;         // .. 
  int vectorPosition;      // if I need it..
  int subPos;              // vector of vector position.. 
  int stringLength;        // if I managed to read only the length of a string.. 
  bool stringRead;

 signals:
  void bigError();        // case something goes wrong..
};

#endif  
