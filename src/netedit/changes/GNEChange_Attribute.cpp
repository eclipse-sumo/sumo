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
/// @file    GNEChange_Attribute.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which something is changed (for undo/redo)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/data/GNEDataSet.h>

#include "GNEChange_Attribute.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Attribute, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier* ac, SumoXMLAttr key, const std::string& value) :
    GNEChange(ac->getTagProperty().getSupermode(), true, false),
    myAC(ac),
    myKey(key),
    myForceChange(false),
    myOrigValue(ac->getAttribute(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier* ac, SumoXMLAttr key, const std::string& value, const std::string& origValue) :
    GNEChange(ac->getTagProperty().getSupermode(), true, false),
    myAC(ac),
    myKey(key),
    myForceChange(false),
    myOrigValue(origValue),
    myNewValue(value) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::~GNEChange_Attribute() {
    // decrease reference
    myAC->decRef("GNEChange_Attribute " + toString(myKey));
    // remove if is unreferenced
    if (myAC->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAC->getTagStr() + " in GNEChange_Attribute");
        // delete AC
        delete myAC;
    }
}


void
GNEChange_Attribute::undo() {
    // show extra information for tests
    WRITE_DEBUG("Restoring previous attribute"/* + toString(myKey)*/);
    // set original value
    myAC->setAttribute(myKey, myOrigValue);
    // certain attributes needs extra operations
    if (myKey != GNE_ATTR_SELECTED) {
        // check if updated attribute requires a update geometry
        if (myAC->getTagProperty().hasAttribute(myKey) && myAC->getTagProperty().getAttributeProperties(myKey).requireUpdateGeometry()) {
            myAC->updateGeometry();
        }
        // if is a dataelement, update attribute colors
        if (myAC->getTagProperty().isGenericData()) {
            myAC->getNet()->getAttributeCarriers()->retrieveDataSet(myAC->getAttribute(GNE_ATTR_DATASET))->updateAttributeColors();
        } else if (myAC->getTagProperty().getTag() == SUMO_TAG_DATASET) {
            myAC->getNet()->getAttributeCarriers()->retrieveDataSet(myAC->getAttribute(SUMO_ATTR_ID))->updateAttributeColors();
        }
        // check if networkElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
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
}


void
GNEChange_Attribute::redo() {
    // show extra information for tests
    WRITE_DEBUG("Setting new attribute"/* + toString(myKey)*/);
    // set new value
    myAC->setAttribute(myKey, myNewValue);
    // certain attributes needs extra operations
    if (myKey != GNE_ATTR_SELECTED) {
        // check if updated attribute requires a update geometry
        if (myAC->getTagProperty().hasAttribute(myKey) && myAC->getTagProperty().getAttributeProperties(myKey).requireUpdateGeometry()) {
            myAC->updateGeometry();
        }
        // if is a dataelement, update attribute colors
        if (myAC->getTagProperty().isGenericData()) {
            myAC->getNet()->getAttributeCarriers()->retrieveDataSet(myAC->getAttribute(GNE_ATTR_DATASET))->updateAttributeColors();
        } else if (myAC->getTagProperty().getTag() == SUMO_TAG_DATASET) {
            myAC->getNet()->getAttributeCarriers()->retrieveDataSet(myAC->getAttribute(SUMO_ATTR_ID))->updateAttributeColors();
        }
        // check if networkElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
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
}


void
GNEChange_Attribute::forceChange() {
    myForceChange = true;
}


bool
GNEChange_Attribute::trueChange() {
    // check if we're editing the value of an attribute or changing a disjoint attribute
    if (myForceChange) {
        return true;
    } else {
        return (myOrigValue != myNewValue);
    }
}


std::string
GNEChange_Attribute::undoName() const {
    return (TL("Undo change ") + myAC->getTagStr() + " attribute");
}


std::string
GNEChange_Attribute::redoName() const {
    return (TL("Redo change ") + myAC->getTagStr() + " attribute");
}

/****************************************************************************/
