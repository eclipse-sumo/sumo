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
// Revision 1.10  2006/01/09 11:56:21  dkrajzew
// includes debugged
//
// Revision 1.9  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.5  2004/11/23 10:20:09  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.4  2004/07/02 09:58:08  dkrajzew
// MeanData refactored (moved to microsim/output); numerical id for online routing added
//
// Revision 1.3  2003/12/11 06:27:13  dkrajzew
// unneeded debug-variables removed
//
// Revision 1.2  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "output/MSInductLoop.h"
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
MSInternalLane::MSInternalLane( /*MSNet &net, */string id, SUMOReal maxSpeed,
                               SUMOReal length, MSEdge *e, size_t numericalID, const Position2DVector &shape)
    :
    MSLane(/*net, */id, maxSpeed, length, e, numericalID, shape)
{
}


MSInternalLane::~MSInternalLane()
{
}


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
    (*myFoesCont)[myFoesIndex] = true;
    MSLane::moveNonCritical();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
