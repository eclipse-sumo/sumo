/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Knotendefinition.cpp
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
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimNodeParticipatingEdge.h"
#include "../tempstructs/NIVissimNodeParticipatingEdgeVector.h"
#include "../tempstructs/NIVissimNodeDef_Edges.h"
#include "../tempstructs/NIVissimNodeDef_Poly.h"
#include "../tempstructs/NIVissimNodeDef.h"
#include "NIVissimSingleTypeParser_Knotendefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Knotendefinition::NIVissimSingleTypeParser_Knotendefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Knotendefinition::~NIVissimSingleTypeParser_Knotendefinition() {}


bool
NIVissimSingleTypeParser_Knotendefinition::parse(std::istream &from) {
    //
    int id;
    from >> id;
    //
    std::string tag;
    from >> tag;
    std::string name = readName(from);
    //
    tag = overrideOptionalLabel(from);
    //
    while (tag!="netzausschnitt") {
        tag = myRead(from);
    }
    //
    tag = myRead(from);
    if (tag=="strecke") {
        NIVissimNodeParticipatingEdgeVector edges;
        while (tag=="strecke") {
            int edgeid;
            SUMOReal from_pos, to_pos;
            from_pos = to_pos = -1.0;
            from >> edgeid;
            tag = readEndSecure(from, "strecke");
            if (tag=="von") {
                from >> from_pos; // type-checking is missing!
                from >> tag;
                from >> to_pos; // type-checking is missing!
                tag = readEndSecure(from, "strecke");
            }
            edges.push_back(new NIVissimNodeParticipatingEdge(edgeid, from_pos, to_pos));
        }
        NIVissimNodeDef_Edges::dictionary(id, name, edges);
    } else {
        int no = TplConvert<char>::_2int(tag.c_str());
        Position2DVector poly;
        for (int i=0; i<no; i++) {
            poly.push_back(getPosition2D(from));
        }
        poly.closePolygon();
        NIVissimNodeDef_Poly::dictionary(id, name, poly);
    }
    return true;
}



/****************************************************************************/

