/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Simdauer.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/MsgHandler.h>
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Simdauer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Simdauer::NIVissimSingleTypeParser_Simdauer(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Simdauer::~NIVissimSingleTypeParser_Simdauer() {}


bool
NIVissimSingleTypeParser_Simdauer::parse(std::istream &from) {
    std::string duration;
    from >> duration;
    // !!!
    try {
        TplConvert<char>::_2SUMOReal(duration.c_str());
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Simulation duration could not be parsed");
        return false;
    }
    return true;
}



/****************************************************************************/

