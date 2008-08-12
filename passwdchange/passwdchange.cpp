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

#include "passwdchange.h"
#include <qstring.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <iostream>

using namespace std;

PasswdChange::PasswdChange(QWidget* parent, const char* name) :
  QWidget(parent, name)
{
  setCaption("Change Password");
  old = new QLineEdit(this, "old");
  old->setEchoMode(QLineEdit::Password);
  QLabel* oldL = new QLabel("Old Password", this);
  new1 = new QLineEdit(this, "new1");
  new1->setEchoMode(QLineEdit::Password);
  QLabel* new1L = new QLabel("New Password", this);
  new2 = new QLineEdit(this, "new2");
  new2->setEchoMode(QLineEdit::Password);
  QLabel* new2L = new QLabel("Repeat New", this);

  QPushButton* acceptButton = new QPushButton("Change", this, "change");
  connect(acceptButton, SIGNAL(clicked()), this, SLOT(change()) );
  QPushButton* cancelButton = new QPushButton("Cancel", this, "cancel");
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()) );

  QPushButton* newUserButton = new QPushButton("New User", this, "newUser");
  connect(newUserButton, SIGNAL(clicked()), this, SLOT(newUser()) );
  newUserLine = new QLineEdit(this, "newUserLine");
  

  QGridLayout* grid = new QGridLayout(this, 11, 5);

  grid->addRowSpacing(0, 30);
  grid->addRowSpacing(2, 5);
  grid->addRowSpacing(4, 5);
  grid->addRowSpacing(6, 5);
  grid->addRowSpacing(8, 5);
  grid->addRowSpacing(10, 20);    // the bottom
  
  grid->addColSpacing(0, 15);
  grid->addColSpacing(2, 5);
  grid->addColSpacing(4, 15);

  // and now the qline edits and the buttons.. 
  grid->addWidget(oldL, 1, 1);
  grid->addWidget(old, 1, 3);
  grid->addWidget(new1L, 3, 1);
  grid->addWidget(new1, 3, 3);
  grid->addWidget(new2L, 5, 1);
  grid->addWidget(new2, 5, 3);
  grid->addWidget(acceptButton, 7, 1);
  grid->addWidget(cancelButton, 7, 3);

  grid->addWidget(newUserButton, 9, 1);
  grid->addWidget(newUserLine, 9, 3);
}

void PasswdChange::passwordChanged(bool ok){
  // if OK, then everything ok,, clear everything..
  if(!ok){
    QMessageBox::information(this, "Password Change", "Failed to Update passwords, try again, if you feel lucky");
  }
  old->clear();
  new1->clear();
  new2->clear();
  newUserLine->clear();
  if(ok){
    close();
  }
}

void PasswdChange::passWordError(){
  QMessageBox::information(this, "Password Change", "Failed to Update passwords, try again, if you feel lucky");
  old->clear();
  new1->clear();
  new2->clear();
  newUserLine->clear();
}
void PasswdChange::change(){
  // read the entries, and emit them, close the window..maybe..
  // or maybe only close the window on confirm.. or something.
  cout << "In the change function" << endl;
  QString oldp = old->text();
  QString new1p = new1->text();
  QString new2p = new2->text();
  if(new1p != new2p){
    QMessageBox::information(this, "Password Change", "New Passords are not the same.\nPlease Try again");
  }else{
    emit changePasswd(oldp, new1p);
  }
  // and clear the boxes anyway..
  old->clear();
  new1->clear();
  new2->clear();
  newUserLine->clear();
}

void PasswdChange::cancel(){
  old->clear();
  new1->clear();
  new2->clear();
  newUserLine->clear();
  close();
  // clear all the entries, and close the window..
}

void PasswdChange::newUser(){
  QString oldp = old->text();
  QString new1p = new1->text();
  QString new2p = new2->text();
  QString newUserName = newUserLine->text();
  if(new1p != new2p){
    QMessageBox::information(this, "New User", "New Passords are not the same.\nPlease Try again");
  }else{
    emit createNewUser(oldp, new1p, newUserName);
  }
  old->clear();
  new1->clear();
  new2->clear();
  newUserLine->clear();
}
