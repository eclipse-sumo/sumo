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
/// @file    GNEPersonTrip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// A class for visualizing person trips in Netedit
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>

#include "GNEPersonTrip.h"


// ===========================================================================
// method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GNEPersonTrip::GNEPersonTrip(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, "", tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementPlan(this, -1, -1) {
}


GNEPersonTrip::GNEPersonTrip(SumoXMLTag tag, GNEDemandElement* personParent, const GNEPlanParents& planParameters,
                             const double arrivalPosition, const std::vector<std::string>& types, const std::vector<std::string>& modes,
                             const std::vector<std::string>& lines, const double walkFactor, const std::string& group) :
    GNEDemandElement(personParent, tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementPlan(this, -1, arrivalPosition),
    myVTypes(types),
    myModes(modes),
    myLines(lines),
    myWalkFactor(walkFactor),
    myGroup(group) {
    // set parents
    setParents<GNEJunction*>(planParameters.getJunctions());
    setParents<GNEEdge*>(planParameters.getEdges());
    setParents<GNEAdditional*>(planParameters.getAdditionalElements());
    setParents<GNEDemandElement*>(planParameters.getDemandElements(personParent));
    // update centering boundary without updating grid
    updatePlanCenteringBoundary(false);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

GNEPersonTrip::~GNEPersonTrip() {}


GNEMoveElement*
GNEPersonTrip::getMoveElement() const {
    return myMoveElementPlan;
}


Parameterised*
GNEPersonTrip::getParameters() {
    return nullptr;
}


const Parameterised*
GNEPersonTrip::getParameters() const {
    return nullptr;
}


GUIGLObjectPopupMenu*
GNEPersonTrip::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return getPlanPopUpMenu(app, parent);
}


void
GNEPersonTrip::writeDemandElement(OutputDevice& device) const {
    // first write origin stop (if this element starts in a stoppingPlace)
    writeOriginStop(device);
    // write rest of attributes
    device.openTag(SUMO_TAG_PERSONTRIP);
    writeLocationAttributes(device);
    if (myModes.size() > 0) {
        device.writeAttr(SUMO_ATTR_MODES, myModes);
    }
    if (myLines.size() > 0) {
        device.writeAttr(SUMO_ATTR_LINES, myLines);
    }
    if (myVTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVTypes);
    }
    if (myWalkFactor > 0) {
        device.writeAttr(SUMO_ATTR_WALKFACTOR, myWalkFactor);
    }
    if (myGroup.size() > 0) {
        device.writeAttr(SUMO_ATTR_GROUP, myGroup);
    }
    device.closeTag();
}


GNEDemandElement::Problem
GNEPersonTrip::isDemandElementValid() const {
    return isPlanPersonValid();
}


std::string
GNEPersonTrip::getDemandElementProblem() const {
    return getPersonPlanProblem();
}


void
GNEPersonTrip::fixDemandElementProblem() {
    // currently the only solution is removing PersonTrip
}


SUMOVehicleClass
GNEPersonTrip::getVClass() const {
    return SVC_PEDESTRIAN;
}


const RGBColor&
GNEPersonTrip::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.personTripColor;
}


void
GNEPersonTrip::updateGeometry() {
    updatePlanGeometry();
}


Position
GNEPersonTrip::getPositionInView() const {
    return getPlanPositionInView();
}


std::string
GNEPersonTrip::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNEPersonTrip::getCenteringBoundary() const {
    return getPlanCenteringBoundary();
}


void
GNEPersonTrip::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEPersonTrip::drawGL(const GUIVisualizationSettings& s) const {
    drawPlanGL(checkDrawPersonPlan(), s, s.colorSettings.personTripColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNEPersonTrip::computePathElement() {
    computePlanPathElement();
}


void
GNEPersonTrip::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    drawPlanLanePartial(checkDrawPersonPlan(), s, segment, offsetFront, s.widthSettings.personTripWidth, s.colorSettings.personTripColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNEPersonTrip::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    drawPlanJunctionPartial(checkDrawPersonPlan(), s, segment, offsetFront, s.widthSettings.personTripWidth, s.colorSettings.personTripColor, s.colorSettings.selectedPersonPlanColor);
}


GNELane*
GNEPersonTrip::getFirstPathLane() const {
    return getFirstPlanPathLane();
}


GNELane*
GNEPersonTrip::getLastPathLane() const {
    return getLastPlanPathLane();
}


std::string
GNEPersonTrip::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        // specific person plan attributes
        case SUMO_ATTR_MODES:
            return joinToString(myModes, " ");
        case SUMO_ATTR_VTYPES:
            return joinToString(myVTypes, " ");
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_WALKFACTOR:
            return toString(myWalkFactor);
        case SUMO_ATTR_GROUP:
            return toString(myGroup);
        default:
            return getPlanAttribute(key);
    }
}


double
GNEPersonTrip::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_WALKFACTOR:
            return myWalkFactor;
        default:
            return getPlanAttributeDouble(key);
    }
}


Position
GNEPersonTrip::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key);
}


void
GNEPersonTrip::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_MODES:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_LINES:
        case SUMO_ATTR_WALKFACTOR:
        case SUMO_ATTR_GROUP:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setPlanAttribute(key, value, undoList);
            break;
    }
}


bool
GNEPersonTrip::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // specific person plan attributes
        case SUMO_ATTR_MODES: {
            SVCPermissions dummyModeSet;
            std::string dummyError;
            return SUMOVehicleParameter::parsePersonModes(value, myTagProperty->getTagStr(), "", dummyModeSet, dummyError);
        }
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_LINES:
            return true;
        case SUMO_ATTR_WALKFACTOR:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_GROUP:
            return true;
        default:
            return isPlanValid(key, value);
    }
}


bool
GNEPersonTrip::isAttributeEnabled(SumoXMLAttr key) const {
    return isPlanAttributeEnabled(key);
}


std::string
GNEPersonTrip::getPopUpID() const {
    return getTagStr();
}


std::string
GNEPersonTrip::getHierarchyName() const {
    return getPlanHierarchyName();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPersonTrip::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // specific person plan attributes
        case SUMO_ATTR_MODES:
            myModes = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_LINES:
            myLines = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_WALKFACTOR:
            myWalkFactor = parse<double>(value);
            break;
        case SUMO_ATTR_GROUP:
            myGroup = value;
            break;
        default:
            setPlanAttribute(key, value);
            break;
    }
}

/****************************************************************************/
