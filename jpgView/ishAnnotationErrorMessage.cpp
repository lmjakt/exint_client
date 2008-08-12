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

#include "ishAnnotationErrorMessage.h"
#include "ishFloatEntry.h"
#include "ishTextEntry.h"
#include "../dataStructs.h"
#include <qdialog.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qfile.h>             // just for the read write constants.. QFile:: 

using namespace std;    /// though I think I probably don't need this..

IshAnnotationErrorMessage::IshAnnotationErrorMessage(ishTextInsert ti, QWidget* parent, const char* name)
  : QDialog(parent, name, true, Qt::WDestructiveClose)
{
  QString errorMessage;
  QTextStream ts(&errorMessage, 2);
  // and now let's think of a suitable error message  
  ts << "One of your inserts didn't complete properly" << endl
     << "The error message returned : " << endl << ti.errorString << endl;
  ts << endl << "Field      : " << ti.fieldName << endl
     <<         "Annotation : " << ti.annotation << endl
     << endl << "Have a nice day!" << endl;
  // hmmm, now we have a trouble of sorts.. -- as we don't really know what possible fields are or whatever.. 
  // that's a bit of an issue.. -there's no real point in retrying without changing something, which is going 
  // to be a bit of an issue.. -if it's screwed up, then anyway we are screwed... -
  QTextEdit* editField = new QTextEdit(this, "editField");
  editField->setText(errorMessage);

  QPushButton* giveUpButton = new QPushButton("give it up", this, "giveUpButton");
  QPushButton* tryAgainButton = new QPushButton("Try again", this, "tryAgainButton");
  
  connect(giveUpButton, SIGNAL(clicked()), this, SLOT(giveUp()) );
  connect(tryAgainButton, SIGNAL(clicked()), this, SLOT(tryAgain()) );

  QGridLayout* grid = new QGridLayout(this, 2, 3, 5, 5);
  grid->addMultiCellWidget(editField, 0, 0, 0, 2);
  grid->addWidget(giveUpButton, 1, 0);
  grid->addWidget(tryAgainButton, 1, 2);
  resize(300, 200);
}

IshAnnotationErrorMessage::IshAnnotationErrorMessage(ishFloatInsert ti, bool classification, QWidget* parent, const char* name){
  QString errorMessage;
  QTextStream ts(&errorMessage, 2);
  // and now let's think of a suitable error message  
  ts << "One of your inserts didn't complete properly" << endl
     << "The error message returned : " << endl << ti.errorString << endl;
  ts << endl;
  if(classification){
    ts << "Class      : ";
  }else{
    ts << "Field      : ";
  }
  ts << ti.fieldName << endl
     <<         "Value  : " << ti.value << endl
     << endl << "Have a nice day!" << endl;
  // hmmm, now we have a trouble of sorts.. -- as we don't really know what possible fields are or whatever.. 
  // that's a bit of an issue.. -there's no real point in retrying without changing something, which is going 
  // to be a bit of an issue.. -if it's screwed up, then anyway we are screwed... -
  QTextEdit* editField = new QTextEdit(this, "editField");
  editField->setText(errorMessage);

  QPushButton* giveUpButton = new QPushButton("give it up", this, "giveUpButton");
  QPushButton* tryAgainButton = new QPushButton("Try again", this, "tryAgainButton");
  
  connect(giveUpButton, SIGNAL(clicked()), this, SLOT(giveUp()) );
  connect(tryAgainButton, SIGNAL(clicked()), this, SLOT(tryAgain()) );

  QGridLayout* grid = new QGridLayout(this, 2, 3, 5, 5);
  grid->addMultiCellWidget(editField, 0, 0, 0, 2);
  grid->addWidget(giveUpButton, 1, 0);
  grid->addWidget(tryAgainButton, 1, 2); 
  resize(300, 200);
}

void IshAnnotationErrorMessage::giveUp(){
  done(0);
}

void IshAnnotationErrorMessage::tryAgain(){
  done(1);
}


