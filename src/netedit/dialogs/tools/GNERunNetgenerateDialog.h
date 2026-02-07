/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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

#include <netedit/dialogs/GNEDialog.h>
#include <utils/foxtools/MFXSynchQue.h>
#include <utils/foxtools/MFXThreadEvent.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNERunNetgenerate;
class GUIEvent;
class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================

class GNERunNetgenerateDialog : public GNEDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNERunNetgenerateDialog)

public:
    /// @brief Constructor
    GNERunNetgenerateDialog(GNEApplicationWindow* applicationWindow, const OptionsCont* netgenerateOptions);

    /// @brief destructor
    ~GNERunNetgenerateDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

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
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief called when the thread signals an event
    long onThreadEvent(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNERunNetgenerateDialog);

    /// @brief update toolDialog
    void updateDialog();

private:
    /// @brief thread for running tool
    GNERunNetgenerate* myRunNetgenerate = nullptr;

    /// @brief text
    FXText* myText = nullptr;

    /// @brief netgenerate options
    const OptionsCont* myNetgenerateOptions;

    /// @brief List of received events
    MFXSynchQue<GUIEvent*> myEvents;

    /// @brief io-event with the runner thread
    FXEX::MFXThreadEvent myThreadEvent;

    /// @brief flag to check if there is an error
    bool myError = false;

    /// @brief Invalidated copy constructor.
    GNERunNetgenerateDialog(const GNERunNetgenerateDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERunNetgenerateDialog& operator=(const GNERunNetgenerateDialog&) = delete;
};
