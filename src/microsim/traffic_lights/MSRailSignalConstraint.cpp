/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
#include <utils/common/StringUtils.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include "MSRailSignal.h"
#include "MSRailSignalConstraint.h"
#include "MSRailSignalControl.h"

//#define DEBUG_PASSED
//#define DEBUG_LANE

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
    if (OptionsCont::getOptions().getBool("save-state.constraints")) {
        for (MSRailSignal* s : MSRailSignalControl::getInstance().getSignals()) {
            if (s->getConstraints().size() > 0) {
                out.openTag(SUMO_TAG_RAILSIGNAL_CONSTRAINTS);
                out.writeAttr(SUMO_ATTR_ID, s->getID());
                for (auto item : s->getConstraints()) {
                    for (MSRailSignalConstraint* c : item.second) {
                        c->write(out, item.first);
                    }
                }
                out.closeTag();
            }
        }
    }
    MSRailSignalConstraint_Predecessor::saveState(out);
}

void
MSRailSignalConstraint::clearState() {
    MSRailSignalConstraint_Predecessor::clearState();
}

void
MSRailSignalConstraint::clearAll() {
    for (MSRailSignal* s : MSRailSignalControl::getInstance().getSignals()) {
        s->removeConstraints();
    }
}


std::string
MSRailSignalConstraint::getVehID(const std::string& tripID) {
    MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt i = c.loadedVehBegin(); i != c.loadedVehEnd(); ++i) {
        SUMOVehicle* veh = i->second;
        if (veh->getParameter().getParameter("tripId") == tripID) {
            return veh->getID();
        }
    }
    return "";
}

// ===========================================================================
// MSRailSignalConstraint_Predecessor method definitions
// ===========================================================================
MSRailSignalConstraint_Predecessor::MSRailSignalConstraint_Predecessor(ConstraintType type, const MSRailSignal* signal, const std::string& tripId, int limit, bool active) :
    MSRailSignalConstraint(type),
    myTripId(tripId),
    myLimit(limit),
    myAmActive(active),
    myFoeSignal(signal) {
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
    const int index = attrs.get<int>(SUMO_ATTR_INDEX, nullptr, ok);
    const std::vector<std::string>& tripIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_STATE, nullptr, ok);
    MSLane* lane = MSLane::dictionary(laneID);
    if (lane == nullptr) {
        throw ProcessError(TLF("Unknown lane '%' in loaded state.", laneID));
    }
    if (myTrackerLookup.count(lane) == 0) {
        WRITE_WARNINGF(TL("Unknown tracker lane '%' in loaded state."), laneID);
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
    if (!myAmActive) {
        return true;
    }
    for (PassedTracker* pt : myTrackers) {
        if (pt->hasPassed(myTripId, myLimit)) {
            return true;
        }
    }
    return false;
}

std::string
MSRailSignalConstraint_Predecessor::getDescription() const {
    // try to retrieve vehicle id that belongs to myTripId
    // this may be slow so it should only be used for debugging
    std::string vehID = getVehID(myTripId);
    if (vehID != "") {
        vehID = " (" + vehID + ")";
    }
    std::vector<std::string> passedIDs;
    for (const std::string& passedTripID : myTrackers.front()->myPassed) {
        if (passedTripID == "") {
            continue;
        }
        const std::string passedID = getVehID(passedTripID);
        if (passedID != "") {
            passedIDs.push_back(passedID);
        }
    }
    std::string passedIDs2 = "";
    if (passedIDs.size() > 0) {
        passedIDs2 = " (" + toString(passedIDs) + ")";
    }
    std::string params = "";
    for (auto item : getParametersMap()) {
        params += ("\n  key=" + item.first + " value=" + item.second);
    }
    return (toString(getTag()) + "  " + myTripId + vehID + " at signal " + myTrackers.front()->getLane()->getEdge().getFromJunction()->getID()
            + " passed=" + StringUtils::prune(toString(myTrackers.front()->myPassed)) + passedIDs2 + params);
}

// ===========================================================================
// MSRailSignalConstraint_Predecessor::PassedTracker method definitions
// ===========================================================================

MSRailSignalConstraint_Predecessor::PassedTracker::PassedTracker(MSLane* lane) :
    MSMoveReminder("PassedTracker_" + lane->getID(), lane, true),
    myPassed(1, ""),
    myLastIndex(-1)
{ }

bool
MSRailSignalConstraint_Predecessor::PassedTracker::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification /*reason*/, const MSLane* /*enteredLane*/) {
    myLastIndex = (myLastIndex + 1) % myPassed.size();
    myPassed[myLastIndex] = veh.getParameter().getParameter("tripId", veh.getID());
#ifdef DEBUG_PASSED
    if (myLane->getID() == DEBUG_LANE) {
        std::cout << SIMTIME << " hasPassed " << veh.getID() << " tripId=" << veh.getParameter().getParameter("tripId", veh.getID()) << " index=" << myLastIndex << "\n";
    }
#endif
    return true;
}

void
MSRailSignalConstraint_Predecessor::PassedTracker::raiseLimit(int limit) {
    while (limit > (int)myPassed.size()) {
        myPassed.insert(myPassed.begin() + (myLastIndex + 1), "");
    }
#ifdef DEBUG_PASSED
    if (myLane->getID() == DEBUG_LANE) {
        std::cout << " raiseLimit=" << limit << "\n";
    }
#endif
}

bool
MSRailSignalConstraint_Predecessor::PassedTracker::hasPassed(const std::string& tripId, int limit) const {
    if (myLastIndex < 0) {
        return false;
    }
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
    const std::string state = toString(myPassed.back() == ""
                                       ? std::vector<std::string>(myPassed.begin(), myPassed.begin() + (myLastIndex + 1))
                                       // wrapped around
                                       : myPassed);
    // no need to save state if no vehicles have passed this tracker
    if (state != "") {
        out.openTag(SUMO_TAG_RAILSIGNAL_CONSTRAINT_TRACKER);
        out.writeAttr(SUMO_ATTR_LANE, getLane()->getID());
        out.writeAttr(SUMO_ATTR_INDEX, myLastIndex);
        out.writeAttr(SUMO_ATTR_STATE, state);
        out.closeTag();
    }
}

void
MSRailSignalConstraint_Predecessor::PassedTracker::loadState(int index, const std::vector<std::string>& tripIDs) {
    raiseLimit((int)tripIDs.size());
    for (int i = 0; i < (int)tripIDs.size(); i++) {
        myPassed[i] = tripIDs[i];
    }
#ifdef DEBUG_PASSED
    if (myLane->getID() == DEBUG_LANE) {
        std::cout << " loadState limit=" << tripIDs.size() << " index=" << index << "\n";
        for (int i = 0; i < (int)myPassed.size(); i++) {
            std::cout << " i=" << i << " passed=" << myPassed[i] << "\n";
        }
    }
#endif
    myLastIndex = index;
}


void
MSRailSignalConstraint_Predecessor::write(OutputDevice& out, const std::string& tripId) const {
    out.openTag(getTag());
    out.writeAttr(SUMO_ATTR_TRIP_ID, tripId);
    out.writeAttr(SUMO_ATTR_TLID, myFoeSignal->getID());
    out.writeAttr(SUMO_ATTR_FOES, myTripId);
    if (myLimit > 1) {
        out.writeAttr(SUMO_ATTR_LIMIT, myLimit);
    }
    if (!myAmActive) {
        out.writeAttr(SUMO_ATTR_ACTIVE, myAmActive);
    }
    writeParams(out);
    out.closeTag();
}

/****************************************************************************/
