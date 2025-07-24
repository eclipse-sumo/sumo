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
/// @file    GNEKeepElementsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEKeepElementsDialog : public GNEDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEKeepElementsDialog)

public:
    /// @brief result
    enum class Result {
        ACCEPT,     // load elements
        CANCEL,     // cancel load
        OVERWRITE   // load elements, overwriting elements with the same ID
    };

    /// @brief Constructor
    GNEKeepElementsDialog(GNEApplicationWindow* applicationWindow, const std::string elementType);

    /// @brief destructor
    ~GNEKeepElementsDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief get result
    Result getResult() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user select an option
    long onCmdSelectOption(FXObject* obj, FXSelector, void*);

    /// @brief called when accept button is pressed
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief called when cancel button is pressed (or dialog is closed)
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEKeepElementsDialog)

    /// @brief button for accept
    FXButton* myKeepOldButton = nullptr;

    /// @brief button for cancel
    FXButton* myCancelButton = nullptr;

    /// @brief button for overwrite
    FXButton* myKeepNewButton = nullptr;

    /// @brief result (by default cancel)
    Result myResult = Result::CANCEL;

private:
    /// @brief Invalidated copy constructor.
    GNEKeepElementsDialog(const GNEKeepElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEKeepElementsDialog& operator=(const GNEKeepElementsDialog&) = delete;
};
