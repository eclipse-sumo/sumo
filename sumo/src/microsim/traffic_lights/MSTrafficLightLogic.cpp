/****************************************************************************/
/// @file    MSTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The parent class for traffic light logics
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * member method definitions
 * ----------------------------------------------------------------------- */
MSTrafficLightLogic::SwitchCommand::SwitchCommand(MSTLLogicControl &tlcontrol,
        MSTrafficLightLogic *tlLogic) throw()
        : myTLControl(tlcontrol), myTLLogic(tlLogic), myAmValid(true)
{}


MSTrafficLightLogic::SwitchCommand::~SwitchCommand() throw()
{}



SUMOTime
MSTrafficLightLogic::SwitchCommand::execute(SUMOTime) throw(ProcessError)
{
    // check whether this command has been descheduled
    if (!myAmValid) {
        return 0;
    }
    //
    bool isActive = myTLControl.isActive(myTLLogic);
    size_t step1 = myTLLogic->getStepNo();
    SUMOTime next = myTLLogic->trySwitch(isActive);
    size_t step2 = myTLLogic->getStepNo();
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
    return next;
}


void
MSTrafficLightLogic::SwitchCommand::deschedule(MSTrafficLightLogic *tlLogic)
{
    if (tlLogic==myTLLogic) {
        myAmValid = false;
    }
}


/* -------------------------------------------------------------------------
 * member method definitions
 * ----------------------------------------------------------------------- */
MSTrafficLightLogic::MSTrafficLightLogic(
    MSTLLogicControl &tlcontrol,
    const std::string &id,
    const std::string &subid,
    SUMOTime delay)
        : myID(id), mySubID(subid), myCurrentDurationIncrement(-1)
{
    mySwitchCommand = new SwitchCommand(tlcontrol, this);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        mySwitchCommand, delay, MSEventControl::ADAPT_AFTER_EXECUTION);
}


void 
MSTrafficLightLogic::init(NLDetectorBuilder &nb,
                          const MSEdgeContinuations &edgeContinuations)
{
}


MSTrafficLightLogic::~MSTrafficLightLogic()
{
}


void
MSTrafficLightLogic::addLink(MSLink *link, MSLane *lane, size_t pos)
{
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


const MSTrafficLightLogic::LaneVector &
MSTrafficLightLogic::getLanesAt(size_t i) const
{
    return myLanes[i];
}


const MSTrafficLightLogic::LaneVectorVector &
MSTrafficLightLogic::getLanes() const
{
    return myLanes;
}


const MSTrafficLightLogic::LinkVector &
MSTrafficLightLogic::getLinksAt(size_t i) const
{
    return myLinks[i];
}


const std::string &
MSTrafficLightLogic::getID() const
{
    return myID;
}


const std::string &
MSTrafficLightLogic::getSubID() const
{
    return mySubID;
}


const MSTrafficLightLogic::LinkVectorVector &
MSTrafficLightLogic::getLinks() const
{
    return myLinks;
}


void
MSTrafficLightLogic::adaptLinkInformationFrom(const MSTrafficLightLogic &logic)
{
    myLinks = logic.myLinks;
    myLanes = logic.myLanes;
}


void
MSTrafficLightLogic::setParameter(const std::map<std::string, std::string> &params)
{
    myParameter = params;
}


std::string
MSTrafficLightLogic::getParameterValue(const std::string &key) const
{
    if (myParameter.find(key)==myParameter.end()) {
        return "";
    }
    return myParameter.find(key)->second;
}


void
MSTrafficLightLogic::addOverridingDuration(SUMOTime duration)
{
    myOverridingTimes.push_back(duration);
}


void
MSTrafficLightLogic::setCurrentDurationIncrement(SUMOTime delay)
{
    myCurrentDurationIncrement = delay;
}


int
MSTrafficLightLogic::getLinkIndex(MSLink *link) const
{
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


std::map<MSLink*, std::pair<MSLink::LinkState, bool> >
MSTrafficLightLogic::collectLinkStates() const
{
    std::map<MSLink*, std::pair<MSLink::LinkState, bool> > ret;
    for (LinkVectorVector::const_iterator i1=myLinks.begin(); i1!=myLinks.end(); ++i1) {
        const LinkVector &l = (*i1);
        for (LinkVector::const_iterator i2=l.begin(); i2!=l.end(); ++i2) {
            ret[*i2] = make_pair((*i2)->getState(), (*i2)->havePriority());
        }
    }
    return ret;
}


void
MSTrafficLightLogic::resetLinkStates(const std::map<MSLink*, std::pair<MSLink::LinkState, bool> > &vals) const
{
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

/****************************************************************************/

