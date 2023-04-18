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
#include <utils/foxtools/MFXCheckableButton.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

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
    void openDialog();

    /// @name FOX-callbacks
    /// @{

    /// @brief open output file
    long onCmdOpenOutputFile(FXObject*, FXSelector, void*);

    /// @brief set output file
    long onCmdSetOutput(FXObject*, FXSelector, void*);

    /// @brief set grid mode
    long onCmdSetGrid(FXObject*, FXSelector, void*);

    /// @brief set spider
    long onCmdSetSpider(FXObject*, FXSelector, void*);

    /// @brief set random grid
    long onCmdSetRandomGrid(FXObject*, FXSelector, void*);

    /// @brief set random
    long onCmdSetRandom(FXObject*, FXSelector, void*);

    /// @brief event after press run button
    long onCmdRun(FXObject*, FXSelector, void*);

    /// @brief event after press advanced button
    long onCmdAdvanced(FXObject*, FXSelector, void*);

    /// @brief check if settings are enabled
    long onUpdSettingsConfigured(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    GNENetgenerateDialog();

private:
    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

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

    /// @brief run button
    FXButton* myRunButton = nullptr;

    /// @brief advanced button
    FXButton* myAdvancedButton = nullptr;

    /// @brief Invalidated copy constructor.
    GNENetgenerateDialog(const GNENetgenerateDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNENetgenerateDialog& operator=(const GNENetgenerateDialog&) = delete;
};

