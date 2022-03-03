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
/// @file    GNEWireFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
// The Widget for editing wires
/****************************************************************************/
#pragma once
#include <config.h>
#include <netedit/frames/GNEFrame.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEConnection;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEWireFrame
 * The Widget for editing connection foes
 */
class GNEWireFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEWireFrame)

public:
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEWireFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEWireFrame();

    /**@brief handle wires and set the relative colouring
     * @param objectsUnderCursor collection of objects under cursor after click over view
     */
    void handleWireClick(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief show wire frame
    void show();

    /// @brief hide wire frame
    void hide();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user presses the OK-Button saves any wire modifications
    long onCmdOK(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Cancel-button discards any wire modifications
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @}

protected:
    FOX_CONSTRUCTOR(GNEWireFrame)
private:
};
