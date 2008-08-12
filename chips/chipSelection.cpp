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

#include "chipSelection.h"
#include <iostream>

using namespace std;

ChipSelection::ChipSelection(){
  // there isn't actually anything I need to do here I think.
}

ChipSelection::~ChipSelection(){
  // and nothing here either really..
}

void ChipSelection::setChipInfo(map<int, chipInfo> info){
  chips = info;
  states.erase(states.begin(), states.end());
  for(map<int, chipInfo>::iterator it=chips.begin(); it != chips.end(); it++){
    states.insert(make_pair((*it).first, true));
  }
  //cout << "chipselection emitting newChipInfo with states being : " << states.size() << endl;
  emit newChipInfo(states);
}

void ChipSelection::toggleChip(int n){
  // set state of n to be something it isn't.. 
  //cout << "toggling chip " << n << endl;
  map<int, bool>::iterator it = states.find(n);
  if(it == states.end()){
    cerr << "no chip defined for n=" << n << endl;
    return;
  }
  (*it).second = !(*it).second;
  //cout << "chip " << n << " is now " << (*it).second << endl;
  // and also set all the equivalents to this; 
  map<int, chipInfo>::iterator cit = chips.find(n);
  set<int>::iterator sit;
  if(cit != chips.end()){
    for(sit = (*cit).second.equivs.begin(); sit != (*cit).second.equivs.end(); sit++){
      states[*sit] = states[n];
      //  cout << "and setting equiv " << *sit << "  to " << states[n] << endl;
    }
  }
    
  // then see if we need to change anything else..
  for(cit=chips.begin(); cit != chips.end(); cit++){
    //cout << "and checking what to do with " << (*cit).first << endl;
    if(!(*it).second){         // this is to say that the state got switched off, then any chip containing this group has to be off too..
      //cout << "thingy for " << (*it).first << "  is " << (*it).second << "  so we do stuff " << endl;
      if((*cit).second.defined(n)){
	states[(*cit).first] = false;
	//cout << "set " << (*cit).first << " to be false .. " << endl;
      }
    }else{
      // in this case its no. if all the members of an equivalent group are now on, then we have to set that to on
      bool check = true;
      for(sit = (*cit).second.equivs.begin(); sit != (*cit).second.equivs.end(); sit++){
	//cout << "going through " << (*cit).first << "  equiv: " << *sit << endl;
	if(!states[(*sit)]){
	  //cout << "setting to false " << endl;
	  check = false;
	}
      }
      //cout << "and setting the " << (*cit).first << " to " << check << endl;
      states[(*cit).first] = check;  // but what if this was already on ?? I'm really not sure..
    }
  }
  vector<int> v;
  for(map<int, bool>::iterator bit=states.begin(); bit != states.end(); bit++){
    if((*bit).second){
      v.push_back((*bit).first);
    }
  }
  // make a map of chip equivalents as well. This should be kind of easy, but can't think of how to do it right now..
  // the long way around is to go through all the chips, their equivants, and read out the value from states.
  // this is kind of ugly, and if we look closely at the structures and the code, there is bound to be a better way, but 
  // right now I'm anyway, kind of too confused to do much about this..
  map<int, bool> equivStates;
  for(map<int, chipInfo>::iterator it=chips.begin(); it != chips.end(); it++){
    for(set<int>::iterator sit=(*it).second.equivs.begin(); sit != (*it).second.equivs.end(); sit++){
      // check the states of the states thingy..
      map<int, bool>::iterator bit = states.find(*sit);
      if(bit !=states.end()){
	equivStates.insert(make_pair(*sit, (*bit).second));
      }
    }
  }
  cout << "emitting equivalent states " << endl;
  emit equivalentStates(equivStates);
  emit toggleButtons(states);         // this way we can connect to several user interfaces rather easily.. 
  emit setClientChips(v);
}
