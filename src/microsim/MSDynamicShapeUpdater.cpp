/*
 * DynamicShapeUpdater.cpp
 *
 *  Created on: Apr 8, 2019
 *      Author: leo
 */

#include "MSDynamicShapeUpdater.h"
#include "utils/vehicle/SUMOVehicle.h"

void
MSDynamicShapeUpdater::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& /* info */) {

    /// @brief Definition of a vehicle state
    switch (to) {
        /// @brief The vehicle arrived at his destination (is deleted)
    case MSNet::VEHICLE_STATE_ARRIVED:
    {
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
    default: {}
    }
}
