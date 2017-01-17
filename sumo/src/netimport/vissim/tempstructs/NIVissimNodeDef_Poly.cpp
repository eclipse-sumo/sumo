/****************************************************************************/
/// @file    NIVissimNodeDef_Poly.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
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


#include <string>
#include <map>
#include <cassert>
#include <algorithm>
#include <utils/geom/PositionVector.h>
#include "NIVissimEdge.h"
#include "NIVissimNodeDef.h"
#include "NIVissimNodeDef_Poly.h"
#include "NIVissimConnection.h"
#include "NIVissimAbstractEdge.h"
#include <utils/geom/Boundary.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimNodeDef_Poly::NIVissimNodeDef_Poly(int id, const std::string& name,
        const PositionVector& poly)
    : NIVissimNodeDef_Edges(id, name, NIVissimNodeParticipatingEdgeVector()),
      myPoly(poly) {}


NIVissimNodeDef_Poly::~NIVissimNodeDef_Poly() {}


bool
NIVissimNodeDef_Poly::dictionary(int id, const std::string& name,
                                 const PositionVector& poly) {
    NIVissimNodeDef_Poly* o = new NIVissimNodeDef_Poly(id, name, poly);
    if (!NIVissimNodeDef::dictionary(id, o)) {
        delete o;
        assert(false);
        return false;
    }
    return true;
}


/****************************************************************************/

