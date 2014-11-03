/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Signalgruppendefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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

#include <cassert>
#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/VectorHelper.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Signalgruppendefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Signalgruppendefinition::NIVissimSingleTypeParser_Signalgruppendefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Signalgruppendefinition::~NIVissimSingleTypeParser_Signalgruppendefinition() {}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parse(std::istream& from) {
    //
    int id;
    from >> id; // type-checking is missing!
    //
    std::string tag;
    tag = myRead(from);
    std::string name;
    if (tag == "name") {
        name = readName(from);
        tag = myRead(from);
    }
    //
    int lsaid;
    from >> lsaid;
    NIVissimTL* tl = NIVissimTL::dictionary(lsaid);
    if (tl == 0) {
        WRITE_ERROR("A traffic light group with an unknown traffic light occured.\n  Group-ID: " + toString<int>(id)
                    + "\n  TrafficLight-ID: " + toString<int>(lsaid));
        return false;
    }
    std::string type = tl->getType();
    if (type == "festzeit") {
        return parseFixedTime(id, name, lsaid, from);
    }
    if (type == "festzeit_fake") {
        return parseFixedTime(id, name, lsaid, from);
//        return parseExternFixedTime(id, name, lsaid, from);
    }
    if (type == "vas") {
        return parseVAS(id, name, lsaid, from);
    }
    if (type == "vsplus") {
        return parseVSPLUS(id, name, lsaid, from);
    }
    if (type == "trends") {
        return parseTRENDS(id, name, lsaid, from);
    }
    if (type == "vap") {
        return parseVAP(id, name, lsaid, from);
    }
    if (type == "tl") {
        return parseTL(id, name, lsaid, from);
    }
    if (type == "pos") {
        return parsePOS(id, name, lsaid, from);
    }
    WRITE_WARNING("Unsupported LSA-Type '" + type + "' occured.");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseFixedTime(
    int id, const std::string& name, int lsaid, std::istream& from) {
    //
    bool isGreenBegin;
    std::vector<SUMOReal> times;
    std::string tag = myRead(from);
    if (tag == "dauergruen") {
        isGreenBegin = true;
        from >> tag;
    } else if (tag == "dauerrot") {
        isGreenBegin = false;
        from >> tag;
    } else {
        // the first phase will be green
        isGreenBegin = true;
        while (tag == "rotende" || tag == "gruenanfang") {
            SUMOReal point;
            from >> point; // type-checking is missing!
            times.push_back(point);
            from >> tag;
            from >> point; // type-checking is missing!
            times.push_back(point);
            tag = myRead(from);
        }
    }
    //
    SUMOReal tredyellow, tyellow;
    from >> tredyellow;
    from >> tag;
    from >> tyellow;
    NIVissimTL::NIVissimTLSignalGroup* group =
        new NIVissimTL::NIVissimTLSignalGroup(
        id, name, isGreenBegin, times, (SUMOTime) tredyellow, (SUMOTime) tyellow);
    if (!NIVissimTL::NIVissimTLSignalGroup::dictionary(lsaid, id, group)) {
        throw 1; // !!!
    }
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVAS(
    int /*id*/, const std::string& /*name*/, int lsaid, std::istream& from) {
    WRITE_WARNING("VAS traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    std::string tag;
    while (tag != "detektoren") {
        tag = myRead(from);
    }
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVSPLUS(
    int /*id*/, const std::string&, int lsaid, std::istream&) {
    WRITE_WARNING("VSPLUS traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseTRENDS(
    int /*id*/, const std::string&, int lsaid, std::istream&) {
    WRITE_WARNING("TRENDS traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVAP(
    int /*id*/, const std::string&, int lsaid, std::istream&) {
    WRITE_WARNING("VAS traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseTL(
    int /*id*/, const std::string&, int lsaid, std::istream&) {
    WRITE_WARNING("TL traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parsePOS(
    int /*id*/, const std::string&, int lsaid, std::istream&) {
    WRITE_WARNING("POS traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseExternFixedTime(
    int /*id*/, const std::string&, int lsaid, std::istream&) {
    WRITE_WARNING("externally defined traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    return true;
}



/****************************************************************************/

