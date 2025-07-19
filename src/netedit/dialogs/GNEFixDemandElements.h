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
/// @file    GNEFixDemandElements.h
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

class GNEFixDemandElements : public GNEFixElementsDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEFixDemandElements)

public:
    /// @brief Constructor
    GNEFixDemandElements(GNEViewNet* viewNet);

    /// @brief destructor
    ~GNEFixDemandElements();

    /// @brief open fix demand elements dialog
    FXuint openDialog(const std::vector<GNEDemandElement*>& invalidDemandElements);

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogTest* dialogTest);

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
    class FixOptions : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        FixOptions(FXVerticalFrame* frameParent, const std::string& title, GNEViewNet* viewNet);

        /// @brief set invalid demand elements
        void setInvalidElements(const std::vector<GNEDemandElement*>& invalidElements);

        /// @brief fix elements
        virtual void fixElements(bool& abortSaving) = 0;

    protected:
        /// @brief save contents
        bool saveContents() const;

        /// @brief vertical left frame
        FXVerticalFrame* myLeftFrame = nullptr;

        /// @brief vertical right frame
        FXVerticalFrame* myRightFrame = nullptr;

        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief vector with the invalid demand elements
        std::vector<GNEDemandElement*> myInvalidElements;

    private:
        /// @brief enable options
        virtual void enableOptions() = 0;

        /// @brief disable options
        virtual void disableOptions() = 0;

        /// @brief Table with the demand elements
        FXTable* myTable = nullptr;

        /// @brief Invalidated copy constructor.
        FixOptions(const FixOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixOptions& operator=(const FixOptions&) = delete;
    };

    /// @brief groupbox for all radio buttons related with fix route options
    class FixRouteOptions : public FixOptions {

    public:
        /// @brief constructor
        FixRouteOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief fix elements
        void fixElements(bool& abortSaving);

        /// @brief Option "Remove invalid routes"
        FXRadioButton* removeInvalidRoutes;

        /// @brief Option "Save invalid routes"
        FXRadioButton* saveInvalidRoutes;

        /// @brief Option "Select invalid routes and cancel"
        FXRadioButton* selectRouteInvalids;

        /// @brief Option "Remove stops out of route"
        FXCheckButton* removeStopsOutOfRoute;

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
    class FixVehicleOptions : public FixOptions {

    public:
        /// @brief constructor
        FixVehicleOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief fix elements
        void fixElements(bool& abortSaving);

        /// @brief Option "remove invalid elements"
        FXRadioButton* removeInvalidVehicles;

        /// @brief Option "save invalid vehicles"
        FXRadioButton* saveInvalidVehicles;

        /// @brief Option "Select invalid vehicles and cancel"
        FXRadioButton* selectInvalidVehiclesAndCancel;

        /// @brief Option "Remove stops out of vehicle"
        FXCheckButton* removeStopsOutOfVehicle;

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
    class FixStopPositionOptions : public FixOptions {

    public:
        /// @brief build Position Options
        FixStopPositionOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief fix elements
        void fixElements(bool& abortSaving);

        /// @brief Option "Activate friendlyPos and save"
        FXRadioButton* activateFriendlyPositionAndSave;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* fixPositionsAndSave;

        /// @brief Option "Save invalid"
        FXRadioButton* saveInvalid;

        /// @brief Option "Select invalid stops and cancel"
        FXRadioButton* selectInvalidStopsAndCancel;

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
    class FixPersonPlanOptions : public FixOptions {

    public:
        /// @brief build Position Options
        FixPersonPlanOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet);

        /// @brief select option
        void selectOption(FXObject* option);

        /// @brief fix elements
        void fixElements(bool& abortSaving);

        /// @brief Option "delete person plan"
        FXRadioButton* deletePersonPlan;

        /// @brief Option "Save invalid"
        FXRadioButton* saveInvalid;

        /// @brief Option "Select invalid person plans and cancel"
        FXRadioButton* selectInvalidPersonPlansAndCancel;

    private:
        /// @brief enable personPlan options
        void enableOptions();

        /// @brief disable personPlan options
        void disableOptions();

        /// @brief Invalidated copy constructor.
        FixPersonPlanOptions(const FixPersonPlanOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixPersonPlanOptions& operator=(const FixPersonPlanOptions&) = delete;
    };

    /// @brief vertical left frame
    FXVerticalFrame* myLeftFrame = nullptr;

    /// @brief vertical right frame
    FXVerticalFrame* myRightFrame = nullptr;

    /// @brief fix route options
    FixRouteOptions* myFixRouteOptions = nullptr;

    /// @brief fix vehicle options
    FixVehicleOptions* myFixVehicleOptions = nullptr;

    /// @brief fix stop options
    FixStopPositionOptions* myFixStopPositionOptions = nullptr;

    /// @brief fix person plan options
    FixPersonPlanOptions* myFixPersonPlanOptions = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixDemandElements(const GNEFixDemandElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixDemandElements& operator=(const GNEFixDemandElements&) = delete;
};
