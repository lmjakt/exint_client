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

#include "sequenceRequest.h"
//#include <qobject.h>
#include <set>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

genomicRequest::genomicRequest(){
  // reeally do nothing..
}

genomicRequest::genomicRequest(string c, int s, int e){
  chromosome = c;
  start = s;
  end = e;
}

genomicRequest::~genomicRequest(){
  cout << "deleting genomic request : " << chromosome << "  start : " << start << endl;
 }

dnaSequence::dnaSequence(){
  // and again, well do nothing..;
}

dnaSequence::dnaSequence(string ident, string d, string s){
  id = ident;
  description = d;
  sequence = s;
}

dnaSequence::~dnaSequence(){
  cout << "deleting dnaSequence " << id << endl;
}

SequenceRequest::SequenceRequest(set<int> psets, set<int> tscripts, set<int> peps, set<genomicRequest> gmr, string fn, bool display){
  fileName = fn;
  probeSetRequests = psets;
  transcriptRequests = tscripts;
  peptideRequests = peps;
  genomeRequests = gmr;

  id = (int)(long)this; // should be unique for this machine !!
  displayOnly = display;
   // that's really all I do. I rather let the client object work out how to send all the requests, 
  // as that is its job, -i.e. working out the interface with the server. I suppose we could have 
  // a model where this would do it itself, but I don't see the benefits of doing this.
}

SequenceRequest::SequenceRequest(set<int> psets, set<int> tscripts, set<genomicRequest> gmr, string fn, bool display){
  fileName = fn;
  probeSetRequests = psets;
  transcriptRequests = tscripts;
  genomeRequests = gmr;

  id = (int)(long)this; // should be unique for this machine !!
  displayOnly = display;
   // that's really all I do. I rather let the client object work out how to send all the requests, 
  // as that is its job, -i.e. working out the interface with the server. I suppose we could have 
  // a model where this would do it itself, but I don't see the benefits of doing this.
}

SequenceRequest::~SequenceRequest(){
  cout << "deleting Sequence Request" << endl;
  cout << "probe Set requests size " << probeSetRequests.size() << endl;
  cout << "transcript requests     " << transcriptRequests.size() << endl;
  cout << "genome Requests         " << genomeRequests.size() << endl;
  // shouldn't need to do anything, but.. 
}

bool SequenceRequest::saveData(){
  // ok write this function later when I have time, for now, just make lots of noise to 
  // indicate that things are OK..
  cout << "got the signal to save the sequences" << endl
       << "and if I was doing my work I would write the sequences to : " << fileName << endl
       << "with a total of " << sequences.size() << "  sequences  of uncertain length " << endl;
  ofstream out(fileName.c_str());
  if(out.bad()){
    cout << "Some problem with opening the file, don't know what to do, returning" << endl;
    return(false);
  }
  extractSequence(out);
//   multimap<int, dnaSequence>::iterator it;
//   for(it=sequences.begin(); it != sequences.end(); it++){
//     //cout << ">" << (*it).second.id << "   " << (*it).second.description << endl;
//     out << ">" << (*it).second.id << "   " << (*it).second.description << endl;
//     //	<< (*it).second.sequence << endl;
//     for(int i=0; i < (*it).second.sequence.size(); i++){
//       //cout << (*it).second.sequence[i];
//       out << (*it).second.sequence[i];
//       if((i+1) % 100 == 0){
// 	//cout << endl;
// 	out << endl;
//       }
//     }
//     //cout << endl;
//     out << endl;
//   }
  cout << "wrote all of the stuff, try to flush" << endl;
  out.flush();
  cout << "flushed, trying to close " << endl;
  out.close();
  cout << "closed the handle, now trying to return true, what is going on" << endl;
  return(true);
}

string SequenceRequest::seq(){
    ostringstream ss;
    extractSequence(ss);
    return(ss.str());
}


void SequenceRequest::extractSequence(ostream& out){
    multimap<int, dnaSequence>::iterator it;
    for(it=sequences.begin(); it != sequences.end(); it++){
	//cout << ">" << (*it).second.id << "   " << (*it).second.description << endl;
	out << ">" << (*it).second.id << "   " << (*it).second.description << endl;
	//	<< (*it).second.sequence << endl;
	for(int i=0; i < (*it).second.sequence.size(); i++){
	    //cout << (*it).second.sequence[i];
	    out << (*it).second.sequence[i];
	    if((i+1) % 100 == 0){
		//cout << endl;
		out << endl;
	    }
	}
	//cout << endl;
	out << endl;
    }
    cout << "wrote all of the stuff, try to flush" << endl;
}
