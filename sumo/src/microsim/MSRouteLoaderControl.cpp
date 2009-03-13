/****************************************************************************/
/// @file    MSRouteLoaderControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 06 Nov 2002
/// @version $Id$
///
// Class responsible for loading of routes from some files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include "MSRouteLoader.h"
#include "MSRouteLoaderControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSRouteLoaderControl::MSRouteLoaderControl(MSNet &,
        int inAdvanceStepNo,
        LoaderVector loader)
        : myLastLoadTime(-inAdvanceStepNo),
        myInAdvanceStepNo(inAdvanceStepNo),
        myRouteLoaders(loader),
        myVehCont(inAdvanceStepNo>0 ? inAdvanceStepNo : 10), // !!! einen schalueren Wert!
        myAllLoaded(false) {
    myLoadAll = myInAdvanceStepNo<=0;
    myAllLoaded = false;
    myLastLoadTime = -1 * (int) myInAdvanceStepNo;
    // initialize all used loaders
    for (LoaderVector::iterator i=myRouteLoaders.begin();
            i!=myRouteLoaders.end(); ++i) {
        (*i)->init();
    }
}


MSRouteLoaderControl::~MSRouteLoaderControl() {
    for (LoaderVector::iterator i=myRouteLoaders.begin();
            i!=myRouteLoaders.end(); ++i) {
        delete(*i);
    }
}


MSVehicleContainer &
MSRouteLoaderControl::loadNext(SUMOTime step) {
    // check whether new vehicles shall be loaded
    //  return if not
    if ((myLoadAll&&myAllLoaded) || (myLastLoadTime>=0&&myLastLoadTime/*+myInAdvanceStepNo*/>=step)) {
        return myVehCont;
    }
    // load all routes for the specified time period
    SUMOTime run = step;
    bool furtherAvailable = true;
    for (;
            furtherAvailable &&
            (myLoadAll||run<=step+myInAdvanceStepNo);
            run++) {
        furtherAvailable = false;
        for (LoaderVector::iterator i=myRouteLoaders.begin();
                i!=myRouteLoaders.end(); ++i) {
            if ((*i)->moreAvailable()) {
                (*i)->loadUntil(run, myVehCont);
            }
            furtherAvailable |= (*i)->moreAvailable();
        }
    }
    // no further loading when all was loaded
    if (myLoadAll||!furtherAvailable) {
        myAllLoaded = true;
    }
    // set the step information
    myLastLoadTime = run - 1;
    // return the container with new vehicles
    return myVehCont;
}



/****************************************************************************/

