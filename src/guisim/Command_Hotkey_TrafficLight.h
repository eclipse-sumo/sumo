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
/// @file    Command_Hotkey_TrafficLight.h
/// @author  Jakob Erdmann
/// @date    31 Jan 2022
///
// Registers custom hotkey for aborting current traffic light phase
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <utils/common/Command.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrafficLightLogic;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_Hotkey_TrafficLight
 * @brief Set/unsets inductionloop override
 */
class Command_Hotkey_TrafficLight : public Command {
public:
    /** @brief Constructor
     *
     * @param[in] tll The traffic light logic to manipulate
     */
    Command_Hotkey_TrafficLight(MSTrafficLightLogic& tll);

    /// @brief Destructor
    ~Command_Hotkey_TrafficLight();


    /// @name Derived from Command
    /// @{

    /** @brief toggles override
     *
     * Returns always 1.
     *
     * @param[in] currentTime The current simulation time (unused)
     * @return Always DELTA_T (will be executed in next time step)
     * @see Command
     */
    SUMOTime execute(SUMOTime currentTime);
    /// @}

    /// @brief register hotkey actions
    static bool registerHotkey(const std::string& key, MSTrafficLightLogic& tll);

private:

    /// @brief The traffic light logic to be manipulated
    MSTrafficLightLogic& myLogic;

private:
    /// @brief Invalidated copy constructor.
    Command_Hotkey_TrafficLight(const Command_Hotkey_TrafficLight&);

    /// @brief Invalidated assignment operator.
    Command_Hotkey_TrafficLight& operator=(const Command_Hotkey_TrafficLight&);

};
