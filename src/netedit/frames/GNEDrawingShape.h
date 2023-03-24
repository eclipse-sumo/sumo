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
/// @file    GNEDrawingShape.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for draw shapes
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDrawingShape : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEDrawingShape)

public:
    /// @brief constructor
    GNEDrawingShape(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEDrawingShape();

    /// @brief show Drawing mode
    void showDrawingShape();

    /// @brief hide Drawing mode
    void hideDrawingShape();

    /// @brief start drawing
    void startDrawing();

    /// @brief stop drawing and check if shape can be created
    void stopDrawing();

    /// @brief abort drawing
    void abortDrawing();

    /// @brief add new point to temporal shape
    void addNewPoint(const Position& P);

    /// @brief remove last added point
    void removeLastPoint();

    /// @brief get Temporal shape
    const PositionVector& getTemporalShape() const;

    /// @brief return true if currently a shape is drawed
    bool isDrawing() const;

    /// @brief enable or disable delete last created point
    void setDeleteLastCreatedPoint(bool value);

    /// @brief get flag delete last created point
    bool getDeleteLastCreatedPoint();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user press start drawing button
    long onCmdStartDrawing(FXObject*, FXSelector, void*);

    /// @brief Called when the user press stop drawing button
    long onCmdStopDrawing(FXObject*, FXSelector, void*);

    /// @brief Called when the user press abort drawing button
    long onCmdAbortDrawing(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(GNEDrawingShape)

private:
    /// @brief pointer to frame parent
    GNEFrame* myFrameParent;

    /// @brief flag to enable/disable delete point mode
    bool myDeleteLastCreatedPoint;

    /// @brief current drawed shape
    PositionVector myTemporalShape;

    /// @brief button for start drawing
    FXButton* myStartDrawingButton;

    /// @brief button for stop drawing
    FXButton* myStopDrawingButton;

    /// @brief button for abort drawing
    FXButton* myAbortDrawingButton;

    /// @brief Label with information
    FXLabel* myInformationLabel;
};
