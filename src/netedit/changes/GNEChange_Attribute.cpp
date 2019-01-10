/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#include <netedit/GNEViewNet.h>
#include <netedit/netelements/GNENetElement.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/demandelements/GNEDemandElement.h>

#include "GNEChange_Attribute.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Attribute, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Attribute::GNEChange_Attribute(GNENetElement* netElement,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(nullptr, true),
    myAC(netElement),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : netElement->getAttribute(key)),
    myNewValue(value),
    myNet(netElement->getNet()),
    myNetElement(netElement),
    myAdditional(nullptr),
    myShape(nullptr),
    myDemandElement(nullptr) {
    assert(myAC && (myNetElement || myAdditional || myShape || myDemandElement));
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEAdditional* additional,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(nullptr, true),
    myAC(additional),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : additional->getAttribute(key)),
    myNewValue(value),
    myNet(additional->getViewNet()->getNet()),
    myNetElement(nullptr),
    myAdditional(additional),
    myShape(nullptr),
    myDemandElement(nullptr) {
    assert(myAC && (myNetElement || myAdditional || myShape || myDemandElement));
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEShape* shape,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(nullptr, true),
    myAC(shape),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : shape->getAttribute(key)),
    myNewValue(value),
    myNet(shape->getNet()),
    myNetElement(nullptr),
    myAdditional(nullptr),
    myShape(shape),
    myDemandElement(nullptr) {
    assert(myAC && (myNetElement || myAdditional || myShape || myDemandElement));
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEDemandElement* demandElement,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(nullptr, true),
    myAC(demandElement),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : demandElement->getAttribute(key)),
    myNewValue(value),
    myNet(demandElement->getViewNet()->getNet()),
    myNetElement(nullptr),
    myAdditional(nullptr),
    myShape(nullptr),
    myDemandElement(demandElement) {
    assert(myAC && (myNetElement || myAdditional || myShape || myDemandElement));
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::~GNEChange_Attribute() {
    assert(myAC);
    myAC->decRef("GNEChange_Attribute " + toString(myKey));
    if (myAC->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAC->getTagStr() + " '" + myAC->getID() + "' in GNEChange_Attribute");
        // Check if attribute carrier is a shape
        if (myShape) {
            // remove shape using pecify functions
            if (myShape->getTagProperty().getTag() == SUMO_TAG_POLY) {
                myNet->removePolygon(myShape->getID());
            } else if ((myShape->getTagProperty().getTag() == SUMO_TAG_POI) || (myShape->getTagProperty().getTag() == SUMO_TAG_POILANE)) {
                myNet->removePOI(myShape->getID());
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
    // check if netElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
    if (myKey != GNE_ATTR_SELECTED) {
        if (myNetElement) {
            myNet->requiereSaveNet(true);
        } else if (myAdditional) {
            myNet->requiereSaveAdditionals(true);
        } else if (myShape) {
            myNet->requiereSaveShapes(true);
        } else if (myDemandElement) {
            ;
            /*
            myNet->requiereSaveDemand(true);
            */
        }
    }
}


void
GNEChange_Attribute::redo() {
    // show extra information for tests
    WRITE_DEBUG("Setting new attribute " + toString(myKey) + " '" + myNewValue + "' into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
    // set new value
    myAC->setAttribute(myKey, myNewValue);
    // check if netElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
    if (myKey != GNE_ATTR_SELECTED) {
        if (myNetElement) {
            myNet->requiereSaveNet(true);
        } else if (myAdditional) {
            myNet->requiereSaveAdditionals(true);
        } else if (myShape) {
            myNet->requiereSaveShapes(true);
        } else if (myDemandElement) {
            ;
            /*
            myNet->requiereSaveDemand(true);
            */
        }
    }
}


bool
GNEChange_Attribute::trueChange() {
    return myOrigValue != myNewValue;
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
