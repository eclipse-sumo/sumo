/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_FixDemandElements.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// Dialog used to fix demand elements during saving
/****************************************************************************/
#ifndef GNEDialog_FixDemandElements_h
#define GNEDialog_FixDemandElements_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <fx.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEDemandElement;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDialog_FixDemandElements
 * @brief Dialog for edit rerouters
 */
class GNEDialog_FixDemandElements : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEDialog_FixDemandElements)

public:
    /// @brief Constructor
    GNEDialog_FixDemandElements(GNEViewNet* viewNet, const std::vector<GNEDemandElement*>& invalidDemandElements);

    /// @brief destructor
    ~GNEDialog_FixDemandElements();

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user select a option
    long onCmdSelectOption(FXObject* obj, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief groupbox for list
    class DemandList : protected FXGroupBox {

    public:
        /// @brief constructor
        DemandList(GNEDialog_FixDemandElements* fixDemandElementsDialogParents, const std::vector<GNEDemandElement*>& invalidDemandElements);

        /// @brief vector with the invalid routes
        std::vector<GNEDemandElement*> myInvalidRoutes;

        /// @brief vector with the invalid vehicles
        std::vector<GNEDemandElement*> myInvalidVehicles;

        /// @brief vector with the invalid stops
        std::vector<GNEDemandElement*> myInvalidStops;

        /// @brief list with the demand elements
        FXTable* myTable;
    };

    /// @brief groupbox for all radio buttons related with fix route options
    class FixRouteOptions : protected FXGroupBox {

    public:
        /// @brief constructor
        FixRouteOptions(GNEDialog_FixDemandElements* fixDemandElementsDialogParents);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief enable position options
        void enableFixRouteOptions();

        /// @brief disable position options
        void disableFixRouteOptions();

        /// @brief Option "Remove invalid routes"
        FXRadioButton* removeInvalidRoutes;

        /// @brief Option "Save invalid routes"
        FXRadioButton* saveInvalidRoutes;

        /// @brief Option "Select invalid routes and cancel"
        FXRadioButton* selectInvalidRoutesAndCancel;
    };

    /// @brief groupbox for all radio buttons related with fix vehicle options
    class FixVehicleOptions : protected FXGroupBox {

    public:
        /// @brief constructor
        FixVehicleOptions(GNEDialog_FixDemandElements* fixDemandElementsDialogParents);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief enable consecutive lane options
        void enableFixVehicleOptions();

        /// @brief disable consecutive lane options
        void disableFixVehicleOptions();

        /// @brief Option "remove invalid elements"
        FXRadioButton* removeInvalidVehicles;

        /// @brief Option "save invalid vehicles"
        FXRadioButton* saveInvalidVehicles;

        /// @brief Option "Select invalid vehicles and cancel"
        FXRadioButton* selectInvalidVehiclesAndCancel;
    };

    /// @brief groupbox for all radio buttons related with fix stop options
    class FixStopOptions : public FXGroupBox {

    public:
        /// @brief build Position Options
        FixStopOptions(GNEDialog_FixDemandElements* fixDemandElementsDialogParents);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief enable position options
        void enableFixStopOptions();

        /// @brief disable position options
        void disableFixStopOptions();

        /// @brief Option "Activate friendlyPos and save"
        FXRadioButton* activateFriendlyPositionAndSave;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* fixPositionsAndSave;

        /// @brief Option "Save invalid"
        FXRadioButton* saveInvalid;

        /// @brief Option "Select invalid stops and cancel"
        FXRadioButton* selectInvalidStopsAndCancel;
    };

    /// @brief FOX needs this
    GNEDialog_FixDemandElements() {}

    /// @brief view net
    GNEViewNet* myViewNet;

    /// @brief main frame
    FXVerticalFrame* myMainFrame;

    /// @brief list with the demand elements
    DemandList* myDemandList;

    /// @brief fix route options
    FixRouteOptions* myFixRouteOptions;

    /// @brief fix vehicle options
    FixVehicleOptions* myFixVehicleOptions;

    /// @brief fix stop options
    FixStopOptions* myFixStopOptions;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

private:
    /// @brief Invalidated copy constructor.
    GNEDialog_FixDemandElements(const GNEDialog_FixDemandElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDialog_FixDemandElements& operator=(const GNEDialog_FixDemandElements&) = delete;
};

#endif
