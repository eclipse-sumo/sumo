/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2003-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NGEdge.cpp
/// @author  Markus Hartinger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mar, 2003
///
// A netgen-representation of an edge
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <utils/common/RandHelper.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include "NGEdge.h"
#include "NGNode.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NGEdge-definitions
// ---------------------------------------------------------------------------
NGEdge::NGEdge(const std::string& id, NGNode* startNode, NGNode* endNode, const std::string& reverseID) :
    Named(id), myStartNode(startNode), myEndNode(endNode), myReverseID(reverseID == "" ? "-" + id : reverseID) {
    myStartNode->addLink(this);
    myEndNode->addLink(this);
}


NGEdge::~NGEdge() {
    myStartNode->removeLink(this);
    myEndNode->removeLink(this);
}


NBEdge*
NGEdge::buildNBEdge(NBNetBuilder& nb, std::string type, const bool reversed) const {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.getBool("random-type") && nb.getTypeCont().size() > 1) {
        auto it = nb.getTypeCont().begin();
        std::advance(it, RandHelper::rand((int)nb.getTypeCont().size()));
        type = it->first;
    }
    int priority = nb.getTypeCont().getEdgeTypePriority(type);
    if (priority > 1 && oc.getBool("rand.random-priority")) {
        priority = RandHelper::rand(priority) + 1;
    }
    int lanenumber = nb.getTypeCont().getEdgeTypeNumLanes(type);
    if (lanenumber > 1 && oc.getBool("rand.random-lanenumber")) {
        lanenumber = RandHelper::rand(lanenumber) + 1;
    }

    SVCPermissions permissions = nb.getTypeCont().getEdgeTypePermissions(type);
    LaneSpreadFunction lsf = nb.getTypeCont().getEdgeTypeSpreadType(type);
    if (isRailway(permissions) &&  nb.getTypeCont().getEdgeTypeIsOneWay(type)) {
        lsf = LaneSpreadFunction::CENTER;
    }
    const double maxSegmentLength = oc.getFloat("geometry.max-segment-length");
    NBNode* from = nb.getNodeCont().retrieve(reversed ? myEndNode->getID() : myStartNode->getID());
    NBNode* to = nb.getNodeCont().retrieve(reversed ? myStartNode->getID() : myEndNode->getID());
    PositionVector shape;
    if (maxSegmentLength > 0) {
        shape.push_back(from->getPosition());
        shape.push_back(to->getPosition());
        // shape is already cartesian but we must use a copy because the original will be modified
        NBNetBuilder::addGeometrySegments(shape, PositionVector(shape), maxSegmentLength);
    }
    NBEdge* result = new NBEdge(
        reversed ? myReverseID : myID,
        from, to,
        type, nb.getTypeCont().getEdgeTypeSpeed(type), NBEdge::UNSPECIFIED_FRICTION, lanenumber,
        priority, nb.getTypeCont().getEdgeTypeWidth(type), NBEdge::UNSPECIFIED_OFFSET, shape, lsf);
    result->setPermissions(permissions);
    return result;
}


/****************************************************************************/
