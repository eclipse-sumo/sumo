/****************************************************************************/
/// @file    NIVissimClosures.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/VectorHelper.h>
#include "NIVissimClosures.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


NIVissimClosures::DictType NIVissimClosures::myDict;

NIVissimClosures::NIVissimClosures(const std::string &id,
                                   int from_node, int to_node,
                                   IntVector &overEdges)
        : myID(id), myFromNode(from_node), myToNode(to_node),
        myOverEdges(overEdges)
{}


NIVissimClosures::~NIVissimClosures()
{}


bool
NIVissimClosures::dictionary(const std::string &id,
                             int from_node, int to_node,
                             IntVector &overEdges)
{
    NIVissimClosures *o = new NIVissimClosures(id, from_node, to_node,
            overEdges);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimClosures::dictionary(const std::string &name, NIVissimClosures *o)
{
    DictType::iterator i=myDict.find(name);
    if (i==myDict.end()) {
        myDict[name] = o;
        return true;
    }
    return false;
}


NIVissimClosures *
NIVissimClosures::dictionary(const std::string &name)
{
    DictType::iterator i=myDict.find(name);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}



void
NIVissimClosures::clearDict()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}



/****************************************************************************/

