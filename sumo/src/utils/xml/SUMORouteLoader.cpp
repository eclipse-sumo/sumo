/****************************************************************************/
/// @file    SUMORouteLoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 6 Nov 2002
/// @version $Id$
///
// A class that performs the loading of routes
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

#include <utils/xml/SUMORouteHandler.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include "SUMORouteLoader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMORouteLoader::SUMORouteLoader(SUMORouteHandler* handler)
    : myParser(0), myMoreAvailable(true), myHandler(handler) {
    myParser = XMLSubSys::getSAXReader(*myHandler);
    if (!myParser->parseFirst(myHandler->getFileName())) {
        throw ProcessError("Can not read XML-file '" + myHandler->getFileName() + "'.");
    }
}


SUMORouteLoader::~SUMORouteLoader() {
    delete myParser;
    delete myHandler;
}


SUMOTime
SUMORouteLoader::loadUntil(SUMOTime time) {
    SUMOTime firstDepart = SUMOTime_MAX;
    // read only when further data is available, no error occured
    //  and vehicles may be found in the between the departure time of
    //  the last read vehicle and the time to read until
    if (!myMoreAvailable || time <= myHandler->getLastDepart()) {
        return firstDepart;
    }

    // read vehicles until specified time or the period to read vehicles
    //  until is reached
    while (myParser->parseNext()) {
        if (firstDepart == SUMOTime_MAX && myHandler->getLastDepart() >= 0) {
            firstDepart = myHandler->getLastDepart();
        }
        // return when the last read vehicle is beyond the period
        if (time <= myHandler->getLastDepart()) {
            return firstDepart;
        }
    }

    // no data available anymore
    myMoreAvailable = false;
    return firstDepart;
}


bool
SUMORouteLoader::moreAvailable() const {
    return myMoreAvailable;
}


/****************************************************************************/
