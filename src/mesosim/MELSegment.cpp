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
// A LIFT-model segment 
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
    MESegment(id, parent, next, length, speed, idx, multiQueue, edgeType)
{
    myGapTimes.resize(myQueues.size());
}


void
MELSegment::updateBlockTime(Queue& q, const Queue& /*qNext*/, const MESegment* const /*next*/, const MEVehicle* veh) {
    myLastHeadway = tauWithVehLength(myTau_ff, veh->getVehicleType().getLengthWithGap(), veh->getVehicleType().getCarFollowModel().getHeadwayTime());
    if (myTLSPenalty) {
        const MSLink* const tllink = getLink(veh, true);
        if (tllink != nullptr && tllink->isTLSControlled()) {
            assert(tllink->getGreenFraction() > 0);
            myLastHeadway = (SUMOTime)((double)myLastHeadway / tllink->getGreenFraction());
        }
    }
    q.setBlockTime(q.getBlockTime() + myLastHeadway);
}


void
MELSegment::send(MEVehicle* veh, MESegment* const next, const int nextQIdx, SUMOTime time, const MSMoveReminder::Notification reason) {
    // record time when the gap vacated by ego will reach the upstream end of the segment
    myGapTimes[veh->getQueIndex()].push_front(time + myLength * myTau_jj / DEFAULT_VEH_LENGTH_WITH_GAP);
    MESegment::send(veh, next, nextQIdx, time, reason);
}


void
MELSegment::updateEntryBlockTime(SUMOTime time) {
    for (int qIdx = 0; qIdx < (int)myQueues.size(); qIdx++) {
        Queue& q = myQueues[qIdx];
        GapTimes& gapTimes = myGapTimes[qIdx];
        while (!gapTimes.empty() && gapTimes.back() <= time) {
            gapTimes.pop_back();
        }
        if (!gapTimes.empty() && q.getOccupancy() + gapTimes.size() * DEFAULT_VEH_LENGTH_WITH_GAP > myQueueCapacity) {
            // segment is jammed because the empty spaces have not yet reached the upstream end.
            q.setEntryBlockTime(MAX2(q.getEntryBlockTime(), gapTimes.back()));
        }
    }
}

/****************************************************************************/
