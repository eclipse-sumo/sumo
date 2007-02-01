/****************************************************************************/
/// @file    MSTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <microsim/MSLink.h>
#include <microsim/MSLane.h>
#include "MSTrafficLightLogic.h"
#include <microsim/MSEventControl.h>
#include "MSTLLogicControl.h"
#include <utils/helpers/DiscreteCommand.h>
#include <microsim/MSJunctionLogic.h>

#ifdef RAKNET_DEMO
#include <raknet_demo/sumo_add/ampel.h>
#include <raknet_demo/constants.h>
#include <utils/geom/Line2D.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
#ifdef RAKNET_DEMO
Ampel *myAmpel = 0;
std::map<std::string, int> myIDs;
int myAmpelRunningID = 0;
#endif


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * member method definitions
 * ----------------------------------------------------------------------- */
MSTrafficLightLogic::SwitchCommand::SwitchCommand(MSTLLogicControl &tlcontrol,
        MSTrafficLightLogic *tlLogic)
        : myTLControl(tlcontrol), myTLLogic(tlLogic), myAmValid(true)
{}


MSTrafficLightLogic::SwitchCommand::~SwitchCommand()
{}



SUMOTime
MSTrafficLightLogic::SwitchCommand::execute(SUMOTime)
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
        myTLLogic->onSwitch();
        if (isActive) {
            myTLLogic->setLinkPriorities();
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
MSTrafficLightLogic::MSTrafficLightLogic(MSNet &net,
        MSTLLogicControl &tlcontrol,
        const std::string &id,
        const std::string &subid,
        size_t delay)
        : myID(id), mySubID(subid), myNet(net), myCurrentDurationIncrement(-1)
{
    mySwitchCommand = new SwitchCommand(tlcontrol, this);
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        mySwitchCommand, delay, MSEventControl::ADAPT_AFTER_EXECUTION);
#ifdef RAKNET_DEMO
    if (myAmpel==0) {
        myAmpel = new Ampel();
    }
#endif
}


MSTrafficLightLogic::~MSTrafficLightLogic()
{}


void
MSTrafficLightLogic::setLinkPriorities()
{
    const std::bitset<64> &linkPrios = linkPriorities();
    const std::bitset<64> &yMask = yellowMask();
    for (size_t i=0; i<myLinks.size(); i++) {
        const LinkVector &currGroup = myLinks[i];
        for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
            (*j)->setPriority(linkPrios.test(i), yMask.test(i));
        }
    }
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

#ifdef RAKNET_DEMO
    myAmpel->addTrafficLight(myIDs[getID()]*1000 + pos,
                             lane->getShape()[-1].x(), 0, lane->getShape()[-1].y(), lane->getShape().getEndLine().atan2DegreeAngle());
#endif
}


bool
MSTrafficLightLogic::maskRedLinks()
{
    // get the current traffic light signal combination
    const std::bitset<64> &allowedLinks = allowed();
    const std::bitset<64> &yellowLinks = yellowMask();
    // go through the links
    for (size_t i=0; i<myLinks.size(); i++) {
        // mark out links having red
        if (!allowedLinks.test(i)&&!yellowLinks.test(i)) {
            const LinkVector &currGroup = myLinks[i];
            for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                (*j)->deleteRequest();
            }
        }
        // set the states for assigned links
        // !!! one should let the links ask for it
        if (!allowedLinks.test(i)) {
            if (yellowLinks.test(i)) {
                const LinkVector &currGroup = myLinks[i];
                for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    (*j)->setTLState(MSLink::LINKSTATE_TL_YELLOW);
                }
            } else {
                const LinkVector &currGroup = myLinks[i];
                for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    (*j)->setTLState(MSLink::LINKSTATE_TL_RED);
                }
            }
        } else {
            const LinkVector &currGroup = myLinks[i];
            for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                (*j)->setTLState(MSLink::LINKSTATE_TL_GREEN);
            }
        }
    }
    return true;
}


bool
MSTrafficLightLogic::maskYellowLinks()
{
    // get the current traffic light signal combination
    const std::bitset<64> &allowedLinks = allowed();
    // go through the links
    for (size_t i=0; i<myLinks.size(); i++) {
        // mark out links having red
        if (!allowedLinks.test(i)) {
            const LinkVector &currGroup = myLinks[i];
            for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                (*j)->deleteRequest();
            }
        }
    }
    return true;
}


std::string
MSTrafficLightLogic::buildStateList() const
{
    std::ostringstream strm;
    const std::bitset<64> &allowedLinks = allowed();
    const std::bitset<64> &yellowLinks = yellowMask();
    for (size_t i=0; i<myLinks.size(); i++) {
        if (yellowLinks.test(i)) {
            strm << "Y";
        } else {
            if (allowedLinks.test(i)) {
                strm << "G";
            } else {
                strm << "R";
            }
        }
    }
    return strm.str();
}


const MSTrafficLightLogic::LaneVector &
MSTrafficLightLogic::getLanesAt(size_t i) const
{
    return myLanes[i];
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
MSTrafficLightLogic::addSwitchAction(DiscreteCommand *a)
{
    myOnSwitchActions.push_back(a);
}


void
MSTrafficLightLogic::onSwitch()
{
    for (std::vector<DiscreteCommand*>::iterator i=myOnSwitchActions.begin(); i!=myOnSwitchActions.end();) {
        if (!(*i)->execute()) {
            i = myOnSwitchActions.erase(i);
            // !!! delete???
        } else {
            i++;
        }
    }
#ifdef RAKNET_DEMO {
    // get the current traffic light signal combination
    const std::bitset<64> &allowedLinks = allowed();
    const std::bitset<64> &yellowLinks = yellowMask();
    // go through the links
    for (size_t i=0; i<myLinks.size(); i++) {
        // set the states for assigned links
        if (!allowedLinks.test(i)) {
            if (yellowLinks.test(i)) {
                const LinkVector &currGroup = myLinks[i];
                for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    myAmpel->setTrafficLightState(myIDs[getID()]*1000 + i, TRAFFIC_SIGN_YELLOW);
                }
            } else {
                const LinkVector &currGroup = myLinks[i];
                for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    myAmpel->setTrafficLightState(myIDs[getID()]*1000 + i, TRAFFIC_SIGN_RED);
                }
            }
        } else {
            const LinkVector &currGroup = myLinks[i];
            for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                myAmpel->setTrafficLightState(myIDs[getID()]*1000 + i, TRAFFIC_SIGN_GREEN);
            }
        }
    }
}
#endif
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


void
MSTrafficLightLogic::init(NLDetectorBuilder &,
                          const MSEdgeContinuations &/*edgeContinuations*/)
{
    MSJunction *j = MSJunction::dictionary(getID());
    if (j!=0) {
//        j->rebuildPriorities();
    }
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



/****************************************************************************/

