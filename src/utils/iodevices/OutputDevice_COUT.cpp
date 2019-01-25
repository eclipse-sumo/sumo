/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDevice_COUT.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cout
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include "OutputDevice_COUT.h"


// ===========================================================================
// static member definitions
// ===========================================================================
OutputDevice* OutputDevice_COUT::myInstance = nullptr;


// ===========================================================================
// static method definitions
// ===========================================================================
OutputDevice*
OutputDevice_COUT::getDevice() {
    // check whether the device has already been aqcuired
    if (myInstance == nullptr) {
        myInstance = new OutputDevice_COUT();
    }
    return myInstance;
}


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_COUT::OutputDevice_COUT() {
    myFilename = "COUT";
}


OutputDevice_COUT::~OutputDevice_COUT() {
    myInstance = nullptr;
}


std::ostream&
OutputDevice_COUT::getOStream() {
    return std::cout;
}


void
OutputDevice_COUT::postWriteHook() {
    std::cout.flush();
}


/****************************************************************************/
