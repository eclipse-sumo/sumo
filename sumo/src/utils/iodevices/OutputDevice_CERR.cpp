/****************************************************************************/
/// @file    OutputDevice_CERR.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cout
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include "OutputDevice_CERR.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
OutputDevice* OutputDevice_CERR::myInstance = 0;


// ===========================================================================
// static method definitions
// ===========================================================================
OutputDevice*
OutputDevice_CERR::getDevice() {
    // check whether the device has already been aqcuired
    if (myInstance == 0) {
        myInstance = new OutputDevice_CERR();
    }
    return myInstance;
}


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_CERR::OutputDevice_CERR() {}


OutputDevice_CERR::~OutputDevice_CERR() {
    myInstance = 0;
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
