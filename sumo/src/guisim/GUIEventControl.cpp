/****************************************************************************/
/// @file    GUIEventControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 04 Feb 2008
/// @version $Id$
///
// Stores time-dependant events and executes them at the proper time (guisim)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "GUIEventControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods from GUIEventControl
 * ----------------------------------------------------------------------- */
GUIEventControl::GUIEventControl() throw() {}


GUIEventControl::~GUIEventControl() throw() {
}


SUMOTime
GUIEventControl::addEvent(Command* operation,
                          SUMOTime execTimeStep,
                          AdaptType type) throw() {
    myLock.lock();
    SUMOTime ret = MSEventControl::addEvent(operation, execTimeStep, type);
    myLock.unlock();
    return ret;
}


void
GUIEventControl::execute(SUMOTime execTime) throw(ProcessError) {
    myLock.lock();
    try {
        MSEventControl::execute(execTime);
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
    myLock.unlock();
}



/****************************************************************************/

