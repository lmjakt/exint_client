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

#include "annotationWidget.h"
#include "../../smallToggle/smallToggle.h"
#include "../../probe_set/probe_set.h"     // for the annotationInformation struct.. 
#include "../../client.h"
#include <map>
#include <qwidget.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qsize.h>
#include <qregexp.h>
#include <qcolor.h>
#include <qfont.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <string>             // annotationInformation uses strings !!. 
#include <iostream>
#include <vector>

using namespace std;

AnnotationWidget::AnnotationWidget(annotationInformation an, Client* client, bool ce, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  widgetType = 1;   // annotation.. 
  setBackgroundColor(QColor(255, 255, 255));
  annotation.index = an.index;
  annotation.owner = an.owner;
  annotation.title = "";
  annotation.description = an.annotation;
  annotation.otherMembers = an.otherMembers;

  canEdit = ce;
  
  int buttonHeight = 23;
  updateButton = new QPushButton("update", this, "updatebutton");
  //  connect(updateButton, SIGNAL(clicked()), this, SIGNAL(updateMe()) );
  connect(updateButton, SIGNAL(clicked()), this, SLOT(updateInformation()) );
  setDisabled(updateButton, true);
  updateButton->setFixedHeight(buttonHeight);
  
  annotationArea = new QTextEdit(this, "annotationArea");
  annotationArea->setMinimumHeight(30);

  QColor labelBackground(200, 225, 225);
  QLabel* userLabel = new QLabel(this, "userLabel");
  userLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
  userLabel->setBackgroundColor(labelBackground);
  userLabel->setIndent(5);
  userLabel->setFixedHeight(buttonHeight);
  
  QLabel* geneNo = new QLabel(this, "geneNo");
  geneNo->setBackgroundColor(labelBackground);
  geneNo->setFixedHeight(buttonHeight);
  geneNo->setFixedWidth(60);
  geneNo->setIndent(3);

  QString genes;
  genes.setNum(annotation.otherMembers.size());
  genes.prepend("genes :");
  geneNo->setText(genes);

  QString text("Owner : ");
  map<int, userInformation>::iterator it = client->userInfo.find(annotation.owner);
  if(it != client->userInfo.end()){
    text += (*it).second.userName.c_str();
  }else{
    text += "unknown";
    cout << "couldn't get an entry in the uerInfo Map.. " << endl;
  }
  userLabel->setText(text);
  QString temp = annotation.description.c_str();
  dirtyText(temp);
  annotationArea->setText(temp);
  //  annotationArea->setText(annotation.description.c_str());  // ohh, so ugly. 
  annotationArea->setReadOnly(!canEdit);
  annotationArea->setFrameStyle(QFrame::NoFrame);
  connect(annotationArea, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)) );  // just enable the button if true. 

  SmallToggle* includeButton = new SmallToggle(1, this, "include");
  connect(includeButton, SIGNAL(toggled(bool)), this, SIGNAL(includeMe(bool)) );
  includeButton->setFixedHeight(buttonHeight);
  includeButton->setFixedWidth(buttonHeight);

  // and arrange the guys into some sort of thing..
  
  QVBoxLayout* vbox = new QVBoxLayout(this, 0, 1);
  vbox->addWidget(annotationArea);

  QHBoxLayout* hbox = new QHBoxLayout(0);
  vbox->addLayout(hbox);
  hbox->addWidget(includeButton);
  hbox->addWidget(updateButton);
  hbox->addWidget(geneNo);
  hbox->addWidget(userLabel);
  hbox->setStretchFactor(userLabel, 2);
}

AnnotationWidget::AnnotationWidget(sessionInformation an, Client* client, bool ce, QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  widgetType = 2;       // session type.. 
  setBackgroundColor(QColor(255, 255, 255));
  annotation = an;      // which is in fact a sessionInformation.. sigh..

  canEdit = ce;
  
  int buttonHeight = 23;
  updateButton = new QPushButton("update", this, "updatebutton");
  connect(updateButton, SIGNAL(clicked()), this, SLOT(updateInformation()) );
  //  connect(updateButton, SIGNAL(clicked()), this, SIGNAL(updateMe()) );
  setDisabled(updateButton, true);
  updateButton->setFixedHeight(buttonHeight);
  
  titleLine = new QLineEdit(this);
  titleLine->setFont(QFont("Arial", 10, QFont::Bold));
  titleLine->setAlignment(Qt::AlignHCenter);
  QString temp = annotation.title.c_str();
  dirtyText(temp);
  titleLine->setText(temp);
  // set some font attributes as well.
  keywordLine = new QLineEdit(this);
  QString keywords;
  for(int i=0; i < annotation.keywords.size(); i++){
    keywords += annotation.keywords[i].c_str();
    if(i < annotation.keywords.size()-1){
      keywords += " ";
    }
  }
  dirtyText(keywords);
  keywordLine->setText(keywords);

  annotationArea = new QTextEdit(this, "annotationArea");
  annotationArea->setMinimumHeight(30);

  QColor labelBackground(200, 225, 225);
  QLabel* userLabel = new QLabel(this, "userLabel");
  userLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
  userLabel->setBackgroundColor(labelBackground);
  userLabel->setIndent(5);
  userLabel->setFixedHeight(buttonHeight);
  
  QLabel* geneNo = new QLabel(this, "geneNo");
  geneNo->setBackgroundColor(labelBackground);
  geneNo->setFixedHeight(buttonHeight);
  geneNo->setFixedWidth(60);
  geneNo->setIndent(3);

  QString genes;
  genes.setNum(annotation.otherMembers.size());
  genes.prepend("genes :");
  geneNo->setText(genes);

  QString text("Owner : ");
  map<int, userInformation>::iterator it = client->userInfo.find(annotation.owner);
  if(it != client->userInfo.end()){
    text += (*it).second.userName.c_str();
  }else{
    text += "unknown";
    cout << "couldn't get an entry in the uerInfo Map.. " << endl;
  }
  userLabel->setText(text);
  temp = annotation.description.c_str();
  dirtyText(temp);
  annotationArea->setText(temp);
  //  annotationArea->setText(annotation.description.c_str());  // ohh, so ugly. 
  annotationArea->setReadOnly(!canEdit);
  titleLine->setReadOnly(!canEdit);
  keywordLine->setReadOnly(!canEdit);
  annotationArea->setFrameStyle(QFrame::NoFrame);

  connect(annotationArea, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)) );  // just enable the button if true. 
  connect(titleLine, SIGNAL(textChanged(const QString&)), this, SLOT(lineChanged(const QString&)) );  // just enable the button if true. 
  connect(keywordLine, SIGNAL(textChanged(const QString&)), this, SLOT(lineChanged(const QString&)) );  // just enable the button if true. 

  SmallToggle* includeButton = new SmallToggle(1, this, "include");
  connect(includeButton, SIGNAL(toggled(bool)), this, SIGNAL(includeMe(bool)) );
  includeButton->setFixedHeight(buttonHeight);
  includeButton->setFixedWidth(buttonHeight);

  // and arrange the guys into some sort of thing..
  
  QVBoxLayout* vbox = new QVBoxLayout(this, 0, 1);
  vbox->addWidget(titleLine);
  vbox->addWidget(annotationArea);
  vbox->addWidget(keywordLine);

  QHBoxLayout* hbox = new QHBoxLayout(0);
  vbox->addLayout(hbox);
  hbox->addWidget(includeButton);
  hbox->addWidget(updateButton);
  hbox->addWidget(geneNo);
  hbox->addWidget(userLabel);
  hbox->setStretchFactor(userLabel, 2);
}

void AnnotationWidget::modificationChanged(bool c){
  setDisabled(updateButton, !c);
}

void AnnotationWidget::lineChanged(const QString& word){
  // no use for it, but just change the
  setDisabled(updateButton, false);
  //  cout << "changing the disabled state of the update button " << endl;
}

QSize AnnotationWidget::sizeHint(){
 //  cout << "AnnotationWidget annotationArea sizeHint is " << annotationArea->sizeHint().height() 
//        << "  min size hint : " << annotationArea->minimumSizeHint().height() << endl
//        << "AnnotationAreas viewport size is " << annotationArea->viewport()->size().height() 
//        << "  min size hint " << annotationArea->viewport()->minimumSizeHint().height()
//        << endl;
//   cout << "And the contents width is : " << annotationArea->contentsWidth() << endl
//        << "                 height is : " << annotationArea->contentsHeight() << endl;
//   QWidget* child = annotationArea->childAt(20, 20);
//   cout << "Found a child with name : " << child->name() << endl;
//   QSize childSize = child->size();
//   QSize childHint = child->sizeHint();
//   cout << "Child's size : " << childSize.width() << " x " << childSize.height() << endl
//        << "Child's hint : " << childHint.width() << " x " << childHint.height() << endl;
  //  int h = (annotationArea->text().length() * 160) / annotationArea->width();   // letters are twice as high as they are big.. 
  //int h = childSize.height();
  int h = annotationArea->contentsHeight();
  h += 25;
  if(widgetType == 2){ h += 75; }
  if(h < 40) { h = 40; }
  return(QSize(250, h));
}
  
void AnnotationWidget::setDisabled(QPushButton* button, bool disabled){
  if(!disabled){
    button->setPaletteBackgroundColor(palette().color(QPalette::Active, QColorGroup::Button));
  }else{
    button->setPaletteBackgroundColor(QColor(100, 150, 150));
  }
  button->setDisabled(disabled);
}

void AnnotationWidget::updateInformation(){
  // at the moment, I don't display the title, or the keywords, so I can just sort of ignore them.. 
  if(widgetType == 1){           // we are annotation Information thingy..
    QString temp = annotationArea->text();
    cleanText(temp);
    emit updateAnnotation(annotation.index, temp);
    annotationArea->setModified(false);
    return;
  }
  if(widgetType == 2){         // session type.. a bit more complex...
    vector<QString> temp;
    QString ts = titleLine->text();
    cleanText(ts);
    temp.push_back(ts);
    //    temp.push_back(annotation.title.c_str());
    temp.push_back(annotationArea->text());
    cleanText(temp[1]);
    
    QStringList sList = QStringList::split(QRegExp("\\s+|,"), keywordLine->text());
    for(QStringList::iterator it = sList.begin(); it != sList.end(); it++){
      cleanText(*it);
      temp.push_back(*it);
    }
    //for(int i=0; i < annotation.keywords.size(); i++){
    //  temp.push_back(annotation.keywords[i].c_str());
    //}
    cout << "emmitting update session anntotation from annotationwidget" << endl;
    emit updateSessionAnnotation(annotation.index, temp);
    titleLine->setEdited(false);
    keywordLine->setEdited(false);
  }
  annotationArea->setModified(false);
  setDisabled(updateButton, true);
  /// will it emit// modification changed?? 
}

void AnnotationWidget::cleanText(QString& note){
  note.replace(QRegExp("<"), "&lt");
  note.replace(QRegExp(">"), "&gt");
  note.replace(QRegExp("\\|"), "&pipe");
  note.replace(QRegExp("'"), "&apo");
}

void AnnotationWidget::dirtyText(QString& note){
  note.replace(QRegExp("&lt"), "<");
  note.replace(QRegExp("&gt"), ">");
  note.replace(QRegExp("&pipe"), "|");
  note.replace(QRegExp("&apo"), "'");
}
