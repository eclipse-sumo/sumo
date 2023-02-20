/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <utils/foxtools/MFXGroupBoxModule.h>

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
    /// @brief result
    enum class Result {
        ACCEPT,     // load elements
        CANCEL,     // cancel load
        OVERWRITE   // load elements, overwritting elements with the same ID
    };

    /// @brief Constructor
    GNEOverwriteElementsDialog(GNEApplicationWindow* applicationWindow, const std::string elementType);

    /// @brief destructor
    ~GNEOverwriteElementsDialog();

    /// @brief get result
    Result getResult() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user select an option
    long onCmdSelectOption(FXObject* obj, FXSelector, void*);

    /// @brief event when dialog is closed
    long onCmdClose(FXObject* obj, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEOverwriteElementsDialog)

    /// @brief button for accept
    FXButton* myAcceptButton = nullptr;

    /// @brief button for cancel
    FXButton* myCancelButton = nullptr;

    /// @brief button for overwritte
    FXButton* myOverwriteButton = nullptr;

    /// @brief result (by default cancel)
    Result myResult = Result::CANCEL;

private:
    /// @brief Invalidated copy constructor.
    GNEOverwriteElementsDialog(const GNEOverwriteElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEOverwriteElementsDialog& operator=(const GNEOverwriteElementsDialog&) = delete;
};

