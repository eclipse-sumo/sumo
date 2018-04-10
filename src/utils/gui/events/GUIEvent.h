/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIEvent.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Definition of an own event class
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

    /// send when a status change occured
    EVENT_STATUS_OCCURED,

    /** @brief Send when the simulation is over;
        The reason and the time step are stored within the event */
    EVENT_SIMULATION_ENDED,

    /** @brief Send when a screenshot is requested;
        View and file name are stored within the event */
    EVENT_SCREENSHOT,

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

