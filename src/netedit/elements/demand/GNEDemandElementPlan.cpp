/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNEDemandElementPlan.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// An auxiliar, asbtract class for plan elements
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEDemandElementPlan.h"
#include "GNERoute.h"

// ===========================================================================
// static definitions
// ===========================================================================

const double GNEDemandElementPlan::myPersonPlanArrivalPositionDiameter = SUMO_const_halfLaneWidth;

// ===========================================================================
// GNEDemandElement method definitions
// ===========================================================================

GNEDemandElementPlan::GNEDemandElementPlan(GNEDemandElement* planElement) :
    myPlanElement(planElement) {
}


std::string
GNEDemandElementPlan::getPlanAttribute(SumoXMLAttr key) const {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_ID:
        case GNE_ATTR_PARENT:
            return myPlanElement->getParentDemandElements().front()->getID();
        // edges
        case SUMO_ATTR_FROM:
            /*****/
            if (myPlanElement->myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
                return myPlanElement->getParentDemandElements().at(1)->getParentEdges().front()->getID();
            } else {
                return myPlanElement->getParentEdges().front()->getID();
            }
        case SUMO_ATTR_TO:
            /*****/
            if (myPlanElement->myTagProperty.getTag() == GNE_TAG_WALK_ROUTE) {
                return myPlanElement->getParentDemandElements().at(1)->getParentEdges().back()->getID();
            } else {
                return myPlanElement->getParentEdges().back()->getID();
            }
        case SUMO_ATTR_EDGES:
            return myPlanElement->parseIDs(myPlanElement->getParentEdges());
        // junctions
        case SUMO_ATTR_FROMJUNCTION:
            return myPlanElement->getParentJunctions().front()->getID();
        case SUMO_ATTR_TOJUNCTION:
            return myPlanElement->getParentJunctions().back()->getID();
        // additionals
        case SUMO_ATTR_FROM_TAZ:
            return myPlanElement->getParentAdditionals().front()->getID();
            return myPlanElement->getParentAdditionals().back()->getID();
        case GNE_ATTR_TO_BUSSTOP:
        case GNE_ATTR_TO_TRAINSTOP:
        case SUMO_ATTR_TO_TAZ:
            return myPlanElement->getParentAdditionals().back()->getID();
        // route
        case SUMO_ATTR_ROUTE:
            return myPlanElement->getParentDemandElements().at(1)->getID();
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEDemandElementPlan::getPlanAttributeDouble(SumoXMLAttr key, const double arrivalPosition) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
            if (myPlanElement->getParentJunctions().size() > 0) {
                return 0;
            } else if (arrivalPosition != -1) {
                return arrivalPosition;
            } else {
                return (myPlanElement->getLastPathLane()->getLaneShape().length() - POSITION_EPS);
            }
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have a doubleattribute of type '" + toString(key) + "'");
    }
}


Position
GNEDemandElementPlan::getPlanAttributePosition(SumoXMLAttr key, const double arrivalPosition) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS: {
            if (myPlanElement->getParentJunctions().size() > 0) {
                return myPlanElement->getParentJunctions().back()->getPositionInView();
            } else {
                // get lane shape
                const PositionVector& laneShape = myPlanElement->getLastPathLane()->getLaneShape();
                // continue depending of arrival position
                if (arrivalPosition == 0) {
                    return laneShape.front();
                } else if ((arrivalPosition == -1) || (arrivalPosition >= laneShape.length2D())) {
                    return laneShape.back();
                } else {
                    return laneShape.positionAtOffset2D(arrivalPosition);
                }
            }
        }
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have a position attribute of type '" + toString(key) + "'");
    }
}


void
GNEDemandElementPlan::setPlanAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // declare plan parent
    const auto planParent = myPlanElement->getParentDemandElements().at(0);
    // declare ACs
    const auto &ACs = myPlanElement->getNet()->getAttributeCarriers();
    // continue depending of key
    switch (key) {
        // parent
        case GNE_ATTR_PARENT:
            GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            break;
        // from attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_FROMJUNCTION:
        case SUMO_ATTR_FROM_TAZ:
            // plans placed over routes cannot change their from attribute
            if (!myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            }

/// CHECK IF MODIFY TO ATTRIBUTE OF PREVIOUS PERSON PLAN

            break;
        // to attributes
        case SUMO_ATTR_TO:
        case SUMO_ATTR_TOJUNCTION:
        case SUMO_ATTR_TO_TAZ: {
            // plans placed over routes cannot change their to attribute
            if (!myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
                // get next personPlan
                auto nextPersonPlan = planParent->getNextChildDemandElement(myPlanElement);
                // check if change to attribute of this plan and from attribute of the next plan
                if (nextPersonPlan) {
                    undoList->begin(myPlanElement, "Change from attribute of next personPlan");
                    nextPersonPlan->setAttribute(SUMO_ATTR_FROM, value, undoList);
                    GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
                    undoList->end();
                } else {
                    GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
                }
            }
            break;
        }
        case GNE_ATTR_TO_BUSSTOP: {
            // get next person plan
            auto nextPersonPlan = planParent->getNextChildDemandElement(myPlanElement);
            // check if change to attribute of this plan and from attribute of the next plan
            if (nextPersonPlan) {
                // obtain busStop
                const GNEAdditional* busStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, value);
                // change from attribute using edge ID
                undoList->begin(myPlanElement, "Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, busStop->getParentLanes().front()->getParentEdge()->getID(), undoList);
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
                undoList->end();
            } else {
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            }
            break;
        }
        case GNE_ATTR_TO_TRAINSTOP: {
            // get next person plan
            auto nextPersonPlan = planParent->getNextChildDemandElement(myPlanElement);
            // check if change to attribute of this plan and from attribute of the next plan
            if (nextPersonPlan) {
                // obtain trainStop
                const GNEAdditional* trainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, value);
                // change from attribute using edge ID
                undoList->begin(myPlanElement, "Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, trainStop->getParentLanes().front()->getParentEdge()->getID(), undoList);
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
                undoList->end();
            } else {
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            }
            break;
        }
        case SUMO_ATTR_EDGES: {
            // get next person plan
            auto nextPersonPlan = planParent->getNextChildDemandElement(myPlanElement);
            // check if change to attribute of this plan and from attribute of the next plan
            if (nextPersonPlan) {
                // obtain edges
                const auto edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myPlanElement->getNet(), value);
                // change from attribute using edge ID
                undoList->begin(myPlanElement, "Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, edges.back()->getID(), undoList);
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
                undoList->end();
            } else {
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            }
            break;
        }
        case SUMO_ATTR_ROUTE: {
            // get next person plan
            auto nextPersonPlan = planParent->getNextChildDemandElement(myPlanElement);
            // check if change to attribute of this plan and from attribute of the next plan
            if (nextPersonPlan) {
                // obtain route
                const GNEDemandElement* route = ACs->retrieveDemandElement(SUMO_TAG_ROUTE, value);
                // change from attribute using edge ID
                undoList->begin(myPlanElement, "Change from attribute of next personPlan");
                nextPersonPlan->setAttribute(SUMO_ATTR_FROM, route->getParentEdges().back()->getID(), undoList);
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
                undoList->end();
            } else {
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            }
            break;
        }
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDemandElementPlan::isPlanValid(SumoXMLAttr key, const std::string& value) {
    // declare ACs
    const auto &ACs = myPlanElement->getNet()->getAttributeCarriers();
    // continue depending of key
    switch (key) {
        // parent
        case GNE_ATTR_PARENT:
///// CHECK FOR CONTAINERS

            if (ACs->retrieveDemandElement(SUMO_TAG_PERSON, value, false) != nullptr) {
                return true;
            } else if (ACs->retrieveDemandElement(SUMO_TAG_PERSONFLOW, value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        // edges
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            if (!myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
                return (ACs->retrieveEdge(value, false) != nullptr);
            } else {
                return false;
            }
        case SUMO_ATTR_EDGES:
            if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myPlanElement->getNet(), value, false)) {
                // all edges exist, then check if compounds a valid route
                return GNERoute::isRouteValid(GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myPlanElement->getNet(), value)).empty();
            } else {
                return false;
            }
        // junctions
        case SUMO_ATTR_FROMJUNCTION:
        case SUMO_ATTR_TOJUNCTION:
            return (ACs->retrieveJunction(value, false) != nullptr);
        // TAZs
        case SUMO_ATTR_FROM_TAZ:
        case SUMO_ATTR_TO_TAZ:
            return (ACs->retrieveAdditional(SUMO_TAG_TAZ, value, false) != nullptr);
        // busStop
        case GNE_ATTR_TO_BUSSTOP:
            return (ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        // trainStop
        case GNE_ATTR_TO_TRAINSTOP:
            return (ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, value, false) != nullptr);
        // route
        case SUMO_ATTR_ROUTE:
            return (ACs->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDemandElementPlan::isPlanAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_FROMJUNCTION:
        case SUMO_ATTR_FROM_TAZ:
            return (myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement) == nullptr);
        default:
            return true;
    }
}


std::string
GNEDemandElementPlan::getPlanHierarchyName() const {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // declare tagStr
    const auto tagStr = myPlanElement->getTagStr() + ": ";
    // continue depending of attributes
    if (tagProperty.hasAttribute(SUMO_ATTR_EDGES)) {
        // edges
        return tagStr + myPlanElement->getParentEdges().front()->getID() + " ... " + myPlanElement->getParentEdges().back()->getID();
    } else if (tagProperty.hasAttribute(SUMO_ATTR_ROUTE)) {
        // route
        return tagStr + myPlanElement->getParentDemandElements().at(1)->getID();
    } else if (myPlanElement->getParentEdges().size() == 2) {
        // edge -> edge
        return tagStr + myPlanElement->getParentEdges().front()->getID() + " -> " + myPlanElement->getParentEdges().back()->getID();
    } else if (myPlanElement->getParentJunctions().size() == 2) {
        // junction -> Junction
        return tagStr + myPlanElement->getParentJunctions().front()->getID() + " -> " + myPlanElement->getParentJunctions().back()->getID();
    } else if (myPlanElement->getParentAdditionals().size() == 2) {
        // additional -> additional
        return tagStr + myPlanElement->getParentAdditionals().front()->getID() + " -> " + myPlanElement->getParentAdditionals().back()->getID();
    } else if ((myPlanElement->getParentEdges().size() == 1) && (myPlanElement->getParentAdditionals().size() == 1)) {
        // edge -> additional
        return tagStr + myPlanElement->getParentJunctions().front()->getID() + " -> " + myPlanElement->getParentJunctions().back()->getID();
    } else {
        throw ProcessError("Invalid plan configuration");
    }
}


bool
GNEDemandElementPlan::drawPersonPlan() const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // check conditions
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() &&
        viewNet->getNetworkViewOptions().showDemandElements() &&
        viewNet->getDemandViewOptions().showAllPersonPlans()) {
        // show all person plans in network mode
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() &&
               viewNet->getDemandViewOptions().showAllPersonPlans()) {
        // show all person plans
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() && myPlanElement->isAttributeCarrierSelected()) {
        // show selected
        return true;
    } else if (viewNet->isAttributeCarrierInspected(myPlanElement->getParentDemandElements().front())) {
        // person parent is inspected
        return true;
    } else if (viewNet->getDemandViewOptions().getLockedPerson() == myPlanElement->getParentDemandElements().front()) {
        // person parent is locked
        return true;
    } else if (viewNet->getInspectedAttributeCarriers().empty()) {
        // nothing is inspected
        return false;
    } else {
        // get inspected AC
        const GNEAttributeCarrier* AC = viewNet->getInspectedAttributeCarriers().front();
        // check condition
        if (AC->getTagProperty().isPersonPlan() && AC->getAttribute(GNE_ATTR_PARENT) == myPlanElement->getAttribute(GNE_ATTR_PARENT)) {
            // common person parent
            return true;
        } else {
            // all conditions are false
            return false;
        }
    }
}


bool
GNEDemandElementPlan::drawContainerPlan() const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // check conditions
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() &&
        viewNet->getNetworkViewOptions().showDemandElements() &&
        viewNet->getDemandViewOptions().showAllContainerPlans()) {
        // show all container plans in network mode
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() &&
               viewNet->getDemandViewOptions().showAllContainerPlans()) {
        // show all container plans
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() && myPlanElement->isAttributeCarrierSelected()) {
        // show selected
        return true;
    } else if (viewNet->isAttributeCarrierInspected(myPlanElement->getParentDemandElements().front())) {
        // container parent is inspected
        return true;
    } else if (viewNet->getDemandViewOptions().getLockedContainer() == myPlanElement->getParentDemandElements().front()) {
        // container parent is locked
        return true;
    } else if (viewNet->getInspectedAttributeCarriers().empty()) {
        // nothing is inspected
        return false;
    } else {
        // get inspected AC
        const GNEAttributeCarrier* AC = viewNet->getInspectedAttributeCarriers().front();
        // check condition
        if (AC->getTagProperty().isContainerPlan() && AC->getAttribute(GNE_ATTR_PARENT) == myPlanElement->getAttribute(GNE_ATTR_PARENT)) {
            // common container parent
            return true;
        } else {
            // all conditions are false
            return false;
        }
    }
}


void
GNEDemandElementPlan::drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment,
                                        const double offsetFront, const double personPlanWidth, const RGBColor& personPlanColor) const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // get inspected and front flags
    const bool dottedElement = viewNet->isAttributeCarrierInspected(myPlanElement) || (viewNet->getFrontAttributeCarrier() == myPlanElement);
    // get person parent
    const GNEDemandElement* personParent = myPlanElement->getParentDemandElements().front();
    // check if draw person plan element can be drawn
    if ((personPlanColor.alpha() != 0) && drawPlan && myPlanElement->getNet()->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, lane, myPlanElement->getTagProperty().getTag())) {
        // get inspected attribute carriers
        const auto& inspectedACs = viewNet->getInspectedAttributeCarriers();
        // get inspected person plan
        const GNEAttributeCarrier* personPlanInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (personPlanInspected == myPlanElement) || (personPlanInspected == personParent);
        // calculate path width
        const double pathWidth = s.addSize.getExaggeration(s, lane) * personPlanWidth * (duplicateWidth ? 2 : 1);
        // declare path geometry
        GUIGeometry personPlanGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                              myPlanElement->getPathElementDepartValue(), myPlanElement->getPathElementArrivalValue(),    // extrem positions
                                              myPlanElement->getPathElementDepartPos(), myPlanElement->getPathElementArrivalPos());       // extra positions
        } else if (segment->isFirstSegment()) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                              myPlanElement->getPathElementDepartValue(), -1,                 // extrem positions
                                              myPlanElement->getPathElementDepartPos(), Position::INVALID);   // extra positions
        } else if (segment->isLastSegment()) {
            personPlanGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                              -1, myPlanElement->getPathElementArrivalValue(),                // extrem positions
                                              Position::INVALID, myPlanElement->getPathElementArrivalPos());  // extra positions
        } else {
            personPlanGeometry = lane->getLaneGeometry();
        }
        // get color
        const RGBColor& pathColor = myPlanElement->drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : personPlanColor;
        // Start drawing adding an gl identificator
        GLHelper::pushName(myPlanElement->getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, myPlanElement->getType(), offsetFront);
        // Set color
        GLHelper::setColor(pathColor);
        // draw geometry
        GUIGeometry::drawGeometry(s, viewNet->getPositionInformation(), personPlanGeometry, pathWidth);
        // Pop last matrix
        GLHelper::popMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            myPlanElement->drawName(myPlanElement->getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // check if myPlanElement is the last segment
        if (segment->isLastSegment()) {
            // calculate circle width
            const double circleRadius = (duplicateWidth ? myPersonPlanArrivalPositionDiameter : (myPersonPlanArrivalPositionDiameter / 2.0));
            const double circleWidth = circleRadius * MIN2((double)0.5, s.laneWidthExaggeration);
            const double circleWidthSquared = circleWidth * circleWidth;
            // get geometryEndPos
            const Position geometryEndPos = myPlanElement->getPathElementArrivalPos();
            // check if endPos can be drawn
            if (!s.drawForRectangleSelection || (viewNet->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= (circleWidthSquared + 2))) {
                // push draw matrix
                GLHelper::pushMatrix();
                // Start with the drawing of the area traslating matrix to origin
                viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, myPlanElement->getType());
                // translate to pos and move to upper using GLO_PERSONTRIP (to avoid overlapping)
                glTranslated(geometryEndPos.x(), geometryEndPos.y(), 0);
                // Set person plan color
                GLHelper::setColor(pathColor);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                // pop draw matrix
                GLHelper::popMatrix();
            }
        }
        // check if we have to draw an red arrow or line
        if (segment->getNextSegment() && segment->getNextSegment()->getLane()) {
            // get firstPosition (last position of current lane shape)
            const Position from = lane->getLaneShape().back();
            // get lastPosition (first position of next lane shape)
            const Position to = segment->getNextSegment()->getLane()->getLaneShape().front();
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, myPlanElement->getType());
            // draw child line
            GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || myPlanElement->isAttributeCarrierSelected(), .05);
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // check if we have to draw an red arrow or line
        if (segment->getPreviousSegment() && segment->getPreviousSegment()->getLane()) {
            // get firstPosition (last position of current lane shape)
            const Position from = lane->getLaneShape().front();
            // get lastPosition (first position of next lane shape)
            const Position to = segment->getPreviousSegment()->getLane()->getLaneShape().back();
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, myPlanElement->getType());
            // draw child line
            GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || myPlanElement->isAttributeCarrierSelected(), .05);
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // Pop name
        GLHelper::popName();
        // declare trim geometry to draw
        const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? personPlanGeometry.getShape() : lane->getLaneShape();
        // check if mouse is over element
        myPlanElement->mouseWithinGeometry(shape, pathWidth);
        // check if shape dotted contour has to be drawn
        if (dottedElement) {
            // inspect contour
            if (viewNet->isAttributeCarrierInspected(myPlanElement)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // front element contour
            if (viewNet->getFrontAttributeCarrier() == myPlanElement) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // delete contour
            if (viewNet->drawDeleteContour(myPlanElement, myPlanElement)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // select contour
            if (viewNet->drawSelectContour(myPlanElement, myPlanElement)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, shape, pathWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
        }
    }
    // draw person parent if myPlanElement is the edge first edge and myPlanElement is the first plan
    if (myPlanElement->getParentJunctions().empty() && (myPlanElement->getFirstPathLane()->getParentEdge() == lane->getParentEdge()) &&
            (personParent->getChildDemandElements().front() == myPlanElement)) {
        personParent->drawGL(s);
    }
}


void
GNEDemandElementPlan::drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/,
                                        const double offsetFront, const double personPlanWidth, const RGBColor& personPlanColor) const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // get inspected and front flags
    const bool dottedElement = viewNet->isAttributeCarrierInspected(myPlanElement) || (viewNet->getFrontAttributeCarrier() == myPlanElement);
    // check if draw person plan elements can be drawn
    if ((personPlanColor.alpha() != 0) && drawPlan && myPlanElement->getNet()->getPathManager()->getPathDraw()->drawPathGeometry(false, fromLane, toLane, myPlanElement->getTagProperty().getTag())) {
        // get inspected attribute carriers
        const auto& inspectedACs = viewNet->getInspectedAttributeCarriers();
        // get person parent
        const GNEDemandElement* personParent = myPlanElement->getParentDemandElements().front();
        // get inspected person plan
        const GNEAttributeCarrier* personPlanInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (personPlanInspected == myPlanElement) || (personPlanInspected == personParent);
        // calculate path width
        const double pathWidth = s.addSize.getExaggeration(s, fromLane) * personPlanWidth * (duplicateWidth ? 2 : 1);
        // get color
        const RGBColor& color = myPlanElement->drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : personPlanColor;
        // Start drawing adding an gl identificator
        GLHelper::pushName(myPlanElement->getGlID());
        // push a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, myPlanElement->getType(), offsetFront);
        // check if draw lane2lane connection or a red line
        if (fromLane && fromLane->getLane2laneConnections().exist(toLane)) {
            // obtain lane2lane geometry
            const GUIGeometry& lane2laneGeometry = fromLane->getLane2laneConnections().getLane2laneGeometry(toLane);
            // Set person plan color
            GLHelper::setColor(color);
            // draw lane2lane
            GUIGeometry::drawGeometry(s, viewNet->getPositionInformation(), lane2laneGeometry, pathWidth);
        } else {
            // Set invalid person plan color
            GLHelper::setColor(RGBColor::RED);
            // draw line between end of first shape and first position of second shape
            GLHelper::drawBoxLines({fromLane->getLaneShape().back(), toLane->getLaneShape().front()}, (0.5 * pathWidth));
        }
        // Pop last matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(myPlanElement, myPlanElement->getType(), myPlanElement->getPositionInView(), 0.5);
        // check if shape dotted contour has to be drawn
        if (fromLane->getLane2laneConnections().exist(toLane) && dottedElement) {
            // check if mouse is over element
            myPlanElement->mouseWithinGeometry(fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(), pathWidth);
            // inspect contour
            if (viewNet->isAttributeCarrierInspected(myPlanElement)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
            // front contour
            if (viewNet->getFrontAttributeCarrier() == myPlanElement) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
            // delete contour
            if (viewNet->drawDeleteContour(myPlanElement, myPlanElement)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
            // select contour
            if (viewNet->drawSelectContour(myPlanElement, myPlanElement)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        pathWidth, 1, false, false);
            }
        }
    }
}


GNEDemandElement::Problem
GNEDemandElementPlan::isPersonPlanValid() const {
    // get previous child
    const auto previousChild = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
    if (previousChild) {
        // get previous edge
        GNEEdge* previousEdge = nullptr;
        if (previousChild->getParentLanes().size() == 1) {
            previousEdge = previousChild->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentAdditionals().size() == 1) {
            previousEdge = previousChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentEdges().size() > 0) {
            previousEdge = previousChild->getParentEdges().back();
        } else if (previousChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            previousEdge = previousChild->getParentDemandElements().at(1)->getParentEdges().back();
        }
        // get first edge
        GNEEdge* firstEdge = nullptr;
        // check edge
        if (myPlanElement->getParentLanes().size() == 1) {
            firstEdge = myPlanElement->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getParentEdges().size() > 0) {
            firstEdge = myPlanElement->getParentEdges().front();
        } else if (myPlanElement->getParentAdditionals().size() == 1) {
            firstEdge = myPlanElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            firstEdge = myPlanElement->getParentDemandElements().at(1)->getParentEdges().front();
        }
        // check junctions
        if ((previousChild->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            if (previousChild->getParentJunctions().back() != myPlanElement->getParentJunctions().front()) {
                return GNEDemandElement::Problem::DISCONNECTED_PLAN;
            }
        } else if (previousEdge && (myPlanElement->getParentJunctions().size() > 0)) {
            if (previousEdge->getToJunction() != myPlanElement->getParentJunctions().front()) {
                return GNEDemandElement::Problem::DISCONNECTED_PLAN;
            }
        } else if (previousEdge != firstEdge) {
            return GNEDemandElement::Problem::DISCONNECTED_PLAN;
        }
    }
    // get next child
    const auto nextChild = myPlanElement->getParentDemandElements().at(0)->getNextChildDemandElement(myPlanElement);
    if (nextChild) {
        // get previous edge
        GNEEdge* nextEdge = nullptr;
        if (nextChild->getParentLanes().size() == 1) {
            nextEdge = nextChild->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getParentEdges().size() > 0) {
            nextEdge = nextChild->getParentEdges().front();
        } else if (nextChild->getParentAdditionals().size() == 1) {
            nextEdge = nextChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            nextEdge = nextChild->getParentDemandElements().at(1)->getParentEdges().front();
        }
        // get last edge
        GNEEdge* lastEdge = nullptr;
        // check edge
        if (myPlanElement->getParentLanes().size() == 1) {
            lastEdge = myPlanElement->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getParentAdditionals().size() == 1) {
            lastEdge = myPlanElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getParentEdges().size() > 0) {
            lastEdge = myPlanElement->getParentEdges().back();
        } else if (myPlanElement->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            lastEdge = myPlanElement->getParentDemandElements().at(1)->getParentEdges().back();
        }
        // compare both edges
        if ((nextChild->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            if (nextChild->getParentJunctions().front() != myPlanElement->getParentJunctions().back()) {
                return GNEDemandElement::Problem::DISCONNECTED_PLAN;
            }
        } else if (nextEdge && (myPlanElement->getParentJunctions().size() > 0)) {
            if (nextEdge->getFromJunction() != myPlanElement->getParentJunctions().back()) {
                return GNEDemandElement::Problem::DISCONNECTED_PLAN;
            }
        } else if (lastEdge && (nextChild->getParentJunctions().size() > 0)) {
            if (lastEdge->getToJunction() != nextChild->getParentJunctions().front()) {
                return GNEDemandElement::Problem::DISCONNECTED_PLAN;
            }
        } else if (nextEdge != lastEdge) {
            return GNEDemandElement::Problem::DISCONNECTED_PLAN;
        }
    }
    // all ok, then return true
    return GNEDemandElement::Problem::OK;
}


std::string
GNEDemandElementPlan::getPersonPlanProblem() const {
    // get previous child
    const auto previousChild = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
    if (previousChild) {
        // get previous edge
        GNEEdge* previousEdge = nullptr;
        if (previousChild->getParentLanes().size() == 1) {
            previousEdge = previousChild->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentAdditionals().size() == 1) {
            previousEdge = previousChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (previousChild->getParentEdges().size() > 0) {
            previousEdge = previousChild->getParentEdges().back();
        } else if (previousChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            previousEdge = previousChild->getParentDemandElements().at(1)->getParentEdges().back();
        }
        // get first edge
        GNEEdge* firstEdge = nullptr;
        // check edge
        if (myPlanElement->getParentLanes().size() == 1) {
            firstEdge = myPlanElement->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getParentEdges().size() > 0) {
            firstEdge = myPlanElement->getParentEdges().front();
        } else if (myPlanElement->getParentAdditionals().size() == 1) {
            firstEdge = myPlanElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            firstEdge = myPlanElement->getParentDemandElements().at(1)->getParentEdges().front();
        }
        // compare elements
        if ((previousChild->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("Junction '" + previousChild->getParentJunctions().back()->getID() +
                    "' is not consecutive with junction '" + myPlanElement->getParentJunctions().front()->getID() + "'");
        } else if (previousEdge && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("edge '" + previousEdge->getID() + "' is not consecutive with junction '" + myPlanElement->getParentJunctions().front()->getID() + "'");
        } else if (previousEdge && firstEdge && (previousEdge != firstEdge)) {
            return "Edge '" + previousEdge->getID() + "' is not consecutive with edge '" + firstEdge->getID() + "'";
        }
    }
    // get next child
    const auto nextChild = myPlanElement->getParentDemandElements().at(0)->getNextChildDemandElement(myPlanElement);
    if (nextChild) {
        // get previous edge
        GNEEdge* nextEdge = nullptr;
        if (nextChild->getParentLanes().size() == 1) {
            nextEdge = nextChild->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getParentAdditionals().size() == 1) {
            nextEdge = nextChild->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (nextChild->getParentEdges().size() > 0) {
            nextEdge = nextChild->getParentEdges().front();
        } else if (nextChild->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            nextEdge = nextChild->getParentDemandElements().at(1)->getParentEdges().front();
        }
        // get last edge
        GNEEdge* lastEdge = nullptr;
        // check edge
        if (myPlanElement->getParentLanes().size() == 1) {
            lastEdge = myPlanElement->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getParentAdditionals().size() == 1) {
            lastEdge = myPlanElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (myPlanElement->getParentEdges().size() > 0) {
            lastEdge = myPlanElement->getParentEdges().back();
        } else if (myPlanElement->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            lastEdge = myPlanElement->getParentDemandElements().at(1)->getParentEdges().back();
        }
        // compare elements
        if ((nextChild->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("Junction '" + nextChild->getParentJunctions().front()->getID() +
                    "' is not consecutive with junction '" + myPlanElement->getParentJunctions().back()->getID() + "'");
        } else if (nextEdge && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("edge '" + nextEdge->getID() + "' is not consecutive with junction '" + myPlanElement->getParentJunctions().back()->getID() + "'");
        } else if (lastEdge && (nextChild->getParentJunctions().size() > 0)) {
            return ("edge '" + lastEdge->getID() + "' is not consecutive with junction '" + nextChild->getParentJunctions().back()->getID() + "'");
        } else if (nextEdge && lastEdge && (nextEdge != lastEdge)) {
            return "Edge '" + lastEdge->getID() + "' is not consecutive with edge '" + nextEdge->getID() + "'";
        }
    }
    // undefined problem
    return "undefined problem";
}

/****************************************************************************/
