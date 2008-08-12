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

#include "exptInfoReader.h"
#include "exptinfo.h"      // just the exptInfo struct.. needed in lots of places. so.. 
#include "socketReadFunctions.h"
#include <qsocket.h>
#include <qobject.h>
#include <map>
#include <iostream>

using namespace std;

ExptInfoReader::ExptInfoReader(QObject* parent, const char* name) :
  QObject(parent, name)
{
  // not much to do but clear..
  clear();
}

void ExptInfoReader::clear(){
  readState = 0;
  bytesNeeded = 0;
}

bool ExptInfoReader::readInfo(QSocket* socket, map<float, exptInfo>& info){
  // really should clear the map, but I don't have any methods for doing so, but what the hell
  // uses my new method utilising the QByteArray essentially for counting.. so. should just need to
  // read the first number in, and make sure that we have enough space for the things..
  if(readState == 0){        // we don't know how many bytes we need!!
    if(!readInt(socket, bytesNeeded)) { return(false); }
    readState = 1;
  }
  if(socket->bytesAvailable() < bytesNeeded){
    cout << "ExptInfoReader:  bytes Available: " << socket->bytesAvailable() << "\tBytes Needed: " << bytesNeeded << endl;
    return(false);
  }
  // so we have enough bytes, we can go right ahead and read everything without worrying about readStates or whatever.
  // May have to change this if packets get too large, but there you go..
  // that sounds like it might be related to operating systems..
  // first we better clear the memory used by the info..
  map<float, exptInfo>::iterator it;
  for(it=info.begin(); it != info.end(); it++){
    info.erase(it);
  }

  int size;
  readInt(socket, size);       // the number of mape entries don't bother checking it should work//
  for(int i=0; i < size; i++){
    float currentKey;
    if(!readFloat(socket, currentKey)){
      cout << "WHAT IS GOING ON??? I DON'T UNDERSTAND" << endl;
    }
    exptInfo* currentInfo = new exptInfo();
    readInt(socket, currentInfo->dbaseIndex);
    readInt(socket, currentInfo->exptGroup);
    readFloat(socket, currentInfo->index);
    readString(socket, currentInfo->shortName);
    readString(socket, currentInfo->description);
    int chipSize;
    readInt(socket, chipSize);
    for(int j=0; j < chipSize; j++){
      int chip;
      readInt(socket, chip);
      readBool(socket, currentInfo->chips[chip]);
      //      cout << "current expt: " << currentKey << "\tchip: " << chip << "\ton: " << currentInfo->chips[chip] << endl;
    }
    info[currentKey] = *currentInfo;    // copy function??
    delete currentInfo;
  }
  string endToken;
  readString(socket, endToken);
  clear();
  if(endToken == string("<ExptInfoEnd>")){    // wonderful
    return(true);
  }
  emit bigError();
  cout << "exptInfoReader : wrong endToken: token is: " << endToken << endl;
  return(false);
}

      
  

    
      
	
		      
  
