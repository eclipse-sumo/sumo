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
/// @file    GNEAdditionalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// A abstract class for editing additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalDialog : public GNEDialog {

public:
    /// @brief constructor
    GNEAdditionalDialog(GNEAdditional* additional, const bool updatingElement,
                        const int width, const int height) :
        GNEDialog(additional->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                  TLF("Edit '%' data", additional->getID()), additional->getTagProperty()->getGUIIcon(),
                  Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC, width, height),
        myEditedAdditional(additional),
        myUpdatingElement(updatingElement),
        myChangesDescription(TLF("change % values", additional->getTagStr())),
        myNumberOfChanges(0) {
    }

    /// @brief destructor
    ~GNEAdditionalDialog() {}

    /// @brief get edited Additional
    GNEAdditional* getEditedAdditional() const {
        return myEditedAdditional;
    }

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    virtual long onCmdAccept(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdCancel(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdReset(FXObject*, FXSelector, void*) = 0;

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEAdditionalDialog)

    /// @brief pointer to edited additional
    GNEAdditional* myEditedAdditional = nullptr;

    /// @brief flag to indicate if additional are being created or modified (cannot be changed after open dialog)
    bool myUpdatingElement = false;

    /// @brief change additional dialog header
    void changeAdditionalDialogHeader(const std::string& newHeader) {
        // change GNEDialog title
        setTitle(newHeader.c_str());
    }

    /// @brief init a new group of changes that will be do it in dialog
    void initChanges() {
        // init commandGroup
        myEditedAdditional->getNet()->getViewNet()->getUndoList()->begin(myEditedAdditional, myChangesDescription);
        // save number of command group changes
        myNumberOfChanges = myEditedAdditional->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
    }

    /// @brief Accept changes did in this dialog.
    void acceptChanges() {
        // commit changes or abort last command group depending of number of changes did
        if (myNumberOfChanges < myEditedAdditional->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize()) {
            myEditedAdditional->getNet()->getViewNet()->getUndoList()->end();
        } else {
            myEditedAdditional->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        }
    }

    /// @brief Cancel changes did in this dialog.
    void cancelChanges() {
        myEditedAdditional->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    }

    /// @brief reset changes did in this dialog.
    void resetChanges() {
        // abort last command group an start editing again
        myEditedAdditional->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        myEditedAdditional->getNet()->getViewNet()->getUndoList()->begin(myEditedAdditional, myChangesDescription);
    }

private:
    /// @brief description of changes did in this additional dialog
    std::string myChangesDescription;

    /// @brief number of GNEChanges_... in dialog
    int myNumberOfChanges;

    /// @brief Invalidated copy constructor
    GNEAdditionalDialog(const GNEAdditionalDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEAdditionalDialog& operator=(const GNEAdditionalDialog&) = delete;
};
