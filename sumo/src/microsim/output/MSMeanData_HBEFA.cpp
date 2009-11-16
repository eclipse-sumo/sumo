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
        : MSMoveReminder(lane), sampleSeconds(0), travelledDistance(0), CO2(0), CO(0), HC(0),
        NOx(0), PMx(0), fuel(0), myVehicleTypes(vTypes) {}


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
MSMeanData_HBEFA::MSLaneMeanDataValues::add(MSMeanData_HBEFA::MSLaneMeanDataValues &val) throw() {
    sampleSeconds += val.sampleSeconds;
    travelledDistance += val.travelledDistance;
    CO2 += val.CO2;
    CO += val.CO;
    HC += val.HC;
    NOx += val.NOx;
    PMx += val.PMx;
    fuel += val.fuel;
}


bool
MSMeanData_HBEFA::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (myVehicleTypes != 0 && !myVehicleTypes->empty() &&
        myVehicleTypes->find(veh.getVehicleType().getID()) == myVehicleTypes->end()) {
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
MSMeanData_HBEFA::MSMeanData_HBEFA(const std::string &id, const MSEdgeControl &ec,
                                   const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                                   const bool useLanes, const bool withEmpty, const bool withInternal,
                                   const SUMOReal maxTravelTime, const SUMOReal minSamples,
                                   const std::set<std::string> vTypes) throw()
        : myID(id),
        myAmEdgeBased(!useLanes), myDumpBegin(dumpBegin), myDumpEnd(dumpEnd),
        myDumpEmpty(withEmpty), myMaxTravelTime(maxTravelTime), myMinSamples(minSamples), myVehicleTypes(vTypes) {
    const std::vector<MSEdge*> &edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        std::vector<MSLaneMeanDataValues*> v;
        const std::vector<MSLane*> &lanes = (*e)->getLanes();
        for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
            v.push_back(new MSLaneMeanDataValues(*lane, &myVehicleTypes));
        }
        myMeasures.push_back(v);
        myEdges.push_back(*e);
    }
}


MSMeanData_HBEFA::~MSMeanData_HBEFA() throw() {}


void
MSMeanData_HBEFA::resetOnly(SUMOTime stopTime) throw() {
    for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        for (std::vector<MSLaneMeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            (*j)->reset();
        }
    }
}


void
MSMeanData_HBEFA::writeEdge(OutputDevice &dev,
                            const std::vector<MSLaneMeanDataValues*> &edgeValues,
                            MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    std::vector<MSLaneMeanDataValues*>::const_iterator lane;
    if (!myAmEdgeBased) {
        bool writeCheck = myDumpEmpty;
        if (!writeCheck) {
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                if ((*lane)->sampleSeconds>0) {
                    writeCheck = true;
                    break;
                }
            }
        }
        if (writeCheck) {
            dev.openTag("edge")<<" id=\""<<edge->getID()<<"\">\n";
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                MSLaneMeanDataValues& meanData = **lane;
                writeValues(dev, "<lane id=\""+meanData.getLane()->getID(),
                            meanData, (SUMOReal)(stopTime - startTime), 1.f, meanData.getLane()->getLength());
                meanData.reset();
            }
            dev.closeTag();
        }
    } else {
        MSLaneMeanDataValues sumData(0);
        for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
            MSLaneMeanDataValues& meanData = **lane;
            sumData.add(meanData);
            meanData.reset();
        }
        writeValues(dev, "<edge id=\""+edge->getID(),
                    sumData, (SUMOReal)(stopTime - startTime),
                    (SUMOReal)edge->getLanes().size(), edge->getLanes()[0]->getLength());
    }
}


void
MSMeanData_HBEFA::writeValues(OutputDevice &dev, const std::string prefix,
                            const MSLaneMeanDataValues &values, const SUMOReal period,
                            const SUMOReal numLanes, const SUMOReal length) throw(IOError) {
    if (myDumpEmpty||values.sampleSeconds>0) {
        dev<<std::resetiosflags(std::ios::floatfield);
        const SUMOReal normFactor = SUMOReal(3600. * 1000. / period / length);
        dev.indent() << prefix << "\" sampledSeconds=\"" << values.sampleSeconds <<
        "\" CO_abs=\""<<SUMOReal(values.CO*1000.) <<
        "\" CO2_abs=\""<<SUMOReal(values.CO2*1000.) <<
        "\" HC_abs=\""<<SUMOReal(values.HC*1000.) <<
        "\" PMx_abs=\""<<SUMOReal(values.PMx*1000.) <<
        "\" NOx_abs=\""<<SUMOReal(values.NOx*1000.) <<
        "\" fuel_abs=\""<<SUMOReal(values.fuel*1000.) <<
        "\"\n            CO_normed=\""<<normFactor * values.CO <<
        "\" CO2_normed=\""<<normFactor * values.CO2<<
        "\" HC_normed=\""<<normFactor * values.HC <<
        "\" PMx_normed=\""<<normFactor * values.PMx <<
        "\" NOx_normed=\""<<normFactor * values.NOx <<
        "\" fuel_normed=\""<<normFactor * values.fuel;
        if (values.sampleSeconds > myMinSamples) {
            SUMOReal vehFactor = myMaxTravelTime / values.sampleSeconds;
            if (values.travelledDistance > 0.f) {
                vehFactor = MIN2(vehFactor, length / values.travelledDistance);
            }
            dev<<"\"\n            CO_perVeh=\""<<values.CO*vehFactor<<
            "\" CO2_perVeh=\""<<values.CO2*vehFactor<<
            "\" HC_perVeh=\""<<values.HC*vehFactor<<
            "\" PMx_perVeh=\""<<values.PMx*vehFactor<<
            "\" NOx_perVeh=\""<<values.NOx*vehFactor<<
            "\" fuel_perVeh=\""<<values.fuel*vehFactor;
        }
        dev<<"\"/>\n";
        dev<<std::setiosflags(std::ios::fixed); // use decimal format
    }
}


void
MSMeanData_HBEFA::writeXMLOutput(OutputDevice &dev,
                                 SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    // check whether this dump shall be written for the current time
    if (myDumpBegin < stopTime && myDumpEnd-DELTA_T >= startTime) {
        dev.openTag("interval")<<" begin=\""<<startTime<<"\" end=\""<<
        stopTime<<"\" "<<"id=\""<<myID<<"\">\n";
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            writeEdge(dev, (*i), *edge, startTime, stopTime);
        }
        dev.closeTag();
    } else {
        resetOnly(stopTime);
    }
}


void
MSMeanData_HBEFA::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("netstats");
}


/****************************************************************************/

