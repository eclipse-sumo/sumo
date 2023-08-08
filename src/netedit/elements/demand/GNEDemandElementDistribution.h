/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNEDemandElementDistribution.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// A abstract class for demand elements distributions
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNEMoveElement.h>
#include <netedit/GNEPathManager.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/common/Parameterised.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GUIGeometry.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/vehicle/SUMOVehicleParameter.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;
class GNEAdditional;
class GNEDemandElementDistribution;
class GNENetworkElement;
class GNEGenericData;
class GNEEdge;
class GNELane;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDemandElementDistribution
 * @brief An Element which don't belong to GNENet but has influence in the simulation
 */
class GNEDemandElementDistribution {

public:

    /**@brief Constructor
     */
    GNEDemandElementDistribution();

private:

    /// @brief Invalidated copy constructor.
    GNEDemandElementDistribution(const GNEDemandElementDistribution&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementDistribution& operator=(const GNEDemandElementDistribution&) = delete;
};
