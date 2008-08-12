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

#ifndef BOOLPARSER_H
#define BOOLPARSER_H

#include <set>
#include <vector>
#include <qstring.h>   // for input

using namespace std;        // bad.... don't do it.. but lazy..

// define some structs and stuff that can be used to parse a boolean expression that
// combines a set of groups of integers. -- use lots of void pointers rather than 
// inheritance,, as it's simpler to write, and since we are going to keep the whole 
// thing kind of small and tidy.. maybe..

// structs have the below id's.. -hopefully ok.. 
const int GROUPINDEX = 1;
const int LB = 2;       // left bracket
const int RB = 3;       // right bracket.. 
const int COMB = 4;


struct Unit {
  Unit* fp;
  Unit* rp;
  virtual bool setNext(Unit* p){
    fp = p;
  }
  virtual bool setPrior(Unit* p){
    rp = p;
  }
  virtual int id(){
    return(0);
  }
  Unit(){
    fp = 0;
    rp = 0;
  }
};

struct Group : public Unit
{
  set<int> g;
  bool setNext(Unit* p);      // check rules and if OK, assign pointer.. 
  bool setPrior(Unit* p);
  int id(){
    return(GROUPINDEX);
  }
  Group(set<int> group){
    g = group;
  }
  set<int> group;
};

struct Combiner : public Unit
{
  virtual Group* combine(){
    return(0);
  }
  int id(){
    return(COMB);
  }
  Combiner(){
    // do nothing;
  }
  ~Combiner(){
  }
};

struct Lb : public Unit
{
  bool setNext(Unit* p);
  bool setPrior(Unit* p);
  int id(){
    return(LB);
  }
  Lb(){       // I may not need to call anything as long as the parent is called, hmm 
  }
};


struct Rb : public Unit 
{
  bool setNext(Unit* p);
  bool setPrior(Unit* p);
  int id(){
    return(RB);
  }
  Rb(){
  }
};

struct And : public Combiner 
{
  bool setNext(Unit* p);
  bool setPrior(Unit* p);
  Group* combine();    // look to the left and right,, -if we can combine what we see return true.. reset pointers in chain and delete left and right.. 
  And(){
  }
};

struct Or : public Combiner 
{
  bool setNext(Unit* p);
  bool setPrior(Unit* p);
  Group* combine();    // look to the left and right,, -if we can combine what we see return true.. reset pointers in chain and delete left and right.. 
  Or(){
  }
};

struct Not : public Combiner
{
  bool setNext(Unit* p);
  bool setPrior(Unit* p);
  Group* combine();    // look to the left and right,, -if we can combine what we see return true.. reset pointers in chain and delete left and right.. 
  Not(){
  }
};

class BoolParser {
  public :
    BoolParser(QString input, vector<vector<int> > g);         // vectors rather than sets as I have been using these in a different circumstance.. 
  ~BoolParser();                                               // may need to make some destructors.. 
  set<int> parseExpression();                      // do the actual parsing if OK.. 
  bool ok(){
    return(textOk);
  }

  private :
    vector<set<int> > groups;
  Unit* entryPoint;   // the beginning of the expression.. -- then follow the links, in the doubly linked list.. 
  QString text;       // the text containing the thingy.. 
  bool textOk;

  void parseString();  // parse the string and build up a thingy..
  
};

#endif
