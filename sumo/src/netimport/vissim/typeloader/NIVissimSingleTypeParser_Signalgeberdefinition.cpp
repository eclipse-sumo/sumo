/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Signalgeberdefinition.cpp
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

#include <cassert>
#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/VectorHelper.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Signalgeberdefinition.h"

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
NIVissimSingleTypeParser_Signalgeberdefinition::NIVissimSingleTypeParser_Signalgeberdefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Signalgeberdefinition::~NIVissimSingleTypeParser_Signalgeberdefinition() {}


bool
NIVissimSingleTypeParser_Signalgeberdefinition::parse(std::istream &from) {
    //
    int id;
    from >> id;
    //
    string tag, name;
    tag = myRead(from);
    if (tag=="name") {
        name = readName(from);
        tag = myRead(from);
    }
    // skip optional "Beschriftung"
    tag = overrideOptionalLabel(from, tag);
    //
    int lsaid;
    IntVector groupids;
    if (tag=="lsa") {
        int groupid;
        from >> lsaid; // type-checking is missing!
        from >> tag; // "Gruppe"
        do {
            from >> groupid;
            groupids.push_back(groupid);
            tag = myRead(from);
        } while (tag=="oder");
        //
    } else {
        from >> tag; // strecke
        WRITE_WARNING("Omitting unknown traffic light.");
        return true;
    }
    if (tag=="typ") {
        from >> tag; // typ-value
        from >> tag; // "ort"
    }

    //
    from >> tag;
    int edgeid;
    from >> edgeid;

    from >> tag;
    int laneno;
    from >> laneno;

    from >> tag;
    int position;
    from >> position;
    //
    while (tag!="fahrzeugklassen") {
        tag = myRead(from);
    }
    IntVector assignedVehicleTypes = parseAssignedVehicleTypes(from, "N/A");
    //
    NIVissimTL::dictionary(lsaid); // !!! check whether someting is really done here
    NIVissimTL::NIVissimTLSignal *signal =
        new NIVissimTL::NIVissimTLSignal(lsaid, id, name, groupids, edgeid,
                                         laneno, (SUMOReal) position, assignedVehicleTypes);
    if (!NIVissimTL::NIVissimTLSignal::dictionary(lsaid, id, signal)) {
        throw 1; // !!!
    }
    return true;
}



/****************************************************************************/

