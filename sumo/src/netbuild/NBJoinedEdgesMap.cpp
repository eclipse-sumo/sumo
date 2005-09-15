//---------------------------------------------------------------------------//
//                        NBJoinedEdgesMap.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.4  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:32:47  dkrajzew
// code style adapted; guessing of ramps and unregulated near districts implemented; debugging
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <cassert>
#include <iostream>
#include "NBJoinedEdgesMap.h"
#include "NBEdgeCont.h"
#include "NBEdge.h"
#include <algorithm>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member variables
 * ======================================================================= */
NBJoinedEdgesMap gJoinedEdges;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
NBJoinedEdgesMap::NBJoinedEdgesMap()
{
}


NBJoinedEdgesMap::~NBJoinedEdgesMap()
{
}


void
NBJoinedEdgesMap::init(NBEdgeCont &ec)
{
    const std::vector<std::string> edgeNames = ec.getAllNames();
    myMap.clear();
    for(std::vector<std::string>::const_iterator i=edgeNames.begin(); i!=edgeNames.end(); i++) {
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
    for(i=jemap.myMap.begin(); i!=jemap.myMap.end(); ++i) {
        os << (*i).first << "\t";
        const NBJoinedEdgesMap::MappedEdgesVector &e = (*i).second;
        for(NBJoinedEdgesMap::MappedEdgesVector::const_iterator j=e.begin(); j!=e.end(); ++j) {
            os << (*j) << ":" << jemap.myLengths.find(*j)->second << "\t";
        }
        os << endl;
    }
    return os;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
