/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2020 German Aerospace Center (DLR) and others.
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
/// @file    NBVehicle.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
///
// A vehicle as used by router
/****************************************************************************/
#pragma once


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <iostream>


// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBVehicle
 * @brief A vehicle as used by router
 */
class NBVehicle {
public:
    /** @brief Constructor
     *
     * @param[in] pars Parameter of this vehicle
     * @param[in] route The definition of the route the vehicle shall use
     * @param[in] type The type of the vehicle
     */
    NBVehicle(const std::string& id, SUMOVehicleClass vClass):
        myID(id), myVClass(vClass) {}

    const std::string& getID() const {
        return myID;
    }

    SUMOVehicleClass getVClass() const {
        return myVClass;
    }


    /// @brief Destructor
    virtual ~NBVehicle() {}


private:
    /// @brief vehicle ID for error reporting
    std::string myID;

    /// @brief The vehicle class of the
    SUMOVehicleClass myVClass;


private:
    /// @brief Invalidated copy constructor
    NBVehicle(const NBVehicle& src);

    /// @brief Invalidated assignment operator
    NBVehicle& operator=(const NBVehicle& src);

};


/****************************************************************************/
