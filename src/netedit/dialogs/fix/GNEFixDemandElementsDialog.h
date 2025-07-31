/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEFixDemandElementsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// Dialog used to fix demand elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEFixElementsDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixDemandElementsDialog : public GNEFixElementsDialog<GNEDemandElement*> {

public:
    /// @brief groupbox for all radio buttons related with fix route options
    class FixRouteOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(FixRouteOptions)

    public:
        /// @brief constructor
        FixRouteOptions(GNEFixDemandElementsDialog* fixDemandElementsParent);

        /// @brief run internal test
        void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);
		
		/// @brief apply selected fix option
        bool applyFixOption();
		
		/// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(FixRouteOptions)

        /// @brief Option "Remove invalid routes"
        FXRadioButton* myRemoveInvalidRoutes;

        /// @brief Option "Save invalid routes"
        FXRadioButton* mySaveInvalidRoutes;

        /// @brief Option "Select invalid routes and cancel"
        FXRadioButton* mySelectRouteInvalids;

        /// @brief Option "Remove stops out of route"
        FXCheckButton* myRemoveStopsOutOfRoute;

    private:
        /// @brief enable route options
        void enableOptions();

        /// @brief disable route options
        void disableOptions();

        /// @brief Invalidated copy constructor.
        FixRouteOptions(const FixRouteOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixRouteOptions& operator=(const FixRouteOptions&) = delete;
    };

    /// @brief groupbox for all radio buttons related with fix vehicle options
    class FixVehicleOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(FixVehicleOptions)

    public:
        /// @brief constructor
        FixVehicleOptions(GNEFixDemandElementsDialog* fixDemandElementsParent);

        /// @brief run internal test
        void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);
		
		/// @brief apply selected fix option
        bool applyFixOption();
		
		/// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(FixVehicleOptions)

        /// @brief Option "remove invalid elements"
        FXRadioButton* myRemoveInvalidVehicles;

        /// @brief Option "save invalid vehicles"
        FXRadioButton* mySaveInvalidVehicles;

        /// @brief Option "Select invalid vehicles and cancel"
        FXRadioButton* mySelectInvalidVehicles;

        /// @brief Option "Remove stops out of vehicle"
        FXCheckButton* myRemoveStopsOutOfVehicle;

    private:
        /// @brief enable vehicle options
        void enableOptions();

        /// @brief disable vehicle options
        void disableOptions();

        /// @brief Invalidated copy constructor.
        FixVehicleOptions(const FixVehicleOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixVehicleOptions& operator=(const FixVehicleOptions&) = delete;
    };

    /// @brief groupbox for all radio buttons related with fix stop options
    class FixStopPositionOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(FixStopPositionOptions)

    public:
        /// @brief build Position Options
        FixStopPositionOptions(GNEFixDemandElementsDialog* fixDemandElementsParent);

        /// @brief run internal test
        void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);
		
		/// @brief apply selected fix option
        bool applyFixOption();
		
		/// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(FixStopPositionOptions)

        /// @brief Option "Activate friendlyPos and save"
        FXRadioButton* myActivateFriendlyPosition;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* myFixPositions;

        /// @brief Option "Save invalid"
        FXRadioButton* mySaveInvalid;

        /// @brief Option "Select invalid stops and cancel"
        FXRadioButton* mySelectInvalidStops;

    private:
        /// @brief enable stop options
        void enableOptions();

        /// @brief disable stop options
        void disableOptions();

        /// @brief Invalidated copy constructor.
        FixStopPositionOptions(const FixStopPositionOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixStopPositionOptions& operator=(const FixStopPositionOptions&) = delete;
    };

    /// @brief groupbox for all radio buttons related with fix person plan options
    class FixPlanOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(FixPlanOptions)

    public:
        /// @brief build Position Options
        FixPlanOptions(GNEFixDemandElementsDialog* fixDemandElementsParent);

        /// @brief run internal test
        void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);
		
		/// @brief apply selected fix option
        bool applyFixOption();
		
		/// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(FixPlanOptions)

        /// @brief Option "remove invalid plan"
        FXRadioButton* myRemoveInvalidPlan;

        /// @brief Option "Save invalid"
        FXRadioButton* mySaveInvalid;

        /// @brief Option "Select invalid person plans and cancel"
        FXRadioButton* mySelectInvalidPlans;

    private:
        /// @brief enable personPlan options
        void enableOptions();

        /// @brief disable personPlan options
        void disableOptions();

        /// @brief Invalidated copy constructor.
        FixPlanOptions(const FixPlanOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixPlanOptions& operator=(const FixPlanOptions&) = delete;
    };

    /// @brief Constructor
    GNEFixDemandElementsDialog(GNEApplicationWindow *mainWindow);

    /// @brief destructor
    ~GNEFixDemandElementsDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief open fix dialog
    GNEDialog::Result openDialog(const std::vector<GNEDemandElement*>& element);

protected:
    /// @brief fix route options
    FixRouteOptions* myFixRouteOptions = nullptr;

    /// @brief fix vehicle options
    FixVehicleOptions* myFixVehicleOptions = nullptr;

    /// @brief fix stop options
    FixStopPositionOptions* myFixStopPositionOptions = nullptr;

    /// @brief fix person plan options
    FixPlanOptions* myFixPlanOptions = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixDemandElementsDialog(const GNEFixDemandElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixDemandElementsDialog& operator=(const GNEFixDemandElementsDialog&) = delete;
};
