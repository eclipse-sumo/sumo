/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStopFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// The Widget for add Stops elements
/****************************************************************************/
#ifndef GNEStopFrame_h
#define GNEStopFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"

#include <utils/vehicle/SUMOVehicleParameter.h>

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

    class HelpCreation : protected FXGroupBox {

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
        FXLabel* myInformationLabel;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEStopFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEStopFrame();

    /// @brief show Frame
    void show();

    /**@brief add Stop element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @param shiftPressed flag to check if during clicking shift key was pressed
     * @return true if Stop was sucesfully added
     */
    bool addStop(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool shiftPressed);

    /// @brief get stop parameters
    static bool getStopParameter(SUMOVehicleParameter::Stop& stop, bool& friendlyPosition, const SumoXMLTag stopTag,
                                 GNEViewNet* viewNet, const GNEFrameAttributesModuls::AttributesCreator* stopAttributes,
                                 const GNEFrameAttributesModuls::NeteditAttributes* myNeteditAttributes,
                                 const GNELane* lane, const GNEAdditional* stoppingPlace);

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    void demandElementSelected();

private:
    /// @brief Stop parent selectors
    GNEFrameModuls::DemandElementSelector* myStopParentSelector;

    /// @brief stop tag selector selector (used to select diffent kind of Stops)
    GNEFrameModuls::TagSelector* myStopTagSelector;

    /// @brief internal Stop attributes
    GNEFrameAttributesModuls::AttributesCreator* myStopAttributes;

    /// @brief Netedit parameter
    GNEFrameAttributesModuls::NeteditAttributes* myNeteditAttributes;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};


#endif

/****************************************************************************/
