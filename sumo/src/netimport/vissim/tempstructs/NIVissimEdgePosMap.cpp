/****************************************************************************/
/// @file    NIVissimEdgePosMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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


#include <map>
#include "NIVissimEdgePosMap.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


NIVissimEdgePosMap::NIVissimEdgePosMap() {}


NIVissimEdgePosMap::~NIVissimEdgePosMap() {}


void
NIVissimEdgePosMap::add(int edgeid, SUMOReal pos) {
    add(edgeid, pos, pos);
}


void
NIVissimEdgePosMap::add(int edgeid, SUMOReal from, SUMOReal to) {
    if (from > to) {
        SUMOReal tmp = from;
        from = to;
        to = tmp;
    }
    ContType::iterator i = myCont.find(edgeid);
    if (i == myCont.end()) {
        myCont[edgeid] = Range(from, to);
    } else {
        SUMOReal pfrom = (*i).second.first;
        SUMOReal pto = (*i).second.second;
        if (pfrom < from) {
            from = pfrom;
        }
        if (pto > to) {
            to = pto;
        }
        myCont[edgeid] = Range(from, to);
    }
}


void
NIVissimEdgePosMap::join(NIVissimEdgePosMap& with) {
    for (ContType::iterator i = with.myCont.begin(); i != with.myCont.end(); i++) {
        add((*i).first, (*i).second.first, (*i).second.second);
    }
}



/****************************************************************************/

