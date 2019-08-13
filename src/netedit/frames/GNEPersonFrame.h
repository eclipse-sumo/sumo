/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPersonFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
/// @version $Id$
///
// The Widget for add person elements
/****************************************************************************/
#ifndef GNEPersonFrame_h
#define GNEPersonFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"


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
    GNEFrameHelper::EdgePathCreator* getEdgePathCreator() const;

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    void demandElementSelected();

    /// @brief finish edge path creation
    void edgePathCreated();

private:
    /// @brief person tag selector (used to select diffent kind of persons)
    GNEFrameHelper::TagSelector* myPersonTagSelector;
    
    /// @brief Person Type selectors
    GNEFrameHelper::DemandElementSelector* myPTypeSelector;

    /// @brief person plan selector (used to select diffent kind of person plan)
    GNEFrameHelper::TagSelector* myPersonPlanTagSelector;

    /// @brief internal vehicle attributes
    GNEFrameHelper::AttributesCreator* myPersonAttributes;

    /// @brief internal person plan attributes
    GNEFrameHelper::AttributesCreator* myPersonPlanAttributes;

    /// @brief Netedit parameter
    GNEFrameHelper::NeteditAttributes* myNeteditAttributes;

    /// @brief edge path creator (used for Walks, rides and trips)
    GNEFrameHelper::EdgePathCreator* myEdgePathCreator;

    /// @brief Help creation
    HelpCreation* myHelpCreation;

    /// @brief build person over route
    bool buildPersonOverRoute(GNEDemandElement *route);

    /// @brief build person over stop
    bool buildPersonOverStop(GNELane *lane, GNEAdditional *busStop);

    /// @brief build person and return it (note: function includes a call to p_begin(...), but NOT a call to p_end(...))
    GNEDemandElement* buildPerson();
};


#endif

/****************************************************************************/
