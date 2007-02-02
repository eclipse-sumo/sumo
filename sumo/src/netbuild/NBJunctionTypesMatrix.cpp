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
    _map['t'] = NBNode::NODETYPE_TRAFFIC_LIGHT;
    _map['x'] = NBNode::NODETYPE_NOJUNCTION;
    _map['p'] = NBNode::NODETYPE_PRIORITY_JUNCTION;
    _map['r'] = NBNode::NODETYPE_RIGHT_BEFORE_LEFT;
    _ranges.push_back(pair<SUMOReal, SUMOReal>((SUMOReal)(0./3.6), (SUMOReal)(10./3.6)));
    _ranges.push_back(pair<SUMOReal, SUMOReal>((SUMOReal)(10./3.6), (SUMOReal)(30./3.6)));
    _ranges.push_back(pair<SUMOReal, SUMOReal>((SUMOReal)(30./3.6), (SUMOReal)(50./3.6)));
    _ranges.push_back(pair<SUMOReal, SUMOReal>((SUMOReal)(50./3.6), (SUMOReal)(70./3.6)));
    _ranges.push_back(pair<SUMOReal, SUMOReal>((SUMOReal)(70./3.6), (SUMOReal)(100./3.6)));
    _ranges.push_back(pair<SUMOReal, SUMOReal>((SUMOReal)(100./3.6), (SUMOReal)(999999./3.6)));
    //                 00001x
    //                 13570x
    //                 00000x
    _values.push_back("rppppp"); // 000 - 010
    _values.push_back(" rpppp"); // 010 - 030
    _values.push_back("  rppp"); // 030 - 050
    _values.push_back("   ttp"); // 050 - 070
    _values.push_back("    tp"); // 070 - 100
    _values.push_back("     p"); // 100 -
}


NBJunctionTypesMatrix::~NBJunctionTypesMatrix()
{}


NBNode::BasicNodeType
NBJunctionTypesMatrix::getType(SUMOReal speed1, SUMOReal speed2) const
{
    RangeCont::const_iterator p1 = find_if(_ranges.begin(), _ranges.end(), range_finder(speed1));
    RangeCont::const_iterator p2 = find_if(_ranges.begin(), _ranges.end(), range_finder(speed2));
    char name = getNameAt(distance(_ranges.begin(), p1), distance(_ranges.begin(), p2));
    return _map.find(name)->second;
}


char
NBJunctionTypesMatrix::getNameAt(size_t pos1, size_t pos2) const
{
    string str = _values[pos1];
    if (str[pos2]==' ') {
        return getNameAt(pos2, pos1);
    }
    return str[pos2];
}



/****************************************************************************/

