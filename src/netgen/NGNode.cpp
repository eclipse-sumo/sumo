/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    NGNode.cpp
/// @author  Markus Hartinger
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mar, 2003
///
// A netgen-representation of a node
/****************************************************************************/
#include <config.h>

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


// ===========================================================================
// method definitions
// ===========================================================================
NGNode::NGNode(const std::string& id)
    : Named(id), myXID(-1), myYID(-1), myAmCenter(false), myAmFringe(false) {}


NGNode::NGNode(const std::string& id, int xIDa, int yIDa)
    : Named(id), myXID(xIDa), myYID(yIDa), myAmCenter(false), myAmFringe(false) {}


NGNode::NGNode(const std::string& id, int xIDa, int yIDa, bool amCenter)
    : Named(id), myXID(xIDa), myYID(yIDa), myAmCenter(amCenter), myAmFringe(false) {}


NGNode::~NGNode() {
    NGEdgeList::iterator li;
    while (myLinkList.size() != 0) {
        li = myLinkList.begin();
        delete (*li);
    }
}


NBNode*
NGNode::buildNBNode(NBNetBuilder& nb, const Position& perturb) const {
    Position pos(myPosition + perturb);
    GeoConvHelper::getProcessing().x2cartesian(pos);
    // the center will have no logic!
    if (myAmCenter) {
        return new NBNode(myID, pos, SumoXMLNodeType::NOJUNCTION);
    }
    NBNode* node = nullptr;
    std::string typeS = OptionsCont::getOptions().isSet("default-junction-type") ?
                        OptionsCont::getOptions().getString("default-junction-type") : "";

    if (SUMOXMLDefinitions::NodeTypes.hasString(typeS)) {
        const SumoXMLNodeType type = SUMOXMLDefinitions::NodeTypes.get(typeS);
        node = new NBNode(myID, pos, type);

        // check whether it is a traffic light junction
        if (NBNode::isTrafficLight(type)) {
            const TrafficLightType tlType = SUMOXMLDefinitions::TrafficLightTypes.get(
                                                OptionsCont::getOptions().getString("tls.default-type"));
            NBTrafficLightDefinition* tlDef = new NBOwnTLDef(myID, node, 0, tlType);
            if (!nb.getTLLogicCont().insert(tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError();
            }
        }
    } else {
        // otherwise netbuild may guess SumoXMLNodeType::TRAFFIC_LIGHT without actually building one
        node = new NBNode(myID, pos, SumoXMLNodeType::PRIORITY);
    }
    if (myAmFringe) {
        node->setFringeType(FringeType::OUTER);
    }

    return node;
}


void
NGNode::addLink(NGEdge* link) {
    myLinkList.push_back(link);
}


void
NGNode::removeLink(NGEdge* link) {
    myLinkList.remove(link);
}


bool
NGNode::connected(const NGNode* const node, const bool withDir) const {
    for (NGEdge* ngEdge : myLinkList) {
        if (ngEdge->getStartNode() == this && ngEdge->getEndNode() == node) {
            return true;
        }
        if (!withDir && ngEdge->getEndNode() == this && ngEdge->getStartNode() == node) {
            return true;
        }
    }
    return false;
}


/****************************************************************************/
