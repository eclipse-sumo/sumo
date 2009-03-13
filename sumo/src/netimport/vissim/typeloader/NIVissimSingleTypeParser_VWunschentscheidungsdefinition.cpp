/****************************************************************************/
/// @file    NIVissimSingleTypeParser_VWunschentscheidungsdefinition.cpp
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
#include <vector>
#include <cassert>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimEdge.h"
#include "../tempstructs/NIVissimConnection.h"
#include "NIVissimSingleTypeParser_VWunschentscheidungsdefinition.h"

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
NIVissimSingleTypeParser_VWunschentscheidungsdefinition::NIVissimSingleTypeParser_VWunschentscheidungsdefinition(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_VWunschentscheidungsdefinition::~NIVissimSingleTypeParser_VWunschentscheidungsdefinition() {}


bool
NIVissimSingleTypeParser_VWunschentscheidungsdefinition::parse(std::istream &from) {
    string tag;
    from >> tag; // id
    from >> tag; // name
    tag = readName(from);
    tag = overrideOptionalLabel(from);
    from >> tag; // strecke
    string edgeid;
    from >> edgeid;
    from >> tag; // spur
    string lane;
    from >> lane;
    from >> tag; // bei
    string pos;
    from >> pos;
    from >> tag; // fahrzeugklasse
    from >> tag; // <fahrzeugklasse>
    from >> tag; // vwunsch
    string vwunsch;
    from >> vwunsch; // vwunsch
    std::vector<std::string> tmp;
    tmp.push_back("zeit");
    tmp.push_back("fahrzeugklasse");
    tag = readEndSecure(from, tmp);
    while (tag!="DATAEND"&&tag!="zeit") {
        from >> tag;
        from >> tag;
        from >> tag;
        tag = myRead(from);
    }
    if (tag=="zeit") {
        from >> tag;
        from >> tag;
        from >> tag;
        from >> tag;
    }
    int numid = TplConvert<char>::_2int(edgeid.c_str());
    int numlane = TplConvert<char>::_2int(lane.c_str()) - 1;
    int numv = TplConvert<char>::_2int(vwunsch.c_str());
    NIVissimEdge *e = NIVissimEdge::dictionary(numid);
    if (e==0) {
        NIVissimConnection *c = NIVissimConnection::dictionary(numid);
        const IntVector &lanes = c->getToLanes();
        e = NIVissimEdge::dictionary(c->getToEdgeID());
        for (IntVector::const_iterator j=lanes.begin(); j!=lanes.end(); j++) {
            e->setSpeed((*j), numv);
        }
        assert(e!=0);
    } else {
        e->setSpeed(numlane, numv);
    }
    return true;
}



/****************************************************************************/

