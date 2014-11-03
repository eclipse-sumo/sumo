/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fensterdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 21 Mar 2003
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
#include <utils/common/TplConvert.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Fensterdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fensterdefinition::NIVissimSingleTypeParser_Fensterdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Fensterdefinition::~NIVissimSingleTypeParser_Fensterdefinition() {}


bool
NIVissimSingleTypeParser_Fensterdefinition::parse(std::istream& from) {
    std::string id;
    from >> id; // "typ"
    std::string type = myRead(from);
    if (type == "fzinfo") {
        std::string tmp;
        from >> tmp;
        from >> tmp;
    } else if (type == "ldp" || type == "szp") {
        std::string tmp;
        readUntil(from, "lsa");
    }
    return true;
}



/****************************************************************************/

