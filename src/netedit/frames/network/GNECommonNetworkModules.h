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
    // class NetworkElementSelector
    // ===========================================================================

    class NetworkElementsSelector : public FXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNECommonNetworkModules::NetworkElementsSelector)

    public:
        /// @brief network element selector type
        enum class NetworkElementType {
            EDGE,
            LANE,
        };

        /// @brief constructor
        NetworkElementsSelector(GNEFrame* frameParent, const NetworkElementType networkElementType);

        /// @brief destructor
        ~NetworkElementsSelector();

        /// @brief get selected IDs
        std::vector<std::string> getSelectedIDs() const;

        /// @brief check if the given networkElement is being selected
        bool isNetworkElementSelected(const GNENetworkElement* networkElement) const;

        /// @brief show NetworkElementsSelector Module
        void showNetworkElementsSelector();

        /// @brief hide NetworkElementsSelector Module
        void hideNetworkElementsSelector();

        /// @brief return true if modul is shown
        bool isShown() const;

        /// @brief toogle selected networkElement
        bool toogleSelectedElement(const GNENetworkElement *networkElement);

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedNetworkElements
        long onCmdUseSelectedNetworkElements(FXObject*, FXSelector, void*);

        /// @brief called when user select a networkElement of the list
        long onCmdSelectNetworkElement(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        NetworkElementsSelector();

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief CheckBox for selected networkElements
        FXCheckButton* myUseSelectedNetworkElementsCheckButton = nullptr;

        /// @brief List of NetworkElementsSelector
        FXList* myList = nullptr;

        /// @brief button for clear selection
        FXButton* myClearNetworkElementsSelection = nullptr;

        /// @brrief network element type
        const NetworkElementType myNetworkElementType;
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
