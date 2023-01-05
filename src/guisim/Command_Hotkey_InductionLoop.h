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
/// @file    Command_Hotkey_InductionLoop.h
/// @author  Jakob Erdmann
/// @date    31 Jan 2022
///
// Registers custom hotkey for overriding detector value
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>
#include <utils/common/Command.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSInductLoop;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_Hotkey_InductionLoop
 * @brief Set/unsets inductionloop override
 */
class Command_Hotkey_InductionLoop : public Command {
public:
    /** @brief Constructor
     *
     * @param[in] det The Detector to manipulate
     * @param[in] set whether to set or unset the override
     */
    Command_Hotkey_InductionLoop(MSInductLoop* det, bool set);

    /// @brief Destructor
    ~Command_Hotkey_InductionLoop();


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
    static bool registerHotkey(const std::string& key, MSInductLoop* det);

private:

    /// @brief The detector to be manipulated
    MSInductLoop* myDetector;

    /// @brief whether the override shall be set or reset
    bool mySet;


private:
    /// @brief Invalidated copy constructor.
    Command_Hotkey_InductionLoop(const Command_Hotkey_InductionLoop&);

    /// @brief Invalidated assignment operator.
    Command_Hotkey_InductionLoop& operator=(const Command_Hotkey_InductionLoop&);

};
