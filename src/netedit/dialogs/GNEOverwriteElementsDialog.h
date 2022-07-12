/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEOverwriteElementsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/FXGroupBoxModule.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEOverwriteElementsDialog
 * @brief Dialog for edit rerouters
 */
class GNEOverwriteElementsDialog : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEOverwriteElementsDialog)

public:
    /// @brief Constructor
    GNEOverwriteElementsDialog(GNEApplicationWindow* applicationWindow);

    /// @brief destructor
    ~GNEOverwriteElementsDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user select an option
    long onCmdSelectOption(FXObject* obj, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEOverwriteElementsDialog)

private:
    /// @brief Invalidated copy constructor.
    GNEOverwriteElementsDialog(const GNEOverwriteElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEOverwriteElementsDialog& operator=(const GNEOverwriteElementsDialog&) = delete;
};

