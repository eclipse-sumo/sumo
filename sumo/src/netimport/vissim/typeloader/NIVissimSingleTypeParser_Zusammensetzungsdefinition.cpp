/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Zusammensetzungsdefinition.cpp
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
#include <utils/common/TplConvert.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Zusammensetzungsdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Zusammensetzungsdefinition::NIVissimSingleTypeParser_Zusammensetzungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Zusammensetzungsdefinition::~NIVissimSingleTypeParser_Zusammensetzungsdefinition() {}


bool
NIVissimSingleTypeParser_Zusammensetzungsdefinition::parse(std::istream& from) {
    std::string tag = myRead(from);
    while (tag != "fahrzeugtyp") {
        tag = readEndSecure(from, "fahrzeugtyp");
    }
    do {
        tag = myRead(from); // id
        tag = myRead(from); // "anteil"
        tag = myRead(from); // value
        tag = myRead(from); // "VWunsch"
        tag = myRead(from); // value
        tag = readEndSecure(from, "fahrzeugtyp"); // "fahrzeugtyp"?
    } while (tag == "fahrzeugtyp");
    return true;
}



/****************************************************************************/

