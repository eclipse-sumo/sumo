/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimEdgePosMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <map>
#include "NIVissimEdgePosMap.h"


NIVissimEdgePosMap::NIVissimEdgePosMap() {}


NIVissimEdgePosMap::~NIVissimEdgePosMap() {}


void
NIVissimEdgePosMap::add(int edgeid, double pos) {
    add(edgeid, pos, pos);
}


void
NIVissimEdgePosMap::add(int edgeid, double from, double to) {
    if (from > to) {
        double tmp = from;
        from = to;
        to = tmp;
    }
    ContType::iterator i = myCont.find(edgeid);
    if (i == myCont.end()) {
        myCont[edgeid] = Range(from, to);
    } else {
        double pfrom = (*i).second.first;
        double pto = (*i).second.second;
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

