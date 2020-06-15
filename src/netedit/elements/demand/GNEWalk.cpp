/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEWalk.h"
#include "GNERoute.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, double arrivalPosition) :
    GNEDemandElement(net->generateDemandElementID(GNE_TAG_WALK_EDGE_EDGE), net, GLO_WALK, GNE_TAG_WALK_EDGE_EDGE,
        {}, {fromEdge, toEdge}, {}, {}, {}, {}, {personParent}, {}, // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                            // Childrens
    myArrivalPosition(arrivalPosition) {
    // compute walk
    computePath();
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* toBusStop, double arrivalPosition) :
    GNEDemandElement(net->generateDemandElementID(GNE_TAG_WALK_EDGE_BUSSTOP), net, GLO_WALK, GNE_TAG_WALK_EDGE_BUSSTOP,
        {}, {fromEdge}, {}, {toBusStop}, {}, {}, {personParent}, {},    // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                                // Childrens
    myArrivalPosition(arrivalPosition) {
    // compute walk
    computePath();
}

GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEAdditional* fromBusStop, GNEEdge* toEdge, double arrivalPosition) :
    GNEDemandElement(net->generateDemandElementID(GNE_TAG_WALK_BUSSTOP_EDGE), net, GLO_WALK, GNE_TAG_WALK_BUSSTOP_EDGE,
        {}, {toEdge}, {}, {fromBusStop}, {}, {}, {personParent}, {},    // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                                // Childrens
    myArrivalPosition(arrivalPosition) {
    // compute walk
    computePath();
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEAdditional* fromBusStop, GNEAdditional* toBusStop, double arrivalPosition) :
    GNEDemandElement(net->generateDemandElementID(GNE_TAG_WALK_BUSSTOP_BUSSTOP), net, GLO_WALK, GNE_TAG_WALK_BUSSTOP_BUSSTOP,
        {}, {}, {}, {fromBusStop, toBusStop}, {}, {}, {personParent}, {},   // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                                    // Childrens
    myArrivalPosition(arrivalPosition) {
    // compute walk
    computePath();
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, std::vector<GNEEdge*> edges, double arrivalPosition) :
    GNEDemandElement(net->generateDemandElementID(GNE_TAG_WALK_EDGES), net, GLO_WALK, GNE_TAG_WALK_EDGES,
        {}, {edges}, {}, {}, {}, {}, {personParent}, {},    // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                    // Childrens
    myArrivalPosition(arrivalPosition) {
    // compute walk
    computePath();
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEDemandElement* route, double arrivalPosition) :
    GNEDemandElement(net->generateDemandElementID(GNE_TAG_WALK_ROUTE), net, GLO_WALK, GNE_TAG_WALK_ROUTE,
        {}, {}, {}, {}, {}, {}, {personParent, route}, {},  // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                    // Childrens
    myArrivalPosition(arrivalPosition) {
    // compute walk
    computePath();
}


GNEWalk::~GNEWalk() {}


GUIGLObjectPopupMenu*
GNEWalk::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " name to clipboard").c_str(), nullptr, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " typed name to clipboard").c_str(), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
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
        // check if we have to write "from" attributes
        if (getParentDemandElements().at(0)->getPreviousChildDemandElement(this) == nullptr) {
            // write "to" attributes depending of start and end
            if (myTagProperty.personPlanStartEdge()) {
                device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
            } else if (myTagProperty.personPlanStartBusStop()) {
                device.writeAttr(SUMO_ATTR_FROM, getParentAdditionals().front()->getID());
            }
        }
        // write "to" attributes depending of start and end
        if (myTagProperty.personPlanStartEdge()) {
            device.writeAttr(SUMO_ATTR_TO, getParentEdges().back()->getID());
        } else if (myTagProperty.personPlanStartBusStop()) {
            device.writeAttr(SUMO_ATTR_BUS_STOP, getParentAdditionals().back()->getID());
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
            return (myNet->getPathCalculator()->calculatePath(getParentDemandElements().at(0)->getVClass(), getParentEdges()).size() > 0);
        }
    } else if (getPath().size() > 0) {
        // if path edges isn't empty, then there is a valid route
        return true;
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
            if (myNet->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().front()->getVClass(), getParentEdges().at((int)i - 1), getParentEdges().at(i)) == false) {
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


GNEEdge*
GNEWalk::getFromEdge() const {
    if (getParentDemandElements().size() == 2) {
        // obtain position and rotation of first edge route
        return getParentDemandElements().at(1)->getFromEdge();
    } else {
        return getParentEdges().front();
    }
}


GNEEdge*
GNEWalk::getToEdge() const {
    if (getParentDemandElements().size() == 2) {
        // obtain position and rotation of first edge route
        return getParentDemandElements().at(1)->getToEdge();
    } else {
        return getParentEdges().back();
    }
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
GNEWalk::startGeometryMoving() {
    // only start geometry moving if arrival position isn't -1
    if (myArrivalPosition != -1) {
        // always save original position over view
        myWalkMove.originalViewPosition = getPositionInView();
        // save arrival position
        myWalkMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_ARRIVALPOS);
        // save current centering boundary
        myWalkMove.movingGeometryBoundary = getCenteringBoundary();
    }
}


void
GNEWalk::endGeometryMoving() {
    // check that myArrivalPosition isn't -1 and endGeometryMoving was called only once
    if ((myArrivalPosition != -1) && myWalkMove.movingGeometryBoundary.isInitialised()) {
        // reset myMovingGeometryBoundary
        myWalkMove.movingGeometryBoundary.reset();
    }
}


void
GNEWalk::moveGeometry(const Position& offset) {
    // only move if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        // Calculate new position using old position
        Position newPosition = myWalkMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myNet->getViewNet()->snapToActiveGrid(newPosition);
        // obtain lane shape (to improve code legibility)
        const PositionVector& laneShape = getParentEdges().back()->getLanes().front()->getLaneShape();
        // calculate offset lane
        double offsetLane = laneShape.nearest_offset_to_point2D(newPosition, false) - laneShape.nearest_offset_to_point2D(myWalkMove.originalViewPosition, false);
        // Update arrival Position
        myArrivalPosition = parse<double>(myWalkMove.firstOriginalLanePosition) + offsetLane;
        // Update geometry
        updateGeometry();
    }
}


void
GNEWalk::commitGeometryMoving(GNEUndoList* undoList) {
    // only commit geometry moving if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        undoList->p_begin("arrivalPos of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ARRIVALPOS, toString(myArrivalPosition), myWalkMove.firstOriginalLanePosition));
        undoList->p_end();
    }
}


void
GNEWalk::updateGeometry() {
    // declare depart and arrival pos lane
    double departPosLane = -1;
    double arrivalPosLane = -1;
    // declare start and end positions
    Position startPos = Position::INVALID;
    Position endPos = Position::INVALID;
    // calculate person plan start and end positions
    calculatePersonPlanLaneStartEndPos(departPosLane, arrivalPosLane, startPos, endPos);
    // calculate geometry path depending if is a Walk over route
    if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        // calculate edge geometry path using parent route
        GNEGeometry::calculateLaneGeometricPath(this, myDemandElementSegmentGeometry, getParentDemandElements().at(1)->getPath(), departPosLane, arrivalPosLane);
    } else {
        // calculate edge geometry path using path
        GNEGeometry::calculateLaneGeometricPath(this, myDemandElementSegmentGeometry, getPath(), departPosLane, arrivalPosLane);
    }
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        i->updateGeometry();
    }
}


void
GNEWalk::updateDottedContour() {
    //
}


void
GNEWalk::updatePartialGeometry(const GNEEdge* edge) {
    // declare depart and arrival pos lane
    double departPosLane = -1;
    double arrivalPosLane = -1;
    // declare start and end positions
    Position startPos = Position::INVALID;
    Position endPos = Position::INVALID;
    // calculate person plan start and end positions
    calculatePersonPlanLaneStartEndPos(departPosLane, arrivalPosLane, startPos, endPos);
    // udpate geometry path
    GNEGeometry::updateGeometricPath(myDemandElementSegmentGeometry, edge, departPosLane, arrivalPosLane, startPos, endPos);
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        i->updatePartialGeometry(edge);
    }
}


void
GNEWalk::computePath() {
    // update lanes depending of walk tag
    if (myTagProperty.getTag() == GNE_TAG_WALK_EDGE_EDGE) {
        calculatePathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getLastAllowedVehicleLane(), 
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGE_BUSSTOP) {
        calculatePathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getParentAdditionals().back()->getParentLanes().front(), 
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_BUSSTOP_EDGE) {
        calculatePathLanes(getVClass(), true, 
            getParentAdditionals().front()->getParentLanes().front(), 
            getLastAllowedVehicleLane(),
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_BUSSTOP_BUSSTOP) {
        calculatePathLanes(getVClass(), true, 
            getParentAdditionals().front()->getParentLanes().front(), 
            getParentAdditionals().back()->getParentLanes().front(), 
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        calculatePathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getLastAllowedVehicleLane(), 
            getParentEdges());
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        calculatePathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getLastAllowedVehicleLane(), 
            getParentDemandElements().back()->getParentEdges());
    }
    // update geometry
    updateGeometry();
}


void
GNEWalk::invalidatePath() {
    // update lanes depending of walk tag
    if (myTagProperty.getTag() == GNE_TAG_WALK_EDGE_EDGE) {
        resetPathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getLastAllowedVehicleLane(), 
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGE_BUSSTOP) {
        resetPathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getParentAdditionals().back()->getParentLanes().front(), 
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_BUSSTOP_EDGE) {
        resetPathLanes(getVClass(), true, 
            getParentAdditionals().front()->getParentLanes().front(), 
            getLastAllowedVehicleLane(),
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_BUSSTOP_BUSSTOP) {
        resetPathLanes(getVClass(), true, 
            getParentAdditionals().front()->getParentLanes().front(), 
            getParentAdditionals().back()->getParentLanes().front(), 
            {});
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        resetPathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getLastAllowedVehicleLane(), 
            getParentEdges());
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        resetPathLanes(getVClass(), true, 
            getFirstAllowedVehicleLane(), 
            getLastAllowedVehicleLane(), 
            getParentDemandElements().back()->getParentEdges());
    }
    // update geometry
    updateGeometry();
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
    // Walks are drawn in GNEEdges
}


void 
GNEWalk::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane) const {
    // get GNEViewNet
    GNEViewNet* viewNet = lane->getNet()->getViewNet();
    // declare flag to enable or disable draw person plan
    bool drawPersonPlan = false;
    if (viewNet->getDemandViewOptions().showAllPersonPlans()) {
        drawPersonPlan = true;
    } else if (viewNet->getDottedAC() == getParentDemandElements().front()) {
        drawPersonPlan = true;
    } else if (viewNet->getDemandViewOptions().getLockedPerson() == getParentDemandElements().front()) {
        drawPersonPlan = true;
    } else if (viewNet->getDottedAC() && viewNet->getDottedAC()->getTagProperty().isPersonPlan() &&
        (viewNet->getDottedAC()->getAttribute(GNE_ATTR_PARENT) == getAttribute(GNE_ATTR_PARENT))) {
        drawPersonPlan = true;
    }
    // check if draw person plan elements can be drawn
    if (drawPersonPlan) {
        // calculate myDemandElement width
        double myDemandElementWidth = 0;
        // flag to check if width must be duplicated
        bool duplicateWidth = (viewNet->getDottedAC() == this) || (viewNet->getDottedAC() == getParentDemandElements().front()) ? true : false;
        // Set width depending of person plan type
        if (myTagProperty.isPersonTrip()) {
            myDemandElementWidth = s.addSize.getExaggeration(s, lane) * s.widthSettings.personTrip;
        } else if (myTagProperty.isWalk()) {
            myDemandElementWidth = s.addSize.getExaggeration(s, lane) * s.widthSettings.walk;
        } else if (myTagProperty.isRide()) {
            myDemandElementWidth = s.addSize.getExaggeration(s, lane) * s.widthSettings.ride;
        }
        // check if width has to be duplicated
        if (duplicateWidth) {
            myDemandElementWidth *= 2;
        }
        // set myDemandElement color
        RGBColor myDemandElementColor;
        // Set color depending of person plan type
        if (drawUsingSelectColor()) {
            myDemandElementColor = s.colorSettings.selectedPersonPlanColor;
        } else if (myTagProperty.isPersonTrip()) {
            myDemandElementColor = s.colorSettings.personTrip;
        } else if (myTagProperty.isWalk()) {
            myDemandElementColor = s.colorSettings.walk;
        } else if (myTagProperty.isRide()) {
            myDemandElementColor = s.colorSettings.ride;
        }
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType());
        // iterate over segments
        for (const auto& segment : myDemandElementSegmentGeometry) {
            // draw partial segment
            if ((segment.edge == lane->getParentEdge()) && (segment.AC == this)) {
                // Set person plan color (needed due drawShapeDottedContour)
                GLHelper::setColor(myDemandElementColor);
                // draw box line
                GNEGeometry::drawSegmentGeometry(viewNet, segment, myDemandElementWidth);
                // check if shape dotted contour has to be drawn
                if (viewNet->getDottedAC() == this) {
                    GNEGeometry::drawSegmentGeometry(viewNet, segment, myDemandElementWidth);
                }
            }
        }
        // Pop last matrix
        glPopMatrix();

    // Draw name if isn't being drawn for selecting
    if (!s.drawForRectangleSelection) {
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    }
    // Pop name
    glPopName();
    // check if person plan ArrivalPos attribute
    if (myTagProperty.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
        // obtain arrival position using last segment
        const Position& arrivalPos = getDemandElementSegmentGeometry().getLastPosition();
        // only draw arrival position point if isn't -1
        if (arrivalPos != Position::INVALID) {
            // obtain circle width
            const double circleWidth = (duplicateWidth ? SNAP_RADIUS : (SNAP_RADIUS / 2.0)) * MIN2((double)0.5, s.laneWidthExaggeration);
            const double circleWidthSquared = circleWidth * circleWidth;
            if (!s.drawForRectangleSelection || (viewNet->getPositionInformation().distanceSquaredTo2D(arrivalPos) <= (circleWidthSquared + 2))) {
                glPushMatrix();
                // translate to pos and move to upper using GLO_PERSONTRIP (to avoid overlapping)
                glTranslated(arrivalPos.x(), arrivalPos.y(), GLO_PERSONTRIP + 0.01);
                // Set color depending of person plan type
                if (drawUsingSelectColor()) {
                    GLHelper::setColor(s.colorSettings.selectedPersonPlanColor);
                } else if (myTagProperty.isPersonTrip()) {
                    GLHelper::setColor(s.colorSettings.personTrip);
                } else if (myTagProperty.isWalk()) {
                    GLHelper::setColor(s.colorSettings.walk);
                } else if (myTagProperty.isRide()) {
                    GLHelper::setColor(s.colorSettings.ride);
                }
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                glPopMatrix();
                }
            }
        }
    }
    // draw person if this edge correspond to the first edge of first Person's person plan
    const GNEDemandElement* firstPersonPlan = getParentDemandElements().front()->getChildDemandElements().front();
    const GNEEdge* firstEdge = GNERouteHandler::getFirstPersonPlanEdge(firstPersonPlan);
    // draw person parent if this is the edge first edge and this is the first plan
    if ((firstEdge == lane->getParentEdge()) && (firstPersonPlan == this)) {
        getParentDemandElements().front()->drawGL(s);
    }
}


std::string
GNEWalk::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_FROM:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_TO:
            return getParentEdges().back()->getID();
        case GNE_ATTR_FROM_BUSSTOP:
            return getParentAdditionals().front()->getID();
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
                return (getLastAllowedVehicleLane()->getLaneShape().length() - POSITION_EPS);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_TO:
        case GNE_ATTR_FROM_BUSSTOP:
        case GNE_ATTR_TO_BUSSTOP:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_ROUTE:
        // specific person plan attributes
        case SUMO_ATTR_ARRIVALPOS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
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
        case GNE_ATTR_FROM_BUSSTOP:
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
                if ((parsedValue < 0) || (parsedValue > getLastAllowedVehicleLane()->getLaneShape().length())) {
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
GNEWalk::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNEWalk::getPopUpID() const {
    return getTagStr();
}


std::string
GNEWalk::getHierarchyName() const {
    if (myTagProperty.getTag() == GNE_TAG_WALK_EDGE_EDGE) {
        return "walk: " + getParentEdges().front()->getID() + " -> " + getParentEdges().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGE_BUSSTOP) {
        return "walk: " + getParentEdges().front()->getID() + " -> " + getParentAdditionals().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_BUSSTOP_EDGE) {
        return "walk: " + getParentAdditionals().front()->getID() + " -> " + getParentEdges().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_BUSSTOP_BUSSTOP) {
        return "walk: " + getParentAdditionals().front()->getID() + " -> " + getParentAdditionals().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        return "walk: " + getParentEdges().front()->getID() + " ... " + getParentEdges().back()->getID();
    } else if (myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
        return "walk: " + getParentDemandElements().at(1)->getID();
    } else {
        throw ("Invalid walk tag");
    }
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
            replaceFirstParentEdge(this, myNet->retrieveEdge(value));
            // compute person trip
            computePath();
            break;
        case SUMO_ATTR_TO:
            // change last edge
            replaceLastParentEdge(this, myNet->retrieveEdge(value));
            // compute person trip
            computePath();
            break;
        case GNE_ATTR_FROM_BUSSTOP:
            replaceParentAdditional(this, value, 0);
            // compute person trip
            computePath();
            break;
        case GNE_ATTR_TO_BUSSTOP:
            // -> check this
            if (getParentAdditionals().size() > 1) {
                replaceParentAdditional(this, value, 1);
            } else {
                replaceParentAdditional(this, value, 0);
            }
            // compute person trip
            computePath();
            break;
        case SUMO_ATTR_EDGES:
            replaceParentEdges(this, value);
            updateGeometry();
            break;
        case SUMO_ATTR_ROUTE:
            replaceParentDemandElement(this, value, 1);
            updateGeometry();
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


/****************************************************************************/
