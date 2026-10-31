/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MELSegment.cpp
/// @author  Jakob Erdmann
/// @date    Tue, July 2026
///
// A LTM (LIFT)-model segment
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <limits>
#include <utils/common/StdDefs.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSMoveReminder.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSDriveWay.h>
#include <microsim/traffic_lights/MSRailSignalControl.h>
#include <microsim/output/MSXMLRawOut.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/devices/MSDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/RandHelper.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include "MEVehicle.h"
#include "MELoop.h"
#include "MELSegment.h"

#define DEFAULT_VEH_LENGTH_WITH_GAP (SUMOVTypeParameter::getDefault().length + SUMOVTypeParameter::getDefault().minGap)

//#define DEBUG_OPENED
//#define DEBUG_JAMTHRESHOLD
//#define DEBUG_COND (getID() == "blocker")
//#define DEBUG_COND (true)
#define DEBUG_COND (myEdge.isSelected())
#define DEBUG_COND2(obj) ((obj != 0 && (obj)->isSelected()))


// ===========================================================================
// static member definition
// ===========================================================================


// ===========================================================================
// MELSegment method definitions
// ===========================================================================
MELSegment::MELSegment(const std::string& id,
                       const MSEdge& parent, MESegment* next,
                       const double length, const double speed,
                       const int idx,
                       const bool multiQueue,
                       const MesoEdgeType& edgeType):
    MESegment(id, parent, next, length, speed, idx, multiQueue, edgeType),
    // the gap only has to pass the vehicles that fit into the segment (the remaining length cannot be occupied)
    myJamLength(MAX2(1., floor(length / DEFAULT_VEH_LENGTH_WITH_GAP)) * DEFAULT_VEH_LENGTH_WITH_GAP) {
    myGaps.resize(myQueues.size());
}


SUMOTime
MELSegment::computeHeadway(Queue& /*q*/, const Queue& /*qNext*/, const MESegment* const /*next*/, const MEVehicle* veh) const {
    // @note: it might seem sensible to check veh->getQueuingTimeLoss() here but
    // then the queing state would be "sticky" because queuingTimeLoss is computed with tau_ff
    // and every leader car that uses tau_jf applies queing timeLoss to it's followers
    SUMOTime headway = tauWithVehLength(veh->getWaitingTime() > 0 ? myTau_jf : myTau_ff,
                                        veh->getVehicleType().getLengthWithGap(),
                                        veh->getVehicleType().getCarFollowModel().getHeadwayTime());
    if (myTLSPenalty) {
        const MSLink* const tllink = getLink(veh, true);
        if (tllink != nullptr && tllink->isTLSControlled()) {
            assert(tllink->getGreenFraction() > 0);
            headway = (SUMOTime)((double)headway / tllink->getGreenFraction());
        }
    }
    return headway;
}


void
MELSegment::send(MEVehicle* veh, MESegment* const next, const int nextQIdx, SUMOTime time, const MSMoveReminder::Notification reason) {
    Gaps& gaps = myGaps[veh->getQueIndex()];
    // record time when the gap vacated by ego will reach the upstream end of the segment
    // gaps travel quickly in free flow (the number of gaps and vehicles stays below the segment capacity)
    // but they travel more slowly when queued (startupDelay)
    const bool queued = veh->getQueuingTimeLoss() > 0;
    const SUMOTime waveTime = (SUMOTime)(myJamLength * (double)(queued ? myTau_jj : myTau_ff) / DEFAULT_VEH_LENGTH_WITH_GAP);
    // the entering vehicle needs this long to drive through the length that cannot be occupied
    const SUMOTime remainderTime = TIME2STEPS((myLength - myJamLength) / myEdge.getSpeedLimit());
    myEdge.lock(); // the gui reads the gaps when drawing vehicles
    gaps.insert(gaps.begin(), {time + MAX2((SUMOTime)0, waveTime - remainderTime), time, queued});
    myEdge.unlock();
    MESegment::send(veh, next, nextQIdx, time, reason);
}


void
MELSegment::updateEntryBlockTime(SUMOTime time) {
    for (int qIdx = 0; qIdx < (int)myQueues.size(); qIdx++) {
        Queue& q = myQueues[qIdx];
        const int numGaps = numTravelingGaps(qIdx, time);
        // the entering vehicle needs space in addition to the vehicles and the traveling gaps
        if (numGaps > 0 && q.getOccupancy() + (double)(numGaps + 1) * DEFAULT_VEH_LENGTH_WITH_GAP > myQueueCapacity) {
            // segment is jammed because the empty spaces have not yet reached the upstream end.
            q.setEntryBlockTime(MAX2(q.getEntryBlockTime(), myGaps[qIdx].back().arrival));
        }
    }
}


bool
MELSegment::hasSpaceForInsertion(const Queue& /*q*/, int qIdx, double newOccupancy, SUMOTime entryTime) const {
    return newOccupancy + (double)numTravelingGaps(qIdx, entryTime) * DEFAULT_VEH_LENGTH_WITH_GAP <= myQueueCapacity;
}


bool
MELSegment::hasTravelingGaps(int qIdx, SUMOTime time) const {
    return numTravelingGaps(qIdx, time) > 0;
}


double
MELSegment::getMinDistanceToEnd(int qIdx, double lengthAhead, SUMOTime time) const {
    // a queued vehicle may only move into the space vacated by a departing vehicle once the start wave
    // has reached the place where it waits (one jam spacing behind the vacated space).
    // The distance grows with the number of waves that did not yet arrive so we search the smallest consistent distance
    const double lengthToDist = myLength / myQueueCapacity;
    double dist = lengthAhead * lengthToDist;
    int waveNotArrived = 0;
    while (true) {
        const double distFraction = MIN2(1., (dist + DEFAULT_VEH_LENGTH_WITH_GAP * lengthToDist) / myJamLength);
        int count = 0;
        for (const Gap& gap : myGaps[qIdx]) {
            if (gap.queued && gap.departure + (SUMOTime)(myJamLength * (double)myTau_jj / DEFAULT_VEH_LENGTH_WITH_GAP * distFraction) > time) {
                count++;
            }
        }
        if (count <= waveNotArrived) {
            return dist;
        }
        waveNotArrived = count;
        dist = (lengthAhead + (double)waveNotArrived * DEFAULT_VEH_LENGTH_WITH_GAP) * lengthToDist;
    }
}


int
MELSegment::numTravelingGaps(int qIdx, SUMOTime time) const {
    Gaps& gaps = const_cast<Gaps&>(myGaps[qIdx]);
    if (!gaps.empty() && gaps.back().arrival <= time) {
        myEdge.lock(); // the gui reads the gaps when drawing vehicles
        while (!gaps.empty() && gaps.back().arrival <= time) {
            gaps.pop_back();
        }
        myEdge.unlock();
    }
    return (int)gaps.size();
}


std::string
MELSegment::getQueueExtraState(int qIdx) const {
    // three values per gap: arrival, departure, queued
    std::string result;
    for (const Gap& gap : myGaps[qIdx]) {
        if (!result.empty()) {
            result += " ";
        }
        result += toString(gap.arrival);
        result += " ";
        result += toString(gap.departure);
        result += gap.queued ? " 1" : " 0";
    }
    return result;
}


void
MELSegment::loadQueueExtraState(int qIdx, const std::string& state, SUMOTime offset) {
    Gaps& gaps = myGaps[qIdx];
    gaps.clear();
    const std::vector<std::string> values = StringTokenizer(state).getVector();
    for (int i = 0; i + 2 < (int)values.size(); i += 3) {
        gaps.push_back({StringUtils::toLong(values[i]) - offset, StringUtils::toLong(values[i + 1]) - offset, values[i + 2] == "1"});
    }
}


/****************************************************************************/
