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

#include "socketReadFunctions.h"
#include <qsocket.h>
#include <string>
#include <iostream>

using namespace std;

int size = 4;        // the size of an int or float..


bool readFloat(QSocket* s, float& f){
  if(s->bytesAvailable() < size){
    return(false);
  }
  char* data = new char[4];
  s->readBlock(data, size);
  f = *(float*)data;
  delete []data;
  //cout << "\t\tf: " << f << endl;
  return(true);
}

bool readInt(QSocket* s, int& i){
  if(s->bytesAvailable() < size) {return(false); }
  char* data = new char[4];
  s->readBlock(data, size);
  i = *(int*)data;
  delete []data;
  //  cout << "\t\ti: " << i << endl;
  return(true);
}

bool readBool(QSocket* s, bool& ok){
  if(s->bytesAvailable() == 0) { return(false); }
  char* data = new char[1];
  s->readBlock(data, 1);
  ok = *(bool*)data;
  delete []data;
  return(true);
}

int readString(QSocket* s, string& word){
  // assumes that the string is packaged as an int describing the length
  // of the char* including the null terminator (needed for the string construction)
  // returns -1 if the word length was not read,,
  // the length of the word if this was determined, but the word could not be read,
  // and 0 on complete success. Hence, if the word length is 0, (which shouldn't happen)
  // the word just stays the same anyway..
  int length;
  if(!readInt(s, length)) { return(-1); }      
  if(s->bytesAvailable() < length){
    return(length);
  }
  char* data = new char[length];
  s->readBlock(data, length);
  word = data;
  delete []data;
  return(0);
}
  
bool readString(QSocket* s, string& word, int length){
  // length is the length of the word, -- will read this and use to create a string
  // hmm. somewhat dangerous.. maybe,, -if no null terminator the behaviour of the string
  // class might be a little screwed up. -- still maybe we can force this behaviour.
  // retunrs -1 if it can't read, and 0 on success..
  if(s->bytesAvailable() < length) { return(false); }
  char* data = new char[length];
  s->readBlock(data, length);
  word = data;
  delete []data;
  //cout << "\t\tword: " << word << endl;
  return(true);
}
