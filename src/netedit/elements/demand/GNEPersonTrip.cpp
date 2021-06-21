/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEPersonTrip.h"
#include "GNERouteHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPersonTrip::GNEPersonTrip(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge,
                             double arrivalPosition, const std::vector<std::string>& types, const std::vector<std::string>& modes) :
    GNEDemandElement(personParent, net, GLO_PERSONTRIP, GNE_TAG_PERSONTRIP_EDGE,
{}, {fromEdge, toEdge}, {}, {}, {}, {}, {personParent}, {}),
myArrivalPosition(arrivalPosition),
myVTypes(types),
myModes(modes) {
}


GNEPersonTrip::GNEPersonTrip(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* toBusStop,
                             double arrivalPosition, const std::vector<std::string>& types, const std::vector<std::string>& modes) :
    GNEDemandElement(personParent, net, GLO_PERSONTRIP, GNE_TAG_PERSONTRIP_BUSSTOP,
{}, {fromEdge}, {}, {toBusStop}, {}, {}, {personParent}, {}),
myArrivalPosition(arrivalPosition),
myVTypes(types),
myModes(modes) {
}


GNEPersonTrip::~GNEPersonTrip() {}


GNEMoveOperation*
GNEPersonTrip::getMoveOperation(const double /*shapeOffset*/) {
    // avoid move person plan that ends in busStop
    if (getParentAdditionals().size() > 0) {
        return nullptr;
    }
    // get geometry end pos
    const Position geometryEndPos = getPathElementArrivalPos();
    // calculate circle width squared
    const double circleWidthSquared = myPersonPlanArrivalPositionDiameter * myPersonPlanArrivalPositionDiameter;
    // check if we clicked over a geometry end pos
    if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= ((circleWidthSquared + 2))) {
        return new GNEMoveOperation(this, getParentEdges().back()->getLaneByAllowedVClass(getVClass()), {myArrivalPosition}, false);
    } else {
        return nullptr;
    }
}


GUIGLObjectPopupMenu*
GNEPersonTrip::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " name to clipboard", nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " typed name to clipboard", nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, "Open " + getTagStr() + " Dialog", getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y()), nullptr, nullptr, 0);
    return ret;
}


void
GNEPersonTrip::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_PERSONTRIP);
    // check if from attribute is enabled
    if (isAttributeEnabled(SUMO_ATTR_FROM)) {
        device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
    }
    // write to depending if personplan ends in a busStop
    if (getParentAdditionals().size() > 0) {
        device.writeAttr(SUMO_ATTR_BUS_STOP, getParentAdditionals().back()->getID());
    } else {
        device.writeAttr(SUMO_ATTR_TO, getParentEdges().back()->getID());
    }
    // only write arrivalPos if is different of -1
    if (myArrivalPosition != -1) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    // write modes
    if (myModes.size() > 0) {
        device.writeAttr(SUMO_ATTR_MODES, myModes);
    }
    // write vTypes
    if (myVTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVTypes);
    }
    // write parameters
    writeParams(device);
    // close tag
    device.closeTag();
}


bool
GNEPersonTrip::isDemandElementValid() const {
    if ((getParentEdges().size() == 2) && (getParentEdges().at(0) == getParentEdges().at(1))) {
        // from and to are the same edges
        return true;
        /*
            } else if (getPath().size() > 0) {
                // if path edges isn't empty, then there is a valid route
                return true;
        */
    } else {
        return false;
    }
}


std::string
GNEPersonTrip::getDemandElementProblem() const {
    if (getParentEdges().size() == 0) {
        return ("A person trip need at least one edge");
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getParentEdges().size(); i++) {
            if (myNet->getPathManager()->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().front()->getVClass(), getParentEdges().at((int)i - 1), getParentEdges().at(i)) == false) {
                return ("Edge '" + getParentEdges().at((int)i - 1)->getID() + "' and edge '" + getParentEdges().at(i)->getID() + "' aren't consecutives");
            }
        }
        // there is connections bewteen all edges, then all ok
        return "";
    }
}


void
GNEPersonTrip::fixDemandElementProblem() {
    // currently the only solution is removing PersonTrip
}


SUMOVehicleClass
GNEPersonTrip::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEPersonTrip::getColor() const {
    return getParentDemandElements().front()->getColor();
}


void
GNEPersonTrip::updateGeometry() {
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        i->updateGeometry();
    }
}


Position
GNEPersonTrip::getPositionInView() const {
    return Position();
}


std::string
GNEPersonTrip::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNEPersonTrip::getCenteringBoundary() const {
    Boundary personTripBoundary;
    // return the combination of all parent edges's boundaries
    for (const auto& i : getParentEdges()) {
        personTripBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (personTripBoundary.isInitialised()) {
        return personTripBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEPersonTrip::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEPersonTrip::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // PersonTrips are drawn in drawPartialGL
}


void
GNEPersonTrip::computePathElement() {
    // calculate path
    myNet->getPathManager()->calculatePathLanes(this, SVC_PEDESTRIAN, {getFirstPathLane(), getLastPathLane()});
    // update geometry
    updateGeometry();
}


void
GNEPersonTrip::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over lane
    drawPersonPlanPartial(drawPersonPlan(), s, lane, segment, offsetFront, s.widthSettings.personTrip, s.colorSettings.personTrip);
}


void
GNEPersonTrip::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over junction
    drawPersonPlanPartial(drawPersonPlan(),s, fromLane, toLane, segment, offsetFront, s.widthSettings.personTrip, s.colorSettings.personTrip);
}


GNELane*
GNEPersonTrip::getFirstPathLane() const {
    return getParentEdges().front()->getLaneByDisallowedVClass(SVC_PEDESTRIAN);
}


GNELane*
GNEPersonTrip::getLastPathLane() const {
    // check if personPlan ends in a BusStop
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getParentLanes().front();
    } else {
        return getParentEdges().back()->getLaneByDisallowedVClass(SVC_PEDESTRIAN);
    }
}


std::string
GNEPersonTrip::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_ID:
            return getParentDemandElements().front()->getID();
        case SUMO_ATTR_FROM:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_TO:
            return getParentEdges().back()->getID();
        case GNE_ATTR_TO_BUSSTOP:
            return getParentAdditionals().back()->getID();
        // specific person plan attributes
        case SUMO_ATTR_MODES:
            return joinToString(myModes, " ");
        case SUMO_ATTR_VTYPES:
            return joinToString(myVTypes, " ");
        case SUMO_ATTR_ARRIVALPOS:
            if (myTagProperty.getTag() == GNE_TAG_PERSONTRIP_BUSSTOP) {
                return getParentAdditionals().front()->getAttribute(SUMO_ATTR_ENDPOS);
            } else if (myArrivalPosition == -1) {
                return "";
            } else {
                return toString(myArrivalPosition);
            }
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_PARENT:
            return getParentDemandElements().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEPersonTrip::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
            if (myArrivalPosition != -1) {
                return myArrivalPosition;
            } else {
                return (getLastPathLane()->getLaneShape().length() - POSITION_EPS);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


Position
GNEPersonTrip::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS: {
            // get lane shape
            const PositionVector& laneShape = getLastPathLane()->getLaneShape();
            // continue depending of arrival position
            if (myArrivalPosition == 0) {
                return laneShape.front();
            } else if ((myArrivalPosition == -1) || (myArrivalPosition >= laneShape.length2D())) {
                return laneShape.back();
            } else {
                return laneShape.positionAtOffset2D(myArrivalPosition);
            }
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a position attribute of type '" + toString(key) + "'");
    }
}


void
GNEPersonTrip::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_ARRIVALPOS:
        case SUMO_ATTR_MODES:
        case SUMO_ATTR_VTYPES:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        // special case for "to" attributes
        case SUMO_ATTR_TO: {
            // get next personPlan
            GNEDemandElement* nextPersonPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextPersonPlan
            if (nextPersonPlan) {
                undoList->p_begin("Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, value, undoList);
                undoList->p_add(new GNEChange_Attribute(this, key, value));
                undoList->p_end();
            } else {
                undoList->p_add(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        case GNE_ATTR_TO_BUSSTOP: {
            // get next person plan
            GNEDemandElement* nextPersonPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextPersonPlan
            if (nextPersonPlan) {
                // obtain busStop
                const GNEAdditional* busStop = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, value);
                // change from attribute using edge ID
                undoList->p_begin("Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, busStop->getParentLanes().front()->getParentEdge()->getID(), undoList);
                undoList->p_add(new GNEChange_Attribute(this, key, value));
                undoList->p_end();
            } else {
                undoList->p_add(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPersonTrip::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrieveEdge(value, false) != nullptr);
        case GNE_ATTR_TO_BUSSTOP:
            return (myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        // specific person plan attributes
        case SUMO_ATTR_MODES: {
            SVCPermissions dummyModeSet;
            std::string dummyError;
            return SUMOVehicleParameter::parsePersonModes(value, myTagProperty.getTagStr(), getID(), dummyModeSet, dummyError);
        }
        case SUMO_ATTR_VTYPES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                const double parsedValue = canParse<double>(value);
                if ((parsedValue < 0) || (parsedValue > getLastPathLane()->getLaneShape().length())) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEPersonTrip::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNEPersonTrip::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNEPersonTrip::isAttributeEnabled(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_FROM) {
        return (getParentDemandElements().at(0)->getPreviousChildDemandElement(this) == nullptr);
    } else {
        return true;
    }
}


std::string
GNEPersonTrip::getPopUpID() const {
    return getTagStr();
}


std::string
GNEPersonTrip::getHierarchyName() const {
    if (myTagProperty.getTag() == GNE_TAG_PERSONTRIP_EDGE) {
        return "personTrip: " + getParentEdges().front()->getID() + " -> " + getParentEdges().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_PERSONTRIP_BUSSTOP) {
        return "personTrip: " + getParentEdges().front()->getID() + " -> " + getParentAdditionals().front()->getID();
    } else {
        throw ("Invalid personTrip tag");
    }
}


const std::map<std::string, std::string>&
GNEPersonTrip::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPersonTrip::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_FROM:
            // change first edge
            replaceFirstParentEdge(value);
            // compute person trip
            computePathElement();
            break;
        case SUMO_ATTR_TO:
            // change last edge
            replaceLastParentEdge(value);
            // compute person trip
            computePathElement();
            break;
        case GNE_ATTR_TO_BUSSTOP:
            replaceAdditionalParent(SUMO_TAG_BUS_STOP, value);
            // compute person trip
            computePathElement();
            break;
        // specific person plan attributes
        case SUMO_ATTR_MODES:
            myModes = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                myArrivalPosition = -1;
            } else {
                myArrivalPosition = parse<double>(value);
            }
            updateGeometry();
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEPersonTrip::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


void
GNEPersonTrip::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNEPersonTrip::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->p_begin("arrivalPos of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->p_end();
}

/****************************************************************************/
