/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIEventControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 04 Feb 2008
/// @version $Id$
///
// Stores time-dependant events and executes them at the proper time (guisim)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <fx.h>
#include "GUIEventControl.h"


// ===========================================================================
// member definitions
// ===========================================================================
GUIEventControl::GUIEventControl() :
    myLock(true)
{}


GUIEventControl::~GUIEventControl() {
}


void
GUIEventControl::addEvent(Command* operation, SUMOTime execTimeStep) {
    FXMutexLock locker(myLock);
    MSEventControl::addEvent(operation, execTimeStep);
}


void
GUIEventControl::execute(SUMOTime execTime) {
    FXMutexLock locker(myLock);
    MSEventControl::execute(execTime);
}



/****************************************************************************/

