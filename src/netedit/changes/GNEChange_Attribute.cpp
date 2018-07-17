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
// A network change in which something is moved (for undo/redo)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/netelements/GNENetElement.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEShape.h>

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
    GNEChange(0, true),
    myAC(netElement),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : netElement->getAttribute(key)),
    myNewValue(value),
    myNetElement(netElement),
    myAdditional(nullptr),
    myShape(nullptr) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEAdditional* additionals,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(0, true),
    myAC(additionals),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : additionals->getAttribute(key)),
    myNewValue(value),
    myNetElement(nullptr),
    myAdditional(additionals),
    myShape(nullptr) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEShape* shapes,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(0, true),
    myAC(shapes),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : shapes->getAttribute(key)),
    myNewValue(value),
    myNetElement(nullptr),
    myAdditional(nullptr),
    myShape(shapes) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::~GNEChange_Attribute() {
    assert(myAC);
    myAC->decRef("GNEChange_Attribute " + toString(myKey));
    if (myAC->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(myAC->getTag()) + " '" + myAC->getID() + "' in GNEChange_Attribute");
        }
        // Check if attribute carrier is a shape
        if (myShape) {
            // remove shape using pecify functions
            if (myShape->getTag() == SUMO_TAG_POLY) {
                myShape->getNet()->removePolygon(myShape->getID());
            } else if ((myShape->getTag() == SUMO_TAG_POI) || (myShape->getTag() == SUMO_TAG_POILANE)) {
                myShape->getNet()->removePOI(myShape->getID());
            }
        } else {
            delete myAC;
        }
    }
}


void
GNEChange_Attribute::undo() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Setting previous attribute " + toString(myKey) + " '" + myOrigValue + "' into " + toString(myAC->getTag()) + " '" + myAC->getID() + "'");
    }
    // set original value
    myAC->setAttribute(myKey, myOrigValue);
    // check if netElements, additional or shapes has to be saved
    if (myNetElement) {
        myNetElement->getNet()->requiereSaveNet(true);
    } else if (myAdditional) {
        myAdditional->getViewNet()->getNet()->requiereSaveAdditionals(true);
    } else if (myShape) {
        myShape->getNet()->requiereSaveShapes(true);
    }
}


void
GNEChange_Attribute::redo() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Setting new attribute " + toString(myKey) + " '" + myNewValue + "' into " + toString(myAC->getTag()) + " '" + myAC->getID() + "'");
    }
    // set new value
    myAC->setAttribute(myKey, myNewValue);
    // check if netElements, additional or shapes has to be saved
    if (myNetElement) {
        myNetElement->getNet()->requiereSaveNet(true);
    } else if (myAdditional) {
        myAdditional->getViewNet()->getNet()->requiereSaveAdditionals(true);
    } else if (myShape) {
        myShape->getNet()->requiereSaveShapes(true);
    }
}


bool
GNEChange_Attribute::trueChange() {
    return myOrigValue != myNewValue;
}


FXString
GNEChange_Attribute::undoName() const {
    return ("Undo change " + toString(myAC->getTag()) + " attribute").c_str();
}


FXString
GNEChange_Attribute::redoName() const {
    return ("Redo change " + toString(myAC->getTag()) + " attribute").c_str();
}


/****************************************************************************/
