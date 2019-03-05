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

#include <utils/router/SUMOAbstractRouter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEVehicleFrame
 */
class GNEVehicleFrame : public GNEFrame {

public:

    // ===========================================================================
    // class VTypeSelector
    // ===========================================================================

    class VTypeSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEVehicleFrame::VTypeSelector)

    public:
        /// @brief constructor
        VTypeSelector(GNEVehicleFrame* vehicleFrameParent);

        /// @brief destructor
        ~VTypeSelector();

        /// @brief get current type tag
        const GNEDemandElement* getCurrentVehicleType() const;

        /// @brief show VType selector
        void showVTypeSelector(const GNEAttributeCarrier::TagProperties& tagProperties);

        /// @brief hide VType selector
        void hideVTypeSelector();

        /// @brief refresh VType selector
        void refreshVTypeSelector();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectVType(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        VTypeSelector() {}

    private:
        /// @brief pointer to Vehicle Frame Parent
        GNEVehicleFrame* myVehicleFrameParent;

        /// @brief comboBox with the list of elements type
        FXComboBox* myTypeMatchBox;

        /// @brief current vehicle type
        GNEDemandElement* myCurrentVehicleType;
    };

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

    /// @brief structs used for creating routes only givin from-to routes
    struct AutoRoute {
        /// @brief default constructor
        AutoRoute(GNEVehicleFrame* vehicleFrameParent);

        /// @brief destructor
        ~AutoRoute();

        /// @brief check if from and to edges create a valid route
        bool isValid(SUMOVehicleClass vehicleClass) const;

        /// @brief get edge form
        GNEEdge* getFrom() const;
        
        /// @brief get edge to
        GNEEdge* getTo() const;

        /// @brief set edge from (and change color)
        void setFrom(GNEEdge* from);
        
        /// @brief set edge to (and change color)
        void setTo(GNEEdge* to);

        /// @brief clear edges (and restore colors)
        void clearEdges();

        /// @brief update abstract router edges
        void updateAbstractRouterEdges();

        /// @brief draw temporal route
        void drawRoute();

    private:
        /// @brief pointer to Vehicle Frame Parent
        GNEVehicleFrame* myVehicleFrameParent;

        /// @brief from Edge
        GNEEdge* myFrom;

        /// @brief to Edge
        GNEEdge* myTo;

        /// @brief SUMO Abstract Router
        SUMOAbstractRouter<NBEdge, NBVehicle>* myRouter;
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

protected:
    /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

private:
    /// @brief item selector (used to select diffent kind of vehicles)
    ItemSelector* myItemSelector;

    /// @brief Vehicle Type selectors
    VTypeSelector* myVTypeSelector;

    /// @brief internal vehicle attributes
    AttributesCreator* myVehicleAttributes;

    /// @brief Help creation
    HelpCreation* myHelpCreation;

    /// @brief AutoRoute
    AutoRoute myAutoRoute;
};


#endif

/****************************************************************************/
