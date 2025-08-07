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
/// @file    GNEFixNetworkElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog used to fix network elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEFixElementsDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNENetworkElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixNetworkElements : public GNEFixElementsDialog<GNENetworkElement*> {

public:
    /// @brief groupbox for all radio buttons related with fix edges options
    class FixEdgeOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(FixEdgeOptions)

    public:
        /// @brief constructor
        FixEdgeOptions(GNEFixNetworkElements* fixNetworkElementsParent);

        /// @brief select internal test solution
        void selectInternalTestSolution(const std::string &solution);

        /// @brief apply selected fix option
        bool applyFixOption();

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(FixEdgeOptions)

        /// @brief Option "Remove invalid edges"
        FXRadioButton* myRemoveInvalidEdges = nullptr;

        /// @brief Option "Save invalid edges"
        FXRadioButton* mySaveInvalidEdges = nullptr;

        /// @brief Option "Select invalid edges and cancel"
        FXRadioButton* mySelectInvalidEdges = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        FixEdgeOptions(const FixEdgeOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixEdgeOptions& operator=(const FixEdgeOptions&) = delete;
    };

    /// @brief groupbox for all radio buttons related with fix crossing options
    class FixCrossingOptions : public GNEFixElementsDialog::FixOptions {
        /// @brief FOX-declaration
        FXDECLARE(FixCrossingOptions)

    public:
        /// @brief constructor
        FixCrossingOptions(GNEFixNetworkElements* fixNetworkElementsParent);

        /// @brief select internal test solution
        void selectInternalTestSolution(const std::string &solution);

        /// @brief apply selected fix option
        bool applyFixOption();

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        long onCmdSelectOption(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(FixCrossingOptions)

        /// @brief Option "remove invalid elements"
        FXRadioButton* myRemoveInvalidCrossings = nullptr;

        /// @brief Option "save invalid crossings"
        FXRadioButton* mySaveInvalidCrossings = nullptr;

        /// @brief Option "Select invalid crossings and cancel"
        FXRadioButton* mySelectInvalidCrossings = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        FixCrossingOptions(const FixCrossingOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixCrossingOptions& operator=(const FixCrossingOptions&) = delete;
    };

    /// @brief Constructor
    GNEFixNetworkElements(GNEApplicationWindow *mainWindow,
                          const std::vector<GNENetworkElement*>& elements);

    /// @brief destructor
    ~GNEFixNetworkElements();

    /// @name FOX-callbacks
    /// @{

    /// @brief event when user select a option
    long onCmdSelectOption(FXObject* obj, FXSelector, void*);

    /// @}

protected:
    /// @brief fix edge options
    FixEdgeOptions* myFixEdgeOptions = nullptr;

    /// @brief fix crossing options
    FixCrossingOptions* myFixCrossingOptions = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixNetworkElements(const GNEFixNetworkElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixNetworkElements& operator=(const GNEFixNetworkElements&) = delete;
};
