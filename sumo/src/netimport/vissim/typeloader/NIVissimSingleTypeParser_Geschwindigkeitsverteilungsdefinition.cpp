/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
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

#include <iostream>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <netbuild/NBDistribution.h>
#include <utils/distribution/Distribution_Points.h>
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::~NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition() {}


bool
NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::parse(std::istream &from) {
    // id
    std::string id;
    from >> id;
    // list of points
    Position2DVector points;
    std::string tag;
    do {
        tag = readEndSecure(from);
        if (tag!="DATAEND") {
            SUMOReal p1 = TplConvert<char>::_2SUMOReal(tag.c_str());
            from >> tag;
            SUMOReal p2 = TplConvert<char>::_2SUMOReal(tag.c_str());
            points.push_back(Position2D(p1, p2));
        }
    } while (tag!="DATAEND");
    NBDistribution::dictionary("speed",
                               id, new Distribution_Points(id, points));
    return true;
}



/****************************************************************************/

