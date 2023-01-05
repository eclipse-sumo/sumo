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
/// @file    Command_RouteReplacement.h
/// @author  Jakob Erdmann
/// @date    1 Feb 2022
///
// Replaces a vehicles route at a pre-determined time
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>
#include <utils/common/Command.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSRoute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_RouteReplacement
 * @brief Writes the state of the tls to a file (in each second)
 */
class Command_RouteReplacement : public Command {
public:
    /** @brief Constructor
     *
     * @param[in] tlls The logic to write state of
     * @param[in] od The output device to write the state into
     */
    Command_RouteReplacement(const std::string& vehID, ConstMSRoutePtr route);

    /// @brief Destructor
    ~Command_RouteReplacement();


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

private:

    /// @brief The id of the vehicle to be rerouted
    std::string myVehID;

    /// @brief The replacement route
    ConstMSRoutePtr myRoute;


private:
    /// @brief Invalidated copy constructor.
    Command_RouteReplacement(const Command_RouteReplacement&);

    /// @brief Invalidated assignment operator.
    Command_RouteReplacement& operator=(const Command_RouteReplacement&);

};
