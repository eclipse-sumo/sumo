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
/// @file    GNEFixElementsRowDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2023
///
// Row used in GNEFixElementsDialog
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEFixElementsDialog.h"


// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixElementsRowDialog : public MFXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEFixElementsRowDialog)

public:
    /// @brief Constructor
    GNEFixElementsRowDialog(GNEFixElementsDialog* fixElementsDialog);

    /// @brief destructor
    ~GNEFixElementsRowDialog();

private:
    /// @brief fix demand element dialog parent
    GNEFixElementsDialog* myFixElementsDialogParent;

    /// @brief Invalidated copy constructor.
    GNEFixElementsRowDialog(const GNEFixElementsRowDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixElementsRowDialog& operator=(const GNEFixElementsRowDialog&) = delete;
};
