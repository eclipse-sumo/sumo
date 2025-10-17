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
/// @file    GNEMoveElementViewResizable.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over view and resized
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEMoveElementView.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementViewResizable : public GNEMoveElementView {

public:
    /// @brief attributes format
    enum class ResizingFormat {
        WIDTH_HEIGHT,   /// @brief width and height
        WIDTH_LENGTH,   /// @brief width and length
    };

    /// @brief constructor
    GNEMoveElementViewResizable(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
                                ResizingFormat resizingFormat, SumoXMLAttr posAttr,
                                Position& position, double& width, double& height);

    //// @brief empty destructor
    ~GNEMoveElementViewResizable();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    GNEMoveOperation* getMoveOperation();

// temporal
//protected:
    /// @brief shape width
    PositionVector myShapeWidth;

    /// @brief shape height
    PositionVector myShapeHeight;

    /// @brief variable used for moving geometry point contour up
    GNEContour myMovingContourUp;

    /// @brief variable used for moving geometry point contour down
    GNEContour myMovingContourDown;

    /// @brief variable used for moving geometry point contour left
    GNEContour myMovingContourLeft;

    /// @brief variable used for moving geometry point contour right
    GNEContour myMovingContourRight;

private:
    /// @brief width
    double& myWidth;

    /// @brief height
    double& myHeight;

    /// @brief check if width can be edited
    const double myEditWidth = false;

    /// @brief check if height can be edited
    const double myEditHeight = false;

    /// @brief check if length can be edited
    const double myEditLength = false;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief Invalidated copy constructor.
    GNEMoveElementViewResizable(const GNEMoveElementViewResizable&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElementViewResizable& operator=(const GNEMoveElementViewResizable&) = delete;
};
