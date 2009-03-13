/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Auswertungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 21 Mar 2003
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Auswertungsdefinition.h"

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
NIVissimSingleTypeParser_Auswertungsdefinition::NIVissimSingleTypeParser_Auswertungsdefinition(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Auswertungsdefinition::~NIVissimSingleTypeParser_Auswertungsdefinition() {}


bool
NIVissimSingleTypeParser_Auswertungsdefinition::parse(std::istream &from) {
    string id;
    from >> id; // "typ"

    if (id=="DATENBANK") { // !!! unverified
        return true;
    }

    string type = myRead(from);
    if (type=="abfluss") {
        while (type!="signalgruppe") {
            type = myRead(from);
        }
        while (type!="DATAEND") {
            type = readEndSecure(from, "messung");
        }
    } else if (type=="vbv") {} else if (type=="dichte") {} else if (type=="emissionen") {} else if (type=="fzprot") {} else if (type=="spwprot") {} else if (type=="segment") {
        while (type!="konfdatei") {
            type = myRead(from);
        }
    } else if (type=="wegeausw") {} else if (type=="knoten") {} else if (type=="konvergenz") {
        while (type!="zeit") {
            type = myRead(from);
        }
    }
    return true;
}



/****************************************************************************/

