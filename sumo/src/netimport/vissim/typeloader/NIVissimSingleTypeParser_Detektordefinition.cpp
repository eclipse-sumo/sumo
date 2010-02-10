/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Detektordefinition.cpp
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
#include "../tempstructs/NIVissimClosures.h"
#include "NIVissimSingleTypeParser_Detektordefinition.h"

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
NIVissimSingleTypeParser_Detektordefinition::NIVissimSingleTypeParser_Detektordefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Detektordefinition::~NIVissimSingleTypeParser_Detektordefinition() {}


bool
NIVissimSingleTypeParser_Detektordefinition::parse(std::istream &from) {
    string tag;
    while (tag!="strecke") {
        tag = myRead(from);
    }
    while (tag!="DATAEND") {
        tag = readEndSecure(from);
    }
    return true;
}



/****************************************************************************/

