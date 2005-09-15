#ifndef GUIEvent_SimulationEnded_h
#define GUIEvent_SimulationEnded_h
//---------------------------------------------------------------------------//
//                        GUIEvent_SimulationEnded.h -
//  Event send when the the simulation is over
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
// Revision 1.2  2004/12/16 12:21:39  dkrajzew
// debugging
//
// Revision 1.1  2004/11/23 10:38:30  dkrajzew
// debugging
//
// Revision 1.1  2004/03/19 12:56:11  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/12/09 11:22:14  dkrajzew
// errors during simulation are now caught properly
//
// Revision 1.1  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on end and be started with a simulation now;
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "GUIEvent.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * GUIEvent_SimulationEnded
 * Throw from GUIRunThread to GUIApplicationWindow and then further to all
 * displays after a step has been performed
 */
class GUIEvent_SimulationEnded : public GUIEvent {
public:
    enum EndReason {
        /// the simulation has ended as all vehicles have left it
        ER_NO_VEHICLES,

        /// The simulation has ended as the end time step was reached
        ER_END_STEP_REACHED,

        /// The simulation has ended due to an error
        ER_ERROR_IN_SIM,

        /// Someone else has forced the simulation to quit
        ER_FORCED

    };

    /// constructor
    GUIEvent_SimulationEnded(EndReason reason, size_t step);

    /// destructor
    ~GUIEvent_SimulationEnded();

    /// Returns the time step the simulation has ended at
    size_t getTimeStep() const;

    /// Returns the reason the simulation has ended due
    EndReason getReason() const;

protected:
    /// The reason the simulation has ended
    EndReason myReason;

    /// The time step the simulation has ended at
    size_t myStep;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

