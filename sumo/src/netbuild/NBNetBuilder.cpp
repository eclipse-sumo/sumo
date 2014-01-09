/****************************************************************************/
/// @file    NBNetBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Thimor Bohn
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    20 Nov 2001
/// @version $Id$
///
// Instance responsible for building networks
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

#include <string>
#include <fstream>
#include "NBNetBuilder.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include "NBDistrict.h"
#include "NBDistribution.h"
#include "NBRequest.h"
#include "NBTypeCont.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include "NBAlgorithms.h"
#include "NBAlgorithms_Ramps.h"

#ifdef HAVE_INTERNAL
#include <internal/HeightMapper.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBNetBuilder::NBNetBuilder() :
    myEdgeCont(myTypeCont),
    myHaveSeenRoundabouts(false)
{}


NBNetBuilder::~NBNetBuilder() {}


void
NBNetBuilder::applyOptions(OptionsCont& oc) {
    // we possibly have to load the edges to keep
    if (oc.isSet("keep-edges.input-file")) {
        std::ifstream strm(oc.getString("keep-edges.input-file").c_str());
        if (!strm.good()) {
            throw ProcessError("Could not load names of edges too keep from '" + oc.getString("keep-edges.input-file") + "'.");
        }
        std::ostringstream oss;
        bool first = true;
        while (strm.good()) {
            if (!first) {
                oss << ',';
            }
            std::string name;
            strm >> name;
            oss << name;
            first = false;
        }
        oc.set("keep-edges.explicit", oss.str());
    }
    // apply options to type control
    myTypeCont.setDefaults(oc.getInt("default.lanenumber"), oc.getFloat("default.speed"), oc.getInt("default.priority"));
    // apply options to edge control
    myEdgeCont.applyOptions(oc);
    // apply options to traffic light logics control
    myTLLCont.applyOptions(oc);
}


void
NBNetBuilder::compute(OptionsCont& oc,
                      const std::set<std::string>& explicitTurnarounds,
                      bool removeElements) {
    GeoConvHelper& geoConvHelper = GeoConvHelper::getProcessing();


    // MODIFYING THE SETS OF NODES AND EDGES

    // Removes edges that are connecting the same node
    PROGRESS_BEGIN_MESSAGE("Removing self-loops");
    myNodeCont.removeSelfLoops(myDistrictCont, myEdgeCont, myTLLCont);
    PROGRESS_DONE_MESSAGE();
    //
    if (oc.exists("remove-edges.isolated") && oc.getBool("remove-edges.isolated")) {
        PROGRESS_BEGIN_MESSAGE("Finding isolated roads");
        myNodeCont.removeIsolatedRoads(myDistrictCont, myEdgeCont, myTLLCont);
        PROGRESS_DONE_MESSAGE();
    }
    //
    if (oc.exists("keep-edges.postload") && oc.getBool("keep-edges.postload")) {
        if (oc.isSet("keep-edges.explicit")) {
            PROGRESS_BEGIN_MESSAGE("Removing unwished edges");
            myEdgeCont.removeUnwishedEdges(myDistrictCont);
            PROGRESS_DONE_MESSAGE();
        }
    }
    // join junctions (may create new "geometry"-nodes so it needs to come before removing these
    if (oc.exists("junctions.join-exclude") && oc.isSet("junctions.join-exclude")) {
        myNodeCont.addJoinExclusion(oc.getStringVector("junctions.join-exclude"));
    }
    unsigned int numJoined = myNodeCont.joinLoadedClusters(myDistrictCont, myEdgeCont, myTLLCont);
    if (oc.getBool("junctions.join")) {
        PROGRESS_BEGIN_MESSAGE("Joining junction clusters");
        // preliminary geometry computations to determine the length of edges
        // This depends on turning directions and sorting of edge list
        // in case junctions are joined geometry computations have to be repeated
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont);
        NBNodesEdgesSorter::sortNodesEdges(myNodeCont, oc.getBool("lefthand"));
        myNodeCont.computeNodeShapes(oc.getBool("lefthand"));
        myEdgeCont.computeEdgeShapes();
        // preliminary roundabout computations to avoid destroying roundabouts
        if (oc.getBool("roundabouts.guess") || (oc.isDefault("roundabouts.guess") && myHaveSeenRoundabouts)) {
            assert(myRoundabouts.size() == 0);
            myEdgeCont.guessRoundabouts(myRoundabouts);
            for (std::vector<EdgeVector>::const_iterator it_round = myRoundabouts.begin();
                    it_round != myRoundabouts.end(); ++it_round) {
                std::vector<std::string> nodeIDs;
                for (EdgeVector::const_iterator it_edge = it_round->begin(); it_edge != it_round->end(); ++it_edge) {
                    nodeIDs.push_back((*it_edge)->getToNode()->getID());
                }
                myNodeCont.addJoinExclusion(nodeIDs);
            }
            myRoundabouts.clear();
        }
        numJoined += myNodeCont.joinJunctions(oc.getFloat("junctions.join-dist"), myDistrictCont, myEdgeCont, myTLLCont);
        // reset geometry to avoid influencing subsequent steps (ramps.guess)
        myEdgeCont.computeLaneShapes();
        PROGRESS_DONE_MESSAGE();
    }
    if (numJoined > 0) {
        // bit of a misnomer since we're already done
        WRITE_MESSAGE(" Joined " + toString(numJoined) + " junction cluster(s).");
    }
    //
    if (removeElements) {
        unsigned int no = 0;
        const bool removeGeometryNodes = oc.exists("geometry.remove") && oc.getBool("geometry.remove");
        PROGRESS_BEGIN_MESSAGE("Removing empty nodes" + std::string(removeGeometryNodes ? " and geometry nodes" : ""));
        no = myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myJoinedEdges, myTLLCont, removeGeometryNodes);
        PROGRESS_DONE_MESSAGE();
        WRITE_MESSAGE("   " + toString(no) + " nodes removed.");
    }

    // MOVE TO ORIGIN
    // compute new boundary after network modifications have taken place
    Boundary boundary;
    for (std::map<std::string, NBNode*>::const_iterator it = myNodeCont.begin(); it != myNodeCont.end(); ++it) {
        boundary.add(it->second->getPosition());
    }
    for (std::map<std::string, NBEdge*>::const_iterator it = myEdgeCont.begin(); it != myEdgeCont.end(); ++it) {
        boundary.add(it->second->getGeometry().getBoxBoundary());
    }
    geoConvHelper.setConvBoundary(boundary);

    if (!oc.getBool("offset.disable-normalization") && oc.isDefault("offset.x") && oc.isDefault("offset.y")) {
        moveToOrigin(geoConvHelper);
    }
    geoConvHelper.computeFinal(); // information needed for location element fixed at this point

    if (oc.exists("geometry.min-dist") && oc.isSet("geometry.min-dist")) {
        PROGRESS_BEGIN_MESSAGE("Reducing geometries");
        myEdgeCont.reduceGeometries(oc.getFloat("geometry.min-dist"));
        PROGRESS_DONE_MESSAGE();
    }
    // @note: removing geometry can create similar edges so joinSimilarEdges  must come afterwards
    // @note: likewise splitting can destroy similarities so joinSimilarEdges must come before
    if (removeElements) {
        PROGRESS_BEGIN_MESSAGE("Joining similar edges");
        myJoinedEdges.init(myEdgeCont);
        myNodeCont.joinSimilarEdges(myDistrictCont, myEdgeCont, myTLLCont);
        PROGRESS_DONE_MESSAGE();
    }
    //
    if (oc.exists("geometry.split") && oc.getBool("geometry.split")) {
        PROGRESS_BEGIN_MESSAGE("Splitting geometry edges");
        myEdgeCont.splitGeometry(myNodeCont);
        PROGRESS_DONE_MESSAGE();
    }
    // guess ramps
    if ((oc.exists("ramps.guess") && oc.getBool("ramps.guess")) || (oc.exists("ramps.set") && oc.isSet("ramps.set"))) {
        PROGRESS_BEGIN_MESSAGE("Guessing and setting on-/off-ramps");
        NBNodesEdgesSorter::sortNodesEdges(myNodeCont, oc.getBool("lefthand"));
        NBRampsComputer::computeRamps(*this, oc);
        PROGRESS_DONE_MESSAGE();
    }

    // check whether any not previously setable connections may be set now
    myEdgeCont.recheckPostProcessConnections();
    //
    if (oc.exists("geometry.max-angle")) {
        myEdgeCont.checkGeometries(
            oc.getFloat("geometry.max-angle"),
            oc.getFloat("geometry.min-radius"),
            oc.getBool("geometry.min-radius.fix"));
    }
    //
    myEdgeCont.computeLaneShapes();

    // APPLY SPEED MODIFICATIONS
    if (oc.exists("speed.offset")) {
        const SUMOReal speedOffset = oc.getFloat("speed.offset");
        const SUMOReal speedFactor = oc.getFloat("speed.factor");
        if (speedOffset != 0 || speedFactor != 1) {
            PROGRESS_BEGIN_MESSAGE("Applying speed modifications");
            for (std::map<std::string, NBEdge*>::const_iterator i = myEdgeCont.begin(); i != myEdgeCont.end(); ++i) {
                (*i).second->setSpeed(-1, (*i).second->getSpeed() * speedFactor + speedOffset);
            }
            PROGRESS_DONE_MESSAGE();
        }
    }

    // CONNECTIONS COMPUTATION
    //
    PROGRESS_BEGIN_MESSAGE("Computing turning directions");
    NBTurningDirectionsComputer::computeTurnDirections(myNodeCont);
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Sorting nodes' edges");
    NBNodesEdgesSorter::sortNodesEdges(myNodeCont, oc.getBool("lefthand"));
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Computing node types");
    NBNodeTypeComputer::computeNodeTypes(myNodeCont);
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Computing priorities");
    NBEdgePriorityComputer::computeEdgePriorities(myNodeCont);
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Computing approached edges");
    myEdgeCont.computeEdge2Edges(oc.getBool("no-left-connections"));
    PROGRESS_DONE_MESSAGE();
    //
    if (oc.getBool("roundabouts.guess") || (oc.isDefault("roundabouts.guess") && myHaveSeenRoundabouts)) {
        PROGRESS_BEGIN_MESSAGE("Guessing and setting roundabouts");
        myEdgeCont.guessRoundabouts(myRoundabouts);
        PROGRESS_DONE_MESSAGE();
    }
    //
    PROGRESS_BEGIN_MESSAGE("Computing approaching lanes");
    myEdgeCont.computeLanes2Edges();
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Dividing of lanes on approached lanes");
    myNodeCont.computeLanes2Lanes();
    myEdgeCont.sortOutgoingLanesConnections();
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Processing turnarounds");
    if (!oc.getBool("no-turnarounds")) {
        myEdgeCont.appendTurnarounds(oc.getBool("no-turnarounds.tls"));
    } else {
        myEdgeCont.appendTurnarounds(explicitTurnarounds, oc.getBool("no-turnarounds.tls"));
    }
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Rechecking of lane endings");
    myEdgeCont.recheckLanes();
    PROGRESS_DONE_MESSAGE();


    // GEOMETRY COMPUTATION
    //
    PROGRESS_BEGIN_MESSAGE("Computing node shapes");
    if (oc.exists("geometry.junction-mismatch-threshold")) {
        myNodeCont.computeNodeShapes(oc.getBool("lefthand"), oc.getFloat("geometry.junction-mismatch-threshold"));
    } else {
        myNodeCont.computeNodeShapes(oc.getBool("lefthand"));
    }
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Computing edge shapes");
    myEdgeCont.computeEdgeShapes();
    PROGRESS_DONE_MESSAGE();


    // GUESS TLS POSITIONS
    PROGRESS_BEGIN_MESSAGE("Assigning nodes to traffic lights");
    if (oc.isSet("tls.set")) {
        std::vector<std::string> tlControlledNodes = oc.getStringVector("tls.set");
        TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(oc.getString("tls.default-type"));
        for (std::vector<std::string>::const_iterator i = tlControlledNodes.begin(); i != tlControlledNodes.end(); ++i) {
            NBNode* node = myNodeCont.retrieve(*i);
            if (node == 0) {
                WRITE_WARNING("Building a tl-logic for node '" + *i + "' is not possible." + "\n The node '" + *i + "' is not known.");
            } else {
                myNodeCont.setAsTLControlled(node, myTLLCont, type);
            }
        }
    }
    myNodeCont.guessTLs(oc, myTLLCont);
    PROGRESS_DONE_MESSAGE();
    //
    if (oc.getBool("tls.join")) {
        PROGRESS_BEGIN_MESSAGE("Joining traffic light nodes");
        myNodeCont.joinTLS(myTLLCont, oc.getFloat("tls.join-dist"));
        PROGRESS_DONE_MESSAGE();
    }


    // COMPUTING RIGHT-OF-WAY AND TRAFFIC LIGHT PROGRAMS
    //
    PROGRESS_BEGIN_MESSAGE("Computing traffic light control information");
    myTLLCont.setTLControllingInformation(myEdgeCont);
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Computing node logics");
    myNodeCont.computeLogics(myEdgeCont, oc);
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Computing traffic light logics");
    std::pair<unsigned int, unsigned int> numbers = myTLLCont.computeLogics(myEdgeCont, oc);
    PROGRESS_DONE_MESSAGE();
    std::string progCount = "";
    if (numbers.first != numbers.second) {
        progCount = "(" + toString(numbers.second) + " programs) ";
    }
    WRITE_MESSAGE(" " + toString(numbers.first) + " traffic light(s) " + progCount + "computed.");
    //
    if (oc.isSet("street-sign-output")) {
        PROGRESS_BEGIN_MESSAGE("Generating street signs");
        myEdgeCont.generateStreetSigns();
        PROGRESS_DONE_MESSAGE();
    }

    // FINISHING INNER EDGES
    if (!oc.getBool("no-internal-links")) {
        PROGRESS_BEGIN_MESSAGE("Building inner edges");
        for (std::map<std::string, NBEdge*>::const_iterator i = myEdgeCont.begin(); i != myEdgeCont.end(); ++i) {
            (*i).second->sortOutgoingConnectionsByIndex();
        }
        for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
            (*i).second->buildInnerEdges();
        }
        PROGRESS_DONE_MESSAGE();
    }


    // report
    WRITE_MESSAGE("-----------------------------------------------------");
    WRITE_MESSAGE("Summary:");
    myNodeCont.printBuiltNodesStatistics();
    WRITE_MESSAGE(" Network boundaries:");
    WRITE_MESSAGE("  Original boundary  : " + toString(geoConvHelper.getOrigBoundary()));
    WRITE_MESSAGE("  Applied offset     : " + toString(geoConvHelper.getOffsetBase()));
    WRITE_MESSAGE("  Converted boundary : " + toString(geoConvHelper.getConvBoundary()));
    WRITE_MESSAGE("-----------------------------------------------------");
    NBRequest::reportWarnings();
    // report on very large networks
    if (MAX2(geoConvHelper.getConvBoundary().xmax(), geoConvHelper.getConvBoundary().ymax()) > 1000000 ||
            MIN2(geoConvHelper.getConvBoundary().xmin(), geoConvHelper.getConvBoundary().ymin()) < -1000000) {
        WRITE_WARNING("Network contains very large coordinates and will probably flicker in the GUI. Check for outlying nodes and make sure the network is shifted to the coordinate origin");
    }
}


void
NBNetBuilder::moveToOrigin(GeoConvHelper& geoConvHelper) {
    PROGRESS_BEGIN_MESSAGE("Moving network to origin");
    Boundary boundary = geoConvHelper.getConvBoundary();
    const SUMOReal x = -boundary.xmin();
    const SUMOReal y = -boundary.ymin();
    for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
        (*i).second->reshiftPosition(x, y);
    }
    for (std::map<std::string, NBEdge*>::const_iterator i = myEdgeCont.begin(); i != myEdgeCont.end(); ++i) {
        (*i).second->reshiftPosition(x, y);
    }
    for (std::map<std::string, NBDistrict*>::const_iterator i = myDistrictCont.begin(); i != myDistrictCont.end(); ++i) {
        (*i).second->reshiftPosition(x, y);
    }
    geoConvHelper.moveConvertedBy(x, y);
    PROGRESS_DONE_MESSAGE();
}


bool
NBNetBuilder::transformCoordinates(Position& from, bool includeInBoundary, GeoConvHelper* from_srs) {
    Position orig(from);
    bool ok = GeoConvHelper::getProcessing().x2cartesian(from, includeInBoundary);
#ifdef HAVE_INTERNAL
    if (ok) {
        const HeightMapper& hm = HeightMapper::get();
        if (hm.ready()) {
            if (from_srs != 0 && from_srs->usingGeoProjection()) {
                from_srs->cartesian2geo(orig);
            }
            SUMOReal z = hm.getZ(orig);
            from = Position(from.x(), from.y(), z);
        }
    }
#else
    UNUSED_PARAMETER(from_srs);
#endif
    return ok;
}


bool
NBNetBuilder::transformCoordinates(PositionVector& from, bool includeInBoundary, GeoConvHelper* from_srs) {
    const SUMOReal maxLength = OptionsCont::getOptions().getFloat("geometry.max-segment-length");
    if (maxLength > 0 && from.size() > 1) {
        // transformation to cartesian coordinates must happen before we can check segment length
        PositionVector copy = from;
        for (int i = 0; i < (int) from.size(); i++) {
            transformCoordinates(copy[i], false);
        }
        // check lengths and insert new points where needed (in the original
        // coordinate system)
        int inserted = 0;
        for (int i = 0; i < (int)copy.size() - 1; i++) {
            Position start = from[i + inserted];
            Position end = from[i + inserted + 1];
            SUMOReal length = copy[i].distanceTo(copy[i + 1]);
            const Position step = (end - start) * (maxLength / length);
            int steps = 0;
            while (length > maxLength) {
                length -= maxLength;
                steps++;
                from.insertAt(i + inserted + 1, start + (step * steps));
                inserted++;
            }
        }
        // now perform the transformation again so that height mapping can be
        // performed for the new points
    }
    bool ok = true;
    for (int i = 0; i < (int) from.size(); i++) {
        ok = ok && transformCoordinates(from[i], includeInBoundary, from_srs);
    }
    return ok;
}
/****************************************************************************/
