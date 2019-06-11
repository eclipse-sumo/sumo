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
    EdgePathCreator* getEdgePathCreator() const;

protected:
    /// @brief enable moduls depending of item selected in TagSelector
    void enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

    /// @brief finish edge path creation
    void finishEdgePathCreation();

private:
    /// @brief person tag selector (used to select diffent kind of persons)
    TagSelector* myPersonTagSelector;
    
    /// @brief Person Type selectors
    DemandElementSelector* myPTypeSelector;

    /// @brief person plan selector (used to select diffent kind of person plan)
    TagSelector* myPersonPlanSelector;

    /// @brief internal vehicle attributes
    AttributesCreator* myPersonAttributes;

    /// @brief edge path creator (used for Walks, rides and trips)
    EdgePathCreator* myEdgePathCreator;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};


#endif

/****************************************************************************/
