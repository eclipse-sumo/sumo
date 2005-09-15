#ifndef GUIEvent_SimulationStep_h
#define GUIEvent_SimulationStep_h
//---------------------------------------------------------------------------//
//                        GUIEvent_SimulationStep.h -
//  Event send when the a simulation step has been performed by GUIRunThread
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
// Revision 1.2  2005/09/15 12:19:22  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:30  dkrajzew
// debugging
//
// Revision 1.1  2004/03/19 12:56:11  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/02/07 10:34:15  dkrajzew
// files updated
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
 * GUIEvent_SimulationStep
 * Throw from GUIRunThread to GUIApplicationWindow and then further to all
 * displays after a step has been performed
 */
class GUIEvent_SimulationStep : public GUIEvent {
public:
    /// constructor
    GUIEvent_SimulationStep()
        : GUIEvent(EVENT_SIMULATION_STEP) { }

    /// destructor
    ~GUIEvent_SimulationStep() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

