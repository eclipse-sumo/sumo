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
/// @file    MSDevice_StationFinder.cpp
/// @author  Michael Behrisch
/// @date    2023-05-24
///
// A device which triggers rerouting to nearby charging stations
/****************************************************************************/
#include <config.h>

#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
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
    insertDefaultAssignmentOptions("stationfinder", "Battery", oc);
    oc.doRegister("device.stationfinder.rescueTime", new Option_String("1800", "TIME"));
    oc.addDescription("device.stationfinder.rescueTime", "Battery", TL("Time to wait for a rescue vehicle on the road side when the battery is empty"));
    oc.doRegister("device.stationfinder.reserveFactor", new Option_Float(1.1));
    oc.addDescription("device.stationfinder.reserveFactor", "Battery", TL("Additional battery buffer for unexpected traffic situation when estimating the battery need"));
    oc.doRegister("device.stationfinder.emptyThreshold", new Option_Float(5));
    oc.addDescription("device.stationfinder.emptyThreshold", "Battery", TL("Battery percentage to go into rescue mode"));
    oc.doRegister("device.stationfinder.radius", new Option_String("180", "TIME"));
    oc.addDescription("device.stationfinder.radius", "Battery", TL("Search radius in travel time seconds"));
    oc.doRegister("device.stationfinder.repeat", new Option_String("60", "TIME"));
    oc.addDescription("device.stationfinder.repeat", "Battery", TL("When to trigger a new search if no station has been found"));
    oc.doRegister("device.stationfinder.maxChargePower", new Option_Float(1000.));
    oc.addDescription("device.stationfinder.maxChargePower", "Battery", TL("The maximum charging speed of the vehicle battery"));
    oc.doRegister("device.stationfinder.chargeType", new Option_String("charging"));
    oc.addDescription("device.stationfinder.chargeType", "Battery", TL("Type of energy transfer"));
    oc.doRegister("device.stationfinder.waitForCharge", new Option_String("600", "TIME"));
    oc.addDescription("device.stationfinder.waitForCharge", "Battery", TL("After this waiting time vehicle searches for a new station when the initial one is blocked"));
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
      myBattery(nullptr), myChargingStation(nullptr) {
    OptionsCont& oc = OptionsCont::getOptions();
    myReserveFactor = getFloatParam(holder, oc, "stationfinder.reserveFactor", 1.1);
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
        const double remainingTime = router.recomputeCosts(remainingRoute, &myHolder, now);
        if (now > myHolder.getDeparture()) {
            double expectedConsumption = myBattery->getTotalConsumption() / STEPS2TIME(now - myHolder.getDeparture()) * remainingTime;
            if (expectedConsumption > myBattery->getActualBatteryCapacity() * myReserveFactor) {
                const MSEdge* const start = myHolder.getEdge();
                double minTime = std::numeric_limits<double>::max();
                MSChargingStation* minStation = nullptr;
                ConstMSEdgeVector minRoute;
                // TODO do some form of bulk routing here
                for (const auto& stop : MSNet::getInstance()->getStoppingPlaces(SUMO_TAG_CHARGING_STATION)) {
                    ConstMSEdgeVector routeTo;
                    const MSEdge* const csEdge = &stop.second->getLane().getEdge();
                    if (router.compute(start, myHolder.getPositionOnLane(), csEdge, stop.second->getBeginLanePosition(), &myHolder, now, routeTo)) {
                        ConstMSEdgeVector routeFrom;
                        if (csEdge == route.back() || router.compute(start, &stop.second->getLane().getEdge(), &myHolder, now, routeFrom)) {
                            if (csEdge != route.back()) {
                                routeTo.insert(routeTo.end(), routeFrom.begin() + 1, routeFrom.end());
                            }
                            const double time = router.recomputeCosts(routeTo, &myHolder, now);
                            if (time < minTime) {
                                minTime = time;
                                minStation = static_cast<MSChargingStation*>(stop.second);
                                minRoute = routeTo;
                            }
                        }
                    }
                }
                if (minStation != nullptr) {
                    if (myHolder.hasStops()) {
                        WRITE_WARNINGF(TL("Rerouting using station finder removes all upcoming stops for vehicle '%'."), myHolder.getID());
                    }
                    myHolder.replaceRouteEdges(minRoute, minTime, 0., getID());
                    myChargingStation = minStation;
                    SUMOVehicleParameter::Stop stopPar;
                    stopPar.chargingStation = minStation->getID();
                    stopPar.lane = minStation->getLane().getID();
                    stopPar.endPos = minStation->getEndLanePosition();
                    stopPar.duration = TIME2STEPS(expectedConsumption / minStation->getChargingPower(false) * myReserveFactor);
                    std::string errorMsg;
                    if (!myHolder.addStop(stopPar, errorMsg)) {
                        WRITE_ERROR(errorMsg);
                    } else if (errorMsg != "") {
                        WRITE_WARNING(errorMsg);
                    }
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
