/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNERouteFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2016
///
// The Widget for create route elements
/****************************************************************************/
#pragma once

#include <netedit/frames/GNEFrame.h>

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
    enum class RouteMode {
        INVALID,                // invalid RouteMode
        NONCONSECUTIVE_EDGES,   // Create route clicking over non consecutive edges
        CONSECUTIVE_EDGES       // Create route clicking over consecutive edges
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
        const RouteMode& getCurrentRouteMode() const;

        /// @brief get current selected VClass
        SUMOVehicleClass getCurrentVehicleClass() const;

        /// @brief check if current VClass is Valid
        bool isValidVehicleClass() const;

        /// @brief set current route mode type manually
        void setCurrentRouteMode(RouteMode routemode);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another route mode in ComboBox
        long onCmdSelectRouteMode(FXObject*, FXSelector, void*);

        /// @brief Called when the user select another VClass
        long onCmdSelectVClass(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(RouteModeSelector)

    private:
        /// @brief pointer to Frame Parent
        GNERouteFrame* myRouteFrameParent;

        /// @brief comboBox with the list of route modes
        FXComboBox* myRouteModeMatchBox;

        /// @brief comboBox with the list of VClass
        FXComboBox* myVClassMatchBox;

        /// @brief current selected route mode
        RouteMode myCurrentRouteMode;

        /// @brief current selected VClass
        SUMOVehicleClass myCurrentVehicleClass;

        /// @brief flag to check if VClass is Valid
        bool myValidVClass;

        /// @brief list of Route modes that will be shown in Match Box
        std::vector<std::pair<RouteMode, std::string> > myRouteModesStrings;
    };

    // ===========================================================================
    // class PathCreator
    // ===========================================================================

    class PathCreator : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteFrame::PathCreator)

    public:
        enum class Mode {
            CONSECUTIVE,
            NOCONSECUTIVE
        };

        /// @brief default constructor
        PathCreator(GNERouteFrame* routeFrameParent, Mode mode);

        /// @brief destructor
        ~PathCreator();

        /// @brief show PathCreator
        void showPathCreatorModul();

        /// @brief show PathCreator
        void hidePathCreatorModul();

        /// @brief check if from and to edges create a valid route
        bool isPathValid(SUMOVehicleClass vehicleClass) const;

        /// @brief get current selected edgesm
        std::vector<GNEEdge*> getSelectedEdges() const;

        /// @brief change route mode
        void setPathCreatorMode(Mode mode);

        /// @brief set edge from (and change color)
        bool addEdge(GNEEdge* edge);

        /// @brief clear edges (and restore colors)
        void clearPath();

        /// @brief get temporal route
        const std::vector<GNEEdge*>& getPathRoute() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user click over button "Finish route creation"
        long onCmdCreatePath(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Abort route creation"
        long onCmdAbortPathCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Remove las inserted edge"
        long onCmdRemoveLastElement(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(PathCreator)

        /// @brief update InfoRouteLabel
        void updateInfoRouteLabel();

        /// @brief update reachability
        void updateReachability();

        /// @brief current mode
        Mode myMode;

    private:
        /// @brief set edgereachability (This function will be called recursively)
        void setEdgesReachability(GNEEdge* edge, SUMOVehicleClass vClass);

        /// @brief pointer to Vehicle Frame Parent
        GNERouteFrame* myRouteFrameParent;

        /// @brief label with route info
        FXLabel* myInfoRouteLabel;

        /// @brief current selected edges
        std::vector<GNEEdge*> mySelectedElements;

        /// @brief vector with temporal route edges
        std::vector<GNEEdge*> myTemporalPath;

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
    void hotkeyEnter();

    /// @brief function called when user press BACKSPACE key
    void hotkeyBackSpace();

    /// @brief function called when user press ESC key
    void hotkeyEsc();

    /// @brief draw temporal route
    void drawTemporalRoute() const;

private:
    /// @brief route mode selector
    RouteModeSelector* myRouteModeSelector;

    /// @brief internal route attributes
    GNEFrameAttributesModuls::AttributesCreator* myRouteAttributes;

    /// @brief path creator modul
    PathCreator* myPathCreator;
};
