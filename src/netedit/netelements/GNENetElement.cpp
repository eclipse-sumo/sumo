/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNENetElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A abstract class for netElements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>

#include "GNENetElement.h"


// ===========================================================================
// method definitions
// ===========================================================================


GNENetElement::GNENetElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    GNEHierarchicalElementParents(this, {}, {}, {}, {}, {}, {}),
    GNEHierarchicalElementChilds(this, {}, {}, {}, {}, {}),
    myNet(net),
myMovingGeometryBoundary() {}


GNENetElement::~GNENetElement() {}


std::string
GNENetElement::generateChildID(SumoXMLTag /*childTag*/) {
    return "";
}


GNENet*
GNENetElement::getNet() const {
    return myNet;
}


GUIParameterTableWindow*
GNENetElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, myTagProperty.getNumberOfAttributes());
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.second.isUnique()) {
            ret->mkItem(toString(i.first).c_str(), false, getAttribute(i.first));
        } else {
            ret->mkItem(toString(i.first).c_str(), true, getAttribute(i.first));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


void
GNENetElement::selectAttributeCarrier(bool changeFlag) {
    if (!myNet) {
        throw ProcessError("Net cannot be nullptr");
    } else {
        gSelected.select(getGlID());
        // add object into list of selected objects
        myNet->getViewNet()->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(getType());
        if (changeFlag) {
            mySelected = true;

        }
    }
}


void
GNENetElement::unselectAttributeCarrier(bool changeFlag) {
    if (!myNet) {
        throw ProcessError("Net cannot be nullptr");
    } else {
        gSelected.deselect(getGlID());
        // remove object of list of selected objects
        myNet->getViewNet()->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(getType());
        if (changeFlag) {
            mySelected = false;
        }
    }
}


bool
GNENetElement::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNENetElement::drawUsingSelectColor() const {
    if (mySelected && (myNet->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK)) {
        return true;
    } else {
        return false;
    }
}


std::string
GNENetElement::getPopUpID() const {
    if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM) + "_" + getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO) + "_" + getAttribute(SUMO_ATTR_TO_LANE);
    } else {
        return getTagStr() + ": " + getID();
    }
}


std::string
GNENetElement::getHierarchyName() const {
    if (myTagProperty.getTag() == SUMO_TAG_LANE) {
        return toString(SUMO_TAG_LANE) + " " + getAttribute(SUMO_ATTR_INDEX);
    } else if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO_LANE);
    } else if ((myTagProperty.getTag() == SUMO_TAG_EDGE) || (myTagProperty.getTag() == SUMO_TAG_CROSSING)) {
        return getPopUpID();
    } else {
        return getTagStr();
    }
}

/****************************************************************************/
