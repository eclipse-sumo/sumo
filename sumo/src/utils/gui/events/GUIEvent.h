#ifndef GUIEvent_h
#define GUIEvent_h
//---------------------------------------------------------------------------//
//                        GUIEvent.h -
//  Definition of an own event class
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
// Revision 1.3  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
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

#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/FXBaseObject.h>
#include <fx.h>


/**
 * As events are distinguished by their number, here is the enumeration
 * of our custom events
 */
enum GUIEventType {
    /// send when a simulation has been loaded
    EVENT_SIMULATION_LOADED,

    /// send when a simulation step has been performed
    EVENT_SIMULATION_STEP,

    /// send when a message occured
    EVENT_MESSAGE_OCCURED,

    /// send when a warning occured
    EVENT_WARNING_OCCURED,

    /// send when a error occured
    EVENT_ERROR_OCCURED,

    /** @brief Send when the simulation is over;
        The reason and the time step are stored within the event */
    EVENT_SIMULATION_ENDED,

    EVENT_MAX

};


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * GUIEvent
 *
 */
class GUIEvent {
public:
    /// returns the event type
    GUIEventType getOwnType() const { return myType; }

    /// destructor
    virtual ~GUIEvent() { }

protected:
    /// constructor
    GUIEvent(GUIEventType ownType)
        : myType(ownType) { }


protected:
    /// the type of the event
    GUIEventType myType;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

