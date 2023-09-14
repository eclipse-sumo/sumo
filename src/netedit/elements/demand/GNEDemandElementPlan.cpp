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

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDemandElementPlan.h"
#include "GNERoute.h"

// ===========================================================================
// static definitions
// ===========================================================================

const double GNEDemandElementPlan::myPersonPlanArrivalPositionDiameter = SUMO_const_halfLaneWidth;

// ===========================================================================
// GNEDemandElement method definitions
// ===========================================================================

GNEDemandElementPlan::GNEDemandElementPlan(GNEDemandElement* planElement, double arrivalPosition) :
    myArrivalPosition(arrivalPosition),
    myPlanElement(planElement) {
}


void
GNEDemandElementPlan::writePlanAttributes(OutputDevice& device) const {
    // write attributes depending of parent elements
    if (myPlanElement->myTagProperty.hasAttribute(SUMO_ATTR_ROUTE)) {
        device.writeAttr(SUMO_ATTR_ROUTE, myPlanElement->getParentDemandElements().at(1)->getID());
    } else if (myPlanElement->myTagProperty.hasAttribute(SUMO_ATTR_EDGES)) {
        device.writeAttr(SUMO_ATTR_EDGES, myPlanElement->parseIDs(myPlanElement->getParentEdges()));
    } else {
        // write from attribute (if enabled)
        if (myPlanElement->isAttributeEnabled(SUMO_ATTR_FROM)) {
            // check if write edge or junction
            if (myPlanElement->getParentEdges().size() > 0) {
                device.writeAttr(SUMO_ATTR_FROM, myPlanElement->getParentEdges().front()->getID());
            } else if (myPlanElement->getParentJunctions().size() > 0) {
                device.writeAttr(SUMO_ATTR_FROM_JUNCTION, myPlanElement->getParentJunctions().front()->getID());
            } else if (myPlanElement->getParentAdditionals().size() > 0) {
                device.writeAttr(SUMO_ATTR_FROM_TAZ, myPlanElement->getParentAdditionals().front()->getID());
            }
        }
        // continue writting to attribute
        if (myPlanElement->getParentEdges().size() > 0) {
            device.writeAttr(SUMO_ATTR_TO, myPlanElement->getParentEdges().back()->getID());
        } else if (myPlanElement->getParentJunctions().size() > 0) {
            device.writeAttr(SUMO_ATTR_TO_JUNCTION, myPlanElement->getParentJunctions().back()->getID());
        } else if (myPlanElement->getParentAdditionals().size() > 0) {
            // check additional type
            auto toAdditional = myPlanElement->getParentAdditionals().back();
            if (toAdditional->getTagProperty().getTag() == SUMO_TAG_BUS_STOP) {
                device.writeAttr(SUMO_ATTR_BUS_STOP, toAdditional->getID());
            } else if (toAdditional->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP) {
                device.writeAttr(SUMO_ATTR_TRAIN_STOP, toAdditional->getID());
            } else {
                device.writeAttr(SUMO_ATTR_TO_TAZ, toAdditional->getID());
            }
        }
    }
    // check if write arrival position
    if ((myPlanElement->getParentAdditionals().size() == 0) && (myArrivalPosition > 0)) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
}


GUIGLObjectPopupMenu*
GNEDemandElementPlan::getPlanPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *myPlanElement);
    // build header
    myPlanElement->buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    myPlanElement->buildCenterPopupEntry(ret);
    myPlanElement->buildPositionCopyEntry(ret, app);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + myPlanElement->getTagStr() + " name to clipboard", nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + myPlanElement->getTagStr() + " typed name to clipboard", nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myPlanElement->getNet()->getViewNet()->buildSelectionACPopupEntry(ret, myPlanElement);
    myPlanElement->buildShowParamsPopupEntry(ret);
    GUIDesigns::buildFXMenuCommand(ret, ("Cursor position in view: " + toString(getPlanPositionInView().x()) + "," + toString(getPlanPositionInView().y())).c_str(), nullptr, nullptr, 0);
    return ret;
}


GNELane*
GNEDemandElementPlan::getFirstPlanPathLane() const {
    // get vclass parent
    auto vClassParent = myPlanElement->getParentDemandElements().at(0)->getVClass();
    // get previous plan
    const auto previousPlan = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
    // first check if this is the first person plan
    if (previousPlan) {
        return previousPlan->getLastPathLane();
    } else {
        // check parents
        if (myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
            // route
            return myPlanElement->getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(vClassParent);
        } else if (myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_EDGES)) {
            // edges
            return myPlanElement->getParentEdges().front()->getLaneByAllowedVClass(vClassParent);
        } else if (myPlanElement->getParentEdges().size() == 2) {
            // from-to edges
            return myPlanElement->getParentEdges().front()->getLaneByAllowedVClass(vClassParent);
        } else {
            // in other cases (TAZ, junctions, etc.) return null
            return nullptr;
        }
    }
}


GNELane*
GNEDemandElementPlan::getLastPlanPathLane() const {
    // get vclass parent
    auto vClassParent = myPlanElement->getParentDemandElements().at(0)->getVClass();
    // check parents
    if (myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
        // route
        return myPlanElement->getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(vClassParent);
    } else if (myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_EDGES)) {
        // edges
        return myPlanElement->getParentEdges().back()->getLaneByAllowedVClass(vClassParent);
    } else if (myPlanElement->getParentEdges().size() == 2) {
        // from-to edges
        return myPlanElement->getParentEdges().back()->getLaneByAllowedVClass(vClassParent);
    } else if (myPlanElement->getParentAdditionals().size() == 1) {
        // to additional (stoppingPlaces)
        return myPlanElement->getParentAdditionals().back()->getParentLanes().front();
    } else {
        // in other cases (TAZ, junctions, etc.) return null
        return nullptr;
    }
}


void
GNEDemandElementPlan::computePlanPathElement() {
    // get vClass
    auto vClass = myPlanElement->getVClass();
    // get path manager
    auto pathManager = myPlanElement->getNet()->getPathManager();
    // first draw plan between elements over edges and routes
    if (myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_EDGES)) {
        // calculate consecutive path using edges
        pathManager->calculateConsecutivePathEdges(myPlanElement, vClass, myPlanElement->getParentEdges());
    } else if (myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
        // calculate consecutive path using route edges
        pathManager->calculateConsecutivePathEdges(myPlanElement, vClass, myPlanElement->getParentDemandElements().at(1)->getParentEdges());
    } else {
        // get previous plan
        const auto previousPlan = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
        // first check if this is the first person plan
        if (previousPlan == nullptr) {
            // continue depending of parents
            if (myPlanElement->getParentJunctions().size() > 0) {
                // calculate path between junctions
                pathManager->calculatePathJunctions(myPlanElement, vClass, myPlanElement->getParentJunctions());
            } else {
                // get first and last lane
                auto firstLane = myPlanElement->getFirstPathLane();
                auto lastLane = myPlanElement->getLastPathLane();
                // calculate path between first and last lane
                if (firstLane && lastLane) {
                    pathManager->calculatePathLanes(myPlanElement, vClass, {firstLane, lastLane});
                } else {
                    pathManager->calculatePathLanes(myPlanElement, vClass, {});
                }
            }
        } else {
            // check if previousPlan ends in a junction
            if ((previousPlan->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
                // calculate path between junctions
                pathManager->calculatePathJunctions(myPlanElement, vClass, {previousPlan->getParentJunctions().back(), myPlanElement->getParentJunctions().front()});
            } else {
                // get last lanes of both elements
                auto firstLane = previousPlan->getLastPathLane();
                auto lastLane = myPlanElement->getLastPathLane();
                // calculate path between first and last lane
                if (firstLane && lastLane) {
                    pathManager->calculatePathLanes(myPlanElement, vClass, {firstLane, lastLane});
                } else {
                    pathManager->calculatePathLanes(myPlanElement, vClass, {});
                }
            }
        }
    }
    // update geometry
    updatePlanGeometry();
}


void
GNEDemandElementPlan::updatePlanGeometry() {
    // only for plans defined between TAZs
    if (myPlanElement->getParentAdditionals().size() == 2) {
        // remove from grid
        myPlanElement->getNet()->removeGLObjectFromGrid(myPlanElement);
        // get both TAZs
        const GNEAdditional* TAZA = myPlanElement->getParentAdditionals().front();
        const GNEAdditional* TAZB = myPlanElement->getParentAdditionals().back();
        // check if this is the same TAZ
        if (TAZA == TAZB) {
            // declare ring
            PositionVector ring;
            // declare first point
            std::pair<double, double> p1 = GLHelper::getCircleCoords().at(GLHelper::angleLookup(0));
            // add 8 segments
            for (int i = 0; i <= 8; ++i) {
                const std::pair<double, double>& p2 = GLHelper::getCircleCoords().at(GLHelper::angleLookup(0 + i * 45));
                // make al line between 0,0 and p2
                PositionVector line = {Position(), Position(p2.first, p2.second)};
                // extrapolate
                line.extrapolate(3, false, true);
                // add line back to ring
                ring.push_back(line.back());
                // update p1
                p1 = p2;
            }
            // make a copy of ring
            PositionVector ringCenter = ring;
            // move ring to first geometry point
            ring.add(TAZA->getAdditionalGeometry().getShape().front());
            myPlanElement->myDemandElementGeometry.updateGeometry(ring);
        } else {
            // calculate line between to TAZ centers
            PositionVector line = {TAZA->getAttributePosition(SUMO_ATTR_CENTER), TAZB->getAttributePosition(SUMO_ATTR_CENTER)};
            // check line
            if (line.length() < 1) {
                line = {TAZA->getAttributePosition(SUMO_ATTR_CENTER) - 0.5, TAZB->getAttributePosition(SUMO_ATTR_CENTER) + 0.5};
            }
            // calculate middle point
            const Position middlePoint = line.getLineCenter();
            // get closest points to middlePoint
            Position posA = TAZA->getAdditionalGeometry().getShape().positionAtOffset2D(TAZA->getAdditionalGeometry().getShape().nearest_offset_to_point2D(middlePoint));
            Position posB = TAZB->getAdditionalGeometry().getShape().positionAtOffset2D(TAZB->getAdditionalGeometry().getShape().nearest_offset_to_point2D(middlePoint));
            // check positions
            if (posA == Position::INVALID) {
                posA = TAZA->getAdditionalGeometry().getShape().front();
            }
            if (posB == Position::INVALID) {
                posB = TAZB->getAdditionalGeometry().getShape().front();
            }
            // update geometry
            if (posA.distanceTo(posB) < 1) {
                myPlanElement->myDemandElementGeometry.updateGeometry({posA - 0.5, posB + 0.5});
            } else {
                myPlanElement->myDemandElementGeometry.updateGeometry({posA, posB});
            }
        }
        // add into grid again
        myPlanElement->getNet()->addGLObjectIntoGrid(myPlanElement);
    }
    // update child demand elements
    for (const auto& demandElement : myPlanElement->getChildDemandElements()) {
        demandElement->updateGeometry();
    }
}


Boundary
GNEDemandElementPlan::getPlanCenteringBoundary() const {
    Boundary planBoundary;
    // add the combination of all parent edges's boundaries
    for (const auto& edge : myPlanElement->getParentEdges()) {
        planBoundary.add(edge->getCenteringBoundary());
    }
    // add the combination of all parent edges's boundaries
    for (const auto& junction : myPlanElement->getParentJunctions()) {
        planBoundary.add(junction->getCenteringBoundary());
    }
    // add the combination of all parent additional's boundaries (stoppingPlaces and TAZs)
    for (const auto& additional : myPlanElement->getParentAdditionals()) {
        planBoundary.add(additional->getCenteringBoundary());
    }
    // if this element is over route, add their boundary
    if (myPlanElement->getParentDemandElements().size() > 1) {
        planBoundary.add(myPlanElement->getParentDemandElements().at(1)->getCenteringBoundary());
    }
    for (const auto& additional : myPlanElement->getParentAdditionals()) {
        planBoundary.add(additional->getCenteringBoundary());
    }
    // check if is valid
    if (planBoundary.isInitialised()) {
        return planBoundary;
    } else {
        return myPlanElement->getParentDemandElements().front()->getCenteringBoundary();
    }
}


Position
GNEDemandElementPlan::getPlanPositionInView() const {
    if (myPlanElement->getParentJunctions().size() > 0) {
        // first junction
        return myPlanElement->getParentJunctions().front()->getPositionInView();
    } else if (myPlanElement->getParentEdges().size() > 0) {
        // first edge
        return myPlanElement->getParentEdges().front()->getPositionInView();
    } else if (myPlanElement->getParentAdditionals().size() > 0) {
        // first TAZ
        return myPlanElement->getParentAdditionals().front()->getPositionInView();
    } else if (myPlanElement->myTagProperty.hasAttribute(SUMO_ATTR_ROUTE)) {
        // route
        return myPlanElement->getParentDemandElements().at(1)->getPositionInView();
    } else {
        // return parent position
        return myPlanElement->getParentDemandElements().front()->getPositionInView();
    }
}


std::string
GNEDemandElementPlan::getPlanAttribute(SumoXMLAttr key) const {
    // get route parent
    const auto routeParent = !myPlanElement->isTemplate() && myPlanElement->myTagProperty.hasAttribute(SUMO_ATTR_ROUTE)? myPlanElement->getParentDemandElements().at(1) : nullptr;
    // continue depending of key
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_ID:
        case GNE_ATTR_PARENT:
            return myPlanElement->getParentDemandElements().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(myPlanElement->isAttributeCarrierSelected());
        case SUMO_ATTR_ARRIVALPOS:
            if (myArrivalPosition == -1) {
                return "";
            } else {
                return toString(myArrivalPosition);
            }
        // edges
        case SUMO_ATTR_FROM:
            if (routeParent) {
                return routeParent->getParentEdges().front()->getID();
            } else {
                return myPlanElement->getParentEdges().front()->getID();
            }
        case SUMO_ATTR_TO:
            if (routeParent) {
                return routeParent->getParentEdges().back()->getID();
            } else {
                return myPlanElement->getParentEdges().back()->getID();
            }
        case SUMO_ATTR_EDGES:
            return myPlanElement->parseIDs(myPlanElement->getParentEdges());
        // junctions
        case SUMO_ATTR_FROM_JUNCTION:
            return myPlanElement->getParentJunctions().front()->getID();
        case SUMO_ATTR_TO_JUNCTION:
            return myPlanElement->getParentJunctions().back()->getID();
        // additionals
        case SUMO_ATTR_FROM_TAZ:
            return myPlanElement->getParentAdditionals().front()->getID();
        case GNE_ATTR_TO_BUSSTOP:
        case GNE_ATTR_TO_TRAINSTOP:
        case SUMO_ATTR_TO_TAZ:
            return myPlanElement->getParentAdditionals().back()->getID();
        // route
        case SUMO_ATTR_ROUTE:
            return routeParent->getID();
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEDemandElementPlan::getPlanAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
            return myArrivalPosition;
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have a doubleattribute of type '" + toString(key) + "'");
    }
}


Position
GNEDemandElementPlan::getPlanAttributePosition(SumoXMLAttr key) const {
    // declare plan parent
    const auto planParent = myPlanElement->getParentDemandElements().at(0);
    // continue depending of key
    switch (key) {
        case GNE_ATTR_PLAN_GEOMETRY_STARTPOS: {
            // check parents
            if (myPlanElement->getParentJunctions().size() > 0) {
                // junctions
                return myPlanElement->getParentJunctions().front()->getPositionInView();
            } else if (myPlanElement->getParentAdditionals().size() == 2) {
                // TAZs
                return myPlanElement->getParentAdditionals().front()->getPositionInView();
            } else {
                // get first lane
                const auto firstLane = myPlanElement->getFirstPathLane();
                // check if first lane exists
                if (firstLane) {
                    // declare depart position
                    double departPosition = 0;
                    // get previous plan
                    const auto previousPlan = planParent->getPreviousChildDemandElement(myPlanElement);
                    // check if previous plan exist
                    if (previousPlan) {
                        // use arrival position of previous plan
                        departPosition = previousPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
                    }
                    // get lane shapes
                    const auto &laneShape = firstLane->getLaneShape();
                    // continue depending of arrival position
                    if (departPosition == 0) {
                        return laneShape.front();
                    } else if ((departPosition == -1) || (departPosition >= laneShape.length2D())) {
                        return laneShape.back();
                    } else {
                        return laneShape.positionAtOffset2D(departPosition);
                    }
                } else {
                    WRITE_ERROR("invalid GNE_ATTR_PLAN_GEOMETRY_STARTPOS");
                    // invalid position
                    return Position(0,0);
                }
            }
        }
        case GNE_ATTR_PLAN_GEOMETRY_ENDPOS: {
            // check parents
            if (myPlanElement->getParentJunctions().size() > 0) {
                // junctions
                return myPlanElement->getParentJunctions().back()->getPositionInView();
            } else if (myPlanElement->getParentAdditionals().size() > 0) {
                // TAZs and stoppingPlaces
                return myPlanElement->getParentAdditionals().back()->getPositionInView();
            } else {
                // get last lane
                const auto lastLane = myPlanElement->getLastPathLane();
                // check if last lane exists
                if (lastLane) {
                    // get lane shape
                    const auto& laneShape = lastLane->getLaneShape();
                    // continue depending of arrival position
                    if (myArrivalPosition == 0) {
                        return laneShape.front();
                    } else if ((myArrivalPosition == -1) || (myArrivalPosition >= laneShape.length2D())) {
                        return laneShape.back();
                    } else {
                        return laneShape.positionAtOffset2D(myArrivalPosition);
                    }
                } else {
                    WRITE_ERROR("invalid GNE_ATTR_PLAN_GEOMETRY_ENDPOS");
                    // invalid position
                    return Position(0,0);
                }
            }
        }
        case GNE_ATTR_PLAN_GEOMETRY_PREVIOUS: {
            // get previous plan
            const auto previousPlan = planParent->getPreviousChildDemandElement(myPlanElement);
            // if exists, return previous plan geometry position
            if (previousPlan) {
                return previousPlan->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
            } else {
                return Position::INVALID;
            }
        }
        case GNE_ATTR_PLAN_GEOMETRY_NEXT: {
            // get next plan
            const auto nextPlan = planParent->getNextChildDemandElement(myPlanElement);
            // if exists, return next plan geometry position
            if (nextPlan) {
                return nextPlan->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS);
            } else {
                return Position::INVALID;
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
        // common attributes
        case GNE_ATTR_PARENT:
        case SUMO_ATTR_ARRIVALPOS:
        case GNE_ATTR_SELECTED:
            GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            break;
        // from attributes
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_FROM_JUNCTION:
        case SUMO_ATTR_FROM_TAZ:
            // plans placed over routes cannot change their from attribute
            if (!myPlanElement->getTagProperty().hasAttribute(SUMO_ATTR_ROUTE)) {
                GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            }

/// CHECK IF MODIFY TO ATTRIBUTE OF PREVIOUS PERSON PLAN

            break;
        // to attributes
        case SUMO_ATTR_TO:
        case SUMO_ATTR_TO_JUNCTION:
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
        // common attributes
        case GNE_ATTR_PARENT:
            // check all parents
            for (const auto &parentTag : myPlanElement->getTagProperty().getParentTags()) {
                if (ACs->retrieveDemandElement(parentTag, value, false) != nullptr) {
                    return true;
                }
            }
            return false;
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                return true;
            } else if (GNEAttributeCarrier::canParse<double>(value)) {
                return GNEAttributeCarrier::parse<double>(value) >= 0;
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return GNEAttributeCarrier::canParse<bool>(value);
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
        case SUMO_ATTR_FROM_JUNCTION:
        case SUMO_ATTR_TO_JUNCTION:
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
        case SUMO_ATTR_FROM_JUNCTION:
        case SUMO_ATTR_FROM_TAZ:
            return (myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement) == nullptr);
        default:
            return true;
    }
}


void
GNEDemandElementPlan::setPlanAttribute(SumoXMLAttr key, const std::string& value) {
    bool computeAfterChange = true;
    switch (key) {
        // Common plan attributes
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                myArrivalPosition = -1;
            } else {
                myArrivalPosition = GNEAttributeCarrier::parse<double>(value);
            }
            break;
        case GNE_ATTR_SELECTED:
            if (GNEAttributeCarrier::parse<bool>(value)) {
                myPlanElement->selectAttributeCarrier();
            } else {
                myPlanElement->unselectAttributeCarrier();
            }
            // compute isnt' neccesary
            computeAfterChange = false;
            break;
        case GNE_ATTR_PARENT:
            for (const auto &parentTag : myPlanElement->getTagProperty().getParentTags()) {
                if (myPlanElement->getNet()->getAttributeCarriers()->retrieveDemandElement(parentTag, value, false) != nullptr) {
                    myPlanElement->replaceDemandElementParent(parentTag, value, 0);
                }
            }
            break;
        // edges
        case SUMO_ATTR_FROM:
            // change first edge
            myPlanElement->replaceFirstParentEdge(value);
            break;
        case SUMO_ATTR_TO:
            // change last edge
            myPlanElement->replaceLastParentEdge(value);
            break;
        case SUMO_ATTR_EDGES:
            myPlanElement->replaceDemandParentEdges(value);
            break;
        // junctions
        case SUMO_ATTR_FROM_JUNCTION:
            // change first junction
            myPlanElement->replaceFirstParentJunction(value);
            break;
        case SUMO_ATTR_TO_JUNCTION:
            // change last junction
            myPlanElement->replaceLastParentJunction(value);
            break;
        // TAZs
        case SUMO_ATTR_FROM_TAZ:
            // change first TAZ
            myPlanElement->replaceFirstParentAdditional(SUMO_TAG_TAZ, value);
            break;
        case SUMO_ATTR_TO_TAZ:
            // change last TAZ
            myPlanElement->replaceLastParentAdditional(SUMO_TAG_TAZ, value);
            break;
        // busStop
        case GNE_ATTR_TO_BUSSTOP:
            myPlanElement->replaceFirstParentAdditional(SUMO_TAG_BUS_STOP, value);
            break;
        // trainStop
        case GNE_ATTR_TO_TRAINSTOP:
            myPlanElement->replaceFirstParentAdditional(SUMO_TAG_TRAIN_STOP, value);
            break;
        // route
        case SUMO_ATTR_ROUTE:
            myPlanElement->replaceDemandElementParent(SUMO_TAG_ROUTE, value, 1);
            break;
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // check if compute geometry and path
    if (!myPlanElement->isTemplate() && computeAfterChange) {
        myPlanElement->updateGeometry();
        myPlanElement->computePathElement();
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
        return tagStr + myPlanElement->getParentEdges().front()->getID() + " -> " + myPlanElement->getParentAdditionals().back()->getID();
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
GNEDemandElementPlan::drawPlanGL(const GUIVisualizationSettings& s, const RGBColor& planColor) const {
    // draw TAZRels
    if (myPlanElement->getParentAdditionals().size() == 2) {
        // get viewNet
        auto viewNet = myPlanElement->getNet()->getViewNet();
        // get geometry
        auto &geometry = myPlanElement->myDemandElementGeometry;
        // check if boundary has to be drawn
        if (s.drawBoundaries) {
            GLHelper::drawBoundary(myPlanElement->getCenteringBoundary());
        }
        // push GL ID
        GLHelper::pushName(myPlanElement->getGlID());
        // push matrix
        GLHelper::pushMatrix();
        // translate to front
        viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, GLO_TAZ + 1);
        GLHelper::setColor(planColor);
        // draw line
        GUIGeometry::drawGeometry(s, viewNet->getPositionInformation(), geometry, 0.5);
        GLHelper::drawTriangleAtEnd(
            *(geometry.getShape().end() - 2),
            *(geometry.getShape().end() - 1),
            0.5, 0.5, 0.5);
        // pop matrix
        GLHelper::popMatrix();
        // pop name
        GLHelper::popName();
        // inspect contour
        if (viewNet->isAttributeCarrierInspected(myPlanElement)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, geometry.getShape(), 0.5, 1, true, true);
        }
        // front contour
        if (viewNet->getFrontAttributeCarrier() == myPlanElement) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, geometry.getShape(), 0.5, 1, true, true);
        }
        // delete contour
        if (viewNet->drawDeleteContour(myPlanElement, myPlanElement)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, geometry.getShape(), 0.5, 1, true, true);
        }
        // select contour
        if (viewNet->drawSelectContour(myPlanElement, myPlanElement)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, geometry.getShape(), 0.5, 1, true, true);
        }
        // check if draw person parent
        const GNEDemandElement* personParent = myPlanElement->getParentDemandElements().front();
        if ((personParent->getChildDemandElements().size() > 0) && (personParent->getChildDemandElements().front() == myPlanElement)) {
            personParent->drawGL(s);
        }
    } else if (myPlanElement->getParentJunctions().size() > 0) {
        // check if draw person parent
        const GNEDemandElement* personParent = myPlanElement->getParentDemandElements().front();
        if ((personParent->getChildDemandElements().size() > 0) && (personParent->getChildDemandElements().front() == myPlanElement)) {
            personParent->drawGL(s);
        }
    }
    // force draw path
    myPlanElement->getNet()->getPathManager()->forceDrawPath(s, myPlanElement);
}


void
GNEDemandElementPlan::drawPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment,
                                        const double offsetFront, const double personPlanWidth, const RGBColor& planColor) const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // get inspected and front flags
    const bool dottedElement = viewNet->isAttributeCarrierInspected(myPlanElement) || (viewNet->getFrontAttributeCarrier() == myPlanElement);
    // get person parent
    const GNEDemandElement* personParent = myPlanElement->getParentDemandElements().front();
    // check if draw person plan element can be drawn
    if ((planColor.alpha() != 0) && drawPlan && myPlanElement->getNet()->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, lane, myPlanElement->getTagProperty().getTag())) {
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
        const RGBColor& pathColor = myPlanElement->drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : planColor;
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
GNEDemandElementPlan::drawPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/,
                                        const double offsetFront, const double personPlanWidth, const RGBColor& planColor) const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // get inspected and front flags
    const bool dottedElement = viewNet->isAttributeCarrierInspected(myPlanElement) || (viewNet->getFrontAttributeCarrier() == myPlanElement);
    // check if draw person plan elements can be drawn
    if ((planColor.alpha() != 0) && drawPlan && myPlanElement->getNet()->getPathManager()->getPathDraw()->drawPathGeometry(false, fromLane, toLane, myPlanElement->getTagProperty().getTag())) {
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
        const RGBColor& color = myPlanElement->drawUsingSelectColor() ? s.colorSettings.selectedPersonPlanColor : planColor;
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
    // get previous plan
    const auto previousPlan = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
    if (previousPlan) {
        // get previous edge
        GNEEdge* previousEdge = nullptr;
        if (previousPlan->getParentLanes().size() == 1) {
            previousEdge = previousPlan->getParentLanes().front()->getParentEdge();
        } else if (previousPlan->getParentAdditionals().size() == 1) {
            previousEdge = previousPlan->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (previousPlan->getParentEdges().size() > 0) {
            previousEdge = previousPlan->getParentEdges().back();
        } else if (previousPlan->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            previousEdge = previousPlan->getParentDemandElements().at(1)->getParentEdges().back();
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
        if ((previousPlan->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            if (previousPlan->getParentJunctions().back() != myPlanElement->getParentJunctions().front()) {
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
    const auto nextPlan = myPlanElement->getParentDemandElements().at(0)->getNextChildDemandElement(myPlanElement);
    if (nextPlan) {
        // get previous edge
        GNEEdge* nextEdge = nullptr;
        if (nextPlan->getParentLanes().size() == 1) {
            nextEdge = nextPlan->getParentLanes().front()->getParentEdge();
        } else if (nextPlan->getParentEdges().size() > 0) {
            nextEdge = nextPlan->getParentEdges().front();
        } else if (nextPlan->getParentAdditionals().size() == 1) {
            nextEdge = nextPlan->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (nextPlan->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            nextEdge = nextPlan->getParentDemandElements().at(1)->getParentEdges().front();
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
        if ((nextPlan->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            if (nextPlan->getParentJunctions().front() != myPlanElement->getParentJunctions().back()) {
                return GNEDemandElement::Problem::DISCONNECTED_PLAN;
            }
        } else if (nextEdge && (myPlanElement->getParentJunctions().size() > 0)) {
            if (nextEdge->getFromJunction() != myPlanElement->getParentJunctions().back()) {
                return GNEDemandElement::Problem::DISCONNECTED_PLAN;
            }
        } else if (lastEdge && (nextPlan->getParentJunctions().size() > 0)) {
            if (lastEdge->getToJunction() != nextPlan->getParentJunctions().front()) {
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
    // get previous plan
    const auto previousPlan = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
    if (previousPlan) {
        // get previous edge
        GNEEdge* previousEdge = nullptr;
        if (previousPlan->getParentLanes().size() == 1) {
            previousEdge = previousPlan->getParentLanes().front()->getParentEdge();
        } else if (previousPlan->getParentAdditionals().size() == 1) {
            previousEdge = previousPlan->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (previousPlan->getParentEdges().size() > 0) {
            previousEdge = previousPlan->getParentEdges().back();
        } else if (previousPlan->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            previousEdge = previousPlan->getParentDemandElements().at(1)->getParentEdges().back();
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
        if ((previousPlan->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("Junction '" + previousPlan->getParentJunctions().back()->getID() +
                    "' is not consecutive with junction '" + myPlanElement->getParentJunctions().front()->getID() + "'");
        } else if (previousEdge && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("edge '" + previousEdge->getID() + "' is not consecutive with junction '" + myPlanElement->getParentJunctions().front()->getID() + "'");
        } else if (previousEdge && firstEdge && (previousEdge != firstEdge)) {
            return "Edge '" + previousEdge->getID() + "' is not consecutive with edge '" + firstEdge->getID() + "'";
        }
    }
    // get next child
    const auto nextPlan = myPlanElement->getParentDemandElements().at(0)->getNextChildDemandElement(myPlanElement);
    if (nextPlan) {
        // get previous edge
        GNEEdge* nextEdge = nullptr;
        if (nextPlan->getParentLanes().size() == 1) {
            nextEdge = nextPlan->getParentLanes().front()->getParentEdge();
        } else if (nextPlan->getParentAdditionals().size() == 1) {
            nextEdge = nextPlan->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        } else if (nextPlan->getParentEdges().size() > 0) {
            nextEdge = nextPlan->getParentEdges().front();
        } else if (nextPlan->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
            nextEdge = nextPlan->getParentDemandElements().at(1)->getParentEdges().front();
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
        if ((nextPlan->getParentJunctions().size() > 0) && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("Junction '" + nextPlan->getParentJunctions().front()->getID() +
                    "' is not consecutive with junction '" + myPlanElement->getParentJunctions().back()->getID() + "'");
        } else if (nextEdge && (myPlanElement->getParentJunctions().size() > 0)) {
            return ("edge '" + nextEdge->getID() + "' is not consecutive with junction '" + myPlanElement->getParentJunctions().back()->getID() + "'");
        } else if (lastEdge && (nextPlan->getParentJunctions().size() > 0)) {
            return ("edge '" + lastEdge->getID() + "' is not consecutive with junction '" + nextPlan->getParentJunctions().back()->getID() + "'");
        } else if (nextEdge && lastEdge && (nextEdge != lastEdge)) {
            return "Edge '" + lastEdge->getID() + "' is not consecutive with edge '" + nextEdge->getID() + "'";
        }
    }
    // undefined problem
    return "undefined problem";
}

/****************************************************************************/
