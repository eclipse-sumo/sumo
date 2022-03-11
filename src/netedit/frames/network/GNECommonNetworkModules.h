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
    // class EdgesSelector
    // ===========================================================================

    class EdgesSelector : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNECommonNetworkModules::EdgesSelector)

    public:
        /// @brief constructor
        EdgesSelector(GNEFrame* frameParent);

        /// @brief destructor
        ~EdgesSelector();

        /// @brief get list of selecte id's in string format
        std::vector<std::string> getEdgeIdsSelected() const;

        /// @brief Show EdgesSelector Module
        void showEdgesSelectorModule(std::string search = "");

        /// @brief hide EdgesSelector Module
        void hideEdgesSelectorModule();

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
        FOX_CONSTRUCTOR(EdgesSelector)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief CheckBox for selected edges
        FXCheckButton* myUseSelectedEdgesCheckButton;

        /// @brief List of EdgesSelector
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for clear selection
        FXButton* myClearEdgesSelection;

        /// @brief button for invert selection
        FXButton* myInvertEdgesSelection;
    };

    // ===========================================================================
    // class LanesSelector
    // ===========================================================================

    class LanesSelector : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNECommonNetworkModules::LanesSelector)

    public:
        /// @brief constructor
        LanesSelector(GNEFrame* frameParent);

        /// @brief destructor
        ~LanesSelector();

        /// @brief get list of selecte lane ids in string format
        std::vector<std::string> getLaneIdsSelected() const;

        /// @brief Show list of LanesSelector Module
        void showLanesSelectorModule(std::string search = "");

        /// @brief hide LanesSelector Module
        void hideLanesSelectorModule();

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
        FOX_CONSTRUCTOR(LanesSelector)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief CheckBox for selected lanes
        FXCheckButton* myUseSelectedLanesCheckButton;

        /// @brief List of LanesSelector
        FXList* myList;

        /// @brief text field for search lane IDs
        FXTextField* myLanesSearch;

        /// @brief button for clear selection
        FXButton* clearLanesSelection;

        /// @brief button for invert selection
        FXButton* invertLanesSelection;
    };

    // ===========================================================================
    // class ConsecutiveLaneSelector
    // ===========================================================================

    class ConsecutiveLaneSelector : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNECommonNetworkModules::ConsecutiveLaneSelector)

    public:
        /// @brief default constructor
        ConsecutiveLaneSelector(GNEFrame* frameParent);

        /// @brief destructor
        ~ConsecutiveLaneSelector();

        /// @brief show ConsecutiveLaneSelector
        void showConsecutiveLaneSelectorModule();

        /// @brief show ConsecutiveLaneSelector
        void hideConsecutiveLaneSelectorModule();

        /// @brief get vector with lanes and clicked positions
        const std::vector<std::pair<GNELane*, double> > &getLanePath() const;

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
        FOX_CONSTRUCTOR(ConsecutiveLaneSelector)

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
        ConsecutiveLaneSelector(ConsecutiveLaneSelector*) = delete;

        /// @brief Invalidated assignment operator.
        ConsecutiveLaneSelector& operator=(ConsecutiveLaneSelector*) = delete;
    };
};
