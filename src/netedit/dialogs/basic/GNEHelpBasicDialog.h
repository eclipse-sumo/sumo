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
/// @file    GNEAboutDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Help dialog used in netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/basic/GNEBasicDialog.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEHelpBasicDialog : public GNEDialog {

public:
    /// @brief Constructor
    GNEHelpBasicDialog(GNEApplicationWindow* applicationWindow, const std::string &header, const std::string &text);

    /// @brief Destructor
    ~GNEHelpBasicDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief called when accept button is pressed
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief called when cancel button is pressed (or dialog is closed)
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @}

private:
    /// @brief Invalidated copy constructor.
    GNEHelpBasicDialog(const GNEHelpBasicDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEHelpBasicDialog& operator=(const GNEHelpBasicDialog& src) = delete;
};
