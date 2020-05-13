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

        /// @brief check if current mode is Valid
        bool isValidMode() const;

        /// @brief check if current VClass is Valid
        bool isValidVehicleClass() const;

        /// @brief called after setting a new route or vclass, for showing moduls
        void areParametersValid();

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
        // @brief creation mode
        enum class Mode {
            CONSECUTIVE,
            NOCONSECUTIVE
        };

        /// @brief struct for path
        struct Path {

            /// @brief constructor for single edge
            Path(const SUMOVehicleClass vClass, GNEEdge* edge);

            /// @brief constructor for multiple edges
            Path(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* edgeFrom, GNEEdge* edgeTo);

            ///
            std::vector<GNEEdge*> subPath;

            bool conflictVClass;

            bool conflictDisconnected;

        private:
            /// @brief default constructor
            Path();
        };

        /// @brief default constructor
        PathCreator(GNERouteFrame* routeFrameParent, Mode mode);

        /// @brief destructor
        ~PathCreator();

        /// @brief show PathCreator
        void showPathCreatorModul();

        /// @brief show PathCreator
        void hidePathCreatorModul();

        /// @brief get current selected edgesm
        std::vector<GNEEdge*> getSelectedEdges() const;

        /// @brief change route mode
        void setPathCreatorMode(Mode mode);

        /// @brief set edge from (and change color)
        bool addEdge(GNEEdge* edge, const bool shiftKeyPressed, const bool controlKeyPressed);

        /// @brief clear edges (and restore colors)
        void clearPath();

        /// @brief get path route
        const std::vector<Path>& getPath() const;

        /// @brief draw candidate edges with special color (Only for candidates, special and conflicted)
        bool drawCandidateEdgesWithSpecialColor() const;

        /// @brief update edge colors
        void updateEdgeColors();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user click over button "Finish route creation"
        long onCmdCreatePath(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Abort route creation"
        long onCmdAbortPathCreation(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over button "Remove las inserted edge"
        long onCmdRemoveLastElement(FXObject*, FXSelector, void*);

        /// @brief Called when the user click over check button "show candidate edges"
        long onCmdShowCandidateEdges(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(PathCreator)

        /// @brief update InfoRouteLabel
        void updateInfoRouteLabel();

        /// @brief recalculate path
        void recalculatePath();

    private:
        /// @brief set special candidates (This function will be called recursively)
        void setSpecialCandidates(GNEEdge* originEdge);

        /// @brief set edgereachability (This function will be called recursively)
        void setPossibleCandidates(GNEEdge* originEdge, const SUMOVehicleClass vClass);

        /// @brief pointer to Vehicle Frame Parent
        GNERouteFrame* myRouteFrameParent;

        /// @brief current mode
        Mode myMode;

        /// @brief label with route info
        FXLabel* myInfoRouteLabel;

        /// @brief current selected edges
        std::vector<GNEEdge*> mySelectedElements;

        /// @brief vector with current path
        std::vector<Path> myPath;

        /// @brief button for finish route creation
        FXButton* myFinishCreationButton;

        /// @brief button for abort route creation
        FXButton* myAbortCreationButton;

        /// @brief button for removing last inserted edge
        FXButton* myRemoveLastInsertedEdge;

        /// @brief CheckBox for show candidate edges
        FXCheckButton* myShowCandidateEdges;
    };

    // ===========================================================================
    // class Information
    // ===========================================================================

    class Information : protected FXGroupBox {

    public:
        /// @brief constructor
        Information(GNERouteFrame* routeFrameParent);

        /// @brief destructor
        ~Information();

        /// @brief show information modul
        void showInformationModul();

        /// @brief hide information modul
        void hideInformationModul();

        /// @brief show control and shift label
        void showControlAndShiftLabels(const bool value);

    private:
        /// @brief label for shift information
        FXLabel* myShiftLabel;

        /// @brief label for control information
        FXLabel* myControlLabel;
    };

    // ===========================================================================
    // class Legend
    // ===========================================================================

    class Legend : protected FXGroupBox {

    public:
        /// @brief constructor
        Legend(GNERouteFrame* routeFrameParent);

        /// @brief destructor
        ~Legend();

        /// @brief show Legend modul
        void showLegendModul();

        /// @brief hide Legend modul
        void hideLegendModul();

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
    void handleEdgeClick(GNEEdge* clickedEdge, const bool shiftKeyPressed, const bool controlKeyPressed);

    /// @brief function called when user press ENTER key
    void hotkeyEnter();

    /// @brief function called when user press BACKSPACE key
    void hotkeyBackSpace();

    /// @brief function called when user press ESC key
    void hotkeyEsc();

    /// @brief draw temporal route
    void drawTemporalRoute(const GUIVisualizationSettings* s) const;

    /// @brief get path creator modul
    PathCreator* getPathCreator() const;

private:
    /// @brief route mode selector
    RouteModeSelector* myRouteModeSelector;

    /// @brief internal route attributes
    GNEFrameAttributesModuls::AttributesCreator* myRouteAttributes;

    /// @brief path creator modul
    PathCreator* myPathCreator;

    /// @brief information modul
    Information* myInformation;

    /// @brief legend modul
    Legend* myLegend;
};
