/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEVehicleFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
///
// The Widget for add Vehicles/Flows/Trips/etc. elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/elements/demand/GNERouteHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEVehicleFrame
 */
class GNEVehicleFrame : public GNEFrame {

public:

    // ===========================================================================
    // class HelpCreation
    // ===========================================================================

    class HelpCreation : public FXGroupBoxModule {

    public:
        /// @brief constructor
        HelpCreation(GNEVehicleFrame* vehicleFrameParent);

        /// @brief destructor
        ~HelpCreation();

        /// @brief show HelpCreation
        void showHelpCreation();

        /// @brief hide HelpCreation
        void hideHelpCreation();

        /// @brief update HelpCreation
        void updateHelpCreation();

    private:
        /// @brief pointer to Vehicle Frame Parent
        GNEVehicleFrame* myVehicleFrameParent;

        /// @brief Label with creation information
        FXLabel* myInformationLabel;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEVehicleFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEVehicleFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /**@brief add vehicle element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @param mouseButtonKeyPressed key pressed during click
     * @return true if element was sucesfully added
     */
    bool addVehicle(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed);

    /// @brief get PathCreator modul
    GNEFrameModules::PathCreator* getPathCreator() const;

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

    /// @brief selected vehicle type in DemandElementSelector
    void demandElementSelected();

    /// @brief create path
    void createPath();

private:
    /// @brief route handler
    GNERouteHandler myRouteHandler;

    /// @brief vehicle base object
    CommonXMLStructure::SumoBaseObject* myVehicleBaseObject;

    /// @brief vehicle tag selector (used to select diffent kind of vehicles)
    GNEFrameModules::TagSelector* myVehicleTagSelector;

    /// @brief Vehicle Type selectors
    GNEFrameModules::DemandElementSelector* myVTypeSelector;

    /// @brief internal vehicle attributes
    GNEFrameAttributeModules::AttributesCreator* myVehicleAttributes;

    /// @brief edge path creator (used for trips and flows)
    GNEFrameModules::PathCreator* myPathCreator;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};
