/****************************************************************************/
/// @file    MSMeanData_Harmonoise.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: MSMeanData_Harmonoise.cpp 6726 2009-02-05 08:33:51Z dkrajzew $
///
// Redirector for mean data output (net->edgecontrol)
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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Harmonoise.h"
#include <utils/common/HelpersHarmonoise.h>
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
// MSMeanData_Harmonoise::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane,
                                                                  const std::set<std::string>* const vTypes) throw()
        : MSMeanData::MeanDataValues(lane, vTypes), 
        currentTimeN(0), meanNTemp(0) {}


MSMeanData_Harmonoise::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::reset() throw() {
    sampleSeconds = 0;
    currentTimeN = 0;
    meanNTemp = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::add(MSMeanData::MeanDataValues &val) throw() {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    sampleSeconds += v.sampleSeconds;
    meanNTemp += v.meanNTemp;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::update() throw() {
    meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(currentTimeN)/10.);
    currentTimeN = 0;
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (!MSMeanData::MeanDataValues::isStillActive(veh, oldPos, newPos, newSpeed)) {
        return false;
    }
    bool ret = true;
    SUMOReal fraction = 1.;
    if (oldPos<0&&newSpeed!=0) {
        fraction = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->getLength()&&newSpeed!=0) {
        fraction -= (oldPos+SPEED2DIST(newSpeed) - getLane()->getLength()) / newSpeed;
        ret = false;
    }
    if (fraction<0) {
        MsgHandler::getErrorInstance()->inform("Negative vehicle step fraction on lane '" + getLane()->getID() + "'.");
        return false;
    }
    if (fraction==0) {
        return false;
    }
    SUMOReal a = veh.getPreDawdleAcceleration();
    SUMOReal sn = HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a);
    currentTimeN += (SUMOReal) pow(10., (sn/10.));
    sampleSeconds += fraction;
    return ret;
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw() {
    if (MSMeanData::MeanDataValues::notifyEnter(veh, isEmit, isLaneChange)) {
        SUMOReal fraction = 1.;
        SUMOReal l = veh.getVehicleType().getLength();
        if (veh.getPositionOnLane()+l>getLane()->getLength()) {
            fraction = l - (getLane()->getLength()-veh.getPositionOnLane());
        }
        if (fraction<0) {
            MsgHandler::getErrorInstance()->inform("Negative vehicle step fraction on lane '" + getLane()->getID() + "'.");
            return false;
        }
        if (fraction==0) {
            return false;
        }
        SUMOReal a = veh.getPreDawdleAcceleration();
        SUMOReal sn = (fraction * HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
        currentTimeN += (SUMOReal) pow(10., (sn/10.));
        sampleSeconds += fraction;
        return true;
    }
    return false;
}



// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSMeanData_Harmonoise(const std::string &id,
                                             const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                                             const bool useLanes, const bool withEmpty,
                                             const SUMOReal maxTravelTime, const SUMOReal minSamples,
                                             const std::set<std::string> vTypes) throw()
        : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, maxTravelTime, minSamples, vTypes) {
    MSNet::getInstance()->getDetectorControl().add(this);
}


MSMeanData_Harmonoise::~MSMeanData_Harmonoise() throw() {}


MSMeanData::MeanDataValues*
MSMeanData_Harmonoise::createValues(MSLane * const lane) throw(IOError) {
    return new MSLaneMeanDataValues(lane, &myVehicleTypes);
}


void
MSMeanData_Harmonoise::writeValues(OutputDevice &dev, const std::string prefix,
                                   const MSMeanData::MeanDataValues &values, const SUMOReal period,
                                   const SUMOReal numLanes, const SUMOReal length) throw(IOError) {
    if (myDumpEmpty||!values.isEmpty()) {
        MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) values;
        SUMOReal noise = v.meanNTemp!=0 ? (SUMOReal)(10. * log10(v.meanNTemp/period)) : (SUMOReal) 0.;
        dev.indent() << prefix << "\" sampledSeconds=\"" << v.sampleSeconds <<
        "\" noise=\"" << noise << "\"/>\n";
    }
}


/****************************************************************************/

