/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
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
    for (std::vector<SUMORouteLoader*>::iterator i = myRouteLoaders.begin();
            i != myRouteLoaders.end(); ++i) {
        delete (*i);
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
