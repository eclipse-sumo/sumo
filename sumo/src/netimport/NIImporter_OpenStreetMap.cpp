/****************************************************************************/
/// @file    NIImporter_OpenStreetMap.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in OpenStreetMap format
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
#include <algorithm>
#include <set>
#include <functional>
#include <sstream>
#include <limits>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBOwnTLDef.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include "NILoader.h"
#include "NIImporter_OpenStreetMap.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_LAYER_ELEVATION

// ---------------------------------------------------------------------------
// static members
// ---------------------------------------------------------------------------
const SUMOReal NIImporter_OpenStreetMap::MAXSPEED_UNGIVEN = -1;

const long long int NIImporter_OpenStreetMap::INVALID_ID = std::numeric_limits<long long int>::max();

// ===========================================================================
// Private classes
// ===========================================================================

/** @brief Functor which compares two Edges
 */
class NIImporter_OpenStreetMap::CompareEdges {
public:
    bool operator()(const Edge* e1, const Edge* e2) const {
        if (e1->myHighWayType != e2->myHighWayType) {
            return e1->myHighWayType > e2->myHighWayType;
        }
        if (e1->myNoLanes != e2->myNoLanes) {
            return e1->myNoLanes > e2->myNoLanes;
        }
        if (e1->myNoLanesForward != e2->myNoLanesForward) {
            return e1->myNoLanesForward > e2->myNoLanesForward;
        }
        if (e1->myMaxSpeed != e2->myMaxSpeed) {
            return e1->myMaxSpeed > e2->myMaxSpeed;
        }
        if (e1->myIsOneWay != e2->myIsOneWay) {
            return e1->myIsOneWay > e2->myIsOneWay;
        }
        return e1->myCurrentNodes > e2->myCurrentNodes;
    }
};

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
const std::string NIImporter_OpenStreetMap::compoundTypeSeparator("|");


void
NIImporter_OpenStreetMap::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    NIImporter_OpenStreetMap importer;
    importer.load(oc, nb);
}


NIImporter_OpenStreetMap::NIImporter_OpenStreetMap() {}


NIImporter_OpenStreetMap::~NIImporter_OpenStreetMap() {
    // delete nodes
    for (std::set<NIOSMNode*, CompareNodes>::iterator i = myUniqueNodes.begin(); i != myUniqueNodes.end(); i++) {
        delete *i;
    }
    // delete edges
    for (std::map<long long int, Edge*>::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        delete(*i).second;
    }
}


void
NIImporter_OpenStreetMap::load(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("osm-files")) {
        return;
    }
    /* Parse file(s)
     * Each file is parsed twice: first for nodes, second for edges. */
    std::vector<std::string> files = oc.getStringVector("osm-files");
    // load nodes, first
    NodesHandler nodesHandler(myOSMNodes, myUniqueNodes, oc.getBool("osm.elevation"));
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // nodes
        if (!FileHelpers::isReadable(*file)) {
            WRITE_ERROR("Could not open osm-file '" + *file + "'.");
            return;
        }
        nodesHandler.setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing nodes from osm-file '" + *file + "'");
        if (!XMLSubSys::runParser(nodesHandler, *file)) {
            return;
        }
        PROGRESS_DONE_MESSAGE();
    }
    // load edges, then
    EdgesHandler edgesHandler(myOSMNodes, myEdges);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // edges
        edgesHandler.setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing edges from osm-file '" + *file + "'");
        XMLSubSys::runParser(edgesHandler, *file);
        PROGRESS_DONE_MESSAGE();
    }

    /* Remove duplicate edges with the same shape and attributes */
    if (!oc.getBool("osm.skip-duplicates-check")) {
        PROGRESS_BEGIN_MESSAGE("Removing duplicate edges");
        if (myEdges.size() > 1) {
            std::set<const Edge*, CompareEdges> dupsFinder;
            for (std::map<long long int, Edge*>::iterator it = myEdges.begin(); it != myEdges.end();) {
                if (dupsFinder.count(it->second) > 0) {
                    WRITE_MESSAGE("Found duplicate edges. Removing " + toString(it->first));
                    delete it->second;
                    myEdges.erase(it++);
                } else {
                    dupsFinder.insert(it->second);
                    it++;
                }
            }
        }
        PROGRESS_DONE_MESSAGE();
    }

    /* Mark which nodes are used (by edges or traffic lights).
     * This is necessary to detect which OpenStreetMap nodes are for
     * geometry only */
    std::map<long long int, int> nodeUsage;
    // Mark which nodes are used by edges (begin and end)
    for (std::map<long long int, Edge*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        Edge* e = (*i).second;
        assert(e->myCurrentIsRoad);
        for (std::vector<long long int>::const_iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
            if (nodeUsage.find(*j) == nodeUsage.end()) {
                nodeUsage[*j] = 0;
            }
            nodeUsage[*j] = nodeUsage[*j] + 1;
        }
    }
    // Mark which nodes are used by traffic lights
    for (std::map<long long int, NIOSMNode*>::const_iterator nodesIt = myOSMNodes.begin(); nodesIt != myOSMNodes.end(); ++nodesIt) {
        if (nodesIt->second->tlsControlled /* || nodesIt->second->railwayCrossing*/) {
            // If the key is not found in the map, the value is automatically
            // initialized with 0.
            nodeUsage[nodesIt->first] += 1;
        }
    }

    /* Instantiate edges
     * Only those nodes in the middle of an edge which are used by more than
     * one edge are instantiated. Other nodes are considered as geometry nodes. */
    NBNodeCont& nc = nb.getNodeCont();
    NBTrafficLightLogicCont& tlsc = nb.getTLLogicCont();
    for (std::map<long long int, Edge*>::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        Edge* e = (*i).second;
        assert(e->myCurrentIsRoad);
        if (e->myCurrentNodes.size() < 2) {
            WRITE_WARNING("Discarding way '" + toString(e->id) + "' because it has only " +
                          toString(e->myCurrentNodes.size()) + " node(s)");
            continue;
        }
        // build nodes;
        //  - the from- and to-nodes must be built in any case
        //  - the in-between nodes are only built if more than one edge references them
        NBNode* currentFrom = insertNodeChecking(*e->myCurrentNodes.begin(), nc, tlsc);
        NBNode* last = insertNodeChecking(*(e->myCurrentNodes.end() - 1), nc, tlsc);
        int running = 0;
        std::vector<long long int> passed;
        for (std::vector<long long int>::iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
            passed.push_back(*j);
            if (nodeUsage[*j] > 1 && j != e->myCurrentNodes.end() - 1 && j != e->myCurrentNodes.begin()) {
                NBNode* currentTo = insertNodeChecking(*j, nc, tlsc);
                running = insertEdge(e, running, currentFrom, currentTo, passed, nb);
                currentFrom = currentTo;
                passed.clear();
                passed.push_back(*j);
            }
        }
        if (running == 0) {
            running = -1;
        }
        insertEdge(e, running, currentFrom, last, passed, nb);
    }

    const SUMOReal layerElevation = oc.getFloat("osm.layer-elevation");
    if (layerElevation > 0) {
        reconstructLayerElevation(layerElevation, nb);
    }

    // load relations (after edges are built since we want to apply
    // turn-restrictions directly to NBEdges)
    RelationHandler relationHandler(myOSMNodes, myEdges);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // relations
        relationHandler.setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing relations from osm-file '" + *file + "'");
        XMLSubSys::runParser(relationHandler, *file);
        PROGRESS_DONE_MESSAGE();
    }
}


NBNode*
NIImporter_OpenStreetMap::insertNodeChecking(long long int id, NBNodeCont& nc, NBTrafficLightLogicCont& tlsc) {
    NBNode* node = nc.retrieve(toString(id));
    if (node == 0) {
        NIOSMNode* n = myOSMNodes.find(id)->second;
        Position pos(n->lon, n->lat, n->ele);
        if (!NBNetBuilder::transformCoordinate(pos, true)) {
            WRITE_ERROR("Unable to project coordinates for junction '" + toString(id) + "'.");
            return 0;
        }
        node = new NBNode(toString(id), pos);
        if (!nc.insert(node)) {
            WRITE_ERROR("Could not insert junction '" + toString(id) + "'.");
            delete node;
            return 0;
        }
        n->node = node;
        if (n->railwayCrossing) {
            node->reinit(pos, NODETYPE_RAIL_CROSSING);
        } else if (n->tlsControlled) {
            // ok, this node is a traffic light node where no other nodes
            //  participate
            // @note: The OSM-community has not settled on a schema for differentiating between fixed and actuated lights
            TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(OptionsCont::getOptions().getString("tls.default-type"));
            NBOwnTLDef* tlDef = new NBOwnTLDef(toString(id), node, 0, type);
            if (!tlsc.insert(tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError("Could not allocate tls '" + toString(id) + "'.");
            }
        }
    }
    return node;
}


int
NIImporter_OpenStreetMap::insertEdge(Edge* e, int index, NBNode* from, NBNode* to,
                                     const std::vector<long long int>& passed, NBNetBuilder& nb) {
    NBNodeCont& nc = nb.getNodeCont();
    NBEdgeCont& ec = nb.getEdgeCont();
    NBTypeCont& tc = nb.getTypeCont();
    NBTrafficLightLogicCont& tlsc = nb.getTLLogicCont();
    // patch the id
    std::string id = toString(e->id);
    if (from == 0 || to == 0) {
        WRITE_ERROR("Discarding edge '" + id + "' because the nodes could not be built.");
        return index;
    }
    if (index >= 0) {
        id = id + "#" + toString(index);
    } else {
        index = 0;
    }
    if (from == to) {
        assert(passed.size() >= 2);
        if (passed.size() == 2) {
            WRITE_WARNING("Discarding edge '" + id + "' which connects two identical nodes without geometry.");
            return index;
        }
        // in the special case of a looped way split again using passed
        int intermediateIndex = (int)passed.size() / 2;
        NBNode* intermediate = insertNodeChecking(passed[intermediateIndex], nc, tlsc);
        std::vector<long long int> part1(passed.begin(), passed.begin() + intermediateIndex + 1);
        std::vector<long long int> part2(passed.begin() + intermediateIndex, passed.end());
        index = insertEdge(e, index, from, intermediate, part1, nb);
        return insertEdge(e, index, intermediate, to, part2, nb);
    }
    const int newIndex = index + 1;

    // convert the shape
    PositionVector shape;
    for (std::vector<long long int>::const_iterator i = passed.begin(); i != passed.end(); ++i) {
        NIOSMNode* n = myOSMNodes.find(*i)->second;
        Position pos(n->lon, n->lat, n->ele);
        shape.push_back(pos);
    }
    if (!NBNetBuilder::transformCoordinates(shape)) {
        WRITE_ERROR("Unable to project coordinates for edge '" + id + "'.");
    }

    std::string type = e->myHighWayType;
    if (!tc.knows(type)) {
        if (myUnusableTypes.count(type) > 0) {
            return newIndex;
        } else if (myKnownCompoundTypes.count(type) > 0) {
            type = myKnownCompoundTypes[type];
        } else {
            // this edge has a type which does not yet exist in the TypeContainer
            StringTokenizer tok = StringTokenizer(type, compoundTypeSeparator);
            std::vector<std::string> types;
            while (tok.hasNext()) {
                std::string t = tok.next();
                if (tc.knows(t)) {
                    if (std::find(types.begin(), types.end(), t) == types.end()) {
                        types.push_back(t);
                    }
                } else if (tok.size() > 1) {
                    WRITE_WARNING("Discarding unknown compound '" + t + "' in type '" + type + "' (first occurence for edge '" + id + "').");
                }
            }
            if (types.size() == 0) {
                WRITE_WARNING("Discarding unusable type '" + type + "' (first occurence for edge '" + id + "').");
                myUnusableTypes.insert(type);
                return newIndex;
            } else {
                const std::string newType = joinToString(types, "|");
                if (tc.knows(newType)) {
                    myKnownCompoundTypes[type] = newType;
                    type = newType;
                } else if (myKnownCompoundTypes.count(newType) > 0) {
                    type = myKnownCompoundTypes[newType];
                } else {
                    // build a new type by merging all values
                    int numLanes = 0;
                    SUMOReal maxSpeed = 0;
                    int prio = 0;
                    SUMOReal width = NBEdge::UNSPECIFIED_WIDTH;
                    SUMOReal sidewalkWidth = NBEdge::UNSPECIFIED_WIDTH;
                    SUMOReal bikelaneWidth = NBEdge::UNSPECIFIED_WIDTH;
                    bool defaultIsOneWay = false;
                    SVCPermissions permissions = 0;
                    bool discard = true;
                    for (std::vector<std::string>::iterator it = types.begin(); it != types.end(); it++) {
                        if (!tc.getShallBeDiscarded(*it)) {
                            numLanes = MAX2(numLanes, tc.getNumLanes(*it));
                            maxSpeed = MAX2(maxSpeed, tc.getSpeed(*it));
                            prio = MAX2(prio, tc.getPriority(*it));
                            defaultIsOneWay &= tc.getIsOneWay(*it);
                            permissions |= tc.getPermissions(*it);
                            width = MAX2(width, tc.getWidth(*it));
                            sidewalkWidth = MAX2(sidewalkWidth, tc.getSidewalkWidth(*it));
                            bikelaneWidth = MAX2(bikelaneWidth, tc.getBikeLaneWidth(*it));
                            discard = false;
                        }
                    }
                    if (width != NBEdge::UNSPECIFIED_WIDTH) {
                        width = MAX2(width, SUMO_const_laneWidth);
                    }
                    if (discard) {
                        WRITE_WARNING("Discarding compound type '" + newType + "' (first occurence for edge '" + id + "').");
                        myUnusableTypes.insert(newType);
                        return newIndex;
                    } else {
                        WRITE_MESSAGE("Adding new type '" + type + "' (first occurence for edge '" + id + "').");
                        tc.insert(newType, numLanes, maxSpeed, prio, permissions, width, defaultIsOneWay, sidewalkWidth, bikelaneWidth);
                        for (std::vector<std::string>::iterator it = types.begin(); it != types.end(); it++) {
                            if (!tc.getShallBeDiscarded(*it)) {
                                tc.copyRestrictionsAndAttrs(*it, newType);
                            }
                        }
                        myKnownCompoundTypes[type] = newType;
                        type = newType;
                    }
                }
            }
        }
    }

    // otherwise it is not an edge and will be ignored
    bool ok = true;
    int numLanesForward = tc.getNumLanes(type);
    int numLanesBackward = tc.getNumLanes(type);
    SUMOReal speed = tc.getSpeed(type);
    bool defaultsToOneWay = tc.getIsOneWay(type);
    SVCPermissions forwardPermissions = tc.getPermissions(type);
    SVCPermissions backwardPermissions = tc.getPermissions(type);
    SUMOReal forwardWidth = tc.getWidth(type);
    SUMOReal backwardWidth = tc.getWidth(type);
    const bool addSidewalk = (tc.getSidewalkWidth(type) != NBEdge::UNSPECIFIED_WIDTH);
    const bool addBikeLane = (tc.getBikeLaneWidth(type) != NBEdge::UNSPECIFIED_WIDTH);
    // check directions
    bool addForward = true;
    bool addBackward = true;
    if (e->myIsOneWay == "true" || e->myIsOneWay == "yes" || e->myIsOneWay == "1" || (defaultsToOneWay && e->myIsOneWay != "no" && e->myIsOneWay != "false" && e->myIsOneWay != "0")) {
        addBackward = false;
    }
    if (e->myIsOneWay == "-1" || e->myIsOneWay == "reverse") {
        // one-way in reversed direction of way
        addForward = false;
        addBackward = true;
    }
    if (e->myIsOneWay != "" && e->myIsOneWay != "false" && e->myIsOneWay != "no" && e->myIsOneWay != "true" && e->myIsOneWay != "yes" && e->myIsOneWay != "-1" && e->myIsOneWay != "1" && e->myIsOneWay != "reverse") {
        WRITE_WARNING("New value for oneway found: " + e->myIsOneWay);
    }
    // if we had been able to extract the number of lanes, override the highway type default
    if (e->myNoLanes > 0) {
        if (addForward && !addBackward) {
            numLanesForward = e->myNoLanes;
        } else if (!addForward && addBackward) {
            numLanesBackward = e->myNoLanes;
        } else {
            if (e->myNoLanesForward > 0) {
                numLanesForward = e->myNoLanesForward;
            } else if (e->myNoLanesForward < 0) {
                numLanesForward = e->myNoLanes + e->myNoLanesForward;
            } else {
                numLanesForward = (int)std::ceil(e->myNoLanes / 2.0);
            }
            numLanesBackward = e->myNoLanes - numLanesForward;
            // sometimes ways are tagged according to their physical width of a single
            // lane but they are intended for traffic in both directions
            numLanesForward = MAX2(1, numLanesForward);
            numLanesBackward = MAX2(1, numLanesBackward);
        }
    } else if (e->myNoLanes == 0) {
        WRITE_WARNING("Skipping edge '" + id + "' because it has zero lanes.");
        ok = false;
    }
    // if we had been able to extract the maximum speed, override the type's default
    if (e->myMaxSpeed != MAXSPEED_UNGIVEN) {
        speed = (SUMOReal)(e->myMaxSpeed / 3.6);
    }
    if (speed <= 0) {
        WRITE_WARNING("Skipping edge '" + id + "' because it has speed " + toString(speed));
        ok = false;
    }
    // deal with cycleways that run in the opposite direction of a one-way street
    if (addBikeLane) {
        if (!addForward && (e->myCyclewayType & WAY_FORWARD) != 0) {
            addForward = true;
            forwardPermissions = SVC_BICYCLE;
            forwardWidth = tc.getBikeLaneWidth(type);
            numLanesForward = 1;
            // do not add an additional cycle lane
            e->myCyclewayType = (WayType)(e->myCyclewayType & !WAY_FORWARD);
        }
        if (!addBackward && (e->myCyclewayType & WAY_BACKWARD) != 0) {
            addBackward = true;
            backwardPermissions = SVC_BICYCLE;
            backwardWidth = tc.getBikeLaneWidth(type);
            numLanesBackward = 1;
            // do not add an additional cycle lane
            e->myCyclewayType = (WayType)(e->myCyclewayType & !WAY_BACKWARD);
        }
    }
    // deal with busways that run in the opposite direction of a one-way street
    if (!addForward && (e->myBuswayType & WAY_FORWARD) != 0) {
        addForward = true;
        forwardPermissions = SVC_BUS;
        numLanesForward = 1;
    }
    if (!addBackward && (e->myBuswayType & WAY_BACKWARD) != 0) {
        addBackward = true;
        backwardPermissions = SVC_BUS;
        numLanesBackward = 1;
    }

    if (ok) {
        LaneSpreadFunction lsf = (addBackward || OptionsCont::getOptions().getBool("osm.oneway-spread-right")) ? LANESPREAD_RIGHT : LANESPREAD_CENTER;
        id = StringUtils::escapeXML(id);
        if (addForward) {
            assert(numLanesForward > 0);
            NBEdge* nbe = new NBEdge(id, from, to, type, speed, numLanesForward, tc.getPriority(type),
                                     forwardWidth, NBEdge::UNSPECIFIED_OFFSET, shape,
                                     StringUtils::escapeXML(e->streetName), toString(e->id), lsf, true);
            nbe->setPermissions(forwardPermissions);
            if ((e->myBuswayType & WAY_FORWARD) != 0) {
                nbe->setPermissions(SVC_BUS, 0);
            }
            if (addBikeLane && (e->myCyclewayType == WAY_UNKNOWN || (e->myCyclewayType & WAY_FORWARD) != 0)) {
                nbe->addBikeLane(tc.getBikeLaneWidth(type));
            } else if (nbe->getPermissions(0) == SVC_BUS) {
                // bikes drive on buslanes if no separate cycle lane is available
                nbe->setPermissions(SVC_BUS | SVC_BICYCLE, 0);
            }
            if (addSidewalk) {
                nbe->addSidewalk(tc.getSidewalkWidth(type));
            }
            nbe->addParameter(*e);
            if (!ec.insert(nbe)) {
                delete nbe;
                throw ProcessError("Could not add edge '" + id + "'.");
            }
        }
        if (addBackward) {
            assert(numLanesBackward > 0);
            NBEdge* nbe = new NBEdge("-" + id, to, from, type, speed, numLanesBackward, tc.getPriority(type),
                                     backwardWidth, NBEdge::UNSPECIFIED_OFFSET, shape.reverse(),
                                     StringUtils::escapeXML(e->streetName), toString(e->id), lsf, true);
            nbe->setPermissions(backwardPermissions);
            if ((e->myBuswayType & WAY_BACKWARD) != 0) {
                nbe->setPermissions(SVC_BUS, 0);
            }
            if (addBikeLane && (e->myCyclewayType == WAY_UNKNOWN || (e->myCyclewayType & WAY_BACKWARD) != 0)) {
                nbe->addBikeLane(tc.getBikeLaneWidth(type));
            } else if (nbe->getPermissions(0) == SVC_BUS) {
                // bikes drive on buslanes if no separate cycle lane is available
                nbe->setPermissions(SVC_BUS | SVC_BICYCLE, 0);
            }
            if (addSidewalk) {
                nbe->addSidewalk(tc.getSidewalkWidth(type));
            }
            nbe->addParameter(*e);
            if (!ec.insert(nbe)) {
                delete nbe;
                throw ProcessError("Could not add edge '-" + id + "'.");
            }
        }
    }
    return newIndex;
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_OpenStreetMap::NodesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_OpenStreetMap::NodesHandler::NodesHandler(
    std::map<long long int, NIOSMNode*>& toFill,
    std::set<NIOSMNode*, CompareNodes>& uniqueNodes,
    bool importElevation) :
    SUMOSAXHandler("osm - file"),
    myToFill(toFill),
    myLastNodeID(-1),
    myIsInValidNodeTag(false),
    myHierarchyLevel(0),
    myUniqueNodes(uniqueNodes),
    myImportElevation(importElevation) {
}


NIImporter_OpenStreetMap::NodesHandler::~NodesHandler() {}


void
NIImporter_OpenStreetMap::NodesHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    ++myHierarchyLevel;
    if (element == SUMO_TAG_NODE) {
        bool ok = true;
        if (myHierarchyLevel != 2) {
            WRITE_ERROR("Node element on wrong XML hierarchy level (id='" + toString(attrs.get<long long int>(SUMO_ATTR_ID, 0, ok)) + "', level='" + toString(myHierarchyLevel) + "').");
            return;
        }
        long long int id = attrs.get<long long int>(SUMO_ATTR_ID, 0, ok);
        std::string action = attrs.hasAttribute("action") ? attrs.getStringSecure("action", "") : "";
        if (action == "delete") {
            return;
        }
        if (!ok) {
            return;
        }
        myLastNodeID = -1;
        if (myToFill.find(id) == myToFill.end()) {
            myLastNodeID = id;
            // assume we are loading multiple files...
            //  ... so we won't report duplicate nodes
            bool ok = true;
            double tlat, tlon;
            std::istringstream lon(attrs.get<std::string>(SUMO_ATTR_LON, toString(id).c_str(), ok));
            if (!ok) {
                return;
            }
            lon >> tlon;
            if (lon.fail()) {
                WRITE_ERROR("Node's '" + toString(id) + "' lon information is not numeric.");
                return;
            }
            std::istringstream lat(attrs.get<std::string>(SUMO_ATTR_LAT, toString(id).c_str(), ok));
            if (!ok) {
                return;
            }
            lat >> tlat;
            if (lat.fail()) {
                WRITE_ERROR("Node's '" + toString(id) + "' lat information is not numeric.");
                return;
            }
            NIOSMNode* toAdd = new NIOSMNode(id, tlon, tlat);
            myIsInValidNodeTag = true;

            std::set<NIOSMNode*, CompareNodes>::iterator similarNode = myUniqueNodes.find(toAdd);
            if (similarNode == myUniqueNodes.end()) {
                myUniqueNodes.insert(toAdd);
            } else {
                delete toAdd;
                toAdd = *similarNode;
                WRITE_MESSAGE("Found duplicate nodes. Substituting " + toString(id) + " with " + toString(toAdd->id));
            }
            myToFill[id] = toAdd;
        }
    }
    if (element == SUMO_TAG_TAG && myIsInValidNodeTag) {
        if (myHierarchyLevel != 3) {
            WRITE_ERROR("Tag element on wrong XML hierarchy level.");
            return;
        }
        bool ok = true;
        std::string key = attrs.get<std::string>(SUMO_ATTR_K, toString(myLastNodeID).c_str(), ok, false);
        // we check whether the key is relevant (and we really need to transcode the value) to avoid hitting #1636
        if (key == "highway" || key == "ele" || key == "crossing" || key == "railway") {
            std::string value = attrs.get<std::string>(SUMO_ATTR_V, toString(myLastNodeID).c_str(), ok, false);
            if (key == "highway" && value.find("traffic_signal") != std::string::npos) {
                myToFill[myLastNodeID]->tlsControlled = true;
            } else if (key == "crossing" && value.find("traffic_signals") != std::string::npos) {
                myToFill[myLastNodeID]->tlsControlled = true;
            } else if (key == "railway" && value.find("crossing") != std::string::npos) {
                myToFill[myLastNodeID]->railwayCrossing = true;
            } else if (myImportElevation && key == "ele") {
                try {
                    myToFill[myLastNodeID]->ele = TplConvert::_2SUMOReal(value.c_str());
                } catch (...) {
                    WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in node '" +
                                  toString(myLastNodeID) + "'.");
                }
            }
        }
    }
}


void
NIImporter_OpenStreetMap::NodesHandler::myEndElement(int element) {
    if (element == SUMO_TAG_NODE && myHierarchyLevel == 2) {
        myLastNodeID = -1;
        myIsInValidNodeTag = false;
    }
    --myHierarchyLevel;
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_OpenStreetMap::EdgesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_OpenStreetMap::EdgesHandler::EdgesHandler(
    const std::map<long long int, NIOSMNode*>& osmNodes,
    std::map<long long int, Edge*>& toFill) :
    SUMOSAXHandler("osm - file"),
    myOSMNodes(osmNodes),
    myEdgeMap(toFill) {
    mySpeedMap["signals"] = MAXSPEED_UNGIVEN;
    mySpeedMap["none"] = 300.;
    mySpeedMap["no"] = 300.;
    mySpeedMap["walk"] = 5.;
    mySpeedMap["DE:rural"] = 100.;
    mySpeedMap["DE:urban"] = 50.;
    mySpeedMap["DE:living_street"] = 10.;

}


NIImporter_OpenStreetMap::EdgesHandler::~EdgesHandler() {
}


void
NIImporter_OpenStreetMap::EdgesHandler::myStartElement(int element,
        const SUMOSAXAttributes& attrs) {
    myParentElements.push_back(element);
    // parse "way" elements
    if (element == SUMO_TAG_WAY) {
        bool ok = true;
        long long int id = attrs.get<long long int>(SUMO_ATTR_ID, 0, ok);
        std::string action = attrs.hasAttribute("action") ? attrs.getStringSecure("action", "") : "";
        if (action == "delete") {
            myCurrentEdge = 0;
            return;
        }
        if (!ok) {
            myCurrentEdge = 0;
            return;
        }
        myCurrentEdge = new Edge(id);
    }
    // parse "nd" (node) elements
    if (element == SUMO_TAG_ND) {
        bool ok = true;
        long long int ref = attrs.get<long long int>(SUMO_ATTR_REF, 0, ok);
        if (ok) {
            std::map<long long int, NIOSMNode*>::const_iterator node = myOSMNodes.find(ref);
            if (node == myOSMNodes.end()) {
                WRITE_WARNING("The referenced geometry information (ref='" + toString(ref) + "') is not known");
                return;
            } else {
                ref = node->second->id; // node may have been substituted
                if (myCurrentEdge->myCurrentNodes.size() == 0 ||
                        myCurrentEdge->myCurrentNodes.back() != ref) { // avoid consecutive duplicates
                    myCurrentEdge->myCurrentNodes.push_back(ref);
                }
            }
        }
    }
    // parse values
    if (element == SUMO_TAG_TAG && myParentElements.size() > 2 && myParentElements[myParentElements.size() - 2] == SUMO_TAG_WAY) {
        if (myCurrentEdge == 0) {
            return;
        }
        bool ok = true;
        std::string key = attrs.get<std::string>(SUMO_ATTR_K, toString(myCurrentEdge->id).c_str(), ok, false);
        if (key.size() > 8 && StringUtils::startsWith(key, "cycleway:")) {
            // handle special busway keys
            const std::string cyclewaySpec = key.substr(9);
            key = "cycleway";
            if (cyclewaySpec == "right") {
                myCurrentEdge->myCyclewayType = (WayType)(myCurrentEdge->myCyclewayType | WAY_FORWARD);
            } else if (cyclewaySpec == "left") {
                myCurrentEdge->myCyclewayType = (WayType)(myCurrentEdge->myCyclewayType | WAY_BACKWARD);
            } else if (cyclewaySpec == "both") {
                myCurrentEdge->myCyclewayType = (WayType)(myCurrentEdge->myCyclewayType | WAY_BOTH);
            } else {
                key = "ignore";
            }
            if ((myCurrentEdge->myCyclewayType & WAY_BOTH) != 0) {
                // now we have some info on directionality
                myCurrentEdge->myCyclewayType = (WayType)(myCurrentEdge->myCyclewayType & ~WAY_UNKNOWN);
            }
        } else if (key.size() > 6 && StringUtils::startsWith(key, "busway:")) {
            // handle special busway keys
            const std::string buswaySpec = key.substr(7);
            key = "busway";
            if (buswaySpec == "right") {
                myCurrentEdge->myBuswayType = (WayType)(myCurrentEdge->myBuswayType | WAY_FORWARD);
            } else if (buswaySpec == "left") {
                myCurrentEdge->myBuswayType = (WayType)(myCurrentEdge->myBuswayType | WAY_BACKWARD);
            } else if (buswaySpec == "both") {
                myCurrentEdge->myBuswayType = (WayType)(myCurrentEdge->myBuswayType | WAY_BOTH);
            } else {
                key = "ignore";
            }
        }
        if (key == "bridge" || key == "tunnel") {
            myCurrentEdge->addParameter(key, "true"); // could be differentiated further if necessary
        }

        // we check whether the key is relevant (and we really need to transcode the value) to avoid hitting #1636
        if (!StringUtils::endsWith(key, "way") && !StringUtils::startsWith(key, "lanes")
                && key != "maxspeed" && key != "junction" && key != "name" && key != "tracks" && key != "layer" && key != "route" && key != "postal_code") {
            return;
        }
        std::string value = attrs.get<std::string>(SUMO_ATTR_V, toString(myCurrentEdge->id).c_str(), ok, false);

        if (key == "highway" || key == "railway" || key == "waterway" || key == "cycleway" || key == "busway" || key == "route") {
            myCurrentEdge->myCurrentIsRoad = true;
            // special cycleway stuff
            if (key == "cycleway") {
                if (value == "no") {
                    return;
                } else if (value == "opposite_track") {
                    myCurrentEdge->myCyclewayType = WAY_BACKWARD;
                } else if (value == "opposite_lane") {
                    myCurrentEdge->myCyclewayType = WAY_BACKWARD;
                }
            }
            // special busway stuff
            if (key == "busway") {
                if (value == "no") {
                    return;
                } else if (value == "opposite_track") {
                    myCurrentEdge->myBuswayType = WAY_BACKWARD;
                } else if (value == "opposite_lane") {
                    myCurrentEdge->myBuswayType = WAY_BACKWARD;
                }
                // no need to extend the type id
                return;
            }
            // build type id
            const std::string singleTypeID = key + "." + value;
            if (myCurrentEdge->myHighWayType != "") {
                // osm-ways may be used by more than one mode (eg railway.tram + highway.residential. this is relevant for multimodal traffic)
                // we create a new type for this kind of situation which must then be resolved in insertEdge()
                std::vector<std::string> types = StringTokenizer(myCurrentEdge->myHighWayType, compoundTypeSeparator).getVector();
                types.push_back(singleTypeID);
                myCurrentEdge->myHighWayType = joinToStringSorting(types, compoundTypeSeparator);
            } else {
                myCurrentEdge->myHighWayType = singleTypeID;
            }
        } else if (key == "lanes") {
            try {
                myCurrentEdge->myNoLanes = TplConvert::_2int(value.c_str());
            } catch (NumberFormatException&) {
                // might be a list of values
                StringTokenizer st(value, ";", true);
                std::vector<std::string> list = st.getVector();
                if (list.size() >= 2) {
                    int minLanes = std::numeric_limits<int>::max();
                    try {
                        for (std::vector<std::string>::iterator i = list.begin(); i != list.end(); ++i) {
                            int numLanes = TplConvert::_2int(StringUtils::prune(*i).c_str());
                            minLanes = MIN2(minLanes, numLanes);
                        }
                        myCurrentEdge->myNoLanes = minLanes;
                        WRITE_WARNING("Using minimum lane number from list (" + value + ") for edge '" + toString(myCurrentEdge->id) + "'.");
                    } catch (NumberFormatException&) {
                        WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" +
                                      toString(myCurrentEdge->id) + "'.");
                    }
                }
            } catch (EmptyData&) {
                WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" +
                              toString(myCurrentEdge->id) + "'.");
            }
        } else if (key == "lanes:forward") {
            try {
                myCurrentEdge->myNoLanesForward = TplConvert::_2int(value.c_str());
            } catch (...) {
                WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" +
                              toString(myCurrentEdge->id) + "'.");
            }
        } else if (key == "lanes:backward") {
            try {
                // denote backwards count with a negative sign
                myCurrentEdge->myNoLanesForward = -TplConvert::_2int(value.c_str());
            } catch (...) {
                WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" +
                              toString(myCurrentEdge->id) + "'.");
            }
        } else if (key == "maxspeed") {
            if (mySpeedMap.find(value) != mySpeedMap.end()) {
                myCurrentEdge->myMaxSpeed = mySpeedMap[value];
            } else {
                SUMOReal conversion = 1; // OSM default is km/h
                if (StringUtils::to_lower_case(value).find("km/h") != std::string::npos) {
                    value = StringUtils::prune(value.substr(0, value.find_first_not_of("0123456789")));
                } else if (StringUtils::to_lower_case(value).find("mph") != std::string::npos) {
                    value = StringUtils::prune(value.substr(0, value.find_first_not_of("0123456789")));
                    conversion = 1.609344; // kilometers per mile
                }
                try {
                    myCurrentEdge->myMaxSpeed = TplConvert::_2SUMOReal(value.c_str()) * conversion;
                } catch (...) {
                    WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" +
                                  toString(myCurrentEdge->id) + "'.");
                }
            }
        } else if (key == "junction") {
            if ((value == "roundabout") && (myCurrentEdge->myIsOneWay == "")) {
                myCurrentEdge->myIsOneWay = "yes";
            }
        } else if (key == "oneway") {
            myCurrentEdge->myIsOneWay = value;
        } else if (key == "name") {
            myCurrentEdge->streetName = value;
        } else if (key == "layer") {
            try {
                myCurrentEdge->myLayer = TplConvert::_2int(value.c_str());
            } catch (...) {
                WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" +
                              toString(myCurrentEdge->id) + "'.");
            }
        } else if (key == "tracks") {
            try {
                if (TplConvert::_2int(value.c_str()) > 1) {
                    myCurrentEdge->myIsOneWay = "false";
                } else {
                    myCurrentEdge->myIsOneWay = "true";
                }
            } catch (...) {
                WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" +
                              toString(myCurrentEdge->id) + "'.");
            }
        } else if (key == "postal_code") {
            myCurrentEdge->addParameter(key, value);
        }
    }
}


void
NIImporter_OpenStreetMap::EdgesHandler::myEndElement(int element) {
    myParentElements.pop_back();
    if (element == SUMO_TAG_WAY) {
        if (myCurrentEdge != 0 && myCurrentEdge->myCurrentIsRoad) {
            myEdgeMap[myCurrentEdge->id] = myCurrentEdge;
        } else {
            delete myCurrentEdge;
        }
        myCurrentEdge = 0;
    }
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_OpenStreetMap::RelationHandler-methods
// ---------------------------------------------------------------------------
NIImporter_OpenStreetMap::RelationHandler::RelationHandler(
    const std::map<long long int, NIOSMNode*>& osmNodes,
    const std::map<long long int, Edge*>& osmEdges) :
    SUMOSAXHandler("osm - file"),
    myOSMNodes(osmNodes),
    myOSMEdges(osmEdges) {
    resetValues();
}


NIImporter_OpenStreetMap::RelationHandler::~RelationHandler() {
}

void
NIImporter_OpenStreetMap::RelationHandler::resetValues() {
    myCurrentRelation = INVALID_ID;
    myIsRestriction = false;
    myFromWay = INVALID_ID;
    myToWay = INVALID_ID;
    myViaNode = INVALID_ID;
    myViaWay = INVALID_ID;
    myRestrictionType = RESTRICTION_UNKNOWN;
}

void
NIImporter_OpenStreetMap::RelationHandler::myStartElement(int element,
        const SUMOSAXAttributes& attrs) {
    myParentElements.push_back(element);
    // parse "way" elements
    if (element == SUMO_TAG_RELATION) {
        bool ok = true;
        myCurrentRelation = attrs.get<long long int>(SUMO_ATTR_ID, 0, ok);
        std::string action = attrs.hasAttribute("action") ? attrs.getStringSecure("action", "") : "";
        if (action == "delete" || !ok) {
            myCurrentRelation = INVALID_ID;
        }
        return;
    } else if (myCurrentRelation == INVALID_ID) {
        return;
    }
    // parse member elements
    if (element == SUMO_TAG_MEMBER) {
        bool ok = true;
        std::string role = attrs.hasAttribute("role") ? attrs.getStringSecure("role", "") : "";
        long long int ref = attrs.get<long long int>(SUMO_ATTR_REF, 0, ok);
        if (role == "via") {
            // u-turns for divided ways may be given with 2 via-nodes or 1 via-way
            std::string memberType = attrs.get<std::string>(SUMO_ATTR_TYPE, 0, ok);
            if (memberType == "way" && checkEdgeRef(ref)) {
                myViaWay = ref;
            } else if (memberType == "node") {
                if (myOSMNodes.find(ref) != myOSMNodes.end()) {
                    myViaNode = ref;
                } else {
                    WRITE_WARNING("No node found for reference '" + toString(ref) + "' in relation '" + toString(myCurrentRelation) + "'");
                }
            }
        } else if (role == "from" && checkEdgeRef(ref)) {
            myFromWay = ref;
        } else if (role == "to" && checkEdgeRef(ref)) {
            myToWay = ref;
        }
        return;
    }
    // parse values
    if (element == SUMO_TAG_TAG) {
        bool ok = true;
        std::string key = attrs.get<std::string>(SUMO_ATTR_K, toString(myCurrentRelation).c_str(), ok, false);
        // we check whether the key is relevant (and we really need to transcode the value) to avoid hitting #1636
        if (key == "type" || key == "restriction") {
            std::string value = attrs.get<std::string>(SUMO_ATTR_V, toString(myCurrentRelation).c_str(), ok, false);
            if (key == "type" && value == "restriction") {
                myIsRestriction = true;
                return;
            }
            if (key == "restriction") {
                // @note: the 'right/left/straight' part is ignored since the information is
                // redundantly encoded in the 'from', 'to' and 'via' members
                if (value.substr(0, 5) == "only_") {
                    myRestrictionType = RESTRICTION_ONLY;
                } else if (value.substr(0, 3) == "no_") {
                    myRestrictionType = RESTRICTION_NO;
                } else {
                    WRITE_WARNING("Found unknown restriction type '" + value + "' in relation '" + toString(myCurrentRelation) + "'");
                }
                return;
            }
        }
    }
}


bool
NIImporter_OpenStreetMap::RelationHandler::checkEdgeRef(long long int ref) const {
    if (myOSMEdges.find(ref) != myOSMEdges.end()) {
        return true;
    } else {
        WRITE_WARNING("No way found for reference '" + toString(ref) + "' in relation '" + toString(myCurrentRelation) + "'");
        return false;
    }
}


void
NIImporter_OpenStreetMap::RelationHandler::myEndElement(int element) {
    myParentElements.pop_back();
    if (element == SUMO_TAG_RELATION) {
        if (myIsRestriction) {
            assert(myCurrentRelation != INVALID_ID);
            bool ok = true;
            if (myRestrictionType == RESTRICTION_UNKNOWN) {
                WRITE_WARNING("Ignoring restriction relation '" + toString(myCurrentRelation) + "' with unknown type.");
                ok = false;
            }
            if (myFromWay == INVALID_ID) {
                WRITE_WARNING("Ignoring restriction relation '" + toString(myCurrentRelation) + "' with unknown from-way.");
                ok = false;
            }
            if (myToWay == INVALID_ID) {
                WRITE_WARNING("Ignoring restriction relation '" + toString(myCurrentRelation) + "' with unknown to-way.");
                ok = false;
            }
            if (myViaNode == INVALID_ID && myViaWay == INVALID_ID) {
                WRITE_WARNING("Ignoring restriction relation '" + toString(myCurrentRelation) + "' with unknown via.");
                ok = false;
            }
            if (ok && !applyRestriction()) {
                WRITE_WARNING("Ignoring restriction relation '" + toString(myCurrentRelation) + "'.");
            }
        }
        // other relations might use similar subelements so reset in any case
        resetValues();
    }
}


bool
NIImporter_OpenStreetMap::RelationHandler::applyRestriction() const {
    // since OSM ways are bidirectional we need the via to figure out which direction was meant
    if (myViaNode != INVALID_ID) {
        NBNode* viaNode = myOSMNodes.find(myViaNode)->second->node;
        if (viaNode == 0) {
            WRITE_WARNING("Via-node '" + toString(myViaNode) + "' was not instantiated");
            return false;
        }
        NBEdge* from = findEdgeRef(myFromWay, viaNode->getIncomingEdges());
        NBEdge* to = findEdgeRef(myToWay, viaNode->getOutgoingEdges());
        if (from == 0) {
            WRITE_WARNING("from-edge of restriction relation could not be determined");
            return false;
        }
        if (to == 0) {
            WRITE_WARNING("to-edge of restriction relation could not be determined");
            return false;
        }
        if (myRestrictionType == RESTRICTION_ONLY) {
            from->addEdge2EdgeConnection(to);
        } else {
            from->removeFromConnections(to, -1, -1, true);
        }
    } else {
        // XXX interpreting via-ways or via-node lists not yet implemented
        WRITE_WARNING("direction of restriction relation could not be determined");
        return false;
    }
    return true;
}


NBEdge*
NIImporter_OpenStreetMap::RelationHandler::findEdgeRef(long long int wayRef, const std::vector<NBEdge*>& candidates) const {
    const std::string prefix = toString(wayRef);
    const std::string backPrefix = "-" + prefix;
    NBEdge* result = 0;
    int found = 0;
    for (EdgeVector::const_iterator it = candidates.begin(); it != candidates.end(); ++it) {
        if (((*it)->getID().substr(0, prefix.size()) == prefix) ||
                ((*it)->getID().substr(0, backPrefix.size()) == backPrefix)) {
            result = *it;
            found++;
        }
    }
    if (found > 1) {
        WRITE_WARNING("Ambigous way reference '" + prefix + "' in restriction relation");
        result = 0;
    }
    return result;
}


void
NIImporter_OpenStreetMap::reconstructLayerElevation(const SUMOReal layerElevation, NBNetBuilder& nb) {
    NBNodeCont& nc = nb.getNodeCont();
    NBEdgeCont& ec = nb.getEdgeCont();
    // reconstruct elevation from layer info
    // build a map of raising and lowering forces (attractor and distance)
    // for all nodes unknownElevation
    std::map<NBNode*, std::vector<std::pair<SUMOReal, SUMOReal> > > layerForces;

    // collect all nodes that belong to a way with layer information
    std::set<NBNode*> knownElevation;
    for (std::map<long long int, Edge*>::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        Edge* e = (*i).second;
        if (e->myLayer != 0)  {
            for (std::vector<long long int>::iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
                NBNode* node = nc.retrieve(toString(*j));
                if (node != 0) {
                    knownElevation.insert(node);
                    layerForces[node].push_back(std::make_pair(e->myLayer * layerElevation, POSITION_EPS));
                }
            }
        }
    }
#ifdef DEBUG_LAYER_ELEVATION
    std::cout << "known elevations:\n";
    for (std::set<NBNode*>::iterator it = knownElevation.begin(); it != knownElevation.end(); ++it) {
        const std::vector<std::pair<SUMOReal, SUMOReal> >& primaryLayers = layerForces[*it];
        std::cout << "  node=" << (*it)->getID() << " ele=";
        for (std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator it_ele = primaryLayers.begin(); it_ele != primaryLayers.end(); ++it_ele) {
            std::cout << it_ele->first << " ";
        }
        std::cout << "\n";
    }
#endif
    // layer data only provides a lower bound on elevation since it is used to
    // resolve the relation among overlapping ways.
    // Perform a sanity check for steep inclines and raise the knownElevation if necessary
    std::map<NBNode*, SUMOReal> knownEleMax;
    for (std::set<NBNode*>::iterator it = knownElevation.begin(); it != knownElevation.end(); ++it) {
        SUMOReal eleMax = -std::numeric_limits<SUMOReal>::max();
        const std::vector<std::pair<SUMOReal, SUMOReal> >& primaryLayers = layerForces[*it];
        for (std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator it_ele = primaryLayers.begin(); it_ele != primaryLayers.end(); ++it_ele) {
            eleMax = MAX2(eleMax, it_ele->first);
        }
        knownEleMax[*it] = eleMax;
    }
    const SUMOReal gradeThreshold = OptionsCont::getOptions().getFloat("osm.layer-elevation.max-grade") / 100;
    bool changed = true;
    while (changed) {
        changed = false;
        for (std::set<NBNode*>::iterator it = knownElevation.begin(); it != knownElevation.end(); ++it) {
            std::map<NBNode*, std::pair<SUMOReal, SUMOReal> > neighbors = getNeighboringNodes(*it, knownEleMax[*it] / gradeThreshold * 3, knownElevation);
            for (std::map<NBNode*, std::pair<SUMOReal, SUMOReal> >::iterator it_neigh = neighbors.begin(); it_neigh != neighbors.end(); ++it_neigh) {
                if (knownElevation.count(it_neigh->first) != 0) {
                    const SUMOReal grade = fabs(knownEleMax[*it] - knownEleMax[it_neigh->first]) / MAX2(POSITION_EPS, it_neigh->second.first);
#ifdef DEBUG_LAYER_ELEVATION
                    std::cout << "   grade at node=" << (*it)->getID() << " ele=" << knownEleMax[*it] << " neigh=" << it_neigh->first->getID() << " neighEle=" << knownEleMax[it_neigh->first] << " grade=" << grade << " dist=" << it_neigh->second.first << " speed=" << it_neigh->second.second << "\n";
#endif
                    if (grade > gradeThreshold * 50 / 3.6 / it_neigh->second.second) {
                        // raise the lower node to the higher level
                        const SUMOReal eleMax = MAX2(knownEleMax[*it], knownEleMax[it_neigh->first]);
                        if (knownEleMax[*it] < eleMax) {
                            knownEleMax[*it] = eleMax;
                        } else {
                            knownEleMax[it_neigh->first] = eleMax;
                        }
                        changed = true;
                    }
                }
            }
        }
    }

    // collect all nodes within a grade-dependent range around knownElevation-nodes and apply knowElevation forces
    std::set<NBNode*> unknownElevation;
    for (std::set<NBNode*>::iterator it = knownElevation.begin(); it != knownElevation.end(); ++it) {
        const SUMOReal eleMax = knownEleMax[*it];
        const SUMOReal maxDist = fabs(eleMax) * 100 / layerElevation;
        std::map<NBNode*, std::pair<SUMOReal, SUMOReal> > neighbors = getNeighboringNodes(*it, maxDist, knownElevation);
        for (std::map<NBNode*, std::pair<SUMOReal, SUMOReal> >::iterator it_neigh = neighbors.begin(); it_neigh != neighbors.end(); ++it_neigh) {
            if (knownElevation.count(it_neigh->first) == 0) {
                unknownElevation.insert(it_neigh->first);
                layerForces[it_neigh->first].push_back(std::make_pair(eleMax, it_neigh->second.first));
            }
        }
    }

    // apply forces to ground-level nodes (neither in knownElevation nor unknownElevation)
    for (std::set<NBNode*>::iterator it = unknownElevation.begin(); it != unknownElevation.end(); ++it) {
        SUMOReal eleMax = -std::numeric_limits<SUMOReal>::max();
        const std::vector<std::pair<SUMOReal, SUMOReal> >& primaryLayers = layerForces[*it];
        for (std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator it_ele = primaryLayers.begin(); it_ele != primaryLayers.end(); ++it_ele) {
            eleMax = MAX2(eleMax, it_ele->first);
        }
        const SUMOReal maxDist = fabs(eleMax) * 100 / layerElevation;
        std::map<NBNode*, std::pair<SUMOReal, SUMOReal> > neighbors = getNeighboringNodes(*it, maxDist, knownElevation);
        for (std::map<NBNode*, std::pair<SUMOReal, SUMOReal> >::iterator it_neigh = neighbors.begin(); it_neigh != neighbors.end(); ++it_neigh) {
            if (knownElevation.count(it_neigh->first) == 0 && unknownElevation.count(it_neigh->first) == 0) {
                layerForces[*it].push_back(std::make_pair(0, it_neigh->second.first));
            }
        }
    }
    // compute the elevation for each node as the weighted average of all forces
#ifdef DEBUG_LAYER_ELEVATION
    std::cout << "summation of forces\n";
#endif
    std::map<NBNode*, SUMOReal> nodeElevation;
    for (std::map<NBNode*, std::vector<std::pair<SUMOReal, SUMOReal> > >::iterator it = layerForces.begin(); it != layerForces.end(); ++it) {
        const std::vector<std::pair<SUMOReal, SUMOReal> >& forces = it->second;
        if (knownElevation.count(it->first) != 0) {
            // use the maximum value
            /*
            SUMOReal eleMax = -std::numeric_limits<SUMOReal>::max();
            for (std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator it_force = forces.begin(); it_force != forces.end(); ++it_force) {
                eleMax = MAX2(eleMax, it_force->first);
            }
            */
#ifdef DEBUG_LAYER_ELEVATION
            std::cout << "   node=" << it->first->getID() << " knownElevation=" << knownEleMax[it->first] << "\n";
#endif
            nodeElevation[it->first] = knownEleMax[it->first];
        } else if (forces.size() == 1) {
            nodeElevation[it->first] = forces.front().first;
        } else {
            // use the weighted sum
            SUMOReal distSum = 0;
            for (std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator it_force = forces.begin(); it_force != forces.end(); ++it_force) {
                distSum += it_force->second;
            }
            SUMOReal weightSum = 0;
            SUMOReal elevation = 0;
#ifdef DEBUG_LAYER_ELEVATION
            std::cout << "   node=" << it->first->getID() << "  distSum=" << distSum << "\n";
#endif
            for (std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator it_force = forces.begin(); it_force != forces.end(); ++it_force) {
                const SUMOReal weight = (distSum - it_force->second) / distSum;
                weightSum += weight;
                elevation += it_force->first * weight;

#ifdef DEBUG_LAYER_ELEVATION
                std::cout << "       force=" << it_force->first << " dist=" << it_force->second << "  weight=" << weight << " ele=" << elevation << "\n";
#endif
            }
            nodeElevation[it->first] = elevation / weightSum;
        }
    }
#ifdef DEBUG_LAYER_ELEVATION
    std::cout << "final elevations:\n";
    for (std::map<NBNode*, SUMOReal>::iterator it = nodeElevation.begin(); it != nodeElevation.end(); ++it) {
        std::cout << "  node=" << (it->first)->getID() << " ele=" << it->second << "\n";;
    }
#endif
    // apply node elevations
    for (std::map<NBNode*, SUMOReal>::iterator it = nodeElevation.begin(); it != nodeElevation.end(); ++it) {
        NBNode* n = it->first;
        Position pos = n->getPosition();
        n->reinit(n->getPosition() + Position(0, 0, it->second), n->getType());
    }

    // apply way elevation to all edges that had layer information
    for (std::map<std::string, NBEdge*>::const_iterator it = ec.begin(); it != ec.end(); ++it) {
        NBEdge* edge = it->second;
        const PositionVector& geom = edge->getGeometry();
        const SUMOReal length = geom.length2D();
        const SUMOReal zFrom = nodeElevation[edge->getFromNode()];
        const SUMOReal zTo = nodeElevation[edge->getToNode()];
        // XXX if the from- or to-node was part of multiple ways with
        // different layers, reconstruct the layer value from origID
        SUMOReal dist = 0;
        PositionVector newGeom;
        for (PositionVector::const_iterator it_pos = geom.begin(); it_pos != geom.end(); ++it_pos) {
            if (it_pos != geom.begin()) {
                dist += (*it_pos).distanceTo2D(*(it_pos - 1));
            }
            newGeom.push_back((*it_pos) + Position(0, 0, zFrom + (zTo - zFrom) * dist / length));
        }
        edge->setGeometry(newGeom);
    }
}


std::map<NBNode*, std::pair<SUMOReal, SUMOReal> >
NIImporter_OpenStreetMap::getNeighboringNodes(NBNode* node, SUMOReal maxDist, const std::set<NBNode*>& knownElevation) {
    std::map<NBNode*, std::pair<SUMOReal, SUMOReal> > result;
    std::set<NBNode*> visited;
    std::vector<NBNode*> open;
    open.push_back(node);
    while (open.size() > 0) {
        NBNode* n = open.back();
        open.pop_back();
        if (visited.count(n) != 0) {
            continue;
        }
        visited.insert(n);
        const EdgeVector& edges = n->getEdges();
        for (EdgeVector::const_iterator j = edges.begin(); j != edges.end(); ++j) {
            NBEdge* e = *j;
            NBNode* s = 0;
            if (n->hasIncoming(e)) {
                s = e->getFromNode();
            } else {
                s = e->getToNode();
            }
            const SUMOReal dist = result[n].first + e->getGeometry().length2D();
            const SUMOReal speed = MAX2(e->getSpeed(), result[n].second);
            if (result.count(s) == 0) {
                result[s] = std::make_pair(dist, speed);
            } else {
                result[s] = std::make_pair(MIN2(dist, result[s].first), MAX2(speed, result[s].second));
            }
            if (dist < maxDist && knownElevation.count(s) == 0) {
                open.push_back(s);
            }
        }
    }
    return result;
}


/****************************************************************************/

