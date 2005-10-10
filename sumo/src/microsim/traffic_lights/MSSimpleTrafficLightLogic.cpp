//---------------------------------------------------------------------------//
//                        MSSimpleTrafficLightLogic.cpp -
//  The basic traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.7  2005/10/10 11:56:09  dkrajzew
// reworking the tls-API: made tls-control non-static; made net an element of traffic lights
//
// Revision 1.6  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 11:09:53  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:22:19  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/01 10:10:46  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.1  2004/11/23 10:18:42  dkrajzew
// all traffic lights moved to microsim/traffic_lights
//
// Revision 1.11  2003/11/24 10:21:21  dkrajzew
// some documentation added and dead code removed
//
// Revision 1.10  2003/10/01 13:58:26  dkrajzew
// unneded methods outcommented; new phase mask usage applied
//
// Revision 1.9  2003/09/22 12:33:12  dkrajzew
// actuated traffic lights are now derived from simple traffic lights
//
// Revision 1.8  2003/09/17 06:50:45  dkrajzew
// phase definitions extracted from traffic lights;
//  MSActuatedPhaseDefinition is now derived from MSPhaseDefinition
//
// Revision 1.7  2003/08/04 11:40:20  dkrajzew
// false inclusion hierarchy patched; missing inclusions added
//
// Revision 1.6  2003/06/06 10:39:16  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.5  2003/06/05 16:07:35  dkrajzew
// new usage of traffic lights implemented
//
// Revision 1.4  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.3  2003/04/04 07:13:20  dkrajzew
// Yellow phases must be now explicetely given
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * static member variables definitions
 * ======================================================================= */
std::bitset<64> MSSimpleTrafficLightLogic::_allClear;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSSimpleTrafficLightLogic::MSSimpleTrafficLightLogic(MSNet &net,
                                                     const std::string &id,
                                                     const Phases &phases,
                                                     size_t step,
                                                     size_t delay)
    : MSTrafficLightLogic(net, id, delay), myPhases(phases),
    myStep(step)
{
}


MSSimpleTrafficLightLogic::~MSSimpleTrafficLightLogic()
{
    for(size_t i=0; i<myPhases.size(); i++) {
        delete myPhases[i];
    }
}


const std::bitset<64> &
MSSimpleTrafficLightLogic::linkPriorities() const
{
    assert(myPhases.size()>myStep);
    return myPhases[myStep]->getBreakMask();
}


const std::bitset<64> &
MSSimpleTrafficLightLogic::yellowMask() const
{
    assert(myPhases.size()>myStep);
    return myPhases[myStep]->getYellowMask();
}


const std::bitset<64> &
MSSimpleTrafficLightLogic::allowed() const
{
    assert(myPhases.size()>myStep);
    return myPhases[myStep]->getDriveMask();
}


size_t
MSSimpleTrafficLightLogic::nextStep()
{
    // increment the index to the current phase
    myStep++;
    if(myStep==myPhases.size()) {
        myStep = 0;
    }
    return myStep;
}


SUMOTime
MSSimpleTrafficLightLogic::duration() const
{
    assert(myPhases.size()>myStep);
    return myPhases[myStep]->duration;
}


SUMOTime
MSSimpleTrafficLightLogic::nextPhase()
{
    // increment the index to the current phase
    nextStep();
    // reset the link priorities
    setLinkPriorities();
    // set the next event
    return duration();
}


size_t
MSSimpleTrafficLightLogic::step() const
{
    return myStep;
}


const MSSimpleTrafficLightLogic::Phases &
MSSimpleTrafficLightLogic::getPhases() const
{
    return myPhases;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


