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
#include <map>
#include "MSLink.h"
#include "MSLane.h"
#include "MSTrafficLightLogic.h"
#include "MSEventControl.h"


MSTrafficLightLogic::DictType MSTrafficLightLogic::_dict;

MSTrafficLightLogic::MSTrafficLightLogic(const std::string &id,
                                         MSEventControl &ec, size_t delay)
    : _id(id)
{
    ec.addEvent(new SwitchCommand(this), delay,
        MSEventControl::ADAPT_AFTER_EXECUTION);
}


MSTrafficLightLogic::~MSTrafficLightLogic()
{
}



void
MSTrafficLightLogic::setLinkPriorities()
{
//    size_t pos = 0;
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
MSTrafficLightLogic::addLink(MSLink *link, size_t pos)
{
    // !!! should be done within the loader (checking necessary)
    myLinks.reserve(pos+1);
    while(myLinks.size()<=pos) {
        myLinks.push_back(LinkVector());
    }
    myLinks[pos].push_back(link);
}


void
MSTrafficLightLogic::maskRedLinks()
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




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


