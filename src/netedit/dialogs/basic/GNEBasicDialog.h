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
/// @file    GNEBasicDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// A basic dialog with simple buttons (ok, accept, cancel, etc.)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEBasicDialog : protected GNEDialog {

public:
    // make get result public
    using GNEDialog::getResult;

    /// @brief Constructor
    GNEBasicDialog(GNEApplicationWindow* applicationWindow, const std::string &title,
                   const std::string &info, GNEDialog::Buttons buttons, GUIIcon titleIcon,
                   GUIIcon largeIcon);

    /// @brief Destructor
    ~GNEBasicDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief called when accept / yes / ok button is pressed
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief called when cancel button is pressed
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @}

private:
    /// @brief Invalidated copy constructor.
    GNEBasicDialog(const GNEBasicDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEBasicDialog& operator=(const GNEBasicDialog& src) = delete;
};
