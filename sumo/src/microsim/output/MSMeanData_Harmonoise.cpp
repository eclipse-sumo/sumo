/****************************************************************************/
/// @file    MSMeanData_Harmonoise.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Redirector for mean data output (net->edgecontrol)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Harmonoise.h"
#include <utils/emissions/HelpersHarmonoise.h>
#include <limits>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane* const lane, const SUMOReal length, const bool doAdd,
        const MSMeanData_Harmonoise* parent)
    : MSMeanData::MeanDataValues(lane, length, doAdd, parent),
      currentTimeN(0), meanNTemp(0), myParent(parent) {}


MSMeanData_Harmonoise::MSLaneMeanDataValues::~MSLaneMeanDataValues() {
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::reset(bool) {
    sampleSeconds = 0;
    currentTimeN = 0;
    meanNTemp = 0;
    travelledDistance = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues& val) const {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.sampleSeconds += sampleSeconds;
    v.meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(meanNTemp) / 10.);
    v.travelledDistance += travelledDistance;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::update() {
    meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(currentTimeN) / 10.);
    currentTimeN = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::notifyMoveInternal(const SUMOVehicle& veh, const SUMOReal /* frontOnLane */, const SUMOReal timeOnLane, const SUMOReal /*meanSpeedFrontOnLane*/, const SUMOReal meanSpeedVehicleOnLane, const SUMOReal /*travelledDistanceFrontOnLane*/, const SUMOReal travelledDistanceVehicleOnLane) {
    const SUMOReal sn = HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(),
                        // XXX: recheck, which value to use here for the speed. (Leo) Refs. #2579
                        (double) meanSpeedVehicleOnLane, veh.getAcceleration());
    currentTimeN += (SUMOReal) pow(10., (sn / 10.));
    sampleSeconds += timeOnLane;
    travelledDistance += travelledDistanceVehicleOnLane;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::write(OutputDevice& dev, const SUMOTime period,
        const SUMOReal /*numLanes*/, const SUMOReal defaultTravelTime, const int /*numVehicles*/) const {
    dev.writeAttr("noise", (meanNTemp != 0 ? (SUMOReal)(10. * log10(meanNTemp * TS / STEPS2TIME(period))) : (SUMOReal) 0.));
    if (sampleSeconds > myParent->myMinSamples) {
        SUMOReal traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            traveltime = MIN2(traveltime, myLaneLength * sampleSeconds / travelledDistance);
        }
        dev.writeAttr("traveltime", traveltime);
    } else if (defaultTravelTime >= 0.) {
        // @todo default value for noise
        dev.writeAttr("traveltime", defaultTravelTime);
    }
    dev.closeTag();
}



// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSMeanData_Harmonoise(const std::string& id,
        const SUMOTime dumpBegin, const SUMOTime dumpEnd,
        const bool useLanes, const bool withEmpty,
        const bool printDefaults, const bool withInternal,
        const bool trackVehicles,
        const SUMOReal maxTravelTime, const SUMOReal minSamples,
        const std::string& vTypes)
    : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, printDefaults,
                 withInternal, trackVehicles, maxTravelTime, minSamples, vTypes) {
}


MSMeanData_Harmonoise::~MSMeanData_Harmonoise() {}


MSMeanData::MeanDataValues*
MSMeanData_Harmonoise::createValues(MSLane* const lane, const SUMOReal length, const bool doAdd) const {
    return new MSLaneMeanDataValues(lane, length, doAdd, this);
}


void
MSMeanData_Harmonoise::detectorUpdate(const SUMOTime step) {
    MSMeanData::detectorUpdate(step);
    for (std::vector<std::vector<MeanDataValues*> >::const_iterator i = myMeasures.begin(); i != myMeasures.end(); ++i) {
        const std::vector<MeanDataValues*>& lm = *i;
        for (std::vector<MeanDataValues*>::const_iterator j = lm.begin(); j != lm.end(); ++j) {
            (*j)->update();
        }
    }
}


/****************************************************************************/

