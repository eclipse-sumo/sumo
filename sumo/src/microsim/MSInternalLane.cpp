/****************************************************************************/
/// @file    MSInternalLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// Class representing junction-internal lanes
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
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// member method definitions
// ===========================================================================
MSInternalLane::MSInternalLane(/*MSNet &net, */string id, SUMOReal maxSpeed,
        SUMOReal length, MSEdge *e, size_t numericalID,
        const Position2DVector &shape,
        const std::vector<SUMOVehicleClass> &allowed,
        const std::vector<SUMOVehicleClass> &disallowed)
        :
        MSLane(id, maxSpeed, length, e, numericalID, shape, allowed, disallowed),
        myFoesIndex(-1)
{}


MSInternalLane::~MSInternalLane()
{}


void
MSInternalLane::setParentJunctionInformation(
    MSLogicJunction::InnerState *foescont, size_t foesIdx)
{
    myFoesCont = foescont;
    myFoesIndex = foesIdx;
}


void
MSInternalLane::moveNonCritical()
{
    assert(myVehicles.size()>0);
    if (myFoesIndex>=0) {
        (*myFoesCont)[myFoesIndex] = true;
    }
    MSLane::moveNonCritical();
}


void
MSInternalLane::setPassPosition(SUMOReal passPos)
{
    myPassPosition = passPos;
}



/****************************************************************************/

