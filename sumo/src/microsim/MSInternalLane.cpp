//---------------------------------------------------------------------------//
//                        MSInternalLane.cpp -
//  Class representing junction-internal lanes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <helpers/PreStartInitialised.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSInductLoop.h"
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
#include <utils/convert/ToString.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSInternalLane::MSInternalLane( MSNet &net, string id, double maxSpeed,
                               double length, MSEdge *e)
    :
    MSLane(net, id, maxSpeed, length, e)
{
}


MSInternalLane::~MSInternalLane()
{
}


void
MSInternalLane::setParentJunctionInformation(
        MSLogicJunction::InnerState *foescont, size_t foesIdx)
{
    if(id()==":597993219_3_0") {
        int bla = 0;
    }
    myFoesCont = foescont;
    myFoesIndex = foesIdx;
}


void
MSInternalLane::moveNonCritical()
{
    if(id()==":597993219_3_0") {
        int bla = 0;
    }
    assert(myVehicles.size()>0);
    (*myFoesCont)[myFoesIndex] = true;
    MSLane::moveNonCritical();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
