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
/// @file    GNEAdditionalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// A abstract class for editing additional elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEAdditionalDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalDialog::GNEAdditionalDialog(GNEAdditional* additional, const bool updatingElement,
                                         const int width, const int height) :
    GNEDialog(additional->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
              TLF("Edit '%' data", additional->getID()), additional->getTagProperty()->getGUIIcon(),
              Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, width, height),
    myEditedAdditional(additional),
    myUpdatingElement(updatingElement),
    myChangesDescription(TLF("change % values", additional->getTagStr())),
    myNumberOfChanges(0) {
}


GNEAdditionalDialog::~GNEAdditionalDialog() {
}


GNEAdditional*
GNEAdditionalDialog::getEditedAdditional() const {
    return myEditedAdditional;
}


GNEAdditionalDialog::GNEAdditionalDialog() {}


void
GNEAdditionalDialog::changeAdditionalDialogHeader(const std::string& newHeader) {
    // change GNEDialog title
    setTitle(newHeader.c_str());
}


void
GNEAdditionalDialog::initChanges() {
    // init commandGroup
    myEditedAdditional->getNet()->getViewNet()->getUndoList()->begin(myEditedAdditional, myChangesDescription);
    // save number of command group changes
    myNumberOfChanges = myEditedAdditional->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
}


void
GNEAdditionalDialog::acceptChanges() {
    // commit changes or abort last command group depending of number of changes did
    if (myNumberOfChanges < myEditedAdditional->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize()) {
        myEditedAdditional->getNet()->getViewNet()->getUndoList()->end();
    } else {
        myEditedAdditional->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    }
}


void
GNEAdditionalDialog::cancelChanges() {
    myEditedAdditional->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
}


void
GNEAdditionalDialog::resetChanges() {
    // abort last command group an start editing again
    myEditedAdditional->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    myEditedAdditional->getNet()->getViewNet()->getUndoList()->begin(myEditedAdditional, myChangesDescription);
}

/****************************************************************************/
