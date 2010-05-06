/****************************************************************************/
/// @file    MSMeanData.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Emission data collector for edges/lanes
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
#include "MSMeanData.h"
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
// MSMeanData::MeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData::MeanDataValues::MeanDataValues(MSLane * const lane, const bool doAdd, const std::set<std::string>* const vTypes) throw()
        : MSMoveReminder(lane, doAdd), sampleSeconds(0), travelledDistance(0), myVehicleTypes(vTypes) {}


MSMeanData::MeanDataValues::~MeanDataValues() throw() {
}


bool
MSMeanData::MeanDataValues::vehicleApplies(const SUMOVehicle& veh) const throw() {
    return myVehicleTypes == 0 || myVehicleTypes->empty() ||
           myVehicleTypes->find(veh.getVehicleType().getID()) != myVehicleTypes->end();
}


bool
MSMeanData::MeanDataValues::isEmpty() const throw() {
    return sampleSeconds == 0;
}


void
MSMeanData::MeanDataValues::update() throw() {
}


SUMOReal
MSMeanData::MeanDataValues::getSamples() const throw() {
    return sampleSeconds;
}


// ---------------------------------------------------------------------------
// MSMeanData::MeanDataValueTracker - methods
// ---------------------------------------------------------------------------
MSMeanData::MeanDataValueTracker::MeanDataValueTracker(MSLane * const lane,
        const std::set<std::string>* const vTypes,
        const MSMeanData* const parent) throw()
        : MSMeanData::MeanDataValues(lane, true, vTypes), myParent(parent) {
    myCurrentData.push_back(new TrackerEntry(parent->createValues(lane, false)));
}


MSMeanData::MeanDataValueTracker::~MeanDataValueTracker() throw() {
}


void
MSMeanData::MeanDataValueTracker::reset() throw() {
    myCurrentData.push_back(new TrackerEntry(myParent->createValues(myLane, false)));
}


void
MSMeanData::MeanDataValueTracker::addTo(MSMeanData::MeanDataValues &val) const throw() {
    myCurrentData.front()->myValues->addTo(val);
}


bool
MSMeanData::MeanDataValueTracker::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (vehicleApplies(veh)) {
        return myTrackedData[&veh]->myValues->isStillActive(veh, oldPos, newPos, newSpeed);
    }
    return false;
}


void
MSMeanData::MeanDataValueTracker::notifyLeave(MSVehicle& veh, bool isArrival, bool isLaneChange) throw() {
    if (vehicleApplies(veh) && myTrackedData.find(&veh)!=myTrackedData.end()) { // !!! the second check is a workaround for meso
        myTrackedData[&veh]->myNumVehicleLeft++;
        myTrackedData[&veh]->myValues->notifyLeave(veh, isArrival, isLaneChange);
        // !!! cleanup myTrackedData table
    }
}


bool
MSMeanData::MeanDataValueTracker::notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw() {
    if (vehicleApplies(veh)) {
        myTrackedData[&veh] = myCurrentData.back();
        myTrackedData[&veh]->myNumVehicleEntered++;
        return myTrackedData[&veh]->myValues->notifyEnter(veh, isEmit, isLaneChange);
    }
    return false;
}


bool
MSMeanData::MeanDataValueTracker::isEmpty() const throw() {
    return myCurrentData.front()->myValues->isEmpty();
}


void
MSMeanData::MeanDataValueTracker::write(OutputDevice &dev, const SUMOTime period,
                                        const SUMOReal numLanes, const SUMOReal length, const int numVehicles) const throw(IOError) {
    myCurrentData.front()->myValues->write(dev, period, numLanes, length, myCurrentData.front()->myNumVehicleEntered);
}


size_t
MSMeanData::MeanDataValueTracker::getNumReady() const throw() {
    size_t result = 0;
    for (std::list<TrackerEntry*>::const_iterator it = myCurrentData.begin(); it != myCurrentData.end(); ++it) {
        if ((*it)->myNumVehicleEntered == (*it)->myNumVehicleLeft) {
            result++;
        } else {
            break;
        }
    }
    return result;
}


void
MSMeanData::MeanDataValueTracker::clearFirst() throw() {
    if (myCurrentData.size() == 1) {
        myCurrentData.front()->reset();
    } else {
        myCurrentData.pop_front();
    }
}


SUMOReal
MSMeanData::MeanDataValueTracker::getSamples() const throw() {
    return myCurrentData.front()->myValues->getSamples();
}


#ifdef HAVE_MESOSIM
void
MSMeanData::MeanDataValueTracker::addData(const SUMOVehicle& veh, const SUMOReal timeOnLane,
        const SUMOReal dist) throw() {
    myCurrentData.front()->myValues->addData(veh, timeOnLane, dist);
}


void
MSMeanData::MeanDataValueTracker::getLastReported(SUMOVehicle *v, SUMOTime &lastReportedTime, SUMOReal &lastReportedPos) throw() {
    myCurrentData.front()->myValues->getLastReported(v, lastReportedTime, lastReportedPos);
}


void
MSMeanData::MeanDataValueTracker::setLastReported(SUMOVehicle *v, SUMOTime lastReportedTime, SUMOReal lastReportedPos) throw() {
    myCurrentData.front()->myValues->setLastReported(v, lastReportedTime, lastReportedPos);
}
#endif


// ---------------------------------------------------------------------------
// MSMeanData - methods
// ---------------------------------------------------------------------------
MSMeanData::MSMeanData(const std::string &id,
                       const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                       const bool useLanes, const bool withEmpty,
                       const bool trackVehicles,
                       const SUMOReal maxTravelTime, const SUMOReal minSamples,
                       const std::set<std::string> vTypes) throw()
        : myID(id), myAmEdgeBased(!useLanes), myDumpBegin(dumpBegin), myDumpEnd(dumpEnd),
        myDumpEmpty(withEmpty), myTrackVehicles(trackVehicles),
        myMaxTravelTime(maxTravelTime), myMinSamples(minSamples), myVehicleTypes(vTypes) {
}


void
MSMeanData::init(const std::vector<MSEdge*> &edges, const bool withInternal) throw() {
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        if (withInternal || (*e)->getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL) {
            myEdges.push_back(*e);
            myMeasures.push_back(std::vector<MeanDataValues*>());
#ifdef HAVE_MESOSIM
            if (MSGlobals::gUseMesoSim) {
                MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(**e);
                while (s!=0) {
                    if (myTrackVehicles) {
                        myMeasures.back().push_back(new MeanDataValueTracker(0, &myVehicleTypes, this));
                    } else {
                        myMeasures.back().push_back(createValues(0, false));
                    }
                    s->addDetector(myMeasures.back().back());
                    s = s->getNextSegment();
                }
                continue;
            }
#endif
            if (myAmEdgeBased && myTrackVehicles) {
                myMeasures.back().push_back(new MeanDataValueTracker(0, &myVehicleTypes, this));
            }
            const std::vector<MSLane*> &lanes = (*e)->getLanes();
            for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
                if (myTrackVehicles) {
                    if (myAmEdgeBased) {
                        (*lane)->addMoveReminder(myMeasures.back().back());
                    } else {
                        myMeasures.back().push_back(new MeanDataValueTracker(*lane, &myVehicleTypes, this));
                    }
                } else {
                    myMeasures.back().push_back(createValues(*lane, true));
                }
            }
        }
    }
}


MSMeanData::~MSMeanData() throw() {}


void
MSMeanData::resetOnly(SUMOTime stopTime) throw() {
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(**edge);
            for (std::vector<MeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
                s->prepareMeanDataForWriting(*(*j), stopTime);
                (*j)->reset();
                s = s->getNextSegment();
            }
        }
        return;
    }
#endif
    for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        for (std::vector<MeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            (*j)->reset();
        }
    }
}


void
MSMeanData::writeEdge(OutputDevice &dev,
                      const std::vector<MeanDataValues*> &edgeValues,
                      MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    std::vector<MeanDataValues*>::const_iterator lane;
    if (!myAmEdgeBased) {
        bool writeCheck = myDumpEmpty;
        if (!writeCheck) {
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                if (!(*lane)->isEmpty()) {
                    writeCheck = true;
                    break;
                }
            }
        }
        if (writeCheck) {
            dev.openTag("edge")<<" id=\""<<edge->getID()<<"\">\n";
        }
        for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
            MeanDataValues& meanData = **lane;
            if (writePrefix(dev, meanData, "<lane id=\""+meanData.getLane()->getID())) {
                meanData.write(dev, stopTime - startTime,
                               1.f, meanData.getLane()->getLength());
            }
            if (myTrackVehicles) {
                ((MeanDataValueTracker&)meanData).clearFirst();
            } else {
                meanData.reset();
            }
        }
        if (writeCheck) {
            dev.closeTag();
        }
    } else {
        if (myTrackVehicles) {
            MeanDataValues& meanData = **edgeValues.begin();
            if (writePrefix(dev, meanData, "<edge id=\""+edge->getID())) {
                meanData.write(dev, stopTime - startTime,
                               (SUMOReal)edge->getLanes().size(), edge->getLanes()[0]->getLength());
            }
        } else {
            MeanDataValues* sumData = createValues(0, false);
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                MeanDataValues& meanData = **lane;
                meanData.addTo(*sumData);
                meanData.reset();
            }
            if (writePrefix(dev, *sumData, "<edge id=\""+edge->getID())) {
                sumData->write(dev, stopTime - startTime,
                               (SUMOReal)edge->getLanes().size(), edge->getLanes()[0]->getLength());
            }
            delete sumData;
        }
    }
}


bool
MSMeanData::writePrefix(OutputDevice &dev, const MeanDataValues &values, const std::string prefix) const throw(IOError) {
    if (myDumpEmpty || !values.isEmpty()) {
        dev.indent() << prefix << "\" sampledSeconds=\"" << values.getSamples();
        return true;
    }
    return false;
}


void
MSMeanData::writeXMLOutput(OutputDevice &dev,
                           SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    // check whether this dump shall be written for the current time
    size_t numReady = myDumpBegin < stopTime && myDumpEnd-DELTA_T >= startTime;
    if (myTrackVehicles && myDumpBegin < stopTime) {
        myPendingIntervals.push_back(std::make_pair(startTime, stopTime));
        numReady = myPendingIntervals.size();
        for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
            for (std::vector<MeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
                numReady = MIN2(numReady, ((MeanDataValueTracker*)*j)->getNumReady());
                if (numReady == 0) {
                    break;
                }
            }
            if (numReady == 0) {
                break;
            }
        }
    }
    if (numReady == 0) {
        resetOnly(stopTime);
    }
    while (numReady-- > 0) {
        if (!myPendingIntervals.empty()) {
            startTime = myPendingIntervals.front().first;
            stopTime = myPendingIntervals.front().second;
            myPendingIntervals.pop_front();
        }
        dev.openTag("interval")<<" begin=\""<<time2string(startTime)<<"\" end=\""<<
        time2string(stopTime)<<"\" "<<"id=\""<<myID<<"\">\n";
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            writeEdge(dev, (*i), *edge, startTime, stopTime);
        }
        dev.closeTag();
    }
}


void
MSMeanData::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("netstats");
}


void
MSMeanData::update() throw() {
    for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        const std::vector<MeanDataValues*> &lm = *i;
        for (std::vector<MeanDataValues*>::const_iterator j=lm.begin(); j!=lm.end(); ++j) {
            (*j)->update();
        }
    }
}


/****************************************************************************/

