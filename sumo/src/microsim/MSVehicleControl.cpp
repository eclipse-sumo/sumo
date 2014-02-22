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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSNet.h"
#include <microsim/devices/MSDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
MTRand MSVehicleControl::myVehicleParamsRNG;


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
    myTotalDepartureDelay(0),
    myTotalTravelTime(0),
    myDefaultVTypeMayBeDeleted(true),
    myWaitingForPerson(0),
    myScale(-1) {
    SUMOVTypeParameter defType;
    myVTypeDict[DEFAULT_VTYPE_ID] = MSVehicleType::build(defType);
    OptionsCont& oc = OptionsCont::getOptions();
    if (oc.isSet("incremental-dua-step")) {
        myScale = oc.getInt("incremental-dua-step") / static_cast<SUMOReal>(oc.getInt("incremental-dua-base"));
    }
    if (oc.isSet("scale")) {
        myScale = oc.getFloat("scale");
    }
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


SUMOVehicle*
MSVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                               const MSRoute* route,
                               const MSVehicleType* type) {
    myLoadedVehNo++;
    MSVehicle* built = new MSVehicle(defs, route, type, type->computeChosenSpeedDeviation(myVehicleParamsRNG));
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
        OutputDevice::getDeviceByOption("tripinfo-output").closeTag();
    }
    deleteVehicle(veh);
}


void
MSVehicleControl::vehicleDeparted(const SUMOVehicle& v) {
    ++myRunningVehNo;
    myTotalDepartureDelay += STEPS2TIME(v.getDeparture() - STEPFLOOR(v.getParameter().depart));
    MSNet::getInstance()->informVehicleStateListener(&v, MSNet::VEHICLE_STATE_DEPARTED);
}


void
MSVehicleControl::setState(int runningVehNo, int endedVehNo, SUMOReal totalDepartureDelay, SUMOReal totalTravelTime) {
    myRunningVehNo = runningVehNo;
    myEndedVehNo = endedVehNo;
    myTotalDepartureDelay = totalDepartureDelay;
    myTotalTravelTime = totalTravelTime;
}


void
MSVehicleControl::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_DELAY).writeAttr(SUMO_ATTR_NUMBER, myRunningVehNo).writeAttr(SUMO_ATTR_END, myEndedVehNo);
    out.writeAttr(SUMO_ATTR_DEPART, myTotalDepartureDelay).writeAttr(SUMO_ATTR_TIME, myTotalTravelTime).closeTag();
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
    myVehicleDict.erase(veh->getID());
    delete veh;
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehBegin() const {
    return myVehicleDict.begin();
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehEnd() const {
    return myVehicleDict.end();
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
MSVehicleControl::getVType(const std::string& id) {
    VTypeDictType::iterator it = myVTypeDict.find(id);
    if (it == myVTypeDict.end()) {
        VTypeDistDictType::iterator it2 = myVTypeDistDict.find(id);
        if (it2 == myVTypeDistDict.end()) {
            return 0;
        }
        return it2->second->get(&myVehicleParamsRNG);
    }
    if (id == DEFAULT_VTYPE_ID) {
        myDefaultVTypeMayBeDeleted = false;
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
MSVehicleControl::getWaitingVehicle(const MSEdge* const edge, const std::set<std::string>& lines) {
    if (myWaiting.find(edge) != myWaiting.end()) {
        for (std::vector<SUMOVehicle*>::const_iterator it = myWaiting[edge].begin(); it != myWaiting[edge].end(); ++it) {
            const std::string& line = (*it)->getParameter().line == "" ? (*it)->getParameter().id : (*it)->getParameter().line;
            if (lines.count(line)) {
                return (*it);
            }
        }
    }
    return 0;
}


void
MSVehicleControl::abortWaiting() {
    for (VehicleDictType::iterator i = myVehicleDict.begin(); i != myVehicleDict.end(); ++i) {
        WRITE_WARNING("Vehicle " + i->first + " aborted waiting for a person that will never come.");
    }
}


bool
MSVehicleControl::isInQuota(SUMOReal frac) const {
    frac = frac < 0 ? myScale : frac;
    if (frac < 0) {
        return true;
    }
    const unsigned int resolution = 1000;
    const unsigned int intFrac = (unsigned int)floor(frac * resolution + 0.5);
    // the vehicle in question has already been loaded, hence  the '-1'
    // apply % twice to avoid integer overflow
    return (((myLoadedVehNo - 1) % resolution) * intFrac) % resolution < intFrac;
}

/****************************************************************************/

