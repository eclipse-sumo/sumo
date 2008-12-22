/****************************************************************************/
/// @file    MSVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <utils/common/FileHelpers.h>
#include <utils/common/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>
#include "devices/MSDevice_CPhone.h"

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSVehicleControl::MSVehicleControl() throw()
        : myLoadedVehNo(0), myRunningVehNo(0), myEndedVehNo(0),
        myAbsVehWaitingTime(0), myAbsVehTravelTime(0),
        myDefaultVTypeMayBeDeleted(true)
{
    myVTypeDict[DEFAULT_VTYPE_ID] = new MSVehicleType(DEFAULT_VTYPE_ID, DEFAULT_VEH_LENGTH,
            DEFAULT_VEH_MAXSPEED, DEFAULT_VEH_A,
            DEFAULT_VEH_B, DEFAULT_VEH_SIGMA,
            DEFAULT_VEH_TAU, DEFAULT_VEH_PROB,
            DEFAULT_VEH_SPEEDFACTOR, DEFAULT_VEH_SPEEDDEV,
            SVC_UNKNOWN, SVE_UNKNOWN, SVS_UNKNOWN,
            DEFAULT_VEH_GUIWIDTH, DEFAULT_VEH_GUIOFFSET,
            DEFAULT_VEH_FOLLOW_MODEL, DEFAULT_VEH_LANE_CHANGE_MODEL,
            RGBColor::DEFAULT_COLOR);
}


MSVehicleControl::~MSVehicleControl() throw()
{
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
                               const MSVehicleType* type) throw()
{
    myLoadedVehNo++;
    return new MSVehicle(defs, route, type, myLoadedVehNo-1);
}


void
MSVehicleControl::scheduleVehicleRemoval(MSVehicle *v) throw()
{
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
        od << "    <tripinfo id=\"" << v->getID() << "\" ";
        SUMOTime departTime = -1;
        if (departInfo!=0) {
            routeLength -= departInfo->pos;
            string laneID = departInfo->lane!=0 ? departInfo->lane->getID() : "";
            od << "depart=\"" << departInfo->time << "\" "
            << "departLane=\"" << laneID << "\" "
            << "departPos=\"" << departInfo->pos << "\" "
            << "departSpeed=\"" << departInfo->speed << "\" "
            << "departDelay=\"" << departInfo->time - v->getDesiredDepart() << "\" ";
            departTime = departInfo->time;
        } else {
            if (v->hasCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME)) {
                departTime = v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME);
                od << "depart=\"" << departTime << "\" ";
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
            string laneID = "";
            if (arrivalInfo->lane!=0) {
                routeLength -= arrivalInfo->lane->length() - arrivalInfo->pos;
                laneID = arrivalInfo->lane->getID();
            }
            od << "arrival=\"" << arrivalInfo->time << "\" "
            << "arrivalLane=\"" << laneID << "\" "
            << "arrivalPos=\"" << arrivalInfo->pos << "\" "
            << "arrivalSpeed=\"" << arrivalInfo->speed << "\" ";
            arrivalTime = arrivalInfo->time;
        } else {
            arrivalTime = MSNet::getInstance()->getCurrentTimeStep();
            od << "arrival=\"" << arrivalTime << "\" "
            << "arrivalLane=\"\" "
            << "arrivalPos=\"\" "
            << "arrivalSpeed=\"\" ";
        }
        if (departTime!=-1&&arrivalTime!=-1) {
            od << "duration=\"" << arrivalTime - departTime << "\" ";
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
        od << "\" devices=\"" << v->buildDeviceIDList()
        << "\" vtype=\"" << v->getVehicleType().getID()
        << "\" vaporized=\"";
        if (v->hasCORNIntValue(MSCORN::CORN_VEH_VAPORIZED)) {
            od << 1 << ";" << v->getCORNIntValue(MSCORN::CORN_VEH_VAPORIZED);
        }
        od << "\"/>\n";
    }

    // check whether to generate the information about the vehicle's routes
    if (MSCORN::wished(MSCORN::CORN_OUT_VEHROUTES)) {
        // generate vehicle's trip routes
        MSNet *net = MSNet::getInstance();
        OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
        SUMOTime realDepart = (SUMOTime) v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME);
        SUMOTime time = net->getCurrentTimeStep();
        od
        << "   <vehicle id=\"" << v->getID() << "\" depart=\""
        << v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME)
        << "\" arrival=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\">" << "\n";
        if (v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            int noReroutes = v->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
            for (int i=0; i<noReroutes; ++i) {
                v->writeXMLRoute(od, i);
            }
            od << "\n";
        }
        v->writeXMLRoute(od);
        od << "   </vehicle>\n\n";
    }

#ifdef HAVE_BOYOM_C2C
    // check whether to save c2c info output
    MSNet *net = MSNet::getInstance();
    if (OptionsCont::getOptions().isSet("c2x.saved-info-freq")) {
        int noReroutes = v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)
                         ? v->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE) : 0;
        OutputDevice::getDeviceByOption("c2x.saved-info-freq")
        << "	<vehicle id=\"" << v->getID()
        << "\" timestep=\"" << net->getCurrentTimeStep()
        << "\" numberOfInfos=\"" << v->getTotalInformationNumber()
        << "\" numberRelevant=\"" << v->getNoGotRelevant()
        << "\" got=\"" << v->getNoGot()
        << "\" sent=\"" << v->getNoSent()
        << "\" reroutes=\"" << noReroutes
        << "\"/>\n";
    }
#endif

    // check whether to save information about the vehicle's trip
    if (MSCORN::wished(MSCORN::CORN_MEAN_VEH_TRAVELTIME)) {
        myAbsVehTravelTime +=
            (MSNet::getInstance()->getCurrentTimeStep()
             - v->getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME));
    }
    myRunningVehNo--;
    myEndedVehNo++;
    deleteVehicle(v);
}


SUMOReal
MSVehicleControl::getMeanWaitingTime() const throw()
{
    if (getEmittedVehicleNo()==0) {
        return -1;
    }
    return (SUMOReal) myAbsVehWaitingTime / (SUMOReal) getEmittedVehicleNo();
}


SUMOReal
MSVehicleControl::getMeanTravelTime() const throw()
{
    if (myEndedVehNo==0) {
        return -1;
    }
    return (SUMOReal) myAbsVehTravelTime / (SUMOReal) myEndedVehNo;
}


void
MSVehicleControl::vehicleEmitted(const MSVehicle &v) throw()
{
    ++myRunningVehNo;
    if (MSCORN::wished(MSCORN::CORN_MEAN_VEH_WAITINGTIME)) {
        myAbsVehWaitingTime += (v.getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME) - v.getDesiredDepart());
    }
}


#ifdef HAVE_MESOSIM
void
MSVehicleControl::saveState(std::ostream &os) throw()
{
    FileHelpers::writeUInt(os, myRunningVehNo);
    FileHelpers::writeUInt(os, myEndedVehNo);

    FileHelpers::writeInt(os, myAbsVehWaitingTime);
    FileHelpers::writeInt(os, myAbsVehTravelTime);
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
MSVehicleControl::loadState(BinaryInputDevice &bis) throw()
{
    bis >> myRunningVehNo;
    bis >> myEndedVehNo;
    myLoadedVehNo = myEndedVehNo;

    bis >> myAbsVehWaitingTime;
    bis >> myAbsVehTravelTime;

    // load vehicle types
    unsigned int size;
    bis >> size;
    while (size-->0) {
        string id;
        SUMOReal length, maxSpeed, accel, decel, dawdle, tau, guiWidth, guiOffset,
        prob, speedFactor, speedDev, r, g, b;
        int vclass, emissionClass, shape;
        string followModel, laneChangeModel;
        bis >> id;
        bis >> length;
        bis >> maxSpeed;
        bis >> accel;
        bis >> decel;
        bis >> dawdle;
        bis >> tau;
        bis >> vclass;
        bis >> emissionClass;
        bis >> shape;
        bis >> guiWidth;
        bis >> guiOffset;
        bis >> prob;
        bis >> speedFactor;
        bis >> speedDev;
        bis >> r;
        bis >> g;
        bis >> b;
        bis >> followModel;
        bis >> laneChangeModel;
        MSVehicleType *t = new MSVehicleType(id, length, maxSpeed, accel, decel, dawdle, tau,
                                             prob, speedFactor, speedDev,
                                             (SUMOVehicleClass) vclass, (SUMOEmissionClass) emissionClass, (SUMOVehicleShape) shape,
                                             guiWidth, guiOffset,
                                             followModel, laneChangeModel, RGBColor(r,g,b));
        addVType(t);
    }
    unsigned int numVTypeDists;
    bis >> numVTypeDists;
    for (;numVTypeDists>0;numVTypeDists--) {
        string id;
        bis >> id;
        unsigned int no;
        bis >> no;
        if (getVType(id)==0) {
            RandomDistributor<MSVehicleType*> *dist = new RandomDistributor<MSVehicleType*>();
            for (;no>0;no--) {
                string vtypeID;
                bis >> vtypeID;
                MSVehicleType *t = getVType(vtypeID);
                assert(t!=0);
                SUMOReal prob;
                bis >> prob;
                dist->add(prob, t);
            }
            addVTypeDistribution(id, dist);
        } else {
            for (;no>0;no--) {
                string vtypeID;
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
        SUMOTime lastLaneChangeOffset;
        bis >> lastLaneChangeOffset; // !!! check type= FileHelpers::readInt(os);
        unsigned int waitingTime;
        bis >> waitingTime;
        bis >> p->repetitionNumber;
        bis >> p->repetitionOffset;
        bis >> p->routeid;
        const MSRoute* route;
        unsigned int desiredDepart; // !!! SUMOTime
        bis >> desiredDepart;
        if (OptionsCont::getOptions().isSet("load-state.offset")) {
            SUMOReal offset = OptionsCont::getOptions().getFloat("load-state.offset");
            desiredDepart -= (unsigned int) offset;
        }
        p->depart = desiredDepart;
        bis >> p->vtypeid;
        const MSVehicleType* type;
        unsigned int routeOffset;
        bis >> routeOffset;
        int wasEmitted;
        bis >> wasEmitted;
#ifdef HAVE_MESOSIM
        unsigned int segIndex;
        bis >> segIndex;
        SUMOReal tEvent;
        bis >> tEvent;
        SUMOReal tLastEntry;
        bis >> tLastEntry;
        if (OptionsCont::getOptions().isSet("load-state.offset")) {
            SUMOReal offset = OptionsCont::getOptions().getFloat("load-state.offset");
            tEvent -= offset;
            tLastEntry -= offset;
        }
        bool inserted;
        bis >> inserted;
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
            v->mySegment = MSGlobals::gMesoNet->getSegmentForEdge(*(v->myCurrEdge));
            while (v->mySegment->get_index()!=segIndex) {
                v->mySegment = MSGlobals::gMesoNet->next_segment(v->mySegment, v);
            }
            v->myEventTime = tEvent;
            v->myLastEntryTime = tLastEntry;
            v->inserted = inserted!=0;
        }
#endif
        if (!addVehicle(p->id, v)) {
            MsgHandler::getErrorInstance()->inform("Error: Could not build vehicle " + p->id + "!");
        }
    }
}
#endif


bool
MSVehicleControl::addVehicle(const std::string &id, MSVehicle *v) throw()
{
    VehicleDictType::iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        // id not in myVehicleDict.
        myVehicleDict[id] = v;
        return true;
    }
    return false;
}


MSVehicle *
MSVehicleControl::getVehicle(const std::string &id) throw()
{
    VehicleDictType::iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        // id not in myVehicleDict.
        return 0;
    }
    return it->second;
}


void
MSVehicleControl::deleteVehicle(const std::string &id) throw()
{
    VehicleDictType::iterator i = myVehicleDict.find(id);
    MSVehicle *veh = (*i).second;
    myVehicleDict.erase(id);
    delete veh;
}


void
MSVehicleControl::deleteVehicle(MSVehicle *veh) throw()
{
    deleteVehicle(veh->getID());
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehBegin() const throw()
{
    return myVehicleDict.begin();
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehEnd() const throw()
{
    return myVehicleDict.end();
}


bool
MSVehicleControl::checkVType(const std::string &id) throw()
{
    if (id == DEFAULT_VTYPE_ID) {
        if (myDefaultVTypeMayBeDeleted) {
            delete myVTypeDict[id];
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
MSVehicleControl::addVType(MSVehicleType* vehType) throw()
{
    if (checkVType(vehType->getID())) {
        myVTypeDict[vehType->getID()] = vehType;
        return true;
    }
    return false;
}


bool
MSVehicleControl::addVTypeDistribution(const std::string &id, RandomDistributor<MSVehicleType*> *vehTypeDistribution) throw()
{
    if (checkVType(id)) {
        myVTypeDistDict[id] = vehTypeDistribution;
        return true;
    }
    return false;
}


MSVehicleType*
MSVehicleControl::getVType(const string &id) throw()
{
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


/****************************************************************************/

