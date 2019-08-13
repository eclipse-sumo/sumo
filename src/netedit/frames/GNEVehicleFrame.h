/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
/// @version $Id$
///
// The Widget for add Vehicles/Flows/Trips/etc. elements
/****************************************************************************/
#ifndef GNEVehicleFrame_h
#define GNEVehicleFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"


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

    class HelpCreation : protected FXGroupBox {

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

    /**@brief add vehicle element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if vehicle was sucesfully added
     */
    bool addVehicle(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief get EdgePathCreator modul
    GNEFrameModuls::EdgePathCreator* getEdgePathCreator() const;

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

    /// @brief selected vehicle type in DemandElementSelector
    void demandElementSelected();

    /// @brief finish edge path creation
    void edgePathCreated();

private:
    /// @brief vehicle tag selector (used to select diffent kind of vehicles)
    GNEFrameModuls::TagSelector* myVehicleTagSelector;

    /// @brief Vehicle Type selectors
    GNEFrameModuls::DemandElementSelector* myVTypeSelector;

    /// @brief internal vehicle attributes
    GNEFrameAttributesModuls::AttributesCreator* myVehicleAttributes;

    /// @brief edge path creator (used for trips and flows)
    GNEFrameModuls::EdgePathCreator* myEdgePathCreator;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};


#endif

/****************************************************************************/
