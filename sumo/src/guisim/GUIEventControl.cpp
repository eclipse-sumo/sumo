/****************************************************************************/
/// @file    GUIEventControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 04 Feb 2008
/// @version $Id$
///
// Stores time-dependant events and executes them at the proper time (guisim)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <utils/foxtools/MFXMutex.h>
#include "GUIEventControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
GUIEventControl::GUIEventControl() {}


GUIEventControl::~GUIEventControl() {
}


SUMOTime
GUIEventControl::addEvent(Command* operation,
                          SUMOTime execTimeStep,
                          AdaptType type) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSEventControl::addEvent(operation, execTimeStep, type);
}


void
GUIEventControl::execute(SUMOTime execTime) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSEventControl::execute(execTime);
}



/****************************************************************************/

