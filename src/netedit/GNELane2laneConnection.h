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
/// @file    GNELane2laneConnection.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2019
///
// File for lane2lane geometry classes and functions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/div/GUIGeometry.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief class lane2lane connection geometry
class GNELane2laneConnection {

public:
    /// @brief constructor
    GNELane2laneConnection(const GNELane* fromLane);

    /// @brief update
    void updateLane2laneConnection();

    /// @brief check if exist a lane2lane geometry for the given toLane
    bool exist(const GNELane* toLane) const;

    /// @brief get lane2lane geometry
    const GUIGeometry& getLane2laneGeometry(const GNELane* toLane) const;

protected:
    /// @brief from lane
    const GNELane* myFromLane;

    /// @brief connection shape
    std::map<const GNELane*, GUIGeometry> myConnectionsMap;

private:
    /// @brief constructor
    GNELane2laneConnection();

    /// @brief Invalidated assignment operator
    GNELane2laneConnection& operator=(const GNELane2laneConnection& other) = delete;
};
