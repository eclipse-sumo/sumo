/****************************************************************************/
/// @file    NBJoinedEdgesMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id: $
///
//
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
#pragma warning(disable: 4503)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member variables
// ===========================================================================
NBJoinedEdgesMap gJoinedEdges;


// ===========================================================================
// member method definitions
// ===========================================================================
NBJoinedEdgesMap::NBJoinedEdgesMap()
{}


NBJoinedEdgesMap::~NBJoinedEdgesMap()
{}


void
NBJoinedEdgesMap::init(NBEdgeCont &ec)
{
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
NBJoinedEdgesMap::appended(const std::string &to, const std::string &what)
{
    copy(myMap[what].begin(), myMap[what].end(),
         back_inserter(myMap[to]));
//    myMap[to].push_back(what);
    JoinedEdgesMap::iterator i = myMap.find(what);
    assert(i!=myMap.end());
    myMap.erase(i);
}


std::ostream &
operator<<(std::ostream &os, const NBJoinedEdgesMap &jemap)
{
    NBJoinedEdgesMap::JoinedEdgesMap::const_iterator i;
    for (i=jemap.myMap.begin(); i!=jemap.myMap.end(); ++i) {
        os << (*i).first << "\t";
        const NBJoinedEdgesMap::MappedEdgesVector &e = (*i).second;
        for (NBJoinedEdgesMap::MappedEdgesVector::const_iterator j=e.begin(); j!=e.end(); ++j) {
            os << (*j) << ":" << jemap.myLengths.find(*j)->second << "\t";
        }
        os << endl;
    }
    return os;
}



/****************************************************************************/

