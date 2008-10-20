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

#include "clusterListItem.h"
#include <qlistview.h>
#include "../client.h"
#include <qstring.h>

using namespace std;

ClusterListItem::ClusterListItem(clusterSet* cs, int i, QListViewItem* parent) :
   QListViewItem(parent)
{
  clusterNumber = i;
  cSet = cs;
  QString label;
  int size = 0;
  if(i < (int)cs->indices.size() && i >= 0){
    size = cs->indices[i].size();
  }
  label.sprintf("cluster %d members %d ", i, size);
  setText(0, label);
  rtid = 234576;
}

ClusterListItem::ClusterListItem(clusterSet* cs, int i, QListView* parent) :
   QListViewItem(parent)
{
  clusterNumber = i;
  cSet = cs;
  QString label;
  int size = cs->indices.size();
  label.sprintf("Set %d (%d)", i, size);
  setText(0, label);
  rtid = 234577;
}

// int ClusterListItem::rtti(){
//   return(234576);
// }

clusterSet* ClusterListItem::cluster(){
  return(cSet);
}

int ClusterListItem::number(){
  return(clusterNumber);
}

int ClusterListItem::compare(QListViewItem* i, int col, bool ascending) const{
  if(i->rtti() != rtid){
    return(0);
  }
  ClusterListItem* item = (ClusterListItem*)i;
  if(clusterNumber == item->number()){
    return(0);
  }
  if(clusterNumber > item->number()){
    return(1);
  }
  return(-1);
}

  
