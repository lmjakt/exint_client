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

#include "protocolHolder.h"
#include "../dataStructs.h"
#include <qtextedit.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <iostream>
#include <qmessagebox.h>

using namespace std;

ProtocolHolder::ProtocolHolder(int myId, map<int, userInformation>* uInfo, ProtocolClipboard* clpboard, QWidget* parent, const char* name)
  : QWidget(parent, name, Qt::WDestructiveClose)
{
  setCaption("Protocol Editor");
  userId = myId;
  userData = uInfo;
  clipboard = clpboard;
  connect(clipboard, SIGNAL(gotStep()), this, SLOT(activatePaste()) );
  // see if we exist..
  cout << "My userId is : " << userId << endl;
  map<int, userInformation>::iterator it = userData->find(myId);
  if(it != userData->end()){
    userName = (*it).second.userName;
  }else{
    userName = "unknown";
  }
  
  protocol = 0;
  scroll = new QScrollView(this);
  scroll->setVScrollBarMode(QScrollView::AlwaysOn);
  viewer = new ProtocolViewer(myId, clipboard, scroll->viewport(), "viewer");
  connect(viewer, SIGNAL(IGrew()), this, SLOT(childGrew()) );
  //connect(viewer, SIGNAL(copiedAStep()), this, SLOT(activatePaste()) );
  scroll->addChild(viewer);
  connect(scroll, SIGNAL(contentsMoving(int, int)), this, SLOT(contentsMoved(int, int)));

  /// some meta information..
  int indent = 5;
  nameLine = new QLineEdit(this, "nameLine");
  descriptionArea = new QTextEdit(this, "descriptionArea");
  descriptionArea->setMinimumWidth(300);
  creatorLabel = new QLabel("Creator : ", this, "creatorLabel");
  timeLabel = new QLabel("Creation Time: ", this, "timeLabel");
  creatorLabel->setIndent(indent);
  timeLabel->setIndent(indent);
  //nameLine->setMargin(indent);
  // Some basic labels..
//  int labelHeight = 15;
  QLabel* nameLabel = new QLabel("Protocol Name", this, "nameLabel");
  nameLabel->setMargin(3);
  //  nameLabel->setMinimumHeight(labelHeight);
  QLabel* descriptionLabel = new QLabel("Protocol Description", this, "descriptionLabel");
  QLabel* stepsLabel = new QLabel("Protocol Steps", this, "stepsLabel");
  //stepsLabel->setMinimumHeight(labelHeight);
  stepsLabel->setMargin(3);
  nameLabel->setIndent(indent);
  descriptionLabel->setIndent(indent);
  stepsLabel->setIndent(indent);

  /// some things for adding steps or removing or otherwise..
  QLabel* addStepLabel = new QLabel("New step", this, "addStepLabel");
  QLabel* pasteLabel = new QLabel("Paste step", this, "pasteLabel");
  addStepLabel->setIndent(indent);
  QPushButton* addAfterButton = new QPushButton("After Current", this, "addBeforeButton");
  connect(addAfterButton, SIGNAL(clicked()), viewer, SLOT(addStepAfterCurrent()) );
  QPushButton* addEndButton = new QPushButton("At End", this, "addEndButton");
  connect(addEndButton, SIGNAL(clicked()), viewer, SLOT(addStep()) );
  QPushButton* addBegButton = new QPushButton("At Top", this, "addBegButton");
  connect(addBegButton, SIGNAL(clicked()), viewer, SLOT(prependStep()) );
  /// some things for pasting.. 
  QPushButton* copyCurrent = new QPushButton("Copy Current Step", this, "copyCurrent");
  connect(copyCurrent, SIGNAL(clicked()), viewer, SLOT(copyStep()) );
  QPushButton* cutCurrent = new QPushButton("Cut Current Step", this, "cutCurrent");
  connect(cutCurrent, SIGNAL(clicked()), viewer, SLOT(cutStep()) );


  pasteStep = new QPushButton("After Current", this, "pasteStep");    // ugly,, 
  pasteStep->setDisabled(!clipboard->haveStep());
  connect(pasteStep, SIGNAL(clicked()), viewer, SLOT(pasteStepAfterCurrent()) );
  pasteEnd = new QPushButton("At End", this, "pasteEnd");
  pasteEnd->setDisabled(!clipboard->haveStep());
  connect(pasteEnd, SIGNAL(clicked()), viewer, SLOT(pasteEnd()) );
  pasteTop = new QPushButton("At Top", this, "pasteTop");
  pasteTop->setDisabled(!clipboard->haveStep());
  connect(pasteTop, SIGNAL(clicked()), viewer, SLOT(pasteBeginning()) );
  
  // -- later we can improve this by subclassing the QTextEdit, and remove some of these, and make sure
  //    that appropriate functions are avaialble at the right time.. 
  commitButton = new QPushButton("Commit to DataBase", this, "commitButton");
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commitToDB()) );  // do it with the viewer.. 

  //QVBoxLayout* ibox = new QVBoxLayout(scroll->viewport());
  //ibox->addWidget(viewer);
  QHBoxLayout* hbox = new QHBoxLayout(this, 0, 3);
  QVBoxLayout* lbox = new QVBoxLayout();
  QVBoxLayout* rbox = new QVBoxLayout();
  hbox->addLayout(lbox);
  hbox->addLayout(rbox);
  lbox->addWidget(nameLabel);
  lbox->addWidget(nameLine);
  lbox->addWidget(descriptionLabel);
  lbox->addWidget(descriptionArea);
  lbox->addWidget(creatorLabel);
  lbox->addWidget(timeLabel);
  lbox->addStretch();
  QGridLayout* buttonGrid = new QGridLayout();
  lbox->addLayout(buttonGrid);
  buttonGrid->addWidget(addStepLabel, 0, 0);
  buttonGrid->addWidget(pasteLabel, 0, 1);
  buttonGrid->addWidget(addAfterButton, 1, 0);
  buttonGrid->addWidget(addEndButton, 2, 0);
  buttonGrid->addWidget(addBegButton, 3, 0);
  buttonGrid->addWidget(pasteStep, 1, 1);
  buttonGrid->addWidget(pasteEnd, 2, 1);
  buttonGrid->addWidget(pasteTop, 3, 1);

  buttonGrid->addRowSpacing(4, 5);
  buttonGrid->addWidget(copyCurrent, 5, 0);
  buttonGrid->addWidget(cutCurrent, 5, 1);
  lbox->addWidget(commitButton);

  rbox->addWidget(stepsLabel);
  rbox->addWidget(scroll);
  bottomDistance = viewer->height() - scroll->viewport()->height();
  hbox->setStretchFactor(rbox, 3);
  hbox->setStretchFactor(lbox, 1);
  // resize(300, 200);
  //viewer->resize(scroll->viewport()->size());
}

ProtocolHolder::~ProtocolHolder(){
  if(protocol){
    delete protocol;
    protocol = 0;
  }
}

void ProtocolHolder::setProtocol(Protocol* prot){
  protocol = prot;
  viewer->setProtocol(protocol);
  nameLine->setText(protocol->name.c_str());
  descriptionArea->setText(protocol->description.c_str());
  QString ulabel = "Creator Name : ";
  ulabel.append(protocol->creatorName.c_str());
  creatorLabel->setText(ulabel);
  // and we need to format the time.. 
  QString time = protocol->creationTime.toString("ddd dd MMM yy hh:mm:ss");
  time.prepend("Creation Time: ");
  timeLabel->setText(time);
  commitButton->setDisabled(false);
}

void ProtocolHolder::makeNewProtocol(int parentId){
  if(protocol){
    delete protocol;
    protocol = 0;     // what happens to the steps here... hmm.. 
  }
  protocol = new Protocol(0, parentId, 0, userId, userName, "protocol_name", "protocol_description");  //
  nameLine->setText("New Protocol");
  descriptionArea->clear();
  QString ulabel = "Creator Name : ";
  ulabel.append(userName.c_str());
  creatorLabel->setText(ulabel);
  viewer->setProtocol(protocol);
}

void ProtocolHolder::resizeEvent(QResizeEvent* e){
  cout << "scroll visible width is : " << scroll->visibleWidth() << endl
       << "               height is : " << scroll->visibleHeight() << endl;
  cout << "scroll viewport height is : " << scroll->viewport()->height() << endl;
  
  viewer->setFixedWidth(scroll->viewport()->width());
  viewer->layout()->activate(); // hmm. 
  viewer->updateGeometry();   // ??? 
  cout << "viewer size hint height is : " << viewer->sizeHint().height() << endl;
  if(viewer->sizeHint().height() > scroll->viewport()->height()){
    cout << "\t resizing to size hint " << endl;
    viewer->resize(viewer->sizeHint());
  }else{
    //    viewer->resize(scroll->viewport()->size());
    viewer->setFixedHeight(scroll->visibleHeight());
    //    scroll->resizeContents(scroll->visibleWidth(), scroll->visibleHeight());
    //    viewer->resize(scroll->visibleWidth(), scroll->visibleHeight());
    cout << "\t resizint to viewport size " << endl;
  }
}
// and that's it,, no way man,, it won't work at all.. 

void ProtocolHolder::contentsMoved(int x, int y){
  bottomDistance = viewer->height() - scroll->viewport()->height() - y;
  x = y;
  cout << "contents moved, bottom distance is now : " << bottomDistance << endl;
}

void ProtocolHolder::childGrew(){
  // basically try to keep the same distance to the bottom as we had before.. -- how do I know what that was..
  // hmmmm.... 
  if(viewer->sizeHint().height() < scroll->visibleHeight()){
    viewer->setFixedHeight(scroll->visibleHeight());
    return;
  }
  cout << "bottom Distance was " << bottomDistance << endl
       << "but now is :        " << viewer->height() - scroll->viewport()->height() - scroll->contentsY() << endl;
  int delta = viewer->height() - scroll->viewport()->height() - scroll->contentsY() - bottomDistance;
  if(delta >= 0){
    //  if(bottomDistance >= 0){
    scroll->scrollBy(0, delta);
  }
}

void ProtocolHolder::commitToDB(){
  cout << "calling viewer to commit to DB" << endl;
  bool commit = true;
  switch( QMessageBox::information(this, "Client", 
				   "Do you really want to commit the changes to the Database\n"
				   "Note that you cannot edit this protocol once commited, you can only inherit from it\n"
				   "If you don't understand this concept, please have a conversation with your administrator",
				   "Commit", "Cancel", 0, 1)){
  case 0 :
    cout << "The protocol will be commited.. " << endl;
    break;
  case 1:
    cout << "The user doesn't want to commit this, need to have some other swithc as I can't return from here" << endl;
    commit = false;
    break;
  default :
    cout << "what the hell " << endl;
  }
  if(!commit){
    return;
  }

  viewer->commitToDB(userName);
  // and set the things that we know from here... arghh.. 
  protocol->parentId = protocol->id;    // hmm
  protocol->id = 0;           // assume it is a new protocol..
  protocol->creatorId = userId;
  protocol->creatorName = userName;
  protocol->name = nameLine->text().latin1();
  protocol->description = descriptionArea->text().latin1();
  // and emit some signal.. or other..
  cout << "and edited the protocol to update stuff I know about " << endl;
  emit commitProtocolToDB(protocol);
  cout << "emitted protocol for thingy " << endl;
  commitButton->setDisabled(true);
}

void ProtocolHolder::activatePaste(){
  pasteStep->setDisabled(false);
  pasteEnd->setDisabled(false);
  pasteTop->setDisabled(false);
}

// void ProtocolHolder::makeVisible(QPoint p){
//   // note this QPoint is a global parameter.. which we need to map the to
//   // the parameters used by the 
//   cout << "trying to make sure something is visible" << endl;
//   QPoint p2 = scroll->mapFromGlobal(p);   // ??? 
//   cout << "p x : " << p2.x() << "  and y : " << p2.y() << endl;
//   scroll->ensureVisible(p2.x(), p2.y());   // ??? 
// }
