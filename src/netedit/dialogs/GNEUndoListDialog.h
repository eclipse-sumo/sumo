/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEUndoListDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2021
///
// Dialog for show undo-list
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEAdditionalDialog.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GNEUndoList;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEUndoListDialog
 * @brief Dialog for edit rerouters
 */
class GNEUndoListDialog : protected FXTopWindow {
    /// @brief FOX-declaration
    FXDECLARE(GNEUndoListDialog)

public:
    /// @brief Constructor
    GNEUndoListDialog(GNEViewNet* viewNet);

    /// @brief destructor
    ~GNEUndoListDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after selecting an element in the undoredolist
    long onCmdSelectElement(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(GNEUndoListDialog)

    /// @brief pointer to viewNet
    GNEViewNet* myViewNet;

private:
    /// @brief update data table
    void updateTable();

    /// @brief Invalidated copy constructor.
    GNEUndoListDialog(const GNEUndoListDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEUndoListDialog& operator=(const GNEUndoListDialog&) = delete;
};

