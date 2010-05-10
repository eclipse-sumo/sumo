/****************************************************************************/
/// @file    MSTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The parent class for traffic light logics
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
MSTrafficLightLogic::SwitchCommand::SwitchCommand(MSTLLogicControl &tlcontrol,
        MSTrafficLightLogic *tlLogic, SUMOTime nextSwitch) throw()
        : myTLControl(tlcontrol), myTLLogic(tlLogic),
        myAssumedNextSwitch(nextSwitch), myAmValid(true) {}


MSTrafficLightLogic::SwitchCommand::~SwitchCommand() throw() {}



SUMOTime
MSTrafficLightLogic::SwitchCommand::execute(SUMOTime) throw(ProcessError) {
    // check whether this command has been descheduled
    if (!myAmValid) {
        return 0;
    }
    //
    bool isActive = myTLControl.isActive(myTLLogic);
    size_t step1 = myTLLogic->getCurrentPhaseIndex();
    SUMOTime next = myTLLogic->trySwitch(isActive);
    size_t step2 = myTLLogic->getCurrentPhaseIndex();
    if (step1!=step2) {
        if (isActive) {
            // execute any action connected to this tls
            const MSTLLogicControl::TLSLogicVariants &vars = myTLControl.get(myTLLogic->getID());
            // set link priorities
            myTLLogic->setLinkPriorities();
            // execute switch actions
            vars.executeOnSwitchActions();
        }
    }
    myAssumedNextSwitch += next;
    return next;
}


void
MSTrafficLightLogic::SwitchCommand::deschedule(MSTrafficLightLogic *tlLogic) throw() {
    if (tlLogic==myTLLogic) {
        myAmValid = false;
        myAssumedNextSwitch = -1;
    }
}


/* -------------------------------------------------------------------------
 * member method definitions
 * ----------------------------------------------------------------------- */
MSTrafficLightLogic::MSTrafficLightLogic(MSTLLogicControl &tlcontrol,
        const std::string &id, const std::string &programID,
        SUMOTime delay) throw()
        : myID(id), myProgramID(programID), myCurrentDurationIncrement(-1),
        myDefaultCycleTime(0) {
    mySwitchCommand = new SwitchCommand(tlcontrol, this, delay);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        mySwitchCommand, delay, MSEventControl::NO_CHANGE);
}


void
MSTrafficLightLogic::init(NLDetectorBuilder &) throw(ProcessError) {
}


MSTrafficLightLogic::~MSTrafficLightLogic() throw() {
    mySwitchCommand->deschedule(this);
}


// ----------- Handling of controlled links
void
MSTrafficLightLogic::addLink(MSLink *link, MSLane *lane, unsigned int pos) throw() {
    // !!! should be done within the loader (checking necessary)
    myLinks.reserve(pos+1);
    while (myLinks.size()<=pos) {
        myLinks.push_back(LinkVector());
    }
    myLinks[pos].push_back(link);
    //
    myLanes.reserve(pos+1);
    while (myLanes.size()<=pos) {
        myLanes.push_back(LaneVector());
    }
    myLanes[pos].push_back(lane);
}


void
MSTrafficLightLogic::adaptLinkInformationFrom(const MSTrafficLightLogic &logic) throw() {
    myLinks = logic.myLinks;
    myLanes = logic.myLanes;
}


std::map<MSLink*, std::pair<MSLink::LinkState, bool> >
MSTrafficLightLogic::collectLinkStates() const throw() {
    std::map<MSLink*, std::pair<MSLink::LinkState, bool> > ret;
    for (LinkVectorVector::const_iterator i1=myLinks.begin(); i1!=myLinks.end(); ++i1) {
        const LinkVector &l = (*i1);
        for (LinkVector::const_iterator i2=l.begin(); i2!=l.end(); ++i2) {
            ret[*i2] = std::make_pair((*i2)->getState(), (*i2)->havePriority());
        }
    }
    return ret;
}


void
MSTrafficLightLogic::resetLinkStates(const std::map<MSLink*, std::pair<MSLink::LinkState, bool> > &vals) const throw() {
    for (LinkVectorVector::const_iterator i1=myLinks.begin(); i1!=myLinks.end(); ++i1) {
        const LinkVector &l = (*i1);
        for (LinkVector::const_iterator i2=l.begin(); i2!=l.end(); ++i2) {
            assert(vals.find(*i2)!=vals.end());
            const std::pair<MSLink::LinkState, bool> &lvals = vals.find(*i2)->second;
            (*i2)->setTLState(lvals.first);
            (*i2)->setPriority(lvals.second);
        }
    }
}


// ----------- Static Information Retrieval
int
MSTrafficLightLogic::getLinkIndex(const MSLink * const link) const throw() {
    int index = 0;
    for (LinkVectorVector::const_iterator i1=myLinks.begin(); i1!=myLinks.end(); ++i1, ++index) {
        const LinkVector &l = (*i1);
        for (LinkVector::const_iterator i2=l.begin(); i2!=l.end(); ++i2) {
            if ((*i2)==link) {
                return index;
            }
        }
    }
    return -1;
}



// ----------- Dynamic Information Retrieval
SUMOTime
MSTrafficLightLogic::getNextSwitchTime() const throw() {
    return mySwitchCommand!=0 ? mySwitchCommand->getNextSwitchTime() : -1;
}


// ----------- Changing phases and phase durations
void
MSTrafficLightLogic::addOverridingDuration(SUMOTime duration) throw() {
    myOverridingTimes.push_back(duration);
}


void
MSTrafficLightLogic::setCurrentDurationIncrement(SUMOTime delay) throw() {
    myCurrentDurationIncrement = delay;
}




// ----------- Algorithm parameter handling
void
MSTrafficLightLogic::setParameter(const std::map<std::string, std::string> &params) throw() {
    myParameter = params;
}


std::string
MSTrafficLightLogic::getParameterValue(const std::string &key) const throw() {
    if (myParameter.find(key)==myParameter.end()) {
        return "";
    }
    return myParameter.find(key)->second;
}


/****************************************************************************/

