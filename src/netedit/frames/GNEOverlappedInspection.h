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
/// @file    GNEOverlappedInspection.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for overlapped elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNEViewNetHelper.h>
#include <netedit/frames/common/GNEGroupBoxModule.h>
#include <utils/geom/Position.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEOverlappedInspection : public GNEGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEOverlappedInspection)

public:
    /// @brief constructor (used for filter objects under cusor
    GNEOverlappedInspection(GNEFrame* frameParent, const bool onlyJunctions);

    /// @brief destructor
    ~GNEOverlappedInspection();

    /// @brief show overlapped inspection
    void showOverlappedInspection(GNEViewNetHelper::ViewObjectsSelector& viewObjects, const Position& clickedPosition, const bool shiftKeyPressed);

    /// @brief show template editor
    void refreshOverlappedInspection();

    /// @brief clear overlapped inspection
    void clearOverlappedInspection();

    /// @brief hide overlapped inspection
    void hiderOverlappedInspection();

    /// @brief check if overlappedInspection modul is shown
    bool overlappedInspectionShown() const;

    /// @brief get number of overlapped ACs
    int getNumberOfOverlappedACs() const;

    /// @brief get current AC
    GNEAttributeCarrier* getCurrentAC() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief Inspect next Element (from top to bot)
    long onCmdInspectNextElement(FXObject*, FXSelector, void*);

    /// @brief Inspect previous element (from top to bot)
    long onCmdInspectPreviousElement(FXObject*, FXSelector, void*);

    /// @brief show list of overlapped elements
    long onCmdShowList(FXObject*, FXSelector, void*);

    /// @brief called when a list item is selected
    long onCmdListItemSelected(FXObject*, FXSelector, void*);

    /// @brief Called when user press the help button
    long onCmdOverlappingHelp(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEOverlappedInspection();

private:
    /// @brief current frame parent
    GNEFrame* myFrameParent = nullptr;

    /// @brief Previous element button
    FXButton* myPreviousElement = nullptr;

    /// @brief Button for current index
    FXButton* myCurrentIndexButton = nullptr;

    /// @brief Next element button
    FXButton* myNextElement = nullptr;

    /// @brief list of overlapped elements
    FXList* myOverlappedElementList = nullptr;

    /// @brief button for help
    FXButton* myHelpButton = nullptr;

    /// @brief clicked position
    Position myClickedPosition = Position::INVALID;

    /// @brief shift key pressed
    bool myShiftKeyPressed = false;

    /// @brief flag to indicate that this modul is only for junctions
    const bool myOnlyJunctions = false;

    /// @brief objects under cursor
    std::vector<GNEAttributeCarrier*> myOverlappedACs;

    /// @brief current index item
    int myItemIndex = 0;
};
