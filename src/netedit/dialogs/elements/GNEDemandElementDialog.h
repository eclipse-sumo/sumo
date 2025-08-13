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
/// @file    GNEDemandElementDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
///
// A abstract class for editing additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDemandElementDialog : public GNEDialog {

public:
    /// @brief constructor
    GNEDemandElementDialog(GNEDemandElement* demandElement, const bool updatingElement, const int width, const int height) :
        GNEDialog(demandElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                  TLF("Edit '%' data", demandElement->getID()), demandElement->getTagProperty()->getGUIIcon(),
                  Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC, width, height),
        myEditedDemandElement(demandElement),
        myUpdatingElement(updatingElement),
        myChangesDescription(TLF("Change % values", demandElement->getTagStr())),
        myNumberOfChanges(0) {
    }

    /// @brief destructor
    ~GNEDemandElementDialog() {}

    /// @brief get edited DemandElement
    GNEDemandElement* getEditedDemandElement() const {
        return myEditedDemandElement;
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
    FOX_CONSTRUCTOR(GNEDemandElementDialog)

    /// @brief pointer to edited additional
    GNEDemandElement* myEditedDemandElement;

    /// @brief flag to indicate if additional are being created or modified (cannot be changed after open dialog)
    bool myUpdatingElement;

    /// @brief change additional dialog header
    void changeDemandElementDialogHeader(const std::string& newHeader) {
        // change GNEDialog title
        setTitle(newHeader.c_str());
    }

    /// @brief init a new group of changes that will be do it in dialog
    void initChanges() {
        // init commandGroup
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->begin(myEditedDemandElement, myChangesDescription);
        // save number of command group changes
        myNumberOfChanges = myEditedDemandElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
    }

    /// @brief Accept changes did in this dialog.
    void acceptChanges() {
        // commit changes or abort last command group depending of number of changes did
        if (myNumberOfChanges < myEditedDemandElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize()) {
            myEditedDemandElement->getNet()->getViewNet()->getUndoList()->end();
        } else {
            myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        }
        // refresh frame
        myEditedDemandElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->updateControls();
    }

    /// @brief Cancel changes did in this dialog.
    void cancelChanges() {
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    }

    /// @brief reset changes did in this dialog.
    void resetChanges() {
        // abort last command group an start editing again
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->begin(myEditedDemandElement, myChangesDescription);
    }

private:
    /// @brief description of changes did in this additional dialog
    std::string myChangesDescription;

    /// @brief number of GNEChanges_... in dialog
    int myNumberOfChanges;

    /// @brief Invalidated copy constructor
    GNEDemandElementDialog(const GNEDemandElementDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEDemandElementDialog& operator=(const GNEDemandElementDialog&) = delete;
};
