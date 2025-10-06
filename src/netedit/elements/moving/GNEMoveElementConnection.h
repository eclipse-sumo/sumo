/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEMoveElementConnection.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving connection shapes
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEConnection;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementConnection : public GNEMoveElement {

public:
    /// @brief constructor
    GNEMoveElementConnection(GNEConnection* connection);

    //// @brief empty destructor
    ~GNEMoveElementConnection();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    GNEMoveOperation* getMoveOperation();;

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);;

protected:
    /// @brief pointer to connection
    GNEConnection* myConnection = nullptr;

private:
    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief invalidate default constructor
    GNEMoveElementConnection() = delete;

    /// @brief Invalidated copy constructor.
    GNEMoveElementConnection(const GNEMoveElementConnection&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElementConnection& operator=(const GNEMoveElementConnection&) = delete;
};
