/****************************************************************************/
/// @file    NBTrafficLightLogicVector.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A vector of traffic lights logics
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

#include <vector>
#include <iostream>
#include <set>
#include <cassert>
#include <utils/common/ToString.h>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NBTrafficLightLogicVector::NBTrafficLightLogicVector(
    const NBConnectionVector &inLanes, std::string type)
        : myInLinks(inLanes), myType(type)
{}

NBTrafficLightLogicVector::~NBTrafficLightLogicVector()
{
    for (LogicVector::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete(*i);
    }
}


void
NBTrafficLightLogicVector::add(NBTrafficLightLogic *logic)
{
    if (logic==0) {
        return;
    }
    if (!contains(logic)) {
        _cont.push_back(logic);
    } else {
        delete logic;
    }
}


void
NBTrafficLightLogicVector::add(const NBTrafficLightLogicVector &cont)
{
    for (LogicVector::const_iterator i=cont._cont.begin(); i!=cont._cont.end(); i++) {
        if (!contains(*i)) {
            NBTrafficLightLogic *logic = *i;
            add(new NBTrafficLightLogic(*logic));
        }
    }
}


void
NBTrafficLightLogicVector::writeXML(std::ostream &os) const
{
    SUMOReal distance = 250;
    set<string> inLanes;
    for (NBConnectionVector::const_iterator j=myInLinks.begin(); j!=myInLinks.end(); j++) {
        assert((*j).getFromLane()>=0&&(*j).getFrom()!=0);
        string id = (*j).getFrom()->getID() + "_" + toString<int>((*j).getFromLane());
        inLanes.insert(id);
    }
    size_t pos = 0;
    (*(_cont.begin()))->writeXML(os, pos++, distance, myType, inLanes);
//    for(LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
//        (*i)->writeXML(os, pos++, distance, myType, inLanes);
//    }
}

bool
NBTrafficLightLogicVector::contains(NBTrafficLightLogic *logic) const
{
    for (LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
        if ((*i)->equals(*logic)) {
            return true;
        }
    }
    return false;
}


int
NBTrafficLightLogicVector::size() const
{
    return _cont.size();
}


const std::string &
NBTrafficLightLogicVector::getType() const
{
    return myType;
}



/****************************************************************************/

