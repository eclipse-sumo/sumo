/****************************************************************************/
/// @file    MEVehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MEVehicle.cpp 96 2007-06-06 07:40:46Z behr_mi $
///
// A vehicle from the mesoscopic point of view
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

#ifdef HAVE_MESOSIM

#include <utils/common/StdDefs.h>
#include <iostream>
#include <cassert>
#include <microsim/MSVehicle.h>
#include "MEVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MEVehicle::MEVehicle(MESegment *s, SUMOReal t)
    : seg(s), tEvent(t), inserted(false)
{
}


MEVehicle::MEVehicle(MSVehicle *cor, MESegment *s, SUMOReal t)
    : mySUMOVehicle(cor), seg(s), tEvent(t), inserted(false)
{
}


SUMOReal
MEVehicle::get_tEvent()
{
    return tEvent;
}


MESegment *
MEVehicle::at_segment()
{
    return seg;
}


void
MEVehicle::update_tEvent(SUMOReal t)
{
//    assert(tEvent<=t);
    tEvent = t;
}


void
MEVehicle::update_segment(MESegment *s)
{
    seg = s;
}


MSVehicle * const
MEVehicle::getSUMOVehicle() const
{
    return mySUMOVehicle;
}


void
MEVehicle::setTLastEntry(SUMOReal t)
{
    tLastEntry = t;
}


SUMOReal
MEVehicle::getNeededTime() const
{
    return tEvent-tLastEntry;
}


SUMOReal
MEVehicle::me_length() const
{
    return mySUMOVehicle->getLength();
}

#endif // HAVE_MESOSIM



/****************************************************************************/

