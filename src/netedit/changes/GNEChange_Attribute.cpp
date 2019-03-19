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
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/netelements/GNENetElement.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNEFrame.h>

#include "GNEChange_Attribute.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Attribute, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier *ac, GNENet* net, 
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(net, true),
    myAC(ac),
    myKey(key),
    myForceChange(false),
    myOrigValue(customOrigValue ? origValue : ac->getAttribute(key)),
    myNewValue(value),
    myOldParametersSet(0),
    myNewParametersSet(0) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier *ac, GNENet* net, 
        bool forceChange, SumoXMLAttr key, const std::string& value) :
    GNEChange(net, true),
    myAC(ac),
    myKey(key),
    myForceChange(forceChange),
    myOrigValue(ac->getAttribute(key)),
    myNewValue(value),
    myOldParametersSet(0),
    myNewParametersSet(0) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier *ac,
        GNENet* net, const int oldParametersSet, const int newParametersSet) :
    GNEChange(net, true),
    myAC(ac),
    myKey(SUMO_ATTR_NOTHING),
    myForceChange(false),
    myOldParametersSet(oldParametersSet),
    myNewParametersSet(newParametersSet) {
    myAC->incRef("GNEChange_Attribute parameterSet " + toString(newParametersSet));
}


GNEChange_Attribute::~GNEChange_Attribute() {
    // check if we're editing the value of an attribute or changing a disjoint attribute
    if(myKey != SUMO_ATTR_NOTHING) {
        myAC->decRef("GNEChange_Attribute " + toString(myKey));
    } else {
        myAC->decRef("GNEChange_Attribute parameterSet " + toString(myNewParametersSet));
    }
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
    // check if we're editing the value of an attribute or changing a disjoint attribute
    if(myKey == SUMO_ATTR_NOTHING) {
        // show extra information for tests
        WRITE_DEBUG("Setting previous parameterSet '" + toString(myNewParametersSet) + "' into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
        // set original disjoint attribute
        myAC->setDisjointAttribute(myOldParametersSet);
    } else {
        // show extra information for tests
         WRITE_DEBUG("Setting previous attribute " + toString(myKey) + " '" + myOrigValue + "' into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
        // set original value
        myAC->setAttribute(myKey, myOrigValue);
        // check if netElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
        if (myKey != GNE_ATTR_SELECTED) {
            if (myAC->getTagProperty().isNetElement()) {
                myNet->requiereSaveNet(true);
            } else if (myAC->getTagProperty().isAdditional() || myAC->getTagProperty().isShape()) {
                myNet->requiereSaveAdditionals(true);
            } else if (myAC->getTagProperty().isDemandElement()) {
                myNet->requiereSaveDemandElements(true);
            }
        }
    }
    // update current show frame afgter undo/redo
    if (myNet->getViewNet()->getViewParent()->getCurrentShownFrame()) {
        myNet->getViewNet()->getViewParent()->getCurrentShownFrame()->updateFrameAfterUndoRedo();
    }
}


void
GNEChange_Attribute::redo() {
    // check if we're editing the value of an attribute or changing a disjoint attribute
    if(myKey == SUMO_ATTR_NOTHING) {
        // show extra information for tests
        WRITE_DEBUG("Setting new parameterSet '" + toString(myNewParametersSet) + "' into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
        // set original disjoint attribute
        myAC->setDisjointAttribute(myNewParametersSet);

    } else {
        // show extra information for tests
        WRITE_DEBUG("Setting new attribute " + toString(myKey) + " '" + myNewValue + "' into " + myAC->getTagStr() + " '" + myAC->getID() + "'");
        // set new value
        myAC->setAttribute(myKey, myNewValue);
        // check if netElements, additional or shapes has to be saved (only if key isn't GNE_ATTR_SELECTED)
        if (myKey != GNE_ATTR_SELECTED) {
            if (myAC->getTagProperty().isNetElement()) {
                myNet->requiereSaveNet(true);
            } else if (myAC->getTagProperty().isAdditional() || myAC->getTagProperty().isShape()) {
                myNet->requiereSaveAdditionals(true);
            } else if (myAC->getTagProperty().isDemandElement()) {
                myNet->requiereSaveDemandElements(true);
            }
        }
    }
    // update current show frame afgter undo/redo
    if (myNet->getViewNet()->getViewParent()->getCurrentShownFrame()) {
        myNet->getViewNet()->getViewParent()->getCurrentShownFrame()->updateFrameAfterUndoRedo();
    }
}


bool
GNEChange_Attribute::trueChange() {
    // check if we're editing the value of an attribute or changing a disjoint attribute
    if(myForceChange) {
        return true;
    } else if (myKey == SUMO_ATTR_NOTHING) {
        return (myOldParametersSet != myNewParametersSet);
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
