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

#include "informationReader.h"
#include "probe_set/probe_set.h"
#include <qsocket.h>
#include <string>
#include <qobject.h>
#include <iostream>

using namespace std;

InformationReader::InformationReader(QObject* parent, const char* name) :
  QObject(parent, name)
{
  clear();
  // resets everything.
}

InformationReader::~InformationReader(){
  // ??
}

void InformationReader::clear(){
  bytesToRead = 0;
}

bool InformationReader::startReading(QSocket* socket, vector<nadFileInfo>& fileData){
  clear();
  char data[4];
  unsigned int buf = 4;
  if(bytesToRead == 0){
    if(socket->bytesAvailable() >= 4){
      socket->readBlock(data, buf);
      bytesToRead = *(int*)data;
    }else{
      return(false);
    }
  }
  if(socket->bytesAvailable() < bytesToRead){
    cout << "not enough bytes available on the socket" << endl
	 << "\twant " << bytesToRead << endl
	 << "\thave " << socket->bytesAvailable() << endl;
    return(false);
  }
  int n;    // general purpose
  float f;  // another general purpose thingy..
  // get the size of the vector.. 
  socket->readBlock(data, buf);
  n = *(int*)data;
  fileData.resize(n);
  //  cout << "resized fileData to : " << n << endl;
  // and then read in for each thingy..
  char* fileName;       // for longer reading..
  for(int i=0; i < fileData.size(); i++){
    // first the length of the fileName..
    socket->readBlock(data, buf);
    n = *(int*)data;     // the length of the filename including the \0
    //cout << "length of file name: " << n << endl;
    char* test = new char[n];
    socket->readBlock(test, n);
    //cout << "read filename from socket" << endl;
    fileData[i].fileName = test;
    //cout << "filename          : " << test << endl;
    delete []test;
    // then 4 numbers..
    socket->readBlock(data, buf);
    fileData[i].minIndex = *(int*)data;
    socket->readBlock(data, buf);
    fileData[i].maxIndex = *(int*)data;
    socket->readBlock(data, buf);
    fileData[i].chipIndex = *(int*)data;
    socket->readBlock(data, buf);
    fileData[i].fileNo = *(int*)data;
    // Then the number of experimentIndices
    socket->readBlock(data, buf);
    n = *(int*)data;
    fileData[i].experimentIndices.resize(n);
    //cout << "resized fileData expt. Indices to : " << n << endl;
    for(int j=0; j < fileData[i].experimentIndices.size(); j++){
      socket->readBlock(data, buf);
      fileData[i].experimentIndices[j] = *(float*)data;
    }
    //cout << "read in the exptIndices " << endl;
    // the number of fileNames...
    socket->readBlock(data, buf);
    fileData[i].fileNames.resize(*(int*)data);
    for(int j=0; j < fileData[i].fileNames.size(); j++){
      // first the size of the thingy
      socket->readBlock(data, buf);
      n = *(int*)data;
      char* test = new char[n];
      socket->readBlock(test, n);
      fileData[i].fileNames[j] = test;
      //cout << "fileData fileName: " << fileData[i].fileNames[j] << endl;
      delete []test;
    }
    //cout << "read in the file names.. " << endl;
    socket->readBlock(data, buf);  // the number of descriptions..
    fileData[i].descriptions.resize(*(int*)data);
    for(int j=0; j < fileData[i].descriptions.size(); j++){
      socket->readBlock(data, buf);
      n = *(int*)data;
      char* test = new char[n];
      socket->readBlock(test, n);
      fileData[i].descriptions[j] = test;
      delete []test;
    }
  }
  // then read in the tail, and if we get it allelulia..
  char endToken[21];
  socket->readBlock(endToken, 21);
  if(QString(endToken) == QString("<FileInformationEnd>")){
    return(true);
  }else{
    cout << "didn't get the proper tailing. returning false" << endl
	 << "got:    " << endToken << endl
	 << "wanted: " << "<FileInformationEnd>" << endl;
    emit bigError();
    return(false);
  }
}
    
    
      
  
