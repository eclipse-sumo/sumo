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
MSMeanData_Harmonoise::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane, const bool doAdd,
        const std::set<std::string>* const vTypes, const MSMeanData_Harmonoise *parent) throw()
        : MSMeanData::MeanDataValues(lane, doAdd, vTypes),
        currentTimeN(0), meanNTemp(0), myParent(parent) {}


MSMeanData_Harmonoise::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::reset() throw() {
    sampleSeconds = 0;
    currentTimeN = 0;
    meanNTemp = 0;
    travelledDistance = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues &val) const throw() {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.sampleSeconds += sampleSeconds;
    v.meanNTemp += meanNTemp;
    v.travelledDistance += travelledDistance;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::update() throw() {
    meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(currentTimeN)/10.);
    currentTimeN = 0;
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (!vehicleApplies(veh)) {
        return false;
    }
    bool ret = true;
    SUMOReal timeOnLane = (SUMOReal) DELTA_T / 1000.;
    if (oldPos<0&&newSpeed!=0) {
        timeOnLane = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->getLength()&&newSpeed!=0) {
        timeOnLane -= (oldPos+SPEED2DIST(newSpeed) - getLane()->getLength()) / newSpeed;
        ret = false;
    }
    if (timeOnLane<0) {
        MsgHandler::getErrorInstance()->inform("Negative vehicle step fraction on lane '" + getLane()->getID() + "'.");
        return false;
    }
    if (timeOnLane==0) {
        return false;
    }
    SUMOReal a = veh.getPreDawdleAcceleration();
    SUMOReal sn = HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a);
    currentTimeN += (SUMOReal) pow(10., (sn/10.));
    sampleSeconds += timeOnLane;
    travelledDistance += newSpeed * timeOnLane;
    return ret;
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw() {
    return vehicleApplies(veh);
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::write(OutputDevice &dev, const SUMOReal period,
        const SUMOReal numLanes, const SUMOReal length, const int numVehicles) const throw(IOError) {
    dev << "\" noise=\"" << (meanNTemp!=0 ? (SUMOReal)(10. * log10(meanNTemp/period)) : (SUMOReal) 0.);
    if (sampleSeconds > myParent->myMinSamples) {
        SUMOReal traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            traveltime = MIN2(traveltime, length * sampleSeconds / travelledDistance);
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
        const bool useLanes, const bool withEmpty,
        const bool trackVehicles,
        const SUMOReal maxTravelTime, const SUMOReal minSamples,
        const std::set<std::string> vTypes) throw()
        : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, trackVehicles, maxTravelTime, minSamples, vTypes) {
    MSNet::getInstance()->getDetectorControl().add(this);
}


MSMeanData_Harmonoise::~MSMeanData_Harmonoise() throw() {}


MSMeanData::MeanDataValues*
MSMeanData_Harmonoise::createValues(MSLane * const lane, const bool doAdd) const throw(IOError) {
    return new MSLaneMeanDataValues(lane, doAdd, &myVehicleTypes, this);
}


/****************************************************************************/

