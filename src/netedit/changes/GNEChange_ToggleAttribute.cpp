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
/// @file    GNEChange_ToggleAttribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// A network change in which something is changed (for undo/redo)
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEChange_ToggleAttribute.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNEChange_ToggleAttribute, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_ToggleAttribute::GNEChange_ToggleAttribute(GNEAttributeCarrier* ac, const SumoXMLAttr key, const bool value) :
    GNEChange(ac->getTagProperty()->getSupermode(), true, false),
    myAC(ac),
    myKey(key),
    myOrigValue(ac->isAttributeEnabled(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_ToggleAttribute " + myAC->getTagProperty()->getTagStr());
}


GNEChange_ToggleAttribute::~GNEChange_ToggleAttribute() {
    // only continue we have undo-redo mode enabled
    if (myAC->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        // decrease reference
        myAC->decRef("GNEChange_ToggleAttribute " + myAC->getTagProperty()->getTagStr());
        // remove if is unreferenced
        if (myAC->unreferenced()) {
            // delete AC
            delete myAC;
        }
    }
}


void
GNEChange_ToggleAttribute::undo() {
    // set original value
    myAC->toggleAttribute(myKey, myOrigValue);
    // check if networkElements, additional or shapes has to be saved
    if (myAC->getTagProperty()->isNetworkElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveNetwork();
    } else if (myAC->getTagProperty()->isAdditionalElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveAdditionals();
    } else if (myAC->getTagProperty()->isDemandElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDemandElements();
    } else if (myAC->getTagProperty()->isDataElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDataElements();
    } else if (myAC->getTagProperty()->isMeanData()) {
        myAC->getNet()->getSavingStatus()->requireSaveMeanDatas();
    }
}


void
GNEChange_ToggleAttribute::redo() {
    // set new attributes
    myAC->toggleAttribute(myKey, myNewValue);
    // check if networkElements, additional or shapes has to be saved
    if (myAC->getTagProperty()->isNetworkElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveNetwork();
    } else if (myAC->getTagProperty()->isAdditionalElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveAdditionals();
    } else if (myAC->getTagProperty()->isDemandElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDemandElements();
    } else if (myAC->getTagProperty()->isDataElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDataElements();
    } else if (myAC->getTagProperty()->isMeanData()) {
        myAC->getNet()->getSavingStatus()->requireSaveMeanDatas();
    }
}


std::string
GNEChange_ToggleAttribute::undoName() const {
    return (TLF("Undo toggle % attribute in '%'", myAC->getTagStr(), myAC->getID()));
}


std::string
GNEChange_ToggleAttribute::redoName() const {
    return (TLF("Redo toggle % attribute in '%'", myAC->getTagStr(), myAC->getID()));
}

/****************************************************************************/
