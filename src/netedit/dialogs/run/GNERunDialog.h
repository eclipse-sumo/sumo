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
/// @file    GNERunDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Abstract dialog for running tools
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <utils/foxtools/MFXSynchQue.h>
#include <utils/foxtools/MFXThreadEvent.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GUIEvent;
class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================

class GNERunDialog : public GNEDialog {
    /// @brief FOX-declaration
    FXDECLARE_ABSTRACT(GNERunDialog)

public:
    /// @brief Constructor
    GNERunDialog(GNEApplicationWindow* applicationWindow, const std::string& name, GUIIcon titleIcon);

    /// @brief destructor
    ~GNERunDialog();

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press abort button
    virtual long onCmdAbort(FXObject*, FXSelector, void*) = 0;

    /// @brief event after press rerun button
    virtual long onCmdRerun(FXObject*, FXSelector, void*) = 0;

    /// @brief event after press back button
    virtual long onCmdBack(FXObject*, FXSelector, void*) = 0;

    /// @brief event after press close button
    virtual long onCmdAccept(FXObject*, FXSelector, void*) = 0;

    /// @brief event after press cancel button
    virtual long onCmdCancel(FXObject*, FXSelector, void*) = 0;

    /// @brief event after press save button
    long onCmdSaveLog(FXObject*, FXSelector, void*);

    /// @brief called when the thread signals an event
    long onThreadEvent(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNERunDialog);

    /// @brief text
    FXText* myText = nullptr;

    /// @brief List of received events
    MFXSynchQue<GUIEvent*> myEvents;

    /// @brief io-event with the runner thread
    FXEX::MFXThreadEvent myThreadEvent;

    /// @brief flag to check if there is an error
    bool myError = false;

    /// @brief update dialog buttons
    virtual void updateDialogButtons() = 0;

private:
    /// @brief Invalidated copy constructor.
    GNERunDialog(const GNERunDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERunDialog& operator=(const GNERunDialog&) = delete;
};
