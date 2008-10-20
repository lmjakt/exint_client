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

#include "dataStructs.h"       // which should include everything I need..
#include <iostream>
#include <qobject.h>


vector<int> IndexSet::And(vector<int> b){
  // first create a vector c and a set b..
  vector<int> c;
  c.reserve(Index.size() < b.size() ? Index.size() : b.size());
  set<int> bset;
  for(uint i=0; i < b.size(); i++){
    bset.insert(b[i]);
  }
  // and simply..
  for(uint i=0; i < Index.size(); i++){
    if(bset.count(Index[i])){
      c.push_back(Index[i]);
    }
  }
  return(c);
}

vector<int> IndexSet::Or(vector<int> b){
  // simply concatenate the two vectors.. 
  vector<int> c;
  c.reserve(Index.size() + b.size());
  set<int> aused;
  for(uint i=0; i < Index.size(); i++){
    c.push_back(Index[i]);         // whatever the case..
    aused.insert(Index[i]);
  }
  // and put in the rest from b that we haven't already put in and check the result..
  // keeping the order by using a ugly method..
  for(uint i=0; i < b.size(); i++){
    if(!aused.count(b[i])){
      c.push_back(b[i]);
    }
  }
  return(c);
}

vector<int> IndexSet::Not(vector<int> b){   // subtracts things in this vector from b.. 
  vector<int> c;
  c.reserve(b.size());
  set<int> aset;
  for(uint i=0; i < Index.size(); i++){
    aset.insert(Index[i]);
  }
  for(uint i=0; i < b.size(); i++){
    if(!aset.count(b[i])){
      c.push_back(b[i]);
    }
  }
  return(c);
}
    

ProtocolStep::ProtocolStep(int i, int pi, uint time, int ci, string cname, string des){
  id = i;
  parentId = pi;
  creatorId = ci;
  creatorName = cname;
  description = des;
  creationTime.setTime_t(time);
}

ProtocolStep::~ProtocolStep(){
  cout << "deleting ProtocolStep" << endl;
}

Protocol::Protocol(int i, int pi, uint time, int cid, string cname, string pname, string pdes, int reqId){
  cout << "Making a protocol with a name of " << pname << endl;
  id = i;
  parentId = pi;
  creationTime.setTime_t(time);
  creatorId = cid;
  creatorName = cname;
  name = pname;
  description = pdes;
  parent = 0;
  currentDaughter = 0;
  pointer = 0;
  requestId = reqId;
}

Protocol* Protocol::nextDaughter(){
  Protocol* daughter = parent;          ////  oooohhh,,,, 
  if((int)daughters.size() > currentDaughter){
    daughter = daughters[currentDaughter];
    currentDaughter++;
  }
  return(daughter);
}

Protocol::~Protocol(){
  cout << "deleting protocol " << endl;
  for(uint i=0; i < steps.size(); i++){
    cout << "protocol destructor deleting step : " << i << endl;
    delete steps[i];
    steps[i] = 0;     // for completeness.. 
  }
}

ProtocolCollection::ProtocolCollection(map<int, Protocol*> prots){
  // go through all and fill int the pointers.. allow only one parent, and no circular arrangements (how??)
  // if only a single parent is allowed,, 
  valid = true;   // any problem and change to false.. 
  refcount = 0;
  protocols = prots;
  rootProtocol = 0;
  map<int, Protocol*>::iterator it;
  map<int, Protocol*>::iterator fit;   // for use with find.. 

  bool foundRoot = false;
  for(it = protocols.begin(); it != protocols.end(); it++){
    fit = protocols.find((*it).second->parentId);
    if(fit == protocols.end()){
      valid = false;
      continue;
    }
    /// is this a root node
    if((*fit).second == (*it).second){        // i.e. the parent points to itself.. 
      if(foundRoot){
	valid = false;
	cerr << "Found more than one Root. bugger.. " << endl;
      }
      foundRoot = true;
      (*it).second->parent = 0;                  // prevent infinite loops, but means have to check very carefully.. when parsing trees.. -- may be a bad idea
      (*fit).second->daughters.push_back((*it).second);
      rootProtocol = (*it).second;
      continue;
    }
    // set parent and add daughter to parent..
    (*it).second->parent = (*fit).second;
    (*fit).second->daughters.push_back((*it).second);
  }
  if(!rootProtocol){
    cerr << "Didn't manage to find any root protocols.. bugger, not sure what to do now.. rethink you bastard" << endl;
    valid = false;
  }
}

ProtocolCollection::~ProtocolCollection(){
  cout << "\t\tProtocol Coleection : address " << (long)this << endl;
  //  refcount--;
  cout << "\t\tDELETE PROTOCOL COLLECTION DESTRUCTOR +++++++Protocol Collection refcount is : " << refcount << endl;
  if(refcount > 0){
    cout << "returning because refcount is too high" << endl;
    return;     // can I do this ?? - probably not but lets see anyway.. 
  }
  map<int, Protocol*>::iterator it;
  cout << "deleting protocols from protocol collection" << endl;
  while(protocols.size()){
    it = protocols.begin();
    //  for(it = protocols.begin(); it != protocols.end(); it++){
    Protocol* temp = (*it).second;
    protocols.erase(it);
    delete temp;
  }
}

//// 
