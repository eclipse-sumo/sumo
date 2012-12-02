/****************************************************************************/
/// @file    MSRouteLoaderControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 06 Nov 2002
/// @version $Id$
///
// Class responsible for loading of routes from some files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include "MSRouteLoader.h"
#include "MSRouteLoaderControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSRouteLoaderControl::MSRouteLoaderControl(MSNet&, SUMOTime inAdvanceStepNo, LoaderVector loader):
    myLastLoadTime(-inAdvanceStepNo),
    myInAdvanceStepNo(inAdvanceStepNo),
    myRouteLoaders(loader),
    myAllLoaded(false) {
    myLoadAll = myInAdvanceStepNo <= 0;
    myAllLoaded = false;
    myLastLoadTime = -1 * (int) myInAdvanceStepNo;
    // initialize all used loaders
    for (LoaderVector::iterator i = myRouteLoaders.begin();
            i != myRouteLoaders.end(); ++i) {
        (*i)->init();
    }
}


MSRouteLoaderControl::~MSRouteLoaderControl() {
    for (LoaderVector::iterator i = myRouteLoaders.begin();
            i != myRouteLoaders.end(); ++i) {
        delete(*i);
    }
}


void
MSRouteLoaderControl::loadNext(SUMOTime step) {
    // check whether new vehicles shall be loaded
    //  return if not
    if (myAllLoaded) {
        return;
    }
    SUMOTime loadMaxTime = myLoadAll ? SUMOTime_MAX : step + myInAdvanceStepNo;

    // load all routes for the specified time period
    bool furtherAvailable = false;
    for (LoaderVector::iterator i = myRouteLoaders.begin(); i != myRouteLoaders.end(); ++i) {
        (*i)->loadUntil(loadMaxTime);
        furtherAvailable |= (*i)->moreAvailable();
    }
    myAllLoaded = !furtherAvailable;
}



/****************************************************************************/

