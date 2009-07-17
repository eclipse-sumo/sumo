/****************************************************************************/
/// @file    MSMsgInductLoop.cpp
/// @author  Clemens Honomichl
/// @date    2008-02-21
///
// An unextended detector measuring at a fixed position on a fixed lane.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#ifdef _MESSAGES

#include "MSMsgInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSMsgInductLoop::MSMsgInductLoop(const string& id, const string& msg,
                                 MSLane* lane,
                                 SUMOReal positionInMeters) throw()
        : MSMoveReminder(lane), Named(id), myMsg(msg), myCurrentVehicle(0),
        myCurrentID(""),
        myPosition(positionInMeters), myLastLeaveTimestep(0),
        myVehiclesOnDet(), myVehicleDataCont() {
    assert(myPosition >= 0 && myPosition <= lane->length());
    reset();
    myLastLeaveTimestep = (SUMOReal) MSNet::getInstance()->getCurrentTimeStep();
}


MSMsgInductLoop::~MSMsgInductLoop() throw() {
    if (myCurrentVehicle!=0) {
        myCurrentVehicle->quitRemindedLeft(this);
    }
    myCurrentVehicle = 0;
}


void
MSMsgInductLoop::reset() throw() {
    myDismissedVehicleNumber = 0;
    myVehicleDataCont.clear();
    myCurrentID = "";
}


bool
MSMsgInductLoop::isStillActive(MSVehicle& veh, SUMOReal oldPos,
                               SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }
    if (myVehiclesOnDet.find(&veh) == myVehiclesOnDet.end()) {
        // entered the detector by move
        SUMOReal entryTimestep = (SUMOReal)
                                 ((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos) / newSpeed));
        if (newPos - veh.getLength() > myPosition) {
            // entered and passed detector in a single timestep
            SUMOReal leaveTimestep = (SUMOReal)
                                     ((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos + veh.getLength()) / newSpeed));
            enterDetectorByMove(veh, entryTimestep);
            leaveDetectorByMove(veh, leaveTimestep);
            return false;
        }
        // entered detector, but not passed
        enterDetectorByMove(veh, entryTimestep);
        return true;
    } else {
        // vehicle has been on the detector the previous timestep
        if (newPos - veh.getLength() >= myPosition) {
            // vehicle passed the detector
            SUMOReal leaveTimestep = (SUMOReal)
                                     ((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos + veh.getLength()) / newSpeed));
            leaveDetectorByMove(veh, leaveTimestep);
            return false;
        }
        // vehicle stays on the detector
        return true;
    }
}


void
MSMsgInductLoop::dismissOnLeavingLane(MSVehicle& veh) throw() {
    if (veh.getPositionOnLane() > myPosition && veh.getPositionOnLane() - veh.getLength() <= myPosition) {
        // vehicle is on detector during lane change
        leaveDetectorByLaneChange(veh);
    }
}


bool
MSMsgInductLoop::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw() {
    if (veh.getPositionOnLane() - veh.getLength() > myPosition) {
        // vehicle-front is beyond detector. Ignore
        return false;
    }
    // vehicle is in front of detector
    return true;
}


SUMOReal
MSMsgInductLoop::getCurrentSpeed() const throw() {
    if (myCurrentVehicle!=0) {
        return myCurrentVehicle->getSpeed();
    }
    return -1;
}


SUMOReal
MSMsgInductLoop::getCurrentLength() const throw() {
    if (myCurrentVehicle!=0) {
        return myCurrentVehicle->getLength();
    }
    return -1;
}


SUMOReal
MSMsgInductLoop::getCurrentOccupancy() const throw() {
    if (myCurrentVehicle!=0) {
        return 1.;
    }
    if (myLastLeaveTimestep>MSNet::getInstance()->getCurrentTimeStep()-DELTA_T) {
        return 0.;
    }
    return myLastOccupancy;
}


SUMOReal
MSMsgInductLoop::getCurrentPassedNumber() const throw() {
    if (myCurrentVehicle!=0) {
        return 1.;
    }
    if (myLastLeaveTimestep>MSNet::getInstance()->getCurrentTimeStep()-DELTA_T) {
        return 0.;
    }
    return 1.;
}


unsigned
MSMsgInductLoop::getNVehContributed() const throw() {
    return (unsigned) myVehicleDataCont.size();
}


SUMOReal
MSMsgInductLoop::getTimestepsSinceLastDetection() const throw() {
    if (myVehiclesOnDet.size() != 0) {
        // detector is occupied
        return 0;
    }
    return MSNet::getInstance()->getCurrentTimeStep() - myLastLeaveTimestep;
}


void
MSMsgInductLoop::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("detector");
}


void
MSMsgInductLoop::writeXMLOutput(OutputDevice &dev,
                                SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    //dev<< "	<interval begin=\""<<startTime<<"\" end=\""<<stopTime<<"\" id=\""<<getID()<<"\" msg=\""<<myMsg<<"\" />\n";
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
    //dev<<"   <interval begin=\""<<startTime<<"\" end=\""<<
    //stopTime<<"\" "<<"id=\""<<getID()<<"\" ";
    dev << "   <message timestep=\"" << startTime <<"\" "<<"vID=\""<<myCurrentID<<"\" ";//<<getID()<<"\" ";
    dev<<"nVehContrib=\""<<myVehicleDataCont.size()<<"\" flow=\""<<flow<<
    "\" occupancy=\""<<occupancy<<"\" speed=\""<<meanSpeed<<
    "\" length=\""<<meanLength<<
    "\" nVehEntered=\""<<nVehCrossed<<"\" event_type=\""<<myMsg<<"\" />\n";
    reset();
}


void
MSMsgInductLoop::enterDetectorByMove(MSVehicle& veh,
                                     SUMOReal entryTimestep) throw() {
    myVehiclesOnDet.insert(make_pair(&veh, entryTimestep));
    veh.quitRemindedEntered(this);
    myCurrentVehicle = &veh;
}


void
MSMsgInductLoop::leaveDetectorByMove(MSVehicle& veh,
                                     SUMOReal leaveTimestep) throw() {
    VehicleMap::iterator it = myVehiclesOnDet.find(&veh);
    assert(it != myVehiclesOnDet.end());
    SUMOReal entryTimestep = it->second;
    myVehiclesOnDet.erase(it);
    assert(entryTimestep < leaveTimestep);
    myVehicleDataCont.push_back(VehicleData(veh.getLength(), entryTimestep, leaveTimestep));
    myLastOccupancy = leaveTimestep - entryTimestep;
    myLastLeaveTimestep = leaveTimestep;
    myCurrentID = myCurrentVehicle->getID();
    myCurrentVehicle = 0;
    veh.quitRemindedLeft(this);
}


void
MSMsgInductLoop::leaveDetectorByLaneChange(MSVehicle& veh) throw() {
    // Discard entry data
    myVehiclesOnDet.erase(&veh);
    myDismissedVehicleNumber++;
    myCurrentID = myCurrentVehicle->getID();
    myCurrentVehicle = 0;
    veh.quitRemindedLeft(this);
}


void
MSMsgInductLoop::removeOnTripEnd(MSVehicle *veh) throw() {
    myCurrentVehicle = 0;
    myVehiclesOnDet.erase(veh);
}

#endif

/****************************************************************************/

