/*
 * DynamicShapeUpdater.h
 *
 *  Created on: Apr 8, 2019
 *      Author: leo
 */

#ifndef MICROSIM_MSDYNAMICSHAPEUPDATER_H_
#define MICROSIM_MSDYNAMICSHAPEUPDATER_H_

#include "MSNet.h"
#include "utils/shapes/ShapeContainer.h"

/** @class DynamicShapeUpdater
 * @brief Ensures that dynamic shapes tracking traffic objects are removed when the objects are removed.
 */
class MSDynamicShapeUpdater: public MSNet::VehicleStateListener {
public:
    /// @brief Constructor
    MSDynamicShapeUpdater(ShapeContainer& shapeContainer) : myShapeContainer(shapeContainer){};

    /// @brief Constructor
    void vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& info = "") override;

private:
    ShapeContainer& myShapeContainer;
};

#endif /* MICROSIM_MSDYNAMICSHAPEUPDATER_H_ */
