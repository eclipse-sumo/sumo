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

const double GNEDemandElementPlan::myArrivalPositionDiameter = SUMO_const_halfLaneWidth;

// ===========================================================================
// GNEDemandElement method definitions
// ===========================================================================

std::pair<SumoXMLTag, GUIIcon>
GNEDemandElementPlan::getWalkTagIcon(const std::vector<GNEEdge*>& consecutiveEdges,
                                     const GNEDemandElement* route, const GNEEdge* fromEdge, const GNEEdge* toEdge,
                                     const GNEAdditional* fromTAZ, const GNEAdditional* toTAZ, const GNEJunction* fromJunction,
                                     const GNEJunction* toJunction, const GNEAdditional* fromBusStop, const GNEAdditional* toBusStop,
                                     const GNEAdditional* fromTrainStop, const GNEAdditional* toTrainStop) {
    // special case for elements with from-to edge
    if (fromEdge && !toEdge && !fromTAZ && !toTAZ && !fromJunction && !toJunction &&
            !fromBusStop && !toBusStop && !fromTrainStop && !toTrainStop &&
            consecutiveEdges.empty() && !route) {
        return std::make_pair(GNE_TAG_WALK_EDGES, GUIIcon::WALK_EDGES);
    } else if (consecutiveEdges.size() > 0) {
        return std::make_pair(GNE_TAG_WALK_EDGES, GUIIcon::WALK_EDGES);
    } else if (route) {
        return std::make_pair(GNE_TAG_WALK_ROUTE, GUIIcon::WALK_ROUTE);
    } else if (fromEdge && toEdge) {
        return std::make_pair(GNE_TAG_WALK_EDGE_EDGE, GUIIcon::WALK_EDGE);
    } else if (fromEdge && toTAZ) {
        return std::make_pair(GNE_TAG_WALK_EDGE_TAZ, GUIIcon::WALK_TAZ);
    } else if (fromEdge && toJunction) {
        return std::make_pair(GNE_TAG_WALK_EDGE_JUNCTION, GUIIcon::WALK_JUNCTION);
    } else if (fromEdge && toBusStop) {
        return std::make_pair(GNE_TAG_WALK_EDGE_BUSSTOP, GUIIcon::WALK_BUSSTOP);
    } else if (fromEdge && toTrainStop) {
        return std::make_pair(GNE_TAG_WALK_EDGE_TRAINSTOP, GUIIcon::WALK_TRAINSTOP);
    } else if (fromTAZ && toEdge) {
        return std::make_pair(GNE_TAG_WALK_TAZ_EDGE, GUIIcon::WALK_EDGE);
    } else if (fromTAZ && toTAZ) {
        return std::make_pair(GNE_TAG_WALK_TAZ_TAZ, GUIIcon::WALK_TAZ);
    } else if (fromTAZ && toJunction) {
        return std::make_pair(GNE_TAG_WALK_TAZ_JUNCTION, GUIIcon::WALK_JUNCTION);
    } else if (fromTAZ && toBusStop) {
        return std::make_pair(GNE_TAG_WALK_TAZ_BUSSTOP, GUIIcon::WALK_BUSSTOP);
    } else if (fromTAZ && toTrainStop) {
        return std::make_pair(GNE_TAG_WALK_TAZ_TRAINSTOP, GUIIcon::WALK_TRAINSTOP);
    } else if (fromJunction && toEdge) {
        return std::make_pair(GNE_TAG_WALK_JUNCTION_EDGE, GUIIcon::WALK_EDGE);
    } else if (fromJunction && toTAZ) {
        return std::make_pair(GNE_TAG_WALK_JUNCTION_TAZ, GUIIcon::WALK_TAZ);
    } else if (fromJunction && toJunction) {
        return std::make_pair(GNE_TAG_WALK_JUNCTION_JUNCTION, GUIIcon::WALK_JUNCTION);
    } else if (fromJunction && toBusStop) {
        return std::make_pair(GNE_TAG_WALK_JUNCTION_BUSSTOP, GUIIcon::WALK_BUSSTOP);
    } else if (fromJunction && toTrainStop) {
        return std::make_pair(GNE_TAG_WALK_JUNCTION_TRAINSTOP, GUIIcon::WALK_TRAINSTOP);
    } else if (fromBusStop && toEdge) {
        return std::make_pair(GNE_TAG_WALK_BUSSTOP_EDGE, GUIIcon::WALK_EDGE);
    } else if (fromBusStop && toTAZ) {
        return std::make_pair(GNE_TAG_WALK_BUSSTOP_TAZ, GUIIcon::WALK_TAZ);
    } else if (fromBusStop && toJunction) {
        return std::make_pair(GNE_TAG_WALK_BUSSTOP_JUNCTION, GUIIcon::WALK_JUNCTION);
    } else if (fromBusStop && toBusStop) {
        return std::make_pair(GNE_TAG_WALK_BUSSTOP_BUSSTOP, GUIIcon::WALK_BUSSTOP);
    } else if (fromBusStop && toTrainStop) {
        return std::make_pair(GNE_TAG_WALK_BUSSTOP_TRAINSTOP, GUIIcon::WALK_TRAINSTOP);
    } else if (fromTrainStop && toEdge) {
        return std::make_pair(GNE_TAG_WALK_TRAINSTOP_EDGE, GUIIcon::WALK_EDGE);
    } else if (fromTrainStop && toTAZ) {
        return std::make_pair(GNE_TAG_WALK_TRAINSTOP_TAZ, GUIIcon::WALK_TAZ);
    } else if (fromTrainStop && toJunction) {
        return std::make_pair(GNE_TAG_WALK_TRAINSTOP_JUNCTION, GUIIcon::WALK_JUNCTION);
    } else if (fromTrainStop && toBusStop) {
        return std::make_pair(GNE_TAG_WALK_TRAINSTOP_BUSSTOP, GUIIcon::WALK_BUSSTOP);
    } else if (fromTrainStop && toTrainStop) {
        return std::make_pair(GNE_TAG_WALK_TRAINSTOP_TRAINSTOP, GUIIcon::WALK_TRAINSTOP);
    } else {
        return std::make_pair(SUMO_TAG_NOTHING, GUIIcon::EMPTY);
    }
}


std::pair<SumoXMLTag, GUIIcon>
GNEDemandElementPlan::getPersonTripTagIcon(const GNEEdge* fromEdge, const GNEEdge* toEdge,
        const GNEAdditional* fromTAZ, const GNEAdditional* toTAZ, const GNEJunction* fromJunction,
        const GNEJunction* toJunction, const GNEAdditional* fromBusStop, const GNEAdditional* toBusStop,
        const GNEAdditional* fromTrainStop, const GNEAdditional* toTrainStop) {
    // special case for elements with from-to edge
    if (fromEdge && !toEdge && !fromTAZ && !toTAZ && !fromJunction && !toJunction &&
            !fromBusStop && !toBusStop && !fromTrainStop && !toTrainStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_EDGE_EDGE, GUIIcon::PERSONTRIP_EDGE);
    } else if (fromEdge && toEdge) {
        return std::make_pair(GNE_TAG_PERSONTRIP_EDGE_EDGE, GUIIcon::PERSONTRIP_EDGE);
    } else if (fromEdge && toTAZ) {
        return std::make_pair(GNE_TAG_PERSONTRIP_EDGE_TAZ, GUIIcon::PERSONTRIP_TAZ);
    } else if (fromEdge && toJunction) {
        return std::make_pair(GNE_TAG_PERSONTRIP_EDGE_JUNCTION, GUIIcon::PERSONTRIP_JUNCTION);
    } else if (fromEdge && toBusStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_EDGE_BUSSTOP, GUIIcon::PERSONTRIP_BUSSTOP);
    } else if (fromEdge && toTrainStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_EDGE_TRAINSTOP, GUIIcon::PERSONTRIP_TRAINSTOP);
    } else if (fromTAZ && toEdge) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TAZ_EDGE, GUIIcon::PERSONTRIP_EDGE);
    } else if (fromTAZ && toTAZ) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TAZ_TAZ, GUIIcon::PERSONTRIP_TAZ);
    } else if (fromTAZ && toJunction) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TAZ_JUNCTION, GUIIcon::PERSONTRIP_JUNCTION);
    } else if (fromTAZ && toBusStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TAZ_BUSSTOP, GUIIcon::PERSONTRIP_BUSSTOP);
    } else if (fromTAZ && toTrainStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TAZ_TRAINSTOP, GUIIcon::PERSONTRIP_TRAINSTOP);
    } else if (fromJunction && toEdge) {
        return std::make_pair(GNE_TAG_PERSONTRIP_JUNCTION_EDGE, GUIIcon::PERSONTRIP_EDGE);
    } else if (fromJunction && toTAZ) {
        return std::make_pair(GNE_TAG_PERSONTRIP_JUNCTION_TAZ, GUIIcon::PERSONTRIP_TAZ);
    } else if (fromJunction && toJunction) {
        return std::make_pair(GNE_TAG_PERSONTRIP_JUNCTION_JUNCTION, GUIIcon::PERSONTRIP_JUNCTION);
    } else if (fromJunction && toBusStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_JUNCTION_BUSSTOP, GUIIcon::PERSONTRIP_BUSSTOP);
    } else if (fromJunction && toTrainStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_JUNCTION_TRAINSTOP, GUIIcon::PERSONTRIP_TRAINSTOP);
    } else if (fromBusStop && toEdge) {
        return std::make_pair(GNE_TAG_PERSONTRIP_BUSSTOP_EDGE, GUIIcon::PERSONTRIP_EDGE);
    } else if (fromBusStop && toTAZ) {
        return std::make_pair(GNE_TAG_PERSONTRIP_BUSSTOP_TAZ, GUIIcon::PERSONTRIP_TAZ);
    } else if (fromBusStop && toJunction) {
        return std::make_pair(GNE_TAG_PERSONTRIP_BUSSTOP_JUNCTION, GUIIcon::PERSONTRIP_JUNCTION);
    } else if (fromBusStop && toBusStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_BUSSTOP_BUSSTOP, GUIIcon::PERSONTRIP_BUSSTOP);
    } else if (fromBusStop && toTrainStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_BUSSTOP_TRAINSTOP, GUIIcon::PERSONTRIP_TRAINSTOP);
    } else if (fromTrainStop && toEdge) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TRAINSTOP_EDGE, GUIIcon::PERSONTRIP_EDGE);
    } else if (fromTrainStop && toTAZ) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TRAINSTOP_TAZ, GUIIcon::PERSONTRIP_TAZ);
    } else if (fromTrainStop && toJunction) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TRAINSTOP_JUNCTION, GUIIcon::PERSONTRIP_JUNCTION);
    } else if (fromTrainStop && toBusStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TRAINSTOP_BUSSTOP, GUIIcon::PERSONTRIP_BUSSTOP);
    } else if (fromTrainStop && toTrainStop) {
        return std::make_pair(GNE_TAG_PERSONTRIP_TRAINSTOP_TRAINSTOP, GUIIcon::PERSONTRIP_TRAINSTOP);
    } else {
        return std::make_pair(SUMO_TAG_NOTHING, GUIIcon::EMPTY);
    }
}


std::pair<SumoXMLTag, GUIIcon>
GNEDemandElementPlan::getRideTagIcon(const GNEEdge* fromEdge, const GNEEdge* toEdge,
                                     const GNEAdditional* fromBusStop, const GNEAdditional* toBusStop,
                                     const GNEAdditional* fromTrainStop, const GNEAdditional* toTrainStop) {
    // special case for elements with from-to edge
    if (fromEdge && !toEdge && fromBusStop && !toBusStop && !fromTrainStop && !toTrainStop) {
        return std::make_pair(GNE_TAG_RIDE_EDGE_EDGE, GUIIcon::RIDE_EDGE);
    } else if (fromEdge && toEdge) {
        return std::make_pair(GNE_TAG_RIDE_EDGE_EDGE, GUIIcon::RIDE_EDGE);
    } else if (fromEdge && toBusStop) {
        return std::make_pair(GNE_TAG_RIDE_EDGE_BUSSTOP, GUIIcon::RIDE_BUSSTOP);
    } else if (fromEdge && toTrainStop) {
        return std::make_pair(GNE_TAG_RIDE_EDGE_TRAINSTOP, GUIIcon::RIDE_TRAINSTOP);
    } else if (fromBusStop && toEdge) {
        return std::make_pair(GNE_TAG_RIDE_BUSSTOP_EDGE, GUIIcon::RIDE_EDGE);
    } else if (fromBusStop && toBusStop) {
        return std::make_pair(GNE_TAG_RIDE_BUSSTOP_BUSSTOP, GUIIcon::RIDE_BUSSTOP);
    } else if (fromBusStop && toTrainStop) {
        return std::make_pair(GNE_TAG_RIDE_BUSSTOP_TRAINSTOP, GUIIcon::RIDE_TRAINSTOP);
    } else if (fromTrainStop && toEdge) {
        return std::make_pair(GNE_TAG_RIDE_TRAINSTOP_EDGE, GUIIcon::RIDE_EDGE);
    } else if (fromTrainStop && toBusStop) {
        return std::make_pair(GNE_TAG_RIDE_TRAINSTOP_BUSSTOP, GUIIcon::RIDE_BUSSTOP);
    } else if (fromTrainStop && toTrainStop) {
        return std::make_pair(GNE_TAG_RIDE_TRAINSTOP_TRAINSTOP, GUIIcon::RIDE_TRAINSTOP);
    } else {
        return std::make_pair(SUMO_TAG_NOTHING, GUIIcon::EMPTY);
    }
}


std::pair<SumoXMLTag, GUIIcon>
GNEDemandElementPlan::getTransportTagIcon(const GNEEdge* fromEdge, const GNEEdge* toEdge,
        const GNEAdditional* fromContainerStop, const GNEAdditional* toContainerStop) {
    // special case for elements with from-to edge
    if (fromEdge && !toEdge && !fromContainerStop && !toContainerStop) {
        return std::make_pair(GNE_TAG_TRANSPORT_EDGE_EDGE, GUIIcon::TRANSPORT_EDGE);
    } else if (fromEdge && toEdge) {
        return std::make_pair(GNE_TAG_TRANSPORT_EDGE_EDGE, GUIIcon::TRANSPORT_EDGE);
    } else if (fromEdge && toContainerStop) {
        return std::make_pair(GNE_TAG_TRANSPORT_EDGE_CONTAINERSTOP, GUIIcon::TRANSPORT_CONTAINERSTOP);
    } else if (fromContainerStop && toEdge) {
        return std::make_pair(GNE_TAG_TRANSPORT_CONTAINERSTOP_EDGE, GUIIcon::TRANSPORT_EDGE);
    } else if (fromContainerStop && toContainerStop) {
        return std::make_pair(GNE_TAG_TRANSPORT_CONTAINERSTOP_CONTAINERSTOP, GUIIcon::TRANSPORT_CONTAINERSTOP);
    } else {
        return std::make_pair(SUMO_TAG_NOTHING, GUIIcon::EMPTY);
    }
}


std::pair<SumoXMLTag, GUIIcon>
GNEDemandElementPlan::getTranshipTagIcon(const std::vector<GNEEdge*>& consecutiveEdges,
        const GNEEdge* fromEdge, const GNEEdge* toEdge, const GNEAdditional* fromContainerStop,
        const GNEAdditional* toContainerStop) {
    // special case for elements with from-to edge
    if (fromEdge && !toEdge && !fromContainerStop && !toContainerStop && consecutiveEdges.empty()) {
        return std::make_pair(GNE_TAG_TRANSHIP_EDGES, GUIIcon::TRANSHIP_EDGES);
    } else if (consecutiveEdges.size() > 0) {
        return std::make_pair(GNE_TAG_TRANSHIP_EDGES, GUIIcon::TRANSHIP_EDGES);
    } else if (fromEdge && toEdge) {
        return std::make_pair(GNE_TAG_TRANSHIP_EDGE_EDGE, GUIIcon::TRANSHIP_EDGE);
    } else if (fromEdge && toContainerStop) {
        return std::make_pair(GNE_TAG_TRANSHIP_EDGE_CONTAINERSTOP, GUIIcon::TRANSHIP_CONTAINERSTOP);
    } else if (fromContainerStop && toEdge) {
        return std::make_pair(GNE_TAG_TRANSHIP_CONTAINERSTOP_EDGE, GUIIcon::TRANSHIP_EDGE);
    } else if (fromContainerStop && toContainerStop) {
        return std::make_pair(GNE_TAG_TRANSHIP_CONTAINERSTOP_CONTAINERSTOP, GUIIcon::TRANSHIP_CONTAINERSTOP);
    } else {
        return std::make_pair(SUMO_TAG_NOTHING, GUIIcon::EMPTY);
    }
}


std::pair<SumoXMLTag, GUIIcon>
GNEDemandElementPlan::getPersonStopTagIcon(const GNEEdge* edge, const GNEAdditional* busStop, const GNEAdditional* trainStop) {
    if (edge) {
        return std::make_pair(GNE_TAG_STOPPERSON_EDGE, GUIIcon::STOPELEMENT);
    } else if (busStop) {
        return std::make_pair(GNE_TAG_STOPPERSON_BUSSTOP, GUIIcon::STOPELEMENT);
    } else if (trainStop) {
        return std::make_pair(GNE_TAG_STOPPERSON_TRAINSTOP, GUIIcon::STOPELEMENT);
    } else {
        return std::make_pair(SUMO_TAG_NOTHING, GUIIcon::EMPTY);
    }
}


std::pair<SumoXMLTag, GUIIcon>
GNEDemandElementPlan::getContainerStopTagIcon(const GNEEdge* edge, const GNEAdditional* containerStop) {
    if (edge) {
        return std::make_pair(GNE_TAG_STOPCONTAINER_EDGE, GUIIcon::STOPELEMENT);
    } else if (containerStop) {
        return std::make_pair(GNE_TAG_STOPCONTAINER_CONTAINERSTOP, GUIIcon::STOPELEMENT);
    } else {
        return std::make_pair(SUMO_TAG_NOTHING, GUIIcon::EMPTY);
    }
}


GNEDemandElementPlan::GNEDemandElementPlan(GNEDemandElement* planElement, const double departPosition, const double arrivalPosition) :
    myDepartPosition(departPosition),
    myArrivalPosition(arrivalPosition),
    myPlanElement(planElement) {
}


GNEMoveOperation*
GNEDemandElementPlan::getPlanMoveOperation() {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // only move personTrips defined over edges
    if (tagProperty.planToEdge() || tagProperty.planConsecutiveEdges() || tagProperty.planEdge()) {
        // get geometry end pos
        const Position geometryEndPos = getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
        // calculate circle width squared
        const double circleWidthSquared = myArrivalPositionDiameter * myArrivalPositionDiameter;
        // check if we clicked over a geometry end pos
        if (myPlanElement->myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= ((circleWidthSquared + 2))) {
            // continue depending of parent edges
            if (myPlanElement->getParentEdges().size() > 0) {
                return new GNEMoveOperation(myPlanElement, myPlanElement->getParentEdges().back()->getLaneByAllowedVClass(myPlanElement->getVClass()), myArrivalPosition, false);
            } else {
                return new GNEMoveOperation(myPlanElement, myPlanElement->getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(myPlanElement->getVClass()), myArrivalPosition, false);
            }
        }
    }
    return nullptr;
}


void
GNEDemandElementPlan::writePlanAttributes(OutputDevice& device) const {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // write attributes depending of parent elements
    if (tagProperty.planConsecutiveEdges()) {
        device.writeAttr(SUMO_ATTR_EDGES, myPlanElement->parseIDs(myPlanElement->getParentEdges()));
    } else if (tagProperty.planRoute()) {
        device.writeAttr(SUMO_ATTR_ROUTE, myPlanElement->getParentDemandElements().at(1)->getID());
    } else if (tagProperty.planEdge()) {
        device.writeAttr(SUMO_ATTR_EDGE, myPlanElement->getParentEdges().front()->getID());
    } else if (tagProperty.planBusStop()) {
        device.writeAttr(SUMO_ATTR_BUS_STOP, myPlanElement->getParentAdditionals().front()->getID());
    } else if (tagProperty.planTrainStop()) {
        device.writeAttr(SUMO_ATTR_TRAIN_STOP, myPlanElement->getParentAdditionals().front()->getID());
    } else if (tagProperty.planContainerStop()) {
        device.writeAttr(SUMO_ATTR_CONTAINER_STOP, myPlanElement->getParentAdditionals().front()->getID());
    } else {
        // write from attribute (if this is the first element)
        if (myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement) == 0) {
            // check if write edge or junction
            if (tagProperty.planFromEdge()) {
                device.writeAttr(SUMO_ATTR_FROM, myPlanElement->getParentEdges().front()->getID());
            } else if (tagProperty.planFromTAZ()) {
                device.writeAttr(SUMO_ATTR_FROM_TAZ, myPlanElement->getParentAdditionals().front()->getID());
            } else if (tagProperty.planFromJunction()) {
                device.writeAttr(SUMO_ATTR_FROM_JUNCTION, myPlanElement->getParentJunctions().front()->getID());
            } else if (tagProperty.planFromBusStop()) {
                device.writeAttr(SUMO_ATTR_FROM_BUSSTOP, myPlanElement->getParentAdditionals().front()->getID());
            } else if (tagProperty.planFromTrainStop()) {
                device.writeAttr(SUMO_ATTR_FROM_TRAINSTOP, myPlanElement->getParentAdditionals().front()->getID());
            } else if (tagProperty.planFromContainerStop()) {
                device.writeAttr(SUMO_ATTR_FROM_CONTAINERSTOP, myPlanElement->getParentAdditionals().front()->getID());
            }
        }
        // continue writting to attribute
        if (tagProperty.planToEdge()) {
            device.writeAttr(SUMO_ATTR_TO, myPlanElement->getParentEdges().back()->getID());
        } else if (tagProperty.planToJunction()) {
            device.writeAttr(SUMO_ATTR_TO_JUNCTION, myPlanElement->getParentJunctions().back()->getID());
        } else if (tagProperty.planToTAZ()) {
            device.writeAttr(SUMO_ATTR_TO_TAZ, myPlanElement->getParentAdditionals().back()->getID());
        } else if (tagProperty.planToBusStop()) {
            device.writeAttr(SUMO_ATTR_BUS_STOP, myPlanElement->getParentAdditionals().back()->getID());
        } else if (tagProperty.planToTrainStop()) {
            device.writeAttr(SUMO_ATTR_TRAIN_STOP, myPlanElement->getParentAdditionals().back()->getID());
        } else if (tagProperty.planToContainerStop()) {
            device.writeAttr(SUMO_ATTR_CONTAINER_STOP, myPlanElement->getParentAdditionals().back()->getID());
        }
    }
    // check if write depart position
    if (tagProperty.hasAttribute(SUMO_ATTR_DEPARTPOS) && (myDepartPosition > 0)) {
        device.writeAttr(SUMO_ATTR_DEPARTPOS, myDepartPosition);
    }
    // check if write arrival position
    if (tagProperty.hasAttribute(SUMO_ATTR_ARRIVALPOS) && (myArrivalPosition > 0)) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    // check if write end position
    if (tagProperty.hasAttribute(SUMO_ATTR_ENDPOS)) {
        device.writeAttr(SUMO_ATTR_ENDPOS, myArrivalPosition);
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
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // get vclass
    auto vClass = myPlanElement->getVClass();
    // continue depending of parents
    if (tagProperty.planRoute()) {
        // route
        return myPlanElement->getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(vClass);
    } else if (tagProperty.planConsecutiveEdges() || tagProperty.planFromEdge() || tagProperty.planEdge()) {
        // edges
        return myPlanElement->getParentEdges().front()->getLaneByAllowedVClass(vClass);
    } else if (tagProperty.planStoppingPlace() || tagProperty.planFromStoppingPlace() || tagProperty.planToStoppingPlace()) {
        // additional
        return myPlanElement->getParentAdditionals().front()->getParentLanes().front();
    } else {
        // in other cases (TAZ, junctions, etc.) return null
        return nullptr;
    }
}


GNELane*
GNEDemandElementPlan::getLastPlanPathLane() const {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // get vclass
    auto vClass = myPlanElement->getVClass();
    // check parents
    if (tagProperty.planRoute()) {
        // route
        return myPlanElement->getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(vClass);
    } else if (tagProperty.planConsecutiveEdges() || tagProperty.planToEdge() || tagProperty.planEdge()) {
        // edges
        return myPlanElement->getParentEdges().back()->getLaneByAllowedVClass(vClass);
    } else if (tagProperty.planStoppingPlace() || tagProperty.planFromStoppingPlace() || tagProperty.planToStoppingPlace()) {
        // additional
        return myPlanElement->getParentAdditionals().back()->getParentLanes().front()->getParentEdge()->getLaneByAllowedVClass(vClass);
    } else {
        // in other cases (TAZ, junctions, etc.) return null
        return nullptr;
    }
}


void
GNEDemandElementPlan::computePlanPathElement() {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // get vClass
    auto vClass = myPlanElement->getVClass();
    // get path manager
    auto pathManager = myPlanElement->getNet()->getPathManager();
    // continue depending of parents
    if (tagProperty.planRoute()) {
        // calculate consecutive path using route edges
        pathManager->calculateConsecutivePathEdges(myPlanElement, vClass, myPlanElement->getParentDemandElements().at(1)->getParentEdges());
    } else if (tagProperty.planConsecutiveEdges()) {
        // calculate consecutive path using edges
        pathManager->calculateConsecutivePathEdges(myPlanElement, vClass, myPlanElement->getParentEdges());
    } else if (myPlanElement->myTagProperty.planFromJunction() && myPlanElement->myTagProperty.planToJunction()) {
        // calculate path using junctions
        pathManager->calculatePath(myPlanElement, vClass, myPlanElement->getParentJunctions().front(), myPlanElement->getParentJunctions().back());
    } else if (myPlanElement->myTagProperty.planFromJunction()) {
        // declare last lane
        GNELane* lastLane = nullptr;
        if (myPlanElement->myTagProperty.planStoppingPlace()) {
            lastLane = myPlanElement->getParentAdditionals().back()->getParentLanes().back();
        } else if (myPlanElement->myTagProperty.planToStoppingPlace()) {
            lastLane = myPlanElement->getParentAdditionals().back()->getParentLanes().front();
        } else if (myPlanElement->myTagProperty.planToEdge()) {
            lastLane = myPlanElement->getParentEdges().back()->getLaneByAllowedVClass(vClass);
        }
        // calculate path
        if (lastLane) {
            pathManager->calculatePath(myPlanElement, vClass, myPlanElement->getParentJunctions().front(), lastLane);
        }
    } else if (myPlanElement->myTagProperty.planToJunction()) {
        // declare first lane
        GNELane* firstLane = nullptr;
        if (myPlanElement->myTagProperty.planStoppingPlace()) {
            firstLane = myPlanElement->getParentAdditionals().front()->getParentLanes().back();
        } else if (myPlanElement->myTagProperty.planFromStoppingPlace()) {
            firstLane = myPlanElement->getParentAdditionals().front()->getParentLanes().front();
        } else if (myPlanElement->myTagProperty.planFromEdge()) {
            firstLane = myPlanElement->getParentEdges().front()->getLaneByAllowedVClass(vClass);
        }
        // calculate path
        if (firstLane) {
            pathManager->calculatePath(myPlanElement, vClass, firstLane, myPlanElement->getParentJunctions().back());
        }
    } else {
        // declare first edge
        GNELane* firstLane = nullptr;
        if (myPlanElement->myTagProperty.planFromEdge()) {
            firstLane = myPlanElement->getParentEdges().front()->getLaneByAllowedVClass(vClass);
        } else if (myPlanElement->myTagProperty.planStoppingPlace()) {
            firstLane = myPlanElement->getParentAdditionals().front()->getParentLanes().back();
        } else if (myPlanElement->myTagProperty.planFromStoppingPlace()) {
            firstLane = myPlanElement->getParentAdditionals().front()->getParentLanes().front();
        }
        // declare last lane
        GNELane* lastLane = nullptr;
        if (myPlanElement->myTagProperty.planToEdge()) {
            lastLane = myPlanElement->getParentEdges().back()->getLaneByAllowedVClass(vClass);
        } else if (myPlanElement->myTagProperty.planStoppingPlace()) {
            lastLane = myPlanElement->getParentAdditionals().back()->getParentLanes().back();
        } else if (myPlanElement->myTagProperty.planToStoppingPlace()) {
            lastLane = myPlanElement->getParentAdditionals().back()->getParentLanes().front();
        }
        if (firstLane && lastLane) {
            pathManager->calculatePath(myPlanElement, vClass, firstLane, lastLane);
        } else if (firstLane) {
            pathManager->calculateConsecutivePathLanes(myPlanElement, {firstLane});
        } else if (lastLane) {
            pathManager->calculateConsecutivePathLanes(myPlanElement, {lastLane});
        }
    }
    // update geometry
    updatePlanGeometry();
}


void
GNEDemandElementPlan::updatePlanGeometry() {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // declare first and last positions
    Position firstPos = Position::INVALID;
    Position lastPos = Position::INVALID;
    // set first position
    if (tagProperty.planFromJunction() && tagProperty.planToTAZ()) {
        firstPos = myPlanElement->getParentJunctions().front()->getPositionInView();
        lastPos = myPlanElement->getParentAdditionals().back()->getPositionInView();
    } else if (tagProperty.planFromTAZ() && tagProperty.planToJunction()) {
        firstPos = myPlanElement->getParentAdditionals().front()->getPositionInView();
        lastPos = myPlanElement->getParentJunctions().back()->getPositionInView();
    } else if (tagProperty.planFromTAZ() && tagProperty.planToTAZ()) {
        firstPos = myPlanElement->getParentAdditionals().front()->getPositionInView();
        lastPos = myPlanElement->getParentAdditionals().back()->getPositionInView();
    }
    // use both position to calculate a line
    if ((firstPos != Position::INVALID) && (lastPos != Position::INVALID)) {
        // remove from grid
        myPlanElement->getNet()->removeGLObjectFromGrid(myPlanElement);
        myPlanElement->myDemandElementGeometry.updateGeometry({firstPos, lastPos});
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
    // if this element is over route, add their boundary
    if (myPlanElement->getParentDemandElements().size() > 1) {
        planBoundary.add(myPlanElement->getParentDemandElements().at(1)->getCenteringBoundary());
    }
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
    // check if is valid
    if (planBoundary.isInitialised()) {
        return planBoundary;
    } else {
        return myPlanElement->getParentDemandElements().front()->getCenteringBoundary();
    }
}


Position
GNEDemandElementPlan::getPlanPositionInView() const {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // continue depending of parents
    if (tagProperty.planRoute()) {
        // route
        return myPlanElement->getParentDemandElements().at(1)->getPositionInView();
    } else if (tagProperty.isPlanStop()) {
        return myPlanElement->getDemandElementGeometry().getShape().front();
    } else if (tagProperty.planFromEdge() || tagProperty.planConsecutiveEdges() || tagProperty.planEdge()) {
        // first edge
        return myPlanElement->getParentEdges().front()->getPositionInView();
    } else if (tagProperty.planFromJunction()) {
        // first junction
        return myPlanElement->getParentJunctions().front()->getPositionInView();
    } else if (tagProperty.planStoppingPlace() || tagProperty.planFromStoppingPlace() || tagProperty.planFromTAZ()) {
        // first additional
        return myPlanElement->getParentAdditionals().front()->getPositionInView();
    } else {
        // return parent position
        return Position(0, 0);
    }
}


std::string
GNEDemandElementPlan::getPlanAttribute(SumoXMLAttr key) const {
    // continue depending of key
    switch (key) {
        // Common plan attributes
        case SUMO_ATTR_ID:
        case GNE_ATTR_PARENT:
            return myPlanElement->getParentDemandElements().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(myPlanElement->isAttributeCarrierSelected());
        case SUMO_ATTR_DEPARTPOS:
            if (myDepartPosition < 0) {
                return "";
            } else {
                return toString(myDepartPosition);
            }
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_ARRIVALPOS:
            if (myArrivalPosition < 0) {
                return "";
            } else {
                return toString(myArrivalPosition);
            }
        // route
        case SUMO_ATTR_ROUTE:
            return myPlanElement->getParentDemandElements().at(1)->getID();
        // edges
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_EDGES:
            return myPlanElement->parseIDs(myPlanElement->getParentEdges());
        // stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_TRAIN_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
            return myPlanElement->getParentAdditionals().back()->getID();
        // from elements
        case SUMO_ATTR_FROM:
            return myPlanElement->getParentEdges().front()->getID();
        case SUMO_ATTR_FROM_JUNCTION:
            return myPlanElement->getParentJunctions().front()->getID();
        case SUMO_ATTR_FROM_TAZ:
        case SUMO_ATTR_FROM_BUSSTOP:
        case SUMO_ATTR_FROM_TRAINSTOP:
            return myPlanElement->getParentAdditionals().front()->getID();
        // to elements
        case SUMO_ATTR_TO:
            return myPlanElement->getParentEdges().back()->getID();
        case SUMO_ATTR_TO_JUNCTION:
            return myPlanElement->getParentJunctions().back()->getID();
        case SUMO_ATTR_TO_TAZ:
            return myPlanElement->getParentAdditionals().back()->getID();
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEDemandElementPlan::getPlanAttributeDouble(SumoXMLAttr key) const {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // declare plan parent
    const auto planParent = myPlanElement->getParentDemandElements().at(0);
    // continue depending of key
    switch (key) {
        case GNE_ATTR_PLAN_GEOMETRY_STARTPOS: {
            if (tagProperty.planStoppingPlace()) {
                // use startpos of stoppingPlace parent (stops)
                return myPlanElement->getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_STARTPOS);
            } else if (tagProperty.planFromStoppingPlace()) {
                // use end position of stoppingPlace parent (for plans that starts in stoppingPlaces)
                return myPlanElement->getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
            } else if (tagProperty.planFromJunction() && tagProperty.planFromTAZ()) {
                // junctions and TAZs return always -1
                return -1;
            } else {
                // get previous plan element
                const auto previousPlan = planParent->getPreviousChildDemandElement(myPlanElement);
                // continue depending of previous plan
                if (previousPlan) {
                    // use previous plan end position (normally the arrival position)
                    const auto posOverLane = previousPlan->getAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
                    // if posOverLane is -1, means that previousPlan ends in the end of lane.
                    if (posOverLane == -1) {
                        // INVALID_DOUBLE will put the startPositio at the end of line
                        return INVALID_DOUBLE;
                    } else {
                        return previousPlan->getAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
                    }
                } else {
                    // use depart position defined in parent (person or container)
                    return planParent->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
                }
            }
        }
        case GNE_ATTR_PLAN_GEOMETRY_ENDPOS:
            // continue depending of parents
            if (tagProperty.planStoppingPlace()) {
                // use end position of the stoppingPlace (stops)
                return myPlanElement->getParentAdditionals().back()->getAttributeDouble(SUMO_ATTR_ENDPOS);
            } else if (tagProperty.planToStoppingPlace()) {
                // use start position of the stoppingPlace (for elements that ends in stoppingPlaces)
                return myPlanElement->getParentAdditionals().back()->getAttributeDouble(SUMO_ATTR_STARTPOS);
            } else if (tagProperty.planToJunction() || tagProperty.planToTAZ()) {
                // junctions and TAZs return always -1
                return -1;
            } else if ((tagProperty.isPlanStopPerson() || tagProperty.isPlanStopContainer()) && tagProperty.planEdge()) {
                // elements that ends in stop always uses the end (arrival) position of the stops over edges
                return myArrivalPosition;
            } else {
                // check if next plan is a stop over edge
                const auto nextPlan = planParent->getNextChildDemandElement(myPlanElement);
                if (nextPlan && (nextPlan->getTagProperty().isPlanStopPerson() ||
                                 nextPlan->getTagProperty().isPlanStopContainer()) &&
                        nextPlan->getTagProperty().planEdge()) {
                    // if next plan is an stop over stoppingPlaces, use ends of stoppingPlace
                    return nextPlan->getAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
                } else {
                    // use arrival position
                    return myArrivalPosition;
                }
            }
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have a doubleattribute of type '" + toString(key) + "'");
    }
}


Position
GNEDemandElementPlan::getPlanAttributePosition(SumoXMLAttr key) const {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // declare plan parent
    const auto planParent = myPlanElement->getParentDemandElements().at(0);
    // continue depending of key
    switch (key) {
        case GNE_ATTR_PLAN_GEOMETRY_STARTPOS: {
            // get previous plan
            const auto previousPlan = planParent->getPreviousChildDemandElement(myPlanElement);
            if (previousPlan && previousPlan->getTagProperty().isPlanStop() && previousPlan->getTagProperty().planStoppingPlace()) {
                return previousPlan->getParentAdditionals().front()->getAdditionalGeometry().getShape().back();
            }
            // continue depending of from element
            if (tagProperty.planStoppingPlace()) {
                return myPlanElement->getParentAdditionals().front()->getAdditionalGeometry().getShape().front();
            } else if (tagProperty.planFromStoppingPlace()) {
                return myPlanElement->getParentAdditionals().front()->getAdditionalGeometry().getShape().back();
            } else if (tagProperty.planFromJunction()) {
                // junction view position
                return myPlanElement->getParentJunctions().front()->getPositionInView();
            } else if (tagProperty.planFromTAZ()) {
                // TAZ view position
                return myPlanElement->getParentAdditionals().front()->getPositionInView();
            } else if (tagProperty.planConsecutiveEdges() || tagProperty.planRoute() || tagProperty.planFromEdge()) {
                // get first path lane
                const auto firstLane = myPlanElement->getFirstPathLane();
                // check if first lane exists
                if (firstLane == nullptr) {
                    return Position::INVALID;
                }
                // declare lane position
                double lanePosition = 0;
                // continue depending of conditions
                if (previousPlan) {
                    // use previous geometry end position
                    lanePosition = previousPlan->getAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
                } else if (myDepartPosition < 0) {
                    // use departPos defined in planParent
                    lanePosition = planParent->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
                } else {
                    // use departPos (only for tranships)
                    lanePosition = myDepartPosition;
                }
                // get lane shape
                const auto& laneShape = firstLane->getLaneShape();
                // continue depending of lane position
                if (lanePosition <= 0) {
                    return laneShape.front();
                } else if (lanePosition >= laneShape.length2D()) {
                    return laneShape.back();
                } else {
                    return laneShape.positionAtOffset2D(lanePosition);
                }
            } else {
                return Position::INVALID;
            }
        }
        case GNE_ATTR_PLAN_GEOMETRY_ENDPOS: {
            // check parents
            if (tagProperty.planToJunction()) {
                // junctions
                return myPlanElement->getParentJunctions().back()->getPositionInView();
            } else if (tagProperty.planToTAZ()) {
                // taz
                return myPlanElement->getParentAdditionals().back()->getPositionInView();
            } else if (tagProperty.planStoppingPlace()) {
                // get additional back shape (stops)
                return myPlanElement->getParentAdditionals().back()->getAdditionalGeometry().getShape().back();
            } else if (tagProperty.planToStoppingPlace()) {
                // get additional front shape
                return myPlanElement->getParentAdditionals().back()->getAdditionalGeometry().getShape().front();
            } else if (tagProperty.planConsecutiveEdges() || tagProperty.planRoute() || tagProperty.planFromEdge()) {
                // get next plan
                const auto nextPlan = planParent->getNextChildDemandElement(myPlanElement);
                // if next plan exist, then use their first lane (needed to maintain connectivity with rides)
                const auto lastLane = nextPlan ? nextPlan->getFirstPathLane() : myPlanElement->getLastPathLane();
                // check if last lane exists
                if (lastLane == nullptr) {
                    return Position::INVALID;
                }
                // get lane shape
                const auto& laneShape = lastLane->getLaneShape();
                // continue depending of arrival position
                if (nextPlan && nextPlan->getTagProperty().isPlanStop()) {
                    return nextPlan->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
                } else if (myArrivalPosition == 0) {
                    return laneShape.front();
                } else if ((myArrivalPosition == -1) || (myArrivalPosition >= laneShape.length2D())) {
                    return laneShape.back();
                } else {
                    return laneShape.positionAtOffset2D(myArrivalPosition);
                }
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
    // continue depending of key
    switch (key) {
        // common attributes
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_ARRIVALPOS:
        case SUMO_ATTR_ENDPOS:
        case GNE_ATTR_SELECTED:
            GNEChange_Attribute::changeAttribute(myPlanElement, key, value, undoList);
            break;
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDemandElementPlan::isPlanValid(SumoXMLAttr key, const std::string& value) {
    // continue depending of key
    switch (key) {
        // common attributes
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                return true;
            } else if (GNEAttributeCarrier::canParse<double>(value)) {
                return GNEAttributeCarrier::parse<double>(value) >= 0;
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            return GNEAttributeCarrier::canParse<double>(value);
        case GNE_ATTR_SELECTED:
            return GNEAttributeCarrier::canParse<bool>(value);
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDemandElementPlan::isPlanAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        // edges
        case SUMO_ATTR_EDGES:
        // edge
        case SUMO_ATTR_EDGE:
        // route
        case SUMO_ATTR_ROUTE:
        // from
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_FROM_JUNCTION:
        case SUMO_ATTR_FROM_TAZ:
        case SUMO_ATTR_FROM_BUSSTOP:
        case SUMO_ATTR_FROM_TRAINSTOP:
        // to
        case SUMO_ATTR_TO:
        case SUMO_ATTR_TO_JUNCTION:
        case SUMO_ATTR_TO_TAZ:
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_TRAIN_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
            return false;
        case SUMO_ATTR_DEPARTPOS:
            // depart position only enabled for first plan element
            if (myPlanElement->getParentDemandElements().size() > 0) {
                return myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement) == 0;
            } else {
                return false;
            }
        default:
            return true;
    }
}


void
GNEDemandElementPlan::setPlanAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common plan attributes
        case SUMO_ATTR_DEPARTPOS:
            if (value.empty()) {
                myDepartPosition = -1;
            } else {
                myDepartPosition = GNEAttributeCarrier::parse<double>(value);
            }
            break;
        case SUMO_ATTR_ENDPOS:
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
            // check if compute geometry and path
            if (!myPlanElement->isTemplate()) {
                myPlanElement->updateGeometry();
                myPlanElement->computePathElement();
            }
            break;
        default:
            throw InvalidArgument(myPlanElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEDemandElementPlan::getPlanHierarchyName() const {
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // declare result
    std::string result;
    // clear result
    int index = 0;
    while (tagProperty.getTagStr().at(index) != ':') {
        result.push_back(tagProperty.getTagStr().at(index));
        index++;
    }
    result += ": ";
    // continue depending of attributes
    if (tagProperty.planConsecutiveEdges()) {
        // edges
        return result + myPlanElement->getParentEdges().front()->getID() + " ... " + myPlanElement->getParentEdges().back()->getID();
    } else if (tagProperty.planRoute()) {
        // route
        return result + myPlanElement->getParentDemandElements().at(1)->getID();
    } else if (tagProperty.planEdge()) {
        // edge
        return result + myPlanElement->getParentEdges().front()->getID();
    } else if (tagProperty.planStoppingPlace()) {
        // stoppingPlace
        return myPlanElement->getParentAdditionals().front()->getID();
    } else {
        // additional
        if (tagProperty.planFromStoppingPlace() || tagProperty.planFromTAZ()) {
            result += myPlanElement->getParentAdditionals().front()->getID();
        }
        // junction
        if (tagProperty.planFromJunction()) {
            result += myPlanElement->getParentJunctions().front()->getID();
        }
        // edge
        if (tagProperty.planFromEdge()) {
            result += myPlanElement->getParentEdges().front()->getID();
        }
        // arrow
        result += " -> ";
        // additional
        if (tagProperty.planToStoppingPlace() || tagProperty.planToTAZ()) {
            result += myPlanElement->getParentAdditionals().back()->getID();
        }
        // junction
        if (tagProperty.planToJunction()) {
            result += myPlanElement->getParentJunctions().back()->getID();
        }
        // edge
        if (tagProperty.planToEdge()) {
            result += myPlanElement->getParentEdges().back()->getID();
        }
        return result;
    }
}


bool
GNEDemandElementPlan::checkDrawPersonPlan() const {
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
        if (AC->getTagProperty().isPlanPerson() && AC->getAttribute(GNE_ATTR_PARENT) == myPlanElement->getAttribute(GNE_ATTR_PARENT)) {
            // common person parent
            return true;
        } else {
            // all conditions are false
            return false;
        }
    }
}


bool
GNEDemandElementPlan::checkDrawContainerPlan() const {
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
        if (AC->getTagProperty().isPlanContainer() && AC->getAttribute(GNE_ATTR_PARENT) == myPlanElement->getAttribute(GNE_ATTR_PARENT)) {
            // common container parent
            return true;
        } else {
            // all conditions are false
            return false;
        }
    }
}


void
GNEDemandElementPlan::drawPlanGL(const bool drawPlan, const GUIVisualizationSettings& s, const RGBColor& planColor, const RGBColor& planSelectedColor) const {
    // get plan parent
    const GNEDemandElement* planParent = myPlanElement->getParentDemandElements().front();
    // get tag property
    const auto tagProperty = myPlanElement->getTagProperty();
    // get plan geometry
    auto& planGeometry = myPlanElement->myDemandElementGeometry;
    // draw relations between TAZs
    if (drawPlan && (planGeometry.getShape().size() > 0)) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // get viewNet
        auto viewNet = myPlanElement->getNet()->getViewNet();
        // get inspected attribute carriers
        const auto& inspectedACs = viewNet->getInspectedAttributeCarriers();
        // get inspected plan
        const GNEAttributeCarrier* planInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (planInspected == myPlanElement) || (planInspected == planParent);
        // calculate path width
        const double pathWidth = 0.25 * (duplicateWidth ? 2 : 1);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (!s.drawForObjectUnderCursor) {
            // push matrix
            GLHelper::pushMatrix();
            // translate to front
            viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, GLO_TAZ + 1);
            // set color
            GLHelper::setColor(myPlanElement->drawUsingSelectColor() ? planSelectedColor : planColor);
            // draw line
            GUIGeometry::drawGeometry(d, planGeometry, pathWidth);
            GLHelper::drawTriangleAtEnd(
                *(planGeometry.getShape().end() - 2),
                *(planGeometry.getShape().end() - 1),
                0.5, 0.5, 0.5);
            // pop matrix
            GLHelper::popMatrix();
        }
        // draw dotted geometry
        myPlanElement->getContour().drawDottedContourExtruded(s, d, planGeometry.getShape(), pathWidth, 1, true, true,
                s.dottedContourSettings.segmentWidth);
    }
    // check if draw plan parent
    if (planParent->getPreviousChildDemandElement(myPlanElement) == nullptr) {
        planParent->drawGL(s);
    }
}


void
GNEDemandElementPlan::drawPlanLanePartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment,
        const double offsetFront, const double planWidth, const RGBColor& planColor, const RGBColor& planSelectedColor) const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // get plan parent
    const GNEDemandElement* planParent = myPlanElement->getParentDemandElements().front();
    // check if this is a dotted element
    const bool dottedElement = myPlanElement->checkDrawContour();
    // check if draw plan element can be drawn
    if (drawPlan && segment->getLane() && myPlanElement->getNet()->getPathManager()->getPathDraw()->checkDrawPathGeometry(s, dottedElement, segment->getLane(), myPlanElement->getTagProperty().getTag())) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // get inspected attribute carriers
        const auto& inspectedACs = viewNet->getInspectedAttributeCarriers();
        // get inspected plan
        const GNEAttributeCarrier* planInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (planInspected == myPlanElement) || (planInspected == planParent);
        // calculate path width
        const double pathWidth = s.addSize.getExaggeration(s, segment->getLane()) * planWidth * (duplicateWidth ? 2 : 1);
        // declare path geometry
        GUIGeometry planGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            planGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                        getPlanAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_STARTPOS),
                                        getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS),
                                        getPlanAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS),
                                        getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS));
        } else if (segment->isFirstSegment()) {
            planGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                        getPlanAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_STARTPOS),
                                        getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS),
                                        -1,
                                        Position::INVALID);
        } else if (segment->isLastSegment()) {
            planGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                        -1,
                                        Position::INVALID,
                                        getPlanAttributeDouble(GNE_ATTR_PLAN_GEOMETRY_ENDPOS),
                                        getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS));
        } else {
            planGeometry = segment->getLane()->getLaneGeometry();
        }
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, myPlanElement->getType(), offsetFront);
        // Set color
        GLHelper::setColor(myPlanElement->drawUsingSelectColor() ? planSelectedColor : planColor);
        // draw geometry
        GUIGeometry::drawGeometry(d, planGeometry, pathWidth);
        // draw red arrows
        drawFromArrow(s, segment->getLane(), segment, dottedElement);
        drawToArrow(s, segment->getLane(), segment, dottedElement);
        // draw end position
        drawEndPosition(s, d, segment, duplicateWidth);
        // Pop last matrix
        GLHelper::popMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            myPlanElement->drawName(myPlanElement->getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // declare trim geometry to draw
        const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? planGeometry.getShape() : segment->getLane()->getLaneShape();
        // draw dotted geometry
        if (duplicateWidth) {
            myPlanElement->getContour().drawDottedContourExtruded(s, d, shape, pathWidth, 1, true, true,
                    s.dottedContourSettings.segmentWidth);
        } else {
            myPlanElement->getContour().drawDottedContourExtruded(s, d, shape, pathWidth, 1, true, true,
                    s.dottedContourSettings.segmentWidthSmall);
        }
    }
    // check if draw plan parent
    if (planParent->getPreviousChildDemandElement(myPlanElement) == nullptr) {
        planParent->drawGL(s);
    }
}


void
GNEDemandElementPlan::drawPlanJunctionPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment,
        const double offsetFront, const double planWidth, const RGBColor& planColor, const RGBColor& planSelectedColor) const {
    // get view net
    auto viewNet = myPlanElement->getNet()->getViewNet();
    // get plan parent
    const GNEDemandElement* planParent = myPlanElement->getParentDemandElements().front();
    // check if draw plan elements can be drawn
    if (drawPlan && myPlanElement->getNet()->getPathManager()->getPathDraw()->checkDrawPathGeometry(s, myPlanElement->checkDrawContour(), segment, myPlanElement->getTagProperty().getTag())) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // get inspected attribute carriers
        const auto& inspectedACs = viewNet->getInspectedAttributeCarriers();
        // get inspected plan
        const GNEAttributeCarrier* planInspected = (inspectedACs.size() > 0) ? inspectedACs.front() : nullptr;
        // flag to check if width must be duplicated
        const bool duplicateWidth = (planInspected == myPlanElement) || (planInspected == planParent);
        // calculate path width
        const double pathWidth = s.addSize.getExaggeration(s, segment->getPreviousLane()) * planWidth * (duplicateWidth ? 2 : 1);
        // push a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        viewNet->drawTranslateFrontAttributeCarrier(myPlanElement, myPlanElement->getType(), offsetFront);
        // Set plan color
        GLHelper::setColor(myPlanElement->drawUsingSelectColor() ? planSelectedColor : planColor);
        // check if draw lane2lane connection or a red line
        if (segment->getPreviousLane() && segment->getNextLane()) {
            if (segment->getPreviousLane()->getLane2laneConnections().exist(segment->getNextLane())) {
                // obtain lane2lane geometry
                const GUIGeometry& lane2laneGeometry = segment->getPreviousLane()->getLane2laneConnections().getLane2laneGeometry(segment->getNextLane());
                // draw lane2lane
                GUIGeometry::drawGeometry(d, lane2laneGeometry, pathWidth);
            } else {
                // Set invalid plan color
                GLHelper::setColor(RGBColor::RED);
                // draw line between end of first shape and first position of second shape
                GLHelper::drawBoxLines({segment->getPreviousLane()->getLaneShape().back(), segment->getNextLane()->getLaneShape().front()}, (0.5 * pathWidth));
            }
        } else if (segment->getPreviousLane()) {
            // draw line between center of junction and last lane shape
            GLHelper::drawBoxLines({segment->getPreviousLane()->getLaneShape().back(), myPlanElement->getParentJunctions().back()->getPositionInView()}, pathWidth);
        } else if (segment->getNextLane()) {
            // draw line between center of junction and first lane shape
            GLHelper::drawBoxLines({myPlanElement->getParentJunctions().front()->getPositionInView(), segment->getNextLane()->getLaneShape().front()}, pathWidth);
        }
        // Pop last matrix
        GLHelper::popMatrix();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(s, d, myPlanElement, myPlanElement->getType(), myPlanElement->getPositionInView(), 0.5);
        // check if shape dotted contour has to be drawn
        if (segment->getPreviousLane() && segment->getNextLane()) {
            if (segment->getPreviousLane()->getLane2laneConnections().exist(segment->getNextLane())) {
                // get shape
                const auto& shape = segment->getPreviousLane()->getLane2laneConnections().getLane2laneGeometry(segment->getNextLane()).getShape();
                // draw dotted geometry
                if (duplicateWidth) {
                    myPlanElement->getContour().drawDottedContourExtruded(s, d, shape, pathWidth, 1, true, true,
                            s.dottedContourSettings.segmentWidth);
                } else {
                    myPlanElement->getContour().drawDottedContourExtruded(s, d, shape, pathWidth, 1, true, true,
                            s.dottedContourSettings.segmentWidthSmall);
                }
            }
        } else if (segment->getPreviousLane()) {
            myPlanElement->getContour().drawDottedContourExtruded(s, d, {segment->getPreviousLane()->getLaneShape().back(), myPlanElement->getParentJunctions().back()->getPositionInView()},
                    pathWidth, 1, true, true, s.dottedContourSettings.segmentWidth);
        } else if (segment->getNextLane()) {
            myPlanElement->getContour().drawDottedContourExtruded(s, d, {myPlanElement->getParentJunctions().front()->getPositionInView(), segment->getNextLane()->getLaneShape().front()},
                    pathWidth, 1, true, true, s.dottedContourSettings.segmentWidth);
        }
    }
    // check if draw plan parent
    if (planParent->getPreviousChildDemandElement(myPlanElement) == nullptr) {
        planParent->drawGL(s);
    }
}


GNEDemandElement::Problem
GNEDemandElementPlan::isPlanPersonValid() const {
    // get previous plan
    const auto previousPlan = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
    if (previousPlan) {
        // get previous lane
        const auto previousLastLane = previousPlan->getLastPathLane();
        // get first lane
        const auto firstLane = myPlanElement->getFirstPathLane();
        // compare edges
        if (previousLastLane && firstLane && (previousLastLane->getParentEdge() != firstLane->getParentEdge())) {
            return GNEDemandElement::Problem::DISCONNECTED_PLAN;
        }
        // in the future, check more elements
    }
    // get next child
    const auto nextPlan = myPlanElement->getParentDemandElements().at(0)->getNextChildDemandElement(myPlanElement);
    if (nextPlan) {
        // get previous lane
        const auto nextFirstLane = nextPlan->getFirstPathLane();
        // get first lane
        const auto lastLane = myPlanElement->getLastPathLane();
        // compare edges
        if (nextFirstLane && lastLane && (nextFirstLane->getParentEdge() != lastLane->getParentEdge())) {
            return GNEDemandElement::Problem::DISCONNECTED_PLAN;
        }
        // in the future, check more elements
    }
    // all ok, then return true
    return GNEDemandElement::Problem::OK;
}


std::string
GNEDemandElementPlan::getPersonPlanProblem() const {
    // get previous plan
    const auto previousPlan = myPlanElement->getParentDemandElements().at(0)->getPreviousChildDemandElement(myPlanElement);
    if (previousPlan) {
        // get previous lane
        const auto previousLastLane = previousPlan->getLastPathLane();
        // get first lane
        const auto firstLane = myPlanElement->getLastPathLane();
        // compare edges
        if (previousLastLane && firstLane && (previousLastLane->getParentEdge() != firstLane->getParentEdge())) {
            return TLF("Edge '%' is not consecutive with edge '%'", previousLastLane->getParentEdge()->getID(), firstLane->getParentEdge()->getID());
        }
        // in the future, check more elements
    }
    // get next child
    const auto nextPlan = myPlanElement->getParentDemandElements().at(0)->getNextChildDemandElement(myPlanElement);
    if (nextPlan) {
        // get previous lane
        const auto nextFirstLane = nextPlan->getFirstPathLane();
        // get first lane
        const auto lastLane = myPlanElement->getLastPathLane();
        // compare edges
        if (nextFirstLane && lastLane && (nextFirstLane->getParentEdge() != lastLane->getParentEdge())) {
            return TLF("Edge '%' is not consecutive with edge '%'", nextFirstLane->getParentEdge()->getID(), lastLane->getParentEdge()->getID());
        }
        // in the future, check more elements
    }
    // undefined problem
    return "undefined problem";
}


void
GNEDemandElementPlan::drawFromArrow(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment,
                                    const bool dottedElement) const {
    // draw ifcurrent amd next segment is placed over lanes
    if (segment->getNextLane()) {
        // get firstPosition (last position of current lane shape)
        const Position from = lane->getLaneShape().back();
        // get lastPosition (first position of next lane shape)
        const Position to = segment->getNextLane()->getLaneShape().front();
        // push draw matrix
        GLHelper::pushMatrix();
        // move front
        glTranslated(0, 0, 4);
        // draw child line
        GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || myPlanElement->isAttributeCarrierSelected(), .05);
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


void
GNEDemandElementPlan::drawToArrow(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment,
                                  const bool dottedElement) const {
    // draw the line if previos segment and current segment is placed over lanes
    if (segment->getPreviousLane()) {
        // get firstPosition (last position of current lane shape)
        const Position from = lane->getLaneShape().front();
        // get lastPosition (first position of next lane shape)
        const Position to = segment->getPreviousLane()->getLaneShape().back();
        // push draw matrix
        GLHelper::pushMatrix();
        // move front
        glTranslated(0, 0, 4);
        // draw child line
        GUIGeometry::drawChildLine(s, from, to, RGBColor::RED, dottedElement || myPlanElement->isAttributeCarrierSelected(), .05);
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


void
GNEDemandElementPlan::drawEndPosition(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const GNEPathManager::Segment* segment, const bool duplicateWidth) const {
    // check if myPlanElement is the last segment
    if (segment->isLastSegment()) {
        // calculate circle width
        const double circleRadius = (duplicateWidth ? myArrivalPositionDiameter : (myArrivalPositionDiameter / 2.0));
        const double circleWidth = circleRadius * MIN2((double)0.5, s.laneWidthExaggeration);
        const double circleWidthSquared = circleWidth * circleWidth;
        // get geometryEndPos
        const Position geometryEndPos = getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
        // check if endPos can be drawn
        if (!s.drawForRectangleSelection || (myPlanElement->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= (circleWidthSquared + 2))) {
            // push draw matrix
            GLHelper::pushMatrix();
            // translate to pos and move to
            glTranslated(geometryEndPos.x(), geometryEndPos.y(), 4);
            // resolution of drawn circle depending of the zoom (To improve smothness)
            GLHelper::drawFilledCircleDetailled(d, circleWidth);
            // pop draw matrix
            GLHelper::popMatrix();
        }
    }
}

/****************************************************************************/
