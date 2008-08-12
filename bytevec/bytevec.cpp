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

#include "bytevec.h"
#include <vector>
#include <string>
#include <string.h>
#include <qobject.h>      // for Q_INT32 I suppose..?? do with proper functions later
#include <iostream>
#ifdef Q_WS_WIN
#include <winsock.h>
#else
#include <netinet/in.h>   // I hope.. 
#endif
#ifdef Q_OS_MACX
#include <CoreFoundation/CFByteOrder.h>
#endif

using namespace std;

ByteVec::ByteVec(){
  curSize = 0;
  memSize = 1000;
  data = new char[memSize];
  currentPosition = 0;
  error = false;
  int testValue = 123;  // test if we are bigendian...
  bigEndian = (testValue == htonl(testValue));
}

ByteVec::ByteVec(int inSize){
  memSize = inSize;
  data = new char[memSize];
  curSize = 0;
  currentPosition = 0;
  error = false;
  int testValue = 123;  // test if we are bigendian...
  bigEndian = (testValue == htonl(testValue));
}

void ByteVec::init(){
  curSize = 0;
  currentPosition = 0;       // Do not change the reserve status keep our block of memory!
  error = false;
}

int ByteVec::size(){
  return(curSize);
}

void ByteVec::reStart(){
  currentPosition = 0;       // for restarting the reading from the beginning.. -- just for reading!!
}

void ByteVec::app(char c){
  if(memSize <= curSize){
    grow();
  }
  data[curSize] = c;
  curSize++;
}

void ByteVec::grow(){
  // double the size..
  memSize = memSize * 2;
  char* tmp = new char[memSize];
  for(int i=0; i < curSize; i++){
    tmp[i] = data[i];     // isn't there a better way of doing this??
  }
  delete []data;          // does this really free all the memory??
  data = tmp;   // should I delete the tmp/???
}
 

int ByteVec::i(){
  int value = 0;
  if(currentPosition + 3 < curSize){
    value = *(int*)(data + currentPosition);     /// whoaaah. loooks dodgy. should be OK, but is dependent on how vector stores data. 
    currentPosition += 4;        // as its 4 bytes. 
  }
  return(ntohl(value));
}

unsigned int ByteVec::ui(){
  unsigned int value = 0;
    if(currentPosition + 3 < curSize){
    value = *(unsigned int*)(data + currentPosition);     /// whoaaah. loooks dodgy. should be OK, but is dependent on how vector stores data. 
    currentPosition += 4;        // as its 4 bytes. 
  }
  return(ntohl(value));
}

char ByteVec::c(){
  char ch = '0';   // as good as any other.. 
  if(currentPosition < curSize){
    ch = data[currentPosition];
    currentPosition++;
  }
  return(ch);
}

int ByteVec::qi(){
  //  Q_INT32 value = 0;
  int value;
  if(currentPosition + 3 < curSize){
    value = *(int*)(data + currentPosition);
    currentPosition += 4;
  }
  //int conValue = ntohl(value);
  return(ntohl(value));
  //  return((int)value);
}

float ByteVec::f(){
    //float sf = 0;
    //CFSwappedFloat32 f;
    float f = 0;
    char fc[sizeof(float)];
    int testValue = 123;  // test if we are bigendian...
    if(currentPosition + 3 < curSize){
	if(bigEndian){
//    if(testValue == htonl(testValue)){   // we are big endian.. but the server is not ?? ugly hack, should be fixed later
	    for(int i=0; i < sizeof(float); i++){
		fc[sizeof(float) - i - 1] = data[currentPosition + i];
	    }
	    f = *(float*)fc;
	}else{
	    //    f = *(CFSwappedFloat32*)(data + currentPosition);
	    f = *(float*)(data + currentPosition);
	}
	currentPosition += 4;
    }
    //char* c = (char*)&f;
    //char t = 'A';
    //cout << "char " << t << endl;
    //sf = CFConvertFloat32SwappedToHost(f);
    //cout << "float : " << *(float*)&f << "\t" << (int)*c << " : " << (int)*(++c) << " : " << (int)*(++c) << " : " << (int)*(++c) << endl;
    return(f);
}

double ByteVec::d(){
  double d = 0;
  // size of double is 8..
  if(currentPosition + 7 < curSize){
    d = *(double*)(data + currentPosition);
    currentPosition += 8;
  }
  return(d);
}

string ByteVec::s(){
  int length = qi();
  string word;
  if(length < 1){
    return(word);
  }
  if(currentPosition + length > curSize){
    word = "ERROR string length out of bounds";
    cerr << "ERROR, string length out of bounds" << endl;
    return(word);   // its just wrong forget it man..
    error = true;
  }
  for(int i=0; i < length; i++){
    word += data[currentPosition];
    currentPosition++;  // which could be put in the same line, but I feel its more clear this way.
  }
  return(word);
}

bool ByteVec::b(){
  bool ok = (bool)data[currentPosition];
  currentPosition++;
  return(ok);
}
    
bool ByteVec::ok(){
  return(!error);
  error = 0;   // not ideal. but.. 
}

