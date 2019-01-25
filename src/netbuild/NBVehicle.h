/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBVehicle.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
/****************************************************************************/
#ifndef NBVehicle_h
#define NBVehicle_h


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


#endif

/****************************************************************************/
