/****************************************************************************/
/// @file    NBTypeCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id: $
///
// A storage for the available types of an edge
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
#include <map>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBType.h"
#include "NBTypeCont.h"
#include "NBJunctionTypesMatrix.h"

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
NBTypeCont::NBTypeCont()
{}


NBTypeCont::~NBTypeCont()
{
    clear();
}


void
NBTypeCont::setDefaults(int defaultNoLanes,
                        SUMOReal defaultSpeed,
                        int defaultPriority)
{
    myDefaultNoLanes = defaultNoLanes;
    myDefaultSpeed = defaultSpeed;
    myDefaultPriority = defaultPriority;
}


bool
NBTypeCont::insert(NBType *type)
{
    TypesCont::iterator i = myTypes.find(type->myName);
    if (i!=myTypes.end()) {
        return false;
    }
    myTypes[type->myName] = type;
    return true;
}


int
NBTypeCont::getNoLanes(const string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return NBTypeCont::myDefaultNoLanes;
    }
    return (*i).second->myNoLanes;
}


SUMOReal
NBTypeCont::getSpeed(const string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return NBTypeCont::myDefaultSpeed;
    }
    return (*i).second->mySpeed;
}


NBEdge::EdgeBasicFunction
NBTypeCont::getFunction(const std::string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return NBEdge::EDGEFUNCTION_NORMAL;
    }
    return (*i).second->myFunction;
}


int
NBTypeCont::getPriority(const string &type)
{
    TypesCont::iterator i = myTypes.find(type);
    if (i==myTypes.end()) {
        return NBTypeCont::myDefaultPriority;
    }
    return (*i).second->myPriority;
}


int
NBTypeCont::getDefaultNoLanes()
{
    return NBTypeCont::myDefaultNoLanes;
}


int
NBTypeCont::getDefaultPriority()
{
    return NBTypeCont::myDefaultPriority;
}


SUMOReal
NBTypeCont::getDefaultSpeed()
{
    return NBTypeCont::myDefaultSpeed;
}


size_t
NBTypeCont::getNo()
{
    return myTypes.size();
}


NBNode::BasicNodeType
NBTypeCont::getJunctionType(int edgetype1, int edgetype2) const
{
    return myJunctionTypes.getType(edgetype1, edgetype2);
}


void
NBTypeCont::clear()
{
    for (TypesCont::iterator i=myTypes.begin(); i!=myTypes.end(); i++) {
        delete((*i).second);
    }
    myTypes.clear();
}


void
NBTypeCont::report()
{
    WRITE_MESSAGE("   " + toString<int>(getNo()) + " types loaded.");
}



/****************************************************************************/

