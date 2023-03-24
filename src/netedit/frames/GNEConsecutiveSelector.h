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
/// @file    GNEConsecutiveSelector.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Consecutive lane selector module
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEConsecutiveSelector : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEConsecutiveSelector)

public:
    /// @brief default constructor
    GNEConsecutiveSelector(GNEFrame* frameParent, const bool allowOneLane);

    /// @brief destructor
    ~GNEConsecutiveSelector();

    /// @brief show GNEConsecutiveSelector
    void showConsecutiveLaneSelectorModule();

    /// @brief show GNEConsecutiveSelector
    void hideConsecutiveLaneSelectorModule();

    /// @brief get vector with lanes and clicked positions
    const std::vector<std::pair<GNELane*, double> >& getLanePath() const;

    /// @brief get lane IDs
    const std::vector<std::string> getLaneIDPath() const;

    /// @brief add lane
    bool addLane(GNELane* lane);

    /// @brief draw candidate lanes with special color (Only for candidates, special and conflicted)
    bool drawCandidateLanesWithSpecialColor() const;

    /// @brief update lane colors
    void updateLaneColors();

    /// @brief draw temporal consecutive lane path
    void drawTemporalConsecutiveLanePath(const GUIVisualizationSettings& s) const;

    /// @brief abort path creation
    void abortPathCreation();

    /// @brief remove path element
    void removeLastElement();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user click over button "Finish route creation"
    long onCmdCreatePath(FXObject*, FXSelector, void*);

    /// @brief Called when the user click over button "Abort route creation"
    long onCmdAbortPathCreation(FXObject*, FXSelector, void*);

    /// @brief Called when the user click over button "Remove las inserted lane"
    long onCmdRemoveLastElement(FXObject*, FXSelector, void*);

    /// @brief Called when the user click over check button "show candidate lanes"
    long onCmdShowCandidateLanes(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX need this
    GNEConsecutiveSelector();

    /// @brief update InfoRouteLabel
    void updateInfoRouteLabel();

    /// @brief clear lanes (and restore colors)
    void clearPath();

private:
    /// @brief pointer to frame parent
    GNEFrame* myFrameParent;

    /// @brief vector with lanes and clicked positions
    std::vector<std::pair<GNELane*, double> > myLanePath;

    /// @brief label with path info
    FXLabel* myInfoPathLabel = nullptr;

    /// @brief button for finish route creation
    FXButton* myFinishCreationButton = nullptr;

    /// @brief button for abort route creation
    FXButton* myAbortCreationButton = nullptr;

    /// @brief button for removing last inserted element
    FXButton* myRemoveLastInsertedElement = nullptr;

    /// @brief CheckBox for show candidate lanes
    FXCheckButton* myShowCandidateLanes = nullptr;

    /// @brief allow one lane
    const bool myAllowOneLane;

private:
    /// @brief Invalidated copy constructor.
    GNEConsecutiveSelector(GNEConsecutiveSelector*) = delete;

    /// @brief Invalidated assignment operator.
    GNEConsecutiveSelector& operator=(GNEConsecutiveSelector*) = delete;
};
