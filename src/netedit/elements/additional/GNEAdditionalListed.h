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
/// @file    GNEAdditionalListed.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// A abstract class for representation of additional listed elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEContour.h>
#include <utils/gui/images/GUITextureSubSys.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalListed {

public:
    /// @brief default constructor
    GNEAdditionalListed(GNEAdditional* additional);

protected:
    /// @brief update geometry of listed additional
    void updateGeometryListedAdditional();

    /// @brief draw listed additional
    void drawListedAdditional(const GUIVisualizationSettings& s, const RGBColor baseCol, const RGBColor textCol,
                              GUITexture texture, const std::string text) const;

    /// @brief get listed position in view
    Position getListedPositionInView() const;

    /// @brief get draw position index
    int getDrawPositionIndex() const;

private:
    /// @brief pointer to additional
    GNEAdditional* myAdditional;

    /// @name common offsets
    /// @{

    /// @brief line offset (must be divisible by 2)
    const double lineLenght = 1;

    /// @brief line width
    const double lineWidth = 0.05;

    /// @brief shape width
    const double shapeWidth = 6;

    /// @brief shapeHeight
    const double shapeHeight = 0.5;

    /// @brief Y separation
    const double ySeparation = 0.1;

    /// @brief padding between internal and external rectangle
    const double padding = 0.02;

    /// @brief icon padding
    const double iconPadding = 0.05;

    /// @}

    /// @brief draw position index
    int myDrawPositionIndex = 0;

    /// @brief internal rectangle
    GUIGeometry myInternalRectangle;

    /// @brief vector with line geometry
    GUIGeometry myLineGeometry;

    /// @brief sign position
    Position myIconPosition;

    /// @brief icon size
    double myIconSize = 0;

    /// @brief text position
    Position myTextPosition;

    /// @brief adjust listed additional text
    std::string adjustListedAdditionalText(const std::string& text) const;

    /// @brief Invalidated copy constructor.
    GNEAdditionalListed(const GNEAdditionalListed&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAdditionalListed& operator=(const GNEAdditionalListed&) = delete;
};
