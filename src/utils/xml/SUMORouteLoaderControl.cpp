/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <utils/common/StdDefs.h>
#include "SUMORouteLoader.h"
#include "SUMORouteLoaderControl.h"


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
    for (auto & myRouteLoader : myRouteLoaders) {
        delete myRouteLoader;
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
    for (auto & myRouteLoader : myRouteLoaders) {
        myCurrentLoadTime = MIN2(myCurrentLoadTime, myRouteLoader->loadUntil(loadMaxTime));
        if (myRouteLoader->getFirstDepart() != -1) {
            myFirstLoadTime = MIN2(myFirstLoadTime, myRouteLoader->getFirstDepart());
        }
        furtherAvailable |= myRouteLoader->moreAvailable();
    }
    myAllLoaded = !furtherAvailable;
}


/****************************************************************************/
