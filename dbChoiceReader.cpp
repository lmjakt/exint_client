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

#include "dbChoiceReader.h"
#include "socketReadFunctions.h"
#include <qsocket.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

DBChoiceReader::DBChoiceReader(QObject* parent, const char* name) :
  QObject(parent, name)
{
  clear();
}

void DBChoiceReader::clear(){
  readState = 0;
  subState = 0;
  vectorPosition = 0;
}

bool DBChoiceReader::readChoices(QSocket* socket, vector<string>& choices){
  int n;
  string word;
  if(readState == 0){         // nothing read, 
    clear();                 // just in case
    if(!readInt(socket, n)) { return(false); }
    choices.resize(n);
    readState = 1;
  }
  if(readState == 1){        // choices has been resized..
    int start = vectorPosition;
    for(int i=start; i < choices.size(); i++){
      vectorPosition = i;
      if(subState == 0){
	if(!readInt(socket, n)) { return(false); }
	subState = 1;
      }
      if(subState == 1){
	if(!readString(socket, choices[i], n)) { return(false); }
	subState = 0;
      }
    }
    vectorPosition = 0;   // reset the vector counter !!
    readState = 2;
  }
  if(readState == 2){
    if(!readInt(socket, n)) { return(false); }
    readState = 3;
  }
  if(readState == 3){
    if(!readString(socket, word, n)){ return(false); }
    clear();
    if(word == string("<DBChoicesEnd>")){        // everything OK
      return(true);
    }else{
      emit bigError();
      return(false);
    }
  }
  // and we should never GET HERE...
  cout << "UNKNOWN READSTATE IN DBBHOICEREADER,, THAT REALLY SUCKS.." << endl;
}

  
    
