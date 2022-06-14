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
/// @file    GNEToolDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Abstract dialog for tools
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/FXTreeListDynamic.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEUndoList;
class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEToolDialog
 * @brief Abstract dialog for tools
 */
class GNEToolDialog : protected FXTopWindow {
    /// @brief FOX-declaration
    FXDECLARE(GNEToolDialog)

public:
    /// @brief Constructor
    GNEToolDialog(GNEApplicationWindow* GNEApp, const std::string &name, const int dialogWidth, const int dialogHeight);

    /// @brief destructor
    ~GNEToolDialog();

    /// @brief open window
    void openToolDialog();

    /// @brief hide window
    void hideToolDialog();

    /// @brief shown
    bool shown() const;

    /// @brief Move the focus to this window
    void setFocus();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press close button
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief event after update command
    long onCmdUpdate(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEToolDialog)

    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

private:
    /// @brief Invalidated copy constructor.
    GNEToolDialog(const GNEToolDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEToolDialog& operator=(const GNEToolDialog&) = delete;
};

