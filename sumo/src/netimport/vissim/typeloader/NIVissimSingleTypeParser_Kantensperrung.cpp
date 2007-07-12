/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Kantensperrung.cpp
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
#include "../tempstructs/NIVissimClosures.h"
#include "NIVissimSingleTypeParser_Kantensperrung.h"

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
NIVissimSingleTypeParser_Kantensperrung::NIVissimSingleTypeParser_Kantensperrung(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent)
{}


NIVissimSingleTypeParser_Kantensperrung::~NIVissimSingleTypeParser_Kantensperrung()
{}


bool
NIVissimSingleTypeParser_Kantensperrung::parse(std::istream &from)
{
    string tag;
    from >> tag;
    //
    string id;
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
            edges.push_back(TplConvert<char>::my2int(tag.c_str()));
        }
    }
    NIVissimClosures::dictionary(id, from_node, to_node, edges);
    return true;
}



/****************************************************************************/

