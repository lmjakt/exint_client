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

#include "protocolViewer.h"
#include <vector>
#include <iostream>
#include <qpushbutton.h>

using namespace std;

ProtocolViewer::ProtocolViewer(int myId, ProtocolClipboard* clpboard, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  userId = myId;
  protocol = 0;
  clipboard = clpboard;
  //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  firstStep = 0;
  //  copiedStep = 0;

  grid = new QGridLayout();

  QPushButton* addStepButton = new QPushButton("Append Step", this, "addStepButton");
  connect(addStepButton, SIGNAL(clicked()), this, SLOT(addStep()) );
  QVBoxLayout* box = new QVBoxLayout(this);
  box->addLayout(grid);
  box->addWidget(addStepButton);
  // which may be all I need to do for now..
}

ProtocolViewer::~ProtocolViewer(){
  // don't delete the protocol, let the holder delete it.. 
  //if(protocol){
  //  delete protocol;
  //}
}

QSizePolicy ProtocolViewer::sizePolicy() const
{
  return(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
}

void ProtocolViewer::setProtocol(Protocol* prot){
  cout << "setting protocol : " << endl;
  protocol = prot;
  // delete steps..
  for(int i=0; i < steps.size(); i++){
    delete steps[i];
  }
  // create and link the steps together.. 
  steps.resize(protocol->steps.size());
  cout << "steps size is : " << steps.size() << endl;
  for(int i=0; i < protocol->steps.size(); i++){
    // if only one step, we don't do anything.. which is OK..
    cout << "making a step " << endl;
    steps[i] = new ProtocolStepWidget(this);
    //view->addChild(steps[i]);
    steps[i]->setStep(*protocol->steps[i], i+1);
    if(i > 0){
      steps[i-1]->setAfter(steps[i]);
    } // hence implementing a linked list.. -- that can be manipulated.. easily.. 
  }
  if(steps.size()){
    firstStep = steps[0];
  }else{
    firstStep = 0;
  }
  //// -- these are addresses.. which we've copied.. I think I have to rethink
  //// -- as we may well lose ownership of these steps, and they may be deleted..
  // then go through and make visible..
  for(int i=0; i < steps.size(); i++){
    grid->addWidget(steps[i], i, 0);
    cout << "calling show on the steps : " << endl;
    steps[i]->show();
  }
  /// and we should be OK..
}

void ProtocolViewer::addStep(int pos){
  //    << "steps size is           : " << steps.size() << endl;
  ProtocolStepWidget* step = new ProtocolStepWidget(this);
  connect(step, SIGNAL(sizeHintChanged(ProtocolStepWidget*)), this, SLOT(childChanged(ProtocolStepWidget*))); 
  // if pos is larger than the current size add at the end..
  // if pos is 0 or negative, then add at the beginning.. 
  steps.push_back(step);
  //cout << "pushed back steps.. " << endl;
  // if we only have one step at this point we have some trouble..
  if(steps.size() == 1){
    //cout << "only one step, make sure to call it as being something or other " << endl;
    firstStep = step;
    arrangeSteps(0);
    //cout << "arrange steps returned" << endl;
    return;
  }
  
  if(pos < 0){
    // set hints..
    step->setAfter(firstStep);   // which can be 0,, if we don't have any steps..
    firstStep = step;
    // now we have to rearrange all of the steps.. hmm,, 
    arrangeSteps(0);
    return;
  }
  //cout << "going for the loop man.. " << endl;
  int i = 0;    // the iterator counter.. not very great..
  ProtocolStepWidget* it; //= firstStep;
  ProtocolStepWidget* lastOne;   // memory.. very ugly, .. hmm.. 
  for(it = firstStep; it != 0; it = it->next()){     // i.e. iterate until we get a 0.. 
    //cout << "i : " << i << "  it : " << (int)it << "   it next is " << (int)it->next() << endl;
    if(i == pos){
      //cout << "  and i is equal to pos " << endl;
      step->setAfter(it->next());
      it->setAfter(step);
      //cout << "calling arrange steps " << endl;
      arrangeSteps(pos+1);       // arrange until after pos + 1.. 
      return;
    }
    lastOne = it;
    ++i;
  }
  /// if I'm here I want to add it to the end.. which means simply..
  lastOne->setAfter(step);
  arrangeSteps(i);
}

void ProtocolViewer::arrangeSteps(int pos){
  //  cout << "top of arrange steps " << endl;
  int i = 0;
  ProtocolStepWidget* it = firstStep;
  for(it=firstStep; it != 0; it = it->next() ){    // I think that is OK.. hmm.
    //cout << "address of it is now : " << (int)it << endl;
    if(i >= pos){
      if(it->next()){
	//cout << "trying to remove the last one " << endl;
	grid->remove(it->next());
      }
      grid->addWidget(it, i, 0);
      it->setPosition(i+1);
      it->show();
    }
    ++i;
  }
  //  cout << "and outside of the loop, address of it is now " << (int)it << endl;
  layout()->activate();
  setMinimumHeight(layout()->sizeHint().height());
  //  updateGeometry();
  emit IGrew();
}

void ProtocolViewer::addStep(){
  //cout << "Calling addStep what is going on" << endl;
  addStep(steps.size());
  //addStep(-1);
}

void ProtocolViewer::prependStep(){
  addStep(-1);
}

void ProtocolViewer::addStepAfterCurrent(){
  int i = 0;    // the iterator counter.. not very great..
  ProtocolStepWidget* it; //= firstStep;
  for(it = firstStep; it != 0; it = it->next()){     // i.e. iterate until we get a 0.. 
    if(it->isActive()){
      // make a new ProtocolStepWidget..
      ProtocolStepWidget* step = new ProtocolStepWidget(this);
      connect(step, SIGNAL(sizeHintChanged(ProtocolStepWidget*)), this, SLOT(childChanged(ProtocolStepWidget*))); 
      steps.push_back(step);
      step->setAfter(it->next());
      it->setAfter(step);
      arrangeSteps(i+1);
      return;
    }
    i++;
  }
  // if here
  cerr << "No active steps... buggerthat ehh.. " << endl;
}

void ProtocolViewer::pasteStepAfterCurrent(){
  if(!clipboard->haveStep()){
    return;
  }
  int i=0;
  ProtocolStepWidget* it;
  for(it = firstStep; it != 0; it = it->next()){     // i.e. iterate until we get a 0.. 
    if(it->isActive()){
      // make a new ProtocolStepWidget..
      ProtocolStepWidget* step = new ProtocolStepWidget(this);
      connect(step, SIGNAL(sizeHintChanged(ProtocolStepWidget*)), this, SLOT(childChanged(ProtocolStepWidget*))); 
      steps.push_back(step);
      step->setStep(clipboard->step(), 0);  // this way we can keep the copy
      step->setAfter(it->next());
      it->setAfter(step);
      arrangeSteps(i+1);
      return;
    }
    i++;
  }
  // if here
  cerr << "No active steps... buggerthat ehh.. " << endl;
}

void ProtocolViewer::pasteBeginning(){
  if(!clipboard->haveStep()){
    return;
  }
  ProtocolStepWidget* step = new ProtocolStepWidget(this);
  connect(step, SIGNAL(sizeHintChanged(ProtocolStepWidget*)), this, SLOT(childChanged(ProtocolStepWidget*))); 
  steps.push_back(step);
  step->setStep(clipboard->step(), 0);  // this way we can keep the copy
  // put it at the beginning..
  step->setAfter(firstStep);
  firstStep = step;
  arrangeSteps(0);
}

void ProtocolViewer::pasteEnd(){
  if(!clipboard->haveStep()){
    return;
  }
  ProtocolStepWidget* step = new ProtocolStepWidget(this);
  connect(step, SIGNAL(sizeHintChanged(ProtocolStepWidget*)), this, SLOT(childChanged(ProtocolStepWidget*))); 
  steps.push_back(step);
  step->setStep(clipboard->step(), 0);  
  // and we need to find the last step.. 
  ProtocolStepWidget* it;
  ProtocolStepWidget* lastStep = 0;
  int i=0;
  for(it = firstStep; it != 0; it = it->next()){  
    lastStep = it;
    i++;
  }
  if(!lastStep){            // i.e. there are no steps..
    firstStep = step;
    arrangeSteps(0);
    return;
  }
  // otherwise, the last step is just equal to the last step..
  lastStep->setAfter(step);
  arrangeSteps(i);
}
  

void ProtocolViewer::childChanged(ProtocolStepWidget* step){
  // maybe I can just activate the grid and update the geometry or something..
  //cout << "grid sizeHint is " << grid->sizeHint().height() << endl;
  //grid->activate();
  //cout << "grid sizeHint is " << grid->sizeHint().height() << endl;
  //setMinimumHeight(grid->sizeHint().height());
  //for(int i=0; i < steps.size(); i++){
  //  steps[i]->update();
  //}
  //update();
  step->resize(step->sizeHint());
  //step->setMinimumHeight(step->sizeHint().height());
  //  cout << "height of widget is " << height() << endl;
  //cout << "height of the sizeHint is " << layout()->sizeHint().height() << endl;
  layout()->activate();
  //cout << "height of the sizeHint is " << layout()->sizeHint().height() << endl;
  updateGeometry();
  //resize(layout()->sizeHint());
  setMinimumHeight(layout()->sizeHint().height());
  // make sure that it is visible..
  // by telling the world where its lower left corner is..
  //emit keepVisible(mapToGlobal(QPoint(step->x(), step->y()+step->height())));
  emit IGrew();    // to make sure we can reposition nicely.. 
}

QSize ProtocolViewer::sizeHint() const
{
  return(layout()->sizeHint());
}

  //updateGeometry();  // 
  //}
void ProtocolViewer::resizeEvent(QResizeEvent* e){
  //cout << "viewer resize event " << endl;
  for(int i=0; i < steps.size(); i++){
    steps[i]->resize(steps[i]->sizeHint());
  }
  layout()->activate();
}

void ProtocolViewer::commitToDB(string userName){
  // if we have a protocol* then we will inherit from that protocol...
  vector<ProtocolStep*> pSteps;    // the actual steps.. make these from the thingy..
  ProtocolStepWidget* it; //= firstStep;
  for(it = firstStep; it != 0; it = it->next()){
    /////////  ahh for the ugly hacks.. why did I ever think of using pointers for this
    ProtocolStep* tstep = new ProtocolStep();
    *tstep = it->theStep(userId, userName);    // copy.. maybe I can do this.. 
    pSteps.push_back(tstep);
    //    pSteps.push_back(it->theStep(userId, userName));
  }
  ////////////  I SUSPECT THAT THIS WILL LEAD TO A MEMORY LEAK AS THE POINTERS IN THE
  ///////////   OLD VECTOR WILL NOT END UP GETTING DELETED.. PLEASE CHANGE THESE STRUCTURES
  ///////////   TO SOMETHING THAT IS A LITTLE BIT MORE REASONABLE.... 
  /// andooh.. 
  protocol->steps = pSteps;   // arghh.. not good..
}

void ProtocolViewer::cutStep(){
  // find the active step, 
  // don't delete it,, -as that would be troublesome with the steps in avector,, 
  // but change the links such that it won't be shown..
  int i = 0;    // the iterator counter.. not very great..
  ProtocolStepWidget* it; //= firstStep;
  ProtocolStepWidget* lastStep = 0;    // needed for the rearrangments.. 
  for(it = firstStep; it != 0; it = it->next()){     // i.e. iterate until we get a 0.. 
    if(it->isActive()){
      // make a new ProtocolStepWidget..
      //copiedStep = it;
      clipboard->setStep(it->theStep());    // makes a local copy.. 
      //connect(step, SIGNAL(sizeHintChanged(ProtocolStepWidget*)), this, SLOT(childChanged(ProtocolStepWidget*))); 
      // -do we need to disconnect ?? 
      if(!lastStep){
	firstStep = it->next();
      }else{
	lastStep->setAfter(it->next());
      }
      it->setAfter(0);    // we don't know where it should go.. 
      grid->remove(it);
      it->hide();
      arrangeSteps(i);  // this might be i, not i+1, we'll find out.. !
      emit copiedAStep();
      return;
    }
    lastStep = it;
    i++;
  }
  // if here
  cerr << "No active steps... buggerthat ehh.. " << endl;
}

void ProtocolViewer::copyStep(){
  int i = 0;    // the iterator counter.. not very great..
  ProtocolStepWidget* it; //= firstStep;
  for(it = firstStep; it != 0; it = it->next()){     // i.e. iterate until we get a 0.. 
    if(it->isActive()){
      // make a new ProtocolStepWidget..
      //ProtocolStepWidget* step = new ProtocolStepWidget(this);
      //step->setStep(it->theStep(), 0);;       // should copy it... 
      //step->setAfter(0);  // but it doesn't 
      /// and we should remember it..
      clipboard->setStep(it->theStep());
      //steps.push_back(step);
      //connect(step, SIGNAL(sizeHintChanged(ProtocolStepWidget*)), this, SLOT(childChanged(ProtocolStepWidget*))); 
      //copiedStep = step;
      emit copiedAStep();
      return;
    }
    i++;
  }
}

