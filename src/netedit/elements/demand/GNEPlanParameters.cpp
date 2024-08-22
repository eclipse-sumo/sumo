/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNERouteHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// Builds demand objects for netedit
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <utils/common/StringTokenizer.h>

#include "GNEContainer.h"
#include "GNEPerson.h"
#include "GNEPersonTrip.h"
#include "GNERide.h"
#include "GNERoute.h"
#include "GNERouteDistribution.h"
#include "GNEPlanParameters.h"
#include "GNEStop.h"
#include "GNETranship.h"
#include "GNETransport.h"
#include "GNEVehicle.h"
#include "GNEVType.h"
#include "GNEVTypeDistribution.h"
#include "GNEWalk.h"
#include "GNEStopPlan.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEPlanParameters::GNEPlanParameters() {}


GNEPlanParameters::GNEPlanParameters(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                                     const CommonXMLStructure::PlanParameters& planParameters,
                                     const GNENetHelper::AttributeCarriers* ACs) {
    // junctions
    fromJunction = ACs->retrieveJunction(planParameters.fromJunction, false);
    toJunction = ACs->retrieveJunction(planParameters.toJunction, false);
    // edges
    fromEdge = ACs->retrieveEdge(planParameters.fromEdge, false);
    toEdge = ACs->retrieveEdge(planParameters.toEdge, false);
    // TAZs
    fromTAZ = ACs->retrieveAdditional(SUMO_TAG_TAZ, planParameters.fromTAZ, false);
    toTAZ = ACs->retrieveAdditional(SUMO_TAG_TAZ, planParameters.toTAZ, false);
    // bus stops
    if (fromStoppingPlace == nullptr) {
        fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.fromBusStop, false);
    }
    if (toStoppingPlace == nullptr) {
        toStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.toBusStop, false);
    }
    // train stops
    if (fromStoppingPlace == nullptr) {
        fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.fromTrainStop, false);
    }
    if (toStoppingPlace == nullptr) {
        toStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.toTrainStop, false);
    }
    // container stops
    if (fromStoppingPlace == nullptr) {
        fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, planParameters.fromContainerStop, false);
    }
    if (toStoppingPlace == nullptr) {
        toStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, planParameters.toContainerStop, false);
    }
    // charging station
    if (fromStoppingPlace == nullptr) {
        fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, planParameters.fromChargingStation, false);
    }
    if (toStoppingPlace == nullptr) {
        toStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, planParameters.toChargingStation, false);
    }
    // parking area
    if (fromStoppingPlace == nullptr) {
        fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, planParameters.fromParkingArea, false);
    }
    if (toStoppingPlace == nullptr) {
        toStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, planParameters.toParkingArea, false);
    }
    // edges
    for (const auto& edgeID : planParameters.consecutiveEdges) {
        auto parsedEdge = ACs->retrieveEdge(edgeID, false);
        // avoid null and consecutive dulicated edges
        if (parsedEdge && (consecutiveEdges.empty() || (consecutiveEdges.back() != parsedEdge))) {
            consecutiveEdges.push_back(parsedEdge);
        }
    }
    // route
    route = ACs->retrieveDemandElement(SUMO_TAG_ROUTE, planParameters.route, false);
    // stops
    edge = ACs->retrieveEdge(planParameters.edge, false);
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.busStop, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.trainStop, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, planParameters.containerStop, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, planParameters.chargingStation, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, planParameters.parkingArea, false);
    }
    // update from attributes
    updateFromAttributes(sumoBaseObject, ACs);
}


void
GNEPlanParameters::addChildElements(GNEDemandElement* element) {
    if (fromJunction) {
        fromJunction->addChildElement(element);
    }
    if (toJunction) {
        toJunction->addChildElement(element);
    }
    if (fromEdge) {
        fromEdge->addChildElement(element);
    }
    if (toEdge) {
        toEdge->addChildElement(element);
    }
    if (fromTAZ) {
        fromTAZ->addChildElement(element);
    }
    if (toTAZ) {
        toTAZ->addChildElement(element);
    }
    if (fromStoppingPlace) {
        fromStoppingPlace->addChildElement(element);
    }
    if (toStoppingPlace) {
        toStoppingPlace->addChildElement(element);
    }
    for (const auto& it : consecutiveEdges) {
        it->addChildElement(element);
    }
    if (route) {
        route->addChildElement(element);
    }
    if (edge) {
        edge->addChildElement(element);
    }
    if (stoppingPlace) {
        stoppingPlace->addChildElement(element);
    }
}


void
GNEPlanParameters::clear() {
    fromJunction = nullptr;
    toJunction = nullptr;
    fromEdge = nullptr;
    toEdge = nullptr;
    fromTAZ = nullptr;
    toTAZ = nullptr;
    fromStoppingPlace = nullptr;
    toStoppingPlace = nullptr;
    consecutiveEdges.clear();
    route = nullptr;
    edge = nullptr;
    stoppingPlace = nullptr;
}


bool
GNEPlanParameters::getFromBusStop() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getFromTrainStop() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getFromContainerStop() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getFromChargingStation() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getFromParkingArea() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getToBusStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getToTrainStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getToContainerStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getToChargingStation() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getToParkingArea() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getBusStop() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getTrainStop() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getContainerStop() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getChargingStation() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION);
    } else {
        return false;
    }
}


bool
GNEPlanParameters::getParkingArea() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA);
    } else {
        return false;
    }
}


std::vector<GNEJunction*>
GNEPlanParameters::getJunctions() const {
    std::vector<GNEJunction*> junctions;
    if (fromJunction) {
        junctions.push_back(fromJunction);
    }
    if (toJunction) {
        junctions.push_back(toJunction);
    }
    return junctions;
}


std::vector<GNEEdge*>
GNEPlanParameters::getEdges() const {
    if (consecutiveEdges.size() > 0) {
        return consecutiveEdges;
    } else {
        std::vector<GNEEdge*> edges;
        if (fromEdge) {
            edges.push_back(fromEdge);
        }
        if (toEdge) {
            edges.push_back(toEdge);
        }
        if (edge) {
            edges.push_back(edge);
        }
        return edges;
    }
}


std::vector<GNEAdditional*>
GNEPlanParameters::getAdditionalElements() const {
    std::vector<GNEAdditional*> additionals;
    if (fromStoppingPlace) {
        additionals.push_back(fromStoppingPlace);
    }
    if (toStoppingPlace) {
        additionals.push_back(toStoppingPlace);
    }
    if (fromTAZ) {
        additionals.push_back(fromTAZ);
    }
    if (toTAZ) {
        additionals.push_back(toTAZ);
    }
    if (stoppingPlace) {
        additionals.push_back(stoppingPlace);
    }
    return additionals;
}


std::vector<GNEDemandElement*>
GNEPlanParameters::getDemandElements(GNEDemandElement* parent) const {
    std::vector<GNEDemandElement*> demandElements;
    // always add parent first
    demandElements.push_back(parent);
    if (route) {
        demandElements.push_back(route);
    }
    return demandElements;
}


const CommonXMLStructure::SumoBaseObject*
GNEPlanParameters::getPreviousPlanObj(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    // first check if object exist
    if (sumoBaseObject == nullptr) {
        return nullptr;
    }
    // check if object has parent
    const CommonXMLStructure::SumoBaseObject* parentObject = sumoBaseObject->getParentSumoBaseObject();
    if (parentObject == nullptr) {
        return nullptr;
    }
    // check number of children
    if (parentObject->getSumoBaseObjectChildren().size() < 2) {
        return nullptr;
    }
    // search position of the given plan obj in the parent children
    const auto objIterator = std::find(parentObject->getSumoBaseObjectChildren().begin(), parentObject->getSumoBaseObjectChildren().end(), sumoBaseObject);
    // if obj is the first plan of person/container parent, then return null. If not, return previous object
    if (objIterator == parentObject->getSumoBaseObjectChildren().begin()) {
        return nullptr;
    } else {
        return *(objIterator - 1);
    }
}


void
GNEPlanParameters::updateFromAttributes(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                                        const GNENetHelper::AttributeCarriers* ACs) {
    // check if previous plan object was defined but not the from
    const auto previousPlanObj = getPreviousPlanObj(sumoBaseObject);
    if (previousPlanObj && !(fromJunction || fromEdge || fromTAZ || fromStoppingPlace)) {
        // edges
        if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TO)) {
            fromEdge = ACs->retrieveEdge(previousPlanObj->getStringAttribute(SUMO_ATTR_TO), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_EDGE)) {
            fromEdge = ACs->retrieveEdge(previousPlanObj->getStringAttribute(SUMO_ATTR_EDGE), false);
        } else if (previousPlanObj->hasStringListAttribute(SUMO_ATTR_EDGES)) {
            auto previousEdges = previousPlanObj->getStringListAttribute(SUMO_ATTR_EDGE);
            if (previousEdges.size() > 0) {
                fromEdge = ACs->retrieveEdge(previousEdges.back(), false);
            }
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_ROUTE)) {
            auto previousRoute = ACs->retrieveEdge(previousPlanObj->getStringAttribute(SUMO_ATTR_ROUTE), false);
            if (previousRoute) {
                fromEdge = previousRoute->getParentEdges().back();
            }
            // junction
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TO_JUNCTION)) {
            fromJunction = ACs->retrieveJunction(previousPlanObj->getStringAttribute(SUMO_ATTR_TO_JUNCTION), false);
            // TAZ
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TO_TAZ)) {
            fromTAZ = ACs->retrieveAdditional(SUMO_TAG_TAZ, previousPlanObj->getStringAttribute(SUMO_ATTR_FROM_TAZ), false);
            // stoppingPlaces
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_BUS_STOP)) {
            fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_BUS_STOP), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TRAIN_STOP)) {
            fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_TRAIN_STOP), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_CONTAINER_STOP)) {
            fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_CONTAINER_STOP), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_CHARGING_STATION)) {
            fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, previousPlanObj->getStringAttribute(SUMO_ATTR_CHARGING_STATION), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_PARKING_AREA)) {
            fromStoppingPlace = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, previousPlanObj->getStringAttribute(SUMO_ATTR_PARKING_AREA), false);
        }
    }
}

/****************************************************************************/
