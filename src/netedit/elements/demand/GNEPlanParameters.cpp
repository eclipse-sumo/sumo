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
    fromBusStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.fromBusStop, false);
    toBusStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.toBusStop, false);
    // train stops
    fromTrainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.fromTrainStop, false);
    toTrainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.toTrainStop, false);
    // container stops
    fromContainerStop = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, planParameters.fromContainerStop, false);
    toContainerStop = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, planParameters.toContainerStop, false);
    // charging station
    fromChargingStation = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, planParameters.fromChargingStation, false);
    toChargingStation = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, planParameters.toChargingStation, false);
    // parking area
    fromParkingArea = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, planParameters.fromParkingArea, false);
    toParkingArea = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, planParameters.toParkingArea, false);
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
    edge = ACs->retrieveEdge(planParameters.fromEdge, false);
    busStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.busStop, false);
    trainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.trainStop, false);
    containerStop = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, planParameters.containerStop, false);
    chargingStation = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, planParameters.chargingStation, false);
    parkingArea = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, planParameters.parkingArea, false);
    // due busStops and trainStops share namespace, we need to check in both directions
    if ((fromBusStop == nullptr) && (fromTrainStop == nullptr)) {
        fromBusStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.fromTrainStop, false);
        fromTrainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.fromBusStop, false);
    }
    if ((toBusStop == nullptr) && (toTrainStop == nullptr)) {
        toBusStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.toTrainStop, false);
        toTrainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.toBusStop, false);
    }
    if ((busStop == nullptr) && (trainStop == nullptr)) {
        busStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.trainStop, false);
        trainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.busStop, false);
    }
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
    if (fromBusStop) {
        fromBusStop->addChildElement(element);
    }
    if (toBusStop) {
        toBusStop->addChildElement(element);
    }
    if (fromTrainStop) {
        fromTrainStop->addChildElement(element);
    }
    if (toTrainStop) {
        toTrainStop->addChildElement(element);
    }
    if (fromContainerStop) {
        fromContainerStop->addChildElement(element);
    }
    if (toContainerStop) {
        toContainerStop->addChildElement(element);
    }
    if (fromChargingStation) {
        fromChargingStation->addChildElement(element);
    }
    if (toChargingStation) {
        toChargingStation->addChildElement(element);
    }
    if (fromParkingArea) {
        fromParkingArea->addChildElement(element);
    }
    if (toParkingArea) {
        toParkingArea->addChildElement(element);
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
    if (busStop) {
        busStop->addChildElement(element);
    }
    if (trainStop) {
        trainStop->addChildElement(element);
    }
    if (chargingStation) {
        chargingStation->addChildElement(element);
    }
    if (containerStop) {
        containerStop->addChildElement(element);
    }
    if (parkingArea) {
        parkingArea->addChildElement(element);
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
    if (fromBusStop) {
        additionals.push_back(fromBusStop);
    }
    if (toBusStop) {
        additionals.push_back(toBusStop);
    }
    if (fromTrainStop) {
        additionals.push_back(fromTrainStop);
    }
    if (toTrainStop) {
        additionals.push_back(toTrainStop);
    }
    if (fromContainerStop) {
        additionals.push_back(fromContainerStop);
    }
    if (toContainerStop) {
        additionals.push_back(toContainerStop);
    }
    if (fromChargingStation) {
        additionals.push_back(fromChargingStation);
    }
    if (toChargingStation) {
        additionals.push_back(toChargingStation);
    }
    if (fromParkingArea) {
        additionals.push_back(fromParkingArea);
    }
    if (toParkingArea) {
        additionals.push_back(toParkingArea);
    }
    if (fromTAZ) {
        additionals.push_back(fromTAZ);
    }
    if (toTAZ) {
        additionals.push_back(toTAZ);
    }
    if (busStop) {
        additionals.push_back(busStop);
    }
    if (trainStop) {
        additionals.push_back(trainStop);
    }
    if (chargingStation) {
        additionals.push_back(chargingStation);
    }
    if (containerStop) {
        additionals.push_back(containerStop);
    }
    if (parkingArea) {
        additionals.push_back(parkingArea);
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
    if (previousPlanObj && !(fromJunction || fromEdge || fromTAZ || fromBusStop || fromTrainStop || fromContainerStop || fromChargingStation)) {
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
            fromBusStop = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_BUS_STOP), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TRAIN_STOP)) {
            fromTrainStop = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_TRAIN_STOP), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_CONTAINER_STOP)) {
            fromContainerStop = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_CONTAINER_STOP), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_CHARGING_STATION)) {
            fromChargingStation = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, previousPlanObj->getStringAttribute(SUMO_ATTR_CHARGING_STATION), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_PARKING_AREA)) {
            fromParkingArea = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, previousPlanObj->getStringAttribute(SUMO_ATTR_PARKING_AREA), false);
        }
    }
}

/****************************************************************************/
