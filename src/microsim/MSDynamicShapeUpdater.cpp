/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2019-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDynamicShapeUpdater.cpp
/// @author  Leonhard Luecken
/// @date    08 Apr 2019
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include "MSDynamicShapeUpdater.h"
#include "utils/vehicle/SUMOVehicle.h"

//#define DEBUG_DYNAMIC_SHAPES

void
MSDynamicShapeUpdater::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& /* info */) {

    /// @brief Definition of a vehicle state
    switch (to) {
        /// @brief The vehicle arrived at his destination (is deleted)
        case MSNet::VEHICLE_STATE_ARRIVED: {
#ifdef DEBUG_DYNAMIC_SHAPES
            std::cout << "MSDynamicShapeUpdater: Vehicle '" << vehicle->getID() << "' has arrived. Informing ShapeContainer." << std::endl;
#endif
            myShapeContainer.removeTrackers(vehicle->getID());
        }
        break;
        /// @brief The vehicle is involved in a collision
        case MSNet::VEHICLE_STATE_COLLISION:
            // TODO: can this lead to removal without calling arrived?
            break;
        default:
        {}
    }
}
