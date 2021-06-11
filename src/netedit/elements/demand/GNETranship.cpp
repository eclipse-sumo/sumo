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
/// @file    GNETranship.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// A class for visualizing tranships in Netedit
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNETranship.h"
#include "GNERoute.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNETranship::GNETranship(GNENet* net, GNEDemandElement* containerParent, GNEEdge* fromEdge, GNEEdge* toEdge, 
    const double speed, const double departPosition, const double arrivalPosition) :
    GNEDemandElement(containerParent, net, GLO_TRANSHIP, GNE_TAG_TRANSHIP_EDGE,
    {}, {fromEdge, toEdge}, {}, {}, {}, {}, {containerParent}, {}),
    myArrivalPosition(arrivalPosition) {
}


GNETranship::GNETranship(GNENet* net, GNEDemandElement* containerParent, GNEEdge* fromEdge, GNEAdditional* toContainerStop, 
    const double speed, const double departPosition, const double arrivalPosition) :
    GNEDemandElement(containerParent, net, GLO_TRANSHIP, GNE_TAG_TRANSHIP_CONTAINERSTOP,
    {}, {fromEdge}, {}, {toContainerStop}, {}, {}, {containerParent}, {}),
    myArrivalPosition(arrivalPosition) {
}


GNETranship::GNETranship(GNENet* net, GNEDemandElement* containerParent, std::vector<GNEEdge*> edges, 
    const double speed, const double departPosition, const double arrivalPosition) :
    GNEDemandElement(containerParent, net, GLO_TRANSHIP, GNE_TAG_TRANSHIP_EDGES,
    {}, {edges}, {}, {}, {}, {}, {containerParent}, {}),
    myArrivalPosition(arrivalPosition) {
}


GNETranship::~GNETranship() {}


GNEMoveOperation*
GNETranship::getMoveOperation(const double /*shapeOffset*/) {
    // avoid move container plan that ends in containerStop
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
GNETranship::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNETranship::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_TRANSHIP);
    // write attributes depending  of tranship type
    if (myTagProperty.getTag() == GNE_TAG_TRANSHIP_EDGES) {
        device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getParentEdges()));
    } else {
        // check if from attribute is enabled
        if (isAttributeEnabled(SUMO_ATTR_FROM)) {
            device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
        }
        // write to depending if containerplan ends in a containerStop
        if (getParentAdditionals().size() > 0) {
            device.writeAttr(SUMO_ATTR_CONTAINER_STOP, getParentAdditionals().back()->getID());
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
GNETranship::isDemandElementValid() const {
    if (getParentEdges().size() == 2) {
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
GNETranship::getDemandElementProblem() const {
    if (getParentEdges().size() == 0) {
        return ("A tranship need at least one edge");
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
GNETranship::fixDemandElementProblem() {
    // currently the only solution is removing Tranship
}


SUMOVehicleClass
GNETranship::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNETranship::getColor() const {
    return getParentDemandElements().front()->getColor();
}


void
GNETranship::updateGeometry() {
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        i->updateGeometry();
    }
}


Position
GNETranship::getPositionInView() const {
    return Position();
}


std::string
GNETranship::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNETranship::getCenteringBoundary() const {
    Boundary transhipBoundary;
    // return the combination of all parent edges's boundaries
    for (const auto& i : getParentEdges()) {
        transhipBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (transhipBoundary.isInitialised()) {
        return transhipBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNETranship::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // only split geometry of TranshipEdges
    if (myTagProperty.getTag() == GNE_TAG_TRANSHIP_EDGES) {
        // obtain new list of tranship edges
        std::string newTranshipEdges = getNewListOfParents(originalElement, newElement);
        // update tranship edges
        if (newTranshipEdges.size() > 0) {
            setAttribute(SUMO_ATTR_EDGES, newTranshipEdges, undoList);
        }
    }
}


void
GNETranship::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Tranships are drawn in drawPartialGL
}


void
GNETranship::computePathElement() {
    // update lanes depending of tranship tag
    if (myTagProperty.getTag() == GNE_TAG_TRANSHIP_EDGES) {
        // calculate consecutive path using parent edges
        myNet->getPathManager()->calculateConsecutivePathEdges(this, getVClass(), getParentEdges());
    } else {
        // calculate path
        myNet->getPathManager()->calculatePathLanes(this, getVClass(), {getFirstPathLane(), getLastPathLane()});
    }
    // update geometry
    updateGeometry();
}


void
GNETranship::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw container plan over lane
    drawPersonPlanPartial(drawContainerPlan(), s, lane, segment, offsetFront, s.widthSettings.tranship, s.colorSettings.tranship);
}


void
GNETranship::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw container plan over junction
    drawPersonPlanPartial(drawContainerPlan(), s, fromLane, toLane, segment, offsetFront, s.widthSettings.tranship, s.colorSettings.tranship);
}


GNELane*
GNETranship::getFirstPathLane() const {
    return getParentEdges().front()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
}


GNELane*
GNETranship::getLastPathLane() const {
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getParentLanes().front();
    } else {
        return getParentEdges().back()->getLaneByDisallowedVClass(SVC_PEDESTRIAN);
    }
}


std::string
GNETranship::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        // Common container plan attributes
        case SUMO_ATTR_ID:
            return getParentDemandElements().front()->getID();
        case SUMO_ATTR_FROM:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_TO:
            return getParentEdges().back()->getID();
        case GNE_ATTR_TO_CONTAINERSTOP:
            return getParentAdditionals().back()->getID();
        case SUMO_ATTR_EDGES:
            return parseIDs(getParentEdges());
        // specific container plan attributes
        case SUMO_ATTR_SPEED:
            return toString(mySpeed);
        case SUMO_ATTR_DEPARTPOS:
            if (myDepartPosition == -1) {
                return "";
            } else {
                return toString(myDepartPosition);
            }
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
GNETranship::getAttributeDouble(SumoXMLAttr key) const {
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
GNETranship::getAttributePosition(SumoXMLAttr key) const {
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
GNETranship::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        // Common container plan attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_ARRIVALPOS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        // special case for "to" attributes
        case SUMO_ATTR_TO: {
            // get next containerPlan
            GNEDemandElement* nextContainerPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextContainerPlan
            if (nextContainerPlan) {
                undoList->p_begin("Change from attribute of next containerPlan");
                nextContainerPlan->setAttribute(SUMO_ATTR_FROM, value, undoList);
                undoList->p_add(new GNEChange_Attribute(this, key, value));
                undoList->p_end();
            } else {
                undoList->p_add(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        case GNE_ATTR_TO_CONTAINERSTOP: {
            // get next container plan
            GNEDemandElement* nextContainerPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextContainerPlan
            if (nextContainerPlan) {
                // obtain containerStop
                const GNEAdditional* containerStop = myNet->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value);
                // change from attribute using edge ID
                undoList->p_begin("Change from attribute of next containerPlan");
                nextContainerPlan->setAttribute(SUMO_ATTR_FROM, containerStop->getParentLanes().front()->getParentEdge()->getID(), undoList);
                undoList->p_add(new GNEChange_Attribute(this, key, value));
                undoList->p_end();
            } else {
                undoList->p_add(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        case SUMO_ATTR_EDGES: {
            // get next container plan
            GNEDemandElement* nextContainerPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextContainerPlan
            if (nextContainerPlan) {
                // obtain edges
                const std::vector<GNEEdge*> edges = parse<std::vector<GNEEdge*> >(myNet, value);
                // change from attribute using edge ID
                undoList->p_begin("Change from attribute of next containerPlan");
                nextContainerPlan->setAttribute(SUMO_ATTR_FROM, edges.back()->getID(), undoList);
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
GNETranship::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common container plan attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrieveEdge(value, false) != nullptr);
        case GNE_ATTR_TO_CONTAINERSTOP:
            return (myNet->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value, false) != nullptr);
        case SUMO_ATTR_EDGES:
            if (canParse<std::vector<GNEEdge*> >(myNet, value, false)) {
                // all edges exist, then check if compounds a valid route
                return GNERoute::isRouteValid(parse<std::vector<GNEEdge*> >(myNet, value)).empty();
            } else {
                return false;
            }
        // specific container plan attributes
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_DEPARTPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                const double parsedValue = canParse<double>(value);
                if ((parsedValue < 0) || (parsedValue > getFirstPathLane()->getLaneShape().length())) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
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
GNETranship::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNETranship::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNETranship::isAttributeEnabled(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_FROM) {
        return (getParentDemandElements().at(0)->getPreviousChildDemandElement(this) == nullptr);
    } else {
        return true;
    }
}


std::string
GNETranship::getPopUpID() const {
    return getTagStr();
}


std::string
GNETranship::getHierarchyName() const {
    if (myTagProperty.getTag() == GNE_TAG_TRANSHIP_EDGE) {
        return "tranship: " + getParentEdges().front()->getID() + " -> " + getParentEdges().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_TRANSHIP_CONTAINERSTOP) {
        return "tranship: " + getParentEdges().front()->getID() + " -> " + getParentAdditionals().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_TRANSHIP_EDGES) {
        return "tranship: " + getParentEdges().front()->getID() + " ... " + getParentEdges().back()->getID();
    } else {
        throw ("Invalid tranship tag");
    }
}


const std::map<std::string, std::string>&
GNETranship::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNETranship::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common container plan attributes
        case SUMO_ATTR_FROM:
            // change first edge
            replaceFirstParentEdge(value);
            // compute tranship
            computePathElement();
            break;
        case SUMO_ATTR_TO:
            // change last edge
            replaceLastParentEdge(value);
            // compute tranship
            computePathElement();
            break;
        case GNE_ATTR_TO_CONTAINERSTOP:
            replaceAdditionalParent(SUMO_TAG_CONTAINER_STOP, value);
            // compute tranship
            computePathElement();
            break;
        case SUMO_ATTR_EDGES:
            replaceDemandParentEdges(value);
            // compute tranship
            computePathElement();
            break;
        // specific container plan attributes
        case SUMO_ATTR_SPEED:
            mySpeed = parse<double>(value);
            break;
        case SUMO_ATTR_DEPARTPOS:
            if (value.empty()) {
                myDepartPosition = -1;
            } else {
                myDepartPosition = parse<double>(value);
            }
            updateGeometry();
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
GNETranship::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


void
GNETranship::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNETranship::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->p_begin("arrivalPos of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->p_end();
}

/****************************************************************************/
