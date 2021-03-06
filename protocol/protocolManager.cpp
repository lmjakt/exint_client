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

#include "protocolManager.h"
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <iostream>

using namespace std;

ProtocolManager::ProtocolManager(const int* myId, map<int, userInformation>* uInfo, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  collection = 0;      // we don't have one yet.. 
  setCaption("Protocols");
  userId = myId;
  userData = uInfo;
  listView = new QListView(this, "listView");
  listView->setRootIsDecorated(true);
  listView->addColumn("Protocol");
  connect(listView, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(selectionChanged(QListViewItem*)) );
  connect(listView, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), 
	  this, SLOT(popMenu(QListViewItem*, const QPoint&, int)) );
  requestId = 0;

  int indent = 5;
  QLabel* desLabel = new QLabel("Protocol Description", this, "desLabel");
  descriptionView = new QTextEdit(this, "descriptionView");
  descriptionView->setReadOnly(true);
  descriptionView->setMinimumWidth(300);
  creatorLabel = new QLabel("Creator Name", this, "creatorLabel");
  creatorLabel->setIndent(indent);
  timeLabel = new QLabel("Creation Time", this, "timeLabel");
  timeLabel->setIndent(indent);

  selectedProtocol = 0;

  loadMenu = new QPopupMenu(this, "loadMenu");
  loadMenu->insertItem("Load/Inherit", this, SLOT(loadProtocol()) );
  updateMenu = new QPopupMenu(this, "updateMenu");
  updateMenu->insertItem("Update", this, SLOT(requestCollection()) );
  updateMenu->insertItem("New Protocol", this, SLOT(newProtocol()) );
  
  //QPushButton* updateButton = new QPushButton("update", this, "updateButton");
  //connect(updateButton, SIGNAL(clicked()), this, SLOT(requestCollection()) );
  
  //QPushButton* newButton = new QPushButton("New Protocol", this, "newButton");
  //connect(newButton, SIGNAL(clicked()), this, SLOT(newProtocol()) );

  //QPushButton* loadButton = new QPushButton("load/inherit", this, "loadButton");
  //connect(loadButton, SIGNAL(clicked()), this, SLOT(loadProtocol()) );

  QHBoxLayout* hbox = new QHBoxLayout(this, 1, 2);
  QVBoxLayout* vbox = new QVBoxLayout();
  hbox->addLayout(vbox);
  vbox->addWidget(listView);
  QHBoxLayout* bbox = new QHBoxLayout();
  vbox->addLayout(bbox);
  //bbox->addWidget(updateButton);
  //bbox->addWidget(newButton);
  //vbox->addWidget(loadButton);

  rbox = new QVBoxLayout(hbox, 2);
  //  hbox->addLayout(rbox);
  rbox->addWidget(desLabel);
  rbox->addWidget(descriptionView);
  
  rbox->addWidget(creatorLabel);
  rbox->addWidget(timeLabel);
  //rbox->addWidget(loadButton);
  
  hbox->setStretchFactor(vbox, 3);
  hbox->setStretchFactor(rbox, 1);
  // which is all for now.. later we'll need to implement a million different things..
}

void ProtocolManager::setProtocolCollection(int reqid, ProtocolCollection* coll){
  cout << "Receiving New Protocol Collection " << endl;
//   if(reqid != requestId){
//     cerr << "wrong request id " << endl;
//     delete coll;
//     return;
//   }
  if(collection){
    //    cout << "trying to delete : " << endl;
    // note that we should probably empty the map first.. and delete the QListViewItems
    map<QListViewItem*, Protocol*>::iterator it;
    while(protocolMap.size()){
      it = protocolMap.begin();
      //    for(it = protocolMap.begin(); it != protocolMap.end(); it++){
      //cout << "\ta qlistviewitem : "; 
      QListViewItem* temp = (*it).first;
      cout << "  --" << temp->text(0) << endl;
      protocolMap.erase(it);
      //cout << "\t\terased " << endl;
      //delete temp;
    }
    cout << "emptied the protocolMap.. " << endl;
    QListViewItem* temp;
    while( (temp = listView->firstChild()) ){
      //      cout << "\t\t\tdeleteting a qlistviewitem " << endl;
      delete temp;
    }
    //    cout << "trying to delete collection : " << endl;
    collection->refcount--;
    cout << "collection refcout is : " << collection->refcount << endl;
    if(collection->refcount < 1){
      delete collection;
    }
  }
  //  cout << "deleteed stuff " << endl;
  collection = coll;
  //cout << "calling on refcount to increase : " << collection->refcount << endl;
  collection->refcount++;
  cout << "refcount increased and is now : " << collection->refcount << endl;
  //// and now.. we parse the selection and see how it works.. I don't remember exactly how but,,hmmm we have to follow the tree..
  // - start with the root protocol and find all the children, what is the best way.. hmm. 
  //  parsing the tree should'nt be a problem, but.. to do so naturally..
  //  if the protocol->nextDaughter function doesn't have a next daughter it returns the parent..
  //  hence I can traverse the whole tree just by calling this function..
  // first make sure all of the pointers are set to 0..
  for(map<int, Protocol*>::iterator it=collection->protocols.begin(); it != collection->protocols.end(); it++){
    (*it).second->pointer = 0;
    (*it).second->currentDaughter = 0;   // otherwise this fails if its been parsed by someone else.. 
  }
  // better safe than sorry as they say.. .. !
  Protocol* prot = collection->rootProtocol;
  //  Protocol* prot = collection->rootProtocol->nextDaughter();
  // here make a new top level type of protocol.. -i.e. a type or something like that..
  cout << "about to parse the tree, and see how that goes " << endl;
  while( (prot = prot->nextDaughter()) ){     //  
    //cout << "got to the first protocol, and the name is : " << prot->name << endl;
    if(prot == collection->rootProtocol){     // back at the top do nothing... 
      continue;
    }
    if(prot->parent == collection->rootProtocol){     // consider this as a top level protocol..
      //cout << "this is obviusly a top level protocol, ok, so let's be careful here : " << endl;
      // it is possible that we have been here before,, so check the useful void pointer to find out..
      if(!prot->pointer){   // then make a new one.. 
	//cout << "and the pointer doesn't apper to be defined : " << endl; 
	cout << "parent is listview making qlistview item with name of " << prot->name.c_str()  << endl; 
	QListViewItem* item = new QListViewItem(listView, prot->name.c_str());
	//cout << "address of item is : " << (int)item << endl;
	prot->pointer = (void*)item;
	protocolMap.insert(make_pair(item, prot));
      }
      continue;
    }
    if(!prot->pointer){
      //cout << "protocol isn't top level but has an inheritance of some sort " << endl;
      //cout << "and the parents pointer is : " << (int)prot->parent->pointer << endl;
      cout << "making a daughter qlistview item with name of " << prot->name.c_str()  << endl; 
      QListViewItem* item = new QListViewItem((QListViewItem*)prot->parent->pointer, prot->name.c_str());
      prot->pointer = (void*)item;
      protocolMap.insert(make_pair(item, prot));
    }
    // make a widget inheriting from the father of the above protocol.. -- but how to know..
  }
  // when we get here prot = 0.. which isn't really what we want.. 
  // ok let's go through these guys and check which ones should be open and which ones we can leave.. 
  QListViewItemIterator it(listView);
  while(it.current() ){
    QListViewItem* item = it.current();
    ++it;
    //cout << "Checking open state for item with text : " << item->text(0) << endl;
    // find the protocol id associated with this..
    map<QListViewItem*, Protocol*>::iterator pit = protocolMap.find(item);
    if(pit != protocolMap.end()){
      //cout << "\tfound a Protocol associated with this list view item id : " << (*pit).second->id << endl;
      map<int, bool>::iterator bit = openItem.find((*pit).second->id);
      if(bit != openItem.end()){
	//cout << "\t found an open state associated with this and it is : " << (*bit).second << endl;
	item->setOpen((*bit).second);
      }
    }
  }
  // ok, that's pretty ugly, and could be done much nicer with inheritance, but.. 
}
  
void ProtocolManager::addProtocol(Protocol* prot){
  if(prot->requestId){
    cout << "protocol has a request id looking for a holder which is :" << prot->requestId << endl;
    set<ProtocolHolder*>::iterator it;
    for(it = protocols.begin(); it != protocols.end(); it++){
	if((*it)->theProtocol() == (Protocol*)prot->requestId){   /// SERIOUS ERROR ON 64 BIT MACHINES /// 
	cout << "found a protocol holder ! : " << endl;
	(*it)->setProtocol(prot);
	// if this is the case then it makes sense for us to update the protocol collection as well.. 
	requestCollection();
	return;
      }
    }
  }
  ProtocolHolder* holder = new ProtocolHolder(*userId, userData, &clipboard);
  holder->setPalette(palette());
  connect(holder, SIGNAL(commitProtocolToDB(Protocol*)), this, SLOT(commitProtocol(Protocol*)) );
  holder->setProtocol(prot);
  holder->show();
  holder->resize(650, 400);
  protocols.insert(holder); 
  // but probably get rid of the vector, and just have this work by destructive closing of windows, or something like that.. 
  // deleting things in a vector is not very pretty, and really we don't need to remember these...
}

void ProtocolManager::requestCollection(){
  // iterate through the qlistviewitems and work out which ones are open or not..
  QListViewItemIterator it(listView);
  while(it.current() ){
    QListViewItem* item = it.current();
    cout << "checking item with text for open state : " << item->text(0) << endl;
    ++it;
    // find the protocol id associated with this..
    map<QListViewItem*, Protocol*>::iterator pit = protocolMap.find(item);
    if(pit != protocolMap.end()){
      cout << "\t\t and open state is : " << item->isOpen() << "   for protocol id : " << (*pit).second->id << endl;
      openItem[(*pit).second->id] = item->isOpen();
      //      openItem.insert(make_pair((*pit).second->id, item->isOpen()));
    }
  }
  emit getCollection(++requestId);
}

void ProtocolManager::requestProtocol(){
  // find out the number.. maybe we will have a thingy here.. 
}

void ProtocolManager::newProtocol(){
  ProtocolHolder* holder = new ProtocolHolder(*userId, userData, &clipboard);
  holder->setPalette(palette());
  connect(holder, SIGNAL(commitProtocolToDB(Protocol*)), this, SLOT(commitProtocol(Protocol*)) );
  holder->makeNewProtocol(1);    // makes a completely (rootless protocol,, -- will inherit from 1).. 
  holder->show();
  holder->resize(400, 400);
  protocols.insert(holder);
}
  
void ProtocolManager::commitProtocol(Protocol* protocol){
  cout << "Protocol manager emitting a protocol " << endl;
  emit commitProtocolToDB(protocol);
}

void ProtocolManager::selectionChanged(QListViewItem* item){
  map<QListViewItem*, Protocol*>::iterator it = protocolMap.find(item);
  if(it != protocolMap.end()){
    selectedProtocol = (*it).second;
    descriptionView->setText(selectedProtocol->description.c_str());
    QString name = "Creator Name :";
    name += selectedProtocol->creatorName.c_str();
    creatorLabel->setText(name);
    QString date = "Creation Time : ";
    date += selectedProtocol->creationTime.toString("ddd dd MMM yy hh:mm:ss");
    timeLabel->setText(date);
  }else{
    selectedProtocol = 0;
  }
}
    
void ProtocolManager::loadProtocol(){
  if(!selectedProtocol){
    return;
  }
  cout << "emitting get protocol : " << selectedProtocol->id << endl; 
  emit getProtocol(selectedProtocol->id);
 //  ProtocolHolder* holder = new ProtocolHolder(*userId, userData);
//   connect(holder, SIGNAL(commitProtocolToDB(Protocol*)), this, SLOT(commitProtocol(Protocol*)) );
//   holder->setProtocol(selectedProtocol);
//   holder->show();
}

void ProtocolManager::popMenu(QListViewItem* item, const QPoint& point, int i){
  if(item == 0){
    updateMenu->popup(point);
    return;
  }
  // first just chek how this works..
  loadMenu->popup(point);
  i = i;
}
