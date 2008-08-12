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

#include "protocolClipboard.h"
#include "../dataStructs.h"
#include <qobject.h>


ProtocolClipboard::ProtocolClipboard(QObject* parent, const char* name)
  : QObject(parent, name)
{
  stepDefined = false;
  protocolDefined = false;
}

void ProtocolClipboard::setStep(ProtocolStep stp){
  stepCopy = stp;
  stepDefined = true;
  emit gotStep();
}

void ProtocolClipboard::setProtocol(Protocol prot){
  protocolCopy = prot;
  protocolDefined = true;
  emit gotProtocol();
}
