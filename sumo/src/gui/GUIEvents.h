#ifndef GUIEvents_h
#define GUIEvents_h
//---------------------------------------------------------------------------//
//                        GUIEvents.h -
//  An enumeration of SUMO-Events
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
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qevent.h>


/* =========================================================================
 * our own event enumeration
 * ======================================================================= */
/**
 * As events are distinguished by their number, here is the enumeration
 * of our custom events
 */
enum GUIEvent {
    /// thrown when a simulation has been loaded
    EVENT_SIMULATION_LOADED,
    /// thrown when a simulation step has been performed
    EVENT_SIMULATION_STEP
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIEvents.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

