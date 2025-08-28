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
/// @file    GNEFilePathDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// A basic dialog for selecting a file path (used in GNEFileSelector)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declaration
// ===========================================================================

class MFXTextFieldTooltip;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFilePathDialog : public GNEDialog {

public:
    /// @brief Constructor
    GNEFilePathDialog(GNEApplicationWindow* applicationWindow, const std::string& title,
                      const std::string& info, const std::string& originalFilePath);

    /// @brief Destructor
    ~GNEFilePathDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief get the file path
    std::string getFilePath() const;

    /// @brief called when cancel or no button is pressed (can be reimplemented in children)
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief called when reset button is pressed (must be reimplemented in children)
    long onCmdReset(FXObject*, FXSelector, void*);

private:
    /// @brief original file path (used for reset)
    std::string myOriginalFilePath;

    /// @brief text field to enter the path
    MFXTextFieldTooltip* myPathTextField = nullptr;

    /// @brief Invalidated copy constructor.
    GNEFilePathDialog(const GNEFilePathDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEFilePathDialog& operator=(const GNEFilePathDialog& src) = delete;
};
