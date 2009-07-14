/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "../tempstructs/NIVissimExtendedEdgePoint.h"
#include "../tempstructs/NIVissimDisturbance.h"
#include "NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h"

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
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition(NIImporter_Vissim &parent)
        : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::~NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition() {}


bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parse(std::istream &from) {
    string tag;
    tag = myRead(from);
    if (tag=="nureigenestrecke") {
        return parseOnlyMe(from);
    } else if (tag=="ort") {
        return parsePositionDescribed(from);
    } else if (tag=="nummer") {
        return parseNumbered(from);
    }
    MsgHandler::getErrorInstance()->inform(
        "NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition: format problem");
    throw 1;
}

bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parseOnlyMe(std::istream &from) {
    string tag;
    from >> tag;
    return true;
}


bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parsePositionDescribed(std::istream &from) {
    string tag = myRead(from);
    NIVissimExtendedEdgePoint edge = parsePos(from);
//    from >> tag; // "Durch"
    bool ok = true;
    do {
        from >> tag; // "Strecke"
        NIVissimExtendedEdgePoint by = parsePos(from);
        //
        SUMOReal timegap;
        from >> timegap;

        from >> tag;
        SUMOReal waygap;
        from >> waygap;

        SUMOReal vmax = -1;
        tag = readEndSecure(from);
        if (tag=="vmax") {
            from >> vmax;
        }
        ok = NIVissimDisturbance::dictionary(-1, "", edge, by,
                                             timegap, waygap, vmax);
        if (tag!="DATAEND") {
            tag = readEndSecure(from);
        }
    } while (tag!="DATAEND"&&ok);
    return ok;
}



bool
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parseNumbered(std::istream &from) {
    //
    int id;
    from >> id;
    //
    string tag;
    from >> tag;
    string name = readName(from);
    // skip optional "Beschriftung"
    while (tag!="ort") {
        tag = myRead(from);
    }
    //
    from >> tag; // "Strecke"
    NIVissimExtendedEdgePoint edge = parsePos(from);
    bool ok = true;
    do {
        from >> tag; // "Ort"
        from >> tag; // "Strecke"
        NIVissimExtendedEdgePoint by = parsePos(from);
        //
        SUMOReal timegap;
        from >> timegap;

        SUMOReal waygap;
        from >> tag;
        from >> waygap;

        SUMOReal vmax = -1;
        tag = readEndSecure(from);
        if (tag=="vmax") {
            from >> vmax;
        }

        ok = NIVissimDisturbance::dictionary(id, name, edge, by,
                                             timegap, waygap, vmax);
        if (tag!="DATAEND") {
            tag = readEndSecure(from);
        }
    } while (tag!="DATAEND"&&ok);
    return ok;
}



NIVissimExtendedEdgePoint
NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition::parsePos(std::istream &from) {
    int edgeid;
    from >> edgeid; // type-checking is missing!
    //
    string tag;
    from >> tag;
    from >> tag;
    IntVector lanes;
    if (tag=="ALLE") {
        //lanes.push_back(1); // !!!
    } else {
        lanes.push_back(TplConvert<char>::_2int(tag.c_str()));
    }
    //
    SUMOReal position;
    from >> tag;
    from >> position;
    // assigned vehicle types
    IntVector types;
    from >> tag;
    while (tag!="zeitluecke"&&tag!="durch"&&tag!="DATAEND"&&tag!="alle") {
        tag = readEndSecure(from);
        if (tag!="DATAEND") {
            if (tag=="alle") {
                types.push_back(-1);
                from >> tag;
                tag = "alle";
            } else if (tag!="zeitluecke"&&tag!="durch"&&tag!="DATAEND") {
                int tmp = TplConvert<char>::_2int(tag.c_str());
                types.push_back(tmp);
            }
        }
    }
    return NIVissimExtendedEdgePoint(edgeid, lanes, position, types);
}



/****************************************************************************/

