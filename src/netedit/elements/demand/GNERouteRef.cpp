/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNERouteRef.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2025
///
// A class for route references
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNERouteRef.h"
#include "GNEVehicle.h"

// ===========================================================================
// GNERouteRef - methods
// ===========================================================================

GNERouteRef::GNERouteRef(GNENet* net) :
    GNEDemandElement("", net, "", GNE_TAG_ROUTEREF, GNEPathElement::Options::DEMAND_ELEMENT) {
}


GNERouteRef::GNERouteRef(GNEDemandElement* distributionParent, GNEDemandElement* routeParent, const double probability) :
    GNEDemandElement(distributionParent, GNE_TAG_ROUTEREF, GNEPathElement::Options::DEMAND_ELEMENT),
    myProbability(probability) {
    // set parents
    setParents<GNEDemandElement*>({distributionParent, routeParent});
}


GNERouteRef::~GNERouteRef() {}


GNEMoveElement*
GNERouteRef::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNERouteRef::getParameters() {
    return nullptr;
}


const Parameterised*
GNERouteRef::getParameters() const {
    return nullptr;
}


GUIGLObjectPopupMenu*
GNERouteRef::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // create popup
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
    // build common options
    buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
    return ret;
}


void
GNERouteRef::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE);
    device.writeAttr(SUMO_ATTR_REFID, getAttribute(SUMO_ATTR_REFID));
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    // close tag
    device.closeTag();
}


GNEDemandElement::Problem
GNERouteRef::isDemandElementValid() const {
    return Problem::OK;
}


std::string
GNERouteRef::getDemandElementProblem() const {
    return "";
}


void
GNERouteRef::fixDemandElementProblem() {
    // currently the only solution is removing Route
}


SUMOVehicleClass
GNERouteRef::getVClass() const {
    return getParentDemandElements().back()->getVClass();
}


const RGBColor&
GNERouteRef::getColor() const {
    return getParentDemandElements().back()->getColor();
}


void
GNERouteRef::updateGeometry() {
    // nothing to update
}


Position
GNERouteRef::getPositionInView() const {
    return getParentDemandElements().front()->getPositionInView();
}


std::string
GNERouteRef::getParentName() const {
    return getParentDemandElements().front()->getID();
}


double
GNERouteRef::getExaggeration(const GUIVisualizationSettings& /*s*/) const {
    return 1;
}


Boundary
GNERouteRef::getCenteringBoundary() const {
    return getParentDemandElements().front()->getCenteringBoundary();
}


void
GNERouteRef::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/,
                               const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


void
GNERouteRef::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


void
GNERouteRef::computePathElement() {
    // nothing to do
}


void
GNERouteRef::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // nothing to draw
}


void
GNERouteRef::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // nothing to draw
}


GNELane*
GNERouteRef::getFirstPathLane() const {
    return getParentDemandElements().back()->getFirstPathLane();
}


GNELane*
GNERouteRef::getLastPathLane() const {
    return getParentDemandElements().back()->getLastPathLane();
}


std::string
GNERouteRef::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case GNE_ATTR_ROUTE_DISTRIBUTION:
            return getParentDemandElements().front()->getID();
        case SUMO_ATTR_REFID:
            return getParentDemandElements().back()->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        default:
            return getCommonAttribute(key);
    }
}


double
GNERouteRef::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_PROB:
            return myProbability;
        default:
            return getCommonAttributeDouble(key);
    }
}


Position
GNERouteRef::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


bool
GNERouteRef::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_ROUTE_DISTRIBUTION:
        case SUMO_ATTR_REFID:
            return false;
        default:
            return true;
    }
}


void
GNERouteRef::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_PROB:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNERouteRef::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_PROB:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value) && (parse<double>(value) >= 0);
            }
        default:
            return isCommonAttributeValid(key, value);
    }
}


std::string
GNERouteRef::getPopUpID() const {
    return getTagStr();
}


std::string
GNERouteRef::getHierarchyName() const {
    return TLF("%: % -> %", myTagProperty->getTagStr(), getAttribute(GNE_ATTR_ROUTE_DISTRIBUTION), getAttribute(SUMO_ATTR_REFID));
}

// ===========================================================================
// private
// ===========================================================================

void
GNERouteRef::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_PROB:
            if (value.empty()) {
                myProbability = myTagProperty->getDefaultDoubleValue(key);
            } else {
                myProbability = parse<double>(value);
            }
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
