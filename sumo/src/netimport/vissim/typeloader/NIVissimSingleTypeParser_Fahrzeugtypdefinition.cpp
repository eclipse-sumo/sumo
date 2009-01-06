/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrzeugtypdefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimVehicleType.h"
#include "NIVissimSingleTypeParser_Fahrzeugtypdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fahrzeugtypdefinition::NIVissimSingleTypeParser_Fahrzeugtypdefinition(
    NIVissimLoader &parent, NIVissimLoader::ColorMap &colorMap)
        : NIVissimLoader::VissimSingleTypeParser(parent),
        myColorMap(colorMap)
{}


NIVissimSingleTypeParser_Fahrzeugtypdefinition::~NIVissimSingleTypeParser_Fahrzeugtypdefinition()
{}


bool
NIVissimSingleTypeParser_Fahrzeugtypdefinition::parse(std::istream &from)
{
    // id
    int id;
    from >> id; // type-checking is missing!
    // name
    string tag;
    from >> tag;
    string name = readName(from);
    // category
    string category;
    from >> tag;
    from >> category;
    // color (optional) and length
    RGBColor color;
    tag = myRead(from);
    while (tag!="laenge") {
        if (tag=="farbe") {
            string colorName = myRead(from);
            NIVissimLoader::ColorMap::iterator i=myColorMap.find(colorName);
            if (i!=myColorMap.end()) {
                color = (*i).second;
            } else {
                int r, g, b;
                r = TplConvert<char>::_2int(colorName.c_str());
                from >> g; // type-checking is missing!
                from >> b; // type-checking is missing!
                color = RGBColor(
                            (SUMOReal) r / (SUMOReal) 255.0,
                            (SUMOReal) g / (SUMOReal) 255.0,
                            (SUMOReal) b / (SUMOReal) 255.0);
            }
        }
        tag = myRead(from);
    }
    SUMOReal length;
    from >> length;
    // overread until "Maxbeschleunigung"
    while (tag!="maxbeschleunigung") {
        tag = myRead(from);
    }
    SUMOReal amax;
    from >> amax; // type-checking is missing!
    // overread until "Maxverzoegerung"
    while (tag!="maxverzoegerung") {
        tag = myRead(from);
    }
    SUMOReal dmax;
    from >> dmax; // type-checking is missing!
    while (tag!="besetzungsgrad") {
        tag = myRead(from);
    }
    while (tag!="DATAEND") {
        tag = readEndSecure(from, "verlustzeit");
    }
    return NIVissimVehicleType::dictionary(id, name,
                                           category, length, color, amax, dmax);
}



/****************************************************************************/

