/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Haltestellendefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimClosures.h"
#include "NIVissimSingleTypeParser_Haltestellendefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Haltestellendefinition::NIVissimSingleTypeParser_Haltestellendefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Haltestellendefinition::~NIVissimSingleTypeParser_Haltestellendefinition() {}


bool
NIVissimSingleTypeParser_Haltestellendefinition::parse(std::istream& from) {
    std::string tag;
    while (tag != "strecke") {
        tag = myRead(from);
    }
    while (tag != "laenge") {
        tag = myRead(from);
    }
    from >> tag;
    tag = readEndSecure(from);
    while (tag == "einsteiger" && tag != "DATAEND") {
        while (tag != "linie") {
            tag = myRead(from);
        }
        while (tag != "einsteiger" && tag != "DATAEND") {
            tag = readEndSecure(from);
        }
    }
    return true;
}



/****************************************************************************/

