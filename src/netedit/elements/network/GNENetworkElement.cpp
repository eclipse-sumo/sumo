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
/// @file    GNENetworkElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// A abstract class for networkElements
/****************************************************************************/
#include <config.h>

#include <utils/gui/div/GUIParameterTableWindow.h>
#include <netedit/frames/common/GNEDeleteFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>

#include "GNENetworkElement.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNENetworkElement::GNENetworkElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon,
                                     const std::vector<GNEJunction*>& junctionParents,
                                     const std::vector<GNEEdge*>& edgeParents,
                                     const std::vector<GNELane*>& laneParents,
                                     const std::vector<GNEAdditional*>& additionalParents,
                                     const std::vector<GNEDemandElement*>& demandElementParents,
                                     const std::vector<GNEGenericData*>& genericDataParents) :
    GUIGlObject(type, id, icon),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    myShapeEdited(false) {
}


GNENetworkElement::~GNENetworkElement() {}


GUIGlObject*
GNENetworkElement::getGUIGlObject() {
    return this;
}


void
GNENetworkElement::setShapeEdited(const bool value) {
    myShapeEdited = value;
}


bool
GNENetworkElement::isShapeEdited() const {
    return myShapeEdited;
}


bool
GNENetworkElement::GNENetworkElement::isNetworkElementValid() const {
    // implement in children
    return true;
}


std::string
GNENetworkElement::GNENetworkElement::getNetworkElementProblem() const {
    // implement in children
    return "";
}


GUIParameterTableWindow*
GNENetworkElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.isUnique()) {
            ret->mkItem(i.getAttrStr().c_str(), false, getAttribute(i.getAttr()));
        } else {
            ret->mkItem(i.getAttrStr().c_str(), true, getAttribute(i.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNENetworkElement::getCenteringBoundary() const {
    return myBoundary;
}


bool
GNENetworkElement::isGLObjectLocked() {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return myNet->getViewNet()->getLockManager().isObjectLocked(getType(), isAttributeCarrierSelected());
    } else {
        return true;
    }
}


void
GNENetworkElement::markAsFrontElement() {
    myNet->getViewNet()->setFrontAttributeCarrier(this);
}


void
GNENetworkElement::selectGLObject() {
    if (isAttributeCarrierSelected()) {
        unselectAttributeCarrier();
    } else {
        selectAttributeCarrier();
    }
    // update information label
    myNet->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionInformation()->updateInformationLabel();
}


const std::string
GNENetworkElement::getOptionalName() const {
    try {
        return getAttribute(SUMO_ATTR_NAME);
    } catch (InvalidArgument&) {
        return "";
    }
}


std::string
GNENetworkElement::getPopUpID() const {
    if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM) + "_" + getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO) + "_" + getAttribute(SUMO_ATTR_TO_LANE);
    } else {
        return getTagStr() + ": " + getID();
    }
}


std::string
GNENetworkElement::getHierarchyName() const {
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
