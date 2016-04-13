/****************************************************************************/
/// @file    SUMORouteLoaderControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 06 Nov 2002
/// @version $Id$
///
// Class responsible for loading of routes from some files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2016 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <utils/common/StdDefs.h>
#include "SUMORouteLoader.h"
#include "SUMORouteLoaderControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMORouteLoaderControl::SUMORouteLoaderControl(SUMOTime inAdvanceStepNo):
    myFirstLoadTime(SUMOTime_MAX),
    myCurrentLoadTime(-SUMOTime_MAX),
    myInAdvanceStepNo(inAdvanceStepNo),
    myRouteLoaders(),
    myLoadAll(inAdvanceStepNo <= 0),
    myAllLoaded(false) {
}


SUMORouteLoaderControl::~SUMORouteLoaderControl() {
    for (std::vector<SUMORouteLoader*>::iterator i = myRouteLoaders.begin();
            i != myRouteLoaders.end(); ++i) {
        delete(*i);
    }
}


void
SUMORouteLoaderControl::add(SUMORouteLoader* loader) {
    myRouteLoaders.push_back(loader);
}


void
SUMORouteLoaderControl::loadNext(SUMOTime step) {
    // check whether new vehicles shall be loaded
    //  return if not
    if (myAllLoaded) {
        return;
    }
    if (myCurrentLoadTime > step) {
        return;
    }
    const SUMOTime loadMaxTime = myLoadAll ? SUMOTime_MAX : MAX2(myCurrentLoadTime + myInAdvanceStepNo, step);
    myCurrentLoadTime = SUMOTime_MAX;
    // load all routes for the specified time period
    bool furtherAvailable = false;
    for (std::vector<SUMORouteLoader*>::iterator i = myRouteLoaders.begin(); i != myRouteLoaders.end(); ++i) {
        myCurrentLoadTime = MIN2(myCurrentLoadTime, (*i)->loadUntil(loadMaxTime));
        if ((*i)->getFirstDepart() != -1) {
            myFirstLoadTime = MIN2(myFirstLoadTime, (*i)->getFirstDepart());
        }
        furtherAvailable |= (*i)->moreAvailable();
    }
    myAllLoaded = !furtherAvailable;
}


/****************************************************************************/
