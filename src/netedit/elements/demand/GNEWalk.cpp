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
/// @file    GNEWalk.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// A class for visualizing walks in Netedit
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEWalk.h"
#include "GNERoute.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_EDGE,
{}, {fromEdge, toEdge}, {}, {}, {}, {}, {personParent}, {}),
myArrivalPosition(arrivalPosition) {
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* toBusStop, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_BUSSTOP,
{}, {fromEdge}, {}, {toBusStop}, {}, {}, {personParent}, {}),
myArrivalPosition(arrivalPosition) {
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, std::vector<GNEEdge*> edges, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_EDGES,
{}, {edges}, {}, {}, {}, {}, {personParent}, {}),
myArrivalPosition(arrivalPosition) {
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEDemandElement* route, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_ROUTE,
{}, {}, {}, {}, {}, {}, {personParent, route}, {}),
myArrivalPosition(arrivalPosition) {
}


GNEWalk::~GNEWalk() {}


GNEMoveOperation*
GNEWalk::getMoveOperation(const double /*shapeOffset*/) {
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
        // continue depending of parent edges
        if (getParentEdges().size() > 0) {
            return new GNEMoveOperation(this, getParentEdges().back()->getLaneByAllowedVClass(getVClass()), myArrivalPosition, false);
        } else {
            return new GNEMoveOperation(this, getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(getVClass()), myArrivalPosition, false);
        }
    } else {
        return nullptr;
    }
}


GUIGLObjectPopupMenu*
GNEWalk::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
        GUIDesigns::buildFXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
    return ret;
}


void
GNEWalk::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_WALK);
    // write attributes depending  of walk type
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        device.writeAttr(SUMO_ATTR_ROUTE, getParentDemandElements().at(1)->getID());
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getParentEdges()));
    } else {
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
    }
    // only write arrivalPos if is different of -1
    if (myArrivalPosition != -1) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    // write parameters
    writeParams(device);
    // close tag
    device.closeTag();
}


bool
GNEWalk::isDemandElementValid() const {
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        // check if route parent is valid
        return getParentDemandElements().at(1)->isDemandElementValid();
    } else if (getParentEdges().size() == 2) {
        if (getParentEdges().at(0) == getParentEdges().at(1)) {
            // from and to are the same edges, then path is valid
            return true;
        } else {
            // check if exist a route between parent edges
            return (myNet->getPathManager()->getPathCalculator()->calculateDijkstraPath(getParentDemandElements().at(0)->getVClass(), getParentEdges()).size() > 0);
        }
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
GNEWalk::getDemandElementProblem() const {
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        return "";
    } else if (getParentEdges().size() == 0) {
        return ("A walk need at least one edge");
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
GNEWalk::fixDemandElementProblem() {
    // currently the only solution is removing Walk
}


SUMOVehicleClass
GNEWalk::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEWalk::getColor() const {
    return getParentDemandElements().front()->getColor();
}


void
GNEWalk::updateGeometry() {
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        i->updateGeometry();
    }
}


Position
GNEWalk::getPositionInView() const {
    return Position();
}


std::string
GNEWalk::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNEWalk::getCenteringBoundary() const {
    Boundary walkBoundary;
    // return the combination of all parent edges's boundaries
    for (const auto& i : getParentEdges()) {
        walkBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (walkBoundary.isInitialised()) {
        return walkBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEWalk::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // only split geometry of WalkEdges
    if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        // obtain new list of walk edges
        std::string newWalkEdges = getNewListOfParents(originalElement, newElement);
        // update walk edges
        if (newWalkEdges.size() > 0) {
            setAttribute(SUMO_ATTR_EDGES, newWalkEdges, undoList);
        }
    }
}


void
GNEWalk::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Walks are drawn in drawPartialGL
}


void
GNEWalk::computePathElement() {
    // declare lane vector
    std::vector<GNELane*> lanes;
    // update lanes depending of walk tag
    if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        // calculate consecutive path using parent edges
        myNet->getPathManager()->calculateConsecutivePathEdges(this, getVClass(), getParentEdges());
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        // calculate consecutive path using route edges
        myNet->getPathManager()->calculateConsecutivePathEdges(this, getVClass(), getParentDemandElements().back()->getParentEdges());
    } else {
        // get first and last person plane
        lanes = {getFirstPathLane(), getLastPathLane()};
        // calculate path
        myNet->getPathManager()->calculatePathLanes(this, getVClass(), lanes);
    }
    // update geometry
    updateGeometry();
}


void
GNEWalk::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over lane
    drawPersonPlanPartial(drawPersonPlan(), s, lane, segment, offsetFront, s.widthSettings.walk, s.colorSettings.walk);
}


void
GNEWalk::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over junction
    drawPersonPlanPartial(drawPersonPlan(), s, fromLane, toLane, segment, offsetFront, s.widthSettings.walk, s.colorSettings.walk);
}


GNELane*
GNEWalk::getFirstPathLane() const {
    // check if this walk is over a route
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        return getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
    } else {
        return getParentEdges().front()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
    }
}


GNELane*
GNEWalk::getLastPathLane() const {
    // check if this walk is over a route
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        return getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
    } else if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getParentLanes().front();
    } else {
        return getParentEdges().back()->getLaneByDisallowedVClass(SVC_PEDESTRIAN);
    }
}


std::string
GNEWalk::getAttribute(SumoXMLAttr key) const {
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
        case SUMO_ATTR_EDGES:
            return parseIDs(getParentEdges());
        case SUMO_ATTR_ROUTE:
            return getParentDemandElements().at(1)->getID();
        // specific person plan attributes
        case SUMO_ATTR_ARRIVALPOS:
            if (myArrivalPosition == -1) {
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
GNEWalk::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
            if (myArrivalPosition != -1) {
                return myArrivalPosition;
            } else {
                return (getLastPathLane()->getLaneShape().length() - POSITION_EPS);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a doubleattribute of type '" + toString(key) + "'");
    }
}


Position
GNEWalk::getAttributePosition(SumoXMLAttr key) const {
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
GNEWalk::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_ARRIVALPOS:
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
        case SUMO_ATTR_EDGES: {
            // get next person plan
            GNEDemandElement* nextPersonPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextPersonPlan
            if (nextPersonPlan) {
                // obtain edges
                const std::vector<GNEEdge*> edges = parse<std::vector<GNEEdge*> >(myNet, value);
                // change from attribute using edge ID
                undoList->p_begin("Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, edges.back()->getID(), undoList);
                undoList->p_add(new GNEChange_Attribute(this, key, value));
                undoList->p_end();
            } else {
                undoList->p_add(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        case SUMO_ATTR_ROUTE: {
            // get next person plan
            GNEDemandElement* nextPersonPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextPersonPlan
            if (nextPersonPlan) {
                // obtain route
                const GNEDemandElement* route = myNet->retrieveDemandElement(SUMO_TAG_ROUTE, value);
                // change from attribute using edge ID
                undoList->p_begin("Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, route->getParentEdges().back()->getID(), undoList);
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
GNEWalk::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrieveEdge(value, false) != nullptr);
        case GNE_ATTR_TO_BUSSTOP:
            return (myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_EDGES:
            if (canParse<std::vector<GNEEdge*> >(myNet, value, false)) {
                // all edges exist, then check if compounds a valid route
                return GNERoute::isRouteValid(parse<std::vector<GNEEdge*> >(myNet, value)).empty();
            } else {
                return false;
            }
        case SUMO_ATTR_ROUTE:
            return (myNet->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
        // specific person plan attributes
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
GNEWalk::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNEWalk::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNEWalk::isAttributeEnabled(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_FROM) {
        return (getParentDemandElements().at(0)->getPreviousChildDemandElement(this) == nullptr);
    } else {
        return true;
    }
}


std::string
GNEWalk::getPopUpID() const {
    return getTagStr();
}


std::string
GNEWalk::getHierarchyName() const {
    if (myTagProperty.getTag() == GNE_TAG_WALK_EDGE) {
        return "walk: " + getParentEdges().front()->getID() + " -> " + getParentEdges().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_BUSSTOP) {
        return "walk: " + getParentEdges().front()->getID() + " -> " + getParentAdditionals().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        return "walk: " + getParentEdges().front()->getID() + " ... " + getParentEdges().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        return "walk: " + getParentDemandElements().at(1)->getID();
    } else {
        throw ("Invalid walk tag");
    }
}


const std::map<std::string, std::string>&
GNEWalk::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEWalk::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_FROM:
            // change first edge
            replaceFirstParentEdge(value);
            // compute walk
            computePathElement();
            break;
        case SUMO_ATTR_TO:
            // change last edge
            replaceLastParentEdge(value);
            // compute walk
            computePathElement();
            break;
        case GNE_ATTR_TO_BUSSTOP:
            replaceAdditionalParent(SUMO_TAG_BUS_STOP, value);
            // compute walk
            computePathElement();
            break;
        case SUMO_ATTR_EDGES:
            replaceDemandParentEdges(value);
            // compute walk
            computePathElement();
            break;
        case SUMO_ATTR_ROUTE:
            replaceDemandElementParent(SUMO_TAG_ROUTE, value, 1);
            // compute walk
            computePathElement();
            break;
        // specific person plan attributes
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
GNEWalk::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


void
GNEWalk::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNEWalk::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->p_begin("arrivalPos of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->p_end();
}

/****************************************************************************/
