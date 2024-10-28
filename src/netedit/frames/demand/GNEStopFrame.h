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
/// @file    GNEStopFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// The Widget for add Stops elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEAttributesCreator.h>
#include <netedit/frames/GNENeteditAttributes.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNETagSelector.h>
#include <utils/foxtools/MFXDynamicLabel.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEStopFrame
 */
class GNEStopFrame : public GNEFrame {

public:

    // ===========================================================================
    // class HelpCreation
    // ===========================================================================

    class HelpCreation : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        HelpCreation(GNEStopFrame* StopFrameParent);

        /// @brief destructor
        ~HelpCreation();

        /// @brief show HelpCreation
        void showHelpCreation();

        /// @brief hide HelpCreation
        void hideHelpCreation();

        /// @brief update HelpCreation
        void updateHelpCreation();

    private:
        /// @brief pointer to Stop Frame Parent
        GNEStopFrame* myStopFrameParent;

        /// @brief Label with creation information
        MFXDynamicLabel* myInformationLabel;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEStopFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEStopFrame();

    /// @brief show Frame
    void show();

    /**@brief add Stop element
     * @param viewObjects collection of objects under cursor after click over view
     * @param mouseButtonKeyPressed key pressed during click
     * @return true if Stop was successfully added
     */
    bool addStop(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed);

    /// @brief get stop parameters
    bool getStopParameter(const SumoXMLTag stopTag, const GNELane* lane, const GNEAdditional* stoppingPlace);

    /// @brief get stop parent selector
    GNEDemandElementSelector* getStopParentSelector() const;

protected:
    /// @brief Tag selected in GNETagSelector
    void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    void demandElementSelected();

private:
    /// @brief route handler
    GNERouteHandler myRouteHandler;

    /// @brief stop parent base object
    CommonXMLStructure::SumoBaseObject* myStopParentBaseObject;

    /// @brief plan parameters
    CommonXMLStructure::PlanParameters myPlanParameters;

    /// @brief Stop parent selectors
    GNEDemandElementSelector* myStopParentSelector;

    /// @brief stop tag selector selector (used to select diffent kind of Stops)
    GNETagSelector* myStopTagSelector;

    /// @brief internal Stop attributes
    GNEAttributesCreator* myStopAttributes;

    /// @brief Netedit parameter
    GNENeteditAttributes* myNeteditAttributes;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};
