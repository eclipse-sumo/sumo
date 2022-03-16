/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    MSFrictionCoefficientTrigger.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Thomas Weber
/// @date    Sept 2002
///
// Changes the Friction allowed on a set of lanes
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
#include "MSFrictionCoefficientTrigger.h"

#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>


// ===========================================================================
// static members
// ===========================================================================
std::map<std::string, MSFrictionCoefficientTrigger*> MSFrictionCoefficientTrigger::myInstances;

// ===========================================================================
// method definitions
// ===========================================================================
MSFrictionCoefficientTrigger::MSFrictionCoefficientTrigger(const std::string& id,
                                       const std::vector<MSLane*>& destLanes,
                                       const std::string& file) :
    MSTrigger(id),
    SUMOSAXHandler(file),
    myDestLanes(destLanes),
    myCurrentFriction(destLanes[0]->getFrictionCoefficient()),
    myDefaultFriction(destLanes[0]->getFrictionCoefficient()),
    myAmOverriding(false),
    myFrictionOverrideValue(destLanes[0]->getFrictionCoefficient()),
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
MSFrictionCoefficientTrigger::init() {
    // set it to the right value
    // assert there is at least one
    if (myLoadedFrictions.size() == 0) {
        myLoadedFrictions.push_back(std::make_pair(100000, myCurrentFriction));
    }
    // set the process to the begin
    myCurrentEntry = myLoadedFrictions.begin();
    // pass previous time steps
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    while ((*myCurrentEntry).first < now && myCurrentEntry != myLoadedFrictions.end()) {
        processCommand(true, now);
    }

    // add the processing to the event handler
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
        new WrappingCommand<MSFrictionCoefficientTrigger>(this, &MSFrictionCoefficientTrigger::execute),
        (*myCurrentEntry).first);
    myDidInit = true;
}


MSFrictionCoefficientTrigger::~MSFrictionCoefficientTrigger() {}


SUMOTime
MSFrictionCoefficientTrigger::execute(SUMOTime currentTime) {
    return processCommand(true, currentTime);
}


SUMOTime
MSFrictionCoefficientTrigger::processCommand(bool move2next, SUMOTime currentTime) {
    UNUSED_PARAMETER(currentTime);
    std::vector<MSLane*>::iterator i;
    const double Friction = getCurrentFriction();
    if (MSGlobals::gUseMesoSim) {
        if (myDestLanes.size() > 0 && myDestLanes.front()->getFrictionCoefficient() != Friction) {
            myDestLanes.front()->getEdge().setFrictionCoefficient(Friction);
            MESegment* first = MSGlobals::gMesoNet->getSegmentForEdge(myDestLanes.front()->getEdge());
            while (first != nullptr) {
                first->setFrictionCoefficient(Friction, currentTime); // MSSegment::setFrictionCoefficient empty dummy until implementation in mesosim
                first = first->getNextSegment();
            }
        }
    } else {
        for (i = myDestLanes.begin(); i != myDestLanes.end(); ++i) {
            (*i)->setFrictionCoefficient(Friction);
        }
    }
    if (!move2next) {
        // changed from the gui
        return 0;
    }
    if (myCurrentEntry != myLoadedFrictions.end()) {
        ++myCurrentEntry;
    }
    if (myCurrentEntry != myLoadedFrictions.end()) {
        return ((*myCurrentEntry).first) - ((*(myCurrentEntry - 1)).first);
    } else {
        return 0;
    }
}


void
MSFrictionCoefficientTrigger::myStartElement(int element,
                                   const SUMOSAXAttributes& attrs) {
    // check whether the correct tag is read
    if (element != SUMO_TAG_STEP_COF) {
        return;
    }
    // extract the values
    bool ok = true;
    SUMOTime next = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, getID().c_str(), ok);
    double Friction = attrs.getOpt<double>(SUMO_ATTR_FRICTION, getID().c_str(), ok, -1);
    // check the values
    if (next < 0) {
        WRITE_ERROR("Wrong time in COF '" + getID() + "'.");
        return;
    }
    if (Friction < 0) {
        Friction = myDefaultFriction;
    }
    // set the values for the next step if they are valid
    if (myLoadedFrictions.size() != 0 && myLoadedFrictions.back().first == next) {
        WRITE_WARNING("Time " + time2string(next) + " was set twice for COF '" + getID() + "'; replacing first entry.");
        myLoadedFrictions.back().second = Friction;
    } else {
        myLoadedFrictions.push_back(std::make_pair(next, Friction));
    }
}


void
MSFrictionCoefficientTrigger::myEndElement(int element) {
    if (element == SUMO_TAG_COF && !myDidInit) {
        init();
    }
}


double
MSFrictionCoefficientTrigger::getDefaultFriction() const {
    return myDefaultFriction;
}


void
MSFrictionCoefficientTrigger::setOverriding(bool val) {
    myAmOverriding = val;
    processCommand(false, MSNet::getInstance()->getCurrentTimeStep());
}


void
MSFrictionCoefficientTrigger::setOverridingValue(double val) {
    myFrictionOverrideValue = val;
    processCommand(false, MSNet::getInstance()->getCurrentTimeStep());
}


double
MSFrictionCoefficientTrigger::getLoadedFriction() {
    if (myCurrentEntry != myLoadedFrictions.begin()) {
        return (*(myCurrentEntry - 1)).second;
    } else {
        return (*myCurrentEntry).second;
    }
}


double
MSFrictionCoefficientTrigger::getCurrentFriction() const {
    if (myAmOverriding) {
        return myFrictionOverrideValue;
    } else {
        const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
        // ok, maybe the first shall not yet be the valid one
        if (myCurrentEntry == myLoadedFrictions.begin() && (*myCurrentEntry).first > now) {
            return myDefaultFriction;
        }
        // try the loaded
        if (myCurrentEntry != myLoadedFrictions.end() && (*myCurrentEntry).first <= now) {
            return (*myCurrentEntry).second;
        } else {
            // we have run past the end of the loaded steps or the current step is not yet active:
            // -> use the value of the previous step
            return (*(myCurrentEntry - 1)).second;
        }
    }
}


/****************************************************************************/
