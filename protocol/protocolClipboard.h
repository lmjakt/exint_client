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

#ifndef PROTOCOLCLIPBOARD_H
#define PROTOCOLCLIPBOARD_H

#include <qobject.h>
#include "../dataStructs.h"


class ProtocolClipboard : public QObject
 {
  Q_OBJECT
  private :
    ProtocolStep stepCopy;     // hmmm, 
  Protocol protocolCopy;
  bool protocolDefined;
  bool stepDefined;

  public :
    void setStep(ProtocolStep stp);
  void setProtocol(Protocol prot);
  ProtocolStep step(){
    return(stepCopy);
  }
  Protocol protocol(){
    return(protocolCopy);
  }
  bool haveProtocol(){
    return(protocolDefined);
  }
  bool haveStep(){
    return(stepDefined);
  }
  // constructor..
  ProtocolClipboard(QObject* parent=0, const char* name=0);
  
  signals :
    void gotStep();
  void gotProtocol();
  
};

#endif
