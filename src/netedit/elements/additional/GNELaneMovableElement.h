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
/// @file    GNELaneMovableElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class to pack all functions related to move elements over lane(s)
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNELaneMovableElement {

public:
    /// @brief Default constructor
    GNELaneMovableElement();

    /**@brief Constructor
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNELaneMovableElement(GNEAttributeCarrier* element, GNELane* lane, const double startPos,
                          const double endPos, const bool friendlyPosition);

    /**@brief Constructor
     * @param[in] lanes Lanes of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNELaneMovableElement(GNEAttributeCarrier* element, const std::vector<GNELane*>& lanes,
                          const double startPos, const double endPos, const bool friendlyPosition);

    /// @brief Destructor
    ~GNELaneMovableElement();

    /**@brief get lane movable move operation for elements with
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getStartEndMoveOperation();

protected:
    /// @brief pointer to element
    GNEAttributeCarrier* myElement = nullptr;

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

    /// @brief set move shape
    void setLaneMovableMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitLaneMovableMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief adjust length
    void adjustLaneMovableLength(const double length, GNEUndoList* undoList);

private:
    /// @brief Invalidated copy constructor.
    GNELaneMovableElement(const GNELaneMovableElement&) = delete;

    /// @brief Invalidated assignment operator
    GNELaneMovableElement& operator=(const GNELaneMovableElement& src) = delete;
};
