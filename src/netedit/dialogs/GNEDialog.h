/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Custom FXDialogBox used in Netedit that supports internal tests
/****************************************************************************/
#pragma once
#include <config.h>

#include "fxheader.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDialog : public FXDialogBox {
    FXDECLARE_ABSTRACT(GNEDialog)

public:
    /// @brief constructor
    GNEDialog(GNEApplicationWindow* applicationWindow, const FXString& name, FXuint opts = DECOR_TITLE | DECOR_BORDER, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = 10, FXint pr = 10, FXint pt = 10, FXint pb = 10, FXint hs = 4, FXint vs = 4);

    /// @brief open modal dialog
    bool openModal(FXuint placement = PLACEMENT_CURSOR);

    /// @brief bool to indicate if this dialog was closed accepting or rejecting changes
    bool getAccepted() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when accept button is pressed
    virtual long onCmdAccept(FXObject*, FXSelector, void*) = 0;

    /// @brief called when cancel button is pressed (or dialog is closed)
    virtual long onCmdCancel(FXObject*, FXSelector, void*) = 0;

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNECalibratorDialog)

    /// @brief close dialog accepting the changes
    long closeDialogAccepting();

    /// @brief close dialog canceling the changes
    long closeDialogCanceling();

    /// @brief pointer to the main window
    GNEApplicationWindow* myApplicationWindow;

    /// @brief bool to indicate if this dialog was closed accepting or rejecting changes
    bool myAccepted = false;

    /// @brief flag to indicate if this dialog is being tested using internal test
    bool myTesting = false;

private:
    /// @brief Invalidated copy constructor.
    GNEDialog(const GNEDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEDialog& operator=(const GNEDialog& src) = delete;
};
