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
/// @file    GNENetgenerateDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for netgenerate
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declarations
// ===========================================================================

class MFXCheckableButton;

// ===========================================================================
// class definitions
// ===========================================================================

class GNENetgenerateDialog : protected GNEDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNENetgenerateDialog)

    /// @brief declare friend class
    friend class GNENetgenerateDialogElements;

public:
    /// @brief Constructor
    GNENetgenerateDialog(GNEApplicationWindow* applicationWindow);

    /// @brief destructor
    ~GNENetgenerateDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief open output file
    long onCmdOpenOutputFile(FXObject*, FXSelector, void*);

    /// @brief set output file
    long onCmdSetOutput(FXObject*, FXSelector, void*);

    /// @brief set grid mode
    long onCmdSetGridNetwork(FXObject*, FXSelector, void*);

    /// @brief set spider
    long onCmdSetSpiderNetwork(FXObject*, FXSelector, void*);

    /// @brief set random grid
    long onCmdSetRandomNetworkGridNetwork(FXObject*, FXSelector, void*);

    /// @brief set random
    long onCmdSetRandomNetwork(FXObject*, FXSelector, void*);

    /// @brief event after press run button
    long onCmdRun(FXObject*, FXSelector, void*);

    /// @brief event after press advanced button
    long onCmdAdvanced(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNENetgenerateDialog);

private:
    /// @brief flag to indicate if networkType was selected
    bool mySelectedNetworktypeFlag = false;

    /// @brief grid network
    FXLabel* myGridNetworkLabel = nullptr;

    /// @brief grid network
    FXLabel* mySpiderNetworkLabel = nullptr;

    /// @brief random grid network
    FXLabel* myRandomGridNetworkLabel = nullptr;

    /// @brief grid network
    FXLabel* myRandomNetworkLabel = nullptr;

    /// @brief grid network
    MFXCheckableButton* myGridNetworkButton = nullptr;

    /// @brief grid network
    MFXCheckableButton* mySpiderNetworkButton = nullptr;

    /// @brief random grid network
    MFXCheckableButton* myRandomGridNetworkButton = nullptr;

    /// @brief grid network
    MFXCheckableButton* myRandomNetworkButton = nullptr;

    /// @brief output text field
    FXTextField* myOutputTextField = nullptr;

    /// @brief update run buttons
    void updateRunButtons();

    /// @brief Invalidated copy constructor.
    GNENetgenerateDialog(const GNENetgenerateDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNENetgenerateDialog& operator=(const GNENetgenerateDialog&) = delete;
};
