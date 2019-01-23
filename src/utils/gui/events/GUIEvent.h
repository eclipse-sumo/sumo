/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/FXBaseObject.h>
#include <fx.h>


/**
 * As events are distinguished by their number, here is the enumeration
 * of our custom events
 */
enum GUIEventType {
    /// @brief send when a simulation has been loaded
    EVENT_SIMULATION_LOADED,

    /// @brief send when a simulation step has been performed
    EVENT_SIMULATION_STEP,

    /// @brief send when a message occured
    EVENT_MESSAGE_OCCURRED,

    /// @brief send when a warning occured
    EVENT_WARNING_OCCURRED,

    /// @brief send when a error occured
    EVENT_ERROR_OCCURRED,

    /// @brief send when a debug occured
    EVENT_DEBUG_OCCURRED,

    /// @brief send when a gldebug occured
    EVENT_GLDEBUG_OCCURRED,

    /// @brief send when a status change occured
    EVENT_STATUS_OCCURRED,

    /**@brief Send when the simulation is over;
     * @note The reason and the time step are stored within the event
    */
    EVENT_SIMULATION_ENDED,

    /// @brief End of events list; use this to define new
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
    /// @brief returns the event type
    GUIEventType getOwnType() const {
        return myType;
    }

    /// @brief destructor
    virtual ~GUIEvent() { }

protected:
    /// @brief constructor
    GUIEvent(GUIEventType ownType) :
        myType(ownType) { }

    /// @brief the type of the event
    GUIEventType myType;
};


#endif

/****************************************************************************/

