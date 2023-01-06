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
/// @file    GNEGeometryPointDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2021
///
// A dialog for set Geometry Points
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <vector>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEGeometryPointDialog
 * @brief Dialog to edit geometry points
 *
 * This is a modal dialog which blocks in its constructor so it is save to delete it
 *  directly after construction
 */
class GNEGeometryPointDialog : protected FXTopWindow {
    /// @brief FOX-declaration abstract
    FXDECLARE_ABSTRACT(GNEGeometryPointDialog)

public:
    /// @brief constructor
    GNEGeometryPointDialog(GNEViewNet* viewNet, Position* pos);

    /// @brief destructor
    ~GNEGeometryPointDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after change position in TextFields
    long onCmdChangeGeometryPoint(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief event after press accept button
    long onCmdAccept(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief event after press cancel button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    GNEGeometryPointDialog();

    /// @brief viewNet
    GNEViewNet* myViewNet;

    /// @brief text field for X, Y
    FXTextField* myTextFieldXY = nullptr;

    /// @brief text field for lon, Lat
    FXTextField* myTextFieldLonLat = nullptr;

    /// @brief position to be edited
    Position* myPos = nullptr;

    /// @brief original position (used for reset)
    const Position myOriginalPos;

    /// @brief flag for geo
    const bool myGeo;

private:
    /// @brief accept button
    FXButton* myAcceptButton = nullptr;

    /// @brief cancel button
    FXButton* myCancelButton = nullptr;

    /// @brief cancel button
    FXButton* myResetButton = nullptr;

    /// @brief Invalidated copy constructor
    GNEGeometryPointDialog(const GNEGeometryPointDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEGeometryPointDialog& operator=(const GNEGeometryPointDialog&) = delete;
};

