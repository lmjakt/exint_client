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

#include "sessionWidget.h"
#include "../../smallToggle/smallToggle.h"
#include <qpushbutton.h>
#include <qwidget.h>
#include <qstring.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcolor.h>
#include <qfont.h>
#include <qsizepolicy.h>
#include <set>
#include <iostream>

using namespace std;

SessionWidget::SessionWidget(int id, set<int>& g, set<QString>& kwd, QString& ti, QString& des, bool ownership, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  sessionId = id;
  setMaximumHeight(20);
  //setFont(QFont("Arial", 8));  -- take the appliation font size.. 
  genes = g;
  keyWords = kwd;
  title = ti;
  description = des;
  
  readOnly = !ownership;
  QColor bgColor;
  if(readOnly){
    bgColor = QColor(175, 150, 150);
  }else{
    bgColor = QColor(255, 255, 255);
  }

  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  //and set up a couple of buttons and a QLabel, or two.. or something like that.. ho yeahh.
  //QLineEdit* titleLabel = new QLineEdit(title, this, "titleLabel");
  QLabel* titleLabel = new QLabel(title, this, "titleLabel");
  titleLabel->setPaletteBackgroundColor(bgColor);
  //  titleLabel->setPaletteBackgroundColor(QColor(255, 255, 255));
  titleLabel->setMargin(3);
  QString size;
  size.setNum(genes.size());
  //QLineEdit* sizeLabel = new QLineEdit(size, this, "sizeLabel");  // this one we'll need to set a small space for.. 
  QLabel* sizeLabel = new QLabel(size, this, "sizeLabel");  // this one we'll need to set a small space for.. 
  sizeLabel->setPaletteBackgroundColor(bgColor);
  //  sizeLabel->setPaletteBackgroundColor(QColor(255, 255, 255));
  sizeLabel->setFixedWidth(35);
  sizeLabel->setMargin(2);

  SmallToggle* includeButton = new SmallToggle(1, this, "include"); 
  connect(includeButton, SIGNAL(toggled(bool)), this, SIGNAL(includeMe(bool)) );
  includeButton->setFixedWidth(25);
  QPushButton* displayButton = new QPushButton("info", this, "info");
  connect(displayButton, SIGNAL(clicked()), this, SIGNAL(displayMe()) );
  displayButton->setFixedWidth(40);
  displayButton->setSizePolicy(sizePolicy);
  includeButton->setSizePolicy(sizePolicy);

  QHBoxLayout* hBox = new QHBoxLayout(this);  // could have some other numbers..
  
  hBox->addWidget(sizeLabel);
  hBox->addWidget(displayButton);
  hBox->addWidget(includeButton);
  hBox->addWidget(titleLabel);
}

SessionWidget::SessionWidget(QWidget* parent, const char* name){
  title = "NULL NULL NULL NULL";
  QString size = "0";
  description = "NULL";
  readOnly = true;
 
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  //and set up a couple of buttons and a QLabel, or two.. or something like that.. ho yeahh.
  QLabel* titleLabel = new QLabel(title, this, "titleLabel");
  QLabel* sizeLabel = new QLabel(size, this, "sizeLabel");  // this one we'll need to set a small space for.. 
  titleLabel->setPaletteBackgroundColor(QColor(255, 255, 255));
  sizeLabel->setPaletteBackgroundColor(QColor(255, 255, 255));
  titleLabel->setSizePolicy(sizePolicy);
  sizeLabel->setSizePolicy(sizePolicy);
  
  SmallToggle* includeButton = new SmallToggle(1, this, "include");
  connect(includeButton, SIGNAL(toggled(bool)), this, SIGNAL(includeMe(bool)) );
  QPushButton* displayButton = new QPushButton("info", this, "info");
  connect(displayButton, SIGNAL(clicked()), this, SIGNAL(displayMe()) );
  displayButton->setSizePolicy(sizePolicy);
  includeButton->setSizePolicy(sizePolicy);

  QHBoxLayout* hBox = new QHBoxLayout(this);  // could have some other numbers..
  
  hBox->addWidget(titleLabel);
  hBox->addWidget(sizeLabel);
  hBox->addWidget(displayButton);
  hBox->addWidget(includeButton);
}

set<int> SessionWidget::geneIds(){
  return(genes);
}

set<QString> SessionWidget::sessionKeyWords(){
  return(keyWords);
}

QString SessionWidget::sessionTitle(){
  return(title);
}

QString SessionWidget::sessionDescription(){
  return(description);
}

bool SessionWidget::canEdit(){
  return(!readOnly);
}
