/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2016
/// @version $Id$
///
// The Widget for create route elements
/****************************************************************************/
#ifndef GNERouteFrame_h
#define GNERouteFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERouteFrame
 * The Widget for create route elements
 */
class GNERouteFrame : public GNEFrame {

public:

    /// @brief route creation modes
    enum RouteMode {
        ROUTEMODE_INVALID,              // invalid RouteMode
        ROUTEMODE_CONSECUTIVE_EDGES,    // Create route clicking over consecutive edges
        ROUTEMODE_NONCONSECUTIVE_EDGES  // Create route clicking over non consecutive edges
    };

    // ===========================================================================
    // class RouteModeSelector
    // ===========================================================================

    class RouteModeSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteFrame::RouteModeSelector)

    public:
        /// @brief constructor
        RouteModeSelector(GNERouteFrame* routeFrameParent);

        /// @brief destructor
        ~RouteModeSelector();

        /// @brief get current route mode
        const RouteMode& getCurrenRouteMode() const;

        /// @brief set current route mode type manually
        void setCurrentRouteMode(RouteMode routemode);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another route mode in ComboBox
        long onCmdSelectRouteMode(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        RouteModeSelector() {}

    private:
        /// @brief pointer to Frame Parent
        GNERouteFrame* myRouteFrameParent;

        /// @brief comboBox with the list of route modes
        FXComboBox* myTypeMatchBox;

        /// @brief current selected route mode
        RouteMode myCurrentRouteMode;

        /// @brief list of Route modes that will be shown in Match Box
        std::vector<std::pair<RouteMode, std::string> > myRouteModesStrings;
    };

    // ===========================================================================
    // class ConsecutiveEdges
    // ===========================================================================

    class ConsecutiveEdges : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteFrame::ConsecutiveEdges)

    public:
        /// @brief constructor
        ConsecutiveEdges(GNERouteFrame* routeFrameParent);

        /// @brief destructor
        ~ConsecutiveEdges();

        /// @brief show ConsecutiveEdges modul
        void showConsecutiveEdgesModul();

        /// @brief hide ConsecutiveEdges modul
        void hideConsecutiveEdgesModul();

        /// @brief add edge to current route (note: edge must be included in set of candidate edges
        bool addEdgeIntoRoute(GNEEdge* edge);

        /// @brief create route with the current edges
        void createRoute();

        /// @brief abort creation of current route
        void abortRouteCreation();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press create route button
        long onCmdCreateRoute(FXObject*, FXSelector, void*);

        /// @brief Called when the user press create route button
        long onCmdAbortCreateRoute(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ConsecutiveEdges() {}

        /// @brief update InfoRouteLabel
        void updateInfoRouteLabel();

    private:
        /// @brief pointer to Frame Parent
        GNERouteFrame* myRouteFrameParent;

        /// @brief label with route info
        FXLabel* myInfoRouteLabel;

        /// @brief FXButton for create routes
        FXButton* myCreateRouteButton;

        /// @bief FXButton for abort creating route
        FXButton* myAbortCreationButton;

        /// @brief vector with current route edges
        std::vector<GNEEdge*> myRouteEdges;
    };

    // ===========================================================================
    // class NonConsecutiveEdges
    // ===========================================================================

    class NonConsecutiveEdges : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteFrame::NonConsecutiveEdges)

    public:
        /// @brief default constructor
        NonConsecutiveEdges(GNERouteFrame* routeFrameParent);

        /// @brief destructor
        ~NonConsecutiveEdges();

        /// @brief show NonConsecutiveEdges
        void showNonConsecutiveEdgesModul();

        /// @brief show NonConsecutiveEdges
        void hideNonConsecutiveEdgesModul();

        /// @brief check if from and to edges create a valid route
        bool isValid(SUMOVehicleClass vehicleClass) const;

        /// @brief get current selected edgesm
        std::vector<GNEEdge*> getSelectedEdges() const;

        /// @brief set edge from (and change color)
        bool addEdge(GNEEdge* edge);

        /// @brief clear edges (and restore colors)
        void clearEdges();

        /// @brief draw temporal route
        void drawTemporalRoute() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user click over button "Abort route creation"
        long onCmdAbortRouteCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Finish route creation"
        long onCmdFinishRouteCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Remove las inserted edge"
        long onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        NonConsecutiveEdges() {}

    private:
        /// @brief pointer to Vehicle Frame Parent
        GNERouteFrame* myRouteFrameParent;

        /// @brief current selected edges
        std::vector<GNEEdge*> mySelectedEdges;

        /// @brief vector with temporal route edges
        std::vector<const NBEdge*> myTemporalRoute;

        /// @brief button for finish route creation
        FXButton* myFinishCreationButton;

        /// @brief button for abort route creation
        FXButton* myAbortCreationButton;

        /// @brief button for removing last inserted edge
        FXButton* myRemoveLastInsertedEdge;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNERouteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNERouteFrame();

    /// @brief show delete frame
    void show();

    /// @brief hide delete frame
    void hide();

    /// @brief handle edge click
    void handleEdgeClick(GNEEdge* clickedEdge);

    /// @brief function called when user press ENTER key
    void hotKeyEnter();

    /// @brief function called when user press ESC key
    void hotKeyEsc();

    /// @brief return non consecutive edges modul
    NonConsecutiveEdges* getNonConsecutiveEdges() const;

private:
    /// @brief route mode selector
    RouteModeSelector* myRouteModeSelector;

    /// @brief Create routes using consecutive edges modul
    ConsecutiveEdges* myConsecutiveEdges;

    /// @brief Create routes using non consecutive edges modul
    NonConsecutiveEdges* myNonConsecutiveEdges;
};


#endif

/****************************************************************************/

