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
#include "MSLane.h"
#include "MSTrafficLightLogic.h"


MSTrafficLightLogic::DictType MSTrafficLightLogic::_dict;

MSTrafficLightLogic::MSTrafficLightLogic(const std::string &id)
    : _id(id)
{
}


MSTrafficLightLogic::~MSTrafficLightLogic()
{
}



void
MSTrafficLightLogic::setLinkPriorities(MSLogicJunction::InLaneCont &inLanes)
{
    size_t pos = 0;
    const std::bitset<64> &linkPrios = linkPriorities();
    for(MSLogicJunction::InLaneCont::iterator i=inLanes.begin(); i!=inLanes.end(); i++) {
        (*i).myLane->setLinkPriorities(linkPrios, pos);
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


