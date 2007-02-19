/****************************************************************************/
/// @file    MSJunction.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junctions.
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

#include "MSJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class declarations
// ===========================================================================
class MSLink;


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
MSJunction::DictType MSJunction::myDict;


// ===========================================================================
// member method definition
// ===========================================================================
MSJunction::MSJunction(std::string id, const Position2D &position)
        : myID(id), myPosition(position)
{}

//-------------------------------------------------------------------------//

MSJunction::~MSJunction()
{}

//-------------------------------------------------------------------------//

bool
MSJunction::dictionary(string id, MSJunction* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------//

MSJunction*
MSJunction::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

//-------------------------------------------------------------------------//

void
MSJunction::clear()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}

//-------------------------------------------------------------------------//

std::vector<std::string>
MSJunction::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSJunction::myDict.size());
    for (MSJunction::DictType::iterator i=MSJunction::myDict.begin();
            i!=MSJunction::myDict.end(); i++) {
        ret.push_back((*i).first);
    }
    return ret;
}

//-------------------------------------------------------------------------//

const Position2D &
MSJunction::getPosition() const
{
    return myPosition;
}

//-------------------------------------------------------------------------//

void
MSJunction::postloadInit()
{}

//-------------------------------------------------------------------------//

void
MSJunction::postloadInitContainer()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->postloadInit();
    }
}


const std::string &
MSJunction::getID() const
{
    return myID;
}


GUIJunctionWrapper *
MSJunction::buildJunctionWrapper(GUIGlObjectStorage &)
{
    return 0;//!!!
}

size_t
MSJunction::dictSize()
{
    return myDict.size();
}



/****************************************************************************/

