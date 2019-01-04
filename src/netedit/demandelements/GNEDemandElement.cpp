/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
/// @version $Id$
///
// A abstract class for demand elements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>

#include "GNEDemandElement.h"


// ===========================================================================
// method definitions
// ===========================================================================


GNEDemandElement::GNEDemandElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    myNet(net),
    myMovingGeometryBoundary() {}


GNEDemandElement::~GNEDemandElement() {}


GNENet*
GNEDemandElement::getNet() const {
    return myNet;
}


void
GNEDemandElement::addDemandParent(GNEDemandElement* demand) {
    // First check that demand wasn't already inserted
    if (std::find(myDemandParents.begin(), myDemandParents.end(), demand) != myDemandParents.end()) {
        throw ProcessError(demand->getTagStr() + " with ID='" + demand->getID() + "' was already inserted in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myDemandParents.push_back(demand);
        // update geometry is needed for stacked demands (routeProbes and Vaporicers)
        updateGeometry(true);
    }
}


void
GNEDemandElement::removeDemandParent(GNEDemandElement* demand) {
    // First check that demand was already inserted
    auto it = std::find(myDemandParents.begin(), myDemandParents.end(), demand);
    if (it == myDemandParents.end()) {
        throw ProcessError(demand->getTagStr() + " with ID='" + demand->getID() + "' doesn't exist in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myDemandParents.erase(it);
        // update geometry is needed for stacked demands (routeProbes and Vaporizers)
        updateGeometry(true);
    }
}


void
GNEDemandElement::addDemandChild(GNEDemandElement* demand) {
    // First check that demand wasn't already inserted
    if (std::find(myDemandChilds.begin(), myDemandChilds.end(), demand) != myDemandChilds.end()) {
        throw ProcessError(demand->getTagStr() + " with ID='" + demand->getID() + "' was already inserted in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myDemandChilds.push_back(demand);
        // update geometry is needed for stacked demands (routeProbes and Vaporicers)
        updateGeometry(true);
    }
}


void
GNEDemandElement::removeDemandChild(GNEDemandElement* demand) {
    // First check that demand was already inserted
    auto it = std::find(myDemandChilds.begin(), myDemandChilds.end(), demand);
    if (it == myDemandChilds.end()) {
        throw ProcessError(demand->getTagStr() + " with ID='" + demand->getID() + "' doesn't exist in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myDemandChilds.erase(it);
        // update geometry is needed for stacked demands (routeProbes and Vaporizers)
        updateGeometry(true);
    }
}


const std::vector<GNEDemandElement*>&
GNEDemandElement::getDemandParents() const {
    return myDemandParents;
}


const std::vector<GNEDemandElement*>&
GNEDemandElement::getDemandChilds() const {
    return myDemandChilds;
}


GUIParameterTableWindow*
GNEDemandElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, myTagProperty.getNumberOfAttributes());
    // Iterate over attributes
    for (const auto &i : myTagProperty) {
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
GNEDemandElement::selectAttributeCarrier(bool changeFlag) {
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
GNEDemandElement::unselectAttributeCarrier(bool changeFlag) {
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
GNEDemandElement::isAttributeCarrierSelected() const {
    return mySelected;
}


std::string
GNEDemandElement::getPopUpID() const {
    if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM) + "_" + getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO) + "_" + getAttribute(SUMO_ATTR_TO_LANE);
    } else {
        return getTagStr() + ": " + getID();
    }
}


std::string
GNEDemandElement::getHierarchyName() const {
    if (myTagProperty.getTag() == SUMO_TAG_LANE) {
        return toString(SUMO_TAG_LANE) + " " + getAttribute(SUMO_ATTR_INDEX);
    } else if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO_LANE);
    } else if (myTagProperty.getTag() == SUMO_TAG_CROSSING) {
        return toString(SUMO_TAG_CROSSING) + " " + getAttribute(SUMO_ATTR_ID);
    } else {
        return getTagStr();
    }
}

/****************************************************************************/
