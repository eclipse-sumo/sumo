/****************************************************************************/
/// @file    FXThreadEvent.cpp
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fxver.h>
#include <xincs.h>
#include <fx.h>
#include <utils/common/StdDefs.h>
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
// ===========================================================================
// used namespaces
// ===========================================================================
#endif // _DEBUG
using namespace FXEX;
namespace FXEX {

#ifndef WIN32
# define PIPE_READ 0
# define PIPE_WRITE 1
#endif

// Message map
FXDEFMAP(FXThreadEvent) FXThreadEventMap[] = {
    FXMAPTYPE(0, FXThreadEvent::onThreadEvent),
    FXMAPFUNC(SEL_THREAD, 0, FXThreadEvent::onThreadEvent),
    FXMAPFUNC(SEL_IO_READ, FXThreadEvent::ID_THREAD_EVENT, FXThreadEvent::onThreadSignal),
};
FXIMPLEMENT(FXThreadEvent, FXBaseObject, FXThreadEventMap, ARRAYNUMBER(FXThreadEventMap))

// FXThreadEvent : Constructor
FXThreadEvent::FXThreadEvent(FXObject* tgt, FXSelector sel) : FXBaseObject(tgt, sel) {
#ifndef WIN32
    FXMALLOC(&event, FXThreadEventHandle, 2);
    FXint res = pipe(event);
    FXASSERT(res == 0);
    UNUSED_PARAMETER(res); // only used for assertion
    getApp()->addInput(event[PIPE_READ], INPUT_READ, this, ID_THREAD_EVENT);
#else
    event = CreateEvent(NULL, FALSE, FALSE, NULL);
    FXASSERT(event != NULL);
    getApp()->addInput(event, INPUT_READ, this, ID_THREAD_EVENT);
#endif
}

// ~FXThreadEvent : Destructor
FXThreadEvent::~FXThreadEvent() {
#ifndef WIN32
    getApp()->removeInput(event[PIPE_READ], INPUT_READ);
    ::close(event[PIPE_READ]);
    ::close(event[PIPE_WRITE]);
    FXFREE(&event);
#else
    getApp()->removeInput(event, INPUT_READ);
    ::CloseHandle(event);
#endif
}

// signal the target using the SEL_THREAD seltype
// this method is meant to be called from the worker thread
void FXThreadEvent::signal() {
#ifndef WIN32
    FXuint seltype = SEL_THREAD;
    ::write(event[PIPE_WRITE], &seltype, sizeof(seltype));
#else
    ::SetEvent(event);
#endif
}

// signal the target using some seltype
// this method is meant to be called from the worker thread
void FXThreadEvent::signal(FXuint seltype) {
#ifndef WIN32
    ::write(event[PIPE_WRITE], &seltype, sizeof(seltype));
#else
    UNUSED_PARAMETER(seltype);
    ::SetEvent(event);
#endif
}

// this thread is signalled via the IO/event, from other thread.
// We also figure out what SEL_type to generate.
// We forward it to ourselves first, to allow child classes to handle the event.
long FXThreadEvent::onThreadSignal(FXObject*, FXSelector, void*) {
    FXuint seltype = SEL_THREAD;
#ifndef WIN32
    ::read(event[PIPE_READ], &seltype, sizeof(seltype));
#else
    //FIXME need win32 support
#endif
    handle(this, FXSEL(seltype, 0), NULL);
    return 0;
}

// forward thread event to application - we generate the appropriate FOX event
// which is now in the main thread (ie no longer in the worker thread)
long FXThreadEvent::onThreadEvent(FXObject*, FXSelector sel, void*) {
    FXuint seltype = FXSELTYPE(sel);
    return target && target->handle(this, FXSEL(seltype, message), NULL);
}

}



/****************************************************************************/

