/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_EnableAttribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// A network change in which something is changed (for undo/redo)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>

#include "GNEChange_EnableAttribute.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_EnableAttribute, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_EnableAttribute::GNEChange_EnableAttribute(GNEAttributeCarrier* ac, const SumoXMLAttr key, const bool value) :
    GNEChange(ac->getTagProperty().getSupermode(), true, false),
    myAC(ac),
    myKey(key),
    myOrigValue(ac->isAttributeEnabled(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_EnableAttribute " + myAC->getTagProperty().getTagStr());
}


GNEChange_EnableAttribute::GNEChange_EnableAttribute(GNEAttributeCarrier* ac, const SumoXMLAttr key, const bool value, const int /* previousParameters */) :
    GNEChange(ac->getTagProperty().getSupermode(), true, false),
    myAC(ac),
    myKey(key),
    myOrigValue(ac->isAttributeEnabled(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_EnableAttribute " + myAC->getTagProperty().getTagStr());
}


GNEChange_EnableAttribute::~GNEChange_EnableAttribute() {
    // decrease reference
    myAC->decRef("GNEChange_EnableAttribute " + myAC->getTagProperty().getTagStr());
    // remove if is unreferenced
    if (myAC->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAC->getTagStr() + " '" + myAC->getID() + "' in GNEChange_EnableAttribute");
        // delete AC
        delete myAC;
    }
}


void
GNEChange_EnableAttribute::undo() {
    // show extra information for tests
    WRITE_DEBUG("Setting previous attribute into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
    // set original value
    myAC->toggleAttribute(myKey, myOrigValue);
    // check if networkElements, additional or shapes has to be saved
    if (myAC->getTagProperty().isNetworkElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveNetwork();
    } else if (myAC->getTagProperty().isAdditionalElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveAdditionals();
    } else if (myAC->getTagProperty().isDemandElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDemandElements();
    } else if (myAC->getTagProperty().isDataElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDataElements();
    } else if (myAC->getTagProperty().isMeanData()) {
        myAC->getNet()->getSavingStatus()->requireSaveMeanDatas();
    }
}


void
GNEChange_EnableAttribute::redo() {
    // show extra information for tests
    WRITE_DEBUG("Setting new attribute into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
    // set new attributes
    myAC->toggleAttribute(myKey, myNewValue);
    // check if networkElements, additional or shapes has to be saved
    if (myAC->getTagProperty().isNetworkElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveNetwork();
    } else if (myAC->getTagProperty().isAdditionalElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveAdditionals();
    } else if (myAC->getTagProperty().isDemandElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDemandElements();
    } else if (myAC->getTagProperty().isDataElement()) {
        myAC->getNet()->getSavingStatus()->requireSaveDataElements();
    } else if (myAC->getTagProperty().isMeanData()) {
        myAC->getNet()->getSavingStatus()->requireSaveMeanDatas();
    }
}


std::string
GNEChange_EnableAttribute::undoName() const {
    if (myNewValue) {
        return (TL("Undo enable ") + myAC->getTagStr() + TL(" attribute in '") + myAC->getID() + "'");
    } else {
        return (TL("Undo enable ") + myAC->getTagStr() + TL(" attribute in '") + myAC->getID() + "'");
    }
}


std::string
GNEChange_EnableAttribute::redoName() const {
    if (myNewValue) {
        return (TL("Redo enable ") + myAC->getTagStr() + TL(" attribute in '") + myAC->getID() + "'");
    } else {
        return (TL("Redo enable ") + myAC->getTagStr() + TL(" attribute in '") + myAC->getID() + "'");
    }
}

/****************************************************************************/
