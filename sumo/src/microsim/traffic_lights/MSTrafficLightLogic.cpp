//---------------------------------------------------------------------------//
//                        MSTrafficLightLogic.cpp -
//  The parent class for traffic light logics
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.17  2006/12/21 13:23:55  dkrajzew
// added visualization of tls/junction link indices
//
// Revision 1.16  2006/11/16 10:50:45  dkrajzew
// warnings removed
//
// Revision 1.15  2006/10/25 12:22:41  dkrajzew
// updated
//
// Revision 1.14  2006/08/02 11:58:23  dkrajzew
// first try to make junctions tls-aware
//
// Revision 1.13  2006/05/15 06:01:51  dkrajzew
// added the possibility to stretch/change the current phase and consecutive phases
//
// Revision 1.13  2006/05/08 11:03:44  dkrajzew
// debugging: all structures now return their id via getID()
//
// Revision 1.12  2006/04/11 10:59:07  dkrajzew
// all structures now return their id via getID()
//
// Revision 1.11  2006/03/17 08:57:51  dkrajzew
// changed the Event-interface (execute now gets the current simulation time, event handlers are non-static)
//
// Revision 1.10  2006/03/08 13:13:20  dkrajzew
// debugging
//
// Revision 1.9  2006/02/27 12:06:17  dkrajzew
// parameter-API and raknet-support added
//
// Revision 1.8  2006/02/23 11:27:57  dkrajzew
// tls may have now several programs
//
// Revision 1.7  2005/11/09 06:36:48  dkrajzew
// changing the LSA-API: MSEdgeContinuation added; changed the calling API
//
// Revision 1.6  2005/10/10 11:56:09  dkrajzew
// reworking the tls-API: made tls-control non-static; made net an element of traffic lights
//
// Revision 1.5  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:09:53  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:22:19  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.1  2004/11/23 10:18:42  dkrajzew
// all traffic lights moved to microsim/traffic_lights
//
// Revision 1.12  2004/01/26 07:48:48  dkrajzew
// added the possibility to trigger detectors when switching
//
// Revision 1.11  2003/11/24 10:21:21  dkrajzew
// some documentation added and dead code removed
//
// Revision 1.10  2003/11/12 13:51:14  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.9  2003/09/24 13:28:55  dkrajzew
// retrival of lanes by the position within the bitset added
//
// Revision 1.8  2003/09/05 15:13:58  dkrajzew
// saving of tl-states implemented
//
// Revision 1.7  2003/08/04 11:42:35  dkrajzew
// missing deletion of traffic light logics on closing a network added
//
// Revision 1.6  2003/07/30 09:16:10  dkrajzew
// a better (correct?) processing of yellow lights added; debugging
//
// Revision 1.5  2003/06/06 10:39:17  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.4  2003/06/05 16:11:03  dkrajzew
// new usage of traffic lights implemented
//
// Revision 1.3  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;
#ifdef RAKNET_DEMO
Ampel *myAmpel = 0;
std::map<std::string, int> myIDs;
int myAmpelRunningID = 0;
#endif


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * member method definitions
 * ----------------------------------------------------------------------- */
MSTrafficLightLogic::SwitchCommand::SwitchCommand(MSTLLogicControl &tlcontrol,
                                                  MSTrafficLightLogic *tlLogic)
    : myTLControl(tlcontrol), myTLLogic(tlLogic), myAmValid(true)
{
}


MSTrafficLightLogic::SwitchCommand::~SwitchCommand()
{
}



SUMOTime
MSTrafficLightLogic::SwitchCommand::execute(SUMOTime )
{
    // check whether this command has been descheduled
    if(!myAmValid) {
        return 0;
    }
    //
    bool isActive = myTLControl.isActive(myTLLogic);
    size_t step1 = myTLLogic->getStepNo();
    SUMOTime next = myTLLogic->trySwitch(isActive);
    size_t step2 = myTLLogic->getStepNo();
    if(step1!=step2) {
        myTLLogic->onSwitch();
        if(isActive) {
            myTLLogic->setLinkPriorities();
        }
    }
    return next;
}


void
MSTrafficLightLogic::SwitchCommand::deschedule(MSTrafficLightLogic *tlLogic)
{
    if(tlLogic==myTLLogic) {
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
	if(myAmpel==0) {
		myAmpel = new Ampel();
	}
#endif
}


MSTrafficLightLogic::~MSTrafficLightLogic()
{
}


void
MSTrafficLightLogic::setLinkPriorities()
{
    const std::bitset<64> &linkPrios = linkPriorities();
    const std::bitset<64> &yMask = yellowMask();
    for(size_t i=0; i<myLinks.size(); i++) {
        const LinkVector &currGroup = myLinks[i];
        for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
            (*j)->setPriority(linkPrios.test(i), yMask.test(i));
        }
    }
}


void
MSTrafficLightLogic::addLink(MSLink *link, MSLane *lane, size_t pos)
{
    // !!! should be done within the loader (checking necessary)
    myLinks.reserve(pos+1);
    while(myLinks.size()<=pos) {
        myLinks.push_back(LinkVector());
    }
    myLinks[pos].push_back(link);
    //
    myLanes.reserve(pos+1);
    while(myLanes.size()<=pos) {
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
    for(size_t i=0; i<myLinks.size(); i++) {
        // mark out links having red
        if(!allowedLinks.test(i)&&!yellowLinks.test(i)) {
            const LinkVector &currGroup = myLinks[i];
            for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                (*j)->deleteRequest();
            }
        }
        // set the states for assigned links
        // !!! one should let the links ask for it
        if(!allowedLinks.test(i)) {
            if(yellowLinks.test(i)) {
                const LinkVector &currGroup = myLinks[i];
                for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    (*j)->setTLState(MSLink::LINKSTATE_TL_YELLOW);
                }
            } else {
                const LinkVector &currGroup = myLinks[i];
                for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    (*j)->setTLState(MSLink::LINKSTATE_TL_RED);
                }
            }
        } else {
            const LinkVector &currGroup = myLinks[i];
            for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
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
    for(size_t i=0; i<myLinks.size(); i++) {
        // mark out links having red
        if(!allowedLinks.test(i)) {
            const LinkVector &currGroup = myLinks[i];
            for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
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
    for(size_t i=0; i<myLinks.size(); i++) {
        if(yellowLinks.test(i)) {
            strm << "Y";
        } else {
            if(allowedLinks.test(i)) {
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
    for(std::vector<DiscreteCommand*>::iterator i=myOnSwitchActions.begin(); i!=myOnSwitchActions.end(); ) {
        if(!(*i)->execute()) {
            i = myOnSwitchActions.erase(i);
            // !!! delete???
        } else {
            i++;
        }
    }
#ifdef RAKNET_DEMO
    {
        // get the current traffic light signal combination
        const std::bitset<64> &allowedLinks = allowed();
        const std::bitset<64> &yellowLinks = yellowMask();
        // go through the links
        for(size_t i=0; i<myLinks.size(); i++) {
            // set the states for assigned links
            if(!allowedLinks.test(i)) {
                if(yellowLinks.test(i)) {
                    const LinkVector &currGroup = myLinks[i];
                    for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
					    myAmpel->setTrafficLightState(myIDs[getID()]*1000 + i, TRAFFIC_SIGN_YELLOW);
                    }
                } else {
                    const LinkVector &currGroup = myLinks[i];
                    for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
			    		myAmpel->setTrafficLightState(myIDs[getID()]*1000 + i, TRAFFIC_SIGN_RED);
                    }
                }
            } else {
                const LinkVector &currGroup = myLinks[i];
                for(LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
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
    if(myParameter.find(key)==myParameter.end()) {
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
    if(j!=0) {
//        j->rebuildPriorities();
    }
}


int
MSTrafficLightLogic::getLinkIndex(MSLink *link) const
{
    int index = 0;
    for(LinkVectorVector::const_iterator i1=myLinks.begin(); i1!=myLinks.end(); ++i1, ++index) {
        const LinkVector &l = (*i1);
        for(LinkVector::const_iterator i2=l.begin(); i2!=l.end(); ++i2) {
            if((*i2)==link) {
                return index;
            }
        }
    }
    return -1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


