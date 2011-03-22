/****************************************************************************/
/// @file    NBJunctionTypesMatrix.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Definition of a junction's type in dependence
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "NBJunctionTypesMatrix.h"
#include <utils/common/UtilExceptions.h>
#include "NBNode.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBJunctionTypesMatrix::NBJunctionTypesMatrix() {
    myMap['t'] = NODETYPE_TRAFFIC_LIGHT;
    myMap['x'] = NODETYPE_NOJUNCTION;
    myMap['p'] = NODETYPE_PRIORITY_JUNCTION;
    myMap['r'] = NODETYPE_RIGHT_BEFORE_LEFT;
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(0./3.6), (SUMOReal)(10./3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(10./3.6), (SUMOReal)(30./3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(30./3.6), (SUMOReal)(50./3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(50./3.6), (SUMOReal)(70./3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(70./3.6), (SUMOReal)(100./3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(100./3.6), (SUMOReal)(999999./3.6)));
    //                 00001x
    //                 13570x
    //                 00000x
    myValues.push_back("rppppp"); // 000 - 010
    myValues.push_back(" rpppp"); // 010 - 030
    myValues.push_back("  rppp"); // 030 - 050
    myValues.push_back("   ttp"); // 050 - 070
    myValues.push_back("    tp"); // 070 - 100
    myValues.push_back("     p"); // 100 -
}


NBJunctionTypesMatrix::~NBJunctionTypesMatrix() {}


SumoXMLNodeType
NBJunctionTypesMatrix::getType(SUMOReal speed1, SUMOReal speed2) const {
    RangeCont::const_iterator p1 = find_if(myRanges.begin(), myRanges.end(), range_finder(speed1));
    RangeCont::const_iterator p2 = find_if(myRanges.begin(), myRanges.end(), range_finder(speed2));
    char name = getNameAt(distance(myRanges.begin(), p1), distance(myRanges.begin(), p2));
    return myMap.find(name)->second;
}


char
NBJunctionTypesMatrix::getNameAt(size_t pos1, size_t pos2) const {
    std::string str = myValues[pos1];
    if (str[pos2]==' ') {
        return getNameAt(pos2, pos1);
    }
    return str[pos2];
}



/****************************************************************************/

