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
/// @file    GNENetgenerateDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for netgenerate
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/options/OptionsCont.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;
class GNENetgenerate;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNENetgenerateDialog
 * @brief Dialog for netgenerate tool dialog
 */
class GNENetgenerateDialog : protected FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNENetgenerateDialog)

    /// @brief declare friend class
    friend class GNENetgenerateDialogElements;

public:
    /// @brief Constructor
    GNENetgenerateDialog(GNEApplicationWindow* GNEApp);

    /// @brief destructor
    ~GNENetgenerateDialog();

    /// @brief open dialog
    long openDialog(const OptionsCont *netgenerateOptions);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press run button
    long onCmdRun(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    GNENetgenerateDialog();

private:
    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief parameter label
    FXLabel* myParameterLabel = nullptr;

    /// @brief separator
    FXSeparator* mySeparator = nullptr;

    /// @brief buttons frame
    FXHorizontalFrame* myButtonsFrame = nullptr;

    /// @brief Invalidated copy constructor.
    GNENetgenerateDialog(const GNENetgenerateDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNENetgenerateDialog& operator=(const GNENetgenerateDialog&) = delete;
};

