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
/// @file    GNEAdditionalSquared.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// A abstract class for representation of additional squared elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/images/GUITextureSubSys.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAdditional;
class GNEMoveElementView;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalSquared {

public:
    /// @brief default constructor
    GNEAdditionalSquared(GNEAdditional* additional);

    /// @brief parameter constructor
    GNEAdditionalSquared(GNEAdditional* additional, const Position pos);

    /// @brief destructor
    ~GNEAdditionalSquared();

protected:
    /// @brief position over view
    Position myPosOverView;

    /// @brief move element over view
    GNEMoveElementView* myMoveElementView = nullptr;

    /// @brief updated squared geometry
    void updatedSquaredGeometry();

    /// @brief updated squared centering boundary
    void updatedSquaredCenteringBoundary(const bool updateGrid);

    /// @brief draw squared additional
    void drawSquaredAdditional(const GUIVisualizationSettings& s, const double size,
                               GUITexture texture, GUITexture selectedTexture) const;

private:
    /// @brief pointer to additional
    GNEAdditional* myAdditional;

    /// @brief Invalidated copy constructor.
    GNEAdditionalSquared(const GNEAdditionalSquared&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAdditionalSquared& operator=(const GNEAdditionalSquared&) = delete;
};
