/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Stopschilddefinition.cpp
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
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Stopschilddefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Stopschilddefinition::NIVissimSingleTypeParser_Stopschilddefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Stopschilddefinition::~NIVissimSingleTypeParser_Stopschilddefinition() {}


bool
NIVissimSingleTypeParser_Stopschilddefinition::parse(std::istream &from) {
    readUntil(from, "strecke");
    std::string tag;
    from >> tag; // edge name
    from >> tag; // "spur"
    from >> tag; // lane no
    from >> tag; // "bei"
    from >> tag; // pos
    from >> tag;
    if (tag=="RTOR") {
        from >> tag; // "lsa"
        from >> tag; // lsa id
        from >> tag; // "gruppe"
        from >> tag; // gruppe id
    } else {
        while (tag=="fahrzeugklasse") {
            from >> tag; // class no
            from >> tag; // "zeiten"
            from >> tag; // times no
            tag = readEndSecure(from, "fahrzeugklasse");
        }
    }
    return true;
}



/****************************************************************************/

