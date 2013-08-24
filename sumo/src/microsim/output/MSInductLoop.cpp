/****************************************************************************/
/// @file    MSInductLoop.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2004-11-23
/// @version $Id$
///
// An unextended detector measuring at a fixed position on a fixed lane.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include "MSInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSInductLoop::MSInductLoop(const std::string& id, MSLane* const lane,
                           SUMOReal positionInMeters, bool splitByType) :
    MSMoveReminder(id, lane),
    MSDetectorFileOutput(id),
    myPosition(positionInMeters), mySplitByType(splitByType),
    myLastLeaveTime(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())),
    myVehicleDataCont(),
    myVehiclesOnDet() {
    assert(myPosition >= 0 && myPosition <= myLane->getLength());
    reset();
}


MSInductLoop::~MSInductLoop() {
}


void
MSInductLoop::reset() {
    myDismissedVehicleNumber = 0;
    myLastVehicleDataCont = myVehicleDataCont;
    myVehicleDataCont.clear();
}


bool
MSInductLoop::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                         SUMOReal newPos, SUMOReal newSpeed) {
    if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }
    if (newPos >= myPosition && oldPos < myPosition) {
        // entered the detector by move
        SUMOReal entryTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
        if (newSpeed != 0) {
            if (myPosition > oldPos) {
                entryTime += (myPosition - oldPos) / newSpeed;
            }
        }
        enterDetectorByMove(veh, entryTime);
    }
    if (newPos - veh.getVehicleType().getLength() > myPosition && oldPos - veh.getVehicleType().getLength() <= myPosition) {
        // vehicle passed the detector
        SUMOReal leaveTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
        leaveTime += (myPosition - oldPos + veh.getVehicleType().getLength()) / newSpeed;
        leaveDetectorByMove(veh, leaveTime);
        return false;
    }
    // vehicle stays on the detector
    return true;
}


bool
MSInductLoop::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/, MSMoveReminder::Notification reason) {
    if (reason != MSMoveReminder::NOTIFICATION_JUNCTION) {
        // vehicle is on detector during lane change or arrival, or ...
        leaveDetectorByLaneChange(veh);
        return false;
    }
    return true;
}


bool
MSInductLoop::notifyEnter(SUMOVehicle& /* veh */, MSMoveReminder::Notification) {
    return true;
}


SUMOReal
MSInductLoop::getCurrentSpeed() const {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep() - DELTA_T);
    return d.size() != 0
           ? accumulate(d.begin(), d.end(), (SUMOReal) 0.0, speedSum) / (SUMOReal) d.size()
           : -1;
}


SUMOReal
MSInductLoop::getCurrentLength() const {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep() - DELTA_T);
    return d.size() != 0
           ? accumulate(d.begin(), d.end(), (SUMOReal) 0.0, lengthSum) / (SUMOReal) d.size()
           : -1;
}


SUMOReal
MSInductLoop::getCurrentOccupancy() const {
    SUMOTime tbeg = MSNet::getInstance()->getCurrentTimeStep() - DELTA_T;
    std::vector<VehicleData> d = collectVehiclesOnDet(tbeg);
    if (d.size() == 0) {
        return -1;
    }
    SUMOReal occupancy = 0;
    for (std::vector< VehicleData >::const_iterator i = d.begin(); i != d.end(); ++i) {
        SUMOReal timeOnDetDuringInterval = (*i).leaveTimeM - MAX2(STEPS2TIME(tbeg), (*i).entryTimeM);
        timeOnDetDuringInterval = MIN2(timeOnDetDuringInterval, TS);
        occupancy += timeOnDetDuringInterval;
    }
    return occupancy / TS * (SUMOReal) 100.;
}


unsigned int
MSInductLoop::getCurrentPassedNumber() const {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep() - DELTA_T);
    return (unsigned int) d.size();
}


std::vector<std::string>
MSInductLoop::getCurrentVehicleIDs() const {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep() - DELTA_T);
    std::vector<std::string> ret;
    for (std::vector<VehicleData>::iterator i = d.begin(); i != d.end(); ++i) {
        ret.push_back((*i).idM);
    }
    return ret;
}


SUMOReal
MSInductLoop::getTimestepsSinceLastDetection() const {
    if (myVehiclesOnDet.size() != 0) {
        // detector is occupied
        return 0;
    }
    return STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) - myLastLeaveTime;
}


void
MSInductLoop::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("detector", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.sf.net/xsd/det_e1_file.xsd\"");
}


void
MSInductLoop::writeXMLOutput(OutputDevice& dev,
                             SUMOTime startTime, SUMOTime stopTime) {
    writeTypedXMLOutput(dev, startTime, stopTime, "", myVehicleDataCont, myVehiclesOnDet);
    if (mySplitByType) {
        dev << ">\n";
        std::map<std::string, std::pair<VehicleDataCont, VehicleMap> > types;
        // collect / divide
        for (std::deque< VehicleData >::const_iterator i = myVehicleDataCont.begin(); i != myVehicleDataCont.end(); ++i) {
            if (types.find((*i).typeIDM) == types.end()) {
                types[(*i).typeIDM] = make_pair(VehicleDataCont(), VehicleMap());
            }
            types[(*i).typeIDM].first.push_back(*i);
        }
        for (std::map< SUMOVehicle*, SUMOReal >::const_iterator i = myVehiclesOnDet.begin(); i != myVehiclesOnDet.end(); ++i) {
            const std::string& type = (*i).first->getVehicleType().getID();
            if (types.find(type) == types.end()) {
                types[type] = make_pair(VehicleDataCont(), VehicleMap());
            }
            types[type].second[(*i).first] = (*i).second;
        }
        // write
        for (std::map<std::string, std::pair<VehicleDataCont, VehicleMap> >::const_iterator i = types.begin(); i != types.end(); ++i) {
            writeTypedXMLOutput(dev, startTime, stopTime, (*i).first, (*i).second.first, (*i).second.second);
            dev << "/>\n";
        }
        dev << "   </interval>\n";
    } else {
        dev << "/>\n";
    }
    reset();
}

void
MSInductLoop::writeTypedXMLOutput(OutputDevice& dev, SUMOTime startTime, SUMOTime stopTime,
                                  const std::string& type, const VehicleDataCont& vdc, const VehicleMap& vm) {
    SUMOReal t(STEPS2TIME(stopTime - startTime));
    unsigned nVehCrossed = (unsigned) vdc.size();
    if (type == "") {
        nVehCrossed += myDismissedVehicleNumber;
    }
    SUMOReal flow = ((SUMOReal) vdc.size() / (SUMOReal) t) * (SUMOReal) 3600.0;
    SUMOReal occupancy = 0;
    for (std::deque< VehicleData >::const_iterator i = vdc.begin(); i != vdc.end(); ++i) {
        SUMOReal timeOnDetDuringInterval = (*i).leaveTimeM - MAX2(STEPS2TIME(startTime), (*i).entryTimeM);
        timeOnDetDuringInterval = MIN2(timeOnDetDuringInterval, t);
        occupancy += timeOnDetDuringInterval;
    }
    for (std::map< SUMOVehicle*, SUMOReal >::const_iterator i = vm.begin(); i != vm.end(); ++i) {
        SUMOReal timeOnDetDuringInterval = STEPS2TIME(stopTime) - MAX2(STEPS2TIME(startTime), (*i).second);
        occupancy += timeOnDetDuringInterval;
    }
    occupancy = occupancy / t * (SUMOReal) 100.;
    SUMOReal meanSpeed = vdc.size() != 0
                         ? accumulate(vdc.begin(), vdc.end(), (SUMOReal) 0.0, speedSum) / (SUMOReal) vdc.size()
                         : -1;
    SUMOReal meanLength = vdc.size() != 0
                          ? accumulate(vdc.begin(), vdc.end(), (SUMOReal) 0.0, lengthSum) / (SUMOReal) vdc.size()
                          : -1;
    if (type != "") {
        dev << "      <typedInterval type=\"" + type + "\" ";
    } else {
        dev << "   <interval ";
    }
    dev << "begin=\"" << time2string(startTime) << "\" end=\"" <<
        time2string(stopTime) << "\" " << "id=\"" << StringUtils::escapeXML(getID()) << "\" ";
    dev << "nVehContrib=\"" << vdc.size() << "\" flow=\"" << flow <<
        "\" occupancy=\"" << occupancy << "\" speed=\"" << meanSpeed <<
        "\" length=\"" << meanLength <<
        "\" nVehEntered=\"" << nVehCrossed << "\"";
}


void
MSInductLoop::enterDetectorByMove(SUMOVehicle& veh,
                                  SUMOReal entryTimestep) {
    myVehiclesOnDet.insert(std::make_pair(&veh, entryTimestep));
}


void
MSInductLoop::leaveDetectorByMove(SUMOVehicle& veh,
                                  SUMOReal leaveTimestep) {
    VehicleMap::iterator it = myVehiclesOnDet.find(&veh);
    if (it != myVehiclesOnDet.end()) {
        SUMOReal entryTimestep = it->second;
        myVehiclesOnDet.erase(it);
        assert(entryTimestep < leaveTimestep);
        myVehicleDataCont.push_back(VehicleData(veh.getID(), veh.getVehicleType().getLength(), entryTimestep, leaveTimestep, veh.getVehicleType().getID()));
        myLastOccupancy = leaveTimestep - entryTimestep;
    }
    myLastLeaveTime = leaveTimestep;
}


void
MSInductLoop::leaveDetectorByLaneChange(SUMOVehicle& veh) {
    // Discard entry data
    myVehiclesOnDet.erase(&veh);
    myDismissedVehicleNumber++;
}


std::vector<MSInductLoop::VehicleData>
MSInductLoop::collectVehiclesOnDet(SUMOTime tMS) const {
    SUMOReal t = STEPS2TIME(tMS);
    std::vector<VehicleData> ret;
    for (VehicleDataCont::const_iterator i = myVehicleDataCont.begin(); i != myVehicleDataCont.end(); ++i) {
        if ((*i).leaveTimeM >= t) {
            ret.push_back(*i);
        }
    }
    for (VehicleDataCont::const_iterator i = myLastVehicleDataCont.begin(); i != myLastVehicleDataCont.end(); ++i) {
        if ((*i).leaveTimeM >= t) {
            ret.push_back(*i);
        }
    }
    SUMOTime ct = MSNet::getInstance()->getCurrentTimeStep();
    for (VehicleMap::const_iterator i = myVehiclesOnDet.begin(); i != myVehiclesOnDet.end(); ++i) {
        SUMOVehicle* v = (*i).first;
        VehicleData d(v->getID(), v->getVehicleType().getLength(), (*i).second, STEPS2TIME(ct), v->getVehicleType().getID());
        d.speedM = v->getSpeed();
        ret.push_back(d);
    }
    return ret;
}


/****************************************************************************/

