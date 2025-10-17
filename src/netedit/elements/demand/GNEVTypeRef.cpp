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
/// @file    GNEVTypeRef.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2025
///
// A class for vType references
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEVTypeRef.h"
#include "GNEVehicle.h"

// ===========================================================================
// GNEVTypeRef - methods
// ===========================================================================

GNEVTypeRef::GNEVTypeRef(GNENet* net) :
    GNEDemandElement("", net, "", GNE_TAG_VTYPEREF, GNEPathElement::Options::DEMAND_ELEMENT) {
}


GNEVTypeRef::GNEVTypeRef(GNEDemandElement* distributionParent, GNEDemandElement* vTypeParent, const double probability) :
    GNEDemandElement(distributionParent, GNE_TAG_VTYPEREF, GNEPathElement::Options::DEMAND_ELEMENT),
    myProbability(probability) {
    // set parents
    setParents<GNEDemandElement*>({distributionParent, vTypeParent});
}


GNEVTypeRef::~GNEVTypeRef() {}


GNEMoveElement*
GNEVTypeRef::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEVTypeRef::getParameters() {
    return nullptr;
}


const Parameterised*
GNEVTypeRef::getParameters() const {
    return nullptr;
}


GUIGLObjectPopupMenu*
GNEVTypeRef::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // create popup
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
    // build common options
    buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
    return ret;
}


void
GNEVTypeRef::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_VTYPE);
    device.writeAttr(SUMO_ATTR_REFID, getAttribute(SUMO_ATTR_REFID));
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    // close tag
    device.closeTag();
}


GNEDemandElement::Problem
GNEVTypeRef::isDemandElementValid() const {
    return Problem::OK;
}


std::string
GNEVTypeRef::getDemandElementProblem() const {
    return "";
}


void
GNEVTypeRef::fixDemandElementProblem() {
    // currently the only solution is removing VType
}


SUMOVehicleClass
GNEVTypeRef::getVClass() const {
    return getParentDemandElements().back()->getVClass();
}


const RGBColor&
GNEVTypeRef::getColor() const {
    return getParentDemandElements().back()->getColor();
}


void
GNEVTypeRef::updateGeometry() {
    // nothing to update
}


Position
GNEVTypeRef::getPositionInView() const {
    return getParentDemandElements().front()->getPositionInView();
}


std::string
GNEVTypeRef::getParentName() const {
    return getParentDemandElements().front()->getID();
}


double
GNEVTypeRef::getExaggeration(const GUIVisualizationSettings& /*s*/) const {
    return 1;
}


Boundary
GNEVTypeRef::getCenteringBoundary() const {
    return getParentDemandElements().front()->getCenteringBoundary();
}


void
GNEVTypeRef::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/,
                               const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


void
GNEVTypeRef::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


void
GNEVTypeRef::computePathElement() {
    // nothing to do
}


void
GNEVTypeRef::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // nothing to draw
}


void
GNEVTypeRef::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // nothing to draw
}


GNELane*
GNEVTypeRef::getFirstPathLane() const {
    return getParentDemandElements().back()->getFirstPathLane();
}


GNELane*
GNEVTypeRef::getLastPathLane() const {
    return getParentDemandElements().back()->getLastPathLane();
}


std::string
GNEVTypeRef::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case GNE_ATTR_VTYPE_DISTRIBUTION:
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
GNEVTypeRef::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_PROB:
            return myProbability;
        default:
            return getCommonAttributeDouble(key);
    }
}


Position
GNEVTypeRef::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


bool
GNEVTypeRef::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_VTYPE_DISTRIBUTION:
        case SUMO_ATTR_REFID:
            return false;
        default:
            return true;
    }
}


void
GNEVTypeRef::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNEVTypeRef::isValid(SumoXMLAttr key, const std::string& value) {
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
GNEVTypeRef::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVTypeRef::getHierarchyName() const {
    return TLF("%: % -> %", myTagProperty->getTagStr(), getAttribute(GNE_ATTR_VTYPE_DISTRIBUTION), getAttribute(SUMO_ATTR_REFID));
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVTypeRef::setAttribute(SumoXMLAttr key, const std::string& value) {
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
