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
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/MSEventControl.h>
#include <microsim/MSJunctionLogic.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include "MSTLLogicControl.h"
#include "MSTrafficLightLogic.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static value definitions
// ===========================================================================
const MSTrafficLightLogic::LaneVector MSTrafficLightLogic::myEmptyLaneVector;


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
    const bool isActive = myTLControl.isActive(myTLLogic);
    int step1 = myTLLogic->getCurrentPhaseIndex();
    SUMOTime next = myTLLogic->trySwitch();
    int step2 = myTLLogic->getCurrentPhaseIndex();
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
    if (phases.size() > 0 && MSGlobals::gMesoTLSPenalty > 0) {
        initMesoTLSPenalties();
    }
    if (phases.size() > 1) {
        bool haveWarnedAboutUnusedStates = false;
        std::vector<bool> foundGreen(phases.front()->getState().size(), false);
        for (int i = 0; i < (int)phases.size(); ++i) {
            // warn about unused stats
            const int iNext = (i + 1) % phases.size();
            const std::string& state1 = phases[i]->getState();
            const std::string& state2 = phases[iNext]->getState();
            assert(state1.size() == state2.size());
            if (!haveWarnedAboutUnusedStates && state1.size() > myLanes.size()) {
                WRITE_WARNING("Unused states in tlLogic '" + getID()
                              + "', program '" + getProgramID() + "' in phase " + toString(i)
                              + " after tl-index " + toString((int)myLanes.size() - 1));
                haveWarnedAboutUnusedStates = true;
            }
            // warn about transitions from green to red without intermediate yellow
            for (int j = 0; j < (int)MIN3(state1.size(), state2.size(), myLanes.size()); ++j) {
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
            // warn about links that never get the green light
            for (int j = 0; j < (int)state1.size(); ++j) {
                LinkState ls = (LinkState)state1[j];
                if (ls == LINKSTATE_TL_GREEN_MAJOR || ls == LINKSTATE_TL_GREEN_MINOR) {
                    foundGreen[j] = true;
                }
            }
        }
        for (int j = 0; j < (int)foundGreen.size(); ++j) {
            if (!foundGreen[j]) {
                WRITE_WARNING("Missing green phase in tlLogic '" + getID()
                              + "', program '" + getProgramID() + "' for tl-index " + toString(j));
                break;
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
MSTrafficLightLogic::addLink(MSLink* link, MSLane* lane, int pos) {
    // !!! should be done within the loader (checking necessary)
    myLinks.reserve(pos + 1);
    while ((int)myLinks.size() <= pos) {
        myLinks.push_back(LinkVector());
    }
    myLinks[pos].push_back(link);
    //
    myLanes.reserve(pos + 1);
    while ((int)myLanes.size() <= pos) {
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
    for (int i = 0; i < (int)myLinks.size(); i++) {
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


void MSTrafficLightLogic::initMesoTLSPenalties() {
    // set mesoscopic time penalties
    const Phases& phases = getPhases();
    const int numLinks = (int)myLinks.size();
    // warning already given if not all states are used
    assert(numLinks <= (int)phases.front()->getState().size());
    SUMOTime duration = 0;
    std::vector<SUMOReal> redDuration(numLinks, 0);
    std::vector<SUMOReal> totalRedDuration(numLinks, 0);
    std::vector<SUMOReal> penalty(numLinks, 0);
    for (int i = 0; i < (int)phases.size(); ++i) {
        const std::string& state = phases[i]->getState();
        duration += phases[i]->duration;
        // warn about transitions from green to red without intermediate yellow
        for (int j = 0; j < numLinks; ++j) {
            if ((LinkState)state[j] == LINKSTATE_TL_RED
                    || (LinkState)state[j] == LINKSTATE_TL_REDYELLOW) {
                redDuration[j] += STEPS2TIME(phases[i]->duration);
                totalRedDuration[j] += STEPS2TIME(phases[i]->duration);
            } else if (redDuration[j] > 0) {
                penalty[j] += 0.5 * (redDuration[j] * redDuration[j] + redDuration[j]);
                redDuration[j] = 0;
            }
        }
    }
    /// XXX penalty for wrap-around red phases is underestimated
    for (int j = 0; j < numLinks; ++j) {
        if (redDuration[j] > 0) {
            penalty[j] += 0.5 * (redDuration[j] * redDuration[j] + redDuration[j]);
            redDuration[j] = 0;
        }
    }
    const SUMOReal durationSeconds = STEPS2TIME(duration);
    std::set<const MSJunction*> controlledJunctions;
    for (int j = 0; j < numLinks; ++j) {
        for (int k = 0; k < (int)myLinks[j].size(); ++k) {
            myLinks[j][k]->setMesoTLSPenalty(TIME2STEPS(MSGlobals::gMesoTLSPenalty * penalty[j] / durationSeconds));
            myLinks[j][k]->setGreenFraction(MAX2((durationSeconds - MSGlobals::gMesoTLSPenalty * totalRedDuration[j]) / durationSeconds, NUMERICAL_EPS)); // avoid zero capacity (warning issued before)
            controlledJunctions.insert(myLinks[j][k]->getLane()->getEdge().getFromJunction()); // MSLink::myJunction is not yet initialized
            //std::cout << " tls=" << getID() << " i=" << j << " link=" << myLinks[j][k]->getViaLaneOrLane()->getID() << " penalty=" << penalty[j] / durationSeconds << " durSecs=" << durationSeconds << " greenTime=" << " gF=" << myLinks[j][k]->getGreenFraction() << "\n";
        }
    }
    // initialize empty-net travel times
    // XXX refactor after merging sharps (links know their incoming edge)
    for (std::set<const MSJunction*>::iterator it = controlledJunctions.begin(); it != controlledJunctions.end(); ++it) {
        const ConstMSEdgeVector incoming = (*it)->getIncoming();
        for (ConstMSEdgeVector::const_iterator it_e = incoming.begin(); it_e != incoming.end(); ++it_e) {
            const_cast<MSEdge*>(*it_e)->recalcCache();
        }
    }

}

/****************************************************************************/

