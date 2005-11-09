#ifndef MSSimpleTrafficLightLogic_h
#define MSSimpleTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSSimpleTrafficLightLogic.h -
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
// Revision 1.8  2005/11/09 06:36:48  dkrajzew
// changing the LSA-API: MSEdgeContinuation added; changed the calling API
//
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
// Revision 1.14  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.13  2003/11/24 10:21:21  dkrajzew
// some documentation added and dead code removed
//
// Revision 1.12  2003/10/01 13:58:26  dkrajzew
// unneded methods outcommented; new phase mask usage applied
//
// Revision 1.11  2003/09/22 12:33:12  dkrajzew
// actuated traffic lights are now derived from simple traffic lights
//
// Revision 1.10  2003/09/17 06:50:45  dkrajzew
// phase definitions extracted from traffic lights;
//  MSActuatedPhaseDefinition is now derived from MSPhaseDefinition
//
// Revision 1.9  2003/07/30 09:16:10  dkrajzew
// a better (correct?) processing of yellow lights added; debugging
//
// Revision 1.8  2003/06/06 10:39:16  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.7  2003/06/05 16:07:36  dkrajzew
// new usage of traffic lights implemented
//
// Revision 1.6  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.5  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok);
//  detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.4  2003/04/04 07:13:20  dkrajzew
// Yellow phases must be now explicetely given
//
// Revision 1.3  2003/03/03 14:56:25  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
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

#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "MSPhaseDefinition.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSSimpleTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
class MSSimpleTrafficLightLogic : public MSTrafficLightLogic
{
public:
    /// definition of a list of phases, being the junction logic
    typedef std::vector<MSPhaseDefinition*> Phases;

public:
    /// constructor
    MSSimpleTrafficLightLogic(MSNet &net, const std::string &id,
        const Phases &phases, size_t step, size_t delay);

    /// destructor
    ~MSSimpleTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    SUMOTime trySwitch();

    /// Returns the priorities for all lanes for the current phase
    const std::bitset<64> &linkPriorities() const;

    /// Returns a bitset where all links having yellow are set
    const std::bitset<64> &yellowMask() const;

    const std::bitset<64> &allowed() const;

	/// returns the current step
	size_t getStepNo() const;

    /// Returns the phases of this tls
    const Phases &getPhases() const;

protected:
    /// the list of phases this logic uses
    Phases myPhases;

    /// The current step
    size_t myStep;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

