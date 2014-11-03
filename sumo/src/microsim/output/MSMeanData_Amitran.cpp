/****************************************************************************/
/// @file    MSMeanData_Amitran.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSMeanData_Amitran.h"
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
// MSMeanData_Amitran::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Amitran::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane* const lane,
        const SUMOReal length,
        const bool doAdd,
        const std::set<std::string>* const vTypes,
        const MSMeanData_Amitran* /* parent */)
    : MSMeanData::MeanDataValues(lane, length, doAdd, vTypes), amount(0) {}


MSMeanData_Amitran::MSLaneMeanDataValues::~MSLaneMeanDataValues() {
}


void
MSMeanData_Amitran::MSLaneMeanDataValues::reset(bool) {
    amount = 0;
    typedAmount.clear();
    typedSamples.clear();
    typedTravelDistance.clear();
}


void
MSMeanData_Amitran::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues& val) const {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.amount += amount;
    v.sampleSeconds += sampleSeconds;
    v.travelledDistance += travelledDistance;
    for (std::map<const MSVehicleType*, unsigned>::const_iterator it = typedAmount.begin(); it != typedAmount.end(); ++it) {
        v.typedAmount[it->first] += it->second;
    }
    for (std::map<const MSVehicleType*, SUMOReal>::const_iterator it = typedSamples.begin(); it != typedSamples.end(); ++it) {
        v.typedSamples[it->first] += it->second;
    }
    for (std::map<const MSVehicleType*, SUMOReal>::const_iterator it = typedTravelDistance.begin(); it != typedTravelDistance.end(); ++it) {
        v.typedTravelDistance[it->first] += it->second;
    }
}


void
MSMeanData_Amitran::MSLaneMeanDataValues::notifyMoveInternal(SUMOVehicle& veh, SUMOReal timeOnLane, SUMOReal speed) {
    sampleSeconds += timeOnLane;
    travelledDistance += speed * timeOnLane;
    typedSamples[&veh.getVehicleType()] += timeOnLane;
    typedTravelDistance[&veh.getVehicleType()] += speed * timeOnLane;
}


bool
MSMeanData_Amitran::MSLaneMeanDataValues::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) {
    if (vehicleApplies(veh)) {
        if (getLane() == 0 || getLane() == static_cast<MSVehicle&>(veh).getLane()) {
            if (reason == MSMoveReminder::NOTIFICATION_DEPARTED || reason == MSMoveReminder::NOTIFICATION_JUNCTION) {
                ++amount;
                typedAmount[&veh.getVehicleType()]++;
            }
        }
        return true;
    }
    return false;
}


bool
MSMeanData_Amitran::MSLaneMeanDataValues::isEmpty() const {
    return sampleSeconds == 0 && amount == 0;
}


void
MSMeanData_Amitran::MSLaneMeanDataValues::write(OutputDevice& dev, const SUMOTime /* period */,
        const SUMOReal /* numLanes */, const SUMOReal defaultTravelTime, const int /* numVehicles */) const {
    if (sampleSeconds > 0) {
        dev.writeAttr("amount", amount).writeAttr("averageSpeed", int(100 * travelledDistance / sampleSeconds));
    } else if (defaultTravelTime >= 0.) {
        dev.writeAttr("amount", amount).writeAttr("averageSpeed", int(100 * myLaneLength / defaultTravelTime));
    } else {
        dev.writeAttr("amount", amount).writeAttr("averageSpeed", "-1");
    }
    if (myVehicleTypes != 0 && !myVehicleTypes->empty()) {
        for (std::map<const MSVehicleType*, unsigned>::const_iterator it = typedAmount.begin(); it != typedAmount.end(); ++it) {
            dev.openTag("actorConfig").writeAttr(SUMO_ATTR_ID, it->first->getNumericalID());
            dev.writeAttr("amount", it->second).writeAttr("averageSpeed", int(100 * typedTravelDistance.find(it->first)->second / typedSamples.find(it->first)->second));
            dev.closeTag();
        }
    }
    dev.closeTag();
}

// ---------------------------------------------------------------------------
// MSMeanData_Amitran - methods
// ---------------------------------------------------------------------------
MSMeanData_Amitran::MSMeanData_Amitran(const std::string& id,
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


MSMeanData_Amitran::~MSMeanData_Amitran() {}


void
MSMeanData_Amitran::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("linkData", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/amitran/linkdata.xsd\"");
}


std::string
MSMeanData_Amitran::getEdgeID(const MSEdge* const edge) {
    return toString(edge->getNumericalID());
}


void
MSMeanData_Amitran::openInterval(OutputDevice& dev, const SUMOTime startTime, const SUMOTime stopTime) {
    const int duration = int(1000 * STEPS2TIME(stopTime - startTime) + 0.5);
    dev.openTag(SUMO_TAG_TIMESLICE).writeAttr(SUMO_ATTR_STARTTIME, int(1000 * STEPS2TIME(startTime) + 0.5)).writeAttr(SUMO_ATTR_DURATION, duration);
}


bool
MSMeanData_Amitran::writePrefix(OutputDevice& dev, const MeanDataValues& values, const SumoXMLTag /* tag */, const std::string id) const {
    if (myDumpEmpty || !values.isEmpty()) {
        dev.openTag("link").writeAttr(SUMO_ATTR_ID, id);
        return true;
    }
    return false;
}


MSMeanData::MeanDataValues*
MSMeanData_Amitran::createValues(MSLane* const lane, const SUMOReal length, const bool doAdd) const {
    return new MSLaneMeanDataValues(lane, length, doAdd, &myVehicleTypes, this);
}


/****************************************************************************/

