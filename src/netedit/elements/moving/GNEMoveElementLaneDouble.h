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
/// @file    GNEMoveElementLaneDouble.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over a lane with two positions
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementLaneDouble : public GNEMoveElement {

public:
    /// @brief Default constructor
    GNEMoveElementLaneDouble(GNEAttributeCarrier* element);

    /**@brief Constructor
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNEMoveElementLaneDouble(GNEAttributeCarrier* element, GNELane* lane, const double startPos,
                             const double endPos, const bool friendlyPosition);

    /**@brief Constructor
     * @param[in] lanes Lanes of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNEMoveElementLaneDouble(GNEAttributeCarrier* element, const std::vector<GNELane*>& lanes,
                             const double startPos, const double endPos, const bool friendlyPosition);

    /// @brief Destructor
    ~GNEMoveElementLaneDouble();

    /**@brief get lane movable move operation for elements with
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

protected:
    /// @brief The relative start position this stopping place is located at (-1 means empty)
    double myStartPosition = 0;

    /// @brief The  position this stopping place is located at (-1 means empty)
    double myEndPosition = 0;

    /// @brief Flag for friendly position
    bool myFriendlyPosition = false;

    /// @brief size (only use in AttributeCarrier templates)
    double mySize = 10;

    /// @brief force size (only used in AttributeCarrier templates
    bool myForceSize = false;

    /// @brief reference position
    ReferencePosition myReferencePosition = ReferencePosition::CENTER;

    /// @brief get start position over lane that is applicable to the shape
    double getStartGeometryPositionOverLane() const;

    /// @brief get end position over lane that is applicable to the shape
    double getEndGeometryPositionOverLane() const;

    /// @brief adjust length
    void adjustLaneMovableLength(const double length, GNEUndoList* undoList);

private:
    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEMoveElementLaneDouble(const GNEMoveElementLaneDouble&) = delete;

    /// @brief Invalidated assignment operator
    GNEMoveElementLaneDouble& operator=(const GNEMoveElementLaneDouble& src) = delete;
};
