/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_Tripinfo.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects info on the vehicle trip
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSTransportableControl.h>
#include <mesosim/MEVehicle.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include "MSDevice_Tripinfo.h"

#define NOT_ARRIVED TIME2STEPS(-1)


// ===========================================================================
// static members
// ===========================================================================
std::set<const MSDevice_Tripinfo*, ComparatorNumericalIdLess> MSDevice_Tripinfo::myPendingOutput;

double MSDevice_Tripinfo::myVehicleCount(0);
double MSDevice_Tripinfo::myTotalRouteLength(0);
SUMOTime MSDevice_Tripinfo::myTotalDuration(0);
SUMOTime MSDevice_Tripinfo::myTotalWaitingTime(0);
SUMOTime MSDevice_Tripinfo::myTotalTimeLoss(0);
SUMOTime MSDevice_Tripinfo::myTotalDepartDelay(0);
SUMOTime MSDevice_Tripinfo::myWaitingDepartDelay(-1);

int MSDevice_Tripinfo::myWalkCount(0);
double MSDevice_Tripinfo::myTotalWalkRouteLength(0);
SUMOTime MSDevice_Tripinfo::myTotalWalkDuration(0);
SUMOTime MSDevice_Tripinfo::myTotalWalkTimeLoss(0);

int MSDevice_Tripinfo::myRideCount(0);
int MSDevice_Tripinfo::myRideBusCount(0);
int MSDevice_Tripinfo::myRideRailCount(0);
int MSDevice_Tripinfo::myRideBikeCount(0);
int MSDevice_Tripinfo::myRideAbortCount(0);
double MSDevice_Tripinfo::myTotalRideWaitingTime(0);
double MSDevice_Tripinfo::myTotalRideRouteLength(0);
SUMOTime MSDevice_Tripinfo::myTotalRideDuration(0);

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Tripinfo::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    if (OptionsCont::getOptions().isSet("tripinfo-output") || OptionsCont::getOptions().getBool("duration-log.statistics")) {
        MSDevice_Tripinfo* device = new MSDevice_Tripinfo(v, "tripinfo_" + v.getID());
        into.push_back(device);
        myPendingOutput.insert(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Tripinfo-methods
// ---------------------------------------------------------------------------
MSDevice_Tripinfo::MSDevice_Tripinfo(SUMOVehicle& holder, const std::string& id) :
    MSVehicleDevice(holder, id),
    myDepartLane(""),
    myDepartSpeed(-1),
    myDepartPosLat(0),
    myWaitingTime(0),
    myAmWaiting(false),
    myWaitingCount(0),
    myStoppingTime(0),
    myParkingStarted(0),
    myArrivalTime(NOT_ARRIVED),
    myArrivalLane(""),
    myArrivalPos(-1),
    myArrivalPosLat(0.),
    myArrivalSpeed(-1),
    myMesoTimeLoss(0),
    myRouteLength(0.) {
}


MSDevice_Tripinfo::~MSDevice_Tripinfo() {
    // ensure clean up for vaporized vehicles which do not generate output
    myPendingOutput.erase(this);
}

void
MSDevice_Tripinfo::cleanup() {
    myVehicleCount = 0;
    myTotalRouteLength = 0;
    myTotalDuration = 0;
    myTotalWaitingTime = 0;
    myTotalTimeLoss = 0;
    myTotalDepartDelay = 0;
    myWaitingDepartDelay = -1;

    myWalkCount = 0;
    myTotalWalkRouteLength = 0;
    myTotalWalkDuration = 0;
    myTotalWalkTimeLoss = 0;

    myRideCount = 0;
    myRideBusCount = 0;
    myRideRailCount = 0;
    myRideBikeCount = 0;
    myRideAbortCount = 0;
    myTotalRideWaitingTime = 0;
    myTotalRideRouteLength = 0;
    myTotalRideDuration = 0;
}

bool
MSDevice_Tripinfo::notifyMove(SUMOTrafficObject& veh, double /*oldPos*/,
                              double /*newPos*/, double newSpeed) {
    if (veh.isStopped()) {
        myStoppingTime += DELTA_T;
    } else if (newSpeed <= SUMO_const_haltingSpeed) {
        myWaitingTime += DELTA_T;
        if (!myAmWaiting) {
            myWaitingCount++;
            myAmWaiting = true;
        }
    } else {
        myAmWaiting = false;
    }
    return true;
}


void
MSDevice_Tripinfo::notifyMoveInternal(const SUMOTrafficObject& veh,
                                      const double /* frontOnLane */,
                                      const double timeOnLane,
                                      const double /* meanSpeedFrontOnLane */,
                                      const double meanSpeedVehicleOnLane,
                                      const double /* travelledDistanceFrontOnLane */,
                                      const double /* travelledDistanceVehicleOnLane */,
                                      const double /* meanLengthOnLane */) {

    // called by meso
    const MEVehicle* mesoVeh = dynamic_cast<const MEVehicle*>(&veh);
    assert(mesoVeh);
    const double vmax = veh.getEdge()->getVehicleMaxSpeed(&veh);
    if (vmax > 0) {
        myMesoTimeLoss += TIME2STEPS(timeOnLane * (vmax - meanSpeedVehicleOnLane) / vmax);
    }
    myWaitingTime += veh.getWaitingTime();
    myStoppingTime += TIME2STEPS(mesoVeh->getCurrentStoppingTimeSeconds());
}


bool
MSDevice_Tripinfo::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        if (!MSGlobals::gUseMesoSim) {
            myDepartLane = static_cast<MSVehicle&>(veh).getLane()->getID();
            myDepartPosLat = static_cast<MSVehicle&>(veh).getLateralPositionOnLane();
        }
        myDepartSpeed = veh.getSpeed();
        myRouteLength = -veh.getPositionOnLane();
    } else if (reason == MSMoveReminder::NOTIFICATION_PARKING) {
        // notifyMove is not called while parking
        // @note insertion delay when resuming after parking is included
        myStoppingTime += (MSNet::getInstance()->getCurrentTimeStep() - myParkingStarted);
    }
    return true;
}


bool
MSDevice_Tripinfo::notifyLeave(SUMOTrafficObject& veh, double /*lastPos*/,
                               MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        myArrivalTime = MSNet::getInstance()->getCurrentTimeStep();
        if (!MSGlobals::gUseMesoSim) {
            myArrivalLane = static_cast<MSVehicle&>(veh).getLane()->getID();
            myArrivalPosLat = static_cast<MSVehicle&>(veh).getLateralPositionOnLane();
        }
        // @note vehicle may have moved past its arrivalPos during the last step
        // due to non-zero arrivalspeed but we consider it as arrived at the desired position
        // However, vaporization may happen anywhere (via TraCI)
        if (reason == MSMoveReminder::NOTIFICATION_VAPORIZED) {
            myArrivalPos = veh.getPositionOnLane();
        } else {
            myArrivalPos = myHolder.getArrivalPos();
        }
        myArrivalSpeed = veh.getSpeed();
    } else if (reason == MSMoveReminder::NOTIFICATION_PARKING) {
        myParkingStarted = MSNet::getInstance()->getCurrentTimeStep();
    } else if (reason == NOTIFICATION_JUNCTION || reason == NOTIFICATION_TELEPORT) {
        if (MSGlobals::gUseMesoSim) {
            myRouteLength += myHolder.getEdge()->getLength();
        } else {
            myRouteLength += static_cast<MSVehicle&>(veh).getLane()->getLength();
        }
    }
    return true;
}


void
MSDevice_Tripinfo::generateOutput() const {
    const SUMOTime timeLoss = MSGlobals::gUseMesoSim ? myMesoTimeLoss : static_cast<MSVehicle&>(myHolder).getTimeLoss();
    const double routeLength = myRouteLength + (myArrivalTime == NOT_ARRIVED ? myHolder.getPositionOnLane() : myArrivalPos);
    const SUMOTime duration = (myArrivalTime == NOT_ARRIVED ? SIMSTEP : myArrivalTime) - myHolder.getDeparture();

    myVehicleCount++;
    myTotalRouteLength += routeLength;
    myTotalDuration += duration;
    myTotalWaitingTime += myWaitingTime;
    myTotalTimeLoss += timeLoss;
    myTotalDepartDelay += myHolder.getDepartDelay();
    if (!OptionsCont::getOptions().isSet("tripinfo-output")) {
        return;
    }
    myPendingOutput.erase(this);

    // write
    OutputDevice& os = OutputDevice::getDeviceByOption("tripinfo-output");
    os.openTag("tripinfo").writeAttr("id", myHolder.getID());
    os.writeAttr("depart", time2string(myHolder.getDeparture()));
    os.writeAttr("departLane", myDepartLane);
    os.writeAttr("departPos", myHolder.getDepartPos());
    if (MSGlobals::gLateralResolution > 0) {
        os.writeAttr("departPosLat", myDepartPosLat);
    }
    os.writeAttr("departSpeed", myDepartSpeed);
    os.writeAttr("departDelay", time2string(myHolder.getDepartDelay()));
    os.writeAttr("arrival", time2string(myArrivalTime));
    os.writeAttr("arrivalLane", myArrivalLane);
    os.writeAttr("arrivalPos", myArrivalPos);
    if (MSGlobals::gLateralResolution > 0) {
        os.writeAttr("arrivalPosLat", myArrivalPosLat);
    }
    os.writeAttr("arrivalSpeed", myArrivalSpeed);
    os.writeAttr("duration", time2string(duration));
    os.writeAttr("routeLength", routeLength);
    os.writeAttr("waitingTime", time2string(myWaitingTime));
    os.writeAttr("waitingCount", myWaitingCount);
    os.writeAttr("stopTime", time2string(myStoppingTime));
    os.writeAttr("timeLoss", time2string(timeLoss));
    os.writeAttr("rerouteNo", myHolder.getNumberReroutes());
    os.writeAttr("devices", toString(myHolder.getDevices()));
    os.writeAttr("vType", myHolder.getVehicleType().getID());
    os.writeAttr("speedFactor", myHolder.getChosenSpeedFactor());
    os.writeAttr("vaporized", (myHolder.getEdge() == *(myHolder.getRoute().end() - 1) ? "" : "0"));
    // cannot close tag because emission device output might follow
}


void
MSDevice_Tripinfo::generateOutputForUnfinished() {
    MSNet* net = MSNet::getInstance();
    const bool writeTripinfos = OptionsCont::getOptions().isSet("tripinfo-output");
    myWaitingDepartDelay = 0;
    int undeparted = 0;
    int departed = 0;
    const SUMOTime t = net->getCurrentTimeStep();
    while (myPendingOutput.size() > 0) {
        const MSDevice_Tripinfo* d = *myPendingOutput.begin();
        if (d->myHolder.hasDeparted()) {
            departed++;
            d->generateOutput();
            if (writeTripinfos) {
                // @todo also generate emission output if holder has a device
                OutputDevice::getDeviceByOption("tripinfo-output").closeTag();
            } else {
                myPendingOutput.erase(d);
            }
        } else {
            undeparted++;
            myWaitingDepartDelay += (t - d->myHolder.getParameter().depart);
            myPendingOutput.erase(d);
        }
    }
    if (myWaitingDepartDelay > 0) {
        myWaitingDepartDelay /= undeparted;
    }
    // unfinished persons
    if (net->hasPersons()) {
        MSTransportableControl& pc = net->getPersonControl();
        while (pc.loadedBegin() != pc.loadedEnd()) {
            pc.erase(pc.loadedBegin()->second);
        }
    }

}


void
MSDevice_Tripinfo::addPedestrianData(double walkLength, SUMOTime walkDuration, SUMOTime walkTimeLoss) {
    myWalkCount++;
    myTotalWalkRouteLength += walkLength;
    myTotalWalkDuration += walkDuration;
    myTotalWalkTimeLoss += walkTimeLoss;
}


void
MSDevice_Tripinfo::addRideData(double rideLength, SUMOTime rideDuration, SUMOVehicleClass vClass, const std::string& line, SUMOTime waitingTime) {
    myRideCount++;
    if (rideDuration > 0) {
        myTotalRideWaitingTime += waitingTime;
        myTotalRideRouteLength += rideLength;
        myTotalRideDuration += rideDuration;
        if (vClass == SVC_BICYCLE) {
            myRideBikeCount++;
        } else if (!line.empty()) {
            if (isRailway(vClass)) {
                myRideRailCount++;
            } else {
                // some kind of road vehicle
                myRideBusCount++;
            }
        }
    } else {
        myRideAbortCount++;
    }
}


std::string
MSDevice_Tripinfo::printStatistics() {
    std::ostringstream msg;
    msg.setf(msg.fixed);
    msg.precision(gPrecision);
    msg << "Statistics (avg):\n"
        << " RouteLength: " << getAvgRouteLength() << "\n"
        << " Duration: " << getAvgDuration() << "\n"
        << " WaitingTime: " << getAvgWaitingTime() << "\n"
        << " TimeLoss: " << getAvgTimeLoss() << "\n"
        << " DepartDelay: " << getAvgDepartDelay() << "\n";
    if (myWaitingDepartDelay >= 0) {
        msg << " DepartDelayWaiting: " << STEPS2TIME(myWaitingDepartDelay) << "\n";
    }
    if (myWalkCount > 0) {
        msg << "Pedestrian Statistics (avg of " << myWalkCount << " walks):\n"
            << " RouteLength: " << getAvgWalkRouteLength() << "\n"
            << " Duration: " << getAvgWalkDuration() << "\n"
            << " TimeLoss: " << getAvgWalkTimeLoss() << "\n";
    }
    if (myRideCount > 0) {
        msg << "Ride Statistics (avg of " << myRideCount << " rides):\n"
            << " WaitingTime: " << getAvgRideWaitingTime() << "\n"
            << " RouteLength: " << getAvgRideRouteLength() << "\n"
            << " Duration: " << getAvgRideDuration() << "\n"
            << " Bus: " << myRideBusCount << "\n"
            << " Train: " << myRideRailCount << "\n"
            << " Bike: " << myRideBikeCount << "\n"
            << " Aborted: " << myRideAbortCount << "\n";
    }
    return msg.str();
}


double
MSDevice_Tripinfo::getAvgRouteLength() {
    if (myVehicleCount > 0) {
        return myTotalRouteLength / myVehicleCount;
    } else {
        return 0;
    }
}

double
MSDevice_Tripinfo::getAvgDuration() {
    if (myVehicleCount > 0) {
        return STEPS2TIME(myTotalDuration / myVehicleCount);
    } else {
        return 0;
    }
}

double
MSDevice_Tripinfo::getAvgWaitingTime() {
    if (myVehicleCount > 0) {
        return STEPS2TIME(myTotalWaitingTime / myVehicleCount);
    } else {
        return 0;
    }
}


double
MSDevice_Tripinfo::getAvgTimeLoss() {
    if (myVehicleCount > 0) {
        return STEPS2TIME(myTotalTimeLoss / myVehicleCount);
    } else {
        return 0;
    }
}


double
MSDevice_Tripinfo::getAvgDepartDelay() {
    if (myVehicleCount > 0) {
        return STEPS2TIME(myTotalDepartDelay / myVehicleCount);
    } else {
        return 0;
    }
}


double
MSDevice_Tripinfo::getAvgWalkRouteLength() {
    if (myWalkCount > 0) {
        return myTotalWalkRouteLength / myWalkCount;
    } else {
        return 0;
    }
}

double
MSDevice_Tripinfo::getAvgWalkDuration() {
    if (myWalkCount > 0) {
        return STEPS2TIME(myTotalWalkDuration / myWalkCount);
    } else {
        return 0;
    }
}


double
MSDevice_Tripinfo::getAvgWalkTimeLoss() {
    if (myWalkCount > 0) {
        return STEPS2TIME(myTotalWalkTimeLoss / myWalkCount);
    } else {
        return 0;
    }
}


double
MSDevice_Tripinfo::getAvgRideDuration() {
    if (myRideCount > 0) {
        return STEPS2TIME(myTotalRideDuration / myRideCount);
    } else {
        return 0;
    }
}

double
MSDevice_Tripinfo::getAvgRideWaitingTime() {
    if (myRideCount > 0) {
        return STEPS2TIME(myTotalRideWaitingTime / myRideCount);
    } else {
        return 0;
    }
}

double
MSDevice_Tripinfo::getAvgRideRouteLength() {
    if (myRideCount > 0) {
        return myTotalRideRouteLength / myRideCount;
    } else {
        return 0;
    }
}


void
MSDevice_Tripinfo::saveState(OutputDevice& out) const {
    out.openTag(SUMO_TAG_DEVICE);
    out.writeAttr(SUMO_ATTR_ID, getID());
    std::vector<std::string> internals;
    if (!MSGlobals::gUseMesoSim) {
        internals.push_back(myDepartLane);
        internals.push_back(toString(myDepartPosLat));
    }
    internals.push_back(toString(myDepartSpeed));
    internals.push_back(toString(myRouteLength));
    out.writeAttr(SUMO_ATTR_STATE, toString(internals));
    out.closeTag();
}


void
MSDevice_Tripinfo::loadState(const SUMOSAXAttributes& attrs) {
    std::istringstream bis(attrs.getString(SUMO_ATTR_STATE));
    if (!MSGlobals::gUseMesoSim) {
        bis >> myDepartLane;
        bis >> myDepartPosLat;
    }
    bis >> myDepartSpeed;
    bis >> myRouteLength;
}


/****************************************************************************/
