/****************************************************************************/
/// @file    MSVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSVehicleControl.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSNet.h"
#include "MSRouteHandler.h"
#include <microsim/devices/MSDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RGBColor.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSVehicleControl::MSVehicleControl() :
    myLoadedVehNo(0),
    myRunningVehNo(0),
    myEndedVehNo(0),
    myDiscarded(0),
    myCollisions(0),
    myTeleportsJam(0),
    myTeleportsYield(0),
    myTeleportsWrongLane(0),
    myEmergencyStops(0),
    myTotalDepartureDelay(0),
    myTotalTravelTime(0),
    myDefaultVTypeMayBeDeleted(true),
    myDefaultPedTypeMayBeDeleted(true),
    myWaitingForPerson(0),
    myWaitingForContainer(0),
    myMaxSpeedFactor(1),
    myMinDeceleration(SUMOVTypeParameter::getDefaultDecel(SVC_IGNORING)) {
    SUMOVTypeParameter defType(DEFAULT_VTYPE_ID, SVC_PASSENGER);
    myVTypeDict[DEFAULT_VTYPE_ID] = MSVehicleType::build(defType);
    SUMOVTypeParameter defPedType(DEFAULT_PEDTYPE_ID, SVC_PEDESTRIAN);
    defPedType.setParameter |= VTYPEPARS_VEHICLECLASS_SET;
    myVTypeDict[DEFAULT_PEDTYPE_ID] = MSVehicleType::build(defPedType);
    OptionsCont& oc = OptionsCont::getOptions();
    myScale = oc.getFloat("scale");
    myMaxRandomDepartOffset = string2time(oc.getString("random-depart-offset"));
}


MSVehicleControl::~MSVehicleControl() {
    // delete vehicles
    for (VehicleDictType::iterator i = myVehicleDict.begin(); i != myVehicleDict.end(); ++i) {
        delete(*i).second;
    }
    myVehicleDict.clear();
    // delete vehicle type distributions
    for (VTypeDistDictType::iterator i = myVTypeDistDict.begin(); i != myVTypeDistDict.end(); ++i) {
        delete(*i).second;
    }
    myVTypeDistDict.clear();
    // delete vehicle types
    for (VTypeDictType::iterator i = myVTypeDict.begin(); i != myVTypeDict.end(); ++i) {
        delete(*i).second;
    }
    myVTypeDict.clear();
}

SUMOTime
MSVehicleControl::computeRandomDepartOffset() const {
    if (myMaxRandomDepartOffset > 0) {
        // round to the closest usable simulation step
        return DELTA_T * int((MSRouteHandler::getParsingRNG()->rand((int)myMaxRandomDepartOffset) + 0.5 * DELTA_T) / DELTA_T);
    } else {
        return 0;
    }
}

SUMOVehicle*
MSVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                               const MSRoute* route,
                               const MSVehicleType* type,
                               const bool ignoreStopErrors, const bool fromRouteFile) {
    myLoadedVehNo++;
    if (fromRouteFile) {
        defs->depart += computeRandomDepartOffset();
    }
    MSVehicle* built = new MSVehicle(defs, route, type, type->computeChosenSpeedDeviation(fromRouteFile ? MSRouteHandler::getParsingRNG() : 0));
    built->addStops(ignoreStopErrors);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


void
MSVehicleControl::scheduleVehicleRemoval(SUMOVehicle* veh) {
    assert(myRunningVehNo > 0);
    myTotalTravelTime += STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep() - veh->getDeparture());
    myRunningVehNo--;
    MSNet::getInstance()->informVehicleStateListener(veh, MSNet::VEHICLE_STATE_ARRIVED);
    for (std::vector<MSDevice*>::const_iterator i = veh->getDevices().begin(); i != veh->getDevices().end(); ++i) {
        (*i)->generateOutput();
    }
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        // close tag after tripinfo (possibly including emissions from another device) have been written
        OutputDevice::getDeviceByOption("tripinfo-output").closeTag();
    }
    deleteVehicle(veh);
}


void
MSVehicleControl::vehicleDeparted(const SUMOVehicle& v) {
    ++myRunningVehNo;
    myTotalDepartureDelay += STEPS2TIME(v.getDeparture() - STEPFLOOR(v.getParameter().depart));
    MSNet::getInstance()->informVehicleStateListener(&v, MSNet::VEHICLE_STATE_DEPARTED);
    myMaxSpeedFactor = MAX2(myMaxSpeedFactor, v.getChosenSpeedFactor());
    myMinDeceleration = MIN2(myMinDeceleration, v.getVehicleType().getCarFollowModel().getMaxDecel());
}


void
MSVehicleControl::setState(int runningVehNo, int loadedVehNo, int endedVehNo, SUMOReal totalDepartureDelay, SUMOReal totalTravelTime) {
    myRunningVehNo = runningVehNo;
    myLoadedVehNo = loadedVehNo;
    myEndedVehNo = endedVehNo;
    myTotalDepartureDelay = totalDepartureDelay;
    myTotalTravelTime = totalTravelTime;
}


void
MSVehicleControl::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_DELAY);
    out.writeAttr(SUMO_ATTR_NUMBER, myRunningVehNo);
    out.writeAttr(SUMO_ATTR_BEGIN, myLoadedVehNo);
    out.writeAttr(SUMO_ATTR_END, myEndedVehNo);
    out.writeAttr(SUMO_ATTR_DEPART, myTotalDepartureDelay);
    out.writeAttr(SUMO_ATTR_TIME, myTotalTravelTime).closeTag();
    // save vehicle types
    for (VTypeDictType::iterator it = myVTypeDict.begin(); it != myVTypeDict.end(); ++it) {
        it->second->getParameter().write(out);
    }
    for (VTypeDistDictType::iterator it = myVTypeDistDict.begin(); it != myVTypeDistDict.end(); ++it) {
        out.openTag(SUMO_TAG_VTYPE_DISTRIBUTION).writeAttr(SUMO_ATTR_ID, it->first);
        out.writeAttr(SUMO_ATTR_VTYPES, (*it).second->getVals());
        out.writeAttr(SUMO_ATTR_PROBS, (*it).second->getProbs());
        out.closeTag();
    }
    for (VehicleDictType::iterator it = myVehicleDict.begin(); it != myVehicleDict.end(); ++it) {
        (*it).second->saveState(out);
    }
}


bool
MSVehicleControl::addVehicle(const std::string& id, SUMOVehicle* v) {
    VehicleDictType::iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        // id not in myVehicleDict.
        myVehicleDict[id] = v;
        return true;
    }
    return false;
}


SUMOVehicle*
MSVehicleControl::getVehicle(const std::string& id) const {
    VehicleDictType::const_iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        return 0;
    }
    return it->second;
}


void
MSVehicleControl::deleteVehicle(SUMOVehicle* veh, bool discard) {
    myEndedVehNo++;
    if (discard) {
        myDiscarded++;
    }
    if (veh != 0) {
        myVehicleDict.erase(veh->getID());
    }
    delete veh;
}


bool
MSVehicleControl::checkVType(const std::string& id) {
    if (id == DEFAULT_VTYPE_ID) {
        if (myDefaultVTypeMayBeDeleted) {
            delete myVTypeDict[id];
            myVTypeDict.erase(myVTypeDict.find(id));
            myDefaultVTypeMayBeDeleted = false;
        } else {
            return false;
        }
    } else if (id == DEFAULT_PEDTYPE_ID) {
        if (myDefaultPedTypeMayBeDeleted) {
            delete myVTypeDict[id];
            myVTypeDict.erase(myVTypeDict.find(id));
            myDefaultPedTypeMayBeDeleted = false;
        } else {
            return false;
        }
    } else {
        if (myVTypeDict.find(id) != myVTypeDict.end() || myVTypeDistDict.find(id) != myVTypeDistDict.end()) {
            return false;
        }
    }
    return true;
}

bool
MSVehicleControl::addVType(MSVehicleType* vehType) {
    if (checkVType(vehType->getID())) {
        myVTypeDict[vehType->getID()] = vehType;
        return true;
    }
    return false;
}


bool
MSVehicleControl::addVTypeDistribution(const std::string& id, RandomDistributor<MSVehicleType*>* vehTypeDistribution) {
    if (checkVType(id)) {
        myVTypeDistDict[id] = vehTypeDistribution;
        return true;
    }
    return false;
}


bool
MSVehicleControl::hasVTypeDistribution(const std::string& id) const {
    return myVTypeDistDict.find(id) != myVTypeDistDict.end();
}


MSVehicleType*
MSVehicleControl::getVType(const std::string& id, MTRand* rng) {
    VTypeDictType::iterator it = myVTypeDict.find(id);
    if (it == myVTypeDict.end()) {
        VTypeDistDictType::iterator it2 = myVTypeDistDict.find(id);
        if (it2 == myVTypeDistDict.end()) {
            return 0;
        }
        return it2->second->get(rng);
    }
    if (id == DEFAULT_VTYPE_ID) {
        myDefaultVTypeMayBeDeleted = false;
    } else if (id == DEFAULT_PEDTYPE_ID) {
        myDefaultPedTypeMayBeDeleted = false;
    }
    return it->second;
}


void
MSVehicleControl::insertVTypeIDs(std::vector<std::string>& into) const {
    into.reserve(into.size() + myVTypeDict.size() + myVTypeDistDict.size());
    for (VTypeDictType::const_iterator i = myVTypeDict.begin(); i != myVTypeDict.end(); ++i) {
        into.push_back((*i).first);
    }
    for (VTypeDistDictType::const_iterator i = myVTypeDistDict.begin(); i != myVTypeDistDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


void
MSVehicleControl::addWaiting(const MSEdge* const edge, SUMOVehicle* vehicle) {
    if (myWaiting.find(edge) == myWaiting.end()) {
        myWaiting[edge] = std::vector<SUMOVehicle*>();
    }
    myWaiting[edge].push_back(vehicle);
}


void
MSVehicleControl::removeWaiting(const MSEdge* const edge, SUMOVehicle* vehicle) {
    if (myWaiting.find(edge) != myWaiting.end()) {
        std::vector<SUMOVehicle*>::iterator it = std::find(myWaiting[edge].begin(), myWaiting[edge].end(), vehicle);
        if (it != myWaiting[edge].end()) {
            myWaiting[edge].erase(it);
        }
    }
}


SUMOVehicle*
MSVehicleControl::getWaitingVehicle(const MSEdge* const edge, const std::set<std::string>& lines, const SUMOReal position, const std::string ridingID) {
    if (myWaiting.find(edge) != myWaiting.end()) {
        // for every vehicle waiting vehicle at this edge
        std::vector<SUMOVehicle*> waitingTooFarAway;
        for (std::vector<SUMOVehicle*>::const_iterator it = myWaiting[edge].begin(); it != myWaiting[edge].end(); ++it) {
            const std::string& line = (*it)->getParameter().line == "" ? (*it)->getParameter().id : (*it)->getParameter().line;
            SUMOReal vehiclePosition = (*it)->getPositionOnLane();
            // if the line of the vehicle is contained in the set of given lines and the vehicle is stopped and is positioned
            // in the interval [position - t, position + t] for a tolerance t=10
            if (lines.count(line)) {
                if ((position - 10 <= vehiclePosition) && (vehiclePosition <= position + 10)) {
                    return (*it);
                } else if ((*it)->isStoppedTriggered() ||
                           (*it)->getParameter().departProcedure == DEPART_TRIGGERED) {
                    // maybe we are within the range of the stop
                    MSVehicle* veh = static_cast<MSVehicle*>(*it);
                    if (veh->isStoppedInRange(position)) {
                        return (*it);
                    } else {
                        waitingTooFarAway.push_back(*it);
                    }
                }
            }
        }
        for (std::vector<SUMOVehicle*>::iterator it = waitingTooFarAway.begin(); it != waitingTooFarAway.end(); ++it) {
            WRITE_WARNING(ridingID + " at edge '" + edge->getID() + "' position " + toString(position) + " cannot use waiting vehicle '" + (*it)->getID() + "' at position " + toString((*it)->getPositionOnLane()) + " because it is too far away.");
        }
    }
    return 0;
}


void
MSVehicleControl::abortWaiting() {
    for (VehicleDictType::iterator i = myVehicleDict.begin(); i != myVehicleDict.end(); ++i) {
        WRITE_WARNING("Vehicle " + i->first + " aborted waiting for a person or a container that will never come.");
    }
}


int
MSVehicleControl::getQuota(SUMOReal frac) const {
    frac = frac < 0 ? myScale : frac;
    if (frac < 0 || frac == 1.) {
        return 1;
    }
    // the vehicle in question has already been loaded, hence  the '-1'
    const int loaded = frac > 1. ? (int)(myLoadedVehNo / frac) : myLoadedVehNo - 1;
    const int base = (int)frac;
    const int resolution = 1000;
    const int intFrac = (int)floor((frac - base) * resolution + 0.5);
    // apply % twice to avoid integer overflow
    if (((loaded % resolution) * intFrac) % resolution < intFrac) {
        return base + 1;
    }
    return base;
}

int
MSVehicleControl::getTeleportCount() const {
    return (MSLane::teleportOnCollision() ? myCollisions : 0) + myTeleportsJam + myTeleportsYield + myTeleportsWrongLane;
}

/****************************************************************************/

