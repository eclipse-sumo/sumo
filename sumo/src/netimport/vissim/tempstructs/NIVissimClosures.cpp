/****************************************************************************/
/// @file    NIVissimClosures.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// -------------------
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
#include <utils/common/IntVector.h>
#include "NIVissimClosures.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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

