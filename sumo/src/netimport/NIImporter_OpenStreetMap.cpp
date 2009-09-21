/****************************************************************************/
/// @file    NIImporter_OpenStreetMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in OpenStreetMap format
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
#include "NIImporter_OpenStreetMap.h"
#include <algorithm>
#include <functional>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
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
// used namespaces
// ===========================================================================
using namespace std;



// ===========================================================================
// Private classes
// ===========================================================================

/** @brief Functor which compares two NIOSMNodes according
 * to their coordinates
 *
 * Both nodes are given as pointers in the second element of two pairs.
 * This functor is helpful for example in the find_if algorithm.
 */
class NIImporter_OpenStreetMap::CompareNodesInPairs: public std::unary_function<
            std::pair<int, NIOSMNode*>, bool> {
public:
    /** @brief Initializes the functor with the fixed comparison partner
     *
     * @param[in] p0 A pair with the NIOSMNode all other nodes
     *               should be compared with.
     */

    CompareNodesInPairs(const std::pair<int, NIOSMNode*>& p0) :
            myP0(p0) {
    }
    /** @brief Compares the NIOSMNode (p1.second) with the node given
     * in the constructor for equality
     *
     * The geo coordinates (lat and lon) are compared to test the equality.
     *
     * @param[in] p1 A pair with the node to compare.
     * @return true if both nodes are equal; otherwise false.
     */
    bool operator()(const std::pair<int, NIOSMNode*>& p1) const {
        return (p1.second->lat == myP0.second->lat) && (p1.second->lon
                == myP0.second->lon);
    }

private:
    std::pair<int, NIOSMNode*> myP0;
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




// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NIImporter_OpenStreetMap::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("osm-files")) {
        return;
    }
    // preset types
    //  for highways
    NBTypeCont &tc = nb.getTypeCont();
    tc.insert("highway.motorway", 3, (SUMOReal)(160./3.6), 13, SVC_UNKNOWN, true);
    tc.insert("highway.motorway_link", 1, (SUMOReal)(80./3.6), 12, SVC_UNKNOWN, true);
    tc.insert("highway.trunk", 2, (SUMOReal)(100./3.6), 11);  // !!! 130km/h?
    tc.insert("highway.trunk_link", 1, (SUMOReal)(80./3.6), 10);
    tc.insert("highway.primary", 2, (SUMOReal)(100./3.6), 9);
    tc.insert("highway.primary_link", 1, (SUMOReal)(80./3.6), 8);
    tc.insert("highway.secondary", 2, (SUMOReal)(100./3.6), 7);
    tc.insert("highway.tertiary", 1, (SUMOReal)(80./3.6), 6);
    tc.insert("highway.unclassified", 1, (SUMOReal)(80./3.6), 5);
    tc.insert("highway.residential", 1, (SUMOReal)(50./3.6), 4);  // actually, maybe one lane for parking would be nice...
    tc.insert("highway.living_street", 1, (SUMOReal)(10./3.6), 3);
    tc.insert("highway.service", 1, (SUMOReal)(20./3.6), 2, SVC_DELIVERY);
    tc.insert("highway.track", 1, (SUMOReal)(20./3.6), 1);
    tc.insert("highway.pedestrian", 1, (SUMOReal)(30./3.6), 1, SVC_PEDESTRIAN);
    tc.insert("highway.services", 1, (SUMOReal)(30./3.6), 1);
    tc.insert("highway.unsurfaced", 1, (SUMOReal)(30./3.6), 1);  // additional
    tc.insert("highway.footway", 1, (SUMOReal)(30./3.6), 1, SVC_PEDESTRIAN);  // additional
    tc.insert("highway.pedestrian", 1, (SUMOReal)(30./3.6), 1, SVC_PEDESTRIAN);

    tc.insert("highway.path", 1, (SUMOReal)(10./3.6), 1, SVC_PEDESTRIAN);
    tc.insert("highway.bridleway", 1, (SUMOReal)(10./3.6), 1, SVC_PEDESTRIAN);  // no horse stuff
    tc.insert("highway.cycleway", 1, (SUMOReal)(20./3.6), 1, SVC_BICYCLE);
    tc.insert("highway.footway", 1, (SUMOReal)(10./3.6), 1, SVC_PEDESTRIAN);
    tc.insert("highway.step", 1, (SUMOReal)(5./3.6), 1, SVC_PEDESTRIAN);  // additional
    tc.insert("highway.steps", 1, (SUMOReal)(5./3.6), 1, SVC_PEDESTRIAN);  // :-) do not run too fast
    tc.insert("highway.stairs", 1, (SUMOReal)(5./3.6), 1, SVC_PEDESTRIAN);  // additional
    tc.insert("highway.bus_guideway", 1, (SUMOReal)(30./3.6), 1, SVC_BUS);

    //  for railways
    tc.insert("railway.rail", 1, (SUMOReal)(30./3.6), 1, SVC_RAIL_FAST);
    tc.insert("railway.tram", 1, (SUMOReal)(30./3.6), 1, SVC_CITYRAIL);
    tc.insert("railway.light_rail", 1, (SUMOReal)(30./3.6), 1, SVC_LIGHTRAIL);
    tc.insert("railway.subway", 1, (SUMOReal)(30./3.6), 1, SVC_CITYRAIL);
    tc.insert("railway.preserved", 1, (SUMOReal)(30./3.6), 1, SVC_LIGHTRAIL);
    tc.insert("railway.monorail", 1, (SUMOReal)(30./3.6), 1, SVC_LIGHTRAIL);  // rail stuff has to be discussed


    // parse file(s)
    vector<string> files = oc.getStringVector("osm-files");
    // load nodes, first
    std::map<int, NIOSMNode*> nodes;
    NodesHandler nodesHandler(nodes);
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        // nodes
        if (!FileHelpers::exists(*file)) {
            MsgHandler::getErrorInstance()->inform("Could not open osm-file '" + *file + "'.");
            return;
        }
        nodesHandler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing nodes from osm-file '" + *file + "'...");
        XMLSubSys::runParser(nodesHandler, *file);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // load edges, then
    std::map<std::string, Edge*> edges;
    EdgesHandler edgesHandler(nodes, edges);
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
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
    MsgHandler::getMessageInstance()->beginProcessMsg(
        "Removing duplicate nodes...");
    if (nodes.size() > 1) {
        // The algorithm compares a node (it) with the remaining part
        // of the list ( [itnext; end()[ ).
        for (map<int, NIOSMNode*>::iterator it = nodes.begin(), itnext =
                    ++nodes.begin(); itnext != nodes.end(); ++it, ++itnext) {

            map<int, NIOSMNode*>::iterator dupNode = find_if(itnext,
                    nodes.end(), CompareNodesInPairs(*it));

            if (dupNode != nodes.end()) {
                MsgHandler *mh = MsgHandler::getMessageInstance();
                mh->inform("Found duplicate nodes. Substitute " + toString(
                               dupNode->second->id) + " with " + toString(
                               it->second->id));

                for_each(edges.begin(), edges.end(), SubstituteNode(
                             dupNode->second, it->second));
            }
        }
    }
    MsgHandler::getMessageInstance()->endProcessMsg("...done.");

    // build all
    std::map<int, int> nodeUsage;
    // mark which nodes are used
    for (std::map<std::string, Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
        Edge *e = (*i).second;
        if (!e->myCurrentIsRoad) {
            continue;
        }
        for (std::vector<int>::iterator j=e->myCurrentNodes.begin(); j!=e->myCurrentNodes.end(); ++j) {
            if (nodeUsage.find(*j)==nodeUsage.end()) {
                nodeUsage[*j] = 0;
            }
            nodeUsage[*j] = nodeUsage[*j] + 1;
        }
    }
    // instatiate edges
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
            if (nodeUsage[*j]>1&&j!=e->myCurrentNodes.end()-1&&j!=e->myCurrentNodes.begin()) {
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
        if (!GeoConvHelper::x2cartesian(pos)) {
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
    // patch the id
    string id = e->id;
    if (index>=0) {
        id = id + "#" + toString(index);
    }
    // convert the shape
    Position2DVector shape;
    for (std::vector<int>::const_iterator i=passed.begin(); i!=passed.end(); ++i) {
        NIOSMNode *n = osmNodes.find(*i)->second;
        Position2D pos(n->lon, n->lat);
        if (!GeoConvHelper::x2cartesian(pos)) {
            throw ProcessError("Unable to project coordinates for edge " + id + ".");
        }
        shape.push_back_noDoublePos(pos);
    }
    if (!tc.knows(e->myHighWayType)) {
        // we do not know the type -> something else, ignore
        return;
    }
    // otherwise it is not an edge and will be ignored
    int noLanes = tc.getNoLanes(e->myHighWayType);
    SUMOReal speed = tc.getSpeed(e->myHighWayType);
    bool defaultsToOneWay = tc.getIsOneWay(e->myHighWayType);
    vector<SUMOVehicleClass> allowedClasses = tc.getAllowedClasses(e->myHighWayType);
    vector<SUMOVehicleClass> disallowedClasses = tc.getDisallowedClasses(e->myHighWayType);
    // check directions
    bool addSecond = true;
    // TODO: e->myIsOneWay=="-1" for oneway streets in opposite direction?
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
    // if we had been able to extract the maximum speed, override the highway type default
    if (e->myMaxSpeed >= 0) {
        speed = (SUMOReal)(e->myMaxSpeed / 3.6);
    }

    if (noLanes!=0&&speed!=0) {
        if (e->myIsOneWay!=""&&e->myIsOneWay!="false"&&e->myIsOneWay!="no"&&e->myIsOneWay!="true"&&e->myIsOneWay!="yes") {
            WRITE_WARNING("New value for oneway found: " + e->myIsOneWay);
        }
        NBEdge::LaneSpreadFunction lsf = addSecond ? NBEdge::LANESPREAD_RIGHT : NBEdge::LANESPREAD_CENTER;
        NBEdge *nbe = new NBEdge(id, from, to, e->myHighWayType, speed, noLanes, tc.getPriority(e->myHighWayType), shape, lsf);
        nbe->setVehicleClasses(allowedClasses, disallowedClasses);
        if (!ec.insert(nbe)) {
            delete nbe;
            throw ProcessError("Could not add edge '" + id + "'.");
        }
        if (addSecond) {
            nbe = new NBEdge("-" + id, to, from, e->myHighWayType, speed, noLanes, tc.getPriority(e->myHighWayType), shape.reverse(), lsf);
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
        : SUMOSAXHandler("osm - file"), myToFill(toFill), myLastNodeID(-1) {}


NIImporter_OpenStreetMap::NodesHandler::~NodesHandler() throw() {}


void
NIImporter_OpenStreetMap::NodesHandler::myStartElement(SumoXMLTag element, const SUMOSAXAttributes &attrs) throw(ProcessError) {
    myParentElements.push_back(element);
    if (element==SUMO_TAG_NODE) {
        bool ok = true;
        int id = attrs.getIntReporting(SUMO_ATTR_ID, "node", 0, ok);
        if (!ok) {
            return;
        }
        myLastNodeID = -1;
        if (myToFill.find(id)==myToFill.end()) {
            myLastNodeID = id;
            // assume we are loading multiple files...
            //  ... so we won't report duplicate nodes
            NIOSMNode *toAdd = new NIOSMNode();
            toAdd->id = id;
            toAdd->tlsControlled = false;
            try {
                toAdd->lon = attrs.getFloat(SUMO_ATTR_LON);
            } catch (EmptyData &) {
                MsgHandler::getErrorInstance()->inform("Node '" + toString(id) + "' has no lon information.");
                delete toAdd;
                return;
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("Node's '" + toString(id) + "' lon information is not numeric.");
                delete toAdd;
                return;
            }
            try {
                toAdd->lat = attrs.getFloat(SUMO_ATTR_LAT);
            } catch (EmptyData &) {
                MsgHandler::getErrorInstance()->inform("Node '" + toString(id) + "' has no lat information.");
                delete toAdd;
                return;
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("Node's '" + toString(id) + "' lat information is not numeric.");
                delete toAdd;
                return;
            }
            myToFill[toAdd->id] = toAdd;
        }
    }
    if (element==SUMO_TAG_TAG&&myParentElements.size()>2&&myParentElements[myParentElements.size()-2]==SUMO_TAG_NODE) {
        string key, value;
        try {
            // retrieve the id of the (geometry) node
            key = attrs.getString(SUMO_ATTR_K);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("'tag' in node '" + toString(myLastNodeID) + "' misses a value.");
            return;
        }
        try {
            // retrieve the id of the (geometry) node
            value = attrs.getString(SUMO_ATTR_V);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("'value' in node '" + toString(myLastNodeID) + "' misses a value.");
            return;
        }
        if (key=="highway"&&value.find("traffic_signal")!=string::npos) {
            if (myLastNodeID!=-1) {
                myToFill[myLastNodeID]->tlsControlled = true;
            }
        }
    }
}


void
NIImporter_OpenStreetMap::NodesHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_NODE) {
        myLastNodeID = -1;
    }
    myParentElements.pop_back();
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_OpenStreetMap::EdgesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_OpenStreetMap::EdgesHandler::EdgesHandler(
    const std::map<int, NIOSMNode*> &osmNodes,
    std::map<std::string, Edge*> &toFill) throw()
        : SUMOSAXHandler("osm - file"),
        myOSMNodes(osmNodes), myEdgeMap(toFill) {
}


NIImporter_OpenStreetMap::EdgesHandler::~EdgesHandler() throw() {
}


void
NIImporter_OpenStreetMap::EdgesHandler::myStartElement(SumoXMLTag element,
        const SUMOSAXAttributes &attrs) throw(ProcessError) {
    myParentElements.push_back(element);
    // parse "way" elements
    if (element==SUMO_TAG_WAY) {
        myCurrentEdge = new Edge();
        myCurrentEdge->myNoLanes = -1;
        myCurrentEdge->myMaxSpeed = -1;
        myCurrentEdge->myCurrentIsRoad = false;
        try {
            // retrieve the id of the edge
            myCurrentEdge->id = attrs.getString(SUMO_ATTR_ID);
        } catch (EmptyData &) {
            WRITE_WARNING("No edge id given... Skipping.");
            return;
        }
    }
    // parse "nd" (node) elements
    if (element==SUMO_TAG_ND) {
        bool ok = true;
        int ref = attrs.getIntReporting(SUMO_ATTR_REF, "nd", 0, ok);
        if (ok) {
            if (myOSMNodes.find(ref)==myOSMNodes.end()) {
                MsgHandler::getErrorInstance()->inform("The referenced geometry information (ref='" + toString(ref) + "') is not known");
                return;
            }
            myCurrentEdge->myCurrentNodes.push_back(ref);
        }
    }
    // parse values
    if (element==SUMO_TAG_TAG&&myParentElements.size()>2&&myParentElements[myParentElements.size()-2]==SUMO_TAG_WAY) {
        string key, value;
        try {
            // retrieve the id of the (geometry) node
            key = attrs.getString(SUMO_ATTR_K);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("'tag' in edge '" + myCurrentEdge->id + "' misses a value.");
            return;
        }
        try {
            // retrieve the id of the (geometry) node
            value = attrs.getString(SUMO_ATTR_V);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("'value' in edge '" + myCurrentEdge->id + "' misses a value.");
            return;
        }
        if (key=="highway"||key=="railway") {
            myCurrentEdge->myHighWayType = key + "." + value;
            myCurrentEdge->myCurrentIsRoad = true;
        } else if (key=="lanes") {
            try {
                myCurrentEdge->myNoLanes = TplConvert<char>::_2int(value.c_str());
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" + myCurrentEdge->id + "'.");
            }
        } else if (key=="maxspeed") {
            try {
                myCurrentEdge->myMaxSpeed = TplConvert<char>::_2SUMOReal(value.c_str());
            } catch (NumberFormatException &) {
                WRITE_WARNING("Value of key '" + key + "' is not numeric ('" + value + "') in edge '" + myCurrentEdge->id + "'.");
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
        if (myCurrentEdge->myCurrentIsRoad) {
            myEdgeMap[myCurrentEdge->id] = myCurrentEdge;
        } else {
            delete myCurrentEdge;
        }
        myCurrentEdge = 0;
    }
}




/****************************************************************************/

