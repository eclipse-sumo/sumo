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
/// @file    GNEElementDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// A abstract dialog class for editing elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <netedit/elements/GNEHierarchicalStructureChildren.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#include "GNEElementTable.h"

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNEElementDialog : public GNEDialog {

public:
    /// @brief constructor
    GNEElementDialog(T* element, const bool updatingElement) :
        GNEDialog(element->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                  TLF("Edit '%' data", element->getID()), element->getTagProperty()->getGUIIcon(),
                  Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC),
        myElement(element),
        myUpdatingElement(updatingElement),
        myChangesDescription(TLF("change % values", element->getTagStr())),
        myNumberOfChanges(0) {
        // change dialog title depending if we are updating or creating an element
        if (updatingElement) {
            setTitle(TLF("Create %", element->getTagStr()).c_str());
        } else {
            setTitle(TLF("edit % '%'", element->getTagStr(), element->getID()).c_str());
        }
        // init commandGroup
        myElement->getNet()->getViewNet()->getUndoList()->begin(myElement, myChangesDescription);
        // save number of command group changes
        myNumberOfChanges = myElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
    }

    /// @brief destructor
    ~GNEElementDialog() {}

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
    virtual long onCmdCancel(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdReset(FXObject*, FXSelector, void*) = 0;

    /// @}

protected:
    /// @brief default constructor
    GNEElementDialog() :
        GNEDialog() {}

    /// @brief pointer to edited element
    T* myElement = nullptr;

    /// @brief flag to indicate if element are being created or modified (cannot be changed after open dialog)
    bool myUpdatingElement = false;

    /// @brief Accept changes did in this dialog.
    void acceptChanges() {
        // commit changes or abort last command group depending of number of changes did
        if (myNumberOfChanges < myElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize()) {
            myElement->getNet()->getViewNet()->getUndoList()->end();
        } else {
            myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        }
    }

    /// @brief Cancel changes did in this dialog.
    void cancelChanges() {
        myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
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

    /// @brief number of GNEChanges_... in dialog
    int myNumberOfChanges;

    /// @brief Invalidated copy constructor
    GNEElementDialog(const GNEElementDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEElementDialog& operator=(const GNEElementDialog&) = delete;
};
