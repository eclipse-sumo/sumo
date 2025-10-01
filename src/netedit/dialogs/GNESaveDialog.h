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
/// @file    GNESaveDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Dialog used for saving elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNESaveDialog : public GNEDialog {

public:
    /// @brief Constructor
    GNESaveDialog(GNEApplicationWindow* applicationWindow, const std::string& elementTypes);

    /// @brief Destructor
    ~GNESaveDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief called when accept or yes button is pressed (can be reimplemented in children)
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief called when cancel or no button is pressed (can be reimplemented in children)
    long onCmdCancel(FXObject*, FXSelector, void*);

protected:
    /// @brief apply to all button
    FXCheckButton* myApplyToAllButton = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNESaveDialog(const GNESaveDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNESaveDialog& operator=(const GNESaveDialog& src) = delete;
};
