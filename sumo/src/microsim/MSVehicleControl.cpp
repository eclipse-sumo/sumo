/****************************************************************************/
/// @file    MSVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
// member method definitions
// ===========================================================================
MSVehicleControl::MSVehicleControl() throw()
        : myLoadedVehNo(0), myRunningVehNo(0), myEndedVehNo(0),
        myAbsVehWaitingTime(0), myAbsVehTravelTime(0),
        myDefaultVTypeMayBeDeleted(true) {
    SUMOVTypeParameter defType;
    myVTypeDict[DEFAULT_VTYPE_ID] = MSVehicleType::build(defType);
}


MSVehicleControl::~MSVehicleControl() throw() {
    // delete vehicles
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); ++i) {
        delete(*i).second;
    }
    myVehicleDict.clear();
    // delete vehicle type distributions
    for (VTypeDistDictType::iterator i=myVTypeDistDict.begin(); i!=myVTypeDistDict.end(); ++i) {
        delete(*i).second;
    }
    myVTypeDistDict.clear();
    // delete vehicle types
    for (VTypeDictType::iterator i=myVTypeDict.begin(); i!=myVTypeDict.end(); ++i) {
        delete(*i).second;
    }
    myVTypeDict.clear();
}


SUMOVehicle *
MSVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                               const MSRoute* route,
                               const MSVehicleType* type) throw(ProcessError) {
    myLoadedVehNo++;
    MSVehicle *built = new MSVehicle(defs, route, type, myLoadedVehNo-1);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


void
MSVehicleControl::scheduleVehicleRemoval(SUMOVehicle *veh) throw() {
    assert(myRunningVehNo>0);
    for (std::vector<MSDevice*>::const_iterator i=veh->getDevices().begin(); i!=veh->getDevices().end(); ++i) {
        (*i)->generateOutput();
    }
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice::getDeviceByOption("tripinfo-output").closeTag(veh->getDevices().size()==1);
    }
    myAbsVehTravelTime += MSNet::getInstance()->getCurrentTimeStep() - veh->getDeparture();
    myRunningVehNo--;
    MSNet::getInstance()->informVehicleStateListener(veh, MSNet::VEHICLE_STATE_ARRIVED);
    deleteVehicle(veh);
}


SUMOReal
MSVehicleControl::getMeanWaitingTime() const throw() {
    if (getEmittedVehicleNo()==0) {
        return -1;
    }
    return (SUMOReal) myAbsVehWaitingTime / (SUMOReal) getEmittedVehicleNo();
}


SUMOReal
MSVehicleControl::getMeanTravelTime() const throw() {
    if (myEndedVehNo==0) {
        return -1;
    }
    return (SUMOReal) myAbsVehTravelTime / (SUMOReal) myEndedVehNo;
}


void
MSVehicleControl::vehicleEmitted(const SUMOVehicle &v) throw() {
    ++myRunningVehNo;
    myAbsVehWaitingTime += MAX2(v.getDeparture() - v.getParameter().depart, static_cast<SUMOTime>(0));
    MSNet::getInstance()->informVehicleStateListener(&v, MSNet::VEHICLE_STATE_DEPARTED);
}


void
MSVehicleControl::saveState(std::ostream &os) throw() {
}


void
MSVehicleControl::loadState(BinaryInputDevice &bis, const SUMOTime offset) throw() {
}


bool
MSVehicleControl::addVehicle(const std::string &id, SUMOVehicle *v) throw() {
    VehicleDictType::iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        // id not in myVehicleDict.
        myVehicleDict[id] = v;
        return true;
    }
    return false;
}


SUMOVehicle *
MSVehicleControl::getVehicle(const std::string &id) const throw() {
    VehicleDictType::const_iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        return 0;
    }
    return it->second;
}


void
MSVehicleControl::deleteVehicle(SUMOVehicle *veh) throw() {
    myEndedVehNo++;
    myVehicleDict.erase(veh->getID());
    delete veh;
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehBegin() const throw() {
    return myVehicleDict.begin();
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehEnd() const throw() {
    return myVehicleDict.end();
}


bool
MSVehicleControl::checkVType(const std::string &id) throw() {
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
MSVehicleControl::addVType(MSVehicleType* vehType) throw() {
    if (checkVType(vehType->getID())) {
        myVTypeDict[vehType->getID()] = vehType;
        return true;
    }
    return false;
}


bool
MSVehicleControl::addVTypeDistribution(const std::string &id, RandomDistributor<MSVehicleType*> *vehTypeDistribution) throw() {
    if (checkVType(id)) {
        myVTypeDistDict[id] = vehTypeDistribution;
        return true;
    }
    return false;
}


bool
MSVehicleControl::hasVTypeDistribution(const std::string &id) const throw() {
    return myVTypeDistDict.find(id) != myVTypeDistDict.end();
}


MSVehicleType*
MSVehicleControl::getVType(const std::string &id) throw() {
    VTypeDictType::iterator it = myVTypeDict.find(id);
    if (it == myVTypeDict.end()) {
        VTypeDistDictType::iterator it2 = myVTypeDistDict.find(id);
        if (it2 == myVTypeDistDict.end()) {
            return 0;
        }
        return it2->second->get();
    }
    if (id == DEFAULT_VTYPE_ID) {
        myDefaultVTypeMayBeDeleted = false;
    }
    return it->second;
}


void
MSVehicleControl::insertVTypeIDs(std::vector<std::string> &into) const throw() {
    into.reserve(into.size()+myVTypeDict.size()+myVTypeDistDict.size());
    for (VTypeDictType::const_iterator i=myVTypeDict.begin(); i!=myVTypeDict.end(); ++i) {
        into.push_back((*i).first);
    }
    for (VTypeDistDictType::const_iterator i=myVTypeDistDict.begin(); i!=myVTypeDistDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


void
MSVehicleControl::addWaiting(const MSEdge* const edge, SUMOVehicle *vehicle) throw() {
    if (myWaiting.find(edge) == myWaiting.end()) {
        myWaiting[edge] = std::vector<SUMOVehicle*>();
    }
    myWaiting[edge].push_back(vehicle);
}


void
MSVehicleControl::removeWaiting(const MSEdge* const edge, SUMOVehicle *vehicle) throw() {
    if (myWaiting.find(edge) != myWaiting.end()) {
        std::vector<SUMOVehicle*>::iterator it = std::find(myWaiting[edge].begin(), myWaiting[edge].end(), vehicle);
        if (it != myWaiting[edge].end()) {
            myWaiting[edge].erase(it);
        }
    }
}


SUMOVehicle *
MSVehicleControl::getWaitingVehicle(const MSEdge* const edge, const std::set<std::string> &lines) throw() {
    if (myWaiting.find(edge) != myWaiting.end()) {
        for (std::vector<SUMOVehicle*>::const_iterator it = myWaiting[edge].begin(); it != myWaiting[edge].end(); ++it) {
            const std::string &line = (*it)->getParameter().line == "" ? (*it)->getParameter().id : (*it)->getParameter().line;
            if (lines.count(line)) {
                return (*it);
            }
        }
    }
    return 0;
}


/****************************************************************************/

