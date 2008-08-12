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

#include "imageAdder.h"
#include <qlayout.h>
#include <iostream>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qglobal.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qregexp.h>

using namespace std;

ImageAdder::ImageAdder(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  setCaption("Image Adder");
  // QText views..
  QLabel* exptLabel = new QLabel("Experiment Details", this, "exptLabel");
  exptView = new QTextView(this, "exptView");
  QLabel* probeLabel = new QLabel("Probe Details", this, "probeLabel");
  probeView = new QTextView(this, "probeView");
  exptView->setReadOnly(true);
  probeView->setReadOnly(true);

  timer = new QTimer(this, "timer");
  item = 0;
  connect(timer, SIGNAL(timeout()), this, SLOT(timeout()) );   // give some sort of error message.. 
//   QLabel* commentLabel = new QLabel("Comment Area", this, "commentLabel");
//   commentArea = new QTextView(this, "commentArea");
//   commentArea->setReadOnly(false);
  QPushButton* commentButton = new QPushButton("Add Comment", this, "commentButton");
  connect(commentButton, SIGNAL(clicked()), this, SLOT(newComment()) );


  QPushButton* getExpt = new QPushButton("Select Experiment", this, "getExpt");
  connect(getExpt, SIGNAL(clicked()), this, SIGNAL(getExperiment()) );
  
  FieldCreator* creator = new FieldCreator();
  connect(creator, SIGNAL(makeField(QString)), this, SIGNAL(makeField(QString)) );
  connect(creator, SIGNAL(makeTissue(QString, float)), this, SIGNAL(makeTissue(QString, float)) );

  QLabel* tissueLabel = new QLabel("Tissue : ", this, "tissueLabel");
  tissueChooser = new QComboBox(false, this, "tissueChooser");
  tissueChooser->insertItem("-----------");       // to make sure that the user actually chooses a tissue.
  QPushButton* makeTissueButton = new QPushButton("New Tissue", this, "makeTissueButton");

//   fieldChooser = new QComboBox(false, this, "fieldChooser");
//   fieldChooser->insertItem("-----------");
//   QPushButton* makeFieldButton = new QPushButton("New Field", this, "makeFieldButton");
//   connect(makeFieldButton, SIGNAL(clicked()), creator, SLOT(createField()) );
  connect(makeTissueButton, SIGNAL(clicked()), creator, SLOT(createTissue()) );

  promoterChooser = new QComboBox(false, this, "promoterChooser");
  promoterChooser->insertItem("-----------");
  promoterChooser->insertItem("sp6");
  promoterChooser->insertItem("t7");
  promoterChooser->insertItem("t3");
  QLabel* promLabel = new QLabel("Promoter : ", this, "promLabel");
  

  fileView = new QListView(this, "fileView");
  //fileView->addColumn("Path");
  fileView->addColumn("File Name");
  connect(fileView, SIGNAL(rightButtonClicked(QListViewItem*, const QPoint&, int)),
	  this, SLOT(popMenu(QListViewItem*, const QPoint&, int)) );
  connect(fileView, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(selectionChanged(QListViewItem*)) );

  commitButton = new QPushButton("Commit to Database", this, "commitButton");
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commitToDB()) );

  fileMenu = new QPopupMenu(this, "fileMenu");
  fileMenu->insertItem("Select Files", this, SLOT(selectFiles()) );

  fSelectMenu = new QPopupMenu(this, "fSelectMenu");
  fSelectMenu->insertItem("Remove", this, SLOT(removeFile()) );

  imageViewer = new JpgView();       // hide it until an image chosen.. 
  
  // and set up the layoub..
  QHBoxLayout* mbox = new QHBoxLayout(this);
  QVBoxLayout* lbox = new QVBoxLayout(mbox, 2);         // need this later on.. 
  QHBoxLayout* ebox = new QHBoxLayout(lbox);
  ebox->addWidget(exptLabel);
  ebox->addStretch();
  ebox->addWidget(getExpt);
  lbox->addWidget(exptView);
  lbox->addWidget(probeLabel);
  lbox->addWidget(probeView);
  QHBoxLayout* pbox = new QHBoxLayout(lbox);
  pbox->addWidget(promLabel);
  pbox->addWidget(promoterChooser);
  lbox->addWidget(tissueLabel);
  QHBoxLayout* tbox = new QHBoxLayout(lbox);
  tbox->addWidget(tissueChooser);
  tbox->addWidget(makeTissueButton);
  commentBox = new QVBoxLayout(lbox);  // which should add it .. 
  lbox->addWidget(commentButton);
//   lbox->addWidget(commentLabel);
//   lbox->addWidget(commentArea);
//   QHBoxLayout* fbox = new QHBoxLayout(lbox);
//   fbox->addWidget(fieldChooser);
//   fbox->addWidget(makeFieldButton);
  
  QVBoxLayout* rbox = new QVBoxLayout(mbox);
  rbox->addWidget(fileView);
  rbox->addWidget(commitButton);
  //
}

void ImageAdder::addImages(ishProbeData data){
  currentProbe = data;
  cout << "Setting the data for the probe.. " << endl; 
  // and format the text for everything..
  QString fontd("<font color=\"#000000\">");
  QString text;
  QTextStream ts(&text, 2);
  ts << "<table width=99% bgcolor=\"#ffffff\" cellspacing=0 border=0 align=center>" << endl; //  "<tr><th colspan=2 align=center>" << fontd << "Probe Details</th></tr>" << endl;
  ts << "<tr><td width=\"40%\">" << fontd << "Probe Name" << "</td><td width=60%>"
     << fontd << data.probeName.c_str() << "</td></tr>" << endl;
  ts << "<tr><td width=\"40%\">" << fontd << "Vector Name" << "</td><td width=60%>"
     << fontd << data.vectorName.c_str() << "</td></tr>" << endl;
  ts << "<tr><td width=\"40%\">" << fontd << "Antisense Promoter" << "</td><td width=60%>"
     << fontd << data.antisensePromoter.c_str() << "</td></tr>" << endl;
  ts << "<tr><td width=\"40%\">" << fontd << "Probe Identifier" << "</td><td width=60%>"
     << fontd << data.probeIdentifier.c_str() << "</td></tr>" << endl;

  ts << "</table></qt>";
  probeView->setText(text);
  // and maybe enough for now.. 
  if(!isVisible()){
    show();
  }
}

void ImageAdder::setTissues(vector<tissue> t){
  tissues = t;
  tissueChooser->clear();
  while(tissueMap.size()){
    tissueMap.erase(tissueMap.begin());
  }
  tissueChooser->insertItem("-----------");
  for(int i=0; i < tissues.size(); i++){
    QString tname = tissues[i].tissueName.c_str();
    tname += "  ";
    QString age;
    age.setNum(tissues[i].age);
    tname += age;
    tissueChooser->insertItem(tname);
    tissueMap.insert(make_pair(i+1, tissues[i].tissueIndex));
  }
}

void ImageAdder::setIshAnnotationFields(vector<ishAnnotationField> fields){
  annotationFields = fields;
  for(set<CommentAdder*>::iterator it=commentAdders.begin(); it != commentAdders.end(); it++){
    (*it)->setIshAnnotationFields(fields);
  }
//   fieldChooser->clear();
//   while(fieldMap.size()){
//     fieldMap.erase(fieldMap.begin());
//   }
//   fieldChooser->insertItem("-----------");
//   for(int i=0; i < fields.size(); i++){
//     fieldChooser->insertItem(fields[i].fieldName);
//     fieldMap.insert(make_pair(i+1, fields[i].index));      // hmm to sort implement < in the struct and sort before.. 
//   }
}

void ImageAdder::setExperiment(Experiment exp){
  currentExperiment = exp;
  cout << "setting the experiment " << endl;
  // and format the text for the various bits.. which is going to be a bit painful..
  // but there you go..
  QString fontd("<font color=\"#000000\">");
  QString text;
  QTextStream ts(&text, 2);
  ts << "<table width=99% bgcolor=\"#ffffff\" cellspacing=0 border=0 align=center>"  << endl  // "<tr><th colspan=2 align=center>" << fontd << "Experiment Details</th></tr>" << endl
     << "<tr><td width=\"20%\">" << fontd << "User" << "</td><td width=80%>"
     << fontd << exp.userName.c_str() << "</td></tr>" << endl;
  ts << "<tr><td width=\"20%\">" << fontd << "Protocol" << "</td><td width=80%>"
     << fontd << exp.protocolName.c_str() << "</td></tr>" << endl;
  ts << "<tr><td width=\"20%\">" << fontd << "Comment" << "</td><td width=80%>"
     << fontd << exp.comment.c_str() << "</td></tr>" << endl;
  ts << "<tr><td width=\"20%\">" << fontd << "Date" << "</td><td width=80%>"
     << fontd << exp.experimentTime.toString("yy MM dd") << "</td></tr>" << endl;
  ts << "</table></qt>";
  exptView->setText(text);
  // and maybe enough for now.. 

}

void ImageAdder::popMenu(QListViewItem* item, const QPoint& p, int i){
  if(!item){
    fileMenu->popup(p);
    return;
  }
  fSelectMenu->popup(p);
}

void ImageAdder::selectFiles(){
  QStringList files = QFileDialog::getOpenFileNames("Images (*.jpg *.JPG *.xpm *.XPM *.png *.PNG)");
  if(!files.size()){
    return;
  }
  // empty the fileview.. 
  QListViewItem* item;
  while( (item = fileView->firstChild()) ){
    delete item;
  }

  QStringList flist = files;   // documentation says so but I don't know why..
  QStringList::Iterator it = flist.begin();
  while( it != flist.end() ){
    // do stuff
    QString fileName;
    QString path;
    char s = '/';
#ifdef Q_OS_MSDOS
    s = '\\';    // but will that work.. 
#endif
    int pos = (*it).findRev(s);
    pos++;
    //    cout << "pos is : " << pos << endl;
    if(pos != -1){
      fileName = (*it).mid(pos);
      path = (*it).left(pos);
    }else{
      fileName = (*it);
      path = "";
    }
    
    QListViewItem item = new QListViewItem(fileView, fileName, path);
    //QListViewItem item = new QListViewItem(fileView, path, fileName);
    ++it;
  }
  fileView->setSelected(fileView->firstChild(), true);
}

void ImageAdder::removeFile(){
  QListViewItem* item = fileView->selectedItem();
  if(!item){
    return;
  }
  cout << "path is : " <<  item->text(1) << endl;
  delete item;
}

void ImageAdder::selectionChanged(QListViewItem* item){
  QString fullpath = item->text(1);
  fullpath += item->text(0);
  imageViewer->setImageFromFile(fullpath);
  imageViewer->setLabel(item->text(0));
  imageViewer->show();
}

void ImageAdder::commitToDB(){              // gather the data, then emit signals containing the appropriate stuff... 
  cout << "I should be committing to DB.. " << endl;
  QString errorString;

  item = fileView->selectedItem();

  QRegExp fileCheck(currentProbe.probeIdentifier.c_str());
  if(item && (fileCheck.search(item->text(0)) == -1)){
    QString error = "Suspicious file name. The file name should contain the probe identifier.\nAre you REALLY sure that the current probe was used to obtain this image?";
    int choice = QMessageBox::information(this, "Image Adder", error, "Cancel", "Commit", 0, 1);
    if(!choice){
      return;
    }
  }

  if(!item){
    errorString += "You need to select an image file in order to commit the image to the Database\n";
  }
  if(currentExperiment.id == -1){
    errorString += "You need to select an experiment before commiting the image\n";
  }
  // the probe will be set so we don't need to worry..
  if(promoterChooser->currentItem() == 0){
    errorString += "You will need to select the appropriate promoter\n";
  }
  if(tissueChooser->currentItem() == 0){
    errorString += "You must select a tissue for the experiment\n";
  }
  for(set<CommentAdder*>::iterator it=commentAdders.begin(); it != commentAdders.end(); it++){
    if((*it)->comment().length() && (*it)->currentIndex() == -1){
      errorString += "Each comment must be associated with a specific field\n";
      break;
    }
  }

  //if(commentArea->text().length() && (fieldChooser->currentItem() == 0)){
  //  errorString += "You must select a field for your given comment\n";
  //}
  if(errorString.length()){
    QMessageBox::information(this, "Image Adder", errorString, "Cancel", 0);
    return;
  }
  cout << "Seems that we have all the appropriate components.. " << endl;
  int tissueIndex, fieldIndex;
  fieldIndex = 0;   // it's ok to be 0..
  map<int, int>::iterator it;
  vector<Comment> comments;
  cout << "And comment adders size is : " << commentAdders.size() << endl;
  for(set<CommentAdder*>::iterator it=commentAdders.begin(); it != commentAdders.end(); it++){
    comments.push_back(Comment((*it)->currentIndex(), (*it)->comment()));
  }
  cout << "this is image adder, comments size is : " << comments.size() << endl;

//   if(commentArea->text().length()){
//     it = fieldMap.find(fieldChooser->currentItem());
//     if(it == fieldMap.end()){
//       cerr << "Image Adder couldn't find an index for the field, bugger " << endl;
//       return;
//     }
//     fieldIndex = (*it).second;
//   }
  it = tissueMap.find(tissueChooser->currentItem());
  if(it == tissueMap.end()){
    cerr << "Couldn't find an index for the tissue selection " << endl;
    return;
  }
  tissueIndex = (*it).second; 
  // and then we can just make the thingy and emit it.. 
  unsigned int identifier = (unsigned int)this;
  ishImageSet imageSet(imageViewer->imagePointer(), currentProbe.probeId, promoterChooser->currentText(), currentExperiment.id, tissueIndex, item->text(0), comments, identifier); 
  //  ishImageSet imageSet(imageViewer->imagePointer(), currentProbe.probeId, promoterChooser->currentText(), currentExperiment.id, tissueIndex, item->text(0), fieldIndex, commentArea->text()); 
  emit commitImageToDB(imageSet);
  // and then do lots of resetting moving images around and so on.. -so that we don't do this again.. 
  // item is the listviewitem for this one,, -text(0) gives the file name, text(1) gives the path (with trailing slash..);
  // and start the timer to see if we get anything..
  timer->start(5000);  // 3 seconds.. should be enough
  // should disable the button, but try that later a
  commitButton->setEnabled(false);
}

void ImageAdder::newComment(){
  cout << "here I should be making a new comment adder and hooking it up to the appropriate signals.. " << endl;
  CommentAdder* adder = new CommentAdder(annotationFields, this, "adder");
  connect(adder, SIGNAL(makeField(QString)), this, SIGNAL(makeField(QString)) );
  commentBox->addWidget(adder);
  adder->show();
  commentAdders.insert(adder);
}

void ImageAdder::newStatus(StatusMessage message){
  if(message.id == (unsigned int)this){
    commitButton->setEnabled(true);
    cout << "We have received notifciation of the insert " << endl;
    timer->stop();
    if(message.ok){
      cout << "And the insert worked without any problems" << endl;
      // so we should do all of the below.. 
      QString path = item->text(1);
      QString donePath = path;
      QDateTime now = QDateTime::currentDateTime();
      donePath += "done_" + now.toString("yy_MM_dd");
      QDir dir;
      if(!dir.exists(donePath)){
	if(!dir.mkdir(donePath)){
	  cerr << "Failed to make the done directory, what's up " << endl;
	  return;
	}
      }
      // see if we can move the file.. 
      // renaming it.. ;->
      QString newName = donePath;
      newName += "/" + item->text(0);
      QString oldName = item->text(1);
      oldName += item->text(0);
      // and now simpley..
      if(!dir.rename(oldName, newName)){
	cerr << "couldn't rename file from " << oldName << "  to " << newName << endl;
      }
      tissueChooser->setCurrentItem(0);
      //fieldChooser->setCurrentItem(0);
      promoterChooser->setCurrentItem(0);      // force the user to specify each time..
      //commentArea->clear();                    // we should really wait for confirmation, before doing this, but that is difficult.. and maybe we can do later.
      // --- we should probably delete the commentadders here, but who knows exactly what to do.. 
      if(item){
	delete item;
	item = 0;
      }
      for(set<CommentAdder*>::iterator it=commentAdders.begin(); it != commentAdders.end(); it++){
	(*it)->clear();   // Find some way to delete these later on.. 
      }
    }else{
      cerr << "Got a notification, but some problem.. " << endl;
      QString errorMessage = "Server had some problem adding the image to the database please talk to your admin\n";
      for(int i=0; i < message.errorMessages.size(); i++){
	errorMessage += message.errorMessages[i];
	errorMessage += "\n";
      }
      QMessageBox::information(this, "Image Adder", errorMessage, "Cancel", 0);
    }
  }else{
    cout << "This is the wrong message id, id is : " << message.id << "  and our address is " << (unsigned int)this << endl;
  }
}

void ImageAdder::timeout(){
  QString errorString = "Timed out waiting for response from Server. Can't confirm or deny data entry\n you may try again or giveup\n";
  QMessageBox::information(this, "Image Adder", errorString, "Cancel", 0);
  commitButton->setEnabled(true);
  cerr << "Time out man, bummer " << endl;
}
