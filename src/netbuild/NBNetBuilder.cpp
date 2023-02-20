/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    NBNetBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Thimor Bohn
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    20 Nov 2001
///
// Instance responsible for building networks
/****************************************************************************/
#include <config.h>

#include <string>
#include <fstream>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/SysUtils.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include "NBAlgorithms.h"
#include "NBAlgorithms_Ramps.h"
#include "NBAlgorithms_Railway.h"
#include "NBHeightMapper.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBPTStop.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include "NBDistrict.h"
#include "NBRequest.h"
#include "NBTypeCont.h"
#include "NBNetBuilder.h"


// ===========================================================================
// method definitions
// ===========================================================================
NBNetBuilder::NBNetBuilder() :
    myEdgeCont(myTypeCont),
    myNetworkHaveCrossings(false) {
}


NBNetBuilder::~NBNetBuilder() {}


void
NBNetBuilder::applyOptions(OptionsCont& oc) {
    // apply options to type control
    myTypeCont.setEdgeTypeDefaults(oc.getInt("default.lanenumber"), oc.getFloat("default.lanewidth"), oc.getFloat("default.speed"), oc.getFloat("default.friction"),
                                   oc.getInt("default.priority"), parseVehicleClasses(oc.getString("default.allow"), oc.getString("default.disallow")),
                                   SUMOXMLDefinitions::LaneSpreadFunctions.get(oc.getString("default.spreadtype")));
    // apply options to edge control
    myEdgeCont.applyOptions(oc);
    // apply options to traffic light logics control
    myTLLCont.applyOptions(oc);
    NBEdge::setDefaultConnectionLength(oc.getFloat("default.connection-length"));
}


void
NBNetBuilder::compute(OptionsCont& oc, const std::set<std::string>& explicitTurnarounds, bool mayAddOrRemove) {
    // reset shapes and angles for stable netedit computation
    if (myNodeCont.resetNodeShapes()) {
        myEdgeCont.computeAngles();
    }

    GeoConvHelper& geoConvHelper = GeoConvHelper::getProcessing();

    const bool lefthand = oc.getBool("lefthand");
    if (lefthand) {
        mirrorX();
    }

    // MODIFYING THE SETS OF NODES AND EDGES
    // Removes edges that are connecting the same node
    long before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Removing self-loops"));
    myNodeCont.removeSelfLoops(myDistrictCont, myEdgeCont, myTLLCont);
    PROGRESS_TIME_MESSAGE(before);
    if (mayAddOrRemove && oc.exists("remove-edges.isolated") && oc.getBool("remove-edges.isolated")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Finding isolated roads"));
        myNodeCont.removeIsolatedRoads(myDistrictCont, myEdgeCont);
        PROGRESS_TIME_MESSAGE(before);
    }
    if (mayAddOrRemove && oc.exists("keep-edges.components") && oc.getInt("keep-edges.components") > 0) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Finding largest components"));
        const bool hasStops = oc.exists("ptstop-output") && oc.isSet("ptstop-output") && !myPTStopCont.getStops().empty();
        myNodeCont.removeComponents(myDistrictCont, myEdgeCont, oc.getInt("keep-edges.components"), hasStops);
        PROGRESS_TIME_MESSAGE(before);
    }
    if (mayAddOrRemove && oc.exists("keep-edges.postload") && oc.getBool("keep-edges.postload")) {
        if (oc.isSet("keep-edges.explicit") || oc.isSet("keep-edges.input-file")) {
            before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Removing unwished edges"));
            myEdgeCont.removeUnwishedEdges(myDistrictCont);
            PROGRESS_TIME_MESSAGE(before);
        }
    }
    // Processing pt stops and lines
    if (!myPTStopCont.getStops().empty()) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Processing public transport stops"));
        if (!(oc.exists("ptline-output") && oc.isSet("ptline-output"))
                && !oc.getBool("ptstop-output.no-bidi")) {
            myPTStopCont.localizePTStops(myEdgeCont);
        }
        myPTStopCont.assignEdgeForFloatingStops(myEdgeCont, 20);
        myPTStopCont.assignLanes(myEdgeCont);
        PROGRESS_TIME_MESSAGE(before);
    }
    if (mayAddOrRemove && oc.exists("keep-edges.components") && oc.getInt("keep-edges.components") > 0) {
        // post process rail components unless they have stops
        myNodeCont.removeRailComponents(myDistrictCont, myEdgeCont, myPTStopCont);
    }

    if (!myPTLineCont.getLines().empty()) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Revising public transport stops based on pt lines"));
        myPTLineCont.process(myEdgeCont, myPTStopCont);
        PROGRESS_TIME_MESSAGE(before);
    }

    if (oc.exists("ptline-clean-up") && oc.getBool("ptline-clean-up")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Cleaning up public transport stops that are not served by any line"));
        myPTStopCont.postprocess(myPTLineCont.getServedPTStops());
        PROGRESS_TIME_MESSAGE(before);
    } else {
        int numDeletedStops = myPTStopCont.cleanupDeleted(myEdgeCont);
        if (numDeletedStops > 0) {
            WRITE_WARNINGF(TL("Removed % pt stops because they could not be assigned to the network"), toString(numDeletedStops));
        }
    }

    if (!myPTStopCont.getStops().empty() && !oc.getBool("ptstop-output.no-bidi")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Align pt stop id signs with corresponding edge id signs"));
        myPTStopCont.alignIdSigns();
        PROGRESS_TIME_MESSAGE(before);
    }

    // analyze and fix railway topology
    int numAddedBidi = 0;
    if (oc.exists("railway.topology.all-bidi") && oc.getBool("railway.topology.all-bidi")) {
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false);
        numAddedBidi = NBRailwayTopologyAnalyzer::makeAllBidi(myEdgeCont);
    } else if (oc.exists("railway.topology.repair") && oc.getBool("railway.topology.repair")) {
        // correct railway angles for angle-based connectivity heuristic
        myEdgeCont.checkGeometries(0,
                                   oc.getFloat("geometry.min-radius"), false,
                                   oc.getBool("geometry.min-radius.fix.railways"), true);
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false);
        numAddedBidi = NBRailwayTopologyAnalyzer::repairTopology(myEdgeCont, myPTStopCont, myPTLineCont);
    }
    if (numAddedBidi > 0) {
        // update routes
        myPTLineCont.process(myEdgeCont, myPTStopCont, true);
    }
    if (oc.exists("railway.topology.direction-priority") && oc.getBool("railway.topology.direction-priority")) {
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false); // recompute after new edges were added
        NBRailwayTopologyAnalyzer::extendDirectionPriority(myEdgeCont, true);
    } else if (oc.exists("railway.topology.extend-priority") && oc.getBool("railway.topology.extend-priority")) {
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false); // recompute after new edges were added
        NBRailwayTopologyAnalyzer::extendDirectionPriority(myEdgeCont, false);
    }
    if (oc.exists("railway.topology.output") && oc.isSet("railway.topology.output")) {
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false); // recompute after new edges were added
        NBRailwayTopologyAnalyzer::analyzeTopology(myEdgeCont);
    }


    if (mayAddOrRemove && oc.exists("edges.join-tram-dist") && oc.getFloat("edges.join-tram-dist") >= 0) {
        // should come before joining junctions
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Joining tram edges"));
        int numJoinedTramEdges = myEdgeCont.joinTramEdges(myDistrictCont, myPTStopCont, myPTLineCont, oc.getFloat("edges.join-tram-dist"));
        PROGRESS_TIME_MESSAGE(before);
        if (numJoinedTramEdges > 0) {
            WRITE_MESSAGEF(TL(" Joined % tram edges into roads."), toString(numJoinedTramEdges));
        }
    }
    if (oc.getBool("junctions.join")
            || (oc.exists("ramps.guess") && oc.getBool("ramps.guess"))
            || oc.getBool("tls.guess.joining")
            || (oc.exists("tls.guess-signals") && oc.getBool("tls.guess-signals"))) {
        // preliminary geometry computations to determine the length of edges
        // This depends on turning directions and sorting of edge list
        // in case junctions are joined geometry computations have to be repeated
        // preliminary roundabout computations to avoid damaging roundabouts via junctions.join or ramps.guess
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false);
        NBNodesEdgesSorter::sortNodesEdges(myNodeCont);
        myEdgeCont.computeLaneShapes();
        myNodeCont.computeNodeShapes();
        myEdgeCont.computeEdgeShapes();
        if (oc.getBool("roundabouts.guess")) {
            myEdgeCont.guessRoundabouts();
        }
        const std::set<EdgeSet>& roundabouts = myEdgeCont.getRoundabouts();
        for (std::set<EdgeSet>::const_iterator it_round = roundabouts.begin();
                it_round != roundabouts.end(); ++it_round) {
            std::vector<std::string> nodeIDs;
            for (EdgeSet::const_iterator it_edge = it_round->begin(); it_edge != it_round->end(); ++it_edge) {
                nodeIDs.push_back((*it_edge)->getToNode()->getID());
            }
            myNodeCont.addJoinExclusion(nodeIDs);
        }
        NBNodeTypeComputer::validateRailCrossings(myNodeCont, myTLLCont);
    } else if (myEdgeCont.hasGuessedRoundabouts() && oc.getBool("roundabouts.guess")) {
        myEdgeCont.guessRoundabouts();
    }
    // join junctions (may create new "geometry"-nodes so it needs to come before removing these
    if (mayAddOrRemove && oc.exists("junctions.join-exclude") && oc.isSet("junctions.join-exclude")) {
        myNodeCont.addJoinExclusion(oc.getStringVector("junctions.join-exclude"));
    }
    int numJoined = myNodeCont.joinLoadedClusters(myDistrictCont, myEdgeCont, myTLLCont);
    if (mayAddOrRemove && oc.getBool("junctions.join")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Joining junction clusters"));
        numJoined += myNodeCont.joinJunctions(oc.getFloat("junctions.join-dist"), myDistrictCont, myEdgeCont, myTLLCont, myPTStopCont);
        PROGRESS_TIME_MESSAGE(before);
    }
    if (numJoined > 0) {
        WRITE_MESSAGEF(TL(" Joined % junction cluster(s)."), toString(numJoined));
    }
    if (mayAddOrRemove && oc.exists("junctions.join-same") && oc.getBool("junctions.join-same")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Joining junctions with identical coordinates"));
        int numJoined2 = myNodeCont.joinSameJunctions(myDistrictCont, myEdgeCont, myTLLCont);
        PROGRESS_TIME_MESSAGE(before);
        if (numJoined2 > 0) {
            WRITE_MESSAGEF(TL(" Joined % junctions."), toString(numJoined2));
        }
    }
    //
    if (mayAddOrRemove && oc.exists("join-lanes") && oc.getBool("join-lanes")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Joining lanes"));
        const int num = myEdgeCont.joinLanes(SVC_IGNORING) + myEdgeCont.joinLanes(SVC_PEDESTRIAN);
        PROGRESS_TIME_MESSAGE(before);
        WRITE_MESSAGEF(TL("   Joined lanes on % edges."), toString(num));
    }
    //
    if (mayAddOrRemove) {
        const bool removeGeometryNodes = oc.exists("geometry.remove") && oc.getBool("geometry.remove");
        before = PROGRESS_BEGIN_TIME_MESSAGE("Removing empty nodes" + std::string(removeGeometryNodes ? " and geometry nodes" : ""));
        // removeUnwishedNodes needs turnDirections. @todo: try to call this less often
        NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false);
        const int numRemoved = myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myTLLCont, myPTStopCont, myPTLineCont, myParkingCont, removeGeometryNodes);
        PROGRESS_TIME_MESSAGE(before);
        WRITE_MESSAGEF(TL("   % nodes removed."), toString(numRemoved));
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
        moveToOrigin(geoConvHelper, lefthand);
    }
    geoConvHelper.computeFinal(lefthand); // information needed for location element fixed at this point

    if (oc.exists("geometry.min-dist") && !oc.isDefault("geometry.min-dist")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Reducing geometries"));
        myEdgeCont.reduceGeometries(oc.getFloat("geometry.min-dist"));
        PROGRESS_TIME_MESSAGE(before);
    }
    // @note: removing geometry can create similar edges so joinSimilarEdges  must come afterwards
    // @note: likewise splitting can destroy similarities so joinSimilarEdges must come before
    if (mayAddOrRemove && oc.getBool("edges.join")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Joining similar edges"));
        const bool removeDuplicates = oc.exists("junctions.join-same") && oc.getBool("junctions.join-same");
        myNodeCont.joinSimilarEdges(myDistrictCont, myEdgeCont, myTLLCont, removeDuplicates);
        // now we may have new chances to remove geometry if wished
        if (oc.exists("geometry.remove") && oc.getBool("geometry.remove")) {
            myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myTLLCont, myPTStopCont, myPTLineCont, myParkingCont, true);
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    if (oc.getBool("opposites.guess")) {
        PROGRESS_BEGIN_MESSAGE(TL("guessing opposite direction edges"));
        myEdgeCont.guessOpposites();
        PROGRESS_DONE_MESSAGE();
    }
    //
    if (mayAddOrRemove && oc.exists("geometry.split") && oc.getBool("geometry.split")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Splitting geometry edges"));
        myEdgeCont.splitGeometry(myDistrictCont, myNodeCont);
        // newly split junctions might also be joinable
        PROGRESS_TIME_MESSAGE(before);
        if (oc.getBool("junctions.join-same")) {
            int numJoined3 = myNodeCont.joinSameJunctions(myDistrictCont, myEdgeCont, myTLLCont);
            if (numJoined3 > 0) {
                WRITE_MESSAGEF(TL(" Joined % junctions after splitting geometry."), toString(numJoined3));
            }
        }
    }
    // turning direction
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing turning directions"));
    NBTurningDirectionsComputer::computeTurnDirections(myNodeCont);
    PROGRESS_TIME_MESSAGE(before);
    // correct edge geometries to avoid overlap
    if (oc.exists("geometry.avoid-overlap") && oc.getBool("geometry.avoid-overlap")) {
        myNodeCont.avoidOverlap();
    }

    // GUESS TLS POSITIONS
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Assigning nodes to traffic lights"));
    if (oc.isSet("tls.set")) {
        std::vector<std::string> tlControlledNodes = oc.getStringVector("tls.set");
        TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(oc.getString("tls.default-type"));
        for (std::vector<std::string>::const_iterator i = tlControlledNodes.begin(); i != tlControlledNodes.end(); ++i) {
            NBNode* node = myNodeCont.retrieve(*i);
            if (node == nullptr) {
                WRITE_WARNING("Building a tl-logic for junction '" + *i + "' is not possible." + "\n The junction '" + *i + "' is not known.");
            } else {
                myNodeCont.setAsTLControlled(node, myTLLCont, type);
            }
        }
    }
    myNodeCont.guessTLs(oc, myTLLCont);
    PROGRESS_TIME_MESSAGE(before);

    // guess ramps (after guessing tls because ramps should not be build at traffic lights)
    const bool modifyRamps = mayAddOrRemove && (
                                 (oc.exists("ramps.guess") && oc.getBool("ramps.guess"))
                                 || (oc.exists("ramps.set") && oc.isSet("ramps.set")));
    if (modifyRamps || (oc.exists("ramps.guess-acceleration-lanes") && oc.getBool("ramps.guess-acceleration-lanes"))) {
        before = SysUtils::getCurrentMillis();
        if (modifyRamps) {
            PROGRESS_BEGIN_MESSAGE(TL("Guessing and setting on-/off-ramps"));
        }
        NBNodesEdgesSorter::sortNodesEdges(myNodeCont);
        NBRampsComputer::computeRamps(*this, oc, mayAddOrRemove);
        if (modifyRamps) {
            PROGRESS_TIME_MESSAGE(before);
        }
    }
    // guess bike lanes
    if (mayAddOrRemove && ((oc.getBool("bikelanes.guess") || oc.getBool("bikelanes.guess.from-permissions")))) {
        const int bikelanes = myEdgeCont.guessSpecialLanes(SVC_BICYCLE, oc.getFloat("default.bikelane-width"),
                              oc.getFloat("bikelanes.guess.min-speed"),
                              oc.getFloat("bikelanes.guess.max-speed"),
                              oc.getBool("bikelanes.guess.from-permissions"),
                              "bikelanes.guess.exclude",
                              myTLLCont);
        WRITE_MESSAGEF(TL("Guessed % bike lanes."), toString(bikelanes));
    }

    // guess sidewalks
    if (mayAddOrRemove && ((oc.getBool("sidewalks.guess") || oc.getBool("sidewalks.guess.from-permissions")))) {
        const int sidewalks = myEdgeCont.guessSpecialLanes(SVC_PEDESTRIAN, oc.getFloat("default.sidewalk-width"),
                              oc.getFloat("sidewalks.guess.min-speed"),
                              oc.getFloat("sidewalks.guess.max-speed"),
                              oc.getBool("sidewalks.guess.from-permissions"),
                              "sidewalks.guess.exclude",
                              myTLLCont);
        WRITE_MESSAGEF(TL("Guessed % sidewalks."), toString(sidewalks));
    }
    // check whether any not previously setable connections may be set now
    myEdgeCont.recheckPostProcessConnections();

    // remap ids if wished
    if (mayAddOrRemove) {
        int numChangedEdges = myEdgeCont.remapIDs(oc.getBool("numerical-ids"), oc.isSet("reserved-ids"), oc.getString("prefix"), myPTStopCont);
        int numChangedNodes = myNodeCont.remapIDs(oc.getBool("numerical-ids"), oc.isSet("reserved-ids"), oc.getString("prefix"), myTLLCont);
        if (numChangedEdges + numChangedNodes > 0) {
            WRITE_MESSAGEF(TL("Remapped % edge IDs and % node IDs."), toString(numChangedEdges), toString(numChangedNodes));
        }
    }

    //
    if (oc.exists("geometry.max-angle")) {
        myEdgeCont.checkGeometries(
            DEG2RAD(oc.getFloat("geometry.max-angle")),
            oc.getFloat("geometry.min-radius"),
            oc.getBool("geometry.min-radius.fix"),
            oc.getBool("geometry.min-radius.fix.railways"));
    }

    // GEOMETRY COMPUTATION
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Sorting nodes' edges"));
    NBNodesEdgesSorter::sortNodesEdges(myNodeCont);
    PROGRESS_TIME_MESSAGE(before);
    myEdgeCont.computeLaneShapes();
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing node shapes"));
    if (oc.exists("geometry.junction-mismatch-threshold")) {
        myNodeCont.computeNodeShapes(oc.getFloat("geometry.junction-mismatch-threshold"));
    } else {
        myNodeCont.computeNodeShapes();
    }
    PROGRESS_TIME_MESSAGE(before);
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing edge shapes"));
    myEdgeCont.computeEdgeShapes(oc.getBool("geometry.max-grade.fix") ? oc.getFloat("geometry.max-grade") / 100 : -1);
    PROGRESS_TIME_MESSAGE(before);
    // resort edges based on the node and edge shapes
    NBNodesEdgesSorter::sortNodesEdges(myNodeCont, true);
    NBTurningDirectionsComputer::computeTurnDirections(myNodeCont, false);

    // APPLY SPEED MODIFICATIONS
    if (oc.exists("speed.offset")) {
        const double speedOffset = oc.getFloat("speed.offset");
        const double speedFactor = oc.getFloat("speed.factor");
        const double speedMin = oc.getFloat("speed.minimum");
        if (speedOffset != 0 || speedFactor != 1 || speedMin > 0) {
            before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Applying speed modifications"));
            for (const auto& it : myEdgeCont) {
                NBEdge* const e = it.second;
                for (int i = 0; i < e->getNumLanes(); i++) {
                    e->setSpeed(i, MAX2(e->getLaneSpeed(i) * speedFactor + speedOffset, speedMin));
                }
            }
            PROGRESS_TIME_MESSAGE(before);
        }
    }

    // CONNECTIONS COMPUTATION
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing node types"));
    NBNodeTypeComputer::computeNodeTypes(myNodeCont, myTLLCont);
    PROGRESS_TIME_MESSAGE(before);
    //
    myNetworkHaveCrossings = oc.getBool("walkingareas");
    if (mayAddOrRemove && oc.getBool("crossings.guess")) {
        myNetworkHaveCrossings = true;
        int crossings = 0;
        for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
            crossings += (*i).second->guessCrossings();
        }
        WRITE_MESSAGEF(TL("Guessed % pedestrian crossings."), toString(crossings));
    }
    if (!myNetworkHaveCrossings) {
        bool haveValidCrossings = false;
        // recheck whether we had crossings in the input
        for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
            if (i->second->getCrossings().size() > 0) {
                myNetworkHaveCrossings = true;
                haveValidCrossings = true;
                break;
            } else if (i->second->getCrossingsIncludingInvalid().size() > 0) {
                myNetworkHaveCrossings = true;
            }
        }
        if (myNetworkHaveCrossings && !haveValidCrossings) {
            // initial crossings removed or invalidated, keep walkingareas
            oc.resetWritable();
            oc.set("walkingareas", "true");
        }
    }

    if (!mayAddOrRemove && myNetworkHaveCrossings) {
        // crossings added via netedit
        oc.resetWritable();
        oc.set("no-internal-links", "false");
    }

    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing priorities"));
    NBEdgePriorityComputer::computeEdgePriorities(myNodeCont);
    PROGRESS_TIME_MESSAGE(before);
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing approached edges"));
    myEdgeCont.computeEdge2Edges(oc.getBool("no-left-connections"));
    PROGRESS_TIME_MESSAGE(before);
    //
    if (oc.getBool("roundabouts.guess")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Guessing and setting roundabouts"));
        const int numGuessed = myEdgeCont.guessRoundabouts();
        if (numGuessed > 0) {
            WRITE_MESSAGEF(TL(" Guessed % roundabout(s)."), toString(numGuessed));
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    myEdgeCont.markRoundabouts();
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing approaching lanes"));
    myEdgeCont.computeLanes2Edges();
    PROGRESS_TIME_MESSAGE(before);
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Dividing of lanes on approached lanes"));
    myNodeCont.computeLanes2Lanes();
    myEdgeCont.sortOutgoingLanesConnections();
    PROGRESS_TIME_MESSAGE(before);
    //
    if (oc.getBool("fringe.guess")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Guessing Network fringe"));
        const int numGuessed = myNodeCont.guessFringe();
        if (numGuessed > 0) {
            WRITE_MESSAGEF(TL(" Guessed % fringe nodes."), toString(numGuessed));
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Processing turnarounds"));
    if (!oc.getBool("no-turnarounds")) {
        myEdgeCont.appendTurnarounds(
            oc.getBool("no-turnarounds.tls"),
            oc.getBool("no-turnarounds.fringe"),
            oc.getBool("no-turnarounds.except-deadend"),
            oc.getBool("no-turnarounds.except-turnlane"),
            oc.getBool("no-turnarounds.geometry"));
    } else {
        myEdgeCont.appendTurnarounds(explicitTurnarounds, oc.getBool("no-turnarounds.tls"));
    }
    if (oc.exists("railway.topology.repair.stop-turn") && oc.getBool("railway.topology.repair.stop-turn")
            && myPTStopCont.getStops().size() > 0) {
        // allow direction reversal at all bidi-edges with stops
        myEdgeCont.appendRailwayTurnarounds(myPTStopCont);
    }
    PROGRESS_TIME_MESSAGE(before);
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Rechecking of lane endings"));
    myEdgeCont.recheckLanes();
    PROGRESS_TIME_MESSAGE(before);

    if (myNetworkHaveCrossings && !oc.getBool("no-internal-links")) {
        for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
            i->second->buildCrossingsAndWalkingAreas();
        }
    } else {
        for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
            // needed by netedit if the last crossings was deleted from the network
            // and walkingareas have been invalidated since the last call to compute()
            i->second->discardWalkingareas();
        }
        if (oc.getBool("no-internal-links")) {
            for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
                i->second->discardAllCrossings(false);
            }
        }
    }
    // join traffic lights (after building connections)
    if (oc.getBool("tls.join")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Joining traffic light nodes"));
        myNodeCont.joinTLS(myTLLCont, oc.getFloat("tls.join-dist"));
        PROGRESS_TIME_MESSAGE(before);
    }

    // COMPUTING RIGHT-OF-WAY AND TRAFFIC LIGHT PROGRAMS
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing traffic light control information"));
    myTLLCont.setTLControllingInformation(myEdgeCont, myNodeCont);
    if (oc.exists("opendrive-files") && oc.isSet("opendrive-files")) {
        myTLLCont.setOpenDriveSignalParameters();
    }
    PROGRESS_TIME_MESSAGE(before);
    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing node logics"));
    myNodeCont.computeLogics(myEdgeCont);
    PROGRESS_TIME_MESSAGE(before);

    //
    before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Computing traffic light logics"));
    std::pair<int, int> numbers = myTLLCont.computeLogics(oc);
    PROGRESS_TIME_MESSAGE(before);
    std::string progCount = "";
    if (numbers.first != numbers.second) {
        progCount = "(" + toString(numbers.second) + " programs) ";
    }
    WRITE_MESSAGEF(TL(" % traffic light(s) %computed."), toString(numbers.first), progCount);

    for (std::map<std::string, NBEdge*>::const_iterator i = myEdgeCont.begin(); i != myEdgeCont.end(); ++i) {
        (*i).second->sortOutgoingConnectionsByIndex();
    }
    // FINISHING INNER EDGES
    std::set<NBTrafficLightDefinition*> largeNodeTLS;
    if (!oc.getBool("no-internal-links")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Building inner edges"));
        // walking areas shall only be built if crossings are wished as well
        for (const auto& item : myNodeCont) {
            if (item.second->buildInnerEdges() > NBTrafficLightDefinition::MIN_YELLOW_SECONDS) {
                const std::set<NBTrafficLightDefinition*>& tlDefs = item.second->getControllingTLS();
                largeNodeTLS.insert(tlDefs.begin(), tlDefs.end());
            }
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    // PATCH NODE SHAPES
    if (oc.getFloat("junctions.scurve-stretch") > 0) {
        // @note: nodes have collected correction hints in buildInnerEdges()
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("stretching junctions to smooth geometries"));
        myEdgeCont.computeLaneShapes();
        myNodeCont.computeNodeShapes();
        myEdgeCont.computeEdgeShapes(oc.getBool("geometry.max-grade.fix") ? oc.getFloat("geometry.max-grade") / 100 : -1);
        for (const auto& item : myNodeCont) {
            item.second->buildInnerEdges();
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    if (myEdgeCont.getNumEdgeSplits() > 0 && !oc.getBool("no-internal-links")) {
        // edges with custom lengths were split, this has to take into account
        // internal edge lengts (after geometry computation)
        myEdgeCont.fixSplitCustomLength();
    }
    // recheck phases for large junctions
    for (NBTrafficLightDefinition* def : largeNodeTLS) {
        myTLLCont.computeSingleLogic(oc, def);
    }
    // compute lane-to-lane node logics (require traffic lights and inner edges to be done)
    myNodeCont.computeLogics2(myEdgeCont, oc);

    // remove guessed traffic lights at junctions without conflicts (requires computeLogics2)
    myNodeCont.recheckGuessedTLS(myTLLCont);

    // compute keepClear status (requires computeLogics2)
    myNodeCont.computeKeepClear();

    //
    if (oc.isSet("street-sign-output")) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Generating street signs"));
        myEdgeCont.generateStreetSigns();
        PROGRESS_TIME_MESSAGE(before);
    }


    if (lefthand != oc.getBool("flip-y-axis")) {
        mirrorX();
    };

    if (oc.exists("geometry.check-overlap")  && oc.getFloat("geometry.check-overlap") > 0) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Checking overlapping edges"));
        myEdgeCont.checkOverlap(oc.getFloat("geometry.check-overlap"), oc.getFloat("geometry.check-overlap.vertical-threshold"));
        PROGRESS_TIME_MESSAGE(before);
    }
    if (geoConvHelper.getConvBoundary().getZRange() > 0 && oc.getFloat("geometry.max-grade") > 0) {
        before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Checking edge grade"));
        // user input is in %
        myEdgeCont.checkGrade(oc.getFloat("geometry.max-grade") / 100);
        PROGRESS_TIME_MESSAGE(before);
    }

    // find accesses for pt rail stops and add bidi-stops
    if (!myPTStopCont.getStops().empty()) {
        // re-adapt stop lanes after adding special lanes and cutting edge shapes at junction
        myPTStopCont.assignLanes(myEdgeCont);
        before = SysUtils::getCurrentMillis();
        int numBidiStops = 0;
        if (!oc.getBool("ptstop-output.no-bidi")) {
            numBidiStops = myPTStopCont.generateBidiStops(myEdgeCont);
        }
        PROGRESS_BEGIN_MESSAGE(TL("Find accesses for pt rail stops"));
        double maxRadius = oc.getFloat("railway.access-distance");
        double accessFactor = oc.getFloat("railway.access-factor");
        int maxCount = oc.getInt("railway.max-accesses");
        myPTStopCont.findAccessEdgesForRailStops(myEdgeCont, maxRadius, maxCount, accessFactor);
        PROGRESS_TIME_MESSAGE(before);
        if (numBidiStops > 0) {
            myPTLineCont.fixBidiStops(myEdgeCont);
        }
    }
    myPTLineCont.removeInvalidEdges(myEdgeCont);
    // ensure that all turning lanes have sufficient permissions
    myPTLineCont.fixPermissions();

    if (oc.exists("ignore-change-restrictions") && !oc.isDefault("ignore-change-restrictions")) {
        SVCPermissions ignoring = parseVehicleClasses(oc.getStringVector("ignore-change-restrictions"));
        myEdgeCont.updateAllChangeRestrictions(ignoring);
    }

    NBRequest::reportWarnings();
    // report on very large networks
    if (MAX2(geoConvHelper.getConvBoundary().xmax(), geoConvHelper.getConvBoundary().ymax()) > 1000000 ||
            MIN2(geoConvHelper.getConvBoundary().xmin(), geoConvHelper.getConvBoundary().ymin()) < -1000000) {
        WRITE_WARNING(TL("Network contains very large coordinates and will probably flicker in the GUI. Check for outlying nodes and make sure the network is shifted to the coordinate origin"));
    }
}


void
NBNetBuilder::moveToOrigin(GeoConvHelper& geoConvHelper, bool lefthand) {
    long before = PROGRESS_BEGIN_TIME_MESSAGE(TL("Moving network to origin"));
    Boundary boundary = geoConvHelper.getConvBoundary();
    const double x = -boundary.xmin();
    const double y = -(lefthand ? boundary.ymax() : boundary.ymin());
    //if (lefthand) {
    //    y = boundary.ymax();
    //}
    for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
        (*i).second->reshiftPosition(x, y);
    }
    for (std::map<std::string, NBEdge*>::const_iterator i = myEdgeCont.begin(); i != myEdgeCont.end(); ++i) {
        (*i).second->reshiftPosition(x, y);
    }
    for (std::map<std::string, NBDistrict*>::const_iterator i = myDistrictCont.begin(); i != myDistrictCont.end(); ++i) {
        (*i).second->reshiftPosition(x, y);
    }
    for (const auto& stopIt : myPTStopCont.getStops()) {
        stopIt.second->reshiftPosition(x, y);
    }
    geoConvHelper.moveConvertedBy(x, y);
    PROGRESS_TIME_MESSAGE(before);
}


void
NBNetBuilder::mirrorX() {
    // mirror the network along the X-axis
    for (std::map<std::string, NBNode*>::const_iterator i = myNodeCont.begin(); i != myNodeCont.end(); ++i) {
        (*i).second->mirrorX();
    }
    for (std::map<std::string, NBEdge*>::const_iterator i = myEdgeCont.begin(); i != myEdgeCont.end(); ++i) {
        (*i).second->mirrorX();
    }
    for (std::map<std::string, NBDistrict*>::const_iterator i = myDistrictCont.begin(); i != myDistrictCont.end(); ++i) {
        (*i).second->mirrorX();
    }
    for (const auto& stopIt : myPTStopCont.getStops()) {
        stopIt.second->mirrorX();
    }
}


bool
NBNetBuilder::transformCoordinate(Position& from, bool includeInBoundary, GeoConvHelper* from_srs) {
    Position orig(from);
    bool ok = true;
    if (GeoConvHelper::getNumLoaded() > 1
            && GeoConvHelper::getLoaded().usingGeoProjection()
            && from_srs != nullptr
            && from_srs->usingGeoProjection()
            && *from_srs != GeoConvHelper::getLoaded()) {
        from_srs->cartesian2geo(from);
        ok &= GeoConvHelper::getLoaded().x2cartesian(from, false);
    }
    if (from_srs == nullptr || !GeoConvHelper::getProcessing().usingGeoProjection()) {
        // if getProcessing is not a geo-projection, assume it a cartesian transformation (i.e. shift)
        ok &= GeoConvHelper::getProcessing().x2cartesian(from, includeInBoundary);

        if (from_srs == nullptr && GeoConvHelper::getProcessing().usingGeoProjection()
                && GeoConvHelper::getNumLoaded() > 0
                && GeoConvHelper::getLoaded().usingGeoProjection()) {
            // apply geo patch to loaded geo-network (offset must match)
            from = from + GeoConvHelper::getLoaded().getOffset();
        }
    }
    if (ok) {
        const NBHeightMapper& hm = NBHeightMapper::get();
        if (hm.ready()) {
            if (from_srs != nullptr && from_srs->usingGeoProjection()) {
                from_srs->cartesian2geo(orig);
            }
            from.setz(hm.getZ(orig));
        }
    }
    return ok;
}


bool
NBNetBuilder::transformCoordinates(PositionVector& from, bool includeInBoundary, GeoConvHelper* from_srs) {
    const double maxLength = OptionsCont::getOptions().getFloat("geometry.max-segment-length");
    if (maxLength > 0 && from.size() > 1) {
        // transformation to cartesian coordinates must happen before we can check segment length
        PositionVector copy = from;
        for (int i = 0; i < (int) from.size(); i++) {
            transformCoordinate(copy[i], false);
        }
        addGeometrySegments(from, copy, maxLength);
    }
    bool ok = true;
    for (int i = 0; i < (int) from.size(); i++) {
        ok = ok && transformCoordinate(from[i], includeInBoundary, from_srs);
    }
    return ok;
}

int
NBNetBuilder::addGeometrySegments(PositionVector& from, const PositionVector& cartesian, const double maxLength) {
    // check lengths and insert new points where needed (in the original
    // coordinate system)
    int inserted = 0;
    for (int i = 0; i < (int)cartesian.size() - 1; i++) {
        Position start = from[i + inserted];
        Position end = from[i + inserted + 1];
        double length = cartesian[i].distanceTo(cartesian[i + 1]);
        const Position step = (end - start) * (maxLength / length);
        int steps = 0;
        while (length > maxLength) {
            length -= maxLength;
            steps++;
            from.insert(from.begin() + i + inserted + 1, start + (step * steps));
            inserted++;
        }
    }
    return inserted;
}


bool
NBNetBuilder::runningNetedit() {
    // see GNELoadThread::fillOptions
    return OptionsCont::getOptions().exists("new");
}


/****************************************************************************/
