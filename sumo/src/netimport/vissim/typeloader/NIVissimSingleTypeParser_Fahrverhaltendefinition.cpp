/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrverhaltendefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Fahrverhaltendefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fahrverhaltendefinition::NIVissimSingleTypeParser_Fahrverhaltendefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Fahrverhaltendefinition::~NIVissimSingleTypeParser_Fahrverhaltendefinition() {}


bool
NIVissimSingleTypeParser_Fahrverhaltendefinition::parse(std::istream& from) {
    std::string tmp;
    from >> tmp;
    // in the both next cases, we do not have to overread anything
    if (tmp == "BEHALT_ALTE_PARA" || tmp == "ANZ_VM") {
        return true;
    }
    //
    from >> tmp;
    if (tmp == "NAME") {
        readUntil(from, "gelbverhalten");
    }
    return true;
}



/****************************************************************************/

