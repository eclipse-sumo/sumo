/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNECommonNetworkModules.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Common network modules
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <utils/foxtools/FXGroupBoxModule.h>


// ===========================================================================
// class definitions
// ===========================================================================


class GNECommonNetworkModules {

public:

    // ===========================================================================
    // class SelectorChildEdges
    // ===========================================================================

    class SelectorChildEdges : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNECommonNetworkModules::SelectorChildEdges)

    public:
        /// @brief constructor
        SelectorChildEdges(GNEFrame* frameParent);

        /// @brief destructor
        ~SelectorChildEdges();

        /// @brief get list of selecte id's in string format
        std::vector<std::string> getEdgeIdsSelected() const;

        /// @brief Show SelectorChildEdges Module
        void showSelectorChildEdgesModule(std::string search = "");

        /// @brief hide SelectorChildEdges Module
        void hideSelectorChildEdgesModule();

        /// @brief Update use selectedEdges
        void updateUseSelectedEdges();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedEdges
        long onCmdUseSelectedEdges(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdTypeInSearchBox(FXObject*, FXSelector, void*);

        /// @brief called when user select a edge of the list
        long onCmdSelectEdge(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(SelectorChildEdges)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief CheckBox for selected edges
        FXCheckButton* myUseSelectedEdgesCheckButton;

        /// @brief List of SelectorChildEdges
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;
    };

    // ===========================================================================
    // class SelectorChildLanes
    // ===========================================================================

    class SelectorChildLanes : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNECommonNetworkModules::SelectorChildLanes)

    public:
        /// @brief constructor
        SelectorChildLanes(GNEFrame* frameParent);

        /// @brief destructor
        ~SelectorChildLanes();

        /// @brief get list of selecte lane ids in string format
        std::vector<std::string> getLaneIdsSelected() const;

        /// @brief Show list of SelectorChildLanes Module
        void showSelectorChildLanesModule(std::string search = "");

        /// @brief hide SelectorChildLanes Module
        void hideSelectorChildLanesModule();

        // @brief Update use selectedLanes
        void updateUseSelectedLanes();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedLanes
        long onCmdUseSelectedLanes(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdTypeInSearchBox(FXObject*, FXSelector, void*);

        /// @brief called when user select a lane of the list
        long onCmdSelectLane(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(SelectorChildLanes)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief CheckBox for selected lanes
        FXCheckButton* myUseSelectedLanesCheckButton;

        /// @brief List of SelectorChildLanes
        FXList* myList;

        /// @brief text field for search lane IDs
        FXTextField* myLanesSearch;

        /// @brief button for clear selection
        FXButton* clearLanesSelection;

        /// @brief button for invert selection
        FXButton* invertLanesSelection;
    };

    // ===========================================================================
    // class E2MultilaneLaneSelector
    // ===========================================================================

    class E2MultilaneLaneSelector : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNECommonNetworkModules::E2MultilaneLaneSelector)

    public:
        /// @brief default constructor
        E2MultilaneLaneSelector(GNEFrame* frameParent);

        /// @brief destructor
        ~E2MultilaneLaneSelector();

        /// @brief show E2MultilaneLaneSelector
        void showE2MultilaneLaneSelectorModule();

        /// @brief show E2MultilaneLaneSelector
        void hideE2MultilaneLaneSelectorModule();

        /// @brief get vector with lanes and clicked positions
        const std::vector<std::pair<GNELane*, double> > &getLanePath() const;

        /// @brief add lane
        bool addLane(GNELane* lane);

        /// @brief draw candidate lanes with special color (Only for candidates, special and conflicted)
        bool drawCandidateLanesWithSpecialColor() const;

        /// @brief update lane colors
        void updateLaneColors();

        /// @brief draw temporal E2Multilane
        void drawTemporalE2Multilane(const GUIVisualizationSettings& s) const;

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
        FOX_CONSTRUCTOR(E2MultilaneLaneSelector)

        /// @brief update InfoRouteLabel
        void updateInfoRouteLabel();

        /// @brief clear lanes (and restore colors)
        void clearPath();

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief vector with lanes and clicked positions
        std::vector<std::pair<GNELane*, double> > myLanePath;

        /// @brief label with route info
        FXLabel* myInfoRouteLabel;

        /// @brief button for finish route creation
        FXButton* myFinishCreationButton;

        /// @brief button for abort route creation
        FXButton* myAbortCreationButton;

        /// @brief button for removing last inserted element
        FXButton* myRemoveLastInsertedElement;

        /// @brief CheckBox for show candidate lanes
        FXCheckButton* myShowCandidateLanes;

    private:
        /// @brief Invalidated copy constructor.
        E2MultilaneLaneSelector(E2MultilaneLaneSelector*) = delete;

        /// @brief Invalidated assignment operator.
        E2MultilaneLaneSelector& operator=(E2MultilaneLaneSelector*) = delete;
    };
};
