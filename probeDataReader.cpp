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

#include "probeDataReader.h"
#include "socketReadFunctions.h"
#include "probe_set/probe_set.h"
#include <vector>
#include <string>
#include <qsocket.h>
#include <iostream>
#include <qobject.h>

using namespace std;

ProbeDataReader::ProbeDataReader(QObject* parent, const char* name) :
  QObject(parent, name)
{
  clear();
}

void ProbeDataReader::clear(){
  readState = 0;
  vectorPosition = 0;
  subPos = 0;
  stringLength = 0;
  subState = 0;
  subSubState = 0;
  stringRead = false;
}  

bool ProbeDataReader::readData(QSocket* socket, probe_data& pdata){
  // just assign the values to pdata
  // if readstate = 0, then I should probably call a clear function to remove
  // the current content of the pdata.. hmm..
  // but I will leave it for now to see if it's really necessary. Just try to remember
  // that this is one potential place for trouble..
  if(readState == 0){       // the beginning
    cout << "data reader state: " << readState << endl;
    if(!readInt(socket, pdata.index)) { return(false); }
    cout << "data index is " << pdata.index << endl;
    readState = 1;
  }
  if(readState == 1){
    cout << "data reader state: " << readState << endl;
    cout << "before assigning length " << stringLength << endl;
    if(!readInt(socket, stringLength)) { return(false); }
    cout << "string Length " << stringLength << endl;
    readState = 2;
  }
  if(readState == 2){
    cout << "data reader state: " << readState << endl;
    if(!readString(socket, pdata.gbid, stringLength)){ return(false); }
    readState = 3;
  }
  if(readState == 3){
    cout << "data reader state: " << readState << endl;
    if(!readInt(socket, stringLength)){ return(false); }
    readState = 4;
  }
  if(readState == 4){
    cout << "data reader state: " << readState << endl;
    if(!readString(socket, pdata.afid, stringLength)){ return(false); }
    readState = 5;
  }
  if(readState == 5){
    cout << "data reader state: " << readState << endl;
    int size;
    if(!readInt(socket, size)) { return(false); }
    pdata.ugData.resize(size);
    readState = 6;
    cout << "resized ugData to " << size << endl; 
  }
  if(readState == 6){     // painful, now we have to think deeply about it.. maybe we need a subState.. 
    cout << "data reader state: " << readState << endl;
    int start = vectorPosition;
    for(int i=start; i < pdata.ugData.size(); i++){
      vectorPosition = i;
      if(subState == 0){
	if(!readInt(socket, pdata.ugData[i].index)){ return(false); }
	subState = 1;
      }
      if(subState == 1){
	if(!readInt(socket, stringLength)){ return(false); }
	subState = 2;
      }
      if(subState == 2){
	if(!readString(socket, pdata.ugData[i].title, stringLength)) { return(false); }
	subState = 3;
      }
      if(subState == 3){
	if(!readInt(socket, stringLength)) { return(false); }
	subState = 4;
      }
      if(subState == 4){
	if(!readString(socket, pdata.ugData[i].gene, stringLength)){ return(false); }
      }
      // if we got to here, then we finished one entry, so substate = 0;
      subState = 0;
    }
    // and if we got to here, then we are done.
    vectorPosition = 0;
    readState = 7;     // go on to read the celeraMatches.. (argh..).
  }
  if(readState == 7){
    cout << "data reader state: " << readState << endl;
    int size;
    if(!readInt(socket, size)){ return(false); }
    pdata.celeraMatches.resize(size);
    readState = 8;
  }
  if(readState == 8){
    cout << "data reader state: " << readState << endl;
    int start = vectorPosition;
    for(int i=start; i < pdata.celeraMatches.size(); i++){
      vectorPosition = i;
      if(subState == 0){
	if(!readInt(socket, stringLength)) { return(false); }
	subState = 1;
      }
      if(subState == 1){
	if(!readString(socket, pdata.celeraMatches[i].celeraGene, stringLength)) { return(false); }
	subState = 2;
      }
      if(subState == 2){
	if(!readFloat(socket, pdata.celeraMatches[i].expectation)) { return(false); }
	subState = 3;
      }
      if(subState == 3){
	if(!readFloat(socket, pdata.celeraMatches[i].match)) { return(false); }
	subState = 4;
      }
      if(subState == 4){
	if(!readInt(socket, stringLength)){ return(false); }
	subState = 5;
      }
      if(subState == 5){
	if(!readString(socket, pdata.celeraMatches[i].sf, stringLength)){ return(false); }
	subState = 6;
      }
      if(subState == 6){
	if(!readInt(socket, stringLength)) { return(false); }
	subState = 7;
      }
      if(subState == 7){
	if(!readString(socket, pdata.celeraMatches[i].fn, stringLength)) { return(false); }
	subState = 8;
      }
      if(subState == 8){
	if(!readInt(socket, stringLength)) { return(false); }
	subState = 9;
      }
      if(subState == 9){
	if(!readString(socket, pdata.celeraMatches[i].gs, stringLength)) { return(false); }
	subState = 10;
      }
      if(subState == 10){
	if(!readInt(socket, stringLength)) { return(false); }
	subState = 11;
      }
      if(subState == 11){
	if(!readString(socket, pdata.celeraMatches[i].nd, stringLength)) { return(false); }
	// thank god we are donee..
      }
      subState = 0;
    }
    // and if we get here, we've done the whole thing!!
    vectorPosition = 0;
    readState = 9;
  }
  if(readState == 9){
    cout << "data reader state: " << readState << endl;
    if(!readInt(socket, stringLength)){ return(false); }
    readState = 10;
  }
  if(readState == 10){
    cout << "data reader state: " << readState << endl;
    if(!readString(socket, pdata.afdes, stringLength)) { return(false); }
    readState = 11;
  }
  if(readState == 11){
    cout << "data reader state: " << readState << endl;
    if(!readInt(socket, stringLength)) { return(false); }
    readState = 12;
  }
  if(readState == 12){
    cout << "data reader state: " << readState << endl;
    if(!readString(socket, pdata.tigrDescription, stringLength)){ return(false); }
    readState = 13;
  }
  if(readState == 13){
    cout << "data reader state: " << readState << endl;
    int size;
    if(!readInt(socket, size)) { return(false); }
    pdata.go.resize(size);
    readState = 14;
  }
  if(readState == 14){
    cout << "data reader state: " << readState << endl;
    int start = vectorPosition;
    for(int i=start; i < pdata.go.size(); i++){
      vectorPosition = i;
      if(subState == 0){
	int size;
	if(!readInt(socket, size)) { return(false); }
	pdata.go[i].resize(size);
	cout << "\tpdata.go[" << i << "] resized to : " << size << endl;
	subState = 1;
      }
      if(subState == 1){
	int inStart = subPos;
	cout << "subPos is: " << subPos << endl;
	cout << "pdata size is: " << pdata.go[i].size() << endl;
	cout << "subsubstate is: " << subSubState << endl;
	for(int j=inStart; j < pdata.go[i].size(); j++){
	  subPos = j;
	  if(subSubState == 0){
	    if(!readInt(socket, stringLength)){ return(false); }
	    cout << "\t\tstringLength for pdata.go[" << i << "][" << j << "] : " << stringLength << endl;
	    subSubState = 1;
	  }
	  if(subSubState == 1){
	    if(!readString(socket, pdata.go[i][j], stringLength)) { return(false); }
	    subSubState = 0; // reset!!
	  }
	}
	subPos = 0; 
	subState = 0; 
      }
    }
    vectorPosition = 0;
    readState = 15;
  }
  if(readState == 15){
    cout << "data reader state: " << readState << endl;
    // alleluliah, we just have to read in the last word and see if it fits..
    //
    if(!readInt(socket, stringLength)) { return(false); }   // as it was written using the function..
    readState = 16;
  }
  if(readState == 16){
    string endToken;
    if(!readString(socket, endToken, stringLength)) { return(false); }
    clear();        // either way we want to clear ourselves at this stage.. 
    if(endToken == string("ProbeDataEnd")){
      return(true);
    }else{
      emit bigError();
      return(false);
    }
  }
}
