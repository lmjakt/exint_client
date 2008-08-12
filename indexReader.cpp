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

#include "indexReader.h"
#include <qsocket.h>
#include <qobject.h>
#include <vector>
#include <iostream>

IndexReader::IndexReader(QObject* parent, const char* name) :
  QObject(parent, name)
{
  vectorResized = false;
  currentPosition = 0;
}

IndexReader::~IndexReader(){
  // ??
}

//bool IndexReader::startReading(QSocket* socket, vector<int>& externalIndex){
//  clear();
//  return(continueReading(socket, externalIndex)); // i.e. false..//
//}

bool IndexReader::read(QSocket* socket, vector<int>& externalIndex){
  char data[4];
  unsigned int bufSize = 4;
  int n;       // for general use
  if(!vectorResized){     // i.e. we haven't actually started reading..
    clear();
    if(socket->bytesAvailable() >= 4){
      socket->readBlock(data, bufSize);
      n = *(int*)data;
      index.resize(n);
      vectorResized = true;
    }else{
      cout << "\tIndex reader didn't even managed to resize the vector" << endl;
      cout << "\tcurrent position size: " << currentPosition << endl;
      cout << "\tcurrent vector size  : " << index.size() << endl;
      return(false);
    }
  }
  int start = currentPosition;
  for(int i=start; i < index.size(); i++){
    if(socket->bytesAvailable() < 4){
      cout << "read in a total of " << currentPosition << "  indices" << endl;
      cout << "                     bytes available: " << socket->bytesAvailable() << endl;
      return(false);
    }
    currentPosition = i+1;
    socket->readBlock(data, bufSize);
    index[i] = *(int*)data;
  }
  // if we got this far we should be looking for the endtoken..
  char endToken[16];
  bufSize = 16;
  socket->readBlock(endToken, bufSize);
  if(QString(endToken) == QString("<ProbeIndexEnd>")){
    // everything is fine..
    externalIndex = index;
    clear();
    return(true);
  }else{
    //hmm. This is quite serious.. emit some sort of signal maybe to reconnect
    cout << "BIG ERROR::: , endtoken is " << endToken << endl;
    clear();
    emit bigError();
    return(false);
  }
}

void IndexReader::clear(){
  index.resize(0);
  currentPosition = 0;
  vectorResized = false;
}
  
