/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    MSDevice_StationFinder.cpp
/// @author  Michael Behrisch
/// @date    2023-05-24
///
// A device which triggers rerouting to nearby charging stations
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSStop.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/options/OptionsCont.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersEnergy.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSRoutingEngine.h"
#include "MSDevice_Battery.h"
#include "MSDevice_StationFinder.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_StationFinder::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("stationfinder", "StationFinder", oc);

    oc.doRegister("device.stationfinder.radius", new Option_String("180", "TIME"));
    oc.addDescription("device.stationfinder.radius", "StationFinder", TL("Search radius in travel time seconds"));
}


void
MSDevice_StationFinder::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "stationfinder", v, false)) {
        into.push_back(new MSDevice_StationFinder(v));
    }
}


// ---------------------------------------------------------------------------
// MSDevice_StationFinder-methods
// ---------------------------------------------------------------------------
MSDevice_StationFinder::MSDevice_StationFinder(SUMOVehicle& holder)
    : MSVehicleDevice(holder, "stationfinder_" + holder.getID()),
      myBattery(nullptr), myBufferFactor(1.1), myChargingStation(nullptr) {
}


MSDevice_StationFinder::~MSDevice_StationFinder() {
}


bool
MSDevice_StationFinder::notifyMove(SUMOTrafficObject& /*veh*/, double /*oldPos*/, double /*newPos*/, double /*newSpeed*/) {
    if (myChargingStation == nullptr) {
        const SUMOTime now = SIMSTEP;
        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSRoutingEngine::getRouterTT(myHolder.getRNGIndex(), myHolder.getVClass());
        const ConstMSEdgeVector& route = myHolder.getRoute().getEdges();
        const ConstMSEdgeVector remainingRoute(route.begin() + myHolder.getRoutePosition(), route.end());
        double remainingTime = router.recomputeCosts(remainingRoute, &myHolder, now);
        if (now > myHolder.getDeparture()) {
            double expectedConsumption = myBattery->getTotalConsumption() / STEPS2TIME(now - myHolder.getDeparture()) * remainingTime;
            if (expectedConsumption > myBattery->getActualBatteryCapacity() * myBufferFactor) {
                const MSEdge* const start = myHolder.getEdge();
                double minTime = std::numeric_limits<double>::max();
                MSStoppingPlace* minStation = nullptr;
                ConstMSEdgeVector minRoute;
                for (const auto& stop : MSNet::getInstance()->getStoppingPlaces(SUMO_TAG_CHARGING_STATION)) {
                    ConstMSEdgeVector route;
                    if (router.compute(start, &stop.second->getLane().getEdge(), &myHolder, now, route)) {
                        const double time = router.recomputeCosts(route, &myHolder, now);
                        if (time < minTime) {
                            minTime = time;
                            minStation = stop.second;
                            minRoute = route;
                        }
                    }
                }
                if (minStation != nullptr) {
                    myHolder.replaceRouteEdges(minRoute, minTime, 0., getID());
                    myChargingStation = minStation;
                }
            }
        }
    }
    return true;
}


bool
MSDevice_StationFinder::notifyIdle(SUMOTrafficObject& /*veh*/) {
    return true;
}


void
MSDevice_StationFinder::notifyMoveInternal(const SUMOTrafficObject& /*veh*/,
                                       const double /* frontOnLane */,
                                       const double /* timeOnLane */,
                                       const double /* meanSpeedFrontOnLane */,
                                       const double /* meanSpeedVehicleOnLane */,
                                       const double /* travelledDistanceFrontOnLane */,
                                       const double /* travelledDistanceVehicleOnLane */,
                                       const double /* meanLengthOnLane */) {

    // called by meso (see MSMeanData_Emissions::MSLaneMeanDataValues::notifyMoveInternal)
}



void
MSDevice_StationFinder::generateOutput(OutputDevice* tripinfoOut) const {
    if (tripinfoOut != nullptr) {
    }
}


/****************************************************************************/
