/****************************************************************************/
/// @file    NBTrafficLightDefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The base class for traffic light logic definitions
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

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBTrafficLightDefinition.h"
#include <utils/options/OptionsCont.h>
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightLogic.h"
#include "NBContHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
const std::string NBTrafficLightDefinition::DefaultProgramID = "0";

// ===========================================================================
// method definitions
// ===========================================================================
NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string& id,
        const std::vector<NBNode*>& junctions, const std::string& programID,
        SUMOTime offset, TrafficLightType type) :
    Named(id),
    myControlledNodes(junctions),
    mySubID(programID), myOffset(offset),
    myType(type) {
    std::vector<NBNode*>::iterator i = myControlledNodes.begin();
    while (i != myControlledNodes.end()) {
        for (std::vector<NBNode*>::iterator j = i + 1; j != myControlledNodes.end();) {
            if (*i == *j) {
                j = myControlledNodes.erase(j);
            } else {
                j++;
            }
        }
        i++;
    }
    std::sort(myControlledNodes.begin(), myControlledNodes.end(), NBNode::nodes_by_id_sorter());
    for (std::vector<NBNode*>::const_iterator i = junctions.begin(); i != junctions.end(); i++) {
        (*i)->addTrafficLight(this);
    }
}


NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string& id,
        NBNode* junction, const std::string& programID, SUMOTime offset, TrafficLightType type) :
    Named(id),
    mySubID(programID),
    myOffset(offset),
    myType(type) {
    addNode(junction);
    junction->addTrafficLight(this);
}


NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string& id, const std::string& programID,
        SUMOTime offset, TrafficLightType type) :
    Named(id),
    mySubID(programID),
    myOffset(offset),
    myType(type) {}


NBTrafficLightDefinition::~NBTrafficLightDefinition() {}


NBTrafficLightLogic*
NBTrafficLightDefinition::compute(const NBEdgeCont& ec, OptionsCont& oc) {
    // it is not really a traffic light if no incoming edge exists
    if (amInvalid()) {
        // make a copy of myControlledNodes because it will be modified;
        std::vector<NBNode*> nodes = myControlledNodes;
        for (std::vector<NBNode*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
            (*it)->removeTrafficLight(this);
        }
        WRITE_WARNING("The traffic light '" + getID() + "' does not control any links; it will not be build.");
        return 0;
    }
    // compute the time needed to brake
    unsigned int brakingTime = computeBrakingTime(oc.getFloat("tls.yellow.min-decel"));
    // perform the computation depending on whether the traffic light
    //  definition was loaded or shall be computed new completely
    if (oc.isSet("tls.yellow.time")) {
        brakingTime = oc.getInt("tls.yellow.time");
    }
    NBTrafficLightLogic*ret = myCompute(ec, brakingTime);
    ret->addParameter(getMap());
    return ret;
}


bool
NBTrafficLightDefinition::amInvalid() const {
    return myControlledLinks.size() == 0;
}


unsigned int
NBTrafficLightDefinition::computeBrakingTime(SUMOReal minDecel) const {
    SUMOReal vmax = NBContHelper::maxSpeed(myIncomingEdges);
    return (unsigned int)(vmax / minDecel);
}


void
NBTrafficLightDefinition::setParticipantsInformation() {
    // collect the information about participating edges and links
    collectEdges();
    collectLinks();
}


void
NBTrafficLightDefinition::collectEdges() {
    myIncomingEdges.clear();
    EdgeVector myOutgoing;
    // collect the edges from the participating nodes
    for (std::vector<NBNode*>::iterator i = myControlledNodes.begin(); i != myControlledNodes.end(); i++) {
        const EdgeVector& incoming = (*i)->getIncomingEdges();
        copy(incoming.begin(), incoming.end(), back_inserter(myIncomingEdges));
        const EdgeVector& outgoing = (*i)->getOutgoingEdges();
        copy(outgoing.begin(), outgoing.end(), back_inserter(myOutgoing));
    }
    // check which of the edges are completely within the junction
    //  remove these edges from the list of incoming edges
    //  add them to the list of edges lying within the node
    for (EdgeVector::iterator j = myIncomingEdges.begin(); j != myIncomingEdges.end();) {
        NBEdge* edge = *j;
        // an edge lies within the logic if it is outgoing as well as incoming
        EdgeVector::iterator k = find(myOutgoing.begin(), myOutgoing.end(), edge);
        if (k != myOutgoing.end()) {
            if (myControlledInnerEdges.count(edge->getID()) == 0) {
                myEdgesWithin.push_back(edge);
                (*j)->setIsInnerEdge();
                j = myIncomingEdges.erase(j);
                continue;
            }
        }
        ++j;
    }
}


bool
NBTrafficLightDefinition::isLeftMover(const NBEdge* const from, const NBEdge* const to) const {
    // the destination edge may be unused
    if (to == 0) {
        return false;
    }
    // get the node which is holding this connection
    std::vector<NBNode*>::const_iterator i =
        find_if(myControlledNodes.begin(), myControlledNodes.end(),
                NBContHelper::node_with_incoming_finder(from));
    assert(i != myControlledNodes.end());
    NBNode* node = *i;
    return node->isLeftMover(from, to);
}


bool
NBTrafficLightDefinition::mustBrake(const NBEdge* const from, const NBEdge* const to) const {
    std::vector<NBNode*>::const_iterator i =
        find_if(myControlledNodes.begin(), myControlledNodes.end(),
                NBContHelper::node_with_incoming_finder(from));
    assert(i != myControlledNodes.end());
    NBNode* node = *i;
    if (!node->hasOutgoing(to)) {
        return true; // !!!
    }
    return node->mustBrake(from, to, -1);
}


bool
NBTrafficLightDefinition::mustBrake(const NBEdge* const possProhibitedFrom,
                                    const NBEdge* const possProhibitedTo,
                                    const NBEdge* const possProhibitorFrom,
                                    const NBEdge* const possProhibitorTo,
                                    bool regardNonSignalisedLowerPriority) const {
    return forbids(possProhibitorFrom, possProhibitorTo,
                   possProhibitedFrom, possProhibitedTo,
                   regardNonSignalisedLowerPriority);
}


bool
NBTrafficLightDefinition::mustBrake(const NBConnection& possProhibited,
                                    const NBConnection& possProhibitor,
                                    bool regardNonSignalisedLowerPriority) const {
    return forbids(possProhibitor.getFrom(), possProhibitor.getTo(),
                   possProhibited.getFrom(), possProhibited.getTo(),
                   regardNonSignalisedLowerPriority);
}


bool
NBTrafficLightDefinition::forbids(const NBEdge* const possProhibitorFrom,
                                  const NBEdge* const possProhibitorTo,
                                  const NBEdge* const possProhibitedFrom,
                                  const NBEdge* const possProhibitedTo,
                                  bool regardNonSignalisedLowerPriority) const {
    if (possProhibitorFrom == 0 || possProhibitorTo == 0 || possProhibitedFrom == 0 || possProhibitedTo == 0) {
        return false;
    }
    // retrieve both nodes
    std::vector<NBNode*>::const_iterator incoming =
        find_if(myControlledNodes.begin(), myControlledNodes.end(), NBContHelper::node_with_incoming_finder(possProhibitorFrom));
    std::vector<NBNode*>::const_iterator outgoing =
        find_if(myControlledNodes.begin(), myControlledNodes.end(), NBContHelper::node_with_outgoing_finder(possProhibitedTo));
    assert(incoming != myControlledNodes.end());
    NBNode* incnode = *incoming;
    NBNode* outnode = *outgoing;
    EdgeVector::const_iterator i;
    if (incnode != outnode) {
        // the links are located at different nodes
        const EdgeVector& ev1 = possProhibitedTo->getConnectedEdges();
        // go through the following edge,
        //  check whether one of these connections is prohibited
        for (i = ev1.begin(); i != ev1.end(); ++i) {
            std::vector<NBNode*>::const_iterator outgoing2 =
                find_if(myControlledNodes.begin(), myControlledNodes.end(), NBContHelper::node_with_outgoing_finder(*i));
            if (outgoing2 == myControlledNodes.end()) {
                continue;
            }
            NBNode* outnode2 = *outgoing2;
            if (incnode != outnode2) {
                continue;
            }
            bool ret1 = incnode->foes(possProhibitorTo, *i,
                                      possProhibitedFrom, possProhibitedTo);
            bool ret2 = incnode->forbids(possProhibitorFrom, possProhibitorTo,
                                         possProhibitedTo, *i,
                                         regardNonSignalisedLowerPriority);
            bool ret = ret1 || ret2;
            if (ret) {
                return true;
            }
        }

        const EdgeVector& ev2 = possProhibitorTo->getConnectedEdges();
        // go through the following edge,
        //  check whether one of these connections is prohibited
        for (i = ev2.begin(); i != ev2.end(); ++i) {
            std::vector<NBNode*>::const_iterator incoming2 =
                find_if(myControlledNodes.begin(), myControlledNodes.end(), NBContHelper::node_with_incoming_finder(possProhibitorTo));
            if (incoming2 == myControlledNodes.end()) {
                continue;
            }
            NBNode* incnode2 = *incoming2;
            if (incnode2 != outnode) {
                continue;
            }
            bool ret1 = incnode2->foes(possProhibitorTo, *i,
                                       possProhibitedFrom, possProhibitedTo);
            bool ret2 = incnode2->forbids(possProhibitorTo, *i,
                                          possProhibitedFrom, possProhibitedTo,
                                          regardNonSignalisedLowerPriority);
            bool ret = ret1 || ret2;
            if (ret) {
                return true;
            }
        }
        return false;
    }
    // both links are located at the same node
    //  check using this node's information
    return incnode->forbids(possProhibitorFrom, possProhibitorTo,
                            possProhibitedFrom, possProhibitedTo,
                            regardNonSignalisedLowerPriority);
}


bool
NBTrafficLightDefinition::foes(const NBEdge* const from1, const NBEdge* const to1,
                               const NBEdge* const from2, const NBEdge* const to2) const {
    if (to1 == 0 || to2 == 0) {
        return false;
    }
    // retrieve both nodes (it is possible that a connection
    std::vector<NBNode*>::const_iterator incoming =
        find_if(myControlledNodes.begin(), myControlledNodes.end(),
                NBContHelper::node_with_incoming_finder(from1));
    std::vector<NBNode*>::const_iterator outgoing =
        find_if(myControlledNodes.begin(), myControlledNodes.end(),
                NBContHelper::node_with_outgoing_finder(to1));
    assert(incoming != myControlledNodes.end());
    NBNode* incnode = *incoming;
    NBNode* outnode = *outgoing;
    if (incnode != outnode) {
        return false;
    }
    return incnode->foes(from1, to1, from2, to2);
}


void
NBTrafficLightDefinition::addNode(NBNode* node) {
    if (std::find(myControlledNodes.begin(), myControlledNodes.end(), node) == myControlledNodes.end()) {
        myControlledNodes.push_back(node);
        std::sort(myControlledNodes.begin(), myControlledNodes.end(), NBNode::nodes_by_id_sorter());
        node->addTrafficLight(this);
    }
}


void
NBTrafficLightDefinition::removeNode(NBNode* node) {
    std::vector<NBNode*>::iterator i = std::find(myControlledNodes.begin(), myControlledNodes.end(), node);
    if (i != myControlledNodes.end()) {
        myControlledNodes.erase(i);
    }
    // !!! remove in node?
}


void
NBTrafficLightDefinition::addControlledInnerEdges(const std::vector<std::string>& edges) {
    myControlledInnerEdges.insert(edges.begin(), edges.end());
}


const EdgeVector&
NBTrafficLightDefinition::getIncomingEdges() const {
    return myIncomingEdges;
}


void
NBTrafficLightDefinition::collectAllLinks() {
    myControlledLinks.clear();
    // build the list of links which are controled by the traffic light
    for (EdgeVector::iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        NBEdge* incoming = *i;
        unsigned int noLanes = incoming->getNumLanes();
        for (unsigned int j = 0; j < noLanes; j++) {
            std::vector<NBEdge::Connection> connected = incoming->getConnectionsFromLane(j);
            for (std::vector<NBEdge::Connection>::iterator k = connected.begin(); k != connected.end(); k++) {
                const NBEdge::Connection& el = *k;
                if (incoming->mayBeTLSControlled(el.fromLane, el.toEdge, el.toLane)) {
                    if (el.toEdge != 0 && el.toLane >= (int) el.toEdge->getNumLanes()) {
                        throw ProcessError("Connection '" + incoming->getID() + "_" + toString(j) + "->" + el.toEdge->getID() + "_" + toString(el.toLane) + "' yields in a not existing lane.");
                    }
                    int tlIndex = (int)myControlledLinks.size();
                    myControlledLinks.push_back(NBConnection(incoming, el.fromLane, el.toEdge, el.toLane, tlIndex));
                }
            }
        }
    }
}

/****************************************************************************/

