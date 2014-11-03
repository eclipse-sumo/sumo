/****************************************************************************/
/// @file    MSTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The parent class for traffic light logics
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <cassert>
#include <string>
#include <iostream>
#include <map>
#include <microsim/MSLink.h>
#include <microsim/MSLane.h>
#include "MSTrafficLightLogic.h"
#include <microsim/MSEventControl.h>
#include "MSTLLogicControl.h"
#include <microsim/MSJunctionLogic.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * member method definitions
 * ----------------------------------------------------------------------- */
MSTrafficLightLogic::SwitchCommand::SwitchCommand(MSTLLogicControl& tlcontrol,
        MSTrafficLightLogic* tlLogic, SUMOTime nextSwitch)
    : myTLControl(tlcontrol), myTLLogic(tlLogic),
      myAssumedNextSwitch(nextSwitch), myAmValid(true) {}


MSTrafficLightLogic::SwitchCommand::~SwitchCommand() {}



SUMOTime
MSTrafficLightLogic::SwitchCommand::execute(SUMOTime t) {
    // check whether this command has been descheduled
    if (!myAmValid) {
        return 0;
    }
    //
    bool isActive = myTLControl.isActive(myTLLogic);
    size_t step1 = myTLLogic->getCurrentPhaseIndex();
    SUMOTime next = myTLLogic->trySwitch(isActive);
    size_t step2 = myTLLogic->getCurrentPhaseIndex();
    if (step1 != step2) {
        if (isActive) {
            // execute any action connected to this tls
            const MSTLLogicControl::TLSLogicVariants& vars = myTLControl.get(myTLLogic->getID());
            // set link priorities
            myTLLogic->setTrafficLightSignals(t);
            // execute switch actions
            vars.executeOnSwitchActions();
        }
    }
    myAssumedNextSwitch += next;
    return next;
}


void
MSTrafficLightLogic::SwitchCommand::deschedule(MSTrafficLightLogic* tlLogic) {
    if (tlLogic == myTLLogic) {
        myAmValid = false;
        myAssumedNextSwitch = -1;
    }
}


/* -------------------------------------------------------------------------
 * member method definitions
 * ----------------------------------------------------------------------- */
MSTrafficLightLogic::MSTrafficLightLogic(MSTLLogicControl& tlcontrol, const std::string& id,
        const std::string& programID, SUMOTime delay, const std::map<std::string, std::string>& parameters) :
    Named(id), Parameterised(parameters),
    myProgramID(programID),
    myCurrentDurationIncrement(-1),
    myDefaultCycleTime(0) {
    mySwitchCommand = new SwitchCommand(tlcontrol, this, delay);
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
        mySwitchCommand, delay, MSEventControl::NO_CHANGE);
}


void
MSTrafficLightLogic::init(NLDetectorBuilder&) {
    const Phases& phases = getPhases();
    if (phases.size() > 1) {
        // warn about transistions from green to red without intermediate yellow
        for (int i = 0; i < (int)phases.size(); ++i) {
            const int iNext = (i + 1) % phases.size();
            const std::string& state1 = phases[i]->getState();
            const std::string& state2 = phases[iNext]->getState();
            assert(state1.size() == state2.size());
            for (int j = 0; j < (int)MIN2(state1.size(), state2.size()); ++j) {
                if ((LinkState)state2[j] == LINKSTATE_TL_RED
                        && ((LinkState)state1[j] == LINKSTATE_TL_GREEN_MAJOR
                            || (LinkState)state1[j] == LINKSTATE_TL_GREEN_MINOR)) {
                    for (LaneVector::const_iterator it = myLanes[j].begin(); it != myLanes[j].end(); ++it) {
                        if ((*it)->getPermissions() != SVC_PEDESTRIAN) {
                            WRITE_WARNING("Missing yellow phase in tlLogic '" + getID()
                                          + "', program '" + getProgramID() + "' for tl-index " + toString(j)
                                          + " when switching to phase " + toString(iNext));
                            return; // one warning per program is enough
                        }
                    }
                }
            }
        }
    }
}


MSTrafficLightLogic::~MSTrafficLightLogic() {
    // no need to do something about mySwitchCommand here,
    // it is handled by the event control
}


// ----------- Handling of controlled links
void
MSTrafficLightLogic::addLink(MSLink* link, MSLane* lane, unsigned int pos) {
    // !!! should be done within the loader (checking necessary)
    myLinks.reserve(pos + 1);
    while (myLinks.size() <= pos) {
        myLinks.push_back(LinkVector());
    }
    myLinks[pos].push_back(link);
    //
    myLanes.reserve(pos + 1);
    while (myLanes.size() <= pos) {
        myLanes.push_back(LaneVector());
    }
    myLanes[pos].push_back(lane);
    link->setTLState((LinkState) getCurrentPhaseDef().getState()[pos], MSNet::getInstance()->getCurrentTimeStep());
}


void
MSTrafficLightLogic::adaptLinkInformationFrom(const MSTrafficLightLogic& logic) {
    myLinks = logic.myLinks;
    myLanes = logic.myLanes;
}


std::map<MSLink*, LinkState>
MSTrafficLightLogic::collectLinkStates() const {
    std::map<MSLink*, LinkState> ret;
    for (LinkVectorVector::const_iterator i1 = myLinks.begin(); i1 != myLinks.end(); ++i1) {
        const LinkVector& l = (*i1);
        for (LinkVector::const_iterator i2 = l.begin(); i2 != l.end(); ++i2) {
            ret[*i2] = (*i2)->getState();
        }
    }
    return ret;
}


bool
MSTrafficLightLogic::setTrafficLightSignals(SUMOTime t) const {
    // get the current traffic light signal combination
    const std::string& state = getCurrentPhaseDef().getState();
    // go through the links
    for (size_t i = 0; i < myLinks.size(); i++) {
        const LinkVector& currGroup = myLinks[i];
        LinkState ls = (LinkState) state[i];
        for (LinkVector::const_iterator j = currGroup.begin(); j != currGroup.end(); j++) {
            (*j)->setTLState(ls, t);
        }
    }
    return true;
}


void
MSTrafficLightLogic::resetLinkStates(const std::map<MSLink*, LinkState>& vals) const {
    for (LinkVectorVector::const_iterator i1 = myLinks.begin(); i1 != myLinks.end(); ++i1) {
        const LinkVector& l = (*i1);
        for (LinkVector::const_iterator i2 = l.begin(); i2 != l.end(); ++i2) {
            assert(vals.find(*i2) != vals.end());
            (*i2)->setTLState(vals.find(*i2)->second, MSNet::getInstance()->getCurrentTimeStep());
        }
    }
}


// ----------- Static Information Retrieval
int
MSTrafficLightLogic::getLinkIndex(const MSLink* const link) const {
    int index = 0;
    for (LinkVectorVector::const_iterator i1 = myLinks.begin(); i1 != myLinks.end(); ++i1, ++index) {
        const LinkVector& l = (*i1);
        for (LinkVector::const_iterator i2 = l.begin(); i2 != l.end(); ++i2) {
            if ((*i2) == link) {
                return index;
            }
        }
    }
    return -1;
}



// ----------- Dynamic Information Retrieval
SUMOTime
MSTrafficLightLogic::getNextSwitchTime() const {
    return mySwitchCommand != 0 ? mySwitchCommand->getNextSwitchTime() : -1;
}


// ----------- Changing phases and phase durations
void
MSTrafficLightLogic::addOverridingDuration(SUMOTime duration) {
    myOverridingTimes.push_back(duration);
}


void
MSTrafficLightLogic::setCurrentDurationIncrement(SUMOTime delay) {
    myCurrentDurationIncrement = delay;
}


/****************************************************************************/

