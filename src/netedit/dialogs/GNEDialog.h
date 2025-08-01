/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Custom FXDialogBox used in Netedit that supports internal tests
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/images/GUIIcons.h>
#include <utils/tests/InternalTestStep.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDialog : public FXDialogBox {
    FXDECLARE_ABSTRACT(GNEDialog)

public:
    /// @name basic dialog type
    enum class Buttons {
        OK,                     // ok button
        YES_NO,                 // yes/no buttons
        YES_NO_CANCEL,          // yes/no/cancel buttons
        ACCEPT,                 // accept button
        ACCEPT_CANCEL,          // accept/cancel buttons
        ACCEPT_CANCEL_RESET,    // accept/cancel/reset buttons
        KEEPNEW_KEEPOLD_CANCEL, // keep new/keep old/cancel buttons
        RUN_CANCEL_RESET,       // run/cancel/reset buttons (used in tools dialogs)
        RUN_ADVANCED_CANCEL,    // run/advanced/cancel buttons (used in tools dialogs)
        ABORT_RERUN_BACK_CLOSE  // abort/rerun/back buttons, and close in a lower line (used in tools dialogs)
    };

    /// @brief Open dialog type
    enum class OpenType {
        MODAL,      // Modal dialog
        NON_MODAL   // Non-modal dialog
    };

    /// @name Resize mode
    enum class ResizeMode {
        SHRINKABLE,     // dialog is shrinkable
        STRETCHABLE,    // dialog is stretchable
        RESIZABLE       // dialog is resizable (in both directions)
    };

    /// @brief list of possible results when closing the dialog
    enum class Result {
        ACCEPT, // dialog was closed accepting changes (used in ok, accept, yes buttons)
        CANCEL, // dialog was closed canceling changes (used in cancel, no buttons)
        ABORT,  // dialog was closed aborting changes (used in abort button)
    };

    /// @brief constructor for rigid dialogs
    GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name, 
              GUIIcon titleIcon, Buttons buttons, OpenType openType);

    /// @brief constructor for rigid dialogs with layout explicit
    GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name, 
              GUIIcon titleIcon, Buttons buttons, OpenType openType,
              const int width, const int height);

    /// @brief constructor for resizable dialogs
    GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name, 
              GUIIcon titleIcon, Buttons buttons, OpenType openType,
              ResizeMode resizeMode);

    /// @brief constructor for resizable dialogs with layout explicit
    GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name, 
              GUIIcon titleIcon, Buttons buttons, OpenType openType,
              ResizeMode resizeMode, const int width, const int height);

    /// @brief get result to indicate if this dialog was closed accepting or rejecting changes
    Result getResult() const;

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when accept or yes button is pressed (can be reimplemented in children)
    virtual long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief called when cancel or nobutton is pressed (can be reimplemented in children)
    virtual long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief called when cancel or nobutton is pressed (can be reimplemented in children)
    virtual long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief called when abort is called (either closing dialog or pressing abort button) 
    long onCmdAbort(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEDialog)

    /// @brief pointer to the main window
    GNEApplicationWindow* myApplicationWindow = nullptr;

    /// @brief content frame
    FXVerticalFrame* myContentFrame = nullptr;

    /// @brief butto used to focus the dialog
    FXButton* myFocusButon = nullptr;

    /// @brief result to indicate if this dialog was closed accepting or rejecting changes
    Result myResult = Result::CANCEL;

    /// @brief flag to indicate if this dialog is being tested using internal test
    bool myTesting = false;

    /// @brief open dialog in modal mode
    void openModalDialog();

    /// @brief close dialog accepting the changes
    long closeDialogAccepting();

    /// @brief close dialog declining the changes
    long closeDialogCanceling();

private:
    /// @brief open type
    OpenType myOpenType;

    /// @brief build dialog
    void buildDialog(GUIIcon titleIcon, Buttons buttons);

    /// @brief Invalidated copy constructor.
    GNEDialog(const GNEDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEDialog& operator=(const GNEDialog& src) = delete;
};
