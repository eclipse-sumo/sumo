/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Parkplatzdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <vector>
#include <utility>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/VectorHelper.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimDistrictConnection.h"
#include "NIVissimSingleTypeParser_Parkplatzdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Parkplatzdefinition::NIVissimSingleTypeParser_Parkplatzdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Parkplatzdefinition::~NIVissimSingleTypeParser_Parkplatzdefinition() {}


bool
NIVissimSingleTypeParser_Parkplatzdefinition::parse(std::istream& from) {
    int id;
    from >> id;

    std::string tag;
    from >> tag;
    std::string name = readName(from);

    // parse the districts
    //  and allocate them if not done before
    //  A district may be already saved when another parking place with
    //  the same district was already build.
    std::vector<int> districts;
    std::vector<SUMOReal> percentages;
    readUntil(from, "bezirke"); // "Bezirke"
    while (tag != "ort") {
        SUMOReal perc = -1;
        int districtid;
        from >> districtid;
        tag = myRead(from);
        if (tag == "anteil") {
            from >> perc;
        }
        districts.push_back(districtid);
        percentages.push_back(perc);
        tag = myRead(from);
    }

    from >> tag; // "Strecke"
    int edgeid;
    from >> edgeid;

    SUMOReal position;
    from >> tag; // "bei"
    from >> position;

    SUMOReal length;
    from >> tag;
    from >> length;

    from >> tag; // "Kapazität"
    from >> tag; // "Kapazität"-value

    tag = myRead(from);
    if (tag == "belegung") {
        from >> tag;
        tag = myRead(from); // "fahrzeugklasse"
    }

    std::vector<std::pair<int, int> > assignedVehicles;
    while (tag != "default") {
        int vclass;
        from >> vclass;
        from >> tag; // "vwunsch"
        int vwunsch;
        from >> vwunsch; // "vwunsch"-value
        assignedVehicles.push_back(std::pair<int, int>(vclass, vwunsch));
        tag = myRead(from);
    }

    from >> tag;
    from >> tag;
//    NIVissimEdge *e = NIVissimEdge::dictionary(edgeid);
//    e->addReferencedDistrict(id);

    // build the district connection
    return NIVissimDistrictConnection::dictionary(id, name,
            districts, percentages, edgeid, position, assignedVehicles);
}



/****************************************************************************/

