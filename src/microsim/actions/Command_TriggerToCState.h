/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Command_TriggerToCState.h
/// @author  Leonhard Luecken
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    06 Apr 2018
/// @version $Id$
///
// Trigger an MRM on the associated MSDevice_ToC
/****************************************************************************/
#ifndef Command_TriggerToCState_h
#define Command_TriggerToCState_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/common/Command.h>
#include <microsim/devices/MSDevice_ToC.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_TriggerToCState
 * @brief Writes the state of the tls to a file (in each second)
 */
class Command_TriggerToCState : public Command {
public:
    /** @brief Constructor
     *
     * @param[in] device The device that issued the command
     * @param[in] timeTillExecution The time offset until the callback is performed
     */
    Command_TriggerToCState(MSDevice_ToC* tocDevice, MSDevice_ToC::ToCState targetState, SUMOTime timeTillExecution);

    /// @brief Destructor
    ~Command_TriggerToCState();

    /// @name Derived from Command
    /// @{

    /** @brief
     *
     * @param[in] currentTime The current simulation time (unused)
     * @return Always DELTA_T (will be executed in next time step)
     * @see Command
     */
    SUMOTime execute(SUMOTime currentTime);
    /// @}


protected:
    /// @brief The traffic light logic to use
    MSDevice_ToC* myDevice;

    /// @brief The target state, which is triggered by the command in {MANUAL, AUTOMATED}
    MSDevice_ToC::ToCState myTargetState;


private:
    /// @brief Invalidated copy constructor.
    Command_TriggerToCState(const Command_TriggerToCState&);

    /// @brief Invalidated assignment operator.
    Command_TriggerToCState& operator=(const Command_TriggerToCState&);

};


#endif

/****************************************************************************/

