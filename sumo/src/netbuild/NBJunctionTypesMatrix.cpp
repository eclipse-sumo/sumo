/****************************************************************************/
/// @file    NBJunctionTypesMatrix.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id: $
///
// Definition of a junction's type in dependence
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
#include <string>
#include <map>
#include <algorithm>
#include "NBJunctionTypesMatrix.h"
#include <utils/common/UtilExceptions.h>
#include "nodes/NBNode.h"

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
NBJunctionTypesMatrix::NBJunctionTypesMatrix()
{
    _map['t'] = NBNode::NODETYPE_PRIORITY_JUNCTION; // !!! was: NODETYPE_SIMPLE_TRAFFIC_LIGHT;
    _map['x'] = NBNode::NODETYPE_NOJUNCTION;
    _map['p'] = NBNode::NODETYPE_PRIORITY_JUNCTION;
    _map['r'] = NBNode::NODETYPE_RIGHT_BEFORE_LEFT;
    _ranges.push_back(pair<int, int>(90, 90));
    _ranges.push_back(pair<int, int>(65, 85));
    _ranges.push_back(pair<int, int>(59, 41));
    _ranges.push_back(pair<int, int>(20, 20));
    _ranges.push_back(pair<int, int>(15, 15));
    _ranges.push_back(pair<int, int>(10, 10));
    _ranges.push_back(pair<int, int>(5, 1));
    _values.push_back("xxxxpxx");
    _values.push_back(" ttttpp");
    _values.push_back("  ppppp");
    _values.push_back("   pppp");
    _values.push_back("    xxx");
    _values.push_back("     rp");
    _values.push_back("      p");
}


NBJunctionTypesMatrix::~NBJunctionTypesMatrix()
{}


NBNode::BasicNodeType
NBJunctionTypesMatrix::getType(int prio1, int prio2) const
{
    RangeCont::const_iterator p1 = find_if(_ranges.begin(), _ranges.end(),
                                           priority_finder(prio1));
    RangeCont::const_iterator p2 = find_if(_ranges.begin(), _ranges.end(),
                                           priority_finder(prio2));
    if (p1==_ranges.end()||p2==_ranges.end()) {
        throw OutOfBoundsException();
    }
    char name = getNameAt(distance(_ranges.begin(), p1),
                          distance(_ranges.begin(), p2));
    return _map.find(name)->second;
}


char
NBJunctionTypesMatrix::getNameAt(int pos1, int pos2) const
{
    string str = _values[pos1];
    if (str[pos2]==' ') {
        return getNameAt(pos2, pos1);
    }
    return str[pos2];
}



/****************************************************************************/

