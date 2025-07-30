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

#include <netedit/dialogs/GNEDialog.h>
#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFixOptions;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixElementsDialog : protected GNEDialog {

public:
    /// @brief Constructor
    GNEFixElementsDialog(GNEApplicationWindow *mainWindow, const std::string title,
                         GUIIcon icon, const int sizeX, const int sizeY);

    /// @brief destructor
    ~GNEFixElementsDialog();

    /// @brief add fix options to the dialog (called automatically during GNEFixOptions constructor)
    void addFixOptions(GNEFixOptions* fixOptions);

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief vector with all fix options
    std::vector<GNEFixOptions*> myFixOptions;

    /// @brief left frame in which place the FXFixOptions
    FXVerticalFrame* myLeftFrame = nullptr;

    /// @brief right frame in which place the FXFixOptions
    FXVerticalFrame* myRightFrame = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixElementsDialog(const GNEFixElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixElementsDialog& operator=(const GNEFixElementsDialog&) = delete;
};
