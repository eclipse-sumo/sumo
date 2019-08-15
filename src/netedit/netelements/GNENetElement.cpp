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
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/geom/GeomHelper.h>

#include "GNENetElement.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNENetElement::NetElementGeometry - methods
// ---------------------------------------------------------------------------

GNENetElement::NetElementGeometry::NetElementGeometry() {}


void
GNENetElement::NetElementGeometry::clearGeometry() {
    shape.clear();
    shapeRotations.clear();
    shapeLengths.clear();
}


void
GNENetElement::NetElementGeometry::calculateShapeRotationsAndLengths() {
    // Get number of parts of the shape
    int numberOfSegments = (int)shape.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Reserve memory (To improve efficiency)
        shapeRotations.reserve(numberOfSegments);
        shapeLengths.reserve(numberOfSegments);
        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {
            // Obtain first position
            const Position& f = shape[i];
            // Obtain next position
            const Position& s = shape[i + 1];
            // Save distance between position into myShapeLengths
            shapeLengths.push_back(f.distanceTo(s));
            // Save rotation (angle) of the vector constructed by points f and s
            shapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
}

// ---------------------------------------------------------------------------
// GNENetElement - methods
// ---------------------------------------------------------------------------

GNENetElement::GNENetElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    GNEHierarchicalElementParents(this, {}, {}, {}, {}, {}),
                              GNEHierarchicalElementChildren(this, {}, {}, {}, {}, {}),
                              myNet(net),
myMovingGeometryBoundary() {
}


GNENetElement::~GNENetElement() {}


const GNENetElement::NetElementGeometry&
GNENetElement::getGeometry() const {
    return myGeometry;
}


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


void
GNENetElement::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNENetElement::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
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


void
GNENetElement::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}

/****************************************************************************/
