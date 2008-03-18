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
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>
#include "devices/MSDevice_CPhone.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSVehicleControl::MSVehicleControl()
        : myLoadedVehNo(0), myRunningVehNo(0), myEndedVehNo(0),
        myAbsVehWaitingTime(0), myAbsVehTravelTime(0), myHaveDefaultVTypeOnly(true)
{
    // add a default vehicle type (probability to choose=1)
    addVType(new MSVehicleType("DEFAULT_VEHTYPE", DEFAULT_VEH_LENGTH, DEFAULT_VEH_MAXSPEED, DEFAULT_VEH_A, DEFAULT_VEH_B, DEFAULT_VEH_SIGMA, DEFAULT_VEH_TAU, SVC_UNKNOWN), 1.);
    // mark that we have a default only
    myHaveDefaultVTypeOnly = true;
}


MSVehicleControl::~MSVehicleControl()
{
    // delete vehicles
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); ++i) {
        delete(*i).second;
    }
    // delete vehicle types
    for (vector<MSVehicleType*>::const_iterator i=myObsoleteVehicleTypes.begin(); i!=myObsoleteVehicleTypes.end(); ++i) {
        delete *i;
    }
    const vector<MSVehicleType*> &vehs1 = myVehicleTypeDistribution.getVals();
    for (vector<MSVehicleType*>::const_iterator i=vehs1.begin(); i!=vehs1.end(); ++i) {
        delete *i;
    }
    // delete current types
    myVehicleDict.clear();
}


MSVehicle *
MSVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               SUMOTime departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset)
{
    myLoadedVehNo++;
    return new MSVehicle(id, route, departTime, type, repNo, repOffset, myLoadedVehNo-1);
}


void
MSVehicleControl::scheduleVehicleRemoval(MSVehicle *v)
{
    assert(myRunningVehNo>0);
    // check whether to generate the information about the vehicle's trip
    if (MSCORN::wished(MSCORN::CORN_OUT_TRIPDURATIONS)) {
        // generate vehicle's trip information
        MSNet *net = MSNet::getInstance();
        OutputDevice& od = OutputDevice::getDeviceByOption("tripinfo-output");
        SUMOTime realDepart = (SUMOTime) v->getCORNIntValue(MSCORN::CORN_VEH_REALDEPART);
        SUMOTime time = net->getCurrentTimeStep();
        od
        << "   <tripinfo vehicle_id=\"" << v->getID() << "\" "
        << "start=\"" << realDepart << "\" "
        << "wished=\"" << v->desiredDepart() << "\" "
        << "end=\"" << time << "\" "
        << "duration=\"" << time-realDepart << "\" "
        << "waited=\"" << realDepart-v->desiredDepart() << "\" "
        // write reroutes
        << "reroutes=\"";
        if (v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            od << v->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
        } else {
            od << '0';
        }
        od << "\" devices=\"" << v->buildDeviceIDList()
            << "\" vtype=\"" << v->getVehicleType().getID() << "\"/>\n";
    }

    // check whether to generate the information about the vehicle's routes
    if (MSCORN::wished(MSCORN::CORN_OUT_VEHROUTES)) {
        // generate vehicle's trip routes
        MSNet *net = MSNet::getInstance();
        OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
        SUMOTime realDepart = (SUMOTime) v->getCORNIntValue(MSCORN::CORN_VEH_REALDEPART);
        SUMOTime time = net->getCurrentTimeStep();
        od
        << "   <vehicle id=\"" << v->getID() << "\" emittedAt=\""
        << v->getCORNIntValue(MSCORN::CORN_VEH_REALDEPART)
        << "\" endedAt=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\">" << "\n";
        if (v->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            int noReroutes = v->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
            for (int i=0; i<noReroutes; ++i) {
                v->writeXMLRoute(od, i);
                od << "\n";
            }
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
             - v->getCORNIntValue(MSCORN::CORN_VEH_REALDEPART));
    }
    myRunningVehNo--;
    myEndedVehNo++;
    deleteVehicle(v);
}


void
MSVehicleControl::newUnbuildVehicleLoaded()
{
    myLoadedVehNo++;
}


void
MSVehicleControl::newUnbuildVehicleBuild()
{
    myLoadedVehNo--;
}



unsigned int
MSVehicleControl::getLoadedVehicleNo() const
{
    return myLoadedVehNo;
}


unsigned int
MSVehicleControl::getEndedVehicleNo() const
{
    return myEndedVehNo;
}


unsigned int
MSVehicleControl::getRunningVehicleNo() const
{
    return myRunningVehNo;
}


unsigned int
MSVehicleControl::getEmittedVehicleNo() const
{
    return myRunningVehNo + myEndedVehNo;
}


SUMOReal
MSVehicleControl::getMeanWaitingTime() const
{
    if (getEmittedVehicleNo()==0) {
        return -1;
    }
    return (SUMOReal) myAbsVehWaitingTime / (SUMOReal) getEmittedVehicleNo();
}


SUMOReal
MSVehicleControl::getMeanTravelTime() const
{
    if (myEndedVehNo==0) {
        return -1;
    }
    return (SUMOReal) myAbsVehTravelTime / (SUMOReal) myEndedVehNo;
}


void
MSVehicleControl::vehiclesEmitted(size_t no)
{
    myRunningVehNo += no;
}


bool
MSVehicleControl::haveAllVehiclesQuit() const
{
    return myLoadedVehNo==myEndedVehNo;
}


void
MSVehicleControl::vehicleEmitted(MSVehicle *v)
{
    if (MSCORN::wished(MSCORN::CORN_MEAN_VEH_WAITINGTIME)) {
        myAbsVehWaitingTime +=
            (v->getCORNIntValue(MSCORN::CORN_VEH_REALDEPART) - v->desiredDepart());
    }
}


void
MSVehicleControl::vehicleMoves(MSVehicle *)
{}


void
MSVehicleControl::saveState(std::ostream &os)
{
    FileHelpers::writeUInt(os, myRunningVehNo);
    FileHelpers::writeUInt(os, myEndedVehNo);

    FileHelpers::writeInt(os, myAbsVehWaitingTime);
    FileHelpers::writeInt(os, myAbsVehTravelTime);
    // save vehicle types
    FileHelpers::writeUInt(os, myVTypeDict.size());
    for (VehTypeDictType::iterator it=myVTypeDict.begin(); it!=myVTypeDict.end(); ++it) {
        (*it).second->saveState(os);
    }
    MSRoute::dict_saveState(os);
    // save vehicles
    FileHelpers::writeUInt(os, myVehicleDict.size());
    for (VehicleDictType::iterator it = myVehicleDict.begin(); it!=myVehicleDict.end(); ++it) {
        (*it).second->saveState(os);
    }
}

void
MSVehicleControl::loadState(BinaryInputDevice &bis)
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
        SUMOReal length, maxSpeed, accel, decel, dawdle, tau;
        int vclass;
        bis >> id;
        bis >> length;
        bis >> maxSpeed;
        bis >> accel;
        bis >> decel;
        bis >> dawdle;
        bis >> tau;
        bis >> vclass;
        MSVehicleType *t = new MSVehicleType(id, length, maxSpeed, accel, decel, dawdle, tau, (SUMOVehicleClass) vclass);
        addVType(t, 1.); // !!!
    }
    MSRoute::dict_loadState(bis);
    // load vehicles
    bis >> size;
    while (size-->0) {
        string id;
        bis >> id;
        SUMOTime lastLaneChangeOffset;
        bis >> lastLaneChangeOffset; // !!! check type= FileHelpers::readInt(os);
        unsigned int waitingTime;
        bis >> waitingTime;
        int repetitionNumber;
        bis >> repetitionNumber;
        int period;
        bis >> period;
        string routeID;
        bis >> routeID;
        MSRoute* route;
        unsigned int desiredDepart;
        bis >> desiredDepart;
        string typeID;
        bis >> typeID;
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
        bool inserted;
        bis >> inserted;
#endif
        route = MSRoute::dictionary(routeID);
        assert(route!=0);
        type = getVType(typeID);
        assert(type!=0);
        assert(getVehicle(id)==0);

        MSVehicle *v = buildVehicle(id, route, desiredDepart, type, repetitionNumber, period);
        if (wasEmitted != -1) {
            v->myIntCORNMap[MSCORN::CORN_VEH_REALDEPART] = wasEmitted;
        }
        while (routeOffset>0) {
            v->myCurrEdge++;
            routeOffset--;
        }
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            v->seg = MSGlobals::gMesoNet->getSegmentForEdge(*(v->myCurrEdge));
            while (v->seg->get_index()!=segIndex) {
                v->seg = MSGlobals::gMesoNet->next_segment(v->seg, v);
            }
            v->tEvent = tEvent;
            v->tLastEntry = tLastEntry;
            v->inserted = inserted!=0;
        }
#endif
        if (!addVehicle(id, v)) {
            MsgHandler::getErrorInstance()->inform("Error: Could not build vehicle " + id + "!");
        }
    }
}


bool
MSVehicleControl::addVehicle(const std::string &id, MSVehicle *v)
{
    VehicleDictType::iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        // id not in myVehicleDict.
        myVehicleDict[id] = v;//.insert(VehicleDictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSVehicle *
MSVehicleControl::getVehicle(const std::string &id)
{
    VehicleDictType::iterator it = myVehicleDict.find(id);
    if (it == myVehicleDict.end()) {
        // id not in myVehicleDict.
        return 0;
    }
    return it->second;
}


void
MSVehicleControl::deleteVehicle(const std::string &id)
{
    VehicleDictType::iterator i = myVehicleDict.find(id);
    MSVehicle *veh = (*i).second;
    myVehicleDict.erase(id);
    delete veh;
}


void
MSVehicleControl::deleteVehicle(MSVehicle *veh)
{
    deleteVehicle(veh->getID());
}


MSVehicle *
MSVehicleControl::detachVehicle(const std::string &id)
{
    VehicleDictType::iterator i = myVehicleDict.find(id);
    if (i==myVehicleDict.end()) {
        return 0;
    }
    MSVehicle *ret = (*i).second;
    myVehicleDict.erase(id);
    return ret;
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehBegin() const
{
    return myVehicleDict.begin();
}


MSVehicleControl::constVehIt
MSVehicleControl::loadedVehEnd() const
{
    return myVehicleDict.end();
}


MSVehicleType *
MSVehicleControl::getRandomVType() const
{
    return myVehicleTypeDistribution.get();
}


bool
MSVehicleControl::addVType(MSVehicleType* vehType, SUMOReal prob)
{
    // if still the default type is used
    if (myHaveDefaultVTypeOnly) {
        // mark it as to be deleted
        if (myVehicleTypeDistribution.getOverallProb()>0) {
            myObsoleteVehicleTypes.push_back(myVehicleTypeDistribution.get());
        }
        // clear the distribution
        myVehicleTypeDistribution.clear();
    }
    // all next vehicle types are non-default
    myHaveDefaultVTypeOnly = false;
    const string &id = vehType->getID();
    VehTypeDictType::iterator it = myVTypeDict.find(id);
    if (it == myVTypeDict.end()) {
        // id not in myDict.
        myVTypeDict[id] = vehType;
        myVehicleTypeDistribution.add(prob, vehType);
        return true;
    }
    return false;
}


MSVehicleType*
MSVehicleControl::getVType(const string &id)
{
    VehTypeDictType::iterator it = myVTypeDict.find(id);
    if (it == myVTypeDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


/****************************************************************************/

