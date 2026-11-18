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
#include <microsim/MSStop.h>
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
        writeEdge(nullptr, threshold, [] {});
        return;
    }
    // The timestep block is opened lazily so that with --queue-output.skip-empty
    // (and, in particular, for columnar formats like Parquet or CSV) no row is
    // emitted for time steps without any queue.
    bool wrapperOpen = false;
    auto ensureOpen = [&]() {
        if (!wrapperOpen) {
            of.openTag("data").writeAttr("timestep", time2string(timestep));
            of.openTag("lanes");
            wrapperOpen = true;
        }
    };
    if (!oc.getBool("queue-output.skip-empty")) {
        ensureOpen();
    }
    writeEdge(&of, threshold, ensureOpen);
    if (wrapperOpen) {
        of.closeTag(); // lanes
        of.closeTag(); // data
    }
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
MSQueueExport::writeEdge(OutputDevice* of, double threshold, const std::function<void()>& ensureOpen) {
    MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
    for (const MSEdge* const edge : ec.getEdges()) {
        if (MSGlobals::gUseMesoSim) {
            double segmentOffset = 0.;
            for (const MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*edge); segment != nullptr; segment = segment->getNextSegment()) {
                for (int qIdx = 0; qIdx < segment->numQueues(); ++qIdx) {
                    writeMesoQueue(of, *edge, *segment, qIdx, segmentOffset, threshold, ensureOpen);
                }
                segmentOffset += segment->getLength();
            }
        } else {
            for (const MSLane* const lane : edge->getLanes()) {
                writeLane(of, *lane, threshold, ensureOpen);
            }
        }
    }
}


void
MSQueueExport::writeLane(OutputDevice* of, const MSLane& lane, double threshold, const std::function<void()>& ensureOpen) {
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
            ensureOpen();
            of->openTag("lane").writeAttr("id", lane.getID()).writeAttr("queueing_time", queueing_time).writeAttr("queueing_length", queueing_length);
            of->writeAttr("queueing_length_experimental", queueing_length2).closeTag();
        }
    } else if (queueing_count > 0) {
        myEdgeSamples[&lane.getEdge()].push_back(std::make_pair((double)queueing_count, queueing_length2));
    }
}


void
MSQueueExport::writeMesoQueue(OutputDevice* of, const MSEdge& edge, const MESegment& segment, int qIdx, double segmentOffset, double threshold, const std::function<void()>& ensureOpen) {
    const std::vector<MEVehicle*>& queue = segment.getQueue(qIdx);
    const int queueSize = (int)queue.size();
    // maximum of all vehicle waiting times
    double queueing_time = 0.0;
    // position of the last queued vehicle counted from the head of the queue (without stopped vehicles)
    int queueing_count = 0;
    // distance from the segment end to the back of the last queued vehicle (without stopped vehicles)
    double queueing_length = 0.0;
    // number and length of the vehicles seen so far which are not stopped
    int count = 0;
    double occupancy = 0.0;
    // length of the stopped vehicles seen so far
    double stoppedOccupancy = 0.0;
    const double segLength = segment.getLength();
    const double segmentEnd = segmentOffset + segLength;
    const double lanesCovered = segment.numQueues() == 1 ? std::round(segment.getCapacity() / segLength) : 1.;
    // with interpolated positions a vehicle is queued if its position barely changed since the previous step
    const bool usePositions = MSGlobals::gMesoInterpolatePos;
    const std::map<const MEVehicle*, std::pair<double, int> >* const mesoPos = usePositions ? &edge.getMesoPositions() : nullptr;
    // positions and earliest exit times are interpolated as in MSEdge::getMesoPositions
    SUMOTime earliestExitTime = segment.getQueueBlockTime(qIdx);
    // conservative exit times use the jam-dependent headway as in MESegment::getMeanSpeed
    SUMOTime conservativeExitTime = earliestExitTime;
    double prevPos = std::numeric_limits<double>::max();
    // whether the vehicle ahead is queued or stopped and thus blocks the current vehicle
    bool prevBlocking = false;
    // whether the blocking vehicles ahead wait behind a stopped vehicle
    bool behindStop = false;
    // the queue is measured from the segment end or from the back of a stopped vehicle at its head
    double queueFront = segmentEnd;
    bool haveQueueFront = false;
    const MSLane* const lane = edge.getLanes()[qIdx];
    const double now = SIMTIME;
    for (int i = 0; i < queueSize; ++i) {
        // vehicles are stored in reverse entry order (the queue head is at the back)
        const MEVehicle* const veh = queue[queueSize - 1 - i];
        const double lengthWithGap = veh->getVehicleType().getLengthWithGap();
        earliestExitTime = MAX2(earliestExitTime, veh->getEventTime());
        conservativeExitTime = MAX2(conservativeExitTime, veh->getEventTime());
        double maxPos = segmentEnd;
        if (i > 0) {
            earliestExitTime += segment.getMinTauWithVehLength(lengthWithGap, veh->getVehicleType().getCarFollowModel().getHeadwayTime());
            conservativeExitTime += segment.getTauWithVehLength(qIdx, lengthWithGap, veh->getVehicleType().getCarFollowModel().getHeadwayTime());
            maxPos = MIN2(maxPos, prevPos - lengthWithGap / lanesCovered);
        }
        const double entry = veh->getLastEntryTimeSeconds();
        const double travelTime = MAX2(STEPS2TIME(earliestExitTime) - entry, TS);
        double pos = MIN2(maxPos, segmentOffset + segLength * (now - entry) / travelTime);
        bool slow;
        if (usePositions) {
            const auto it = mesoPos->find(veh);
            if (it != mesoPos->end()) {
                pos = it->second.first;
            }
            // vehicles without a position in the previous step have just entered the edge and are thus moving
            const double posSpeed = edge.getMesoPositionSpeed(veh);
            // the tolerance allows for a threshold of 0 (vehicle did not move at all)
            slow = posSpeed >= 0 && posSpeed <= threshold + NUMERICAL_EPS;
        } else {
            // conservative speed of the interpolated position trajectory. Unlike
            // getSpeed() this accounts for blocked vehicles further ahead in the queue
            const double interpolatedSpeed = segLength / MAX2(STEPS2TIME(conservativeExitTime) - entry, TS);
            slow = MIN2(veh->getSpeed(), interpolatedSpeed) < threshold;
        }
        // the position the vehicle would have reached at free-flow speed (without being blocked)
        const double freePos = segmentOffset + lane->getVehicleMaxSpeed(veh) * (now - entry);
        if (veh->isStopped()) {
            // stopped vehicles are not part of the queue but the vehicles behind them may be
            const double stopPos = MAX2(segmentOffset, MIN2(segmentEnd, veh->getNextStop().pars.endPos));
            pos = MIN3(maxPos, freePos, stopPos);
            if (!haveQueueFront) {
                // the queue starts behind the stopped vehicle
                queueFront = pos - veh->getVehicleType().getLength();
                haveQueueFront = true;
                count = 0;
                occupancy = 0.;
            } else {
                stoppedOccupancy += lengthWithGap;
            }
            prevBlocking = true;
            behindStop = true;
        } else {
            count++;
            occupancy += lengthWithGap;
            // the vehicle has caught up with the clamped position behind a blocking vehicle ahead
            bool queued = slow || (prevBlocking && pos >= maxPos - POSITION_EPS);
            // The interpolated positions spread the waiting time of a blocked vehicle over its whole travel
            // time on the segment so that the vehicles at the back of a queue (or behind a stop) keep moving slowly.
            // Instead they are queued as soon as they would have reached the vehicle ahead at free-flow speed.
            // Without position interpolation this applies only behind a stopped vehicle.
            if (prevBlocking && (usePositions || behindStop) && freePos >= maxPos - POSITION_EPS) {
                queued = true;
                pos = maxPos;
            }
            if (queued) {
                haveQueueFront = true;
                queueing_count = count;
                queueing_time = MAX2(veh->getWaitingSeconds(), queueing_time);
                if (usePositions) {
                    queueing_length = MAX2(0., queueFront - (pos - veh->getVehicleType().getLength()) - stoppedOccupancy / lanesCovered);
                } else {
                    queueing_length = occupancy / lanesCovered;
                }
            } else {
                behindStop = false;
            }
            prevBlocking = queued;
        }
        prevPos = pos;
    }
    if (queueing_count == 0) {
        return;
    }
    if (of != nullptr) {
        ensureOpen();
        of->openTag("lane").writeAttr("id", edge.getLanes()[qIdx]->getID()).writeAttr("segment", segment.getIndex());
        of->writeAttr("queueing_time", queueing_time).writeAttr("queueing_length", queueing_length).writeAttr("queueing_count", queueing_count).closeTag();
    } else {
        myEdgeSamples[&edge].push_back(std::make_pair((double)queueing_count, queueing_length));
    }
}


void
MSQueueExport::writeInterval(OutputDevice& of, SUMOTime begin, SUMOTime end) {
    // The interval element is opened lazily so that with --queue-output.skip-empty
    // (and, in particular, for columnar formats) no row is emitted for intervals
    // without any queue.
    bool intervalOpen = false;
    auto ensureOpen = [&]() {
        if (!intervalOpen) {
            of.openTag("interval").writeAttr("begin", time2string(begin)).writeAttr("end", time2string(end));
            intervalOpen = true;
        }
    };
    const OptionsCont& oc = OptionsCont::getOptions();
    if (!oc.getBool("queue-output.skip-empty")) {
        ensureOpen();
    }
    const double p = oc.getFloat("queue-output.percentile") / 100.;
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
        ensureOpen();
        of.openTag("edge").writeAttr("id", edge->getID()).writeAttr("samples", (int)counts.size());
        of.writeAttr("maxQueueLengthInVehicles", counts.back());
        of.writeAttr("medianQueueLengthInVehicles", percentile(counts, 0.5));
        of.writeAttr("percentileQueueLengthInVehicles", percentile(counts, p));
        of.writeAttr("maxQueueLengthInMeters", lengths.back());
        of.writeAttr("medianQueueLengthInMeters", percentile(lengths, 0.5));
        of.writeAttr("percentileQueueLengthInMeters", percentile(lengths, p));
        of.closeTag();
    }
    if (intervalOpen) {
        of.closeTag();
    }
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
