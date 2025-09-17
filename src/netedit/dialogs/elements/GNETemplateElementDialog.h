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
/// @file    GNETemplateElementDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// A template based on GNEDialog used for editing elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <netedit/GNETagPropertiesDatabase.h>

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNETemplateElementDialog : public GNEDialog {

public:
    /// @brief constructor
    GNETemplateElementDialog(T* element, DialogType type) :
        GNEDialog(element->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                  TLF("Edit % '%'", element->getTagStr(), element->getID()).c_str(),
                  element->getTagProperty()->getGUIIcon(), type, Buttons::ACCEPT_CANCEL_RESET,
                  OpenType::MODAL, ResizeMode::STATIC),
        myElement(element),
        myChangesDescription(TLF("change % values", element->getTagStr())) {
        // init commandGroup
        myElement->getNet()->getViewNet()->getUndoList()->begin(myElement, myChangesDescription);
    }

    /// @brief destructor
    ~GNETemplateElementDialog() {}

    /// @brief get edited element
    T* getElement() const {
        return myElement;
    }

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    virtual long onCmdAccept(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdReset(FXObject*, FXSelector, void*) = 0;

    /// @brief called when cancel or no button is pressed
    long onCmdCancel(FXObject*, FXSelector, void*) {
        myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        return closeDialogCanceling();
    }

    /// @brief called when abort is called either closing dialog or pressing abort button
    long onCmdAbort(FXObject*, FXSelector, void*) {
        myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        return closeDialogAborting();
    }

    /// @}

protected:
    /// @brief default constructor
    GNETemplateElementDialog() :
        GNEDialog() {}

    /// @brief pointer to edited element
    T* myElement = nullptr;

    /// @brief close dialog commiting changes
    long acceptElementDialog() {
        myElement->getNet()->getViewNet()->getUndoList()->end();
        return closeDialogAccepting();
    }

    /// @brief reset changes did in this dialog.
    void resetChanges() {
        // abort last command group an start editing again
        myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        myElement->getNet()->getViewNet()->getUndoList()->begin(myElement, myChangesDescription);
    }

private:
    /// @brief description of changes did in this element dialog
    std::string myChangesDescription;

    /// @brief Invalidated copy constructor
    GNETemplateElementDialog(const GNETemplateElementDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNETemplateElementDialog& operator=(const GNETemplateElementDialog&) = delete;
};
