/****************************************************************************/
/// @file    RandHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <utils/options/OptionsCont.h>
#include "RandHelper.h"
#include <ctime>
#include <cmath>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
MTRand RandHelper::myRandomNumberGenerator;


// ===========================================================================
// member method definitions
// ===========================================================================
void
RandHelper::insertRandOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
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
RandHelper::initRandGlobal() {
    OptionsCont &oc = OptionsCont::getOptions();
    if (oc.getBool("random")) {
        myRandomNumberGenerator.seed();
    } else {
        myRandomNumberGenerator.seed(oc.getInt("seed"));
    }
}


/****************************************************************************/

