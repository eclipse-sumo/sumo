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
/// @file    MSLaneSpeedTrigger.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
///
// Changes the speed allowed on a set of lanes
/****************************************************************************/
#include <config.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/WrappingCommand.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/StringUtils.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include "MSLaneSpeedTrigger.h"

#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>


// ===========================================================================
// static members
// ===========================================================================
std::map<std::string, MSLaneSpeedTrigger*> MSLaneSpeedTrigger::myInstances;

// ===========================================================================
// method definitions
// ===========================================================================
MSLaneSpeedTrigger::MSLaneSpeedTrigger(const std::string& id,
                                       const std::vector<MSLane*>& destLanes,
                                       const std::string& file) :
    Named(id),
    SUMOSAXHandler(file),
    myDestLanes(destLanes),
    myDefaultSpeed(destLanes[0]->getSpeedLimit()),
    myDefaultFriction(destLanes[0]->getFrictionCoefficient()),
    myAmOverriding(false),
    mySpeedOverrideValue(destLanes[0]->getSpeedLimit()),
    myDidInit(false) {
    myInstances[id] = this;
    if (file != "") {
        if (!XMLSubSys::runParser(*this, file)) {
            throw ProcessError();
        }
        if (!myDidInit) {
            init();
        }
    }
}


void
MSLaneSpeedTrigger::init() {
    // set the process to the begin
    myCurrentSpeedEntry = myLoadedSpeeds.begin();
    myCurrentFrictionEntry = myLoadedFrictions.begin();
    // pass previous time steps
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    while (myCurrentSpeedEntry != myLoadedSpeeds.end() && myCurrentSpeedEntry->first < now) {
        processCommand(true, now);
    }
    while (myCurrentFrictionEntry != myLoadedFrictions.end() && myCurrentFrictionEntry->first < now) {
        executeFrictionChange(now);
    }

    // add the processing to the event handler
    if (myCurrentSpeedEntry != myLoadedSpeeds.end()) {
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
            new WrappingCommand<MSLaneSpeedTrigger>(this, &MSLaneSpeedTrigger::executeSpeedChange),
            myCurrentSpeedEntry->first);
    }
    if (myCurrentFrictionEntry != myLoadedFrictions.end()) {
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
            new WrappingCommand<MSLaneSpeedTrigger>(this, &MSLaneSpeedTrigger::executeFrictionChange),
            myCurrentFrictionEntry->first);
    }
    myDidInit = true;
}


MSLaneSpeedTrigger::~MSLaneSpeedTrigger() {
    myInstances.erase(getID());
}


SUMOTime
MSLaneSpeedTrigger::executeSpeedChange(SUMOTime currentTime) {
    return processCommand(true, currentTime);
}


SUMOTime
MSLaneSpeedTrigger::processCommand(bool move2next, SUMOTime currentTime) {
    const double speed = getCurrentSpeed();
    if (MSGlobals::gUseMesoSim) {
        if (myDestLanes.size() > 0 && myDestLanes.front()->getSpeedLimit() != speed) {
            myDestLanes.front()->getEdge().setMaxSpeed(speed);
            MESegment* first = MSGlobals::gMesoNet->getSegmentForEdge(myDestLanes.front()->getEdge());
            while (first != nullptr) {
                first->setSpeed(speed, currentTime, -1);
                first = first->getNextSegment();
            }
        }
    } else {
        for (MSLane* const lane : myDestLanes) {
            lane->setMaxSpeed(speed);
        }
    }
    if (!move2next) {
        // changed from the gui
        return 0;
    }
    if (myCurrentSpeedEntry != myLoadedSpeeds.end()) {
        ++myCurrentSpeedEntry;
    }
    if (myCurrentSpeedEntry != myLoadedSpeeds.end()) {
        return myCurrentSpeedEntry->first - currentTime;
    }
    return 0;
}


SUMOTime
MSLaneSpeedTrigger::executeFrictionChange(SUMOTime currentTime) {
    const double friction = getCurrentFriction();
    for (MSLane* const lane : myDestLanes) {
        lane->setFrictionCoefficient(friction);
    }
    if (myCurrentFrictionEntry != myLoadedFrictions.end()) {
        ++myCurrentFrictionEntry;
    }
    if (myCurrentFrictionEntry != myLoadedFrictions.end()) {
        return myCurrentFrictionEntry->first - currentTime;
    }
    return 0;
}


void
MSLaneSpeedTrigger::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // check whether the correct tag is read
    if (element != SUMO_TAG_STEP) {
        return;
    }
    // extract the values
    bool ok = true;
    const SUMOTime next = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, getID().c_str(), ok);
    double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, getID().c_str(), ok, -1);
    double friction = attrs.getOpt<double>(SUMO_ATTR_FRICTION, getID().c_str(), ok, -1);
    // check the values
    if (next < 0 || (speed > 0 && !myLoadedSpeeds.empty() && myLoadedSpeeds.back().first > next) ||
            (friction > 0 && !myLoadedFrictions.empty() && myLoadedFrictions.back().first > next)) {
        WRITE_ERRORF(TL("Invalid or unsorted time entry in vss '%'."), getID());
        return;
    }
    if (speed < 0 && friction < 0) {
        speed = myDefaultSpeed;
        friction = myDefaultFriction;
    }
    if (speed < 0 && attrs.hasAttribute(SUMO_ATTR_SPEED)) {
        speed = myDefaultSpeed;
    }
    if (friction < 0 && attrs.hasAttribute(SUMO_ATTR_FRICTION)) {
        friction = myDefaultFriction;
    }
    // set the values for the next step if they are valid
    if (speed >= 0) {
        if (myLoadedSpeeds.size() != 0 && myLoadedSpeeds.back().first == next) {
            WRITE_WARNINGF(TL("Time % was set twice for vss '%'; replacing first entry."), time2string(next), getID());
            myLoadedSpeeds.back().second = speed;
        } else {
            myLoadedSpeeds.push_back(std::make_pair(next, speed));
        }
    }
    if (friction >= 0) {
        myLoadedFrictions.push_back(std::make_pair(next, friction));
    }
}


void
MSLaneSpeedTrigger::myEndElement(int element) {
    if (element == SUMO_TAG_VSS && !myDidInit) {
        init();
    }
}


double
MSLaneSpeedTrigger::getDefaultSpeed() const {
    return myDefaultSpeed;
}


void
MSLaneSpeedTrigger::setOverriding(bool val) {
    myAmOverriding = val;
    processCommand(false, MSNet::getInstance()->getCurrentTimeStep());
}


void
MSLaneSpeedTrigger::setOverridingValue(double val) {
    mySpeedOverrideValue = val;
    processCommand(false, MSNet::getInstance()->getCurrentTimeStep());
}


double
MSLaneSpeedTrigger::getLoadedSpeed() {
    if (myLoadedSpeeds.empty()) {
        return myDefaultSpeed;
    }
    if (myCurrentSpeedEntry != myLoadedSpeeds.begin()) {
        return (*(myCurrentSpeedEntry - 1)).second;
    } else {
        return myCurrentSpeedEntry->second;
    }
}


double
MSLaneSpeedTrigger::getCurrentSpeed() const {
    if (myAmOverriding) {
        return mySpeedOverrideValue;
    } else {
        if (myLoadedSpeeds.empty()) {
            return myDefaultSpeed;
        }
        const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
        // ok, maybe the first shall not yet be the valid one
        if (myCurrentSpeedEntry == myLoadedSpeeds.begin() && myCurrentSpeedEntry->first > now) {
            return myDefaultSpeed;
        }
        // try the loaded
        if (myCurrentSpeedEntry != myLoadedSpeeds.end() && myCurrentSpeedEntry->first <= now) {
            return myCurrentSpeedEntry->second;
        } else {
            // we have run past the end of the loaded steps or the current step is not yet active:
            // -> use the value of the previous step
            return (*(myCurrentSpeedEntry - 1)).second;
        }
    }
}


double
MSLaneSpeedTrigger::getCurrentFriction() const {
    if (myLoadedFrictions.empty()) {
        return myDefaultFriction;
    }
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    // ok, maybe the first shall not yet be the valid one
    if (myCurrentFrictionEntry == myLoadedFrictions.begin() && myCurrentFrictionEntry->first > now) {
        return myDefaultFriction;
    }
    // try the loaded
    if (myCurrentFrictionEntry != myLoadedFrictions.end() && myCurrentFrictionEntry->first <= now) {
        return myCurrentFrictionEntry->second;
    } else {
        // we have run past the end of the loaded steps or the current step is not yet active:
        // -> use the value of the previous step
        return (*(myCurrentFrictionEntry - 1)).second;
    }
}


/****************************************************************************/
