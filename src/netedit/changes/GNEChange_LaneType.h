/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_LaneType.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
// A network change in which a single laneType is created or deleted
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEChange.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_LaneType
 * A network change in which a single laneType is created or deleted
 */
class GNEChange_LaneType : public GNEChange {
    // @brief FOX Declaration
    FXDECLARE_ABSTRACT(GNEChange_LaneType)

public:
    /**@brief Constructor for creating/deleting an laneType
     * @param[in] laneType The laneType to be created/deleted
     * @param[in] position lane position
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_LaneType(GNELaneType* laneType, const int position, bool forward);

    /// @brief Destructor
    ~GNEChange_LaneType();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    FXString undoName() const;

    /// @brief get Redo name
    FXString redoName() const;

    /// @brief undo action
    void undo();

    /// @brief redo action
    void redo();
    /// @}

private:
    /**@brief full information regarding the laneType that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNELaneType* myLaneType;

    /// @brief position in edgeType Parent
    const int myPosition;
};
