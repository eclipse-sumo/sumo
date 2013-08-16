/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Zeitenverteilungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/TplConvert.h>
#include "../NIImporter_Vissim.h"
#include <utils/distribution/Distribution_Points.h>
#include <utils/distribution/Distribution_MeanDev.h>
#include <netbuild/NBDistribution.h>
#include "NIVissimSingleTypeParser_Zeitenverteilungsdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Zeitenverteilungsdefinition::NIVissimSingleTypeParser_Zeitenverteilungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Zeitenverteilungsdefinition::~NIVissimSingleTypeParser_Zeitenverteilungsdefinition() {}


bool
NIVissimSingleTypeParser_Zeitenverteilungsdefinition::parse(std::istream& from) {
    // id
    std::string id;
    from >> id;
    // list of points
    PositionVector points;
    std::string tag;
    do {
        tag = readEndSecure(from);
        if (tag == "mittelwert") {
            SUMOReal mean, deviation;
            from >> mean;
            from >> tag;
            from >> deviation;
            return NBDistribution::dictionary("times", id,
                                              new Distribution_MeanDev(id, mean, deviation));
        }
        if (tag != "DATAEND") {
            SUMOReal p1 = TplConvert::_2SUMOReal(tag.c_str());
            from >> tag;
            SUMOReal p2 = TplConvert::_2SUMOReal(tag.c_str());
            points.push_back(Position(p1, p2));
        }
    } while (tag != "DATAEND");
    return NBDistribution::dictionary("times",
                                      id, new Distribution_Points(id, points));
}



/****************************************************************************/

