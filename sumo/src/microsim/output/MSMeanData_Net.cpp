/****************************************************************************/
/// @file    MSMeanData_Net.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
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
MSMeanData_Net::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane) throw()
        : MSMoveReminder(lane), sampleSeconds(0), nVehLeftLane(0), nVehEnteredLane(0),
        travelledDistance(0), haltSum(0), vehLengthSum(0),
        nVehEmitted(0) {}


MSMeanData_Net::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_Net::MSLaneMeanDataValues::reset() throw() {
    nVehEmitted = 0;
    nVehEnteredLane = 0;
    nVehLeftLane = 0;
    sampleSeconds = 0.;
    travelledDistance = 0;
    haltSum = 0;
    vehLengthSum = 0;
}


void
MSMeanData_Net::MSLaneMeanDataValues::add(MSMeanData_Net::MSLaneMeanDataValues &val) throw() {
    nVehEmitted += val.nVehEmitted;
    nVehEnteredLane += val.nVehEnteredLane;
    nVehLeftLane += val.nVehLeftLane;
    sampleSeconds += val.sampleSeconds;
    travelledDistance += val.travelledDistance;
    haltSum += val.haltSum;
    vehLengthSum += val.vehLengthSum;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    bool ret = true;
    SUMOReal timeOnLane = DELTA_T;
    if (oldPos<0&&newSpeed!=0) {
        timeOnLane = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
        ++nVehEnteredLane;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->length()&&newSpeed!=0) {
        timeOnLane -= (oldPos+SPEED2DIST(newSpeed) - getLane()->length()) / newSpeed;
        ++nVehLeftLane;
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
    vehLengthSum += veh.getVehicleType().getLength() * timeOnLane;
    if (newSpeed<0.1) { // !!! swell
        haltSum++;
    }
    return ret;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw() {
    ++nVehEmitted;
    SUMOReal l = veh.getVehicleType().getLength();
    SUMOReal fraction = 1.;
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
    sampleSeconds += fraction;
    travelledDistance += veh.getSpeed() * fraction;
    vehLengthSum += l * fraction;
    if (veh.getSpeed()<0.1) { // !!! swell
        haltSum++;
    }
    return true;
}



// ---------------------------------------------------------------------------
// MSMeanData_Net - methods
// ---------------------------------------------------------------------------
MSMeanData_Net::MSMeanData_Net(const std::string &id,
                               MSEdgeControl &ec,
                               SUMOTime dumpBegin,
                               SUMOTime dumpEnd,
                               bool useLanes,
                               bool withEmpty) throw()
        : myID(id),
        myAmEdgeBased(!useLanes), myDumpBegin(dumpBegin), myDumpEnd(dumpEnd),
        myDumpEmpty(withEmpty) {
    const std::vector<MSEdge*> &edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        std::vector<MSLaneMeanDataValues*> v;
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(*e);
            while (s!=0) {
                v.push_back(s->addDetector(this));
                s = s->getNextSegment();
            }
        } else {
#endif
            const MSEdge::LaneCont * const lanes = (*e)->getLanes();
            MSEdge::LaneCont::const_iterator lane;
            for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
                v.push_back(new MSLaneMeanDataValues(*lane));
            }
#ifdef HAVE_MESOSIM
        }
#endif
        myMeasures.push_back(v);
        myEdges.push_back(*e);
    }
}


MSMeanData_Net::~MSMeanData_Net() throw() {}


void
MSMeanData_Net::resetOnly(SUMOTime stopTime) throw() {
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(*edge);
            for (std::vector<MSLaneMeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
                s->prepareMeanDataForWriting(*(*j), (SUMOReal) stopTime);
                (*j)->reset();
                s = s->getNextSegment();
            }
        }
    } else {
#endif
        for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
            for (std::vector<MSLaneMeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
                (*j)->reset();
            }
        }
#ifdef HAVE_MESOSIM
    }
#endif
}


void
MSMeanData_Net::writeEdge(OutputDevice &dev,
                          const std::vector<MSLaneMeanDataValues*> &edgeValues,
                          MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSLaneMeanDataValues sumData(0);
        unsigned entered;
        SUMOReal absLen = 0;
        bool isFirst = true;
        for (MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(edge); s!=0; s = s->getNextSegment()) {
            MSLaneMeanDataValues& meanData = s->getDetectorData(this);
            s->prepareMeanDataForWriting(meanData, (SUMOReal) stopTime);
            sumData.add(meanData);
            if (isFirst) {
                entered = meanData.nVehEnteredLane;
                isFirst = false;
            }
            sumData.nVehLeftLane = meanData.nVehLeftLane;
            absLen += s->getLength();
            meanData.reset();
        }
        sumData.nVehEnteredLane = entered;
        writeValues(dev, "<edge id=\""+edge->getID(),
                    sumData, (SUMOReal)(stopTime - startTime),
                    absLen, (SUMOReal)edge->nLanes(),
                    MSGlobals::gMesoNet->getSegmentForEdge(edge)->getMaxSpeed());
        return;
    }
#endif
    std::vector<MSLaneMeanDataValues*>::const_iterator lane;
    if (!myAmEdgeBased) {
        bool writeCheck = myDumpEmpty;
        if (!writeCheck) {
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                if ((*lane)->sampleSeconds>0||(*lane)->nVehEmitted>0) {
                    writeCheck = true;
                    break;
                }
            }
        }
        if (writeCheck) {
            dev.openTag("edge")<<" id=\""<<edge->getID()<<"\">\n";
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                writeValues(dev, "<lane id=\""+(*lane)->getLane()->getID(),
                            *(*lane), (SUMOReal)(stopTime - startTime),
                            (*lane)->getLane()->length(), (SUMOReal)1, (*lane)->getLane()->maxSpeed());
                (*lane)->reset();
            }
            dev.closeTag();
        }
    } else {
        MSLaneMeanDataValues sumData(0);
        for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
            MSLaneMeanDataValues& meanData = *(*lane);
            sumData.add(meanData);
            meanData.reset();
        }
        writeValues(dev, "<edge id=\""+edge->getID(),
                    sumData, (SUMOReal)(stopTime - startTime),
                    edgeValues.front()->getLane()->length(), (SUMOReal)edge->nLanes(),
                    edgeValues.front()->getLane()->maxSpeed());
    }
}


void
MSMeanData_Net::writeValues(OutputDevice &dev, std::string prefix,
                            MSLaneMeanDataValues &values, SUMOReal period,
                            SUMOReal length, SUMOReal numLanes, SUMOReal maxSpeed) throw(IOError) {
    if (myDumpEmpty||values.sampleSeconds>0||values.nVehEmitted>0) {
        SUMOReal meanDensity = values.sampleSeconds / period * (SUMOReal) 1000 / length;
        SUMOReal meanOccupancy = values.vehLengthSum / period / length / numLanes * (SUMOReal) 100;
        SUMOReal meanSpeed = maxSpeed;
        if (values.sampleSeconds>0) {
            meanSpeed = values.travelledDistance / values.sampleSeconds;
        }
        SUMOReal traveltime = meanSpeed > 0 ? length / meanSpeed : (SUMOReal) 1000000;
        SUMOReal flow = values.travelledDistance / length * (SUMOReal) 3600 / period;
        dev.indent()<<prefix<<"\" traveltime=\""<<traveltime<<
        "\" sampledSeconds=\""<< values.sampleSeconds <<
        "\" density=\""<<meanDensity<<
        "\" occupancy=\""<<meanOccupancy<<
        "\" noStops=\""<<values.haltSum<<
        "\" speed=\""<<meanSpeed<<
        "\" entered=\""<<values.nVehEnteredLane<<
        "\" emitted=\""<<values.nVehEmitted<<
        "\" left=\""<<values.nVehLeftLane<<
        "\" flow=\""<<flow<<
        "\"/>\n";
    }
}


void
MSMeanData_Net::writeXMLOutput(OutputDevice &dev,
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
MSMeanData_Net::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("netstats");
}


/****************************************************************************/

