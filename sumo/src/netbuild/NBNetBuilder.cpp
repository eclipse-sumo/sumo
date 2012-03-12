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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBNetBuilder::NBNetBuilder()
    : myEdgeCont(myTypeCont) {}


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
                      const std::set<std::string> &explicitTurnarounds,
                      bool removeUnwishedNodes) {
    GeoConvHelper& geoConvHelper = GeoConvHelper::getProcessing();


    // MODIFYING THE SETS OF NODES AND EDGES
    // join junctions

    if (oc.exists("junctions.join-exclude") && oc.isSet("junctions.join-exclude")) {
        myNodeCont.addJoinExclusion(oc.getStringVector("junctions.join-exclude"));
    }
    unsigned int numJoined = myNodeCont.joinLoadedClusters(myDistrictCont, myEdgeCont, myTLLCont);
    if (oc.getBool("junctions.join")) {
        PROGRESS_BEGIN_MESSAGE("Joining junction clusters");
        numJoined += myNodeCont.joinJunctions(oc.getFloat("junctions.join-dist"), myDistrictCont, myEdgeCont, myTLLCont);
        PROGRESS_DONE_MESSAGE();
    }
    if (numJoined > 0) {
        // bit of a misnomer since we're already done
        WRITE_MESSAGE(" Joined " + toString(numJoined) + " junction cluster(s).");
    }

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
    //
    if (removeUnwishedNodes) {
        unsigned int no = 0;
        const bool removeGeometryNodes = oc.exists("geometry.remove") && oc.getBool("geometry.remove");
        PROGRESS_BEGIN_MESSAGE("Removing empty nodes" + std::string(removeGeometryNodes ? " and geometry nodes" : ""));
        no = myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myJoinedEdges, myTLLCont, removeGeometryNodes);
        PROGRESS_DONE_MESSAGE();
        WRITE_MESSAGE("   " + toString(no) + " nodes removed.");
    }
    // @note: removing geometry can create similar edges so "Joining" must come afterwards
    // @note: likewise splitting can destroy similarities so "Joining" must come before 
    PROGRESS_BEGIN_MESSAGE("Joining similar edges");
    myJoinedEdges.init(myEdgeCont);
    myNodeCont.joinSimilarEdges(myDistrictCont, myEdgeCont, myTLLCont);
    PROGRESS_DONE_MESSAGE();
    //
    if (oc.exists("geometry.split") && oc.getBool("geometry.split")) {
        PROGRESS_BEGIN_MESSAGE("Splitting geometry edges");
        myEdgeCont.splitGeometry(myNodeCont);
        PROGRESS_DONE_MESSAGE();
    }
    // guess ramps
    if ((oc.exists("ramps.guess") && oc.getBool("ramps.guess")) || (oc.exists("ramps.set") && oc.isSet("ramps.set"))) {
        PROGRESS_BEGIN_MESSAGE("Guessing and setting on-/off-ramps");
        myNodeCont.guessRamps(oc, myEdgeCont, myDistrictCont);
        PROGRESS_DONE_MESSAGE();
    }


    // MOVE TO ORIGIN
    if (!oc.getBool("offset.disable-normalization") && oc.isDefault("offset.x") && oc.isDefault("offset.y")) {
        PROGRESS_BEGIN_MESSAGE("Moving network to origin");
        const SUMOReal x = -geoConvHelper.getConvBoundary().xmin();
        const SUMOReal y = -geoConvHelper.getConvBoundary().ymin();
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
    geoConvHelper.computeFinal(); // information needed for location element fixed at this point

    // @todo Why?
    myEdgeCont.recomputeLaneShapes();

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

    // GUESS TLS POSITIONS
    PROGRESS_BEGIN_MESSAGE("Assigning nodes to traffic lights");
    if (oc.isSet("tls.set")) {
        std::vector<std::string> tlControlledNodes = oc.getStringVector("tls.set");
        for (std::vector<std::string>::const_iterator i = tlControlledNodes.begin(); i != tlControlledNodes.end(); ++i) {
            NBNode* node = myNodeCont.retrieve(*i);
            if (node == 0) {
                WRITE_WARNING("Building a tl-logic for node '" + *i + "' is not possible." + "\n The node '" + *i + "' is not known.");
            } else {
                myNodeCont.setAsTLControlled(node, myTLLCont);
            }
        }
    }
    myNodeCont.guessTLs(oc, myTLLCont);
    PROGRESS_DONE_MESSAGE();
    //
    if (oc.getBool("tls.join")) {
        PROGRESS_BEGIN_MESSAGE("Joining traffic light nodes");
        myNodeCont.joinTLS(myTLLCont);
        PROGRESS_DONE_MESSAGE();
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
    if (oc.getBool("roundabouts.guess")) {
        PROGRESS_BEGIN_MESSAGE("Guessing and setting roundabouts");
        myEdgeCont.guessRoundabouts(myRoundabouts);
        PROGRESS_DONE_MESSAGE();
    }
    //
    PROGRESS_BEGIN_MESSAGE("Computing approached edges");
    myEdgeCont.computeEdge2Edges(oc.getBool("no-left-connections"));
    PROGRESS_DONE_MESSAGE();
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
    myNodeCont.computeNodeShapes(oc.getBool("lefthand"));
    PROGRESS_DONE_MESSAGE();
    //
    PROGRESS_BEGIN_MESSAGE("Computing edge shapes");
    myEdgeCont.computeEdgeShapes();
    PROGRESS_DONE_MESSAGE();


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
}


/****************************************************************************/
