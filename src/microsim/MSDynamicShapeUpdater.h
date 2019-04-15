/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2019-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDynamicShapeUpdater.h
/// @author  Leonhard Luecken
/// @date    08 Apr 2019
/// @version $Id$
///
//
/****************************************************************************/
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
    MSDynamicShapeUpdater(ShapeContainer& shapeContainer) : myShapeContainer(shapeContainer) {};

    /// @brief Constructor
    void vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& info = "") override;

private:
    ShapeContainer& myShapeContainer;

private:
    /// @brief invalidated assignment operator
    MSDynamicShapeUpdater& operator=(const MSDynamicShapeUpdater& s) = delete;
};

#endif /* MICROSIM_MSDYNAMICSHAPEUPDATER_H_ */
