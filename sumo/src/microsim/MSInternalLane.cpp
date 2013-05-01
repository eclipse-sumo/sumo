/****************************************************************************/
/// @file    MSInternalLane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2003
/// @version $Id$
///
// Representation of a lane over a junction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
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

#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSLink.h"
#include "MSInternalLane.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSInternalLane::MSInternalLane(const std::string& id, SUMOReal maxSpeed,
                               SUMOReal length, MSEdge* const edge, unsigned int numericalID,
                               const PositionVector& shape, SUMOReal width,
                               SVCPermissions permissions):
    MSLane(id, maxSpeed, length, edge, numericalID, shape, width, permissions),
    myFoesIndex(-1)
{}


MSInternalLane::~MSInternalLane() {}


void
MSInternalLane::setParentJunctionInformation(MSLogicJunction::InnerState* const foescont,
        unsigned int foesIdx) {
    myFoesCont = foescont;
    myFoesIndex = (int) foesIdx;
}


bool
MSInternalLane::planMovements(SUMOTime t) {
    assert(myVehicles.size() > 0);
    if (myFoesIndex >= 0) {
        (*myFoesCont)[myFoesIndex] = true;
    }
    return MSLane::planMovements(t);
}


/****************************************************************************/

