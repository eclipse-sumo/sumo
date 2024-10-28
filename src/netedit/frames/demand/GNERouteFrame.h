/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEPathLegendModule.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GNERoute;

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

    class RouteModeSelector : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteFrame::RouteModeSelector)

    public:
        /// @brief constructor
        RouteModeSelector(GNERouteFrame* routeFrameParent);

        /// @brief destructor
        ~RouteModeSelector();

        /// @brief get current route mode
        const RouteMode& getCurrentRouteMode() const;

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
        MFXComboBoxIcon* myRouteModeMatchBox = nullptr;

        /// @brief comboBox with the list of VClass
        MFXComboBoxIcon* myVClassMatchBox = nullptr;

        /// @brief current selected route mode
        RouteMode myCurrentRouteMode = RouteMode::NONCONSECUTIVE_EDGES;

        /// @brief route template
        GNERoute* myRouteTemplate = nullptr;

        /// @brief flag to check if VClass is Valid
        bool myValidVClass = true;

        /// @brief list of Route modes that will be shown in Match Box
        std::vector<std::pair<RouteMode, std::string> > myRouteModesStrings;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNERouteFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNERouteFrame();

    /// @brief show delete frame
    void show();

    /// @brief hide delete frame
    void hide();

    /**@brief add route edge
    * @param edge edge to be added
    * @param mouseButtonKeyPressed key pressed during click
    * @return true if element was successfully added
    */
    bool addEdgeRoute(GNEEdge* clickedEdge, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed);

    /// @brief get path creator module
    GNEPathCreator* getPathCreator() const;

protected:
    /// @brief create path
    bool createPath(const bool useLastRoute);

private:
    /// @brief route handler
    GNERouteHandler myRouteHandler;

    /// @brief route base object
    CommonXMLStructure::SumoBaseObject* myRouteBaseObject;

    /// @brief route mode selector
    RouteModeSelector* myRouteModeSelector;

    /// @brief internal route attributes
    GNEAttributesCreator* myRouteAttributes;

    /// @brief path creator modul
    GNEPathCreator* myPathCreator;

    /// @brief path legend modul
    GNEPathLegendModule* myPathLegend;
};
