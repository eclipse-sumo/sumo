/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEFixAdditionalElementsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to fix additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEFixElementsDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixAdditionalElementsDialog : public GNEFixElementsDialog<GNEAdditional*> {

public:
    /// @brief Position options
    class PositionOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(PositionOptions)

    public:
        /// @brief build Position Options
        PositionOptions(GNEFixAdditionalElementsDialog* fixAdditionalPositions);

        /// @brief select internal test solution
        void selectInternalTestSolution(const std::string& solution);

        /// @brief apply selected fix option
        bool applyFixOption();

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(PositionOptions)

        /// @brief Option "Activate friendlyPos and save"
        FXRadioButton* myActivateFriendlyPosition;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* myFixPositions;

        /// @brief Option "Save invalid"
        FXRadioButton* mySaveInvalids;

        /// @brief Option "Select invalid stops and cancel"
        FXRadioButton* mySelectInvalids;

    private:
        /// @brief Invalidated copy constructor.
        PositionOptions(const PositionOptions&) = delete;

        /// @brief Invalidated assignment operator.
        PositionOptions& operator=(const PositionOptions&) = delete;
    };

    /// @brief fix consecutive lane options
    class ConsecutiveLaneOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(ConsecutiveLaneOptions)

    public:
        /// @brief build consecutive lane Options
        ConsecutiveLaneOptions(GNEFixAdditionalElementsDialog* fixAdditionalPositions);

        /// @brief select internal test solution
        void selectInternalTestSolution(const std::string& solution);

        /// @brief apply selected fix option
        bool applyFixOption();

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(ConsecutiveLaneOptions)

        /// @brief Option "build connections between lanes"
        FXRadioButton* myBuildConnectionBetweenLanes;

        /// @brief Option "remove invalid elements"
        FXRadioButton* myRemoveInvalidElements;

        /// @brief Option "Activate friendlyPos and save"
        FXRadioButton* myActivateFriendlyPosition;

        /// @brief Option "Fix Positions and save"
        FXRadioButton* myFixPositions;

    private:
        /// @brief Invalidated copy constructor.
        ConsecutiveLaneOptions(const ConsecutiveLaneOptions&) = delete;

        /// @brief Invalidated assignment operator.
        ConsecutiveLaneOptions& operator=(const ConsecutiveLaneOptions&) = delete;
    };

    /// @brief Constructor
    GNEFixAdditionalElementsDialog(GNEApplicationWindow* mainWindow,
                                   const std::vector<GNEAdditional*>& elements);

    /// @brief destructor
    ~GNEFixAdditionalElementsDialog();

protected:
    /// @brief position options
    PositionOptions* myPositionOptions;

    /// @brief consecutive lane options
    ConsecutiveLaneOptions* myConsecutiveLaneOptions;

private:
    /// @brief Invalidated copy constructor.
    GNEFixAdditionalElementsDialog(const GNEFixAdditionalElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixAdditionalElementsDialog& operator=(const GNEFixAdditionalElementsDialog&) = delete;
};
