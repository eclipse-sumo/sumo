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
/// @file    GNEDemandElementDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
///
// A abstract class for editing demand elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>

#include "GNEDemandElementDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandElementDialog::GNEDemandElementDialog(GNEDemandElement* demandElement, bool updatingElement,
                                               int width, int height) :
    GNEDialog(demandElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
              TLF("Edit '%' data", demandElement->getID()), demandElement->getTagProperty()->getGUIIcon(),
              Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, width, height),    
    myEditedDemandElement(demandElement),
    myUpdatingElement(updatingElement),
    myChangesDescription(TLF("Change % values", demandElement->getTagStr())),
    myNumberOfChanges(0) {
}


GNEDemandElementDialog::~GNEDemandElementDialog() {}


GNEDemandElement*
GNEDemandElementDialog::getEditedDemandElement() const {
    return myEditedDemandElement;
}


GNEDemandElementDialog::GNEDemandElementDialog() {}


void
GNEDemandElementDialog::changeDemandElementDialogHeader(const std::string& newHeader) {
    // change GNEDialog title
    setTitle(newHeader.c_str());
}


void
GNEDemandElementDialog::initChanges() {
    // init commandGroup
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->begin(myEditedDemandElement, myChangesDescription);
    // save number of command group changes
    myNumberOfChanges = myEditedDemandElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
}


void
GNEDemandElementDialog::acceptChanges() {
    // commit changes or abort last command group depending of number of changes did
    if (myNumberOfChanges < myEditedDemandElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize()) {
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->end();
    } else {
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    }
    // refresh frame
    myEditedDemandElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->updateControls();
}


void
GNEDemandElementDialog::cancelChanges() {
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
}


void
GNEDemandElementDialog::resetChanges() {
    // abort last command group an start editing again
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->begin(myEditedDemandElement, myChangesDescription);
}


/****************************************************************************/
