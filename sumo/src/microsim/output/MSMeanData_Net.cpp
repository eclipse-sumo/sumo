/****************************************************************************/
/// @file    MSMeanData_Net.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Net.h"
#include <limits>

#ifdef HAVE_MESOSIM
#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// MSMeanData_Net::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Net::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane,
        const SUMOReal length,
        const bool doAdd,
        const std::set<std::string>* const vTypes,
        const MSMeanData_Net *parent) throw()
        : MSMeanData::MeanDataValues(lane, length, doAdd, vTypes), myParent(parent),
        nVehDeparted(0), nVehArrived(0), nVehEntered(0), nVehLeft(0),
        nVehLaneChangeFrom(0), nVehLaneChangeTo(0), waitSeconds(0), vehLengthSum(0) {}


MSMeanData_Net::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_Net::MSLaneMeanDataValues::reset(bool) throw() {
    nVehDeparted = 0;
    nVehArrived = 0;
    nVehEntered = 0;
    nVehLeft = 0;
    nVehLaneChangeFrom = 0;
    nVehLaneChangeTo = 0;
    sampleSeconds = 0.;
    travelledDistance = 0;
    waitSeconds = 0;
    vehLengthSum = 0;
}


void
MSMeanData_Net::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues &val) const throw() {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.nVehDeparted += nVehDeparted;
    v.nVehArrived += nVehArrived;
    v.nVehEntered += nVehEntered;
    v.nVehLeft += nVehLeft;
    v.nVehLaneChangeFrom += nVehLaneChangeFrom;
    v.nVehLaneChangeTo += nVehLaneChangeTo;
    v.sampleSeconds += sampleSeconds;
    v.travelledDistance += travelledDistance;
    v.waitSeconds += waitSeconds;
    v.vehLengthSum += vehLengthSum;
}


void
MSMeanData_Net::MSLaneMeanDataValues::notifyMoveInternal(SUMOVehicle& veh, SUMOReal timeOnLane, SUMOReal speed) throw() {
    sampleSeconds += timeOnLane;
    travelledDistance += speed * timeOnLane;
    vehLengthSum += veh.getVehicleType().getLength() * timeOnLane;
    if (myParent != 0 && speed < myParent->myHaltSpeed) {
        waitSeconds += timeOnLane;
    }
}


bool
MSMeanData_Net::MSLaneMeanDataValues::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/, MSMoveReminder::Notification reason) throw() {
    if (vehicleApplies(veh) && (getLane() == 0 || getLane() == &static_cast<MSVehicle&>(veh).getLane())) {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            myLastVehicleUpdateValues.erase(&veh);
        }
#endif
        if (reason == MSMoveReminder::NOTIFICATION_ARRIVED) {
            ++nVehArrived;
        } else if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
            ++nVehLaneChangeFrom;
        } else {
            ++nVehLeft;
        }
    }
    return reason == MSMoveReminder::NOTIFICATION_JUNCTION;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) throw() {
    if (vehicleApplies(veh)) {
        if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
            ++nVehDeparted;
        } else if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
            ++nVehLaneChangeTo;
        } else {
            ++nVehEntered;
        }
        return true;
    }
    return false;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isEmpty() const throw() {
    return sampleSeconds == 0 && nVehDeparted == 0 && nVehArrived == 0 && nVehEntered == 0 && nVehLeft == 0 && nVehLaneChangeFrom == 0 && nVehLaneChangeTo == 0;
}


void
MSMeanData_Net::MSLaneMeanDataValues::write(OutputDevice &dev, const SUMOTime period,
        const SUMOReal numLanes, const int numVehicles) const throw(IOError) {
    if (myParent == 0) {
        if (sampleSeconds > 0) {
            dev << "\" density=\"" << sampleSeconds / STEPS2TIME(period) *(SUMOReal) 1000 / myLaneLength <<
            "\" occupancy=\"" << vehLengthSum / STEPS2TIME(period) / myLaneLength / numLanes *(SUMOReal) 100 <<
            "\" waitingTime=\"" << waitSeconds <<
            "\" speed=\"" << travelledDistance / sampleSeconds;
        }
        dev<<"\" departed=\""<<nVehDeparted<<
        "\" arrived=\""<<nVehArrived<<
        "\" entered=\""<<nVehEntered<<
        "\" left=\""<<nVehLeft<<
        "\"/>\n";
        return;
    }
    if (sampleSeconds > myParent->myMinSamples) {
        SUMOReal traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            traveltime = MIN2(traveltime, myLaneLength * sampleSeconds / travelledDistance);
        }
        if (numVehicles > 0) {
            dev << "\" traveltime=\"" << sampleSeconds / numVehicles <<
            "\" waitingTime=\"" << waitSeconds <<
            "\" speed=\"" << travelledDistance / sampleSeconds;
        } else {
            dev << "\" traveltime=\"" << traveltime <<
            "\" density=\"" << sampleSeconds / STEPS2TIME(period) *(SUMOReal) 1000 / myLaneLength <<
            "\" occupancy=\"" << vehLengthSum / STEPS2TIME(period) / myLaneLength / numLanes *(SUMOReal) 100 <<
            "\" waitingTime=\"" << waitSeconds <<
            "\" speed=\"" << travelledDistance / sampleSeconds;
        }
    }
    dev<<"\" departed=\""<<nVehDeparted<<
    "\" arrived=\""<<nVehArrived<<
    "\" entered=\""<<nVehEntered<<
    "\" left=\""<<nVehLeft<<
    "\" laneChangedFrom=\""<<nVehLaneChangeFrom<<
    "\" laneChangedTo=\""<<nVehLaneChangeTo<<
    "\"/>\n";
}

// ---------------------------------------------------------------------------
// MSMeanData_Net - methods
// ---------------------------------------------------------------------------
MSMeanData_Net::MSMeanData_Net(const std::string &id,
                               const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                               const bool useLanes, const bool withEmpty, const bool withInternal,
                               const bool trackVehicles,
                               const SUMOReal maxTravelTime, const SUMOReal minSamples,
                               const SUMOReal haltSpeed, const std::set<std::string> vTypes) throw()
        : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, withInternal, trackVehicles, maxTravelTime, minSamples, vTypes),
        myHaltSpeed(haltSpeed) {
}


MSMeanData_Net::~MSMeanData_Net() throw() {}


MSMeanData::MeanDataValues*
MSMeanData_Net::createValues(MSLane * const lane, const SUMOReal length, const bool doAdd) const throw(IOError) {
    return new MSLaneMeanDataValues(lane, length, doAdd, &myVehicleTypes, this);
}


/****************************************************************************/

