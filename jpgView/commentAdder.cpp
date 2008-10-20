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

#include "commentAdder.h"
#include "fieldCreator.h"
#include <qlayout.h>
#include <qlabel.h>

using namespace std;

CommentAdder::CommentAdder(vector<ishAnnotationField> flds, QWidget* parent, const char* name)
: QWidget(parent, name)
{
  fields = flds;
  commentArea = new QTextView(this, "commentArea");
  commentArea->setReadOnly(false);
  fieldChooser = new QComboBox(false, this, "fieldChooser");
  fieldChooser->insertItem("-----------");
  // and then we get to inser some more.. 
  for(uint i=0; i < fields.size(); i++){
    fieldChooser->insertItem(fields[i].fieldName);
    fieldIndex.insert(make_pair(i+1, fields[i]));  // copy constrcuctor, but maybe that makes life easy.
  }
  FieldCreator* creator = new FieldCreator();
  connect(creator, SIGNAL(makeField(QString)), this, SIGNAL(makeField(QString)) );

  QPushButton* makeFieldButton = new QPushButton("New Field", this, "makeFieldButton");
  connect(makeFieldButton, SIGNAL(clicked()), creator, SLOT(createField()) );

  // a label above,, 
  QLabel* commentLabel = new QLabel("Comment Area", this, "commentLabel");

  // and now just the layout, if I'm not wrong..
  QVBoxLayout* vbox = new QVBoxLayout(this);
  vbox->addWidget(commentLabel);
  vbox->addWidget(commentArea);
  QHBoxLayout* hbox = new QHBoxLayout(vbox);
  hbox->addWidget(fieldChooser);
  hbox->addWidget(makeFieldButton);
}

int CommentAdder::currentIndex(){  // returns -1 if nothing selected..
  int currentSelected = fieldChooser->currentItem();
  map<int, ishAnnotationField>::iterator it = fieldIndex.find(currentSelected);
  if(it == fieldIndex.end()){
    return(-1);
  }
  return((*it).second.index);
}
 
QString CommentAdder::comment(){
  return(commentArea->text());
}

void CommentAdder::setIshAnnotationFields(vector<ishAnnotationField> flds){
  fields = flds;      // we probably don't need to store the vector of fields, but.. 
  fieldIndex.erase(fieldIndex.begin(), fieldIndex.end());
  fieldChooser->clear();
  fieldChooser->insertItem("-----------");
  for(uint i=0; i < fields.size(); i++){
    fieldChooser->insertItem(fields[i].fieldName);
    fieldIndex.insert(make_pair(i+1, fields[i]));      // hmm to sort implement < in the struct and sort before.. 
  }
}

void CommentAdder::clear(){
  commentArea->clear();
  fieldChooser->setCurrentItem(0);
}
