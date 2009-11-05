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
MSMeanData_Net::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane,
                                                           const SUMOReal maxHaltingSpeed) throw()
        : MSMoveReminder(lane), myMaxHaltingSpeed(maxHaltingSpeed),
        nVehDeparted(0), nVehArrived(0), nVehEntered(0), nVehLeft(0),
        nVehLaneChangeFrom(0), nVehLaneChangeTo(0), sampleSeconds(0),
        travelledDistance(0), waitSeconds(0), vehLengthSum(0) {}


MSMeanData_Net::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_Net::MSLaneMeanDataValues::reset() throw() {
    nVehDeparted = 0;
    nVehArrived = 0;
    nVehEntered = 0;
    nVehLeft = 0;
    nVehLaneChangeFrom = 0;
    nVehLaneChangeTo = 0;
    sampleSeconds = 0.;
    travelledDistance = 0;
    waitSeconds = 0;
    vehLengthSum = 0;
}


void
MSMeanData_Net::MSLaneMeanDataValues::add(MSMeanData_Net::MSLaneMeanDataValues &val) throw() {
    nVehDeparted += val.nVehDeparted;
    nVehArrived += val.nVehArrived;
    nVehEntered += val.nVehEntered;
    nVehLeft += val.nVehLeft;
    nVehLaneChangeFrom += val.nVehLaneChangeFrom;
    nVehLaneChangeTo += val.nVehLaneChangeTo;
    sampleSeconds += val.sampleSeconds;
    travelledDistance += val.travelledDistance;
    waitSeconds += val.waitSeconds;
    vehLengthSum += val.vehLengthSum;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    bool ret = true;
    SUMOReal timeOnLane = DELTA_T;
    if (oldPos<0&&newSpeed!=0) {
        timeOnLane = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
        ++nVehEntered;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->getLength()&&newSpeed!=0) {
        timeOnLane -= (oldPos+SPEED2DIST(newSpeed) - getLane()->getLength()) / newSpeed;
        ++nVehLeft;
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
    if (newSpeed<myMaxHaltingSpeed) {
        waitSeconds += timeOnLane;
    }
    return ret;
}


void
MSMeanData_Net::MSLaneMeanDataValues::notifyLeave(MSVehicle& veh, bool isArrival, bool isLaneChange) throw() {
    if (isArrival) {
        ++nVehArrived;
    } else if (isLaneChange) {
        ++nVehLaneChangeFrom;
    } else {
        ++nVehLeft;
    }
}


bool
MSMeanData_Net::MSLaneMeanDataValues::notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw() {
    if (isEmit) {
        ++nVehDeparted;
    } else if (isLaneChange) {
        ++nVehLaneChangeTo;
    } else {
        ++nVehEntered;
    }
    return true;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isEmpty() const throw() {
    return sampleSeconds == 0 && nVehDeparted == 0 && nVehArrived == 0 && nVehEntered == 0 && nVehLeft == 0 && nVehLaneChangeFrom == 0 && nVehLaneChangeTo == 0;
}


#ifdef HAVE_MESOSIM
void
MSMeanData_Net::MSLaneMeanDataValues::getLastReported(MEVehicle *v, SUMOReal &lastReportedTime, SUMOReal &lastReportedPos) throw() {
    std::map<MEVehicle*, std::pair<SUMOReal, SUMOReal> >::iterator j=myLastVehicleUpdateValues.find(v);
    if (j!=myLastVehicleUpdateValues.end()) {
        // the vehicle already has reported its values before; use these
        std::pair<SUMOReal, SUMOReal> &vals = (*j).second;
        lastReportedTime = vals.first;
        lastReportedPos = vals.second;
        myLastVehicleUpdateValues.erase(j);
    }
}


void
MSMeanData_Net::MSLaneMeanDataValues::setLastReported(MEVehicle *v, SUMOReal lastReportedTime, SUMOReal lastReportedPos) throw() {
    myLastVehicleUpdateValues[v] = std::pair<SUMOReal, SUMOReal>(lastReportedTime, lastReportedPos);
}
#endif

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
        myDumpEmpty(withEmpty), myMaxTravelTime(100000), myMinSamples(0) {
    const std::vector<MSEdge*> &edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        std::vector<MSLaneMeanDataValues*> v;
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(**e);
            while (s!=0) {
                v.push_back(new MSLaneMeanDataValues(0, POSITION_EPS));
                s->addDetector(v.back());
                s = s->getNextSegment();
            }
        } else {
#endif
            const std::vector<MSLane*> &lanes = (*e)->getLanes();
            for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
                v.push_back(new MSLaneMeanDataValues(*lane, POSITION_EPS));
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
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(**edge);
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
    std::vector<MSLaneMeanDataValues*>::const_iterator data;
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSLaneMeanDataValues sumData(0);
        unsigned entered;
        bool isFirst = true;
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(*edge);
        for (data = edgeValues.begin(); data != edgeValues.end(); ++data) {
            MSLaneMeanDataValues& meanData = **data;
            s->prepareMeanDataForWriting(meanData, (SUMOReal) stopTime);
            sumData.add(meanData);
            if (isFirst) {
                entered = meanData.nVehEntered;
                isFirst = false;
            }
            sumData.nVehLeft = meanData.nVehLeft;
            meanData.reset();
            s = s->getNextSegment();
        }
        sumData.nVehEntered = entered;
        writeValues(dev, "<edge id=\""+edge->getID(),
                    sumData, (SUMOReal)(stopTime - startTime),
                    (SUMOReal)edge->getLanes().size(), edge->getLanes()[0]->getLength());
        return;
    }
#endif
    if (!myAmEdgeBased) {
        bool writeCheck = myDumpEmpty;
        if (!writeCheck) {
            for (data = edgeValues.begin(); data != edgeValues.end(); ++data) {
                if (!(*data)->isEmpty()) {
                    writeCheck = true;
                    break;
                }
            }
        }
        if (writeCheck) {
            dev.openTag("edge")<<" id=\""<<edge->getID()<<"\">\n";
            for (data = edgeValues.begin(); data != edgeValues.end(); ++data) {
                MSLaneMeanDataValues& meanData = **data;
                writeValues(dev, "<lane id=\""+meanData.getLane()->getID(),
                            meanData, (SUMOReal)(stopTime - startTime), 1.f, meanData.getLane()->getLength());
                meanData.reset();
            }
            dev.closeTag();
        }
    } else {
        MSLaneMeanDataValues sumData(0);
        for (data = edgeValues.begin(); data != edgeValues.end(); ++data) {
            MSLaneMeanDataValues& meanData = **data;
            sumData.add(meanData);
            meanData.reset();
        }
        writeValues(dev, "<edge id=\""+edge->getID(),
                    sumData, (SUMOReal)(stopTime - startTime),
                    (SUMOReal)edge->getLanes().size(), edge->getLanes()[0]->getLength());
    }
}


void
MSMeanData_Net::writeValues(OutputDevice &dev, const std::string prefix,
                            const MSLaneMeanDataValues &values, const SUMOReal period,
                            const SUMOReal numLanes, const SUMOReal length) throw(IOError) {
    if (myDumpEmpty||!values.isEmpty()) {
        dev.indent() << prefix << "\" sampledSeconds=\"" << values.sampleSeconds;
        if (values.sampleSeconds > myMinSamples) {
            SUMOReal traveltime = myMaxTravelTime;
            if (values.travelledDistance > 0) {
                traveltime = MIN2(traveltime, length * values.sampleSeconds / values.travelledDistance);
            }
            dev << "\" traveltime=\"" << traveltime <<
                   "\" density=\"" << values.sampleSeconds / period * (SUMOReal) 1000 / length <<
                   "\" occupancy=\"" << values.vehLengthSum / period / length / numLanes * (SUMOReal) 100 <<
                   "\" waitingTime=\"" << values.waitSeconds / values.sampleSeconds * period <<
                   "\" speed=\"" << values.travelledDistance / values.sampleSeconds;
        }
        dev<<"\" departed=\""<<values.nVehDeparted<<
        "\" arrived=\""<<values.nVehArrived<<
        "\" entered=\""<<values.nVehEntered<<
        "\" left=\""<<values.nVehLeft<<
        "\" laneChangedFrom=\""<<values.nVehLaneChangeFrom<<
        "\" laneChangedTo=\""<<values.nVehLaneChangeTo<<
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

