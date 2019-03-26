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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEStopFrame
 */
class GNEStopFrame : public GNEFrame {

public:

    // ===========================================================================
    // class RouteSelector
    // ===========================================================================

    class RouteSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEStopFrame::RouteSelector)

    public:
        /// @brief constructor
        RouteSelector(GNEStopFrame* StopFrameParent);

        /// @brief destructor
        ~RouteSelector();

        /// @brief get current route
        const GNEDemandElement* getCurrentRoute() const;

        /// @brief show VType selector
        void showRouteSelector(const GNEAttributeCarrier::TagProperties& tagProperties);

        /// @brief hide VType selector
        void hideRouteSelector();

        /// @brief refresh VType selector
        void refreshRouteSelector();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectRoute(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        RouteSelector() {}

    private:
        /// @brief pointer to Stop Frame Parent
        GNEStopFrame* myStopFrameParent;

        /// @brief comboBox with the list of routes
        FXComboBox* myRouteMatchBox;

        /// @brief current route
        GNEDemandElement* myCurrentRoute;
    };

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
     * @return true if Stop was sucesfully added
     */
    bool addStop(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

protected:
    /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

private:
    /// @brief item selector (used to select diffent kind of Stops)
    ItemSelector* myItemSelector;

    /// @brief Stop Type selectors
    RouteSelector* myRouteSelector;

    /// @brief internal Stop attributes
    AttributesCreator* myStopAttributes;

    /// @brief Help creation
    HelpCreation* myHelpCreation;
};


#endif

/****************************************************************************/
