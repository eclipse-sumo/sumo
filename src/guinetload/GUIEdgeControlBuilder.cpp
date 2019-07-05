/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIEdgeControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Derivation of NLEdgeControlBuilder which build gui-edges
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUILane.h>
#include <microsim/MSJunction.h>
#include <netload/NLBuilder.h>
#include "GUIEdgeControlBuilder.h"
#include <gui/GUIGlobals.h>


// ===========================================================================
// method definitions
// ===========================================================================
GUIEdgeControlBuilder::GUIEdgeControlBuilder()
    : NLEdgeControlBuilder() {}


GUIEdgeControlBuilder::~GUIEdgeControlBuilder() {}


MSLane*
GUIEdgeControlBuilder::addLane(const std::string& id,
                               double maxSpeed, double length,
                               const PositionVector& shape,
                               double width,
                               SVCPermissions permissions,
                               int index, bool isRampAccel,
                               const std::string& type) {
    MSLane* lane = new GUILane(id, maxSpeed, length, myActiveEdge, myCurrentNumericalLaneID++, shape, width, permissions, index, isRampAccel, type);
    myLaneStorage->push_back(lane);
    myCurrentLaneIndex = index;
    return lane;
}



MSEdge*
GUIEdgeControlBuilder::buildEdge(const std::string& id, const SumoXMLEdgeFunc function,
                                 const std::string& streetName, const std::string& edgeType, const int priority, const double distance) {
    return new GUIEdge(id, myCurrentNumericalEdgeID++, function, streetName, edgeType, priority, distance);
}

/****************************************************************************/

