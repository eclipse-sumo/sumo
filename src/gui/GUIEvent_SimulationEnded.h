/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIEvent_SimulationEnded.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 19 Jun 2003
/// @version $Id$
///
// Event sent when the the simulation is over
/****************************************************************************/
#ifndef GUIEvent_SimulationEnded_h
#define GUIEvent_SimulationEnded_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/events/GUIEvent.h>
#include <utils/common/SUMOTime.h>
#include <microsim/MSNet.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIEvent_SimulationEnded
 * @brief Event sent when the the simulation is over
 *
 * Throw from GUIRunThread to GUIApplicationWindow.
 */
class GUIEvent_SimulationEnded : public GUIEvent {
public:
    /** @brief Constructor
     * @param[in] reason The reason the simulation has ended
     * @param[in] step The time step the simulation has ended at
     */
    GUIEvent_SimulationEnded(MSNet::SimulationState reason, SUMOTime step)
        : GUIEvent(EVENT_SIMULATION_ENDED), myReason(reason), myStep(step) {}


    /// @brief Destructor
    ~GUIEvent_SimulationEnded() { }


    /** @brief Returns the time step the simulation has ended at
     * @return The time step the simulation has ended at
     */
    SUMOTime getTimeStep() const {
        return myStep;
    }


    /** @brief Returns the reason the simulation has ended due
     * @return The reason the simulation has ended
     */
    MSNet::SimulationState getReason() const {
        return myReason;
    }


protected:
    /// @brief The reason the simulation has ended
    MSNet::SimulationState myReason;

    /// @brief The time step the simulation has ended at
    SUMOTime myStep;


};


#endif

/****************************************************************************/

