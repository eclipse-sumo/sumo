/****************************************************************************/
/// @file    MSMeanData_Net.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Net.h"
#include <limits>

#ifdef HAVE_INTERNAL
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
MSMeanData_Net::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane* const lane,
        const SUMOReal length,
        const bool doAdd,
        const std::set<std::string>* const vTypes,
        const MSMeanData_Net* parent)
    : MSMeanData::MeanDataValues(lane, length, doAdd, vTypes),
      nVehDeparted(0), nVehArrived(0), nVehEntered(0), nVehLeft(0),
      nVehVaporized(0), waitSeconds(0),
      nVehLaneChangeFrom(0), nVehLaneChangeTo(0),
      vehLengthSum(0), myParent(parent) {}


MSMeanData_Net::MSLaneMeanDataValues::~MSLaneMeanDataValues() {
}


void
MSMeanData_Net::MSLaneMeanDataValues::reset(bool) {
    nVehDeparted = 0;
    nVehArrived = 0;
    nVehEntered = 0;
    nVehLeft = 0;
    nVehVaporized = 0;
    nVehLaneChangeFrom = 0;
    nVehLaneChangeTo = 0;
    sampleSeconds = 0.;
    travelledDistance = 0;
    waitSeconds = 0;
    vehLengthSum = 0;
}


void
MSMeanData_Net::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues& val) const {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.nVehDeparted += nVehDeparted;
    v.nVehArrived += nVehArrived;
    v.nVehEntered += nVehEntered;
    v.nVehLeft += nVehLeft;
    v.nVehVaporized += nVehVaporized;
    v.nVehLaneChangeFrom += nVehLaneChangeFrom;
    v.nVehLaneChangeTo += nVehLaneChangeTo;
    v.sampleSeconds += sampleSeconds;
    v.travelledDistance += travelledDistance;
    v.waitSeconds += waitSeconds;
    v.vehLengthSum += vehLengthSum;
}


void
MSMeanData_Net::MSLaneMeanDataValues::notifyMoveInternal(SUMOVehicle& veh, SUMOReal timeOnLane, SUMOReal speed) {
    sampleSeconds += timeOnLane;
    travelledDistance += speed * timeOnLane;
    vehLengthSum += veh.getVehicleType().getLength() * timeOnLane;
    if (myParent != 0 && speed < myParent->myHaltSpeed) {
        waitSeconds += timeOnLane;
    }
}


bool
MSMeanData_Net::MSLaneMeanDataValues::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/, MSMoveReminder::Notification reason) {
    if (vehicleApplies(veh) && (getLane() == 0 || getLane() == static_cast<MSVehicle&>(veh).getLane())) {
#ifdef HAVE_INTERNAL
        if (MSGlobals::gUseMesoSim) {
            myLastVehicleUpdateValues.erase(&veh);
        }
#endif
        if (reason == MSMoveReminder::NOTIFICATION_ARRIVED) {
            ++nVehArrived;
        } else if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
            ++nVehLaneChangeFrom;
        } else if (myParent == 0 || reason != MSMoveReminder::NOTIFICATION_SEGMENT) {
            ++nVehLeft;
            if (reason == MSMoveReminder::NOTIFICATION_VAPORIZED) {
                ++nVehVaporized;
            }
        }
    }
#ifdef HAVE_INTERNAL
    if (MSGlobals::gUseMesoSim) {
        return false;
    }
#endif
    return reason == MSMoveReminder::NOTIFICATION_JUNCTION;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) {
    if (vehicleApplies(veh)) {
        if (getLane() == 0 || getLane() == static_cast<MSVehicle&>(veh).getLane()) {
            if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
                ++nVehDeparted;
            } else if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
                ++nVehLaneChangeTo;
            } else if (myParent == 0 || reason != MSMoveReminder::NOTIFICATION_SEGMENT) {
                ++nVehEntered;
            }
        }
        return true;
    }
    return false;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isEmpty() const {
    return sampleSeconds == 0 && nVehDeparted == 0 && nVehArrived == 0 && nVehEntered == 0
           && nVehLeft == 0 && nVehVaporized == 0 && nVehLaneChangeFrom == 0 && nVehLaneChangeTo == 0;
}


void
MSMeanData_Net::MSLaneMeanDataValues::write(OutputDevice& dev, const SUMOTime period,
        const SUMOReal numLanes, const SUMOReal defaultTravelTime, const int numVehicles) const {
    if (myParent == 0) {
        if (sampleSeconds > 0) {
            dev.writeAttr("density", sampleSeconds / STEPS2TIME(period) * (SUMOReal) 1000 / myLaneLength)
            .writeAttr("occupancy", vehLengthSum / STEPS2TIME(period) / myLaneLength / numLanes * (SUMOReal) 100)
            .writeAttr("waitingTime", waitSeconds).writeAttr("speed", travelledDistance / sampleSeconds);
        }
        dev.writeAttr("departed", nVehDeparted).writeAttr("arrived", nVehArrived).writeAttr("entered", nVehEntered).writeAttr("left", nVehLeft);
        if (nVehVaporized > 0) {
            dev.writeAttr("vaporized", nVehVaporized);
        }
        dev.closeTag();
        return;
    }
    if (sampleSeconds > myParent->myMinSamples) {
        SUMOReal traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            traveltime = MIN2(traveltime, myLaneLength * sampleSeconds / travelledDistance);
        }
        if (numVehicles > 0) {
            dev.writeAttr("traveltime", sampleSeconds / numVehicles).writeAttr("waitingTime", waitSeconds).writeAttr("speed", travelledDistance / sampleSeconds);
        } else {
            dev.writeAttr("traveltime", traveltime)
            .writeAttr("density", sampleSeconds / STEPS2TIME(period) * (SUMOReal) 1000 / myLaneLength)
            .writeAttr("occupancy", vehLengthSum / STEPS2TIME(period) / myLaneLength / numLanes * (SUMOReal) 100)
            .writeAttr("waitingTime", waitSeconds).writeAttr("speed", travelledDistance / sampleSeconds);
        }
    } else if (defaultTravelTime >= 0.) {
        dev.writeAttr("traveltime", defaultTravelTime).writeAttr("speed", myLaneLength / defaultTravelTime);
    }
    dev.writeAttr("departed", nVehDeparted).writeAttr("arrived", nVehArrived).writeAttr("entered", nVehEntered).writeAttr("left", nVehLeft)
    .writeAttr("laneChangedFrom", nVehLaneChangeFrom).writeAttr("laneChangedTo", nVehLaneChangeTo);
    if (nVehVaporized > 0) {
        dev.writeAttr("vaporized", nVehVaporized);
    }
    dev.closeTag();
}

// ---------------------------------------------------------------------------
// MSMeanData_Net - methods
// ---------------------------------------------------------------------------
MSMeanData_Net::MSMeanData_Net(const std::string& id,
                               const SUMOTime dumpBegin,
                               const SUMOTime dumpEnd, const bool useLanes,
                               const bool withEmpty, const bool printDefaults,
                               const bool withInternal,
                               const bool trackVehicles,
                               const SUMOReal maxTravelTime,
                               const SUMOReal minSamples,
                               const SUMOReal haltSpeed,
                               const std::set<std::string> vTypes)
    : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, printDefaults,
                 withInternal, trackVehicles, maxTravelTime, minSamples, vTypes),
    myHaltSpeed(haltSpeed) {
}


MSMeanData_Net::~MSMeanData_Net() {}


MSMeanData::MeanDataValues*
MSMeanData_Net::createValues(MSLane* const lane, const SUMOReal length, const bool doAdd) const {
    return new MSLaneMeanDataValues(lane, length, doAdd, &myVehicleTypes, this);
}


/****************************************************************************/

