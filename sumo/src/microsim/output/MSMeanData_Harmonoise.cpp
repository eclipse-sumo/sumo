/****************************************************************************/
/// @file    MSMeanData_Harmonoise.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Redirector for mean data output (net->edgecontrol)
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

#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Harmonoise.h"
#include <utils/common/HelpersHarmonoise.h>
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
MSMeanData_Harmonoise::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane, const SUMOReal length, const bool doAdd,
        const std::set<std::string>* const vTypes, const MSMeanData_Harmonoise *parent) throw()
        : MSMeanData::MeanDataValues(lane, length, doAdd, vTypes),
        currentTimeN(0), meanNTemp(0), myParent(parent) {}


MSMeanData_Harmonoise::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::reset(bool) throw() {
    sampleSeconds = 0;
    currentTimeN = 0;
    meanNTemp = 0;
    travelledDistance = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues &val) const throw() {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.sampleSeconds += sampleSeconds;
    v.meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(meanNTemp)/10.);
    v.travelledDistance += travelledDistance;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::update() throw() {
    meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(currentTimeN)/10.);
    currentTimeN = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::notifyMoveInternal(SUMOVehicle& veh, SUMOReal timeOnLane, SUMOReal speed) throw() {
    const SUMOReal sn = HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(),
                        (double) speed, veh.getPreDawdleAcceleration());
    currentTimeN += (SUMOReal) pow(10., (sn/10.));
    sampleSeconds += timeOnLane;
    travelledDistance += speed * timeOnLane;
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification /*reason*/) throw() {
    return vehicleApplies(veh);
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::write(OutputDevice &dev, const SUMOTime period,
        const SUMOReal /*numLanes*/, const int /*numVehicles*/) const throw(IOError) {
    dev << "\" noise=\"" << (meanNTemp!=0 ? (SUMOReal)(10. * log10(meanNTemp*TS/STEPS2TIME(period))) : (SUMOReal) 0.);
    if (sampleSeconds > myParent->myMinSamples) {
        SUMOReal traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            traveltime = MIN2(traveltime, myLaneLength * sampleSeconds / travelledDistance);
        }
        dev << "\" traveltime=\"" << traveltime;
    }
    dev << "\"/>\n";
}



// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSMeanData_Harmonoise(const std::string &id,
        const SUMOTime dumpBegin, const SUMOTime dumpEnd,
        const bool useLanes, const bool withEmpty, const bool withInternal,
        const bool trackVehicles,
        const SUMOReal maxTravelTime, const SUMOReal minSamples,
        const std::set<std::string> vTypes) throw()
        : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, withInternal, trackVehicles, maxTravelTime, minSamples, vTypes) {
}


MSMeanData_Harmonoise::~MSMeanData_Harmonoise() throw() {}


MSMeanData::MeanDataValues*
MSMeanData_Harmonoise::createValues(MSLane * const lane, const SUMOReal length, const bool doAdd) const throw(IOError) {
    return new MSLaneMeanDataValues(lane, length, doAdd, &myVehicleTypes, this);
}


void
MSMeanData_Harmonoise::update(const SUMOTime step) throw() {
    MSMeanData::update(step);
    for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        const std::vector<MeanDataValues*> &lm = *i;
        for (std::vector<MeanDataValues*>::const_iterator j=lm.begin(); j!=lm.end(); ++j) {
            (*j)->update();
        }
    }
}


/****************************************************************************/

