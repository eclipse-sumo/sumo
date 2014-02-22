/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrzeugtypdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimVehicleType.h"
#include "NIVissimSingleTypeParser_Fahrzeugtypdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fahrzeugtypdefinition::NIVissimSingleTypeParser_Fahrzeugtypdefinition(
    NIImporter_Vissim& parent, NIImporter_Vissim::ColorMap& colorMap)
    : NIImporter_Vissim::VissimSingleTypeParser(parent),
      myColorMap(colorMap) {}


NIVissimSingleTypeParser_Fahrzeugtypdefinition::~NIVissimSingleTypeParser_Fahrzeugtypdefinition() {}


bool
NIVissimSingleTypeParser_Fahrzeugtypdefinition::parse(std::istream& from) {
    // id
    int id;
    from >> id; // type-checking is missing!
    // name
    std::string tag;
    from >> tag;
    std::string name = readName(from);
    // category
    std::string category;
    from >> tag;
    from >> category;
    // color (optional) and length
    RGBColor color;
    tag = myRead(from);
    while (tag != "laenge") {
        if (tag == "farbe") {
            std::string colorName = myRead(from);
            NIImporter_Vissim::ColorMap::iterator i = myColorMap.find(colorName);
            if (i != myColorMap.end()) {
                color = (*i).second;
            } else {
                int r, g, b;
                r = TplConvert::_2int(colorName.c_str());
                if (!(from >> g)) {
                    throw NumberFormatException();
                }
                if (!(from >> b)) {
                    throw NumberFormatException();
                }
                if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
                    throw NumberFormatException();
                }
                color = RGBColor((unsigned char)r, (unsigned char)g, (unsigned char)b, 255);
            }
        }
        tag = myRead(from);
    }
    SUMOReal length;
    from >> length;
    // overread until "Maxbeschleunigung"
    while (tag != "maxbeschleunigung") {
        tag = myRead(from);
    }
    SUMOReal amax;
    from >> amax; // type-checking is missing!
    // overread until "Maxverzoegerung"
    while (tag != "maxverzoegerung") {
        tag = myRead(from);
    }
    SUMOReal dmax;
    from >> dmax; // type-checking is missing!
    while (tag != "besetzungsgrad") {
        tag = myRead(from);
    }
    while (tag != "DATAEND") {
        tag = readEndSecure(from, "verlustzeit");
    }
    return NIVissimVehicleType::dictionary(id, name,
                                           category, length, color, amax, dmax);
}



/****************************************************************************/

