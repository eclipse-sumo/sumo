/****************************************************************************/
/// @file    NBHelpers.cpp
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Some mathematical helper methods
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
//#include <iomanip>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>
#include "NBNode.h"
#include "NBHelpers.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal
NBHelpers::relAngle(SUMOReal angle1, SUMOReal angle2) {
    angle2 -= angle1;
    if (angle2 > 180) {
        angle2 = (360 - angle2) * -1;
    }
    while (angle2 < -180) {
        angle2 = 360 + angle2;
    }
    return angle2;
}


SUMOReal
NBHelpers::normRelAngle(SUMOReal angle1, SUMOReal angle2) {
    SUMOReal rel = relAngle(angle1, angle2);
    if (rel + NUMERICAL_EPS >= 180) {
        return -180;
    } else {
        return rel;
    }
}


std::string
NBHelpers::normalIDRepresentation(const std::string& id) {
    std::stringstream strm1(id);
    long numid;
    strm1 >> numid;
    std::stringstream strm2;
    strm2 << numid;
    return strm2.str();
}


SUMOReal
NBHelpers::distance(NBNode* node1, NBNode* node2) {
    return node1->getPosition().distanceTo(node2->getPosition());
}


void
NBHelpers::loadEdgesFromFile(const std::string& file, std::set<std::string>& into) {
    std::ifstream strm(file.c_str());
    if (!strm.good()) {
        throw ProcessError("Could not load names of edges too keep from '" + file + "'.");
    }
    while (strm.good()) {
        std::string name;
        strm >> name;
        into.insert(name);
        // maybe we're loading an edge-selection
        if (StringUtils::startsWith(name, "edge:")) {
            into.insert(name.substr(5));
        }
    }
}


void
NBHelpers::loadPrefixedIDsFomFile(const std::string& file, const std::string prefix, std::set<std::string>& into) {
    std::ifstream strm(file.c_str());
    if (!strm.good()) {
        throw ProcessError("Could not load IDs from '" + file + "'.");
    }
    while (strm.good()) {
        std::string prefixedID;
        strm >> prefixedID;
        if (StringUtils::startsWith(prefixedID, prefix)) {
            into.insert(prefixedID.substr(prefix.size()));
        }
    }
}


/****************************************************************************/
