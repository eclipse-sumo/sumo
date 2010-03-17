/****************************************************************************/
/// @file    MSInductLoop.cpp
/// @author  Christian Roessel
/// @date    2004-11-23
/// @version $Id$
///
// An unextended detector measuring at a fixed position on a fixed lane.
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
        myPosition(positionInMeters), myLastLeaveTimestep(0),
        myVehiclesOnDet(), myVehicleDataCont() {
    assert(myPosition >= 0 && myPosition <= myLane->getLength());
    reset();
    myLastLeaveTimestep = (SUMOReal) MSNet::getInstance()->getCurrentTimeStep();
}


MSInductLoop::~MSInductLoop() throw() {
    if (myCurrentVehicle!=0) {
        myCurrentVehicle->quitRemindedLeft(this);
    }
    myCurrentVehicle = 0;
}


void
MSInductLoop::reset() throw() {
    myDismissedVehicleNumber = 0;
    myLastVehicleDataCont = myVehicleDataCont;
    myVehicleDataCont.clear();
}


bool
MSInductLoop::isStillActive(MSVehicle& veh, SUMOReal oldPos,
                            SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }
    if (myVehiclesOnDet.find(&veh) == myVehiclesOnDet.end()) {
        // entered the detector by move
        SUMOReal entryTimestep = newSpeed!=0
                                 ? (SUMOReal)((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos) / newSpeed))
                                 : (SUMOReal) MSNet::getInstance()->getCurrentTimeStep();
        if (newPos - veh.getVehicleType().getLength() > myPosition) {
            // entered and passed detector in a single timestep
            SUMOReal leaveTimestep = (SUMOReal)
                                     ((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos + veh.getVehicleType().getLength()) / newSpeed));
            enterDetectorByMove(veh, entryTimestep);
            leaveDetectorByMove(veh, leaveTimestep);
            return false;
        }
        // entered detector, but not passed
        enterDetectorByMove(veh, entryTimestep);
        return true;
    } else {
        // vehicle has been on the detector the previous timestep
        if (newPos - veh.getVehicleType().getLength() >= myPosition) {
            // vehicle passed the detector
            SUMOReal leaveTimestep = (SUMOReal)
                                     ((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos + veh.getVehicleType().getLength()) / newSpeed));
            leaveDetectorByMove(veh, leaveTimestep);
            return false;
        }
        // vehicle stays on the detector
        return true;
    }
}


void
MSInductLoop::notifyLeave(MSVehicle& veh, bool isArrival, bool isLaneChange) throw() {
    if (veh.getPositionOnLane() > myPosition && veh.getPositionOnLane() - veh.getVehicleType().getLength() <= myPosition) {
        // vehicle is on detector during lane change
        leaveDetectorByLaneChange(veh);
    }
}


bool
MSInductLoop::notifyEnter(MSVehicle& veh, bool, bool) throw() {
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
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    return d.size()!=0
           ? accumulate(d.begin(), d.end(), (SUMOReal) 0.0, occupancySum) / (SUMOReal) d.size()
           : -1;
}


SUMOReal
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


unsigned
MSInductLoop::getNVehContributed() const throw() {
    return (unsigned int) collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T).size();
}


SUMOReal
MSInductLoop::getTimestepsSinceLastDetection() const throw() {
    if (myVehiclesOnDet.size() != 0) {
        // detector is occupied
        return 0;
    }
    return MSNet::getInstance()->getCurrentTimeStep() - myLastLeaveTimestep;
}


void
MSInductLoop::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("detector");
}


void
MSInductLoop::writeXMLOutput(OutputDevice &dev,
                             SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    SUMOTime t(stopTime-startTime);
    unsigned nVehCrossed = (unsigned) myVehicleDataCont.size() + myDismissedVehicleNumber;
    SUMOReal flow = ((SUMOReal) myVehicleDataCont.size() / (SUMOReal) t) / DELTA_T * (SUMOReal) 3600.0;
    SUMOReal occupancy = accumulate(myVehicleDataCont.begin(), myVehicleDataCont.end(), (SUMOReal) 0.0, occupancySum) / (SUMOReal) t * (SUMOReal) 100.;
    SUMOReal meanSpeed = myVehicleDataCont.size()!=0
                         ? accumulate(myVehicleDataCont.begin(), myVehicleDataCont.end(), (SUMOReal) 0.0, speedSum) / (SUMOReal) myVehicleDataCont.size()
                         : -1;
    SUMOReal meanLength = myVehicleDataCont.size()!=0
                          ? accumulate(myVehicleDataCont.begin(), myVehicleDataCont.end(), (SUMOReal) 0.0, lengthSum) / (SUMOReal) myVehicleDataCont.size()
                          : -1;
    dev<<"   <interval begin=\""<<startTime<<"\" end=\""<<
    stopTime<<"\" "<<"id=\""<<StringUtils::escapeXML(getID())<<"\" ";
    dev<<"nVehContrib=\""<<myVehicleDataCont.size()<<"\" flow=\""<<flow<<
    "\" occupancy=\""<<occupancy<<"\" speed=\""<<meanSpeed<<
    "\" length=\""<<meanLength<<
    "\" nVehEntered=\""<<nVehCrossed<<"\"/>\n";
    reset();
}


void
MSInductLoop::enterDetectorByMove(MSVehicle& veh,
                                  SUMOReal entryTimestep) throw() {
    myVehiclesOnDet.insert(std::make_pair(&veh, entryTimestep));
    veh.quitRemindedEntered(this);
    if (myCurrentVehicle!=0&&myCurrentVehicle!=&veh) {
        // in fact, this is an error - a second vehicle is on the detector
        //  before the first one leaves... (collision)
        // Still, this seems to happen, but should not be handled herein.
        //  we will inform the user, etc., but continue as nothing had happened
        MsgHandler::getWarningInstance()->inform("Collision on e1-detector '" + getID() + "'.\n Vehicle '" + myCurrentVehicle->getID() +
                "' was aready at detector as '" + veh.getID() + "' entered.");
        leaveDetectorByMove(*myCurrentVehicle, entryTimestep);
    }
    myCurrentVehicle = &veh;
}


void
MSInductLoop::leaveDetectorByMove(MSVehicle& veh,
                                  SUMOReal leaveTimestep) throw() {
    VehicleMap::iterator it = myVehiclesOnDet.find(&veh);
    assert(it != myVehiclesOnDet.end());
    SUMOReal entryTimestep = it->second;
    myVehiclesOnDet.erase(it);
    assert(entryTimestep < leaveTimestep);
    myVehicleDataCont.push_back(VehicleData(veh.getID(), veh.getVehicleType().getLength(), entryTimestep, leaveTimestep));
    myLastOccupancy = leaveTimestep - entryTimestep;
    myLastLeaveTimestep = leaveTimestep;
    myCurrentVehicle = 0;
    veh.quitRemindedLeft(this);
}


void
MSInductLoop::leaveDetectorByLaneChange(MSVehicle& veh) throw() {
    // Discard entry data
    myVehiclesOnDet.erase(&veh);
    myDismissedVehicleNumber++;
    myCurrentVehicle = 0;
    veh.quitRemindedLeft(this);
}


void
MSInductLoop::removeOnTripEnd(MSVehicle *veh) throw() {
    myCurrentVehicle = 0;
    myVehiclesOnDet.erase(veh);
}


std::vector<MSInductLoop::VehicleData>
MSInductLoop::collectVehiclesOnDet(SUMOTime t) const throw() {
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
        MSVehicle *v = (*i).first;
        VehicleData d(v->getID(), v->getVehicleType().getLength(), (*i).second, (SUMOReal) ct);
        d.speedM = v->getSpeed();
        ret.push_back(d);
    }
    return ret;
}


/****************************************************************************/

