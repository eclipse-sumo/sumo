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
/// @file    GNEFixDemandElements.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// Dialog used to fix demand elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/FXGroupBoxModule.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEDemandElement;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEFixDemandElements
 * @brief Dialog for edit rerouters
 */
class GNEFixDemandElements : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEFixDemandElements)

public:
    /// @brief Constructor
    GNEFixDemandElements(GNEViewNet* viewNet, const std::vector<GNEDemandElement*>& invalidDemandElements);

    /// @brief destructor
    ~GNEFixDemandElements();

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
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEFixDemandElements)

    /// @brief general GroupBox for fix options
    class FixOptions : public FXGroupBoxModule {

    public:
        /// @brief constructor
        FixOptions(FXVerticalFrame* frameParent, const std::string &title);

        /// @brief set invalid demand elements
        void setInvalidElements(const std::vector<GNEDemandElement*> &invalidElements);

    protected:
        /// @brief vertical left frame
        FXVerticalFrame* myLeftFrame = nullptr;

        /// @brief vertical right frame
        FXVerticalFrame* myRightFrame = nullptr;

    private:
        /// @brief Table with the demand elements
        FXTable* myTable = nullptr;

        /// @brief vector with the invalid demand elements
        std::vector<GNEDemandElement*> myInvalidElements;
    };


    /// @brief groupbox for all radio buttons related with fix route options
    class FixRouteOptions : public FixOptions {

    public:
        /// @brief constructor
        FixRouteOptions(GNEFixDemandElements* fixDemandElementsParent);

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
    class FixVehicleOptions : public FixOptions {

    public:
        /// @brief constructor
        FixVehicleOptions(GNEFixDemandElements* fixDemandElementsParent);

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
    class FixStopOptions : public FixOptions {

    public:
        /// @brief build Position Options
        FixStopOptions(GNEFixDemandElements* fixDemandElementsParent);

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

    /// @brief groupbox for all radio buttons related with fix person plan options
    class FixPersonPlanOptions : public FixOptions {

    public:
        /// @brief build Position Options
        FixPersonPlanOptions(GNEFixDemandElements* fixDemandElementsParent);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief enable position options
        void enableFixPersonPlanOptions();

        /// @brief disable position options
        void disableFixPersonPlanOptions();

        /// @brief Option "delete person plan"
        FXRadioButton* deletePersonPlan;

        /// @brief Option "Save invalid"
        FXRadioButton* saveInvalid;

        /// @brief Option "Select invalid person plans and cancel"
        FXRadioButton* selectInvalidPersonPlansAndCancel;
    };

    /// @brief view net
    GNEViewNet* myViewNet = nullptr;

    /// @brief main frame
    FXVerticalFrame* myMainFrame = nullptr;

    /// @brief vertical left frame
    FXVerticalFrame* myLeftFrame = nullptr;

    /// @brief vertical right frame
    FXVerticalFrame* myRightFrame = nullptr;

    /// @brief fix route options
    FixRouteOptions* myFixRouteOptions = nullptr;

    /// @brief fix vehicle options
    FixVehicleOptions* myFixVehicleOptions = nullptr;

    /// @brief fix stop options
    FixStopOptions* myFixStopOptions = nullptr;

    /// @brief fix person plan options
    FixPersonPlanOptions* myFixPersonPlanOptions = nullptr;

    /// @brief accept button
    FXButton* myAcceptButton = nullptr;

    /// @brief cancel button
    FXButton* myCancelButton = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixDemandElements(const GNEFixDemandElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixDemandElements& operator=(const GNEFixDemandElements&) = delete;
};

