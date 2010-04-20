/****************************************************************************/
/// @file    MSMeanData_Net.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
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
MSMeanData_Net::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane, const bool doAdd,
        const std::set<std::string>* const vTypes,
        const MSMeanData_Net *parent) throw()
        : MSMeanData::MeanDataValues(lane, doAdd, vTypes), myParent(parent),
        nVehDeparted(0), nVehArrived(0), nVehEntered(0), nVehLeft(0),
        nVehLaneChangeFrom(0), nVehLaneChangeTo(0), waitSeconds(0), vehLengthSum(0) {}


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
MSMeanData_Net::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues &val) const throw() {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.nVehDeparted += nVehDeparted;
    v.nVehArrived += nVehArrived;
    v.nVehEntered += nVehEntered;
    v.nVehLeft += nVehLeft;
    v.nVehLaneChangeFrom += nVehLaneChangeFrom;
    v.nVehLaneChangeTo += nVehLaneChangeTo;
    v.sampleSeconds += sampleSeconds;
    v.travelledDistance += travelledDistance;
    v.waitSeconds += waitSeconds;
    v.vehLengthSum += vehLengthSum;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (!vehicleApplies(veh)) {
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
    vehLengthSum += veh.getVehicleType().getLength() * timeOnLane;
    if (newSpeed<myParent->myHaltSpeed) {
        waitSeconds += timeOnLane;
    }
    return ret;
}


void
MSMeanData_Net::MSLaneMeanDataValues::notifyLeave(MSVehicle& veh, bool isArrival, bool isLaneChange) throw() {
    if (vehicleApplies(veh)) {
        if (isArrival) {
            ++nVehArrived;
        } else if (isLaneChange) {
            ++nVehLaneChangeFrom;
        } else {
            ++nVehLeft;
        }
    }
}


bool
MSMeanData_Net::MSLaneMeanDataValues::notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw() {
    if (vehicleApplies(veh)) {
        if (isEmit) {
            ++nVehDeparted;
        } else if (isLaneChange) {
            ++nVehLaneChangeTo;
        } else {
            ++nVehEntered;
        }
        return true;
    }
    return false;
}


bool
MSMeanData_Net::MSLaneMeanDataValues::isEmpty() const throw() {
    return sampleSeconds == 0 && nVehDeparted == 0 && nVehArrived == 0 && nVehEntered == 0 && nVehLeft == 0 && nVehLaneChangeFrom == 0 && nVehLaneChangeTo == 0;
}


void
MSMeanData_Net::MSLaneMeanDataValues::write(OutputDevice &dev, const SUMOReal period,
        const SUMOReal numLanes, const SUMOReal length, const int numVehicles) const throw(IOError) {
    if (myParent == 0) {
        if (sampleSeconds > 0) {
            dev << "\" density=\"" << sampleSeconds / period *(SUMOReal) 1000 / length <<
            "\" occupancy=\"" << vehLengthSum / period / length / numLanes *(SUMOReal) 100 <<
            "\" waitingTime=\"" << waitSeconds <<
            "\" speed=\"" << travelledDistance / sampleSeconds;
        }
        dev<<"\" departed=\""<<nVehDeparted<<
        "\" arrived=\""<<nVehArrived<<
        "\" entered=\""<<nVehEntered<<
        "\" left=\""<<nVehLeft<<
        "\"/>\n";
        return;
    }
    if (sampleSeconds > myParent->myMinSamples) {
        SUMOReal traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            traveltime = MIN2(traveltime, length * sampleSeconds / travelledDistance);
        }
        if (numVehicles > 0) {
            dev << "\" traveltime=\"" << sampleSeconds / numVehicles <<
            "\" waitingTime=\"" << waitSeconds <<
            "\" speed=\"" << travelledDistance / sampleSeconds;
        } else {
            dev << "\" traveltime=\"" << traveltime <<
            "\" density=\"" << sampleSeconds / period *(SUMOReal) 1000 / length <<
            "\" occupancy=\"" << vehLengthSum / period / length / numLanes *(SUMOReal) 100 <<
            "\" waitingTime=\"" << waitSeconds <<
            "\" speed=\"" << travelledDistance / sampleSeconds;
        }
    }
    dev<<"\" departed=\""<<nVehDeparted<<
    "\" arrived=\""<<nVehArrived<<
    "\" entered=\""<<nVehEntered<<
    "\" left=\""<<nVehLeft<<
    "\" laneChangedFrom=\""<<nVehLaneChangeFrom<<
    "\" laneChangedTo=\""<<nVehLaneChangeTo<<
    "\"/>\n";
}


#ifdef HAVE_MESOSIM
void
MSMeanData_Net::MSLaneMeanDataValues::addData(const MEVehicle& veh, const SUMOReal timeOnLane,
        const SUMOReal dist) throw() {
    if (vehicleApplies(veh)) {
        sampleSeconds += timeOnLane;
        travelledDistance += dist;
        vehLengthSum += veh.getVehicleType().getLength() * timeOnLane;
        if (myParent!=0&&dist/timeOnLane<myParent->myHaltSpeed) {
            waitSeconds += timeOnLane;
        }
    }
}


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
                               const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                               const bool useLanes, const bool withEmpty,
                               const bool trackVehicles,
                               const SUMOReal maxTravelTime, const SUMOReal minSamples,
                               const SUMOReal haltSpeed, const std::set<std::string> vTypes) throw()
        : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, trackVehicles, maxTravelTime, minSamples, vTypes),
        myHaltSpeed(haltSpeed) {
}


MSMeanData_Net::~MSMeanData_Net() throw() {}


MSMeanData::MeanDataValues*
MSMeanData_Net::createValues(MSLane * const lane, const bool doAdd) const throw(IOError) {
    return new MSLaneMeanDataValues(lane, doAdd, &myVehicleTypes, this);
}


void
MSMeanData_Net::writeEdge(OutputDevice &dev,
                          const std::vector<MSMeanData::MeanDataValues*> &edgeValues,
                          MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSLaneMeanDataValues* sumData = (MSLaneMeanDataValues*)createValues(0);
        unsigned entered;
        bool isFirst = true;
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(*edge);
        std::vector<MeanDataValues*>::const_iterator data;
        for (data = edgeValues.begin(); data != edgeValues.end(); ++data) {
            MSLaneMeanDataValues& meanData = (MSLaneMeanDataValues&)**data;
            s->prepareMeanDataForWriting(meanData, (SUMOReal) stopTime);
            meanData.addTo(*sumData);
            if (isFirst) {
                entered = meanData.nVehEntered;
                isFirst = false;
            }
            sumData->nVehLeft = meanData.nVehLeft;
            meanData.reset();
            s = s->getNextSegment();
        }
        sumData->nVehEntered = entered;
        if (writePrefix(dev, *sumData, "<edge id=\""+edge->getID())) {
            sumData->write(dev, (SUMOReal)(stopTime - startTime),
                           (SUMOReal)edge->getLanes().size(), edge->getLanes()[0]->getLength());
        }
        delete sumData;
        return;
    }
#endif
    MSMeanData::writeEdge(dev, edgeValues, edge, startTime, stopTime);
}


/****************************************************************************/

