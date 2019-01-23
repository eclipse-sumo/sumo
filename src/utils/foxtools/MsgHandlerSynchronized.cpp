/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MsgHandlerSynchronized.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Retrieves messages about the process and gives them further to output
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#ifdef HAVE_FOX
#include <fx.h>
#endif
#include "MsgHandlerSynchronized.h"


// ===========================================================================
// static member variables
// ===========================================================================
#ifdef HAVE_FOX
FXMutex MsgHandlerSynchronized::myLock(true);
#endif


// ===========================================================================
// method definitions
// ===========================================================================
void
MsgHandlerSynchronized::inform(std::string msg, bool addType) {
#ifdef HAVE_FOX
    FXMutexLock locker(myLock);
#endif
    MsgHandler::inform(msg, addType);
}


void
MsgHandlerSynchronized::beginProcessMsg(std::string msg, bool addType) {
#ifdef HAVE_FOX
    FXMutexLock locker(myLock);
#endif
    MsgHandler::beginProcessMsg(msg, addType);
}


void
MsgHandlerSynchronized::endProcessMsg(std::string msg) {
#ifdef HAVE_FOX
    FXMutexLock locker(myLock);
#endif
    MsgHandler::endProcessMsg(msg);
}


void
MsgHandlerSynchronized::clear() {
#ifdef HAVE_FOX
    FXMutexLock locker(myLock);
#endif
    MsgHandler::clear();
}


void
MsgHandlerSynchronized::addRetriever(OutputDevice* retriever) {
#ifdef HAVE_FOX
    FXMutexLock locker(myLock);
#endif
    MsgHandler::addRetriever(retriever);
}


void
MsgHandlerSynchronized::removeRetriever(OutputDevice* retriever) {
#ifdef HAVE_FOX
    FXMutexLock locker(myLock);
#endif
    MsgHandler::removeRetriever(retriever);
}


MsgHandlerSynchronized::MsgHandlerSynchronized(MsgType type) :
    MsgHandler(type) {
}


MsgHandlerSynchronized::~MsgHandlerSynchronized() {
}


/****************************************************************************/
