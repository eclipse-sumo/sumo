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
MSMeanData_Harmonoise::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane) throw()
        : MSMoveReminder(lane), sampleSeconds(0),
        currentTimeN(0), meanNTemp(0), myLastTimeStep((SUMOTime) -1) {}


MSMeanData_Harmonoise::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::reset() throw() {
    sampleSeconds = 0;
    currentTimeN = 0;
    meanNTemp = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::add(SUMOReal sn, SUMOReal fraction) throw() {
    SUMOTime step = MSNet::getInstance()->getCurrentTimeStep();
    currentTimeN += (SUMOReal) pow(10., (sn/10.));
    sampleSeconds += fraction;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::flushStep() throw() {
    meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(currentTimeN)/10.);
    currentTimeN = 0;
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    bool ret = true;
    SUMOReal fraction = 1.;
    if (oldPos<0&&newSpeed!=0) {
        fraction = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->length()&&newSpeed!=0) {
        fraction -= (oldPos+SPEED2DIST(newSpeed) - getLane()->length()) / newSpeed;
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
    add(sn, fraction);
    return ret;
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw() {
    SUMOReal fraction = 1.;
    SUMOReal l = veh.getVehicleType().getLength();
    if (veh.getPositionOnLane()+l>getLane()->length()) {
        fraction = l - (getLane()->length()-veh.getPositionOnLane());
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
    add(sn, fraction);
    return true;
}



// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSMeanData_Harmonoise(const std::string &id,
        MSEdgeControl &ec,
        SUMOTime dumpBegin,
        SUMOTime dumpEnd,
        bool useLanes,
        bool withEmptyEdges, bool withEmptyLanes) throw()
        : myID(id),
        myAmEdgeBased(!useLanes), myDumpBegin(dumpBegin), myDumpEnd(dumpEnd),
        myDumpEmptyEdges(withEmptyEdges), myDumpEmptyLanes(withEmptyLanes) {
    MSNet::getInstance()->getDetectorControl().add(this);
    const std::vector<MSEdge*> &edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        std::vector<MSLaneMeanDataValues*> v;
        const MSEdge::LaneCont * const lanes = (*e)->getLanes();
        MSEdge::LaneCont::const_iterator lane;
        for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
            v.push_back(new MSLaneMeanDataValues(*lane));
        }
        myMeasures.push_back(v);
        myEdges.push_back(*e);
    }
}


MSMeanData_Harmonoise::~MSMeanData_Harmonoise() throw() {}


void
MSMeanData_Harmonoise::resetOnly(SUMOTime stopTime) throw() {
    for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        for (std::vector<MSLaneMeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            (*j)->reset();
        }
    }
}


void
MSMeanData_Harmonoise::writeEdge(OutputDevice &dev,
                                 const std::vector<MSLaneMeanDataValues*> &edgeValues,
                                 MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    std::vector<MSLaneMeanDataValues*>::const_iterator lane;
    if (!myAmEdgeBased) {
        bool writeCheck = myDumpEmptyEdges;
        if (!writeCheck) {
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                if ((*lane)->sampleSeconds>0) {
                    writeCheck = true;
                    break;
                }
            }
        }
        if (writeCheck) {
            dev<<"      <edge id=\""<<edge->getID()<<"\">\n";
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                writeLane(dev, *(*lane), startTime, stopTime);
            }
            dev<<"      </edge>\n";
        }
    } else {
        SUMOReal nS = 0;
        SUMOReal nVehS = 0;
        for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
            MSLaneMeanDataValues& meanData = *(*lane);
            // calculate mean data
            nS += meanData.meanNTemp;
            nVehS += meanData.sampleSeconds;
            meanData.reset();
        }
        SUMOReal v = nS!=0. ? (SUMOReal)(10. * log10(nS/(SUMOReal)(stopTime-startTime))) : (SUMOReal) 0.;
        if (myDumpEmptyEdges||nVehS>0) {
            dev<<"      <edge id=\""<<edge->getID()<<
            "\" sampledSeconds=\""<< nVehS <<
            "\" noise=\""<< v <<
            "\"/>\n";
        }
    }
}


void
MSMeanData_Harmonoise::writeLane(OutputDevice &dev,
                                 MSLaneMeanDataValues &laneValues,
                                 SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    if (myDumpEmptyLanes||laneValues.sampleSeconds>0) {
        // calculate mean data
        SUMOReal v = laneValues.meanNTemp!=0 ? (SUMOReal)(10. * log10(laneValues.meanNTemp/(SUMOReal)(stopTime-startTime))) : (SUMOReal) 0.;
        dev<<"         <lane id=\""<<laneValues.getLane()->getID()<<
        "\" sampledSeconds=\""<< laneValues.sampleSeconds <<
        "\" noise=\""<< v <<
        "\"/>\n";
    }
    laneValues.reset();
}


void
MSMeanData_Harmonoise::writeXMLOutput(OutputDevice &dev,
                                      SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    // check whether this dump shall be written for the current time
    if (myDumpBegin < stopTime && myDumpEnd-DELTA_T >= startTime) {
        dev<<"   <interval begin=\""<<startTime<<"\" end=\""<<
        stopTime<<"\" "<<"id=\""<<myID<<"\">\n";
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            writeEdge(dev, (*i), *edge, startTime, stopTime);
        }
        dev<<"   </interval>\n";
    } else {
        resetOnly(stopTime);
    }
}


void
MSMeanData_Harmonoise::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("netstats");
}


void
MSMeanData_Harmonoise::update() throw() {
    for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        const std::vector<MSLaneMeanDataValues*> &lm = *i;
        for (std::vector<MSLaneMeanDataValues*>::const_iterator j=lm.begin(); j!=lm.end(); ++j) {
            (*j)->flushStep();
        }
    }
}


/****************************************************************************/

