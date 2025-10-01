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

#include "GNEDialogEnum.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDialog : protected FXDialogBox {
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
        SAVE_DONTSAVE_CANCEL,   // save/don't save/cancel buttons
        RUN_CANCEL_RESET,       // run/cancel/reset buttons (used in tools dialogs)
        RUN_ADVANCED_CANCEL,    // run/advanced/cancel buttons (used in tools dialogs)
        RERUN_BACK_CLOSE,       // rerun-abort/back buttons (used in run dialogs)
        OK_COPY_REPORT          // ok, copy trace and report to github
    };

    /// @brief Open dialog type
    enum class OpenType {
        MODAL,      // Modal dialog
        NON_MODAL   // Non-modal dialog
    };

    /// @name Resize mode
    enum class ResizeMode {
        STATIC,     // dialog is static (size cannot be edited)
        RESIZABLE   // dialog is resizable (in both directions)
    };

    /// @brief list of possible results when closing the dialog
    enum class Result {
        ACCEPT,     // dialog was closed accepting changes (used in ok, accept, yes buttons)
        CANCEL,     // dialog was closed canceling changes (used in cancel, no buttons)
        ABORT,      // dialog was closed aborting changes (used in abort button)
        ACCEPT_ALL, // dialog was closed acepting all changes (used in ok, accept, yes buttons)
        CANCEL_ALL, // dialog was closed canceling all changes (used in cancel, no buttons)
    };

    /// @brief basic constructor
    GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
              GUIIcon titleIcon, DialogType type, Buttons buttons, OpenType openType,
              ResizeMode resizeMode);

    /// @brief constructor with layout explicit
    GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
              GUIIcon titleIcon, DialogType type, Buttons buttons, OpenType openType,
              ResizeMode resizeMode, const int width, const int height);

    /// @brief get result to indicate if this dialog was closed accepting or rejecting changes
    Result getResult() const;

    /// @brief get pointer to the application window
    GNEApplicationWindow* getApplicationWindow() const;

    /// @brief get content frame
    FXVerticalFrame* getContentFrame() const;

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when accept or yes button is pressed (can be reimplemented in children)
    virtual long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief called when cancel or no button is pressed (can be reimplemented in children)
    virtual long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief called when abort is called either closing dialog or pressing abort button (can be reimplemented in children)
    virtual long onCmdAbort(FXObject*, FXSelector, void*);

    /// @brief called when reset button is pressed (must be reimplemented in children depending of Buttons)
    virtual long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief called when run button is pressed (must be reimplemented in children depending of Buttons)
    virtual long onCmdRun(FXObject*, FXSelector, void*);

    /// @brief called when back button is pressed (must be reimplemented in children depending of Buttons)
    virtual long onCmdBack(FXObject*, FXSelector, void*);

    /// @brief called when advanced button is pressed (must be reimplemented in children depending of Buttons)
    virtual long onCmdAdvanced(FXObject*, FXSelector, void*);

    /// @brief called when copy button is pressed (must be reimplemented in children depending of Buttons)
    virtual long onCmdCopy(FXObject*, FXSelector, void*);

    /// @brief called when report button is pressed (must be reimplemented in children depending of Buttons)
    virtual long onCmdReport(FXObject*, FXSelector, void*);

    /// @brief called when user presses a key on the dialog
    long onKeyPress(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief called when user releases a key on the dialog
    long onKeyRelease(FXObject* obj, FXSelector sel, void* ptr);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEDialog)

    /// @brief pointer to the main window
    GNEApplicationWindow* myApplicationWindow = nullptr;

    /// @brief content frame
    FXVerticalFrame* myContentFrame = nullptr;

    /// @brief accept button
    FXButton* myAcceptButton = nullptr;

    /// @brief cancel button
    FXButton* myCancelButton = nullptr;

    /// @brief abort button
    FXButton* myAbortButton = nullptr;

    /// @brief reset button
    FXButton* myResetButton = nullptr;

    /// @brief run button
    FXButton* myRunButton = nullptr;

    /// @brief back button
    FXButton* myBackButton = nullptr;

    /// @brief advanced button
    FXButton* myAdvancedButton = nullptr;

    /// @brief copy button
    FXButton* myCopyButton = nullptr;

    /// @brief report button
    FXButton* myReportButton = nullptr;

    /// @brief dialog type
    DialogType myType = DialogType::DEFAULT;

    /// @brief result to indicate if this dialog was closed accepting or rejecting changes
    Result myResult = Result::CANCEL;

    /// @brief open dialog
    void openDialog(FXWindow* focusableElement = nullptr);

    /// @brief close dialog accepting the changes
    long closeDialogAccepting();

    /// @brief close dialog declining the changes
    long closeDialogCanceling();

    /// @brief close dialog aborting the changes
    long closeDialogAborting();

    /// @brief update title
    void updateTitle(const std::string& newTitle);

    /// @brief update icon
    void updateIcon(GUIIcon newIcon);

private:
    /// @brief focus button, used for focusing the default button when dialog is opened
    FXButton* myFocusButton = nullptr;

    /// @brief open type
    OpenType myOpenType;

    /// @brief flag to indicate if this dialog is being tested using internal test
    bool myTesting = false;

    /// @brief build dialog
    void buildDialog(GUIIcon titleIcon, Buttons buttons);

    /// @brief Invalidated copy constructor.
    GNEDialog(const GNEDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEDialog& operator=(const GNEDialog& src) = delete;
};
