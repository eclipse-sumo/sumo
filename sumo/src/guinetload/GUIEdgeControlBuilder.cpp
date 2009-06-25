/****************************************************************************/
/// @file    GUIEdgeControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Derivation of NLEdgeControlBuilder which build gui-edges
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

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUILane.h>
#include <guisim/GUIInternalLane.h>
#include <microsim/MSJunction.h>
#include <netload/NLBuilder.h>
#include "GUIEdgeControlBuilder.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>

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
GUIEdgeControlBuilder::GUIEdgeControlBuilder(GUIGlObjectStorage &glObjectIDStorage) throw()
        : NLEdgeControlBuilder(),
        myGlObjectIDStorage(glObjectIDStorage) {}


GUIEdgeControlBuilder::~GUIEdgeControlBuilder() throw() {}


MSEdge *
GUIEdgeControlBuilder::closeEdge() {
    MSEdge *ret = NLEdgeControlBuilder::closeEdge();
    static_cast<GUIEdge*>(ret)->initGeometry(GUIGlObjectStorage::gIDStorage);
    return ret;
}


MSLane *
GUIEdgeControlBuilder::addLane(const std::string &id,
                               SUMOReal maxSpeed, SUMOReal length, bool isDepart,
                               const Position2DVector &shape,
                               const std::vector<SUMOVehicleClass> &allowed,
                               const std::vector<SUMOVehicleClass> &disallowed) {
    // checks if the depart lane was set before
    if (isDepart&&m_pDepartLane!=0) {
        throw InvalidArgument("Lane's '" + id + "' edge already has a depart lane.");
    }
    MSLane *lane = 0;
    switch (m_Function) {
    case MSEdge::EDGEFUNCTION_INTERNAL:
        lane = new GUIInternalLane(id, maxSpeed, length, myActiveEdge,
                                   myCurrentNumericalLaneID++, shape, allowed, disallowed);
        break;
    case MSEdge::EDGEFUNCTION_NORMAL:
    case MSEdge::EDGEFUNCTION_CONNECTOR:
        lane = new GUILane(id, maxSpeed, length, myActiveEdge,
                           myCurrentNumericalLaneID++, shape, allowed, disallowed);
        break;
    default:
        throw InvalidArgument("A lane with an unknown type occured (" + toString(m_Function) + ")");
    }
    m_pLaneStorage->push_back(lane);
    if (isDepart) {
        m_pDepartLane = lane;
    }
    return lane;
}



MSEdge *
GUIEdgeControlBuilder::buildEdge(const std::string &id) throw() {
    return new GUIEdge(id, myCurrentNumericalEdgeID++, myGlObjectIDStorage);
}

/****************************************************************************/

