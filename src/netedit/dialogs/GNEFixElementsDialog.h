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
/// @file    GNEFixElementsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2023
///
// Dialog used to fix elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/tests/InternalTestStep.h>

#include "GNEDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixElementsDialog : protected GNEDialog {
    /// @brief FOX-declaration
    FXDECLARE_ABSTRACT(GNEFixElementsDialog)

public:
    /// @brief Constructor
    GNEFixElementsDialog(GNEViewNet* viewNet, const std::string title, GUIIcon icon, const int sizeX, const int sizeY);

    /// @brief destructor
    ~GNEFixElementsDialog();

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogTest* modalArguments) = 0;

    /// @name FOX-callbacks
    /// @{

    /// @brief event when user select a option
    virtual long onCmdSelectOption(FXObject* obj, FXSelector, void*) = 0;

    /// @brief event after press accept button
    virtual long onCmdAccept(FXObject*, FXSelector, void*) = 0;

    /// @brief event after press cancel button
    virtual long onCmdCancel(FXObject*, FXSelector, void*) = 0;

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEFixElementsDialog)

    /// @brief horizontal frame for buttons
    class Buttons : public FXHorizontalFrame {

    public:
        /// @brief build Position Options
        Buttons(GNEFixElementsDialog* fixElementsDialog);

        /// @brief accept button
        FXButton* myAcceptButton = nullptr;

        /// @brief cancel button
        FXButton* myCancelButton = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        Buttons(const Buttons&) = delete;

        /// @brief Invalidated assignment operator.
        Buttons& operator=(const Buttons&) = delete;
    };

    /// @brief stop fix elements dialog accepting changes
    long closeFixDialog(const bool success);

    /// @brief view net
    GNEViewNet* myViewNet = nullptr;

    /// @brief main
    FXVerticalFrame* myMainFrame = nullptr;

    /// @brief buttons
    Buttons* myButtons = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixElementsDialog(const GNEFixElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixElementsDialog& operator=(const GNEFixElementsDialog&) = delete;
};
