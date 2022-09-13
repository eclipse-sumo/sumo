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
/// @file    GNECursorDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2022
///
// Dialog for edit element under cursor
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>


// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECursorDialog
 * @brief Dialog for edit rerouter intervals
 */

class GNECursorDialog : public GUIGLObjectPopupMenu {
    /// @brief FOX-declaration
    FXDECLARE(GNECursorDialog)

public:
    /// @brief constructor
    GNECursorDialog(GUIMainWindow* app, GUISUMOAbstractView* parent);

    /// @brief destructor
    ~GNECursorDialog();

    /// @name FOX-callbacks
    /// @{

    /// @brief select element in list
    long onCmdSelectElement(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNECursorDialog)

private:
    /// @brief Invalidated copy constructor.
    GNECursorDialog(const GNECursorDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNECursorDialog& operator=(const GNECursorDialog&) = delete;
};

