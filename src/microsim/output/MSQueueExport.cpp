/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSQueueExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    2012-04-26
///
// Export the queueing length in front of a junction (very experimental!)
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#include <mesosim/MEVehicle.h>
#include <microsim/MSVehicleType.h>
#include <microsim/cfmodels/MSCFModel.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSQueueExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>


// ===========================================================================
// static member definitions
// ===========================================================================
SUMOTime MSQueueExport::myIntervalStart = SUMOTime_MIN;
std::map<const MSEdge*, std::vector<std::pair<double, double> > > MSQueueExport::myEdgeSamples;


// ===========================================================================
// method definitions
// ===========================================================================
void
MSQueueExport::write(OutputDevice& of, SUMOTime timestep) {
    const OptionsCont& oc = OptionsCont::getOptions();
    const SUMOTime begin = string2time(oc.getString("begin"));
    const SUMOTime period = string2time(oc.getString("queue-output.period"));
    if (period > 0 && (timestep - begin) % period != 0) {
        return;
    }
    const double threshold = oc.getFloat("queue-output.speed-threshold");
    const SUMOTime aggregation = string2time(oc.getString("queue-output.aggregation"));
    if (aggregation > 0) {
        const SUMOTime intervalStart = begin + ((timestep - begin) / aggregation) * aggregation;
        if (myIntervalStart != SUMOTime_MIN && intervalStart != myIntervalStart) {
            writeInterval(of, myIntervalStart, myIntervalStart + aggregation);
        }
        myIntervalStart = intervalStart;
        writeEdge(nullptr, threshold);
        return;
    }
    of.openTag("data").writeAttr("timestep", time2string(timestep));
    of.openTag("lanes");
    writeEdge(&of, threshold);
    of.closeTag();
    of.closeTag();
}


void
MSQueueExport::finish(OutputDevice& of, SUMOTime timestep) {
    const SUMOTime aggregation = string2time(OptionsCont::getOptions().getString("queue-output.aggregation"));
    if (aggregation > 0 && myIntervalStart != SUMOTime_MIN) {
        writeInterval(of, myIntervalStart, MIN2(myIntervalStart + aggregation, timestep));
    }
    myIntervalStart = SUMOTime_MIN;
    myEdgeSamples.clear();
}


void
MSQueueExport::writeEdge(OutputDevice* of, double threshold) {
    MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
    for (const MSEdge* const edge : ec.getEdges()) {
        if (MSGlobals::gUseMesoSim) {
            double segmentOffset = 0.;
            for (const MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*edge); segment != nullptr; segment = segment->getNextSegment()) {
                for (int qIdx = 0; qIdx < segment->numQueues(); ++qIdx) {
                    writeMesoQueue(of, *edge, *segment, qIdx, segmentOffset, threshold);
                }
                segmentOffset += segment->getLength();
            }
        } else {
            for (const MSLane* const lane : edge->getLanes()) {
                writeLane(of, *lane, threshold);
            }
        }
    }
}


void
MSQueueExport::writeLane(OutputDevice* of, const MSLane& lane, double threshold) {
    // maximum of all vehicle waiting times
    double queueing_time = 0.0;
    // back of last stopped vehicle (XXX does not check for continuous queue)
    double queueing_length = 0.0;
    // back of last slow vehicle (XXX does not check for continuous queue)
    double queueing_length2 = 0.0;
    // number of slow vehicles
    int queueing_count = 0;

    if (!lane.empty()) {
        for (MSLane::VehCont::const_iterator it_veh = lane.myVehicles.begin(); it_veh != lane.myVehicles.end(); ++it_veh) {
            const MSVehicle& veh = **it_veh;
            if (!veh.isOnRoad()) {
                continue;
            }

            if (veh.getWaitingSeconds() > 0) {
                queueing_time = MAX2(veh.getWaitingSeconds(), queueing_time);
                const double veh_back_to_lane_end = (lane.getLength() - veh.getPositionOnLane()) + veh.getVehicleType().getLength();
                queueing_length = MAX2(veh_back_to_lane_end, queueing_length);
            }

            //Experimental
            if (veh.getSpeed() < (threshold) && (veh.getPositionOnLane() > (veh.getLane()->getLength()) * 0.25)) {
                const double veh_back_to_lane_end = (lane.getLength() - veh.getPositionOnLane()) + veh.getVehicleType().getLength();
                queueing_length2 = MAX2(veh_back_to_lane_end, queueing_length2);
                queueing_count++;
            }
        }
    }

    //Output
    if (of != nullptr) {
        if (queueing_length > 1 || queueing_length2 > 1) {
            of->openTag("lane").writeAttr("id", lane.getID()).writeAttr("queueing_time", queueing_time).writeAttr("queueing_length", queueing_length);
            of->writeAttr("queueing_length_experimental", queueing_length2).closeTag();
        }
    } else if (queueing_count > 0) {
        myEdgeSamples[&lane.getEdge()].push_back(std::make_pair((double)queueing_count, queueing_length2));
    }
}


void
MSQueueExport::writeMesoQueue(OutputDevice* of, const MSEdge& edge, const MESegment& segment, int qIdx, double segmentOffset, double threshold) {
    const std::vector<MEVehicle*>& queue = segment.getQueue(qIdx);
    const int queueSize = (int)queue.size();
    // maximum of all vehicle waiting times
    double queueing_time = 0.0;
    // position of the last slow vehicle counted from the head of the queue
    int queueing_count = 0;
    // distance from the segment end to the back of the last slow vehicle
    double queueing_length = 0.0;
    double occupancy = 0.0;
    const double segLength = segment.getLength();
    const double segmentEnd = segmentOffset + segLength;
    const double lanesCovered = segment.numQueues() == 1 ? std::round(segment.getCapacity() / segLength) : 1.;
    // positions and earliest exit times are interpolated as in MSEdge::getMesoPositions
    SUMOTime earliestExitTime = segment.getQueueBlockTime(qIdx);
    double prevPos = std::numeric_limits<double>::max();
    bool prevQueued = false;
    const double now = SIMTIME;
    for (int i = 0; i < queueSize; ++i) {
        // vehicles are stored in reverse entry order (the queue head is at the back)
        const MEVehicle* const veh = queue[queueSize - 1 - i];
        const double lengthWithGap = veh->getVehicleType().getLengthWithGap();
        occupancy += lengthWithGap;
        earliestExitTime = MAX2(earliestExitTime, veh->getEventTime());
        double maxPos = segmentEnd;
        if (i > 0) {
            earliestExitTime += segment.getMinTauWithVehLength(lengthWithGap, veh->getVehicleType().getCarFollowModel().getHeadwayTime());
            maxPos = MIN2(maxPos, prevPos - lengthWithGap / lanesCovered);
        }
        const double entry = veh->getLastEntryTimeSeconds();
        const double travelTime = MAX2(STEPS2TIME(earliestExitTime) - entry, TS);
        const double linearPos = MIN2(segmentEnd, segmentOffset + segLength * (now - entry) / travelTime);
        // slope of the interpolated position trajectory. Unlike getSpeed() this
        // accounts for blocked vehicles further ahead in the queue
        const double interpolatedSpeed = segLength / travelTime;
        // a vehicle is queued if it is slow by itself or if it has caught up
        // with the clamped position behind a queued vehicle ahead
        const bool queued = MIN2(veh->getSpeed(), interpolatedSpeed) < threshold || (prevQueued && linearPos >= maxPos - POSITION_EPS);
        if (queued) {
            queueing_count = i + 1;
            queueing_time = MAX2(veh->getWaitingSeconds(), queueing_time);
            if (MSGlobals::gMesoInterpolatePos) {
                queueing_length = MAX2(0., segmentEnd - (veh->getPositionOnLane() - veh->getVehicleType().getLength()));
            } else {
                queueing_length = occupancy / lanesCovered;
            }
        }
        prevQueued = queued;
        prevPos = MIN2(linearPos, maxPos);
    }
    if (queueing_count == 0) {
        return;
    }
    if (of != nullptr) {
        of->openTag("lane").writeAttr("id", edge.getLanes()[qIdx]->getID()).writeAttr("segment", segment.getIndex());
        of->writeAttr("queueing_time", queueing_time).writeAttr("queueing_length", queueing_length).writeAttr("queueing_count", queueing_count).closeTag();
    } else {
        myEdgeSamples[&edge].push_back(std::make_pair((double)queueing_count, queueing_length));
    }
}


void
MSQueueExport::writeInterval(OutputDevice& of, SUMOTime begin, SUMOTime end) {
    of.openTag("interval").writeAttr("begin", time2string(begin)).writeAttr("end", time2string(end));
    // iterate in network edge order for deterministic output
    for (const MSEdge* const edge : MSNet::getInstance()->getEdgeControl().getEdges()) {
        const auto it = myEdgeSamples.find(edge);
        if (it == myEdgeSamples.end()) {
            continue;
        }
        std::vector<double> counts;
        std::vector<double> lengths;
        for (const auto& sample : it->second) {
            counts.push_back(sample.first);
            lengths.push_back(sample.second);
        }
        std::sort(counts.begin(), counts.end());
        std::sort(lengths.begin(), lengths.end());
        of.openTag("edge").writeAttr("id", edge->getID()).writeAttr("samples", (int)counts.size());
        of.writeAttr("maxQueueLengthInVehicles", counts.back());
        of.writeAttr("medianQueueLengthInVehicles", percentile(counts, 0.5));
        of.writeAttr("p95QueueLengthInVehicles", percentile(counts, 0.95));
        of.writeAttr("maxQueueLengthInMeters", lengths.back());
        of.writeAttr("medianQueueLengthInMeters", percentile(lengths, 0.5));
        of.writeAttr("p95QueueLengthInMeters", percentile(lengths, 0.95));
        of.closeTag();
    }
    of.closeTag();
    myEdgeSamples.clear();
}


double
MSQueueExport::percentile(const std::vector<double>& sorted, double p) {
    assert(!sorted.empty());
    const double rank = p * (double)(sorted.size() - 1);
    const int lower = (int)rank;
    if (lower + 1 >= (int)sorted.size()) {
        return sorted.back();
    }
    const double frac = rank - (double)lower;
    return sorted[lower] * (1. - frac) + sorted[lower + 1] * frac;
}


/****************************************************************************/
