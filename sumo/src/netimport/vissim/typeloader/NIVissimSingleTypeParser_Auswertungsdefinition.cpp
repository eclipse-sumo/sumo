/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Auswertungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 21 Mar 2003
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
#include <utils/common/TplConvert.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Auswertungsdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Auswertungsdefinition::NIVissimSingleTypeParser_Auswertungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Auswertungsdefinition::~NIVissimSingleTypeParser_Auswertungsdefinition() {}


bool
NIVissimSingleTypeParser_Auswertungsdefinition::parse(std::istream& from) {
    std::string id;
    from >> id; // "typ"

    if (id == "DATENBANK") { // !!! unverified
        return true;
    }

    std::string type = myRead(from);
    if (type == "abfluss") {
        while (type != "signalgruppe") {
            type = myRead(from);
        }
        while (type != "DATAEND") {
            type = readEndSecure(from, "messung");
        }
    } else if (type == "vbv") {} else if (type == "dichte") {} else if (type == "emissionen") {} else if (type == "fzprot") {} else if (type == "spwprot") {} else if (type == "segment") {
        while (type != "konfdatei") {
            type = myRead(from);
        }
    } else if (type == "wegeausw") {} else if (type == "knoten") {} else if (type == "konvergenz") {
        while (type != "zeit") {
            type = myRead(from);
        }
    }
    return true;
}



/****************************************************************************/

