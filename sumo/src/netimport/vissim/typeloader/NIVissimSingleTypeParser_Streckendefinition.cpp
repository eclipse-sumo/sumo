/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Streckendefinition.cpp
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
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position2DVector.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimEdge.h"
#include "../tempstructs/NIVissimClosedLaneDef.h"
#include "../tempstructs/NIVissimClosedLanesVector.h"
#include "NIVissimSingleTypeParser_Streckendefinition.h"

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
NIVissimSingleTypeParser_Streckendefinition::NIVissimSingleTypeParser_Streckendefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Streckendefinition::~NIVissimSingleTypeParser_Streckendefinition() {}


bool
NIVissimSingleTypeParser_Streckendefinition::parse(std::istream &from) {
    // read in the id
    int id;
    from >> id;
    //
    string tag;
    // the following elements may occure: "Name", "Beschriftung", "Typ",
    //  followed by the mandatory "Laenge"
    string name, label, type;
    SUMOReal length = -1;
    while (length<0) {
        tag = overrideOptionalLabel(from);
        if (tag=="name") {
            name = readName(from);
        } else if (tag=="typ") {
            type = myRead(from);
        } else if (tag=="laenge") {
            from >> length; // type-checking is missing!
        }
    }
    // read in the number of lanes
    int noLanes;
    tag = myRead(from);
    from >> noLanes;
    // skip some parameter, except optional "Zuschlag" until "Von" (mandatory)
    //  occurs
    SUMOReal zuschlag1, zuschlag2;
    zuschlag1 = zuschlag2 = 0;
    while (tag!="von") {
        tag = myRead(from);
        if (tag=="zuschlag") {
            from >> zuschlag1; // type-checking is missing!
            tag = myRead(from);
            if (tag=="zuschlag") {
                from >> zuschlag2; // type-checking is missing!
            }
        }
    }
    // Read the geometry information
    Position2DVector geom;
    while (tag!="nach") {
        geom.push_back_noDoublePos(getPosition2D(from));
        tag = myRead(from);
        try {
            TplConvert<char>::_2SUMOReal(tag.c_str());
            tag = myRead(from);
        } catch (NumberFormatException &) {}
    }
    geom.push_back_noDoublePos(getPosition2D(from));
    // Read definitions of closed lanes
    NIVissimClosedLanesVector clv;
    // check whether a next close lane definition can be found
    tag = readEndSecure(from);
    while (tag!="DATAEND") {
        if (tag=="spur") {
            // get the lane number
            int laneNo;
            from >> laneNo; // type-checking is missing!
            // get the list of assigned car classes
            IntVector assignedVehicles;
            tag = myRead(from);
            tag = myRead(from);
            while (tag!="DATAEND"&&tag!="spur") {
                int classes = TplConvert<char>::_2int(tag.c_str());
                assignedVehicles.push_back(classes);
                tag = readEndSecure(from);
            }
            // build and add the definition
            NIVissimClosedLaneDef *cld = new NIVissimClosedLaneDef(laneNo, assignedVehicles);
            clv.push_back(cld);
        } else {
            tag = readEndSecure(from);
        }
    }
    NIVissimEdge *e = new NIVissimEdge(id, name, type, noLanes,
                                       zuschlag1, zuschlag2, length, geom, clv);
    if (!NIVissimEdge::dictionary(id, e)) {
        return false;
    }
    return true;
    //return NIVissimAbstractEdge::dictionary(id, e);
}



/****************************************************************************/

