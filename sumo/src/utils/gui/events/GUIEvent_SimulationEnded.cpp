/****************************************************************************/
/// @file    GUIEvent_SimulationEnded.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 19 Jun 2003
/// @version $Id$
///
// Event sent when the the simulation is over
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <fx.h>
#include "GUIEvent_Message.h"
#include "GUIEvent_SimulationEnded.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIEvent_SimulationEnded::GUIEvent_SimulationEnded(EndReason reason,
        SUMOTime step)
        : GUIEvent(EVENT_SIMULATION_ENDED),
        myReason(reason), myStep(step)
{}


GUIEvent_SimulationEnded::~GUIEvent_SimulationEnded()
{}


SUMOTime
GUIEvent_SimulationEnded::getTimeStep() const
{
    return myStep;
}


GUIEvent_SimulationEnded::EndReason
GUIEvent_SimulationEnded::getReason() const
{
    return myReason;
}



/****************************************************************************/

