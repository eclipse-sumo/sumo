/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Zeitschrittfaktor.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 30 Apr 2003
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
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Zeitschrittfaktor.h"

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
NIVissimSingleTypeParser_Zeitschrittfaktor::NIVissimSingleTypeParser_Zeitschrittfaktor(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Zeitschrittfaktor::~NIVissimSingleTypeParser_Zeitschrittfaktor() {}


bool
NIVissimSingleTypeParser_Zeitschrittfaktor::parse(std::istream &) {
    return true;
}



/****************************************************************************/

