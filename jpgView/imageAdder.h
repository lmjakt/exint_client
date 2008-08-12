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

#ifndef IMAGEADDER_H
#define IMAGEADDER_H

#include <qwidget.h>
#include "../dataStructs.h"
#include "fieldCreator.h"
#include "commentAdder.h"
#include "jpgView.h"
#include <vector>
#include <qlistview.h>
#include <qtextview.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <map>
#include <set>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtimer.h>

using namespace std;

class ImageAdder : public QWidget
{
  Q_OBJECT
    
    public :
    ImageAdder(QWidget* parent=0, const char* name=0);

  public slots :
    
  void addImages(ishProbeData data);
  void setTissues(vector<tissue> t);
  void setIshAnnotationFields(vector<ishAnnotationField> fields);
  void setExperiment(Experiment exp);
  void newStatus(StatusMessage message);
  
  private slots :
    void selectFiles();
  void popMenu(QListViewItem* item, const QPoint& p, int i);
  void removeFile();
  void selectionChanged(QListViewItem* item);
  void commitToDB();
  void newComment();   // make a new CommentAdder and sort out connections appropriately.. 
  void timeout();      // time out while waiting for a response from the server.. 
  
  private :
    QPushButton* commitButton;
    QTimer* timer;
    ishProbeData currentProbe;
  vector<tissue> tissues;
  vector<ishAnnotationField> annotationFields;
  Experiment currentExperiment;
  QListViewItem* item;   // the current item.. which we may need to remove if things go our way. 

  // A layout for adding adders...
  QVBoxLayout* commentBox;  // the left box.. 

  // something for viewing the details of the above..
  QTextView* exptView;
  QTextView* probeView;

  //  QTextView* commentArea;
  
  set<CommentAdder*> commentAdders;
  QComboBox* tissueChooser;
  // QComboBox* fieldChooser;
  QComboBox* promoterChooser;    // we have to sort this one out at some stage.. such that we get the relevant information from the probe information.. later though.
  /// its kind of ugly, but we need to be able to map between the tissueChooser and the index,, -best way is to subclass the QListViewItem, but seems to much trouble.
  map<int, int> tissueMap;
  map<int, int> fieldMap;

  // something to view the file list in ..
  QListView* fileView;
  //map<QListViewItem*, QString> filenames;     // so it would be cleaner to inherit the listview item.. abut.. argh.. 
  // something to view the picture in..
  JpgView* imageViewer;

  QPopupMenu* fileMenu;
  QPopupMenu* fSelectMenu;   // if selecting a file.. bugger should have used different naming.. 

  signals :
    void updateTissues();
  void updateFields();
  void getExperiment();        /// ask the parent to show the experimentBrowser... so that we can have a look at it.. 
  void makeField(QString);
  void makeTissue(QString, float);
  void commitImageToDB(ishImageSet);
};

#endif
