//:Source-----------------------------*- mode: c++; tab-width: 2 -*-
//
// $Id$
//
// Copyright (C) 2000 by Daniel Gehriger.  All Rights Reserved
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// $Id$
//
//------------------------------------------------------------------
//:Description
//
//  Inter-thread communication object
//
//------------------------------------------------------------------
//#include <config.h>
#include <fxver.h>
#include <xincs.h>
#include <fx.h>
/*
#include <fxdefs.h>
#include <FXString.h>
#include <FXStream.h>
#include <FXSize.h>
#include <FXPoint.h>
#include <FXRectangle.h>
#include <FXRegistry.h>
#include <FXHash.h>
#include <FXApp.h>
*/
#ifndef WIN32
#include <unistd.h>
#endif

using namespace FX;
#include "fxexdefs.h"
#include "FXThreadEvent.h"
using namespace FXEX;
namespace FXEX {

#ifndef WIN32
# define PIPE_READ 0
# define PIPE_WRITE 1
#endif

// Message map
FXDEFMAP(FXThreadEvent) FXThreadEventMap[]={
  FXMAPTYPE(0,FXThreadEvent::onThreadEvent),
  FXMAPFUNC(SEL_THREAD,0, FXThreadEvent::onThreadEvent),
  FXMAPFUNC(SEL_IO_READ,FXThreadEvent::ID_THREAD_EVENT,FXThreadEvent::onThreadSignal),
  };
FXIMPLEMENT(FXThreadEvent,FXBaseObject,FXThreadEventMap,ARRAYNUMBER(FXThreadEventMap));

// FXThreadEvent : Constructor
FXThreadEvent::FXThreadEvent(FXObject* tgt,FXSelector sel) : FXBaseObject(tgt,sel) {
#ifndef WIN32
  FXMALLOC(&event,FXThreadEventHandle,2);
  FXint res = pipe(event);
  FXASSERT(res == 0);
  getApp()->addInput(event[PIPE_READ],INPUT_READ,this,ID_THREAD_EVENT);
#else
  event=CreateEvent(NULL,FALSE,FALSE,NULL);
  FXASSERT(event != NULL);
  getApp()->addInput(event,INPUT_READ,this, ID_THREAD_EVENT);
#endif
  }

// ~FXThreadEvent : Destructor
FXThreadEvent::~FXThreadEvent() {
#ifndef WIN32
  getApp()->removeInput(event[PIPE_READ],INPUT_READ);
  ::close(event[PIPE_READ]);
  ::close(event[PIPE_WRITE]);
  FXFREE(&event);
#else
  getApp()->removeInput(event,INPUT_READ);
  ::CloseHandle(event);
#endif
  }

// signal the target using the SEL_THREAD seltype
// this method is meant to be called from the worker thread
void FXThreadEvent::signal() {
  FXuint seltype=SEL_THREAD;
#ifndef WIN32
  ::write(event[PIPE_WRITE],&seltype,sizeof(seltype));
#else
  ::SetEvent(event);
#endif
  }

// signal the target using some seltype
// this method is meant to be called from the worker thread
void FXThreadEvent::signal(FXuint seltype) {
#ifndef WIN32
  ::write(event[PIPE_WRITE],&seltype,sizeof(seltype));
#else
  ::SetEvent(event);
#endif
  }

// this thread is signalled via the IO/event, from other thread.
// We also figure out what SEL_type to generate.
// We forward it to ourselves first, to allow child classes to handle the event.
long FXThreadEvent::onThreadSignal(FXObject*,FXSelector,void*) {
  FXuint seltype=SEL_THREAD;
#ifndef WIN32
  ::read(event[PIPE_READ],&seltype,sizeof(seltype));
#else
  //FIXME need win32 support
#endif
  handle(this,FXSEL(seltype,0),NULL);
  return 0;
  }

// forward thread event to application - we generate the appropriate FOX event
// which is now in the main thread (ie no longer in the worker thread)
long FXThreadEvent::onThreadEvent(FXObject*,FXSelector sel,void*){
  FXuint seltype = FXSELTYPE(sel);
  return target && target->handle(this,FXSEL(seltype,message),NULL);
  }

}

