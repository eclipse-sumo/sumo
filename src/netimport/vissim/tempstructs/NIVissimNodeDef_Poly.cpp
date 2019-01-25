/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimNodeDef_Poly.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


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

