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
/// @file    GNEPlanParents.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2024
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
    // edges
    fromEdge = ACs->retrieveEdge(planParameters.fromEdge, false);
    toEdge = ACs->retrieveEdge(planParameters.toEdge, false);
    for (const auto& edgeID : planParameters.consecutiveEdges) {
        auto parsedEdge = ACs->retrieveEdge(edgeID, false);
        // avoid null and consecutive dulicated edges
        if (parsedEdge && (consecutiveEdges.empty() || (consecutiveEdges.back() != parsedEdge))) {
            consecutiveEdges.push_back(parsedEdge);
        }
    }
    // junctions
    fromJunction = ACs->retrieveJunction(planParameters.fromJunction, false);
    toJunction = ACs->retrieveJunction(planParameters.toJunction, false);
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
    // routes
    fromRoute = ACs->retrieveDemandElement(SUMO_TAG_ROUTE, planParameters.fromRoute, false);
    toRoute = ACs->retrieveDemandElement(SUMO_TAG_ROUTE, planParameters.toRoute, false);
}


bool
GNEPlanParents::checkIntegrity(SumoXMLTag planTag, const GNEDemandElement* parent, const CommonXMLStructure::PlanParameters& planParameters) const {
    if (!planParameters.fromEdge.empty() && !fromEdge) {
        WRITE_WARNING(TLF("Invalid from edge '%' used in % of % '%'", planParameters.fromEdge, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toEdge.empty() && !toEdge) {
        WRITE_WARNING(TLF("Invalid to edge '%' used in % of % '%'", planParameters.toEdge, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromJunction.empty() && !fromJunction) {
        WRITE_WARNING(TLF("Invalid from junction '%' used in % of % '%'", planParameters.fromJunction, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toJunction.empty() && !toJunction) {
        WRITE_WARNING(TLF("Invalid to junction '%' used in % of % '%'", planParameters.toJunction, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromTAZ.empty() && !fromTAZ) {
        WRITE_WARNING(TLF("Invalid from TAZ '%' used in % of % '%'", planParameters.fromTAZ, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toTAZ.empty() && !toTAZ) {
        WRITE_WARNING(TLF("Invalid to TAZ '%' used in % of % '%'", planParameters.toTAZ, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromBusStop.empty() && !fromStoppingPlace) {
        WRITE_WARNING(TLF("Invalid from bus stop '%' used in % of % '%'", planParameters.fromBusStop, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromTrainStop.empty() && !fromStoppingPlace) {
        WRITE_WARNING(TLF("Invalid from train stop '%' used in % of % '%'", planParameters.fromTrainStop, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromContainerStop.empty() && !fromStoppingPlace) {
        WRITE_WARNING(TLF("Invalid from container stop '%' used in % of % '%'", planParameters.fromContainerStop, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromChargingStation.empty() && !fromStoppingPlace) {
        WRITE_WARNING(TLF("Invalid from charging station '%' used in % of % '%'", planParameters.fromChargingStation, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromParkingArea.empty() && !fromStoppingPlace) {
        WRITE_WARNING(TLF("Invalid from parking area '%' used in % of % '%'", planParameters.fromParkingArea, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toBusStop.empty() && !toStoppingPlace) {
        WRITE_WARNING(TLF("Invalid to bus stop '%' used in % of % '%'", planParameters.toBusStop, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toTrainStop.empty() && !toStoppingPlace) {
        WRITE_WARNING(TLF("Invalid to train stop '%' used in % of % '%'", planParameters.toTrainStop, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toContainerStop.empty() && !toStoppingPlace) {
        WRITE_WARNING(TLF("Invalid to container stop '%' used in % of % '%'", planParameters.toContainerStop, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toChargingStation.empty() && !toStoppingPlace) {
        WRITE_WARNING(TLF("Invalid to charging station '%' used in % of % '%'", planParameters.toChargingStation, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toParkingArea.empty() && !toStoppingPlace) {
        WRITE_WARNING(TLF("Invalid to parking area '%' used in % of % '%'", planParameters.toParkingArea, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.fromRoute.empty() && !fromRoute) {
        WRITE_WARNING(TLF("Invalid from route '%' used in % of % '%'", planParameters.fromRoute, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else if (!planParameters.toRoute.empty() && !toRoute) {
        WRITE_WARNING(TLF("Invalid to route '%' used in % of % '%'", planParameters.toRoute, toString(planTag), parent->getTagStr(), parent->getID()));
        return false;
    } else {
        return true;
    }
}

void
GNEPlanParents::addChildElements(GNEDemandElement* element) {
    if (fromEdge) {
        fromEdge->addChildElement(element);
    }
    if (toEdge) {
        toEdge->addChildElement(element);
    }
    for (const auto& consecutiveEdge : consecutiveEdges) {
        consecutiveEdge->addChildElement(element);
    }
    if (fromJunction) {
        fromJunction->addChildElement(element);
    }
    if (toJunction) {
        toJunction->addChildElement(element);
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
    if (fromRoute) {
        fromRoute->addChildElement(element);
    }
    if (toRoute) {
        toRoute->addChildElement(element);
    }
}


void
GNEPlanParents::clear() {
    fromEdge = nullptr;
    toEdge = nullptr;
    consecutiveEdges.clear();
    fromJunction = nullptr;
    toJunction = nullptr;
    fromTAZ = nullptr;
    toTAZ = nullptr;
    fromStoppingPlace = nullptr;
    toStoppingPlace = nullptr;
    fromRoute = nullptr;
    toRoute = nullptr;
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
GNEPlanParents::getToBusStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
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
GNEPlanParents::getToTrainStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP);
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
GNEPlanParents::getToContainerStop() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP);
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
GNEPlanParents::getToChargingStation() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION);
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
GNEPlanParents::getToParkingArea() const {
    if (toStoppingPlace) {
        return (toStoppingPlace->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA);
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
    return additionals;
}


std::vector<GNEDemandElement*>
GNEPlanParents::getDemandElements(GNEDemandElement* parent) const {
    std::vector<GNEDemandElement*> demandElements;
    // always add parent first
    demandElements.push_back(parent);
    if (fromRoute) {
        demandElements.push_back(fromRoute);
    }
    if (toRoute) {
        demandElements.push_back(toRoute);
    }
    return demandElements;
}

/****************************************************************************/
