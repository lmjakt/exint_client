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

#ifndef PROBESETREADER_H
#define PROBESETREADER_H

#include "probe_set/probe_set.h"
#include <qsocket.h>
#include <qobject.h>
#include <vector>

class ProbeSetReader : public QObject
{
  Q_OBJECT
    // a class that reads a probe set from a socket.
    // takes a socket as a read argument, and reads from 
    // this until complete..
    public:
  ProbeSetReader(QObject* parent=0, const char* name=0);    // needs no arguments..
  ~ProbeSetReader();   // I probably don't need this as I don't call new anywhere
  
  //bool startReading(QSocket* socket, probe_set& pset);      // reads from socket and tries to set pset to a reasonable probe set
  bool read(QSocket* socket, probe_set& pset);   // return true if it's ok, and false otherwise..
  void clear();                                             // clear internal dataStructures (called by startReading as well)

 private:
  int index;                    // the number of the probe set
  vector<int> exptIndex;
  vector< vector<float> > dataValues;  // the actual data values
  
  int readState;                 // how far I've got..
  int currentVectorPosition;     // how far I've gotten in the present vector, 
  bool vectorResized;            // current vector resized,, i.e. have I got the size of it.. (only for 5)
  //int currentVectorLength;       // and how far I've got to read in the current Vector..
  
};

#endif
