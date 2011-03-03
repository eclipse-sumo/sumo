/****************************************************************************/
/// @file    MSInductLoop.cpp
/// @author  Christian Roessel
/// @date    2004-11-23
/// @version $Id$
///
// An unextended detector measuring at a fixed position on a fixed lane.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include "MSInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
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
MSInductLoop::MSInductLoop(const std::string& id,
                           MSLane * const lane,
                           SUMOReal positionInMeters) throw()
        : MSMoveReminder(lane), Named(id), myCurrentVehicle(0),
        myPosition(positionInMeters), myLastLeaveTime(0),
        myVehiclesOnDet(), myVehicleDataCont() {
    assert(myPosition >= 0 && myPosition <= myLane->getLength());
    reset();
    myLastLeaveTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
}


MSInductLoop::~MSInductLoop() throw() {
}


void
MSInductLoop::reset() throw() {
    myDismissedVehicleNumber = 0;
    myLastVehicleDataCont = myVehicleDataCont;
    myVehicleDataCont.clear();
}


bool
MSInductLoop::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                         SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }
    if (myVehiclesOnDet.find(&veh) == myVehiclesOnDet.end()) {
        // entered the detector by move
        SUMOReal entryTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
        if (newSpeed!=0) {
            entryTime += (myPosition - oldPos) / newSpeed;
        }
        enterDetectorByMove(veh, entryTime);
    }
    if (newPos - veh.getVehicleType().getLength() > myPosition) {
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
MSInductLoop::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/, MSMoveReminder::Notification reason) throw() {
    if (reason != MSMoveReminder::NOTIFICATION_JUNCTION) {
        // vehicle is on detector during lane change or arrival, or ...
        leaveDetectorByLaneChange(veh);
        return false;
    }
    return true;
}


bool
MSInductLoop::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification) throw() {
    if (veh.getPositionOnLane() - veh.getVehicleType().getLength() > myPosition) {
        // vehicle-front is beyond detector. Ignore
        return false;
    }
    // vehicle is in front of detector
    return true;
}


SUMOReal
MSInductLoop::getCurrentSpeed() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    return d.size()!=0
           ? accumulate(d.begin(), d.end(), (SUMOReal) 0.0, speedSum) / (SUMOReal) d.size()
           : -1;
}


SUMOReal
MSInductLoop::getCurrentLength() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    return d.size()!=0
           ? accumulate(d.begin(), d.end(), (SUMOReal) 0.0, lengthSum) / (SUMOReal) d.size()
           : -1;
}


SUMOReal
MSInductLoop::getCurrentOccupancy() const throw() {
    SUMOTime tbeg = MSNet::getInstance()->getCurrentTimeStep()-DELTA_T;
    std::vector<VehicleData> d = collectVehiclesOnDet(tbeg);
    if (d.size()==0) {
        return -1;
    }
    SUMOReal occupancy = 0;
    for (std::vector< VehicleData >::const_iterator i=d.begin(); i!=d.end(); ++i) {
        SUMOReal timeOnDetDuringInterval = (*i).leaveTimeM - MAX2(STEPS2TIME(tbeg), (*i).entryTimeM);
        timeOnDetDuringInterval = MIN2(timeOnDetDuringInterval, TS);
        occupancy += timeOnDetDuringInterval;
    }
    return occupancy / TS *(SUMOReal) 100.;
}


unsigned int
MSInductLoop::getCurrentPassedNumber() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    return (SUMOReal) d.size();
}


std::vector<std::string>
MSInductLoop::getCurrentVehicleIDs() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    std::vector<std::string> ret;
    for (std::vector<VehicleData>::iterator i=d.begin(); i!=d.end(); ++i) {
        ret.push_back((*i).idM);
    }
    return ret;
}


SUMOReal
MSInductLoop::getTimestepsSinceLastDetection() const throw() {
    if (myVehiclesOnDet.size() != 0) {
        // detector is occupied
        return 0;
    }
    return STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) - myLastLeaveTime;
}


void
MSInductLoop::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("detector");
}


void
MSInductLoop::writeXMLOutput(OutputDevice &dev,
                             SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    SUMOReal t(STEPS2TIME(stopTime-startTime));
    unsigned nVehCrossed = (unsigned) myVehicleDataCont.size() + myDismissedVehicleNumber;
    SUMOReal flow = ((SUMOReal) myVehicleDataCont.size() / (SUMOReal) t) * (SUMOReal) 3600.0;
    SUMOReal occupancy = 0;
    for (std::deque< VehicleData >::const_iterator i=myVehicleDataCont.begin(); i!=myVehicleDataCont.end(); ++i) {
        SUMOReal timeOnDetDuringInterval = (*i).leaveTimeM - MAX2(STEPS2TIME(startTime), (*i).entryTimeM);
        timeOnDetDuringInterval = MIN2(timeOnDetDuringInterval, t);
        occupancy += timeOnDetDuringInterval;
    }
    for (std::map< SUMOVehicle*, SUMOReal >::const_iterator i=myVehiclesOnDet.begin(); i!=myVehiclesOnDet.end(); ++i) {
        SUMOReal timeOnDetDuringInterval = STEPS2TIME(stopTime) - MAX2(STEPS2TIME(startTime), (*i).second);
        occupancy += timeOnDetDuringInterval;
    }
    occupancy = occupancy / t * (SUMOReal) 100.;
    SUMOReal meanSpeed = myVehicleDataCont.size()!=0
                         ? accumulate(myVehicleDataCont.begin(), myVehicleDataCont.end(), (SUMOReal) 0.0, speedSum) / (SUMOReal) myVehicleDataCont.size()
                         : -1;
    SUMOReal meanLength = myVehicleDataCont.size()!=0
                          ? accumulate(myVehicleDataCont.begin(), myVehicleDataCont.end(), (SUMOReal) 0.0, lengthSum) / (SUMOReal) myVehicleDataCont.size()
                          : -1;
    dev<<"   <interval begin=\""<<time2string(startTime)<<"\" end=\""<<
    time2string(stopTime)<<"\" "<<"id=\""<<StringUtils::escapeXML(getID())<<"\" ";
    dev<<"nVehContrib=\""<<myVehicleDataCont.size()<<"\" flow=\""<<flow<<
    "\" occupancy=\""<<occupancy<<"\" speed=\""<<meanSpeed<<
    "\" length=\""<<meanLength<<
    "\" nVehEntered=\""<<nVehCrossed<<"\"/>\n";
    reset();
}


void
MSInductLoop::enterDetectorByMove(SUMOVehicle& veh,
                                  SUMOReal entryTimestep) throw() {
    myVehiclesOnDet.insert(std::make_pair(&veh, entryTimestep));
    if (myCurrentVehicle!=0&&myCurrentVehicle!=&veh) {
        // in fact, this is an error - a second vehicle is on the detector
        //  before the first one leaves... (collision)
        // Still, this seems to happen, but should not be handled herein.
        //  we will inform the user, etc., but continue as nothing had happened
        MsgHandler::getWarningInstance()->inform("Collision on e1-detector '" + getID() + "'.\n Vehicle '" + myCurrentVehicle->getID() +
                "' was already at detector as '" + veh.getID() + "' entered at '" + toString(entryTimestep) + "'.");
        leaveDetectorByMove(*myCurrentVehicle, entryTimestep);
    }
    myCurrentVehicle = &veh;
}


void
MSInductLoop::leaveDetectorByMove(SUMOVehicle& veh,
                                  SUMOReal leaveTimestep) throw() {
    VehicleMap::iterator it = myVehiclesOnDet.find(&veh);
    assert(it != myVehiclesOnDet.end());
    SUMOReal entryTimestep = it->second;
    myVehiclesOnDet.erase(it);
    assert(entryTimestep < leaveTimestep);
    myVehicleDataCont.push_back(VehicleData(veh.getID(), veh.getVehicleType().getLength(), entryTimestep, leaveTimestep));
    myLastOccupancy = leaveTimestep - entryTimestep;
    myLastLeaveTime = leaveTimestep;
    myCurrentVehicle = 0;
}


void
MSInductLoop::leaveDetectorByLaneChange(SUMOVehicle& veh) throw() {
    // Discard entry data
    myVehiclesOnDet.erase(&veh);
    myDismissedVehicleNumber++;
    myCurrentVehicle = 0;
}


std::vector<MSInductLoop::VehicleData>
MSInductLoop::collectVehiclesOnDet(SUMOTime tMS) const throw() {
    SUMOReal t = STEPS2TIME(tMS);
    std::vector<VehicleData> ret;
    for (VehicleDataCont::const_iterator i=myVehicleDataCont.begin(); i!=myVehicleDataCont.end(); ++i) {
        if ((*i).leaveTimeM>=t) {
            ret.push_back(*i);
        }
    }
    for (VehicleDataCont::const_iterator i=myLastVehicleDataCont.begin(); i!=myLastVehicleDataCont.end(); ++i) {
        if ((*i).leaveTimeM>=t) {
            ret.push_back(*i);
        }
    }
    SUMOTime ct = MSNet::getInstance()->getCurrentTimeStep();
    for (VehicleMap::const_iterator i=myVehiclesOnDet.begin(); i!=myVehiclesOnDet.end(); ++i) {
        SUMOVehicle *v = (*i).first;
        VehicleData d(v->getID(), v->getVehicleType().getLength(), (*i).second, STEPS2TIME(ct));
        d.speedM = v->getSpeed();
        ret.push_back(d);
    }
    return ret;
}


/****************************************************************************/

