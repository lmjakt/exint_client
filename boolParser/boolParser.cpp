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

#include "boolParser.h"
#include <qregexp.h>
#include <iostream>

using namespace std;

bool Group::setNext(Unit* p){
  // a group must be followed either by a combiner or by a right bracket, otherwise it's not valid..
  if(p){
    if(p->id() == RB || p->id() == COMB){
      fp = p;
      return(true);
    }
  }else{
    fp = 0;
    return(true);
  }
  cerr << "group set next error : " << endl;
  return(false);
}

bool Group::setPrior(Unit* p){
  // a group must be preceded by a combiner or by a left bracket.. p could be 0, in which case this will cause a segmentation fault.. 
  cout << "Group set prior setting to : " << p << endl;
  if(p){
    if(p->id() == LB || p->id() == COMB){
      rp = p;
      cout << "\t set to p" << endl;
      return(true);
    }
  }else{
    cout << "\tset to 0 " << endl;
    rp = 0;
    return(true);
  }
  cerr << "group set prior error : " << endl;
  return(false);
}

bool Lb::setNext(Unit* p){
  if(p){
    if(p->id() == LB || p->id() == GROUPINDEX){     // technically we could have '()' as a valid expression, but I don't have a combiner rule for this.. so let's remove.. 
      fp = p;
      return(true);
    }
  }
  cerr << "left bracket set next error : " << endl;
  return(false);
}

bool Lb::setPrior(Unit* p){
  if(p){
    if(p->id() == LB || p->id() == COMB){
      rp = p;
      return(true);
    }
  }else{
    rp = 0;
    return(true);
  }
  cerr << "left bracket set prior error : " << endl;
  return(false);
}

bool Rb::setNext(Unit* p){
  if(p){
    if(p->id() == RB || p->id() == COMB){
      fp = p;
      return(true);
    }
  }else{
    rp = 0;
    return(true);
  }
  cerr << "right bracket set next error : " << endl;
  return(false);
}

bool Rb::setPrior(Unit* p){
  if(p){
    if(p->id() == GROUPINDEX || p->id() == RB){
      rp = p;
      return(true);
    }
  }
  cerr << "right bracket set prior error : " << endl;
  return(false);
}

bool And::setNext(Unit* p){
  if(p){
    if(p->id() != COMB && p->id() != RB){
      fp = p;
      return(true);
    }
  }
  cerr << "And set next error : " << endl;
  return(false);
}

bool And::setPrior(Unit* p){
  if(p){
    if(p->id() != COMB && p->id() != LB){
      rp = p;
      return(true);
    }
  }
  cerr << "and set prior error : " << endl;
  return(false);
}

Group* And::combine(){
  if(rp->id() != GROUPINDEX || fp->id() != GROUPINDEX){        // only possible to combine two groups.. 
    return(0);
  }
  set<int> combined;
  set<int>::iterator it;    // I wonder if there is a function I can use.. 
  for(it = ((Group*)rp)->g.begin(); it != ((Group*)rp)->g.end(); it++){
    if(((Group*)fp)->g.count(*it)){
      combined.insert(*it);
    }
  }
  Group* g = new Group(combined);
  return(g);
}

bool Or::setNext(Unit* p){
  if(p){
    if(p->id() != COMB && p->id() != RB){
      fp = p;
      return(true);
    }
  }
  cerr << "Or set next error : " << endl;
  return(false);
}

bool Or::setPrior(Unit* p){
  if(p){
    if(p->id() != COMB && p->id() != LB){
      rp = p;
      return(true);
    }
  }
  cerr << "Or set prior error : " << endl;
  return(false);
}

Group* Or::combine(){
  if(rp->id() != GROUPINDEX || fp->id() != GROUPINDEX){
    return(0);
  }
  Group* r = (Group*)rp;
  Group* f = (Group*)fp;
  set<int> combined(r->g.begin(), r->g.end());
  for(set<int>::iterator it = f->g.begin(); it != f->g.end(); it++){
    combined.insert(*it);
  }
  Group* g = new Group(combined);
  return(g);
}

bool Not::setNext(Unit* p){
  if(p){
    if(p->id() != COMB && p->id() != RB){
      fp = p;
      return(true);
    }
  }
  cerr << "not set next error : " << endl;
  return(false);
}

bool Not::setPrior(Unit* p){
  if(p){
    if(p->id() != COMB && p->id() != LB){
      rp = p;
      return(true);
    }
  }
  cerr << "not set prior error : " << endl; 
  return(false);
}

Group* Not::combine(){
  if(rp->id() != GROUPINDEX || fp->id() != GROUPINDEX){
    return(0);
  }
  set<int> combined;
  Group* r = (Group*)rp;
  Group* f = (Group*)fp;
  for(set<int>::iterator it = r->g.begin(); it != r->g.end(); it++){
    if(!f->g.count(*it)){
      combined.insert(*it);
    }
  }
  Group* g = new Group(combined);
  return(g);
}


// so it seems perhaps rather obvious that I could have done with another level of inheritance, but at the moment
// of writing, I'm not actually sure if I'm writint the syntax correctly, so I want to keep new stuff to a minimum.
// I am aware of the obvious unneccessary duplications of code above though.. -- with their increased likelihood of bugs..

BoolParser::BoolParser(QString input, vector<vector<int> > g){
  textOk = true;  // unless set to false by the parseString function..
  text = input;
  entryPoint = 0;
  // and let's make the vectors into sets..
  for(int i=0; i < g.size(); i++){
    set<int> t;  // temporary set..
    for(int j=0; j < g[i].size(); j++){
      t.insert(g[i][j]);
    }
    groups.push_back(t);
  }
  // parse the string and determine if ok.. 
  parseString();
  // and do nothing.. 
}

BoolParser::~BoolParser(){
  cout << "Try to delete the funny structure here... " << endl;
  Unit* p = entryPoint;
  while(p){
    cout << "deleting unit " << endl;
    Unit* tp = p;
    p = p->fp;
    delete(tp);
  }
}

void BoolParser::parseString(){
  // go through the string, and try to build up a series of Units containing the individual groups.. 
  // but first let's check how the thing works..
  // MAKE SURE TO REMEMBER TO COUNT THE BRACKETS.. OK LA.. 
  QRegExp rx("[^ 0-9!&\\|\\)\\(]");
  if(rx.search(text) != -1){
    cerr << "Text contains illegal characters,, speak to your admin.. " << endl;
    textOk = false;
    return;
  }
  // let's remove the spaces for convenience...
  text.replace(' ', "");
  //cout << "and the text is now .. " << text << endl;
  int i=0;
  Unit* lastOne = 0;
  int rbc = 0;
  int lbc = 0;    // left and right bracket counters .. 

  while(i < text.length()){
    QString numstring("");
    char c = text.at(i).latin1();
    bool broken = false;
    while(c >= '0' && c <= '9'){     // i.e. it's a digit..
      numstring.append(text.at(i));
      i++;
      if(i >= text.length()){
	broken = true;
	break;
      }
      c = text.at(i).latin1();             // ugly as hell....
    }
    if(numstring.length()){
      //cout << "got a number string should make a group unit" << endl;
      // then make the group unit, followed by the thingy..
      bool ok;
      int gi = numstring.toInt(&ok) - 1;        // let the user count from 1, they will be happier.. 
      //cout << "and the number is thingy : " << gi << endl;
      if(gi >= groups.size() || gi < 0){          // no negative numbers.. 
	cerr << "Group number is too large : " << gi << endl;
	textOk = false;
	return;
      }
      Group* g = new Group(groups[gi]);
      if(!lastOne){     // we are at the beginning //
	entryPoint = g;
	lastOne = g;
      }else{
	if(!lastOne->setNext(g) || !g->setPrior(lastOne)){
	  // bugger.. it doesn't like this one.. 
	  cerr << "Some error in setting the last one and the first one.. hmmm " << endl;
	  delete(g);
	  lastOne->fp = 0;       // override checks and make sure that that we didn't set this by mistake.. 
	  textOk = false;
	  return;
	}
	lastOne = g;
      }
    }
    // if broken is true, then we are basically done..
    if(broken){     // means at end, i.e. no character following the last number group.. 
      break;
    }
    // otherwise to a switch on the remaining character..
    cout << "remaining character is " << c << endl;
    if(c == '('){ 
      // make a left bracket.. and break
      Lb* lb = new Lb();
      if(!lastOne){
	lastOne = lb;
	entryPoint = lb;
      }else{
	if(!lastOne->setNext(lb) || !lb->setPrior(lastOne)){
	  cerr << "left bracket not appropriate at this point.. " << endl;
	  lastOne->fp = 0;
	  textOk = false;
	  delete(lb);
	  return;
	}
      }
      lastOne = lb;
      lbc++;
    }
    if(c ==  ')'){
      // make a right bracket and break;
      if(!lastOne){               // we can't start with a right bracket.. break
	textOk = false;
	cerr << "can't start an expression with a right bracket.. " << endl;
	return;
      }
      Rb* rib = new Rb();
      if(!lastOne->setNext(rib) || !rib->setPrior(lastOne)){
	lastOne->fp = 0;
	textOk = false;
	delete(rib);
	return;
      }
      lastOne = rib;
      rbc++;
    }
    if(c == '|'){
      if(!lastOne){
	textOk = false;
	cerr << "can't start with an or.. " << endl;
	return;
      }
      Or* o = new Or();
      if(!lastOne->setNext(o) || !o->setPrior(lastOne)){
	cerr << "cant combine or like this.. " << endl;
	textOk = false;
	lastOne->fp = 0;
	delete(o);
	return;
      }
      lastOne = o;
    }
    if(c == '&'){
      if(!lastOne){
	cerr << "can't start with an and" << endl;
	textOk = false;
	return;
      }
      And* a = new And();
      if(!lastOne->setNext(a) || !a->setPrior(lastOne)){
	lastOne->fp = 0;
	textOk = false;
	delete a;
	return;
      }
      lastOne = a;
    }
    if(c == '!'){
      if(!lastOne){
	cerr << "cant start with a not.. " << endl;
	textOk = false;
	return;
      }
      Not* n = new Not();
      if(!lastOne->setNext(n) || !n->setPrior(lastOne)){
	lastOne->fp = 0;
	textOk = false;
	cerr << "coudln't set the not .. " << endl;
	delete(n);
	return;
      }
      lastOne = n;
    }
    if(!textOk){
      break;
    }
    i++;
  }
  if(rbc != lbc){
    cerr << "Mispaired brackets .. rbc " << rbc << "\tlbc " << lbc << endl;
    textOk = false;
  }
  // and that may be Ok..  
}

set<int> BoolParser::parseExpression(){
  // ok, let's just go through the expression..
  set<int> combined;
  if(!textOk){
    cerr << "Illegal Expression, bugger off " << endl;
    return(combined);
  }
  // reduce the expression in two steps...
  // -- combine everything within brackets,, -and reassign the pointers..
  // -- remove superflous brackets..  .. i.e.   (group) and (group)   .. -at the moment () is not allowed.. 
  // -- iterate until we are left with one group.. 
  Unit* p;
  int counter = 2;
  while(counter > 1){
    //cout << "Starting the first round of stuff... going through the thing.. " << endl;
    //cout << "The counter is now : " << counter << endl;
    p = entryPoint;
    while(p){
      //cout << "\t" << p->id();
      if(p->id() == COMB){
	//cout << "  we have a combiner " << endl;
	Group* g = ((Combiner*)p)->combine();
	if(g){          // successful thingy.. let's see if we can now do the relevant stuff..
	  //cout << "we got a combined group size is : " << g->g.size() << endl;
	  if(g->setNext(p->fp->fp) && g->setPrior(p->rp->rp)){
	    // hey man it's cool... let's see if we can set those as well..
	    if(!g->rp){           // g is now the entrypoint..
	      entryPoint = g;
	    }else{
	      if(!g->rp->setNext(g)){
		cerr << "Couldn't setNext for the preceding bugger.. hmm, trouble.. " << endl;
	      }
	    }
	    if(g->fp){         // g isn't the last one.. 
	      if(!g->fp->setPrior(g)){
		cerr << "Couldn't setPrior for the next one ,, bugger, hmm, really don't know how to resolve this" << endl;
	      }
	    }else{
	      //cout << "g fp is nil... we are at the end.. " << endl;
	    }
	    delete p->fp;
	    delete p->rp;
	    delete p;   /// if everything according to plan.. 
	    p = g;
	  }else{
	    cerr << "g is no good, we cannot do the assignments " << endl;
	    delete g;
	  }
	}
      }
      p = p->fp;
      //cout << "and the value of p is now : " << p << endl;
    }
    cout << endl;
    // and let's remove brackets...
    p = entryPoint;
    counter = 0;
    while(p){
      //cout << "bracket eliminator id is : " << p->id() << endl;
      if(p->id() == GROUPINDEX && p->rp && p->fp){
	//cout << "time to eliminate " << endl;
	if(p->rp->id() == LB && p->fp->id() == RB){    // ok... let's remove the brackets..
	  //cout << "\t\tdo it" << endl;
	  Lb* lb = (Lb*)p->rp;
	  Rb* rb = (Rb*)p->fp;
	  //cout << "p rp is " << p->rp << endl;
	  //cout << "p rp -> rp " << p->rp->rp << endl;
	  if(p->setPrior(p->rp->rp) && p->setNext(p->fp->fp)){
	    //cout << "set the prior and thingy of p " << endl;
	    //cout << "p rp is " << p->rp << endl;
	    //cout << "p fp is " << p->fp << endl;
	    if(p->rp){
	      p->rp->setNext(p);
	      //cout << "\tset the next for the rp " << endl;
	    }else{
	      entryPoint = p;
	    }
	    if(p->fp){
	      p->fp->setPrior(p);        // if these don't work, then I'm not sure what to do.. 
	      //cout << "\tset the prior for fp " << endl;
	    }
	    delete lb;
	    delete rb;
	    //cout << "deleteed " << endl;
	  }else{
	    cerr << "coulnd't remove the links to the brackets,, should probably die here.. " << endl;	
	  }
	}
      }
      counter++;
      //cout << "p fp is " << p->fp << endl;
      p = p->fp;
      //cout << "do I get here " << endl;
      //cout << " and the entrypoint is : " << entryPoint << endl;
    }
    // for debuggin, let's just go through and see what the thing consists of
    p = entryPoint;
    //cout << "What's going on here p is " << p << endl;
    //while(p){
    //  cout << "something " << endl;
    //  cout << "\t" << p->id();
    //  p = p->fp;
    //}
    cout << endl;
  }
  //  cout << "by now entry point should be one group.. and it should have some specific size ;; " << endl;
  // cout << "The id of entryPoint is : " << entryPoint->id() << endl;
  if(entryPoint->id() == GROUPINDEX){
    cout << "And the entry point is a group, it's size is : " << ((Group*)entryPoint)->g.size() << endl;
    return(((Group*)entryPoint)->g);
  }
  return(combined);  // have to return something.. 
}

