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
        ROUTEMODE_INVALID,          // invalid RouteMode
        ROUTEMODE_EDGETOEDGE,       // Create route clicking over consecutive edges
        ROUTEMODE_MAXVELOCITY,      // Give start and end edges, and calculate route with maximum velocity
        ROUTEMODE_MINIMUMLENGHT,    // Give start and end edges, and calculate route with minimum edge lenghts
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
        const RouteMode &getCurrenRouteMode() const;

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
    // class EdgeToEdge
    // ===========================================================================

    class EdgeToEdge : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteFrame::EdgeToEdge)

    public:
        /// @brief constructor
        EdgeToEdge(GNERouteFrame* routeFrameParent);

        /// @brief destructor
        ~EdgeToEdge();

        /// @brief show EdgeToEdge modul
        void showEdgeToEdgeModul();

        /// @brief hide EdgeToEdge modul
        void hideEdgeToEdgeModul();

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
        EdgeToEdge() {}

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

private:
    /// @brief route mode selector
    RouteModeSelector* myRouteModeSelector;

    /// @brief Edge To Edge modul
    EdgeToEdge* myEdgeToEdge;
};


#endif

/****************************************************************************/

