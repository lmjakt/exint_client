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

#include "chipChooser.h"
#include <iostream>

using namespace std;

ChipChooser::ChipChooser(ChipSelection* sel, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  cout << "chip chooser beginning " << endl;
  selection = sel;
  connect(selection, SIGNAL(newChipInfo(map<int, bool>)), this, SLOT(newChipInfo(map<int, bool>)));
  connect(selection, SIGNAL(toggleButtons(map<int, bool>)), this, SLOT(toggleButtons(map<int, bool>)));

  hbox = new QHBoxLayout(this);
  cout << "chip chooser the end " << endl;
  // and that is perhaps all that I need to do..
}

void ChipChooser::toggleButtons(map<int, bool> info){
  //cout << "chip chooser toggle buttons " << endl;
  map<QPushButton*, int>::iterator it;
  map<int, bool>::iterator bit;
  for(it = buttons.begin(); it != buttons.end(); it++){
    //cout << "chipchooser button id : " << (*it).second << "   --> ";
    bit = info.find((*it).second);
    if(bit != info.end()){
      //cout << (*bit).second << endl;
      // then change the state of the button, by changing its colour..
      (*it).first->setDown((*bit).second);
      //  cout << "changing the background of button with index : " << (*it).second << "  to have a state of : " << (*bit).second << endl;
    }else{
      cerr << "No state defined for a button with index : " << (*it).second << endl;
    }
  }
  //cout << "chipchooser toggle buttons end " << endl;
}
  
void ChipChooser::newChipInfo(map<int, bool> info){
  // first lets get rid of all the old buttons..
  cout << "ChipChooser new chip info, info size is : " << info.size() << endl;
  QPushButton* temp;
  map<QPushButton*, int>::iterator it;
  while(buttons.size()){
    it = buttons.begin();
    temp = (*it).first;
    buttons.erase(it);
    delete temp;
  }
  // and then//
  map<int, bool>::iterator bit;
  for(bit = info.begin(); bit != info.end(); bit++){
    QString num;
    num.setNum((*bit).first);
    QPushButton* button = new QPushButton(num, this, "button");
    connect(button, SIGNAL(pressed()), this, SLOT(buttonPressed()) );
    button->setDown((*bit).second);
    buttons.insert(make_pair(button, (*bit).first));
    hbox->addWidget(button);
    button->show();
  }
  // which should be everything.. 
  //  cout << "buttons size is now " << buttons.size() << endl;
}

void ChipChooser::buttonPressed(){
  QPushButton* button = (QPushButton*)sender();
  map<QPushButton*, int>::iterator it = buttons.find(button);
  if(it != buttons.end()){
    selection->toggleChip((*it).second);
  }
}
    
