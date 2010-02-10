/****************************************************************************/
/// @file    GUIEvent.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Definition of an own event class
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIEvent_h
#define GUIEvent_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

    /// End of events list; use this to define new
    EVENT_END
};


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * GUIEvent
 *
 */
class GUIEvent {
public:
    /// returns the event type
    GUIEventType getOwnType() const {
        return myType;
    }

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


#endif

/****************************************************************************/

