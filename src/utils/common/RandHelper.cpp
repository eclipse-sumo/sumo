/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RandHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/common/SysUtils.h>
#include "RandHelper.h"


// ===========================================================================
// static member variables
// ===========================================================================
std::mt19937 RandHelper::myRandomNumberGenerator;
int RandHelper::myCallCount(0);
int RandHelper::myDebugIndex(7);


// ===========================================================================
// member method definitions
// ===========================================================================
void
RandHelper::insertRandOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    // registers random number options
    oc.addOptionSubTopic("Random Number");

    oc.doRegister("random", new Option_Bool(false));
    oc.addSynonyme("random", "abs-rand", true);
    oc.addDescription("random", "Random Number", "Initialises the random number generator with the current system time");

    oc.doRegister("seed", new Option_Integer(23423));
    oc.addSynonyme("seed", "srand", true);
    oc.addDescription("seed", "Random Number", "Initialises the random number generator with the given value");
}


void
RandHelper::initRand(std::mt19937* which, const bool random, const int seed) {
    if (which == nullptr) {
        which = &myRandomNumberGenerator;
    }
    if (random) {
        which->seed((unsigned long)time(nullptr));
    } else {
        which->seed(seed);
    }
}


void
RandHelper::initRandGlobal(std::mt19937* which) {
    OptionsCont& oc = OptionsCont::getOptions();
    initRand(which, oc.getBool("random"), oc.getInt("seed"));
}


/****************************************************************************/

