/****************************************************************************/
/// @file    NIImporter_OpenStreetMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in OpenStreetMap format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include "NIImporter_OpenStreetMap.h"
#include <algorithm>
#include <set>
#include <functional>
#include <sstream>
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
#include <utils/iodevices/BinaryInputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// Private classes
// ===========================================================================

/** @brief Functor which compares two NIOSMNodes according
 * to their coordinates
 */
class NIImporter_OpenStreetMap::CompareNodes {
public:
    bool operator()(const NIOSMNode* n1, const NIOSMNode* n2) const {
        return (n1->lat > n2->lat) || (n1->lat == n2->lat && n1->lon > n2->lon);
    }
};

/** @brief A functor to substitute a node in the node list of an Edge
 */
class NIImporter_OpenStreetMap::SubstituteNode: public std::unary_function<
            std::pair<std::string, Edge*>, void> {
public:
    /** @brief Initializes the functor with the node to substitute
     * and the node with that the node in the first argument
     * is substituted
     *
     * @param[in] toSubstitute Every occurrence of this node should
     *                         substituted
     * @param[in] substituteWith This node is overwrites
     *                           the occurrences of toSubstitute
     */
    SubstituteNode(const NIOSMNode* const toSubstitute,
                   const NIOSMNode* const substituteWith) :
            myToSubstitute(toSubstitute), mySubstituteWith(substituteWith) {
    }
    /** @brief Substitutes the nodes in the edge of the given pair
     *
     * In the node list, all occurrences of toSubstitute are substituted
     * with substituteWith. The final list has no equal adjacent nodes.
     *
     * @param[in] p A pair with the edge in which the nodes are
     *              substituted
     */
    void operator()(const std::pair<std::string, Edge*>& p) const {
        std::vector<int>& edgeNodes = p.second->myCurrentNodes;
        // Substitute nodes
        std::replace_if(edgeNodes.begin(), edgeNodes.end(), std::bind2nd(
                            std::equal_to<int>(), myToSubstitute->id), mySubstituteWith->id);

        // Remove adjacent duplicates
        std::vector<int>::iterator newEnd = std::unique(edgeNodes.begin(),
                                            edgeNodes.end());
        edgeNodes.erase(newEnd, edgeNodes.end());
    }

private:
    const NIOSMNode * const myToSubstitute, * const mySubstituteWith;
};

/** @brief Functor which compares two Edges according to all values but id
 */
class NIImporter_OpenStreetMap::SimilarEdge : public std::unary_function<
            std::pair<std::string, Edge*>, bool> {
public:
    /** @brief Initializes the functor with the fixed comparison partner
     *
     * @param[in] p0 A pair with the Edge all other nodes should be compared with.
     */
    SimilarEdge(const std::pair<std::string, Edge*>& p0) :
            myP0(p0) {
    }

    /** @brief Compares the Edge (p1.second) with the node given in the constructor for equality
     *
     * All values but the ids are compared
     *
     * @param[in] p1 A pair with the edge to compare.
     * @return true if both edges are "similar"; otherwise false.
     */
    bool operator()(const std::pair<std::string, Edge*>& p1) const {
        return
            myP0.second->myNoLanes==p1.second->myNoLanes &&
            myP0.second->myMaxSpeed==p1.second->myMaxSpeed &&
            myP0.second->myHighWayType==p1.second->myHighWayType &&
            myP0.second->myIsOneWay==p1.second->myIsOneWay &&
            myP0.second->myCurrentNodes==p1.second->myCurrentNodes &&
            myP0.second->myCurrentIsRoad==p1.second->myCurrentIsRoad;
    }

private:
    std::pair<std::string, Edge*> myP0;
};



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
const std::string NIImporter_OpenStreetMap::compoundTypeSeparator("|");


void
NIImporter_OpenStreetMap::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("osm-files")) {
        return;
    }
    // preset types
    //  for highways
    NBTypeCont &tc = nb.getTypeCont();
    tc.insert("highway.motorway",      3, (SUMOReal)(160./3.6), 13, SVC_UNKNOWN, true);
    tc.insert("highway.motorway_link", 1, (SUMOReal)(80. /3.6), 12, SVC_UNKNOWN, true);
    tc.insert("highway.trunk",         2, (SUMOReal)(100./3.6), 11);  // !!! 130km/h?
    tc.insert("highway.trunk_link",    1, (SUMOReal)(80. /3.6), 10);
    tc.insert("highway.primary",       2, (SUMOReal)(100./3.6),  9);
    tc.insert("highway.primary_link",  1, (SUMOReal)(80. /3.6),  8);
    tc.insert("highway.secondary",     2, (SUMOReal)(100./3.6),  7);
    tc.insert("highway.secondary_link",1, (SUMOReal)(80. /3.6),  6);
    tc.insert("highway.tertiary",      1, (SUMOReal)(80. /3.6),  6);
    tc.insert("highway.unclassified",  1, (SUMOReal)(80. /3.6),  5);
    tc.insert("highway.residential",   1, (SUMOReal)(50. /3.6),  4);  // actually, maybe one lane for parking would be nice...
    tc.insert("highway.living_street", 1, (SUMOReal)(10. /3.6),  3);
    tc.insert("highway.service",       1, (SUMOReal)(20. /3.6),  2, SVC_DELIVERY);
    tc.insert("highway.track",         1, (SUMOReal)(20. /3.6),  1);
    tc.insert("highway.services",      1, (SUMOReal)(30. /3.6),  1);
    tc.insert("highway.unsurfaced",    1, (SUMOReal)(30. /3.6),  1);  // additional
    tc.insert("highway.footway",       1, (SUMOReal)(30. /3.6),  1, SVC_PEDESTRIAN);  // additional
    tc.insert("highway.pedestrian",    1, (SUMOReal)(30. /3.6),  1, SVC_PEDESTRIAN);

    tc.insert("highway.path",          1, (SUMOReal)(10. /3.6),  1, SVC_PEDESTRIAN);
    tc.insert("highway.bridleway",     1, (SUMOReal)(10. /3.6),  1, SVC_PEDESTRIAN);  // no horse stuff
    tc.insert("highway.cycleway",      1, (SUMOReal)(20. /3.6),  1, SVC_BICYCLE);
    tc.insert("highway.footway",       1, (SUMOReal)(10. /3.6),  1, SVC_PEDESTRIAN);
    tc.insert("highway.step",          1, (SUMOReal)(5.  /3.6),  1, SVC_PEDESTRIAN);  // additional
    tc.insert("highway.steps",         1, (SUMOReal)(5.  /3.6),  1, SVC_PEDESTRIAN);  // :-) do not run too fast
    tc.insert("highway.stairs",        1, (SUMOReal)(5.  /3.6),  1, SVC_PEDESTRIAN);  // additional
    tc.insert("highway.bus_guideway",  1, (SUMOReal)(30. /3.6),  1, SVC_BUS);

    //  for railways
    tc.insert("railway.rail",          1, (SUMOReal)(30. /3.6),  1, SVC_RAIL_FAST);
    tc.insert("railway.tram",          1, (SUMOReal)(30. /3.6),  1, SVC_CITYRAIL);
    tc.insert("railway.light_rail",    1, (SUMOReal)(30. /3.6),  1, SVC_LIGHTRAIL);
    tc.insert("railway.subway",        1, (SUMOReal)(30. /3.6),  1, SVC_CITYRAIL);
    tc.insert("railway.preserved",     1, (SUMOReal)(30. /3.6),  1, SVC_LIGHTRAIL);
    tc.insert("railway.monorail",      1, (SUMOReal)(30. /3.6),  1, SVC_LIGHTRAIL);  // rail stuff has to be discussed


    /* Parse file(s)
     * Each file is parsed twice: first for nodes, second for edges. */
    std::vector<std::string> files = oc.getStringVector("osm-files");
    // load nodes, first
    std::map<int, NIOSMNode*> nodes;
    NodesHandler nodesHandler(nodes);
    for (std::vector<std::string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        // nodes
        if (!FileHelpers::exists(*file)) {
            MsgHandler::getErrorInstance()->inform("Could not open osm-file '" + *file + "'.");
            return;
        }
        nodesHandler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing nodes from osm-file '" + *file + "'...");
        if (!XMLSubSys::runParser(nodesHandler, *file)) {
            return;
        }
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // load edges, then
    std::map<std::string, Edge*> edges;
    EdgesHandler edgesHandler(nodes, edges);
    for (std::vector<std::string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        // edges
        edgesHandler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing edges from osm-file '" + *file + "'...");
        XMLSubSys::runParser(edgesHandler, *file);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }

    /* Remove duplicate nodes with the same coordinates
     *
     * Without that, insertEdge can fail, if both nodes start
     * the shape of an edge. (NBEdge::init calls Position2DVector::push_front
     * with the second, which has the same coordinates as the first.) */
    if (!OptionsCont::getOptions().getBool("osm.skip-duplicates-check")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Removing duplicate nodes...");
        if (nodes.size() > 1) {
            std::set<const NIOSMNode*, CompareNodes> dupsFinder;
            for (std::map<int, NIOSMNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
                const std::set<const NIOSMNode*, CompareNodes>::iterator origNode = dupsFinder.find(it->second);
                if (origNode != dupsFinder.end()) {
                    MsgHandler::getMessageInstance()->inform("Found duplicate nodes. Substituting " + toString(it->second->id) + " with " + toString((*origNode)->id));
                    for_each(edges.begin(), edges.end(), SubstituteNode(it->second, *origNode));
                } else {
                    dupsFinder.insert(it->second);
                }
            }
        }
        MsgHandler::getMessageInstance()->endProcessMsg(" done.");
    }

    /* Remove duplicate edges with the same shape and attributes */
    if (!OptionsCont::getOptions().getBool("osm.skip-duplicates-check")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Removing duplicate edges...");
        if (edges.size() > 1) {
            std::set<std::string> toRemove;
            for (std::map<std::string, Edge*>::iterator it = edges.begin(), itnext =++edges.begin(); itnext != edges.end(); ++it, ++itnext) {
                std::map<std::string, Edge*>::iterator dupEdge = find_if(itnext, edges.end(), SimilarEdge(*it));
                while (dupEdge != edges.end()) {
                    MsgHandler::getMessageInstance()->inform("Found duplicate edges. Removing " + dupEdge->first);
                    toRemove.insert(dupEdge->first);
                    dupEdge = find_if(++dupEdge, edges.end(), SimilarEdge(*it));
                }
            }
            for (std::set<std::string>::iterator i=toRemove.begin(); i!=toRemove.end(); ++i) {
                std::map<std::string, Edge*>::iterator j=edges.find(*i);
                delete(*j).second;
                edges.erase(j);
            }
        }
        MsgHandler::getMessageInstance()->endProcessMsg(" done.");
    }

    /* Mark which nodes are used (by edges or traffic lights).
     * This is necessary to detect which OpenStreetMap nodes are for
     * geometry only */
    std::map<int, int> nodeUsage;
    // Mark which nodes are used by edges (begin and end)
    for (std::map<std::string, Edge*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        Edge *e = (*i).second;
        if (!e->myCurrentIsRoad) {
            continue;
        }
        for (std::vector<int>::const_iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
            if (nodeUsage.find(*j)==nodeUsage.end()) {
                nodeUsage[*j] = 0;
            }
            nodeUsage[*j] = nodeUsage[*j] + 1;
        }
    }
    // Mark which nodes are used by traffic lights
    for (std::map<int, NIOSMNode*>::const_iterator nodesIt = nodes.begin(); nodesIt != nodes.end(); ++nodesIt) {
        if (nodesIt->second->tlsControlled) {
            // If the key is not found in the map, the value is automatically
            // initialized with 0.
            nodeUsage[nodesIt->first] += 1;
        }
    }
    /* Instantiate edges
     * Only those nodes in the middle of an edge which are used by more than
     * one edge are instantiated. Other nodes are considered as geometry nodes. */
    NBNodeCont &nc = nb.getNodeCont();
    NBEdgeCont &ec = nb.getEdgeCont();
    NBTrafficLightLogicCont &tlsc = nb.getTLLogicCont();
    for (std::map<std::string, Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
        Edge *e = (*i).second;
        if (!e->myCurrentIsRoad) {
            continue;
        }
        // build nodes;
        //  the from- and to-nodes must be built in any case
        //  the geometry nodes are only built if more than one edge references them
        NBNode *currentFrom = insertNodeChecking(*e->myCurrentNodes.begin(), nodes, nc, tlsc);
        NBNode *last = insertNodeChecking(*(e->myCurrentNodes.end()-1), nodes, nc, tlsc);
        int running = 0;
        std::vector<int> passed;
        for (std::vector<int>::iterator j=e->myCurrentNodes.begin(); j!=e->myCurrentNodes.end(); ++j) {
            passed.push_back(*j);
            if (nodeUsage[*j] > 1 && j != e->myCurrentNodes.end()-1 && j != e->myCurrentNodes.begin()) {
                NBNode *currentTo = insertNodeChecking(*j, nodes, nc, tlsc);
                insertEdge(e, running, currentFrom, currentTo, passed, nodes, nc, ec, tc);
                currentFrom = currentTo;
                running++;
                passed.clear();
            }
        }
        if (running==0) {
            running = -1;
        }
        insertEdge(e, running, currentFrom, last, passed, nodes, nc, ec, tc);
    }
    /* Clean up */
    // delete nodes
    for (std::map<int, NIOSMNode*>::const_iterator i=nodes.begin(); i!=nodes.end(); ++i) {
        delete(*i).second;
    }
    // delete edges
    for (std::map<std::string, Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
        delete(*i).second;
    }
}


NBNode *
NIImporter_OpenStreetMap::insertNodeChecking(int id, const std::map<int, NIOSMNode*> &osmNodes, NBNodeCont &nc,
        NBTrafficLightLogicCont &tlsc) throw(ProcessError) {
    NBNode *from = nc.retrieve(toString(id));
    if (from==0) {
        NIOSMNode *n = osmNodes.find(id)->second;
        Position2D pos(n->lon, n->lat);
        if (!GeoConvHelper::x2cartesian(pos, true, n->lon, n->lat)) {
            MsgHandler::getErrorInstance()->inform("Unable to project coordinates for node " + toString(id) + ".");
            delete from;
            return 0;
        }
        from = new NBNode(toString(id), pos);
        if (!nc.insert(from)) {
            MsgHandler::getErrorInstance()->inform("Could not insert node '" + toString(id) + "').");
            delete from;
            return 0;
        }
        if (n->tlsControlled) {
            // ok, this node is a traffic light node where no other nodes
            //  participate
            NBOwnTLDef *tlDef = new NBOwnTLDef(toString(id), from);
            if (!tlsc.insert(tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError("Could not allocate tls '" + toString(id) + "'.");
            }
        }
    }
    return from;
}


void
NIImporter_OpenStreetMap::insertEdge(Edge *e, int index, NBNode *from, NBNode *to,
                                     const std::vector<int> &passed, const std::map<int, NIOSMNode*> &osmNodes,
                                     NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc) throw(ProcessError) {
    UNUSED_PARAMETER(nc);
    // patch the id
    std::string id = e->id;
    if (index>=0) {
        id = id + "#" + toString(index);
    }
    // convert the shape
    Position2DVector shape;
    for (std::vector<int>::const_iterator i=passed.begin(); i!=passed.end(); ++i) {
        NIOSMNode *n = osmNodes.find(*i)->second;
        Position2D pos(n->lon, n->lat);
        if (!GeoConvHelper::x2cartesian(pos, true, n->lon, n->lat)) {
            throw ProcessError("Unable to project coordinates for edge " + id + ".");
        }
        shape.push_back_noDoublePos(pos);
    }

    std::string type = e->myHighWayType;
    if (!tc.knows(type)) {
        if (type.find(compoundTypeSeparator)!=std::string::npos) {
            // this edge has a combination type which does not yet exist in the TypeContainer
            StringTokenizer tok = StringTokenizer(type, compoundTypeSeparator);
            std::set<std::string> types;
            while (tok.hasNext()) {
                std::string t = tok.next();
                if (tc.knows(t)) {
                    types.insert(t);
                } else {
                    WRITE_WARNING("Discarding edge " + id + " with type \"" + type + "\" (unknown compound \"" + t + "\").");
                    return;
                }
            }

            if (types.size() == 2 &&
                    types.count("railway.tram")==1) {
                // compound types concern mostly the special case of tram tracks on a normal road.
                // in this case we simply discard the tram information since the default for road is to allow all vclasses
                types.erase("railway.tram");
                std::string otherCompound = *(types.begin());
                // XXX if otherCompound does not allow all vehicles (e.g. SVC_DELIVERY), tram will still not be allowed
                type = otherCompound;
            } else {
                // other cases not implemented yet
                WRITE_WARNING("Discarding edge " + id + " with unknown type \"" + type + "\".");
                return;
            }
        } else {
            // we do not know the type -> something else, ignore
            // WRITE_WARNING("Discarding edge " + id + " with unknown type \"" + type + "\".");
            return;
        }
    }



    // otherwise it is not an edge and will be ignored
    int noLanes = tc.getNoLanes(type);
    SUMOReal speed = tc.getSpeed(type);
    bool defaultsToOneWay = tc.getIsOneWay(type);
    std::vector<SUMOVehicleClass> allowedClasses = tc.getAllowedClasses(type);
    std::vector<SUMOVehicleClass> disallowedClasses = tc.getDisallowedClasses(type);
    // check directions
    bool addSecond = true;
    if (e->myIsOneWay=="true"||e->myIsOneWay=="yes"||e->myIsOneWay=="1"||(defaultsToOneWay && e->myIsOneWay!="no" && e->myIsOneWay!="false" && e->myIsOneWay!="0")) {
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
    if (e->myMaxSpeed >= 0) {
        speed = (SUMOReal)(e->myMaxSpeed / 3.6);
    }

    if (noLanes!=0&&speed!=0) {
        if (e->myIsOneWay!=""&&e->myIsOneWay!="false"&&e->myIsOneWay!="no"&&e->myIsOneWay!="true"&&e->myIsOneWay!="yes"&&e->myIsOneWay!="-1"&&e->myIsOneWay!="1") {
            WRITE_WARNING("New value for oneway found: " + e->myIsOneWay);
        }
        NBEdge::LaneSpreadFunction lsf = addSecond ? NBEdge::LANESPREAD_RIGHT : NBEdge::LANESPREAD_CENTER;
        if (e->myIsOneWay!="-1") {
            NBEdge *nbe = new NBEdge(id, from, to, type, speed, noLanes, tc.getPriority(type), shape, lsf);
            nbe->setVehicleClasses(allowedClasses, disallowedClasses);
            if (!ec.insert(nbe)) {
                delete nbe;
                throw ProcessError("Could not add edge '" + id + "'.");
            }
        }
        if (addSecond) {
            if (e->myIsOneWay!="-1") {
                id = "-" + id;
            }
            NBEdge *nbe = new NBEdge(id, to, from, type, speed, noLanes, tc.getPriority(type), shape.reverse(), lsf);
            nbe->setVehicleClasses(allowedClasses, disallowedClasses);
            if (!ec.insert(nbe)) {
                delete nbe;
                throw ProcessError("Could not add edge '-" + id + "'.");
            }
        }
    }
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_OpenStreetMap::NodesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_OpenStreetMap::NodesHandler::NodesHandler(std::map<int, NIOSMNode*> &toFill) throw()
        : SUMOSAXHandler("osm - file"), myToFill(toFill), myLastNodeID(-1), myIsInValidNodeTag(false), myHierarchyLevel(0) {
}


NIImporter_OpenStreetMap::NodesHandler::~NodesHandler() throw() {}


void
NIImporter_OpenStreetMap::NodesHandler::myStartElement(SumoXMLTag element, const SUMOSAXAttributes &attrs) throw(ProcessError) {
    ++myHierarchyLevel;
    if (element == SUMO_TAG_NODE) {
        bool ok = true;
        if (myHierarchyLevel != 2) {
            MsgHandler::getErrorInstance()->inform("Node element on wrong XML hierarchy level (id='" + toString(attrs.getIntReporting(SUMO_ATTR_ID, "node", 0, ok)) + "', level='" + toString(myHierarchyLevel) + "').");
            return;
        }
        int id = attrs.getIntReporting(SUMO_ATTR_ID, "node", 0, ok);
        std::string action = attrs.hasAttribute("action") ? attrs.getStringSecure("action", "") : "";
        if (action=="delete") {
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
            std::istringstream lon(attrs.getStringReporting(SUMO_ATTR_LON, "node", toString(id).c_str(), ok));
            if (!ok) {
                return;
            }
            lon >> tlon;
            if (lon.fail()) {
                MsgHandler::getErrorInstance()->inform("Node's '" + toString(id) + "' lon information is not numeric.");
                return;
            }
            std::istringstream lat(attrs.getStringReporting(SUMO_ATTR_LAT, "node", toString(id).c_str(), ok));
            if (!ok) {
                return;
            }
            lat >> tlat;
            if (lat.fail()) {
                MsgHandler::getErrorInstance()->inform("Node's '" + toString(id) + "' lat information is not numeric.");
                return;
            }
            NIOSMNode *toAdd = new NIOSMNode();
            toAdd->id = id;
            toAdd->tlsControlled = false;
            toAdd->lat = tlat;
            toAdd->lon = tlon;
            myToFill[toAdd->id] = toAdd;
            myIsInValidNodeTag = true;
        }
    }
    if (element == SUMO_TAG_TAG && myIsInValidNodeTag) {
        if (myHierarchyLevel != 3) {
            MsgHandler::getErrorInstance()->inform("Tag element on wrong XML hierarchy level.");
            return;
        }
        bool ok = true;
        std::string key = attrs.getStringReporting(SUMO_ATTR_K, "tag", toString(myLastNodeID).c_str(), ok);
        std::string value = attrs.getStringReporting(SUMO_ATTR_V, "tag", toString(myLastNodeID).c_str(), ok);
        if (!ok) {
            return;
        }
        if (key == "highway" && value.find("traffic_signal") != std::string::npos) {
            myToFill[myLastNodeID]->tlsControlled = true;
        }
    }
}


void
NIImporter_OpenStreetMap::NodesHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_NODE && myHierarchyLevel == 2) {
        myLastNodeID = -1;
        myIsInValidNodeTag = false;
    }
    --myHierarchyLevel;
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_OpenStreetMap::EdgesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_OpenStreetMap::EdgesHandler::EdgesHandler(
    const std::map<int, NIOSMNode*> &osmNodes,
    std::map<std::string, Edge*> &toFill) throw()
        : SUMOSAXHandler("osm - file"),
        myOSMNodes(osmNodes), myEdgeMap(toFill) {
    mySpeedMap["none"] = 300.;
    mySpeedMap["no"] = 300.;
    mySpeedMap["walk"] = 5.;
    mySpeedMap["DE:rural"] = 50.;
    mySpeedMap["DE:living_street"] = 10.;

}


NIImporter_OpenStreetMap::EdgesHandler::~EdgesHandler() throw() {
}


void
NIImporter_OpenStreetMap::EdgesHandler::myStartElement(SumoXMLTag element,
        const SUMOSAXAttributes &attrs) throw(ProcessError) {
    myParentElements.push_back(element);
    // parse "way" elements
    if (element==SUMO_TAG_WAY) {
        bool ok = true;
        std::string id = attrs.getStringReporting(SUMO_ATTR_ID, "way", 0, ok);
        std::string action = attrs.hasAttribute("action") ? attrs.getStringSecure("action", "") : "";
        if (action=="delete") {
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
        myCurrentEdge->myMaxSpeed = -1;
        myCurrentEdge->myCurrentIsRoad = false;
    }
    // parse "nd" (node) elements
    if (element==SUMO_TAG_ND) {
        bool ok = true;
        int ref = attrs.getIntReporting(SUMO_ATTR_REF, "nd", 0, ok);
        if (ok) {
            if (myOSMNodes.find(ref)==myOSMNodes.end()) {
                WRITE_WARNING("The referenced geometry information (ref='" + toString(ref) + "') is not known");
                return;
            }
            myCurrentEdge->myCurrentNodes.push_back(ref);
        }
    }
    // parse values
    if (element==SUMO_TAG_TAG&&myParentElements.size()>2&&myParentElements[myParentElements.size()-2]==SUMO_TAG_WAY) {
        if (myCurrentEdge==0) {
            return;
        }
        bool ok = true;
        std::string key = attrs.getStringReporting(SUMO_ATTR_K, "way", toString(myCurrentEdge->id).c_str(), ok);
        std::string value = attrs.getStringReporting(SUMO_ATTR_V, "way", toString(myCurrentEdge->id).c_str(), ok);
        if (!ok) {
            return;
        }
        if (key=="highway"||key=="railway") {
            if (myCurrentEdge->myHighWayType != "") {
                // osm-ways may be used by more than one mode (eg railway.tram + highway.residential. this is relevant for multimodal traffic)
                // we create a new type for this kind of situation which must then be resolved in insertEdge()
                myCurrentEdge->myHighWayType = myCurrentEdge->myHighWayType + compoundTypeSeparator + key + "." + value;
            } else {
                myCurrentEdge->myHighWayType = key + "." + value;
            }
            myCurrentEdge->myCurrentIsRoad = true;
        } else if (key=="lanes") {
            try {
                myCurrentEdge->myNoLanes = TplConvert<char>::_2int(value.c_str());
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" + myCurrentEdge->id + "'.");
            }
        } else if (key=="maxspeed") {
            if (mySpeedMap.find(value)!=mySpeedMap.end()) {
                myCurrentEdge->myMaxSpeed = mySpeedMap[value];
            } else {
                if (value.find("km/h")!=std::string::npos) {
                    value = StringUtils::prune(value.substr(0, value.find_first_not_of("0123456789")));
                }
                try {
                    myCurrentEdge->myMaxSpeed = TplConvert<char>::_2SUMOReal(value.c_str());
                } catch (NumberFormatException &) {
                    WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" + myCurrentEdge->id + "'.");
                }
            }
        } else if (key=="junction") {
            if ((value == "roundabout") && (myCurrentEdge->myIsOneWay == "")) {
                myCurrentEdge->myIsOneWay = "yes";
            }
        } else if (key=="oneway") {
            myCurrentEdge->myIsOneWay = value;
        }
    }
}


void
NIImporter_OpenStreetMap::EdgesHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    myParentElements.pop_back();
    if (element==SUMO_TAG_WAY) {
        if (myCurrentEdge!=0 && myCurrentEdge->myCurrentIsRoad) {
            myEdgeMap[myCurrentEdge->id] = myCurrentEdge;
        } else {
            delete myCurrentEdge;
        }
        myCurrentEdge = 0;
    }
}




/****************************************************************************/

