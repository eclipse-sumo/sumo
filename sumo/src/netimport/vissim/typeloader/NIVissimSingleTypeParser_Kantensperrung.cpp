/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Kantensperrung.cpp
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
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimClosures.h"
#include "NIVissimSingleTypeParser_Kantensperrung.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Kantensperrung::NIVissimSingleTypeParser_Kantensperrung(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Kantensperrung::~NIVissimSingleTypeParser_Kantensperrung() {}


bool
NIVissimSingleTypeParser_Kantensperrung::parse(std::istream &from) {
    std::string tag;
    from >> tag;
    //
    std::string id;
    from >> id;
    //
    from >> tag;
    from >> tag;
    int from_node;
    from >> from_node;
    //
    from >> tag;
    from >> tag;
    int to_node;
    from >> to_node;
    //
    from >> tag;
    from >> tag;
    IntVector edges;
    while (tag!="DATAEND") {
        tag = readEndSecure(from);
        if (tag!="DATAEND") {
            edges.push_back(TplConvert<char>::_2int(tag.c_str()));
        }
    }
    NIVissimClosures::dictionary(id, from_node, to_node, edges);
    return true;
}



/****************************************************************************/

