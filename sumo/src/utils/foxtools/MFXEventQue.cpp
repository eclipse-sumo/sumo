/****************************************************************************/
/// @file    MFXEventQue.cpp
/// @author  Daniel Krajzewicz
/// @date    void *ret = *(myEvents.begin());
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include "MFXEventQue.h"
#include <cassert>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

void*
MFXEventQue::top() {
    assert(size() != 0);
    myMutex.lock();
    void* ret = *(myEvents.begin());
    myMutex.unlock();
    return ret;
}


void
MFXEventQue::pop() {
    myMutex.lock();
    myEvents.erase(myEvents.begin());
    myMutex.unlock();
}


void
MFXEventQue::add(void* what) {
    myMutex.lock();
    myEvents.push_back(what);
    myMutex.unlock();
}


size_t
MFXEventQue::size() {
    myMutex.lock();
    size_t msize = myEvents.size();
    myMutex.unlock();
    return msize;
}


bool
MFXEventQue::empty() {
    myMutex.lock();
    bool ret = size() == 0;
    myMutex.unlock();
    return ret;
}



/****************************************************************************/

