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
#include "GNEPlanParents.h"
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

GNEPlanParents::GNEPlanParents() {}


GNEPlanParents::GNEPlanParents(const CommonXMLStructure::PlanParameters& planParameters,
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
    route = ACs->retrieveDemandElement(SUMO_TAG_ROUTE, planParameters.toRoute, false);
    // stops
    edge = ACs->retrieveEdge(planParameters.toEdge, false);
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_BUS_STOP, planParameters.toBusStop, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_TRAIN_STOP, planParameters.toTrainStop, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, planParameters.toContainerStop, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_CHARGING_STATION, planParameters.toChargingStation, false);
    }
    if (stoppingPlace == nullptr) {
        stoppingPlace = ACs->retrieveAdditional(SUMO_TAG_PARKING_AREA, planParameters.toParkingArea, false);
    }
}


void
GNEPlanParents::addChildElements(GNEDemandElement* element) {
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
GNEPlanParents::clear() {
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
GNEPlanParents::getFromBusStop() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getFromTrainStop() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getFromContainerStop() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getFromChargingStation() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getFromParkingArea() const {
    if (fromStoppingPlace) {
        return (fromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getToBusStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getToTrainStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getToContainerStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getToChargingStation() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getToParkingArea() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getBusStop() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getTrainStop() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getContainerStop() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getChargingStation() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION);
    } else {
        return false;
    }
}


bool
GNEPlanParents::getParkingArea() const {
    if (stoppingPlace) {
        return (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA);
    } else {
        return false;
    }
}


std::vector<GNEJunction*>
GNEPlanParents::getJunctions() const {
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
GNEPlanParents::getEdges() const {
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
GNEPlanParents::getAdditionalElements() const {
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
GNEPlanParents::getDemandElements(GNEDemandElement* parent) const {
    std::vector<GNEDemandElement*> demandElements;
    // always add parent first
    demandElements.push_back(parent);
    if (route) {
        demandElements.push_back(route);
    }
    return demandElements;
}

/****************************************************************************/
