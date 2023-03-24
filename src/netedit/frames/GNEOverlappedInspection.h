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
/// @file    GNEOverlappedInspection.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for overlapped elements
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

class GNEOverlappedInspection : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEOverlappedInspection)

public:
    /// @brief constructor
    GNEOverlappedInspection(GNEFrame* frameParent);

    /// @brief constructor (used for filter objects under cusor
    GNEOverlappedInspection(GNEFrame* frameParent, const SumoXMLTag filteredTag);

    /// @brief destructor
    ~GNEOverlappedInspection();

    /// @brief show template editor
    void showOverlappedInspection(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition);

    /// @brief hide template editor
    void hideOverlappedInspection();

    /// @brief check if overlappedInspection modul is shown
    bool overlappedInspectionShown() const;

    /// @brief get number of overlapped ACSs
    int getNumberOfOverlappedACs() const;

    /// @brief check if given position is near to saved position
    bool checkSavedPosition(const Position& clickedPosition) const;

    /// @brief try to go to next element if clicked position is near to saved position
    bool nextElement(const Position& clickedPosition);

    /// @brief try to go to previous element if clicked position is near to saved position
    bool previousElement(const Position& clickedPosition);

    /// @name FOX-callbacks
    /// @{

    /// @brief Inspect next Element (from top to bot)
    long onCmdNextElement(FXObject*, FXSelector, void*);

    /// @brief Inspect previous element (from top to bot)
    long onCmdPreviousElement(FXObject*, FXSelector, void*);

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

    /// @brief build Fox Toolkit elemements
    void buildFXElements();

private:
    /// @brief current frame parent
    GNEFrame* myFrameParent;

    /// @brief Previous element button
    FXButton* myPreviousElement;

    /// @brief Button for current index
    FXButton* myCurrentIndexButton;

    /// @brief Next element button
    FXButton* myNextElement;

    /// @brief list of overlapped elements
    FXList* myOverlappedElementList;

    /// @brief button for help
    FXButton* myHelpButton;

    /// @brief filtered tag
    const SumoXMLTag myFilteredTag;

    /// @brief objects under cursor
    std::vector<GNEAttributeCarrier*> myOverlappedACs;

    /// @brief current index item
    size_t myItemIndex;

    /// @brief saved clicked position
    Position mySavedClickedPosition;
};
