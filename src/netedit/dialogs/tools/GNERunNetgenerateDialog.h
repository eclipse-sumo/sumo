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
/// @file    GNERunNetgenerateDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for running tools
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <utils/options/OptionsCont.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;
class GNERunNetgenerate;
class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNERunNetgenerateDialog
 * @brief Abstract dialog for tools
 */
class GNERunNetgenerateDialog : protected FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNERunNetgenerateDialog)

public:
    /// @brief Constructor
    GNERunNetgenerateDialog(GNEApplicationWindow* GNEApp);

    /// @brief destructor
    ~GNERunNetgenerateDialog();

    /// @brief get to GNEApplicationWindow
    GNEApplicationWindow* getGNEApp() const;

    /// @brief run tool (this open windows)
    void run(const OptionsCont *netgenerateOptions);

    /// @brief add info (green) text to output console
    void appendInfoMessage(const std::string text);

    /// @brief add error (green) text to output console
    void appendErrorMessage(const std::string text);

    /// @brief add text buffer to output console
    void appendBuffer(const char *buffer);

    /// @brief update toolDialog
    void updateDialog();

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press save button
    long onCmdSaveLog(FXObject*, FXSelector, void*);

    /// @brief event after press abort button
    long onCmdAbort(FXObject*, FXSelector, void*);

    /// @brief event after press rerun button
    long onCmdRerun(FXObject*, FXSelector, void*);

    /// @brief event after press back button
    long onCmdBack(FXObject*, FXSelector, void*);

    /// @brief event after press close button
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    GNERunNetgenerateDialog();

    /// @brief 

private:
    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief thread for running tool
    GNERunNetgenerate* myRunNetgenerate = nullptr;

    /// @brief text
    FXText* myText = nullptr;

    /// @brief abort button
    FXButton* myAbortButton = nullptr;

    /// @brief rerun button
    FXButton* myRerunButton = nullptr;

    /// @brief back button
    FXButton* myBackButton = nullptr;

    /// @brief close button
    FXButton* myCloseButton = nullptr;

    /// @brief netgenerate options
    const OptionsCont *myNetgenerateOptions;

    /// @brief Invalidated copy constructor.
    GNERunNetgenerateDialog(const GNERunNetgenerateDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERunNetgenerateDialog& operator=(const GNERunNetgenerateDialog&) = delete;
};

