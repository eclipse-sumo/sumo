/****************************************************************************/
/// @file    NBJoinedEdgesMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//  »missingDescription«
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <iostream>
#include "NBJoinedEdgesMap.h"
#include "NBEdgeCont.h"
#include "NBEdge.h"
#include <algorithm>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member variables
// ===========================================================================
NBJoinedEdgesMap gJoinedEdges;


// ===========================================================================
// member method definitions
// ===========================================================================
NBJoinedEdgesMap::NBJoinedEdgesMap() {}


NBJoinedEdgesMap::~NBJoinedEdgesMap() {}


void
NBJoinedEdgesMap::init(NBEdgeCont &ec) {
    const std::vector<std::string> edgeNames = ec.getAllNames();
    myMap.clear();
    for (std::vector<std::string>::const_iterator i=edgeNames.begin(); i!=edgeNames.end(); i++) {
        MappedEdgesVector e;
        e.push_back(*i);
        myMap[*i] = e;
        myLengths[*i] = ec.retrieve(*i)->getLength();
    }
}


void
NBJoinedEdgesMap::appended(const std::string &to, const std::string &what) {
    copy(myMap[what].begin(), myMap[what].end(),
         back_inserter(myMap[to]));
//    myMap[to].push_back(what);
    JoinedEdgesMap::iterator i = myMap.find(what);
    assert(i!=myMap.end());
    myMap.erase(i);
}


std::ostream &
operator<<(std::ostream &os, const NBJoinedEdgesMap &jemap) {
    NBJoinedEdgesMap::JoinedEdgesMap::const_iterator i;
    for (i=jemap.myMap.begin(); i!=jemap.myMap.end(); ++i) {
        os << (*i).first << "\t";
        const NBJoinedEdgesMap::MappedEdgesVector &e = (*i).second;
        for (NBJoinedEdgesMap::MappedEdgesVector::const_iterator j=e.begin(); j!=e.end(); ++j) {
            os << (*j) << ":" << jemap.myLengths.find(*j)->second << "\t";
        }
        os << std::endl;
    }
    return os;
}



/****************************************************************************/

