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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>

#include "GNEChange_Attribute.h"
#include "GNEAttributeCarrier.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNEAdditional.h"
#include "GNEShape.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Attribute, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier* ac,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(0, true),
    myAC(ac),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : ac->getAttribute(key)),
    myNewValue(value),
    myAdditional(NULL),
    myShape(NULL) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
    // try to cast AC as additional and Shape
    myAdditional = dynamic_cast<GNEAdditional*>(myAC);
    myShape = dynamic_cast<GNEShape*>(myAC);
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
    // check if additional or shapes has to be saved
    if (myAdditional) {
        myAdditional->getViewNet()->getNet()->requiereSaveAdditionals();
    } else if (myShape) {
        myShape->getNet()->requiereSaveShapes();
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
    // check if additional or shapes has to be saved
    if (myAdditional) {
        myAdditional->getViewNet()->getNet()->requiereSaveAdditionals();
    } else if (myShape) {
        myShape->getNet()->requiereSaveShapes();
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
