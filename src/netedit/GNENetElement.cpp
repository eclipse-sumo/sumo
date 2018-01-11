/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/div/GUIParameterTableWindow.h>

#include "GNENetElement.h"
#include "GNEAdditional.h"
#include "GNENet.h"

// ===========================================================================
// method definitions
// ===========================================================================


GNENetElement::GNENetElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag, GUIIcon icon) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag, icon),
    myNet(net) {
}


GNENetElement::~GNENetElement() {}


GNENet*
GNENetElement::getNet() const {
    return myNet;
}


void
GNENetElement::addAdditionalParent(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myAdditionalParents.begin(), myAdditionalParents.end(), additional) != myAdditionalParents.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' was already inserted in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionalParents.push_back(additional);
        // update geometry is needed for stacked additionals (routeProbes and Vaporicers)
        updateGeometry();
    }
}


void
GNENetElement::removeAdditionalParent(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myAdditionalParents.begin(), myAdditionalParents.end(), additional);
    if (it == myAdditionalParents.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' doesn't exist in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionalParents.erase(it);
        // update geometry is needed for stacked additionals (routeProbes and Vaporicers)
        updateGeometry();
    }
}


void
GNENetElement::addAdditionalChild(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional) != myAdditionalChilds.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' was already inserted in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionalChilds.push_back(additional);
        // update geometry is needed for stacked additionals (routeProbes and Vaporicers)
        updateGeometry();
    }
}


void
GNENetElement::removeAdditionalChild(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional);
    if (it == myAdditionalChilds.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' doesn't exist in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionalChilds.erase(it);
        // update geometry is needed for stacked additionals (routeProbes and Vaporicers)
        updateGeometry();
    }
}


const std::vector<GNEAdditional*>&
GNENetElement::getAdditionalParents() const {
    return myAdditionalParents;
}


const std::vector<GNEAdditional*>&
GNENetElement::getAdditionalChilds() const {
    return myAdditionalChilds;
}


const std::string&
GNENetElement::getParentName() const {
    return myNet->getMicrosimID();
}


GUIParameterTableWindow*
GNENetElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // get attributes
    std::vector<SumoXMLAttr> attributes = getAttrs();
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, (int)attributes.size());
    // Iterate over attributes
    for (auto i : attributes) {
        // Add attribute and set it dynamic if aren't unique
        if (GNEAttributeCarrier::isUnique(getTag(), i)) {
            ret->mkItem(toString(i).c_str(), false, getAttribute(i));
        } else {
            ret->mkItem(toString(i).c_str(), true, getAttribute(i));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


/****************************************************************************/
