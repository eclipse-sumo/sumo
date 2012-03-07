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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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

// ---------------------------------------------------------------------------
// static members
// ---------------------------------------------------------------------------
const SUMOReal NIImporter_OpenStreetMap::MAXSPEED_UNGIVEN = -1;


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
    for (std::map<std::string, Edge*>::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        delete(*i).second;
    }
}


void
NIImporter_OpenStreetMap::load(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("osm-files")) {
        return;
    }
    // preset types
    //  for highways
    NBTypeCont& tc = nb.getTypeCont();
    SUMOReal const WIDTH = NBEdge::UNSPECIFIED_WIDTH;
    tc.insert("highway.motorway",      3, (SUMOReal)(160./ 3.6), 13, WIDTH, SVC_UNKNOWN, true);
    tc.insert("highway.motorway_link", 1, (SUMOReal)(80. / 3.6), 12, WIDTH, SVC_UNKNOWN, true);
    tc.insert("highway.trunk",         2, (SUMOReal)(100./ 3.6), 11, WIDTH); // !!! 130km/h?
    tc.insert("highway.trunk_link",    1, (SUMOReal)(80. / 3.6), 10, WIDTH);
    tc.insert("highway.primary",       2, (SUMOReal)(100./ 3.6),  9, WIDTH);
    tc.insert("highway.primary_link",  1, (SUMOReal)(80. / 3.6),  8, WIDTH);
    tc.insert("highway.secondary",     2, (SUMOReal)(100./ 3.6),  7, WIDTH);
    tc.insert("highway.secondary_link",1, (SUMOReal)(80. / 3.6),  6, WIDTH);
    tc.insert("highway.tertiary",      1, (SUMOReal)(80. / 3.6),  6, WIDTH);
    tc.insert("highway.tertiary_link", 1, (SUMOReal)(80. / 3.6),  5, WIDTH);
    tc.insert("highway.unclassified",  1, (SUMOReal)(80. / 3.6),  5, WIDTH);
    tc.insert("highway.residential",   1, (SUMOReal)(50. / 3.6),  4, WIDTH); // actually, maybe one lane for parking would be nice...
    tc.insert("highway.living_street", 1, (SUMOReal)(10. / 3.6),  3, WIDTH);
    tc.insert("highway.service",       1, (SUMOReal)(20. / 3.6),  2, WIDTH, SVC_DELIVERY);
    tc.insert("highway.track",         1, (SUMOReal)(20. / 3.6),  1, WIDTH);
    tc.insert("highway.services",      1, (SUMOReal)(30. / 3.6),  1, WIDTH);
    tc.insert("highway.unsurfaced",    1, (SUMOReal)(30. / 3.6),  1, WIDTH); // unofficial value, used outside germany
    tc.insert("highway.footway",       1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_PEDESTRIAN); 
    tc.insert("highway.pedestrian",    1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_PEDESTRIAN);

    tc.insert("highway.path",          1, (SUMOReal)(10. / 3.6),  1, WIDTH, SVC_PEDESTRIAN);
    tc.insert("highway.bridleway",     1, (SUMOReal)(10. / 3.6),  1, WIDTH, SVC_BICYCLE); // no horse stuff
    tc.insert("highway.cycleway",      1, (SUMOReal)(20. / 3.6),  1, WIDTH, SVC_BICYCLE);
    tc.insert("highway.footway",       1, (SUMOReal)(10. / 3.6),  1, WIDTH, SVC_PEDESTRIAN);
    tc.insert("highway.step",          1, (SUMOReal)(5.  / 3.6),  1, WIDTH, SVC_PEDESTRIAN); // additional
    tc.insert("highway.steps",         1, (SUMOReal)(5.  / 3.6),  1, WIDTH, SVC_PEDESTRIAN); // :-) do not run too fast
    tc.insert("highway.stairs",        1, (SUMOReal)(5.  / 3.6),  1, WIDTH, SVC_PEDESTRIAN); // additional
    tc.insert("highway.bus_guideway",  1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_BUS);
    tc.insert("highway.raceway",       2, (SUMOReal)(300./ 3.6), 14, WIDTH, SVC_VIP);
    tc.insert("highway.ford",          1, (SUMOReal)(10. / 3.6),  1, WIDTH, SVC_PUBLIC_ARMY);

    //  for railways
    tc.insert("railway.rail",          1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_RAIL_FAST);
    tc.insert("railway.tram",          1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_CITYRAIL);
    tc.insert("railway.light_rail",    1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_LIGHTRAIL);
    tc.insert("railway.subway",        1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_CITYRAIL);
    tc.insert("railway.preserved",     1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_LIGHTRAIL);
    tc.insert("railway.monorail",      1, (SUMOReal)(30. / 3.6),  1, WIDTH, SVC_LIGHTRAIL); // rail stuff has to be discussed


    /* Parse file(s)
     * Each file is parsed twice: first for nodes, second for edges. */
    std::vector<std::string> files = oc.getStringVector("osm-files");
    // load nodes, first
    NodesHandler nodesHandler(myOSMNodes, myUniqueNodes);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // nodes
        if (!FileHelpers::exists(*file)) {
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
    if (!OptionsCont::getOptions().getBool("osm.skip-duplicates-check")) {
        PROGRESS_BEGIN_MESSAGE("Removing duplicate edges");
        if (myEdges.size() > 1) {
            std::set<const Edge*, CompareEdges> dupsFinder;
            for (std::map<std::string, Edge*>::iterator it = myEdges.begin(); it != myEdges.end();) {
                if (dupsFinder.count(it->second) > 0) {
                    WRITE_MESSAGE("Found duplicate edges. Removing " + it->first);
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
    std::map<long, int> nodeUsage;
    // Mark which nodes are used by edges (begin and end)
    for (std::map<std::string, Edge*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        Edge* e = (*i).second;
        assert(e->myCurrentIsRoad);
        for (std::vector<long>::const_iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
            if (nodeUsage.find(*j) == nodeUsage.end()) {
                nodeUsage[*j] = 0;
            }
            nodeUsage[*j] = nodeUsage[*j] + 1;
        }
    }
    // Mark which nodes are used by traffic lights
    for (std::map<long, NIOSMNode*>::const_iterator nodesIt = myOSMNodes.begin(); nodesIt != myOSMNodes.end(); ++nodesIt) {
        if (nodesIt->second->tlsControlled) {
            // If the key is not found in the map, the value is automatically
            // initialized with 0.
            nodeUsage[nodesIt->first] += 1;
        }
    }
    /* Instantiate edges
     * Only those nodes in the middle of an edge which are used by more than
     * one edge are instantiated. Other nodes are considered as geometry nodes. */
    NBNodeCont& nc = nb.getNodeCont();
    NBEdgeCont& ec = nb.getEdgeCont();
    NBTrafficLightLogicCont& tlsc = nb.getTLLogicCont();
    for (std::map<std::string, Edge*>::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        Edge* e = (*i).second;
        assert(e->myCurrentIsRoad);
        if (e->myCurrentNodes.size() < 2) {
            WRITE_WARNING("Discarding way '" + e->id + "' because it has only " + toString(e->myCurrentNodes.size()) + " node(s)");
            continue;
        }
        // build nodes;
        //  - the from- and to-nodes must be built in any case
        //  - the in-between nodes are only built if more than one edge references them
        NBNode* currentFrom = insertNodeChecking(*e->myCurrentNodes.begin(), nc, tlsc);
        NBNode* last = insertNodeChecking(*(e->myCurrentNodes.end() - 1), nc, tlsc);
        int running = 0;
        std::vector<long> passed;
        for (std::vector<long>::iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
            passed.push_back(*j);
            if (nodeUsage[*j] > 1 && j != e->myCurrentNodes.end() - 1 && j != e->myCurrentNodes.begin()) {
                NBNode* currentTo = insertNodeChecking(*j, nc, tlsc);
                running = insertEdge(e, running, currentFrom, currentTo, passed, nb);
                currentFrom = currentTo;
                passed.clear();
            }
        }
        if (running == 0) {
            running = -1;
        }
        insertEdge(e, running, currentFrom, last, passed, nb);
    }
}


NBNode*
NIImporter_OpenStreetMap::insertNodeChecking(long id, NBNodeCont& nc, NBTrafficLightLogicCont& tlsc) {
    NBNode* from = nc.retrieve(toString(id));
    if (from == 0) {
        NIOSMNode* n = myOSMNodes.find(id)->second;
        Position pos(n->lon, n->lat);
        if (!NILoader::transformCoordinates(pos, true)) {
            WRITE_ERROR("Unable to project coordinates for node " + toString(id) + ".");
            delete from;
            return 0;
        }
        from = new NBNode(toString(id), pos);
        if (!nc.insert(from)) {
            WRITE_ERROR("Could not insert node '" + toString(id) + "').");
            delete from;
            return 0;
        }
        if (n->tlsControlled) {
            // ok, this node is a traffic light node where no other nodes
            //  participate
            NBOwnTLDef* tlDef = new NBOwnTLDef(toString(id), from);
            if (!tlsc.insert(tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError("Could not allocate tls '" + toString(id) + "'.");
            }
        }
    }
    return from;
}


int
NIImporter_OpenStreetMap::insertEdge(Edge* e, int index, NBNode* from, NBNode* to,
                                     const std::vector<long> &passed, NBNetBuilder& nb) {
    NBNodeCont& nc = nb.getNodeCont();
    NBEdgeCont& ec = nb.getEdgeCont();
    NBTypeCont& tc = nb.getTypeCont();
    NBTrafficLightLogicCont& tlsc = nb.getTLLogicCont();

    // patch the id
    std::string id = e->id;
    if (index >= 0) {
        id = id + "#" + toString(index);
    } else {
        index = 0;
    }
    if (from == to) {
        // in the special case of a looped way split again using passed
        assert(passed.size() >= 2);
        std::vector<long> geom(passed);
        geom.pop_back(); // remove to-node
        NBNode* intermediate = insertNodeChecking(geom.back(), nc, tlsc);
        index = insertEdge(e, index, from, intermediate, geom, nb);
        geom.clear();
        return insertEdge(e, index, intermediate, to, geom, nb);
    }
    const int newIndex = index + 1;

    // convert the shape
    PositionVector shape;
    for (std::vector<long>::const_iterator i = passed.begin(); i != passed.end(); ++i) {
        NIOSMNode* n = myOSMNodes.find(*i)->second;
        Position pos(n->lon, n->lat);
        if (!NILoader::transformCoordinates(pos, true)) {
            throw ProcessError("Unable to project coordinates for edge " + id + ".");
        }
        shape.push_back_noDoublePos(pos);
    }

    std::string type = e->myHighWayType;
    if (!tc.knows(type)) {
        if (type.find(compoundTypeSeparator) != std::string::npos) {
            // this edge has a combination type which does not yet exist in the TypeContainer
            StringTokenizer tok = StringTokenizer(type, compoundTypeSeparator);
            std::set<std::string> types;
            while (tok.hasNext()) {
                std::string t = tok.next();
                if (tc.knows(t)) {
                    types.insert(t);
                } else {
                    WRITE_WARNING("Discarding unknown compound \"" + t + "\" for edge " + id + " with type \"" + type + "\".");
                }
            }
            switch (types.size()) {
                case 0:
                    WRITE_WARNING("Discarding edge " + id + " with type unknown compound type \"" + type + "\".");
                    return newIndex;
                    break;
                case 1: {
                    type = *(types.begin());
                    break;
                }
                default:
                    // build a new type by merging all values
                    int noLanes = 0;
                    SUMOReal maxSpeed = 0;
                    int prio = 0;
                    SUMOReal width = NBEdge::UNSPECIFIED_WIDTH;
                    bool defaultIsOneWay = false;
                    for (std::set<std::string>::iterator it = types.begin(); it != types.end(); it++) {
                        noLanes = MAX2(noLanes, tc.getNumLanes(*it));
                        maxSpeed = MAX2(maxSpeed, tc.getSpeed(*it));
                        prio = MAX2(prio, tc.getPriority(*it));
                        defaultIsOneWay |= tc.getIsOneWay(*it);
                    }
                    WRITE_MESSAGE("Adding new compound type \"" + type + "\" for edge " + id + ".");
                    // @todo use the propper bitsets instead of SVC_UNKNOWN (see #675)
                    tc.insert(type, noLanes, maxSpeed, prio, width, SVC_UNKNOWN, defaultIsOneWay);
            }
        } else {
            // we do not know the type -> something else, ignore
            //WRITE_WARNING("Discarding edge " + id + " with unknown type \"" + type + "\".");
            return newIndex;
        }
    }

    // otherwise it is not an edge and will be ignored
    int noLanes = tc.getNumLanes(type);
    SUMOReal speed = tc.getSpeed(type);
    bool defaultsToOneWay = tc.getIsOneWay(type);
    SVCPermissions permissions = tc.getPermissions(type);
    // check directions
    bool addSecond = true;
    if (e->myIsOneWay == "true" || e->myIsOneWay == "yes" || e->myIsOneWay == "1" || (defaultsToOneWay && e->myIsOneWay != "no" && e->myIsOneWay != "false" && e->myIsOneWay != "0")) {
        addSecond = false;
    }
    // if we had been able to extract the number of lanes, override the highway type default
    if (e->myNoLanes >= 0) {
        if (!addSecond) {
            noLanes = e->myNoLanes;
        } else {
            noLanes = e->myNoLanes / 2;
        }
    }
    // if we had been able to extract the maximum speed, override the type's default
    if (e->myMaxSpeed != MAXSPEED_UNGIVEN) {
        speed = (SUMOReal)(e->myMaxSpeed / 3.6);
    }

    if (noLanes != 0 && speed != 0) {
        if (e->myIsOneWay != "" && e->myIsOneWay != "false" && e->myIsOneWay != "no" && e->myIsOneWay != "true" && e->myIsOneWay != "yes" && e->myIsOneWay != "-1" && e->myIsOneWay != "1") {
            WRITE_WARNING("New value for oneway found: " + e->myIsOneWay);
        }
        LaneSpreadFunction lsf = addSecond ? LANESPREAD_RIGHT : LANESPREAD_CENTER;
        if (e->myIsOneWay != "-1") {
            NBEdge* nbe = new NBEdge(StringUtils::escapeXML(id), from, to, type, speed, noLanes, tc.getPriority(type),
                                     tc.getWidth(type), NBEdge::UNSPECIFIED_OFFSET, shape, StringUtils::escapeXML(e->streetName), lsf);
            nbe->setPermissions(permissions);
            if (!ec.insert(nbe)) {
                delete nbe;
                throw ProcessError("Could not add edge '" + id + "'.");
            }
        }
        if (addSecond) {
            if (e->myIsOneWay != "-1") {
                id = "-" + id;
            }
            NBEdge* nbe = new NBEdge(StringUtils::escapeXML(id), to, from, type, speed, noLanes, tc.getPriority(type),
                                     tc.getWidth(type), NBEdge::UNSPECIFIED_OFFSET, shape.reverse(), StringUtils::escapeXML(e->streetName), lsf);
            nbe->setPermissions(permissions);
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
        std::map<long, NIOSMNode*> &toFill,
        std::set<NIOSMNode*, CompareNodes> &uniqueNodes) : 
    SUMOSAXHandler("osm - file"), 
    myToFill(toFill), 
    myUniqueNodes(uniqueNodes),
    myLastNodeID(-1), 
    myIsInValidNodeTag(false), 
    myHierarchyLevel(0) {
}


NIImporter_OpenStreetMap::NodesHandler::~NodesHandler() {}


void
NIImporter_OpenStreetMap::NodesHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    ++myHierarchyLevel;
    if (element == SUMO_TAG_NODE) {
        bool ok = true;
        if (myHierarchyLevel != 2) {
            WRITE_ERROR("Node element on wrong XML hierarchy level (id='" + toString(attrs.getLongReporting(SUMO_ATTR_ID, 0, ok)) + "', level='" + toString(myHierarchyLevel) + "').");
            return;
        }
        long id = attrs.getLongReporting(SUMO_ATTR_ID, 0, ok);
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
            std::istringstream lon(attrs.getStringReporting(SUMO_ATTR_LON, toString(id).c_str(), ok));
            if (!ok) {
                return;
            }
            lon >> tlon;
            if (lon.fail()) {
                WRITE_ERROR("Node's '" + toString(id) + "' lon information is not numeric.");
                return;
            }
            std::istringstream lat(attrs.getStringReporting(SUMO_ATTR_LAT, toString(id).c_str(), ok));
            if (!ok) {
                return;
            }
            lat >> tlat;
            if (lat.fail()) {
                WRITE_ERROR("Node's '" + toString(id) + "' lat information is not numeric.");
                return;
            }
            NIOSMNode* toAdd = new NIOSMNode();
            toAdd->id = id;
            toAdd->tlsControlled = false;
            toAdd->lat = tlat;
            toAdd->lon = tlon;
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
        std::string key = attrs.getStringReporting(SUMO_ATTR_K, toString(myLastNodeID).c_str(), ok);
        std::string value = attrs.getOptStringReporting(SUMO_ATTR_V, toString(myLastNodeID).c_str(), ok, "");
        if (!ok) {
            return;
        }
        if (key == "highway" && value.find("traffic_signal") != std::string::npos &&
                !OptionsCont::getOptions().getBool("osm.discard-tls")) {
            myToFill[myLastNodeID]->tlsControlled = true;
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
    const std::map<long, NIOSMNode*> &osmNodes,
    std::map<std::string, Edge*> &toFill)
    : SUMOSAXHandler("osm - file"),
      myOSMNodes(osmNodes), myEdgeMap(toFill) {
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
        std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
        std::string action = attrs.hasAttribute("action") ? attrs.getStringSecure("action", "") : "";
        if (action == "delete") {
            myCurrentEdge = 0;
            return;
        }
        if (!ok) {
            myCurrentEdge = 0;
            return;
        }
        myCurrentEdge = new Edge();
        myCurrentEdge->id = id;
        myCurrentEdge->myNoLanes = -1;
        myCurrentEdge->myMaxSpeed = MAXSPEED_UNGIVEN;
        myCurrentEdge->myCurrentIsRoad = false;
    }
    // parse "nd" (node) elements
    if (element == SUMO_TAG_ND) {
        bool ok = true;
        long ref = attrs.getLongReporting(SUMO_ATTR_REF, 0, ok);
        if (ok) {
            std::map<long, NIOSMNode*>::const_iterator node = myOSMNodes.find(ref);
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
        std::string key = attrs.getStringReporting(SUMO_ATTR_K, toString(myCurrentEdge->id).c_str(), ok);
        std::string value = attrs.getStringReporting(SUMO_ATTR_V, toString(myCurrentEdge->id).c_str(), ok);
        if (!ok) {
            return;
        }
        if (key == "highway" || key == "railway") {
            if (myCurrentEdge->myHighWayType != "") {
                // osm-ways may be used by more than one mode (eg railway.tram + highway.residential. this is relevant for multimodal traffic)
                // we create a new type for this kind of situation which must then be resolved in insertEdge()
                myCurrentEdge->myHighWayType = myCurrentEdge->myHighWayType + compoundTypeSeparator + key + "." + value;
            } else {
                myCurrentEdge->myHighWayType = key + "." + value;
            }
            myCurrentEdge->myCurrentIsRoad = true;
        } else if (key == "lanes") {
            try {
                myCurrentEdge->myNoLanes = TplConvert<char>::_2int(value.c_str());
            } catch (NumberFormatException&) {
                // might be a list of values
                StringTokenizer st(value, ";", true);
                std::vector<std::string> list = st.getVector();
                if (list.size() >= 2) {
                    int minLanes = std::numeric_limits<int>::max();
                    try {
                        for (std::vector<std::string>::iterator i = list.begin(); i != list.end(); ++i) {
                            int numLanes = TplConvert<char>::_2int(StringUtils::prune(*i).c_str());
                            minLanes = MIN2(minLanes, numLanes);
                        }
                        myCurrentEdge->myNoLanes = minLanes;
                        WRITE_WARNING("Using minimum lane number from list (" + value + ") for edge '" + myCurrentEdge->id + "'.");
                    } catch (NumberFormatException&) {
                        WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" + myCurrentEdge->id + "'.");
                    }
                }
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
                    myCurrentEdge->myMaxSpeed = TplConvert<char>::_2SUMOReal(value.c_str()) * conversion;
                } catch (NumberFormatException&) {
                    WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" + myCurrentEdge->id + "'.");
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




/****************************************************************************/

