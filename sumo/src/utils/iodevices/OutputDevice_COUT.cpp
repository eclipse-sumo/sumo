/****************************************************************************/
/// @file    OutputDevice_COUT.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cout
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

#include <iostream>
#include "OutputDevice_COUT.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
OutputDevice* OutputDevice_COUT::myInstance = 0;


// ===========================================================================
// static method definitions
// ===========================================================================
OutputDevice*
OutputDevice_COUT::getDevice() {
    // check whether the device has already been aqcuired
    if (myInstance == 0) {
        myInstance = new OutputDevice_COUT();
    }
    return myInstance;
}


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_COUT::OutputDevice_COUT() {}


OutputDevice_COUT::~OutputDevice_COUT() {
    myInstance = 0;
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
