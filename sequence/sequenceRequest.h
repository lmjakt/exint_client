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

#ifndef SEQUENCEREQUEST_H
#define SEQUENCEREQUEST_H
//#include <qobject.h>
#include <set>
#include <string>
#include <map>
#include <iostream>

using namespace std;

// first a couple of structs that we'll need to use within and maybe elsewhere.

struct genomicRequest {
  genomicRequest(string c, int s, int e);
  genomicRequest();
  ~genomicRequest();
  string chromosome;  // string as we have also got X and Y
  int start;
  int end;
  friend bool operator<(const genomicRequest& a, const genomicRequest& b){
    return(&a < &b);
  }
};

struct dnaSequence {
  dnaSequence(string ident, string d, string s);
  dnaSequence();
  ~dnaSequence();
  string id;
  string description;
  string sequence;    // pretty selfexplanatory really..
};

class SequenceRequest          //  : public QObject
{
  //Q_OBJECT
  // shouldn't have any need to inherit anything.. // -need to inherit QObject to use slots and signals
  public :
    //    SequenceRequest(set<string> psets, set<int> tscripts, set<genomicRequest> gmr, string fn);
    //SequenceRequest(set<string> psets, set<int> tscripts, set<int> peps, set<genomicRequest> gmr, string fn);
    SequenceRequest(set<int> psets, set<int> tscripts, set<genomicRequest> gmr, string fn, bool display=false);
    SequenceRequest(set<int> psets, set<int> tscripts, set<int> peps, set<genomicRequest> gmr, string fn, bool display=false);
  ~SequenceRequest();
  //void addSequence(dnaSequence s, int type);  // type - 0, 1, 2 for probe set, transcript, genomic. 
  set<int> probeSetRequests;
  set<int> transcriptRequests;
  set<int> peptideRequests;           // uses gene indices, not peptide indices.. for better or worse.. !!
  set<genomicRequest> genomeRequests;   // ok, so I know it's considered bad, but what the hell.. 
  int id;
  bool saveData();  // just the filename as a fasta file. try to do appropriate error checking.
  string seq();  // just return the whole string and let someone else handle it.
  multimap<int, dnaSequence> sequences; // again, 0, 1, 2, for the probe set, transcript, genomic.  
  bool displayOnly;   // if we want to display it rather than do something.. 
   
  private :
    string fileName;
  void extractSequence(ostream& out);
  //signals :
  //  void deleteMe();
};

#endif


  
