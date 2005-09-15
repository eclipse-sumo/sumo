//---------------------------------------------------------------------------//
//                        GUIEvent_SimulationEnded.cpp -
//  Event sent when the the simulation is over
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 19 Jun 2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// $Log$
// Revision 1.3  2005/09/15 12:19:22  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 09:17:24  dkrajzew
// recent changes
//
// Revision 1.1  2004/11/23 10:38:30  dkrajzew
// debugging
//
// Revision 1.1  2004/03/19 12:56:11  dkrajzew
// porting to FOX
//
// Revision 1.1  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on
//  end and be started with a simulation now;
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fx.h>
#include "GUIEvent_Message.h"
#include "GUIEvent_SimulationEnded.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIEvent_SimulationEnded::GUIEvent_SimulationEnded(EndReason reason,
                                                   size_t step)
    : GUIEvent(EVENT_SIMULATION_ENDED),
    myReason(reason), myStep(step)
{
}


GUIEvent_SimulationEnded::~GUIEvent_SimulationEnded()
{
}


size_t
GUIEvent_SimulationEnded::getTimeStep() const
{
    return myStep;
}


GUIEvent_SimulationEnded::EndReason
GUIEvent_SimulationEnded::getReason() const
{
    return myReason;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

