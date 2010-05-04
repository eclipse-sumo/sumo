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

#include "MSCORN.h"
#include "MSVehicleControl.h"
#include "MSVehicle.h"
#include "MSGlobals.h"
#include "MSLane.h"
#include <microsim/devices/MSDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

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


MSVehicle *
MSVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                               const MSRoute* route,
                               const MSVehicleType* type) throw(ProcessError) {
    myLoadedVehNo++;
    MSVehicle *built = new MSVehicle(defs, route, type, myLoadedVehNo-1);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


void
MSVehicleControl::scheduleVehicleRemoval(MSVehicle *v) throw() {
    assert(myRunningVehNo>0);
    // check whether to generate the information about the vehicle's trip
    if (MSCORN::wished(MSCORN::CORN_OUT_TRIPDURATIONS)) {
        OutputDevice& od = OutputDevice::getDeviceByOption("tripinfo-output");
        // obtain and generate vehicle's trip information
        MSVehicle::DepartArrivalInformation *departInfo = v->hasCORNPointerValue(MSCORN::CORN_P_VEH_DEPART_INFO)
                ? (MSVehicle::DepartArrivalInformation*) v->getCORNPointerValue(MSCORN::CORN_P_VEH_DEPART_INFO)
                : 0;
        MSVehicle::DepartArrivalInformation *arrivalInfo = v->hasCORNPointerValue(MSCORN::CORN_P_VEH_ARRIVAL_INFO)
                ? (MSVehicle::DepartArrivalInformation*) v->getCORNPointerValue(MSCORN::CORN_P_VEH_ARRIVAL_INFO)
                : 0;
        SUMOReal routeLength = v->getRoute().getLength();
        // write
        od.openTag("tripinfo") << " id=\"" << v->getID() << "\" ";
        SUMOTime departTime = -1;
        if (departInfo!=0) {
            routeLength -= departInfo->pos;
            std::string laneID = departInfo->lane!=0 ? departInfo->lane->getID() : "";
            od << "depart=\"" << time2string(departInfo->time) << "\" "
            << "departLane=\"" << laneID << "\" "
            << "departPos=\"" << departInfo->pos << "\" "
            << "departSpeed=\"" << departInfo->speed << "\" "
            << "departDelay=\"" << time2string(departInfo->time - v->getDesiredDepart()) << "\" ";
            departTime = departInfo->time;
        } else {
            if (v->hasCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME)) {
                departTime = v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME);
                od << "depart=\"" << time2string(departTime) << "\" ";
            } else {
                od << "depart=\"\" ";
            }
            od << "departLane=\"\" "
            << "departPos=\"\" "
            << "departSpeed=\"\" "
            << "departDelay=\"\" ";
        }
        SUMOTime arrivalTime = -1;
        if (arrivalInfo!=0) {
            std::string laneID = "";
            if (arrivalInfo->lane!=0) {
                routeLength -= arrivalInfo->lane->getLength() - arrivalInfo->pos;
                laneID = arrivalInfo->lane->getID();
            }
            od << "arrival=\"" << time2string(arrivalInfo->time) << "\" "
            << "arrivalLane=\"" << laneID << "\" "
            << "arrivalPos=\"" << arrivalInfo->pos << "\" "
            << "arrivalSpeed=\"" << arrivalInfo->speed << "\" ";
            arrivalTime = arrivalInfo->time;
        } else {
            arrivalTime = MSNet::getInstance()->getCurrentTimeStep();
            od << "arrival=\"" << time2string(arrivalTime) << "\" "
            << "arrivalLane=\"\" "
            << "arrivalPos=\"\" "
            << "arrivalSpeed=\"\" ";
        }
        if (departTime!=-1&&arrivalTime!=-1) {
            od << "duration=\"" << time2string(arrivalTime - departTime) << "\" ";
        } else {
            od << "duration=\"\" ";
        }
        od << "routeLength=\"" << routeLength << "\" "
        << "waitSteps=\"" << v->getCORNIntValue(MSCORN::CORN_VEH_WAITINGTIME) << "\" "
        << "rerouteNo=\"";
        if (v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            od << v->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
        } else {
            od << '0';
        }
        // compute device id list
        const std::vector<MSDevice*> &devices = v->getDevices();
        std::string deviceIDs;
        if (devices.size()!=0) {
            std::ostringstream str;
            bool addSem = false;
            for (std::vector<MSDevice*>::const_iterator i=devices.begin(); i!=devices.end(); ++i) {
                if (addSem) {
                    str << ' ';
                }
                addSem = true;
                str << (*i)->getID();
            }
            deviceIDs = str.str();
        }
        //
        od << "\" devices=\"" << deviceIDs
        << "\" vtype=\"" << v->getVehicleType().getID()
        << "\" vaporized=\"";
        if (v->hasCORNIntValue(MSCORN::CORN_VEH_VAPORIZED)) {
            od << v->getCORNIntValue(MSCORN::CORN_VEH_VAPORIZED);
        }
        od << "\"";
        // write device information
        if (devices.size()!=0) {
            od << ">\n";
            for (std::vector<MSDevice*>::const_iterator i=devices.begin(); i!=devices.end(); ++i) {
                (*i)->tripInfoOutput(od);
            }
        }
        od.closeTag(devices.size()==0);
    }

    // check whether to generate the information about the vehicle's routes
    if (MSCORN::wished(MSCORN::CORN_OUT_VEHROUTES)) {
        // generate vehicle's trip routes
        MSNet *net = MSNet::getInstance();
        OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
        SUMOTime realDepart = (SUMOTime) v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME);
        SUMOTime time = net->getCurrentTimeStep();
        od.openTag("vehicle") << " id=\"" << v->getID() << "\" depart=\""
        << time2string(v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME))
        << "\" arrival=\"" << time2string(MSNet::getInstance()->getCurrentTimeStep());
        if (MSCORN::wished(MSCORN::CORN_OUT_TAZ)) {
            od << "\" fromtaz=\"" << v->getParameter().fromTaz << "\" totaz=\"" << v->getParameter().toTaz;
        }
        od << "\">\n";
        if (MSCORN::wished(MSCORN::CORN_OUT_OLDROUTES) && v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            od.openTag("routeDistribution") << ">\n";
            int noReroutes = v->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
            for (int i=0; i<noReroutes; ++i) {
                v->writeXMLRoute(od, i);
            }
        }
        v->writeXMLRoute(od);
        if (MSCORN::wished(MSCORN::CORN_OUT_OLDROUTES) && v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            od.closeTag();
        }
        od.closeTag();
        od << "\n";
    }
    // check whether to save information about the vehicle's trip
    if (MSCORN::wished(MSCORN::CORN_MEAN_VEH_TRAVELTIME)) {
        myAbsVehTravelTime +=
            (MSNet::getInstance()->getCurrentTimeStep()
             - v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME));
    }
    myRunningVehNo--;
    MSNet::getInstance()->informVehicleStateListener(v, MSNet::VEHICLE_STATE_ARRIVED);
    deleteVehicle(v);
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
MSVehicleControl::vehicleEmitted(const MSVehicle &v) throw() {
    ++myRunningVehNo;
    if (MSCORN::wished(MSCORN::CORN_MEAN_VEH_WAITINGTIME)) {
        myAbsVehWaitingTime += MAX2(v.getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME) - v.getDesiredDepart(), (int) 0);
    }
    MSNet::getInstance()->informVehicleStateListener(&v, MSNet::VEHICLE_STATE_DEPARTED);
}


#ifdef HAVE_MESOSIM
void
MSVehicleControl::saveState(std::ostream &os) throw() {
    FileHelpers::writeUInt(os, myRunningVehNo);
    FileHelpers::writeUInt(os, myEndedVehNo);

    FileHelpers::writeTime(os, myAbsVehWaitingTime);
    FileHelpers::writeTime(os, myAbsVehTravelTime);
    // save vehicle types
    FileHelpers::writeUInt(os, (unsigned) myVTypeDict.size());
    for (VTypeDictType::iterator it=myVTypeDict.begin(); it!=myVTypeDict.end(); ++it) {
        (*it).second->saveState(os);
    }
    FileHelpers::writeUInt(os, (unsigned) myVTypeDistDict.size());
    for (VTypeDistDictType::iterator it=myVTypeDistDict.begin(); it!=myVTypeDistDict.end(); ++it) {
        FileHelpers::writeString(os, (*it).first);
        const unsigned int size = (unsigned int)(*it).second->getVals().size();
        FileHelpers::writeUInt(os, size);
        for (unsigned int i = 0; i < size; ++i) {
            FileHelpers::writeString(os, (*it).second->getVals()[i]->getID());
            FileHelpers::writeFloat(os, (*it).second->getProbs()[i]);
        }
    }
    MSRoute::dict_saveState(os);
    // save vehicles
    FileHelpers::writeUInt(os, (unsigned) myVehicleDict.size());
    for (VehicleDictType::iterator it = myVehicleDict.begin(); it!=myVehicleDict.end(); ++it) {
        (*it).second->saveState(os);
    }
}

void
MSVehicleControl::loadState(BinaryInputDevice &bis) throw() {
    const SUMOTime offset = string2time(OptionsCont::getOptions().getString("load-state.offset"));
    bis >> myRunningVehNo;
    bis >> myEndedVehNo;
    myLoadedVehNo = myEndedVehNo;

    bis >> myAbsVehWaitingTime;
    bis >> myAbsVehTravelTime;

    // load vehicle types
    unsigned int size;
    bis >> size;
    while (size-->0) {
        SUMOReal r, g, b;
        SUMOVTypeParameter defType;
        int vehicleClass, emissionClass, shape;

        bis >> defType.id;
        bis >> defType.length;
        bis >> defType.maxSpeed;
        bis >> vehicleClass;
        defType.vehicleClass = (SUMOVehicleClass) vehicleClass;
        bis >> emissionClass;
        defType.emissionClass = (SUMOEmissionClass) emissionClass;
        bis >> shape;
        defType.shape = (SUMOVehicleShape) shape;
        bis >> defType.width;
        bis >> defType.offset;
        bis >> defType.defaultProbability;
        bis >> defType.speedFactor;
        bis >> defType.speedDev;
        bis >> r;
        bis >> g;
        bis >> b;
        defType.color = RGBColor(r,g,b);
        bis >> defType.cfModel;
        bis >> defType.lcModel;
        MSVehicleType *t = MSVehicleType::build(defType);
        addVType(t);
    }
    unsigned int numVTypeDists;
    bis >> numVTypeDists;
    for (; numVTypeDists>0; numVTypeDists--) {
        std::string id;
        bis >> id;
        unsigned int no;
        bis >> no;
        if (getVType(id)==0) {
            RandomDistributor<MSVehicleType*> *dist = new RandomDistributor<MSVehicleType*>();
            for (; no>0; no--) {
                std::string vtypeID;
                bis >> vtypeID;
                MSVehicleType *t = getVType(vtypeID);
                assert(t!=0);
                SUMOReal prob;
                bis >> prob;
                dist->add(prob, t);
            }
            addVTypeDistribution(id, dist);
        } else {
            for (; no>0; no--) {
                std::string vtypeID;
                bis >> vtypeID;
                SUMOReal prob;
                bis >> prob;
            }
        }
    }
    MSRoute::dict_loadState(bis);
    // load vehicles
    bis >> size;
    while (size-->0) {
        SUMOVehicleParameter* p = new SUMOVehicleParameter();
        bis >> p->id;
        SUMOReal lastLaneChangeOffset;
        bis >> lastLaneChangeOffset;
        SUMOReal waitingTime;
        bis >> waitingTime;
        bis >> p->repetitionNumber;
        bis >> p->repetitionOffset;
        bis >> p->routeid;
        const MSRoute* route;
        SUMOTime desiredDepart;
        bis >> desiredDepart;
        p->depart = desiredDepart - offset;
        bis >> p->vtypeid;
        const MSVehicleType* type;
        unsigned int routeOffset;
        bis >> routeOffset;
        int wasEmitted;
        bis >> wasEmitted;
#ifdef HAVE_MESOSIM
        unsigned int segIndex;
        bis >> segIndex;
        unsigned int queIndex;
        bis >> queIndex;
        SUMOTime tEvent;
        bis >> tEvent;
        SUMOTime tLastEntry;
        bis >> tLastEntry;
#endif
        route = MSRoute::dictionary(p->routeid);
        assert(route!=0);
        type = getVType(p->vtypeid);
        assert(type!=0);
        assert(getVehicle(p->id)==0);

        MSVehicle *v = buildVehicle(p, route, type);
        if (wasEmitted != -1) {
            v->myIntCORNMap[MSCORN::CORN_VEH_DEPART_TIME] = wasEmitted;
        }
        while (routeOffset>0) {
            v->myCurrEdge++;
            routeOffset--;
        }
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            v->mySegment = MSGlobals::gMesoNet->getSegmentForEdge(**(v->myCurrEdge));
            while (v->mySegment->getIndex()!=segIndex) {
                v->mySegment = MSGlobals::gMesoNet->next_segment(v->mySegment, v);
            }
            v->myQueIndex = queIndex;
            v->myEventTime = tEvent - offset;
            v->myLastEntryTime = tLastEntry - offset;
        }
#endif
        if (!addVehicle(p->id, v)) {
            MsgHandler::getErrorInstance()->inform("Error: Could not build vehicle " + p->id + "!");
        }
    }
}
#endif


bool
MSVehicleControl::addVehicle(const std::string &id, MSVehicle *v) throw() {
    VehicleDictType::iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        // id not in myVehicleDict.
        myVehicleDict[id] = v;
        return true;
    }
    return false;
}


MSVehicle *
MSVehicleControl::getVehicle(const std::string &id) const throw() {
    VehicleDictType::const_iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        return 0;
    }
    return it->second;
}


void
MSVehicleControl::deleteVehicle(MSVehicle *veh) throw() {
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
MSVehicleControl::addWaiting(const MSEdge* const edge, MSVehicle *vehicle) throw() {
    if (myWaiting.find(edge) == myWaiting.end()) {
        myWaiting[edge] = std::vector<MSVehicle*>();
    }
    myWaiting[edge].push_back(vehicle);
}


void
MSVehicleControl::removeWaiting(const MSEdge* const edge, MSVehicle *vehicle) throw() {
    if (myWaiting.find(edge) != myWaiting.end()) {
        std::vector<MSVehicle*>::iterator it = std::find(myWaiting[edge].begin(), myWaiting[edge].end(), vehicle);
        if (it != myWaiting[edge].end()) {
            myWaiting[edge].erase(it);
        }
    }
}


MSVehicle *
MSVehicleControl::getWaitingVehicle(const MSEdge* const edge, const std::set<std::string> &lines) throw() {
    if (myWaiting.find(edge) != myWaiting.end()) {
        for (std::vector<MSVehicle*>::const_iterator it = myWaiting[edge].begin(); it != myWaiting[edge].end(); ++it) {
            const std::string &line = (*it)->getParameter().line == "" ? (*it)->getParameter().id : (*it)->getParameter().line;
            if (lines.count(line)) {
                return (*it);
            }
        }
    }
    return 0;
}


/****************************************************************************/

