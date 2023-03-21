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
/// @file    GNEPythonToolDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog for tools
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/options/OptionsCont.h>

#include "GNEPythonToolDialogElements.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;
class GNEPythonTool;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEPythonToolDialog
 * @brief Dialog for python tool dialog
 */
class GNEPythonToolDialog : protected FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEPythonToolDialog)

    /// @brief declare friend class
    friend class GNEPythonToolDialogElements;

public:
    /// @brief Constructor
    GNEPythonToolDialog(GNEApplicationWindow* GNEApp);

    /// @brief destructor
    ~GNEPythonToolDialog();

    /// @brief open dialog
    long openDialog(GNEPythonTool* tool);

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
    GNEPythonToolDialog();

    /// @brief build arguments
    virtual void buildArguments();

    /// @brief adjust parameter column
    void adjustParameterColumn();

    /// @brief list of arguments
    std::vector<GNEPythonToolDialogElements::Argument*> myArguments;

private:
    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief vertical frame for contents
    FXVerticalFrame* myContentFrame = nullptr;

    /// @brief tool
    GNEPythonTool* myPythonTool = nullptr;

    /// @brief parameter label
    FXLabel* myParameterLabel = nullptr;

    /// @brief separator
    FXSeparator* mySeparator = nullptr;

    /// @brief buttons frame
    FXHorizontalFrame* myButtonsFrame = nullptr;

    /// @brief Invalidated copy constructor.
    GNEPythonToolDialog(const GNEPythonToolDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPythonToolDialog& operator=(const GNEPythonToolDialog&) = delete;
};

