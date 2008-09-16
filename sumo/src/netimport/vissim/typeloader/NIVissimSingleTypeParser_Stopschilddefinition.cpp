/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Stopschilddefinition.cpp
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
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "NIVissimSingleTypeParser_Stopschilddefinition.h"

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
NIVissimSingleTypeParser_Stopschilddefinition::NIVissimSingleTypeParser_Stopschilddefinition(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent)
{}


NIVissimSingleTypeParser_Stopschilddefinition::~NIVissimSingleTypeParser_Stopschilddefinition()
{}


bool
NIVissimSingleTypeParser_Stopschilddefinition::parse(std::istream &from)
{
    readUntil(from, "strecke");
    string tag;
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

