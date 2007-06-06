/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrverhaltendefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "../NIVissimLoader.h"
#include "NIVissimSingleTypeParser_Fahrverhaltendefinition.h"

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
NIVissimSingleTypeParser_Fahrverhaltendefinition::NIVissimSingleTypeParser_Fahrverhaltendefinition(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent)
{}


NIVissimSingleTypeParser_Fahrverhaltendefinition::~NIVissimSingleTypeParser_Fahrverhaltendefinition()
{}


bool
NIVissimSingleTypeParser_Fahrverhaltendefinition::parse(std::istream &from)
{
    string tmp;
    from >> tmp;
    // in the both next cases, we do not have to overread anything
    if (tmp=="BEHALT_ALTE_PARA" || tmp=="ANZ_VM") {
        return true;
    }
    //
    from >> tmp;
    if (tmp=="NAME") {
        readUntil(from, "gelbverhalten");
    }
    return true;
}



/****************************************************************************/

