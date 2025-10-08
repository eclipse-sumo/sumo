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
/// @file    GNEMoveElementView.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over view
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEContour.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementView : public GNEMoveElement {

public:
    /// @brief attributes format
    enum class AttributesFormat {
        POSITION,   /// @brief position format
        CARTESIAN,  /// @brief cartesian format (x, y, z)
        GEO         /// @brief geo format (lon, lat, z)
    };

    /// @brief constructor for element with fixed size
    GNEMoveElementView(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
                       const Position& position);

    /// @brief constructor with dynamic position
    GNEMoveElementView(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
                       const Position& position, const double width, const double height,
                       const double length);

    //// @brief empty destructor
    ~GNEMoveElementView();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

//protected:
    /// @brief width
    double myWidth = 0;

    /// @brief height
    double myHeight = 0;

    /// @brief length
    double myLength = 0;

    /// @brief shape width
    PositionVector myShapeWidth;

    /// @brief shape height
    PositionVector myShapeHeight;

    /// @brief shape length
    PositionVector myShapeLength;

    /// @brief variable used for moving geometry point contour up
    GNEContour myMovingContourUp;

    /// @brief variable used for moving geometry point contour down
    GNEContour myMovingContourDown;

    /// @brief variable used for moving geometry point contour left
    GNEContour myMovingContourLeft;

    /// @brief variable used for moving geometry point contour right
    GNEContour myMovingContourRight;

    /// @brief write move attributes
    void writeMoveAttributes(OutputDevice& device) const;

private:
    /// @brief position over view
    Position myPosOverView;

    /// @brief pos attributes format
    AttributesFormat myAttributesFormat = AttributesFormat::POSITION;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEMoveElementView(const GNEMoveElementView&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElementView& operator=(const GNEMoveElementView&) = delete;
};
