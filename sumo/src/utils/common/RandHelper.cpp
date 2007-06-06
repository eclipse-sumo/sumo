/****************************************************************************/
/// @file    RandHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
RandHelper::insertRandOptions(OptionsCont &oc)
{
    // registers random number options
    oc.addOptionSubTopic("Random Number");

    oc.doRegister("srand", new Option_Integer(23423));
    oc.addDescription("srand", "Random Number", "Initialises the random number generator with the given value");

    oc.doRegister("abs-rand", new Option_Bool(false));
    oc.addDescription("abs-rand", "Random Number", "Initialises the random number generator with the current system time");
}


void
RandHelper::initRandGlobal(const OptionsCont &options)
{
    if (options.getBool("abs-rand")) {
        myRandomNumberGenerator.seed();
    } else {
        myRandomNumberGenerator.seed(options.getInt("srand"));
    }
}


/****************************************************************************/

