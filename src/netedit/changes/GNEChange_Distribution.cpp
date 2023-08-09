/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNEChange_Distribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// A change in which the distribution attribute of some object is modified
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>

#include "GNEChange_Distribution.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Distribution, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Distribution::GNEChange_Distribution(GNEAttributeCarrier* ac, const SumoXMLAttr key, const bool value) :
    GNEChange(ac->getTagProperty().getSupermode(), true, false),
    myAC(ac),
    myKey(key),
    myOrigValue(ac->isAttributeEnabled(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_Distribution " + myAC->getTagProperty().getTagStr());
}


GNEChange_Distribution::GNEChange_Distribution(GNEAttributeCarrier* ac, const SumoXMLAttr key, const bool value, const int /* previousParameters */) :
    GNEChange(ac->getTagProperty().getSupermode(), true, false),
    myAC(ac),
    myKey(key),
    myOrigValue(ac->isAttributeEnabled(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_Distribution " + myAC->getTagProperty().getTagStr());
}


GNEChange_Distribution::~GNEChange_Distribution() {
    // decrease reference
    myAC->decRef("GNEChange_Distribution " + myAC->getTagProperty().getTagStr());
    // remove if is unreferenced
    if (myAC->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAC->getTagStr() + " '" + myAC->getID() + "' in GNEChange_Distribution");
        // delete AC
        delete myAC;
    }
}


void
GNEChange_Distribution::undo() {
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
GNEChange_Distribution::redo() {
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
GNEChange_Distribution::undoName() const {
    if (myNewValue) {
        return (TLF("Undo enable % attribute in '%'", myAC->getTagStr(), myAC->getID()));
    } else {
        return (TLF("Undo enable % attribute in '%'", myAC->getTagStr(), myAC->getID()));
    }
}


std::string
GNEChange_Distribution::redoName() const {
    if (myNewValue) {
        return (TLF("Redo enable % attribute in '%'", myAC->getTagStr(), myAC->getID()));
    } else {
        return (TLF("Redo enable % attribute in '%'", myAC->getTagStr(), myAC->getID()));
    }
}

/****************************************************************************/
