/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_EnableAttribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
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
#include <netedit/GNEViewParent.h>
#include <netedit/netelements/GNENetElement.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNEFrame.h>

#include "GNEChange_EnableAttribute.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_EnableAttribute, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_EnableAttribute::GNEChange_EnableAttribute(GNEAttributeCarrier* ac, GNENet* net, const SumoXMLAttr newAttribute) :
    GNEChange(net, true),
    myAC(ac),
    myNewAttribute(newAttribute),
    myOriginalAttributesEnabled(ac->myAttributesEnabled) {
    myAC->incRef("GNEChange_EnableAttribute " + toString(myNewAttribute));
}


GNEChange_EnableAttribute::~GNEChange_EnableAttribute() {
    // decrease reference
    myAC->decRef("GNEChange_EnableAttribute " + toString(myNewAttribute));
    // remove if is unreferenced
    if (myAC->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAC->getTagStr() + " '" + myAC->getID() + "' in GNEChange_EnableAttribute");
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
GNEChange_EnableAttribute::undo() {
    // show extra information for tests
    WRITE_DEBUG("Setting previous attribute " + toString(myNewAttribute) + " into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
    // set original value
    myAC->myAttributesEnabled = myOriginalAttributesEnabled;
    // check if netElements, additional or shapes has to be saved
    if (myAC->getTagProperty().isNetElement()) {
        myNet->requiereSaveNet(true);
    } else if (myAC->getTagProperty().isAdditional() || myAC->getTagProperty().isShape()) {
        myNet->requiereSaveAdditionals(true);
    } else if (myAC->getTagProperty().isDemandElement()) {
        myNet->requiereSaveDemandElements(true);
    }
}


void
GNEChange_EnableAttribute::redo() {
    // show extra information for tests
    WRITE_DEBUG("Setting new attribute " + toString(myNewAttribute) + " into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
    // set new value
    myAC->enableAttribute(myNewAttribute);
    // check if netElements, additional or shapes has to be saved
    if (myAC->getTagProperty().isNetElement()) {
        myNet->requiereSaveNet(true);
    } else if (myAC->getTagProperty().isAdditional() || myAC->getTagProperty().isShape()) {
        myNet->requiereSaveAdditionals(true);
    } else if (myAC->getTagProperty().isDemandElement()) {
        myNet->requiereSaveDemandElements(true);
    }
}


FXString
GNEChange_EnableAttribute::undoName() const {
    return ("Undo change " + myAC->getTagStr() + " attribute").c_str();
}


FXString
GNEChange_EnableAttribute::redoName() const {
    return ("Redo change " + myAC->getTagStr() + " attribute").c_str();
}


/****************************************************************************/
