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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include "MSLink.h"
#include "MSLane.h"
#include "MSTrafficLightLogic.h"
#include "MSEventControl.h"
#include <helpers/DiscreteCommand.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variable definitions
 * ======================================================================= */
MSTrafficLightLogic::DictType MSTrafficLightLogic::_dict;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSTrafficLightLogic::MSTrafficLightLogic(const std::string &id,  size_t delay)
    : _id(id)
{
    MSEventControl::getBeginOfTimestepEvents()->addEvent(
        new SwitchCommand(this), delay, MSEventControl::ADAPT_AFTER_EXECUTION);
}


MSTrafficLightLogic::~MSTrafficLightLogic()
{
    myLinks.clear();
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


bool
MSTrafficLightLogic::dictionary(const std::string &name,
                                MSTrafficLightLogic *logic)
{
    if(_dict.find(name)==_dict.end()) {
        _dict[name] = logic;
        return true;
    }
    return false;
}


MSTrafficLightLogic *
MSTrafficLightLogic::dictionary(const std::string &name)
{
    DictType::iterator i = _dict.find(name);
    if(i==_dict.end()) {
        return 0;
    }
    return (*i).second;
}


void
MSTrafficLightLogic::clear()
{
    // they are destroyed within the control
    //  !!! check deletion consistency
    _dict.clear();
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
}


void
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
}


void
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


std::vector<MSTrafficLightLogic*>
MSTrafficLightLogic::getList()
{
    std::vector<MSTrafficLightLogic*> ret;
    ret.reserve(_dict.size());
    for(DictType::iterator i=_dict.begin(); i!=_dict.end(); i++) {
        ret.push_back((*i).second);
    }
    return ret;
}


const std::string &
MSTrafficLightLogic::id() const
{
    return _id;
}


const MSTrafficLightLogic::LinkVectorVector &
MSTrafficLightLogic::getLinks() const
{
    return myLinks;
}


void
MSTrafficLightLogic::addSwitchAction(DiscreteCommand *a)
{
    mySwitchCommands.push_back(a);
}


void
MSTrafficLightLogic::onSwitch()
{
    for(std::vector<DiscreteCommand*>::iterator i=mySwitchCommands.begin(); i!=mySwitchCommands.end(); ) {
        if(!(*i)->execute()) {
            i = mySwitchCommands.erase(i);
        } else {
            i++;
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


