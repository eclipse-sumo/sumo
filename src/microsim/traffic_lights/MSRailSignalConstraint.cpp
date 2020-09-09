/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    MSRailSignalConstraint.cpp
/// @author  Jakob Erdmann
/// @date    August 2020
///
// A constraint on rail signal switching
/****************************************************************************/
#include <config.h>
#include <cassert>
#include <utility>

#include <utils/xml/SUMOSAXAttributes.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include "MSRailSignal.h"
#include "MSRailSignalConstraint.h"

// ===========================================================================
// static value definitions
// ===========================================================================
std::map<const MSLane*, MSRailSignalConstraint_Predecessor::PassedTracker*> MSRailSignalConstraint_Predecessor::myTrackerLookup;

// ===========================================================================
// MSRailSignalConstraint method definitions
// ===========================================================================
void
MSRailSignalConstraint::cleanup() {
    MSRailSignalConstraint_Predecessor::cleanup();
}

void
MSRailSignalConstraint::saveState(OutputDevice& out) {
    MSRailSignalConstraint_Predecessor::saveState(out);
}

void
MSRailSignalConstraint::clearState() {
    MSRailSignalConstraint_Predecessor::clearState();
}

// ===========================================================================
// MSRailSignalConstraint_Predecessor method definitions
// ===========================================================================
MSRailSignalConstraint_Predecessor::MSRailSignalConstraint_Predecessor(const MSRailSignal* signal, const std::string& tripId, int limit) :
    myTripId(tripId),
    myLimit(limit) {
    for (const auto& lv : signal->getLinks()) {
        for (const MSLink* link : lv) {
            MSLane* lane = link->getViaLaneOrLane();
            PassedTracker* pt = nullptr;
            if (myTrackerLookup.count(lane) == 0) {
                pt = new PassedTracker(lane);
                myTrackerLookup[lane] = pt;
            } else {
                pt = myTrackerLookup[lane];
            }
            pt->raiseLimit(limit);
            myTrackers.push_back(pt);
        }
    }

}

void
MSRailSignalConstraint_Predecessor::cleanup() {
    for (auto item : myTrackerLookup) {
        delete item.second;
    }
    myTrackerLookup.clear();
}

void
MSRailSignalConstraint_Predecessor::saveState(OutputDevice& out) {
    for (auto item : myTrackerLookup) {
        item.second->saveState(out);
    }
}

void
MSRailSignalConstraint_Predecessor::loadState(const SUMOSAXAttributes& attrs) {
    bool ok;
    const std::string laneID = attrs.getString(SUMO_ATTR_LANE);
    const int index = attrs.get<int>(SUMO_ATTR_INDEX, "", ok);
    std::vector<std::string> tripIDs = attrs.getStringVector(SUMO_ATTR_STATE);
    MSLane* lane = MSLane::dictionary(laneID);
    if (lane == nullptr) {
        throw ProcessError("Unknown lane '" + laneID + "' in loaded state");
    }
    if (myTrackerLookup.count(lane) == 0) {
        WRITE_WARNINGF("Unknown tracker lane '%' in loaded state", laneID);
        return;
    }
    PassedTracker* tracker = myTrackerLookup[lane];
    tracker->loadState(index, tripIDs);
}


void
MSRailSignalConstraint_Predecessor::clearState() {
    for (auto item : myTrackerLookup) {
        item.second->clearState();
    }
}


bool
MSRailSignalConstraint_Predecessor::cleared() const {
    for (PassedTracker* pt : myTrackers) {
        if (pt->hasPassed(myTripId, myLimit)) {
            return true;
        }
    }
    return false;
}

std::string
MSRailSignalConstraint_Predecessor::getDescription() const {
    return "predecessor " + myTripId + " at signal " + myTrackers.front()->getLane()->getEdge().getFromJunction()->getID();
}

// ===========================================================================
// MSRailSignalConstraint_Predecessor::PassedTracker method definitions
// ===========================================================================

MSRailSignalConstraint_Predecessor::PassedTracker::PassedTracker(MSLane* lane) :
    MSMoveReminder("PassedTracker_" + lane->getID(), lane, true),
    myPassed(1, ""),
    myLastIndex(0)
{ }

bool
MSRailSignalConstraint_Predecessor::PassedTracker::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification /*reason*/, const MSLane* /*enteredLane*/) {
    myLastIndex = (myLastIndex + 1) % myPassed.size();
    myPassed[myLastIndex] = veh.getParameter().getParameter("tripId", veh.getID());
    return true;
}

void
MSRailSignalConstraint_Predecessor::PassedTracker::raiseLimit(int limit) {
    while (limit > (int)myPassed.size()) {
        myPassed.insert(myPassed.begin() + myLastIndex + 1, "");
    }
}

bool
MSRailSignalConstraint_Predecessor::PassedTracker::hasPassed(const std::string& tripId, int limit) const {
    int i = myLastIndex;
    while (limit > 0) {
        if (myPassed[i] == tripId) {
            return true;
        }
        if (i == 0) {
            i = (int)myPassed.size() - 1;
        } else {
            i--;
        }
        limit--;
    }
    return false;
}

void
MSRailSignalConstraint_Predecessor::PassedTracker::clearState() {
    myPassed = std::vector<std::string>(myPassed.size());
    myLastIndex = 0;
}

void
MSRailSignalConstraint_Predecessor::PassedTracker::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_RAILSIGNAL_CONSTRAINT_TRACKER);
    out.writeAttr(SUMO_ATTR_LANE, getLane()->getID());
    out.writeAttr(SUMO_ATTR_INDEX, myLastIndex);
    out.writeAttr(SUMO_ATTR_STATE, myPassed.back() == "" 
            ? std::vector<std::string>(myPassed.begin(), myPassed.begin() + myLastIndex)
            // wrapped around
            : myPassed);
    out.closeTag();
}

void
MSRailSignalConstraint_Predecessor::PassedTracker::loadState(int index, const std::vector<std::string>& tripIDs) {
    raiseLimit(tripIDs.size());
    for (int i = 0; i < (int)tripIDs.size(); i++) {
        myPassed[i] = tripIDs[i];
    }
    myLastIndex = index;
}

/****************************************************************************/
