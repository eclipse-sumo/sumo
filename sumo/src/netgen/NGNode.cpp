/****************************************************************************/
/// @file    NGNode.cpp
/// @author  Markus Hartinger
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mar, 2003
/// @version $Id$
///
// A netgen-representation of a node
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <algorithm>
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
#include "NGNode.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NGNode::NGNode(const std::string& id)
    : Named(id), xID(-1), yID(-1), myAmCenter(false) {}


NGNode::NGNode(const std::string& id, int xIDa, int yIDa)
    : Named(id), xID(xIDa), yID(yIDa), myAmCenter(false) {}


NGNode::NGNode(const std::string& id, int xIDa, int yIDa, bool amCenter)
    : Named(id), xID(xIDa), yID(yIDa), myAmCenter(amCenter) {}


NGNode::~NGNode() {
    NGEdgeList::iterator li;
    while (LinkList.size() != 0) {
        li = LinkList.begin();
        delete(*li);
    }
}


NBNode*
NGNode::buildNBNode(NBNetBuilder& nb) const {
    Position pos(myPosition);
    GeoConvHelper::getProcessing().x2cartesian(pos);
    // the center will have no logic!
    if (myAmCenter) {
        return new NBNode(myID, pos, NODETYPE_NOJUNCTION);
    }
    NBNode* node = 0;
    std::string typeS = OptionsCont::getOptions().isSet("default-junction-type") ?
                        OptionsCont::getOptions().getString("default-junction-type") : "";

    if (SUMOXMLDefinitions::NodeTypes.hasString(typeS)) {
        SumoXMLNodeType type = SUMOXMLDefinitions::NodeTypes.get(typeS);
        node = new NBNode(myID, pos, type);

        // check whether it is a traffic light junction
        if (type == NODETYPE_TRAFFIC_LIGHT || type == NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) {
            TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(
                                        OptionsCont::getOptions().getString("tls.default-type"));
            NBTrafficLightDefinition* tlDef = new NBOwnTLDef(myID, node, 0, type);
            if (!nb.getTLLogicCont().insert(tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError();
            }
        }
    } else {
        // otherwise netbuild may guess NODETYPE_TRAFFIC_LIGHT without actually building one
        node = new NBNode(myID, pos, NODETYPE_PRIORITY);
    }

    return node;
}


void
NGNode::addLink(NGEdge* link) {
    LinkList.push_back(link);
}


void
NGNode::removeLink(NGEdge* link) {
    LinkList.remove(link);
}


bool
NGNode::connected(NGNode* node) const {
    for (NGEdgeList::const_iterator i = LinkList.begin(); i != LinkList.end(); ++i) {
        if (find(node->LinkList.begin(), node->LinkList.end(), *i) != node->LinkList.end()) {
            return true;
        }
    }
    return false;
}


/****************************************************************************/

