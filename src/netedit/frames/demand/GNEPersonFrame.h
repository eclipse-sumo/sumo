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
/// @file    GNEPersonFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add person elements
/****************************************************************************/
#pragma once

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPersonFrame
 */
class GNEPersonFrame : public GNEFrame {

public:
    // ===========================================================================
    // class HelpCreation
    // ===========================================================================

    class HelpCreation : protected FXGroupBox {

    public:
        /// @brief constructor
        HelpCreation(GNEPersonFrame* vehicleFrameParent);

        /// @brief destructor
        ~HelpCreation();

        /// @brief show HelpCreation
        void showHelpCreation();

        /// @brief hide HelpCreation
        void hideHelpCreation();

        /// @brief update HelpCreation
        void updateHelpCreation();

    private:
        /// @brief pointer to Person Frame Parent
        GNEPersonFrame* myPersonFrameParent;

        /// @brief Label with creation information
        FXLabel* myInformationLabel;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEPersonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEPersonFrame();

    /// @brief show Frame
    void show();

    /**@brief add vehicle element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if vehicle was sucesfully added
     */
    bool addPerson(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief get EdgePathCreator modul
    GNEFrameModuls::EdgePathCreator* getEdgePathCreator() const;

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    void demandElementSelected();

    /// @brief finish edge path creation
    void edgePathCreated();

private:
    /// @brief person tag selector (used to select diffent kind of persons)
    GNEFrameModuls::TagSelector* myPersonTagSelector;

    /// @brief Person Type selectors
    GNEFrameModuls::DemandElementSelector* myPTypeSelector;

    /// @brief person plan selector (used to select diffent kind of person plan)
    GNEFrameModuls::TagSelector* myPersonPlanTagSelector;

    /// @brief internal vehicle attributes
    GNEFrameAttributesModuls::AttributesCreator* myPersonAttributes;

    /// @brief internal person plan attributes
    GNEFrameAttributesModuls::AttributesCreator* myPersonPlanAttributes;

    /// @brief Netedit parameter
    GNEFrameAttributesModuls::NeteditAttributes* myNeteditAttributes;

    /// @brief edge path creator (used for Walks, rides and trips)
    GNEFrameModuls::EdgePathCreator* myEdgePathCreator;

    /// @brief Help creation
    HelpCreation* myHelpCreation;

    /// @brief build person over route
    bool buildPersonOverRoute(GNEDemandElement* route);

    /// @brief build person over stop
    bool buildPersonOverStop(GNELane* lane, GNEAdditional* busStop);

    /// @brief build person and return it (note: function includes a call to p_begin(...), but NOT a call to p_end(...))
    GNEDemandElement* buildPerson();
};
