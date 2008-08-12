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

#include "logindialog.h"
#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>

using namespace std;

LoginDialog::LoginDialog(QString* lName, QString* lPasswd, QString* serverName, QString* port, QWidget* parent, const char* name) :
  QDialog(parent, name, TRUE)
{
  // some line Edits, and stuff
  
  uname = lName;
  passwd = lPasswd;
  server = serverName;
  prt = port;
  
  nameLine = new QLineEdit(this, "name");
  passwdLine = new QLineEdit(this, "passwd");
  passwdLine->setEchoMode(QLineEdit::Password);
  serverLine = new QLineEdit(this, "server");
  portLine = new QLineEdit(this, "portLine");

  serverLine->setText(*serverName);
  portLine->setText(*port);

  QLabel* nameLabel = new QLabel("User Name", this);
  QLabel* passwdLabel = new QLabel("Password", this);
  QLabel* serverLabel = new QLabel("Server", this);
  QLabel* portLabel = new QLabel("Port", this);

  QPushButton* accept = new QPushButton("Accept", this, "accept");
  connect(accept, SIGNAL(clicked()), this, SLOT(acceptChoices()));
  QPushButton* cancel = new QPushButton("Cancel", this, "cancel");
  connect(cancel, SIGNAL(clicked()), this, SLOT(cancel()));
  
  QGridLayout* vBox = new QGridLayout(this, 9, 5);
  
  vBox->addRowSpacing(0, 25);
  vBox->addColSpacing(0, 15);
  vBox->addColSpacing(4, 15);
  vBox->addColSpacing(2, 5);     // between the label and the thing.. 
  vBox->addRowSpacing(2, 5);
  vBox->addRowSpacing(4, 5);      //
  vBox->addRowSpacing(6, 5);
  vBox->addRowSpacing(8, 25);      // the margins for the box,, 
  
  vBox->addWidget(nameLabel, 1, 1);
  vBox->addWidget(nameLine, 1, 3);

  vBox->addWidget(passwdLabel, 3, 1);
  vBox->addWidget(passwdLine, 3, 3);
  
  vBox->addWidget(serverLabel, 5, 1);
  vBox->addWidget(serverLine, 5, 3);

  vBox->addWidget(portLabel, 7, 1);
  vBox->addWidget(portLine, 7, 3);

  vBox->addWidget(accept, 8, 1);
  vBox->addWidget(cancel, 8, 3);

}

void LoginDialog::acceptChoices(){
  // 
  // set the Values of lName, lPasswd, and serverName,,
  *uname = nameLine->text();
  *passwd = passwdLine->text();
  if(serverLine->text().length()){
    *server = serverLine->text();
  }
  if(portLine->text().length()){
    *prt = portLine->text();
  }
  done(1);
}

void LoginDialog::cancel(){
  done(0);
}


