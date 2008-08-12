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

#include "probeSetReader.h"
#include "probe_set/probe_set.h"
#include <vector>
#include <iostream>
#include <qsocket.h>


ProbeSetReader::ProbeSetReader(QObject* parent, const char* name) :
  QObject(parent, name)
{
  readState = 0;
  currentVectorPosition = 0;
  index =0;
  exptIndex.resize(0);
  dataValues.resize(0);
  vectorResized = false;
  // don;t really need to do anything,, 
}

ProbeSetReader::~ProbeSetReader(){
  // and I don't really need to do anything here either..
  // as things get set up in other places..
}

//bool ProbeSetReader::startReading(QSocket* socket, probe_set& pset){
//  clear();
//  bool done = continueReading(socket, pset);
//  return(done);
//}

bool ProbeSetReader::read(QSocket* socket, probe_set& pset){
  int bytesAvailable = socket->bytesAvailable();
  // keep a running track of the number of bytes Available, it might get larger,
  // but it shouldn't shrink.. My feeling is that it will stay the same as this 
  // function should execute very fast..
  char data[4];      // for reading four bytes at a time.
  unsigned int bufSize = 4;  // function pukes otherwise
  int n;                     // general purpose..
  bool noMore = false;
  

  if(readState == 0){
    clear();                            // at the beginning so I should make sure it's cleared. 
    if(socket->bytesAvailable() >= 4){
      socket->readBlock(data, bufSize);
      index = *(int*)data;   
      readState = 1;
      //      cout << "got the index: " << index << endl;
    }
  }
  if(readState == 1){
    //cout << "case 1: readState: " << readState << endl;
    if(socket->bytesAvailable() >= 4){
      socket->readBlock(data, bufSize);
      n = *(int*)data;
      exptIndex.resize(n);
      currentVectorPosition = 0;
      readState = 2;
      //cout << "resized the exptIndex to : " << n << endl;
    }
  }
  if(readState == 2){
    //cout << "case 2: readState: " << readState << endl;
    if(socket->bytesAvailable() >= (exptIndex.size() * 4)){
      for(int i=0; i < exptIndex.size(); i++){
	socket->readBlock(data, bufSize);
	exptIndex[i] = *(int*)data;
      }
      readState = 3;
    }
  }
  if(readState == 3){
    //cout << "case 3: readState: " << readState << endl;
    if(socket->bytesAvailable() >= 4){
      socket->readBlock(data, bufSize);
      n = *(int*)data;
      // n is the number of probes for the given probe Set, shouldn't be that large..
      // we should probably put in checks in places to make sure everything works..
      dataValues.resize(n);     // which could get stupid if we get the wrong info!!
      readState = 4; 
    }
  }
  if(readState == 4){
    //cout << "case 4: readState: " << readState << endl;
    noMore = false;
    for(int i=currentVectorPosition; i < dataValues.size(); i++){
      if(!vectorResized){
	if(socket->bytesAvailable() > 4){
	  // read in the size, and make sure it is not larger than exptIndex.size();
	  socket->readBlock(data, bufSize);
	  n = *(int*)data;
	  if(n > exptIndex.size()){
	    cout << "internal vector larger than the right size.. PUKE PUKE PUKE PUKE.." << endl;
	    readState = -1;  // big error, puke puke..
	    break;
	  }
	  dataValues[i].resize(n);
	  vectorResized = true;
	}
      }
      if(socket->bytesAvailable() >= (dataValues[i].size()*4)){
	for(int j=0; j < dataValues[i].size(); j++){
	  socket->readBlock(data, bufSize);
	  dataValues[i][j] = *(float*)data;
	}
      }else{
	noMore = true;
      }
      if(noMore || readState == -1){
	// we've run out of things to read in. break out of loop with empty..
	currentVectorPosition = i;
	break;
      }
      vectorResized = false; 
    }
    if(!noMore){         // whoa, we got all of the values..
      readState = 5;
    }
  }
  if(readState == 5){
    //cout << "case 5: readState: " << readState << endl;
    char endToken[14];
    bufSize = 14;
    if(socket->bytesAvailable() >= bufSize){
      socket->readBlock(endToken, bufSize);
      //cout << "endtoken is " << endToken << endl;
      if(QString(endToken) == QString("<ProbeSetEnd>")){
	readState = 6;
      }
    }
  }
  // and now if the readstate is good, then make and return..
  if(readState == 6){
    pset = probe_set(index, dataValues, exptIndex);
    clear();
    return(true);
  }else{
    return(false);
  }
}

void ProbeSetReader::clear(){
  index = 0;
  exptIndex.resize(0);
  dataValues.resize(0);
  readState = 0;
  currentVectorPosition = 0;
  vectorResized = false;
}

      
	  
    
