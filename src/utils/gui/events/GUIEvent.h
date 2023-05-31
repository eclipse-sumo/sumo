/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIEvent.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
///
// Definition of an own event class
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXThreadEvent.h>
#include <utils/foxtools/MFXBaseObject.h>
#include <utils/foxtools/fxheader.h>


/**
 * As events are distinguished by their number, here is the enumeration
 * of our custom events
 */
enum class GUIEventType {
    /// @brief send when a simulation has been loaded
    SIMULATION_LOADED,

    /// @brief send when a simulation step has been performed
    SIMULATION_STEP,

    /// @brief send when a message occured
    MESSAGE_OCCURRED,

    /// @brief send when a warning occured
    WARNING_OCCURRED,

    /// @brief send when a error occured
    ERROR_OCCURRED,

    /// @brief send when a debug occured
    DEBUG_OCCURRED,

    /// @brief send when a gldebug occured
    GLDEBUG_OCCURRED,

    /// @brief send when a status change occured
    STATUS_OCCURRED,

    /**@brief Send when a new should be opened (via TraCI) */
    ADD_VIEW,

    /**@brief Send when a view should be closed (via TraCI) */
    CLOSE_VIEW,

    /**@brief Send when the simulation is over;
     * @note The reason and the time step are stored within the event
    */
    SIMULATION_ENDED,

    /// @brief send when a tool produces output
    OUTPUT_OCCURRED,

    /// @brief send when a tool finishes
    TOOL_ENDED,

    /// @brief End of events list; use this to define new
    END
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
