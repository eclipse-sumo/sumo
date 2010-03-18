/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrzeugklassendefinition.cpp
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
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/VectorHelper.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimVehTypeClass.h"
#include "NIVissimSingleTypeParser_Fahrzeugklassendefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fahrzeugklassendefinition::NIVissimSingleTypeParser_Fahrzeugklassendefinition(
    NIImporter_Vissim &parent, NIImporter_Vissim::ColorMap &colorMap)
        : NIImporter_Vissim::VissimSingleTypeParser(parent),
        myColorMap(colorMap) {}


NIVissimSingleTypeParser_Fahrzeugklassendefinition::~NIVissimSingleTypeParser_Fahrzeugklassendefinition() {}


bool
NIVissimSingleTypeParser_Fahrzeugklassendefinition::parse(std::istream &from) {
    // id
    int id;
    from >> id; // type-checking is missing!
    // name
    std::string tag;
    from >> tag;
    std::string name = readName(from);
    // color
    from >> tag;
    std::string colorName = myRead(from);
    RGBColor color;
    NIImporter_Vissim::ColorMap::iterator i=myColorMap.find(colorName);
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
    // types
    from >> tag;
    IntVector types;
    from >> tag;
    do {
        types.push_back(TplConvert<char>::_2int(tag.c_str()));
        tag = readEndSecure(from);
    } while (tag!="DATAEND");
    return NIVissimVehTypeClass::dictionary(id, name, color, types);
}



/****************************************************************************/

