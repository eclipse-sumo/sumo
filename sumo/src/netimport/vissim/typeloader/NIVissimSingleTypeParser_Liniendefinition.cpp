/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Liniendefinition.cpp
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
#include "NIVissimSingleTypeParser_Liniendefinition.h"

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
NIVissimSingleTypeParser_Liniendefinition::NIVissimSingleTypeParser_Liniendefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Liniendefinition::~NIVissimSingleTypeParser_Liniendefinition() {}


bool
NIVissimSingleTypeParser_Liniendefinition::parse(std::istream &from) {
    string tag;
    from >> tag;
    //cout << tag << endl;
    do {
        getline(from, tag);
    } while (tag.length()>0&&tag[0]==' ');
    return true;
}



/****************************************************************************/

