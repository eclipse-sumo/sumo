/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrzeugklassendefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id: $
///
//
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

#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/IntVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimVehTypeClass.h"
#include "NIVissimSingleTypeParser_Fahrzeugklassendefinition.h"

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
NIVissimSingleTypeParser_Fahrzeugklassendefinition::NIVissimSingleTypeParser_Fahrzeugklassendefinition(
    NIVissimLoader &parent, NIVissimLoader::ColorMap &colorMap)
        : NIVissimLoader::VissimSingleTypeParser(parent),
        myColorMap(colorMap)
{}


NIVissimSingleTypeParser_Fahrzeugklassendefinition::~NIVissimSingleTypeParser_Fahrzeugklassendefinition()
{}


bool
NIVissimSingleTypeParser_Fahrzeugklassendefinition::parse(std::istream &from)
{
    // id
    int id;
    from >> id; // type-checking is missing!
    // name
    string tag;
    from >> tag;
    string name = readName(from);
    // color
    from >> tag;
    string colorName = myRead(from);
    RGBColor color;
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

