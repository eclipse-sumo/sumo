/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDevice_CERR.cpp
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
#include "OutputDevice_CERR.h"


// ===========================================================================
// static member definitions
// ===========================================================================
OutputDevice* OutputDevice_CERR::myInstance = nullptr;


// ===========================================================================
// static method definitions
// ===========================================================================
OutputDevice*
OutputDevice_CERR::getDevice() {
    // check whether the device has already been aqcuired
    if (myInstance == nullptr) {
        myInstance = new OutputDevice_CERR();
    }
    return myInstance;
}


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_CERR::OutputDevice_CERR() {
    myFilename = "CERR";
}


OutputDevice_CERR::~OutputDevice_CERR() {
    myInstance = nullptr;
}


std::ostream&
OutputDevice_CERR::getOStream() {
    return std::cerr;
}


void
OutputDevice_CERR::postWriteHook() {
    std::cerr.flush();
}


/****************************************************************************/
