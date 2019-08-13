/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Attribute.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which something is changed (for undo/redo)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNENetElement.h>

#include "GNEChange_Attribute.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Attribute, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier* ac, GNENet* net,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(net, true),
    myAC(ac),
    myKey(key),
    myForceChange(false),
    myOrigValue(customOrigValue ? origValue : ac->getAttribute(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier* ac, GNENet* net,
        bool forceChange, SumoXMLAttr key, const std::string& value) :
    GNEChange(net, true),
    myAC(ac),
    myKey(key),
    myForceChange(forceChange),
    myOrigValue(ac->getAttribute(key)),
    myNewValue(value) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::~GNEChange_Attribute() {
    // decrease reference
    myAC->decRef("GNEChange_Attribute " + toString(myKey));
    // remove if is unreferenced
    if (myAC->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAC->getTagStr() + " '" + myAC->getID() + "' in GNEChange_Attribute");
        // Check if attribute carrier is a shape
        if (myAC->getTagProperty().isShape()) {
            // remove shape using specific functions
            if (myAC->getTagProperty().getTag() == SUMO_TAG_POLY) {
                myNet->removePolygon(myAC->getID());
            } else if ((myAC->getTagProperty().getTag() == SUMO_TAG_POI) || (myAC->getTagProperty().getTag() == SUMO_TAG_POILANE)) {
                myNet->removePOI(myAC->getID());
            }
        } else {
            delete myAC;
        }
    }
}


void
GNEChange_Attribute::undo() {
    // show extra information for tests
    WRITE_DEBUG("Setting previous attribute " + toString(myKey) + " '" + myOrigValue + "' into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
    // set original value
    myAC->setAttribute(myKey, myOrigValue);
    // certain attributes needs extra operations
    if (myKey != GNE_ATTR_SELECTED) {
        // check if updated attribute requieres a update geometry
        if (myAC->getTagProperty().hasAttribute(myKey) && myAC->getTagProperty().getAttributeProperties(myKey).requiereUpdateGeometry()) {
            myAC->updateGeometry();
        }
        // check if netElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
        if (myAC->getTagProperty().isNetElement()) {
            myNet->requiereSaveNet(true);
        } else if (myAC->getTagProperty().isAdditional() || myAC->getTagProperty().isShape()) {
            myNet->requiereSaveAdditionals(true);
        } else if (myAC->getTagProperty().isDemandElement()) {
            myNet->requiereSaveDemandElements(true);
        }
    }
}


void
GNEChange_Attribute::redo() {
    // show extra information for tests
    WRITE_DEBUG("Setting new attribute " + toString(myKey) + " '" + myNewValue + "' into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
    // set new value
    myAC->setAttribute(myKey, myNewValue);
    // certain attributes needs extra operations
    if (myKey != GNE_ATTR_SELECTED) {
        // check if updated attribute requieres a update geometry
        if (myAC->getTagProperty().hasAttribute(myKey) && myAC->getTagProperty().getAttributeProperties(myKey).requiereUpdateGeometry()) {
            myAC->updateGeometry();
        }
        // check if netElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
        if (myAC->getTagProperty().isNetElement()) {
            myNet->requiereSaveNet(true);
        } else if (myAC->getTagProperty().isAdditional() || myAC->getTagProperty().isShape()) {
            myNet->requiereSaveAdditionals(true);
        } else if (myAC->getTagProperty().isDemandElement()) {
            myNet->requiereSaveDemandElements(true);
        }
    }
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


FXString
GNEChange_Attribute::undoName() const {
    return ("Undo change " + myAC->getTagStr() + " attribute").c_str();
}


FXString
GNEChange_Attribute::redoName() const {
    return ("Redo change " + myAC->getTagStr() + " attribute").c_str();
}


/****************************************************************************/
