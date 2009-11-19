/****************************************************************************/
/// @file    MSMeanData_HBEFA.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Emission data collector for edges/lanes
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
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_HBEFA.h"
#include <utils/common/HelpersHBEFA.h>
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
// MSMeanData_HBEFA::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_HBEFA::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane,
                                                             const std::set<std::string>* const vTypes) throw()
        : MSMeanData::MeanDataValues(lane, vTypes), CO2(0), CO(0), HC(0), NOx(0), PMx(0), fuel(0) {}


MSMeanData_HBEFA::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_HBEFA::MSLaneMeanDataValues::reset() throw() {
    sampleSeconds = 0.;
    travelledDistance = 0.;
    CO2 = 0;
    CO = 0;
    HC = 0;
    NOx = 0;
    PMx = 0;
    fuel = 0;
}


void
MSMeanData_HBEFA::MSLaneMeanDataValues::add(MSMeanData::MeanDataValues &val) throw() {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    sampleSeconds += v.sampleSeconds;
    travelledDistance += v.travelledDistance;
    CO2 += v.CO2;
    CO += v.CO;
    HC += v.HC;
    NOx += v.NOx;
    PMx += v.PMx;
    fuel += v.fuel;
}


bool
MSMeanData_HBEFA::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (!MSMeanData::MeanDataValues::isStillActive(veh, oldPos, newPos, newSpeed)) {
        return false;
    }
    bool ret = true;
    SUMOReal timeOnLane = DELTA_T;
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
    sampleSeconds += timeOnLane;
    travelledDistance += newSpeed * timeOnLane;
    SUMOReal a = veh.getPreDawdleAcceleration();
    CO += (timeOnLane * HelpersHBEFA::computeCO(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    CO2 += (timeOnLane * HelpersHBEFA::computeCO2(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    HC += (timeOnLane * HelpersHBEFA::computeHC(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    NOx += (timeOnLane * HelpersHBEFA::computeNOx(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    PMx += (timeOnLane * HelpersHBEFA::computePMx(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    fuel += (timeOnLane * HelpersHBEFA::computeFuel(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    return ret;
}



// ---------------------------------------------------------------------------
// MSMeanData_HBEFA - methods
// ---------------------------------------------------------------------------
MSMeanData_HBEFA::MSMeanData_HBEFA(const std::string &id,
                                   const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                                   const bool useLanes, const bool withEmpty,
                                   const SUMOReal maxTravelTime, const SUMOReal minSamples,
                                   const std::set<std::string> vTypes) throw()
        : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, maxTravelTime, minSamples, vTypes) {
}


MSMeanData_HBEFA::~MSMeanData_HBEFA() throw() {}


MSMeanData::MeanDataValues*
MSMeanData_HBEFA::createValues(MSLane * const lane) throw(IOError) {
    return new MSLaneMeanDataValues(lane, &myVehicleTypes);
}


void
MSMeanData_HBEFA::writeValues(OutputDevice &dev, const std::string prefix,
                              const MSMeanData::MeanDataValues &values, const SUMOReal period,
                              const SUMOReal numLanes, const SUMOReal length) throw(IOError) {
    if (myDumpEmpty||!values.isEmpty()) {
        MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) values;
        dev<<std::resetiosflags(std::ios::floatfield);
        const SUMOReal normFactor = SUMOReal(3600. * 1000. / period / length);
        dev.indent() << prefix << "\" sampledSeconds=\"" << v.sampleSeconds <<
        "\" CO_abs=\""<<SUMOReal(v.CO*1000.) <<
        "\" CO2_abs=\""<<SUMOReal(v.CO2*1000.) <<
        "\" HC_abs=\""<<SUMOReal(v.HC*1000.) <<
        "\" PMx_abs=\""<<SUMOReal(v.PMx*1000.) <<
        "\" NOx_abs=\""<<SUMOReal(v.NOx*1000.) <<
        "\" fuel_abs=\""<<SUMOReal(v.fuel*1000.) <<
        "\"\n            CO_normed=\""<<normFactor * v.CO <<
        "\" CO2_normed=\""<<normFactor * v.CO2<<
        "\" HC_normed=\""<<normFactor * v.HC <<
        "\" PMx_normed=\""<<normFactor * v.PMx <<
        "\" NOx_normed=\""<<normFactor * v.NOx <<
        "\" fuel_normed=\""<<normFactor * v.fuel;
        if (v.sampleSeconds > myMinSamples) {
            SUMOReal vehFactor = myMaxTravelTime / v.sampleSeconds;
            if (v.travelledDistance > 0.f) {
                vehFactor = MIN2(vehFactor, length / v.travelledDistance);
            }
            dev<<"\"\n            CO_perVeh=\""<<v.CO*vehFactor<<
            "\" CO2_perVeh=\""<<v.CO2*vehFactor<<
            "\" HC_perVeh=\""<<v.HC*vehFactor<<
            "\" PMx_perVeh=\""<<v.PMx*vehFactor<<
            "\" NOx_perVeh=\""<<v.NOx*vehFactor<<
            "\" fuel_perVeh=\""<<v.fuel*vehFactor;
        }
        dev<<"\"/>\n";
        dev<<std::setiosflags(std::ios::fixed); // use decimal format
    }
}


/****************************************************************************/

