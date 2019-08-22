/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIImporter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in SUMO format
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBAlgorithms_Ramps.h>
#include <netbuild/NBNetBuilder.h>
#include "NILoader.h"
#include "NIXMLTypesHandler.h"
#include "NIImporter_SUMO.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_SUMO::loadNetwork(OptionsCont& oc, NBNetBuilder& nb) {
    NIImporter_SUMO importer(nb);
    importer._loadNetwork(oc);
}


// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_SUMO::NIImporter_SUMO(NBNetBuilder& nb)
    : SUMOSAXHandler("sumo-network"),
      myNetBuilder(nb),
      myNodeCont(nb.getNodeCont()),
      myTLLCont(nb.getTLLogicCont()),
      myTypesHandler(nb.getTypeCont()),
      myCurrentEdge(nullptr),
      myCurrentLane(nullptr),
      myCurrentTL(nullptr),
      myLocation(nullptr),
      myNetworkVersion(0),
      myHaveSeenInternalEdge(false),
      myAmLefthand(false),
      myCornerDetail(0),
      myLinkDetail(-1),
      myRectLaneCut(false),
      myWalkingAreas(false),
      myLimitTurnSpeed(-1),
      myCheckLaneFoesAll(false),
      myCheckLaneFoesRoundabout(true) {
}


NIImporter_SUMO::~NIImporter_SUMO() {
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        for (std::vector<LaneAttrs*>::const_iterator j = ed->lanes.begin(); j != ed->lanes.end(); ++j) {
            delete *j;
        }
        delete ed;
    }
    delete myLocation;

}


void
NIImporter_SUMO::_loadNetwork(OptionsCont& oc) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("sumo-net-file")) {
        return;
    }
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("sumo-net-file");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::isReadable(*file)) {
            WRITE_ERROR("Could not open sumo-net-file '" + *file + "'.");
            return;
        }
        setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing sumo-net from '" + *file + "'");
        XMLSubSys::runParser(*this, *file, true);
        PROGRESS_DONE_MESSAGE();
    }
    // build edges
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        // skip internal edges
        if (ed->func == EDGEFUNC_INTERNAL || ed->func == EDGEFUNC_CROSSING || ed->func == EDGEFUNC_WALKINGAREA) {
            continue;
        }
        // get and check the nodes
        NBNode* from = myNodeCont.retrieve(ed->fromNode);
        NBNode* to = myNodeCont.retrieve(ed->toNode);
        if (from == nullptr) {
            WRITE_ERROR("Edge's '" + ed->id + "' from-node '" + ed->fromNode + "' is not known.");
            continue;
        }
        if (to == nullptr) {
            WRITE_ERROR("Edge's '" + ed->id + "' to-node '" + ed->toNode + "' is not known.");
            continue;
        }
        if (from == to) {
            WRITE_ERROR("Edge's '" + ed->id + "' from-node and to-node '" + ed->toNode + "' are identical.");
            continue;
        }
        // edge shape
        PositionVector geom;
        if (ed->shape.size() > 0) {
            geom = ed->shape;
        } else {
            // either the edge has default shape consisting only of the two node
            // positions or we have a legacy network
            geom = reconstructEdgeShape(ed, from->getPosition(), to->getPosition());
        }
        // build and insert the edge
        NBEdge* e = new NBEdge(ed->id, from, to,
                               ed->type, ed->maxSpeed,
                               (int) ed->lanes.size(),
                               ed->priority, NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET,
                               geom, ed->streetName, "", ed->lsf, true); // always use tryIgnoreNodePositions to keep original shape
        e->setLoadedLength(ed->length);
        e->updateParameter(ed->getParametersMap());
        e->setDistance(ed->distance);
        if (!myNetBuilder.getEdgeCont().insert(e)) {
            WRITE_ERROR("Could not insert edge '" + ed->id + "'.");
            delete e;
            continue;
        }
        ed->builtEdge = myNetBuilder.getEdgeCont().retrieve(ed->id);
        if (ed->builtEdge != nullptr) {
            ed->builtEdge->setStopOffsets(-1, ed->stopOffsets);
        }
    }
    // assign further lane attributes (edges are built)
    EdgeVector toRemove;
    const bool dismissVclasses = oc.getBool("dismiss-vclasses");
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        NBEdge* nbe = ed->builtEdge;
        if (nbe == nullptr) { // inner edge or removed by explicit list, vclass, ...
            continue;
        }
        const SumoXMLNodeType toType = nbe->getToNode()->getType();
        for (int fromLaneIndex = 0; fromLaneIndex < (int) ed->lanes.size(); ++fromLaneIndex) {
            LaneAttrs* lane = ed->lanes[fromLaneIndex];
            // connections
            const std::vector<Connection>& connections = lane->connections;
            for (const Connection& c : connections) {
                if (myEdges.count(c.toEdgeID) == 0) {
                    WRITE_ERROR("Unknown edge '" + c.toEdgeID + "' given in connection.");
                    continue;
                }
                NBEdge* toEdge = myEdges[c.toEdgeID]->builtEdge;
                if (toEdge == nullptr) { // removed by explicit list, vclass, ...
                    continue;
                }
                if (nbe->hasConnectionTo(toEdge, c.toLaneIdx)) {
                    WRITE_WARNING("Target lane '" + toEdge->getLaneID(c.toLaneIdx) + "' has multiple connections from '" + nbe->getID() + "'.");
                }
                // patch attribute uncontrolled for legacy networks where it is not set explicitly
                bool uncontrolled = c.uncontrolled;

                if ((NBNode::isTrafficLight(toType) || toType == NODETYPE_RAIL_SIGNAL)
                        && c.tlLinkIndex == NBConnection::InvalidTlIndex) {
                    uncontrolled = true;
                }
                nbe->addLane2LaneConnection(
                    fromLaneIndex, toEdge, c.toLaneIdx, NBEdge::L2L_VALIDATED,
                    true, c.mayDefinitelyPass, c.keepClear, c.contPos, c.visibility, c.speed, c.customShape, uncontrolled);
                if (c.getParametersMap().size() > 0) {
                    nbe->getConnectionRef(fromLaneIndex, toEdge, c.toLaneIdx).updateParameter(c.getParametersMap());
                }
                // maybe we have a tls-controlled connection
                if (c.tlID != "" && myRailSignals.count(c.tlID) == 0) {
                    const std::map<std::string, NBTrafficLightDefinition*>& programs = myTLLCont.getPrograms(c.tlID);
                    if (programs.size() > 0) {
                        std::map<std::string, NBTrafficLightDefinition*>::const_iterator it;
                        for (it = programs.begin(); it != programs.end(); it++) {
                            NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it->second);
                            if (tlDef) {
                                tlDef->addConnection(nbe, toEdge, fromLaneIndex, c.toLaneIdx, c.tlLinkIndex, false);
                            } else {
                                throw ProcessError("Corrupt traffic light definition '" + c.tlID + "' (program '" + it->first + "')");
                            }
                        }
                    } else {
                        WRITE_ERROR("The traffic light '" + c.tlID + "' is not known.");
                    }
                }
            }
            // allow/disallow XXX preferred
            if (!dismissVclasses) {
                nbe->setPermissions(parseVehicleClasses(lane->allow, lane->disallow, myNetworkVersion), fromLaneIndex);
            }
            // width, offset
            nbe->setLaneWidth(fromLaneIndex, lane->width);
            nbe->setEndOffset(fromLaneIndex, lane->endOffset);
            nbe->setSpeed(fromLaneIndex, lane->maxSpeed);
            nbe->setAcceleration(fromLaneIndex, lane->accelRamp);
            nbe->getLaneStruct(fromLaneIndex).oppositeID = lane->oppositeID;
            nbe->getLaneStruct(fromLaneIndex).type = lane->type;
            nbe->getLaneStruct(fromLaneIndex).updateParameter(lane->getParametersMap());
            if (lane->customShape) {
                nbe->setLaneShape(fromLaneIndex, lane->shape);
            }
            // stop offset for lane
            bool stopOffsetSet = false;
            if (lane->stopOffsets.size() != 0 || nbe->getStopOffsets().size() == 0) {
                // apply lane-specific stopOffset (might be none as well)
                stopOffsetSet = nbe->setStopOffsets(fromLaneIndex, lane->stopOffsets);
            }
            if (!stopOffsetSet) {
                // apply default stop offset to lane
                nbe->setStopOffsets(fromLaneIndex, nbe->getStopOffsets());
            }
        }
        nbe->declareConnectionsAsLoaded();
        if (!nbe->hasLaneSpecificWidth() && nbe->getLanes()[0].width != NBEdge::UNSPECIFIED_WIDTH) {
            nbe->setLaneWidth(-1, nbe->getLaneWidth(0));
        }
        if (!nbe->hasLaneSpecificEndOffset() && nbe->getEndOffset(0) != NBEdge::UNSPECIFIED_OFFSET) {
            nbe->setEndOffset(-1, nbe->getEndOffset(0));
        }
        if (!nbe->hasLaneSpecificStopOffsets() && nbe->getStopOffsets().size() != 0) {
            nbe->setStopOffsets(-1, nbe->getStopOffsets());
        }
        // check again after permissions are set
        if (myNetBuilder.getEdgeCont().ignoreFilterMatch(nbe)) {
            myNetBuilder.getEdgeCont().ignore(nbe->getID());
            toRemove.push_back(nbe);
        }
    }
    for (EdgeVector::iterator i = toRemove.begin(); i != toRemove.end(); ++i) {
        myNetBuilder.getEdgeCont().erase(myNetBuilder.getDistrictCont(), *i);
    }
    // insert loaded prohibitions
    for (std::vector<Prohibition>::const_iterator it = myProhibitions.begin(); it != myProhibitions.end(); it++) {
        NBEdge* prohibitedFrom = myEdges[it->prohibitedFrom]->builtEdge;
        NBEdge* prohibitedTo = myEdges[it->prohibitedTo]->builtEdge;
        NBEdge* prohibitorFrom = myEdges[it->prohibitorFrom]->builtEdge;
        NBEdge* prohibitorTo = myEdges[it->prohibitorTo]->builtEdge;
        if (prohibitedFrom == nullptr) {
            WRITE_WARNING("Edge '" + it->prohibitedFrom + "' in prohibition was not built");
        } else if (prohibitedTo == nullptr) {
            WRITE_WARNING("Edge '" + it->prohibitedTo + "' in prohibition was not built");
        } else if (prohibitorFrom == nullptr) {
            WRITE_WARNING("Edge '" + it->prohibitorFrom + "' in prohibition was not built");
        } else if (prohibitorTo == nullptr) {
            WRITE_WARNING("Edge '" + it->prohibitorTo + "' in prohibition was not built");
        } else {
            NBNode* n = prohibitedFrom->getToNode();
            n->addSortedLinkFoes(
                NBConnection(prohibitorFrom, prohibitorTo),
                NBConnection(prohibitedFrom, prohibitedTo));
        }
    }
    if (!myHaveSeenInternalEdge && oc.isDefault("no-internal-links")) {
        oc.set("no-internal-links", "true");
    }
    if (oc.isDefault("lefthand")) {
        oc.set("lefthand", toString(myAmLefthand));
    }
    if (oc.isDefault("junctions.corner-detail")) {
        oc.set("junctions.corner-detail", toString(myCornerDetail));
    }
    if (oc.isDefault("junctions.internal-link-detail") && myLinkDetail > 0) {
        oc.set("junctions.internal-link-detail", toString(myLinkDetail));
    }
    if (oc.isDefault("rectangular-lane-cut")) {
        oc.set("rectangular-lane-cut", toString(myRectLaneCut));
    }
    if (oc.isDefault("walkingareas")) {
        oc.set("walkingareas", toString(myWalkingAreas));
    }
    if (oc.isDefault("junctions.limit-turn-speed")) {
        oc.set("junctions.limit-turn-speed", toString(myLimitTurnSpeed));
    }
    if (oc.isDefault("check-lane-foes.all") && oc.getBool("check-lane-foes.all") != myCheckLaneFoesAll) {
        oc.set("check-lane-foes.all", toString(myCheckLaneFoesAll));
    }
    if (oc.isDefault("check-lane-foes.roundabout") && oc.getBool("check-lane-foes.roundabout") != myCheckLaneFoesRoundabout) {
        oc.set("check-lane-foes.roundabout", toString(myCheckLaneFoesRoundabout));
    }
    if (!deprecatedVehicleClassesSeen.empty()) {
        WRITE_WARNING("Deprecated vehicle class(es) '" + toString(deprecatedVehicleClassesSeen) + "' in input network.");
        deprecatedVehicleClassesSeen.clear();
    }
    if (!oc.getBool("no-internal-links")) {
        // add loaded crossings
        for (std::map<std::string, std::vector<Crossing> >::const_iterator it = myPedestrianCrossings.begin(); it != myPedestrianCrossings.end(); ++it) {
            NBNode* node = myNodeCont.retrieve((*it).first);
            for (std::vector<Crossing>::const_iterator it_c = (*it).second.begin(); it_c != (*it).second.end(); ++it_c) {
                const Crossing& crossing = (*it_c);
                EdgeVector edges;
                for (std::vector<std::string>::const_iterator it_e = crossing.crossingEdges.begin(); it_e != crossing.crossingEdges.end(); ++it_e) {
                    NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(*it_e);
                    // edge might have been removed due to options
                    if (edge != nullptr) {
                        edges.push_back(edge);
                    }
                }
                if (edges.size() > 0) {
                    node->addCrossing(edges, crossing.width, crossing.priority, crossing.customTLIndex, crossing.customTLIndex2, crossing.customShape, true);
                }
            }
        }
        // add walking area custom shapes
        for (auto item : myWACustomShapes) {
            std::string nodeID = SUMOXMLDefinitions::getJunctionIDFromInternalEdge(item.first);
            NBNode* node = myNodeCont.retrieve(nodeID);
            std::vector<std::string> edgeIDs;
            if (item.second.fromEdges.size() + item.second.toEdges.size() == 0) {
                // must be a split crossing
                assert(item.second.fromCrossed.size() > 0);
                assert(item.second.toCrossed.size() > 0);
                edgeIDs = item.second.fromCrossed;
                edgeIDs.insert(edgeIDs.end(), item.second.toCrossed.begin(), item.second.toCrossed.end());
            } else if (item.second.fromEdges.size() > 0) {
                edgeIDs = item.second.fromEdges;
            } else {
                edgeIDs = item.second.toEdges;
            }
            EdgeVector edges;
            for (std::string edgeID : edgeIDs) {
                NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(edgeID);
                // edge might have been removed due to options
                if (edge != nullptr) {
                    edges.push_back(edge);
                }
            }
            if (edges.size() > 0) {
                node->addWalkingAreaShape(edges, item.second.shape);
            }
        }
    }
    // add roundabouts
    for (std::vector<std::vector<std::string> >::const_iterator it = myRoundabouts.begin(); it != myRoundabouts.end(); ++it) {
        EdgeSet roundabout;
        for (std::vector<std::string>::const_iterator it_r = it->begin(); it_r != it->end(); ++it_r) {
            NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(*it_r);
            if (edge == nullptr) {
                if (!myNetBuilder.getEdgeCont().wasIgnored(*it_r)) {
                    WRITE_ERROR("Unknown edge '" + (*it_r) + "' in roundabout");
                }
            } else {
                roundabout.insert(edge);
            }
        }
        myNetBuilder.getEdgeCont().addRoundabout(roundabout);
    }
}



void
NIImporter_SUMO::myStartElement(int element,
                                const SUMOSAXAttributes& attrs) {
    /* our goal is to reproduce the input net faithfully
     * there are different types of objects in the netfile:
     * 1) those which must be loaded into NBNetBuilder-Containers for processing
     * 2) those which can be ignored because they are recomputed based on group 1
     * 3) those which are of no concern to NBNetBuilder but should be exposed to
     *      NETEDIT. We will probably have to patch NBNetBuilder to contain them
     *      and hand them over to NETEDIT
     *    alternative idea: those shouldn't really be contained within the
     *    network but rather in separate files. teach NETEDIT how to open those
     *    (POI?)
     * 4) those which are of concern neither to NBNetBuilder nor NETEDIT and
     *    must be copied over - need to patch NBNetBuilder for this.
     *    copy unknown by default
     */
    switch (element) {
        case SUMO_TAG_NET: {
            bool ok;
            myNetworkVersion = attrs.getOpt<double>(SUMO_ATTR_VERSION, nullptr, ok, 0);
            myAmLefthand = attrs.getOpt<bool>(SUMO_ATTR_LEFTHAND, nullptr, ok, false);
            myCornerDetail = attrs.getOpt<int>(SUMO_ATTR_CORNERDETAIL, nullptr, ok, 0);
            myLinkDetail = attrs.getOpt<int>(SUMO_ATTR_LINKDETAIL, nullptr, ok, -1);
            myRectLaneCut = attrs.getOpt<bool>(SUMO_ATTR_RECTANGULAR_LANE_CUT, nullptr, ok, false);
            myWalkingAreas = attrs.getOpt<bool>(SUMO_ATTR_WALKINGAREAS, nullptr, ok, false);
            myLimitTurnSpeed = attrs.getOpt<double>(SUMO_ATTR_LIMIT_TURN_SPEED, nullptr, ok, -1);
            myWalkingAreas = attrs.getOpt<bool>(SUMO_ATTR_WALKINGAREAS, nullptr, ok, false);
            myCheckLaneFoesAll = attrs.getOpt<bool>(SUMO_ATTR_CHECKLANEFOES_ALL, nullptr, ok, false);
            myCheckLaneFoesRoundabout = attrs.getOpt<bool>(SUMO_ATTR_CHECKLANEFOES_ALL, nullptr, ok, true);
            break;
        }
        case SUMO_TAG_EDGE:
            addEdge(attrs);
            break;
        case SUMO_TAG_LANE:
            addLane(attrs);
            break;
        case SUMO_TAG_STOPOFFSET: {
            bool ok = true;
            addStopOffsets(attrs, ok);
        }
        break;
        case SUMO_TAG_NEIGH:
            myCurrentLane->oppositeID = attrs.getString(SUMO_ATTR_LANE);
            break;
        case SUMO_TAG_JUNCTION:
            addJunction(attrs);
            break;
        case SUMO_TAG_REQUEST:
            addRequest(attrs);
            break;
        case SUMO_TAG_CONNECTION:
            addConnection(attrs);
            break;
        case SUMO_TAG_TLLOGIC:
            myCurrentTL = initTrafficLightLogic(attrs, myCurrentTL);
            if (myCurrentTL) {
                myLastParameterised.push_back(myCurrentTL);
            }
            break;
        case SUMO_TAG_PHASE:
            addPhase(attrs, myCurrentTL);
            break;
        case SUMO_TAG_LOCATION:
            myLocation = loadLocation(attrs);
            break;
        case SUMO_TAG_PROHIBITION:
            addProhibition(attrs);
            break;
        case SUMO_TAG_ROUNDABOUT:
            addRoundabout(attrs);
            break;
        case SUMO_TAG_PARAM:
            if (myLastParameterised.size() != 0) {
                bool ok = true;
                const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
                // circumventing empty string test
                const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
                myLastParameterised.back()->setParameter(key, val);
            }
            break;
        default:
            myTypesHandler.myStartElement(element, attrs);
            break;
    }
}


void
NIImporter_SUMO::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_EDGE:
            if (myCurrentEdge != nullptr) {
                if (myEdges.find(myCurrentEdge->id) != myEdges.end()) {
                    WRITE_ERROR("Edge '" + myCurrentEdge->id + "' occurred at least twice in the input.");
                } else {
                    myEdges[myCurrentEdge->id] = myCurrentEdge;
                }
                myCurrentEdge = nullptr;
                myLastParameterised.pop_back();
            }
            break;
        case SUMO_TAG_LANE:
            if (myCurrentEdge != nullptr && myCurrentLane != nullptr) {
                myCurrentEdge->maxSpeed = MAX2(myCurrentEdge->maxSpeed, myCurrentLane->maxSpeed);
                myCurrentEdge->lanes.push_back(myCurrentLane);
                myLastParameterised.pop_back();
            }
            myCurrentLane = nullptr;
            break;
        case SUMO_TAG_TLLOGIC:
            if (!myCurrentTL) {
                WRITE_ERROR("Unmatched closing tag for tl-logic.");
            } else {
                if (!myTLLCont.insert(myCurrentTL)) {
                    WRITE_WARNING("Could not add program '" + myCurrentTL->getProgramID() + "' for traffic light '" + myCurrentTL->getID() + "'");
                    delete myCurrentTL;
                }
                myCurrentTL = nullptr;
                myLastParameterised.pop_back();
            }
            break;
        case SUMO_TAG_JUNCTION:
            if (myCurrentJunction.node != nullptr) {
                myLastParameterised.pop_back();
            }
            break;
        case SUMO_TAG_CONNECTION:
            // !!! this just avoids a crash but is not a real check that it was a connection
            if (!myLastParameterised.empty()) {
                myLastParameterised.pop_back();
            }
            break;
        default:
            break;
    }
}


void
NIImporter_SUMO::addEdge(const SUMOSAXAttributes& attrs) {
    // get the id, report an error if not given or empty...
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    myCurrentEdge = new EdgeAttrs();
    myLastParameterised.push_back(myCurrentEdge);
    myCurrentEdge->builtEdge = nullptr;
    myCurrentEdge->id = id;
    // get the function
    myCurrentEdge->func = attrs.getEdgeFunc(ok);
    if (myCurrentEdge->func == EDGEFUNC_CROSSING) {
        // add the crossing but don't do anything else
        Crossing c(id);
        c.crossingEdges = attrs.get<std::vector<std::string> >(SUMO_ATTR_CROSSING_EDGES, nullptr, ok);
        myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(id)].push_back(c);
        return;
    } else if (myCurrentEdge->func == EDGEFUNC_INTERNAL || myCurrentEdge->func == EDGEFUNC_WALKINGAREA) {
        myHaveSeenInternalEdge = true;
        return; // skip internal edges
    }
    // get the type
    myCurrentEdge->type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    // get the origin and the destination node
    myCurrentEdge->fromNode = attrs.getOpt<std::string>(SUMO_ATTR_FROM, id.c_str(), ok, "");
    myCurrentEdge->toNode = attrs.getOpt<std::string>(SUMO_ATTR_TO, id.c_str(), ok, "");
    myCurrentEdge->priority = attrs.getOpt<int>(SUMO_ATTR_PRIORITY, id.c_str(), ok, -1);
    myCurrentEdge->type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    myCurrentEdge->shape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok, PositionVector());
    NBNetBuilder::transformCoordinates(myCurrentEdge->shape, true, myLocation);
    myCurrentEdge->length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, id.c_str(), ok, NBEdge::UNSPECIFIED_LOADED_LENGTH);
    myCurrentEdge->maxSpeed = 0;
    myCurrentEdge->streetName = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, "");
    myCurrentEdge->distance = attrs.getOpt<double>(SUMO_ATTR_DISTANCE, id.c_str(), ok, 0);
    if (myCurrentEdge->streetName != "" && OptionsCont::getOptions().isDefault("output.street-names")) {
        OptionsCont::getOptions().set("output.street-names", "true");
    }

    std::string lsfS = toString(LANESPREAD_RIGHT);
    lsfS = attrs.getOpt<std::string>(SUMO_ATTR_SPREADTYPE, id.c_str(), ok, lsfS);
    if (SUMOXMLDefinitions::LaneSpreadFunctions.hasString(lsfS)) {
        myCurrentEdge->lsf = SUMOXMLDefinitions::LaneSpreadFunctions.get(lsfS);
    } else {
        WRITE_ERROR("Unknown spreadType '" + lsfS + "' for edge '" + id + "'.");
    }
}


void
NIImporter_SUMO::addLane(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    if (!myCurrentEdge) {
        WRITE_ERROR("Found lane '" + id  + "' not within edge element.");
        return;
    }
    const std::string expectedID = myCurrentEdge->id + "_" + toString(myCurrentEdge->lanes.size());
    if (id != expectedID) {
        WRITE_WARNING("Renaming lane '" + id  + "' to '" + expectedID + "'.");
    }
    myCurrentLane = new LaneAttrs();
    myLastParameterised.push_back(myCurrentLane);
    myCurrentLane->customShape = attrs.getOpt<bool>(SUMO_ATTR_CUSTOMSHAPE, nullptr, ok, false);
    myCurrentLane->shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    myCurrentLane->type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    if (myCurrentEdge->func == EDGEFUNC_CROSSING) {
        // save the width and the lane id of the crossing but don't do anything else
        std::vector<Crossing>& crossings = myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(myCurrentEdge->id)];
        assert(crossings.size() > 0);
        crossings.back().width = attrs.get<double>(SUMO_ATTR_WIDTH, id.c_str(), ok);
        if (myCurrentLane->customShape) {
            crossings.back().customShape = myCurrentLane->shape;
            NBNetBuilder::transformCoordinates(crossings.back().customShape, true, myLocation);
        }
    } else if (myCurrentEdge->func == EDGEFUNC_WALKINGAREA) {
        // save custom shape if needed but don't do anything else
        if (myCurrentLane->customShape) {
            WalkingAreaParsedCustomShape wacs;
            wacs.shape = myCurrentLane->shape;
            NBNetBuilder::transformCoordinates(wacs.shape, true, myLocation);
            myWACustomShapes[myCurrentEdge->id] = wacs;
        }
        return;
    } else if (myCurrentEdge->func == EDGEFUNC_INTERNAL) {
        return; // skip internal edges
    }
    if (attrs.hasAttribute("maxspeed")) {
        // !!! deprecated
        myCurrentLane->maxSpeed = attrs.getFloat("maxspeed");
    } else {
        myCurrentLane->maxSpeed = attrs.get<double>(SUMO_ATTR_SPEED, id.c_str(), ok);
    }
    try {
        myCurrentLane->allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id.c_str(), ok, "", false);
    } catch (EmptyData&) {
        // !!! deprecated
        myCurrentLane->allow = "";
    }
    myCurrentLane->disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    myCurrentLane->width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, id.c_str(), ok, (double) NBEdge::UNSPECIFIED_WIDTH);
    myCurrentLane->endOffset = attrs.getOpt<double>(SUMO_ATTR_ENDOFFSET, id.c_str(), ok, (double) NBEdge::UNSPECIFIED_OFFSET);
    myCurrentLane->accelRamp = attrs.getOpt<bool>(SUMO_ATTR_ACCELERATION, id.c_str(), ok, false);
    // lane coordinates are derived (via lane spread) do not include them in convex boundary
    NBNetBuilder::transformCoordinates(myCurrentLane->shape, false, myLocation);
}


void
NIImporter_SUMO::addStopOffsets(const SUMOSAXAttributes& attrs, bool& ok) {
    std::map<SVCPermissions, double> offsets = parseStopOffsets(attrs, ok);
    if (!ok) {
        return;
    }
    assert(offsets.size() == 1);
    // Admissibility of value will be checked in _loadNetwork(), when lengths are known
    if (myCurrentLane == nullptr) {
        if (myCurrentEdge->stopOffsets.size() != 0) {
            std::stringstream ss;
            ss << "Duplicate definition of stopOffset for edge " << myCurrentEdge->id << ".\nIgnoring duplicate specification.";
            WRITE_WARNING(ss.str());
            return;
        } else {
            myCurrentEdge->stopOffsets = offsets;
        }
    } else {
        if (myCurrentLane->stopOffsets.size() != 0) {
            std::stringstream ss;
            ss << "Duplicate definition of lane's stopOffset on edge " << myCurrentEdge->id << ".\nIgnoring duplicate specifications.";
            WRITE_WARNING(ss.str());
            return;
        } else {
            myCurrentLane->stopOffsets = offsets;
        }
    }
}


void
NIImporter_SUMO::addJunction(const SUMOSAXAttributes& attrs) {
    // get the id, report an error if not given or empty...
    myCurrentJunction.node = nullptr;
    myCurrentJunction.intLanes.clear();
    myCurrentJunction.response.clear();
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    if (id[0] == ':') { // internal node
        return;
    }
    SumoXMLNodeType type = attrs.getNodeType(ok);
    if (ok) {
        if (type == NODETYPE_DEAD_END_DEPRECATED || type == NODETYPE_DEAD_END) {
            // dead end is a computed status. Reset this to unknown so it will
            // be corrected if additional connections are loaded
            type = NODETYPE_UNKNOWN;
        }
    } else {
        WRITE_WARNING("Unknown node type for junction '" + id + "'.");
    }
    Position pos = readPosition(attrs, id, ok);
    NBNetBuilder::transformCoordinate(pos, true, myLocation);
    NBNode* node = new NBNode(id, pos, type);
    myLastParameterised.push_back(node);
    if (!myNodeCont.insert(node)) {
        WRITE_ERROR("Problems on adding junction '" + id + "'.");
        delete node;
        return;
    }
    myCurrentJunction.node = node;
    myCurrentJunction.intLanes = attrs.get<std::vector<std::string> >(SUMO_ATTR_INTLANES, nullptr, ok, false);
    // set optional radius
    if (attrs.hasAttribute(SUMO_ATTR_RADIUS)) {
        node->setRadius(attrs.get<double>(SUMO_ATTR_RADIUS, id.c_str(), ok));
    }
    // handle custom shape
    if (attrs.getOpt<bool>(SUMO_ATTR_CUSTOMSHAPE, nullptr, ok, false)) {
        PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
        NBNetBuilder::transformCoordinates(shape);
        node->setCustomShape(shape);
    }
    if (type == NODETYPE_RAIL_SIGNAL || type == NODETYPE_RAIL_CROSSING) {
        // both types of nodes come without a tlLogic
        myRailSignals.insert(id);
    }
    if (attrs.hasAttribute(SUMO_ATTR_RIGHT_OF_WAY)) {
        node->setRightOfWay(attrs.getRightOfWay(ok));
    }
    if (attrs.hasAttribute(SUMO_ATTR_FRINGE)) {
        node->setFringeType(attrs.getFringeType(ok));
    }
}


void
NIImporter_SUMO::addRequest(const SUMOSAXAttributes& attrs) {
    if (myCurrentJunction.node != nullptr) {
        bool ok = true;
        myCurrentJunction.response.push_back(attrs.get<std::string>(SUMO_ATTR_RESPONSE, nullptr, ok));
    }
}


void
NIImporter_SUMO::addConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, nullptr, ok);
    if (myEdges.count(fromID) == 0) {
        WRITE_ERROR("Unknown edge '" + fromID + "' given in connection.");
        return;
    }
    EdgeAttrs* from = myEdges[fromID];
    Connection conn;
    conn.toEdgeID = attrs.get<std::string>(SUMO_ATTR_TO, nullptr, ok);
    int fromLaneIdx = attrs.get<int>(SUMO_ATTR_FROM_LANE, nullptr, ok);
    conn.toLaneIdx = attrs.get<int>(SUMO_ATTR_TO_LANE, nullptr, ok);
    conn.tlID = attrs.getOpt<std::string>(SUMO_ATTR_TLID, nullptr, ok, "");
    conn.mayDefinitelyPass = attrs.getOpt<bool>(SUMO_ATTR_PASS, nullptr, ok, false);
    conn.keepClear = attrs.getOpt<bool>(SUMO_ATTR_KEEP_CLEAR, nullptr, ok, true);
    conn.contPos = attrs.getOpt<double>(SUMO_ATTR_CONTPOS, nullptr, ok, NBEdge::UNSPECIFIED_CONTPOS);
    conn.visibility = attrs.getOpt<double>(SUMO_ATTR_VISIBILITY_DISTANCE, nullptr, ok, NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE);
    conn.speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, nullptr, ok, NBEdge::UNSPECIFIED_SPEED);
    conn.customShape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, nullptr, ok, PositionVector::EMPTY);
    NBNetBuilder::transformCoordinates(conn.customShape, false, myLocation);
    conn.uncontrolled = attrs.getOpt<bool>(SUMO_ATTR_UNCONTROLLED, nullptr, ok, NBEdge::UNSPECIFIED_CONNECTION_UNCONTROLLED, false);
    if (conn.tlID != "") {
        conn.tlLinkIndex = attrs.get<int>(SUMO_ATTR_TLLINKINDEX, nullptr, ok);
    } else {
        conn.tlLinkIndex = NBConnection::InvalidTlIndex;
    }
    if ((int)from->lanes.size() <= fromLaneIdx) {
        WRITE_ERROR("Invalid lane index '" + toString(fromLaneIdx) + "' for connection from '" + fromID + "'.");
        return;
    }
    from->lanes[fromLaneIdx]->connections.push_back(conn);
    myLastParameterised.push_back(&from->lanes[fromLaneIdx]->connections.back());

    // determine crossing priority and tlIndex
    if (myPedestrianCrossings.size() > 0) {
        if (from->func == EDGEFUNC_WALKINGAREA && myEdges[conn.toEdgeID]->func == EDGEFUNC_CROSSING) {
            // connection from walkingArea to crossing
            std::vector<Crossing>& crossings = myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(fromID)];
            for (std::vector<Crossing>::iterator it = crossings.begin(); it != crossings.end(); ++it) {
                if (conn.toEdgeID == (*it).edgeID) {
                    if (conn.tlID != "") {
                        (*it).priority = true;
                        (*it).customTLIndex = conn.tlLinkIndex;
                    } else {
                        LinkState state = SUMOXMLDefinitions::LinkStates.get(attrs.get<std::string>(SUMO_ATTR_STATE, nullptr, ok));
                        (*it).priority = state == LINKSTATE_MAJOR;
                    }
                }
            }
        } else if (from->func == EDGEFUNC_CROSSING && myEdges[conn.toEdgeID]->func == EDGEFUNC_WALKINGAREA) {
            // connection from crossing to walkingArea (set optional linkIndex2)
            for (Crossing& c : myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(fromID)]) {
                if (fromID == c.edgeID) {
                    c.customTLIndex2 = attrs.getOpt<int>(SUMO_ATTR_TLLINKINDEX, nullptr, ok, -1);
                }
            }
        }
    }
    // determine walking area reference edges
    if (myWACustomShapes.size() > 0) {
        EdgeAttrs* to = myEdges[conn.toEdgeID];
        if (from->func == EDGEFUNC_WALKINGAREA) {
            std::map<std::string, WalkingAreaParsedCustomShape>::iterator it = myWACustomShapes.find(fromID);
            if (it != myWACustomShapes.end()) {
                if (to->func == EDGEFUNC_NORMAL) {
                    // add target sidewalk as reference
                    it->second.toEdges.push_back(conn.toEdgeID);
                } else if (to->func == EDGEFUNC_CROSSING) {
                    // add target crossing edges as reference
                    for (Crossing crossing : myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(fromID)]) {
                        if (conn.toEdgeID == crossing.edgeID) {
                            it->second.toCrossed.insert(it->second.toCrossed.end(), crossing.crossingEdges.begin(), crossing.crossingEdges.end());
                        }
                    }
                }
            }
        } else if (to->func == EDGEFUNC_WALKINGAREA) {
            std::map<std::string, WalkingAreaParsedCustomShape>::iterator it = myWACustomShapes.find(conn.toEdgeID);
            if (it != myWACustomShapes.end()) {
                if (from->func == EDGEFUNC_NORMAL) {
                    // add origin sidewalk as reference
                    it->second.fromEdges.push_back(fromID);
                } else if (from->func == EDGEFUNC_CROSSING) {
                    // add origin crossing edges as reference
                    for (Crossing crossing : myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(fromID)]) {
                        if (fromID == crossing.edgeID) {
                            it->second.fromCrossed.insert(it->second.fromCrossed.end(), crossing.crossingEdges.begin(), crossing.crossingEdges.end());
                        }
                    }
                }
            }
        }
    }
}


void
NIImporter_SUMO::addProhibition(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string prohibitor = attrs.getOpt<std::string>(SUMO_ATTR_PROHIBITOR, nullptr, ok, "");
    std::string prohibited = attrs.getOpt<std::string>(SUMO_ATTR_PROHIBITED, nullptr, ok, "");
    if (!ok) {
        return;
    }
    Prohibition p;
    parseProhibitionConnection(prohibitor, p.prohibitorFrom, p.prohibitorTo, ok);
    parseProhibitionConnection(prohibited, p.prohibitedFrom, p.prohibitedTo, ok);
    if (!ok) {
        return;
    }
    myProhibitions.push_back(p);
}


NIImporter_SUMO::LaneAttrs*
NIImporter_SUMO::getLaneAttrsFromID(EdgeAttrs* edge, std::string lane_id) {
    std::string edge_id;
    int index;
    NBHelpers::interpretLaneID(lane_id, edge_id, index);
    assert(edge->id == edge_id);
    if ((int)edge->lanes.size() <= index) {
        WRITE_ERROR("Unknown lane '" + lane_id + "' given in succedge.");
        return nullptr;
    } else {
        return edge->lanes[index];
    }
}


NBLoadedSUMOTLDef*
NIImporter_SUMO::initTrafficLightLogic(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL) {
    if (currentTL) {
        WRITE_ERROR("Definition of tl-logic '" + currentTL->getID() + "' was not finished.");
        return nullptr;
    }
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    SUMOTime offset = TIME2STEPS(attrs.get<double>(SUMO_ATTR_OFFSET, id.c_str(), ok));
    std::string programID = attrs.getOpt<std::string>(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    std::string typeS = attrs.get<std::string>(SUMO_ATTR_TYPE, nullptr, ok);
    TrafficLightType type;
    if (SUMOXMLDefinitions::TrafficLightTypes.hasString(typeS)) {
        type = SUMOXMLDefinitions::TrafficLightTypes.get(typeS);
    } else {
        WRITE_ERROR("Unknown traffic light type '" + typeS + "' for tlLogic '" + id + "'.");
        return nullptr;
    }
    if (ok) {
        return new NBLoadedSUMOTLDef(id, programID, offset, type);
    } else {
        return nullptr;
    }
}


void
NIImporter_SUMO::addPhase(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL) {
    if (!currentTL) {
        WRITE_ERROR("found phase without tl-logic");
        return;
    }
    const std::string& id = currentTL->getID();
    bool ok = true;
    std::string state = attrs.get<std::string>(SUMO_ATTR_STATE, id.c_str(), ok);
    SUMOTime duration = TIME2STEPS(attrs.get<double>(SUMO_ATTR_DURATION, id.c_str(), ok));
    if (duration < 0) {
        WRITE_ERROR("Phase duration for tl-logic '" + id + "/" + currentTL->getProgramID() + "' must be positive.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    SUMOTime minDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MINDURATION, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    SUMOTime maxDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MAXDURATION, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    std::vector<int> nextPhases = attrs.getOptIntVector(SUMO_ATTR_NEXT, nullptr, ok);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, nullptr, ok, "");
    if (ok) {
        currentTL->addPhase(duration, state, minDuration, maxDuration, nextPhases, name);
    }
}


PositionVector
NIImporter_SUMO::reconstructEdgeShape(const EdgeAttrs* edge, const Position& from, const Position& to) {
    PositionVector result;
    result.push_back(from);

    if (edge->lanes[0]->customShape) {
        // this is a new network where edge shapes are writen if they exist.
        result.push_back(to);
        return result;
    }
    const PositionVector& firstLane = edge->lanes[0]->shape;

    // reverse logic of NBEdge::computeLaneShape
    // !!! this will only work for old-style constant width lanes
    const int noLanes = (int)edge->lanes.size();
    double offset;
    if (edge->lsf == LANESPREAD_RIGHT) {
        offset = (SUMO_const_laneWidth + SUMO_const_laneOffset) / 2.; // @todo: why is the lane offset counted in here?
    } else {
        offset = (SUMO_const_laneWidth) / 2. - (SUMO_const_laneWidth * (double)noLanes - 1) / 2.; ///= -2.; // @todo: actually, when looking at the road networks, the center line is not in the center
    }
    for (int i = 1; i < (int)firstLane.size() - 1; i++) {
        const Position& from = firstLane[i - 1];
        const Position& me = firstLane[i];
        const Position& to = firstLane[i + 1];
        Position offsets = PositionVector::sideOffset(from, me, offset);
        Position offsets2 = PositionVector::sideOffset(me, to, offset);

        PositionVector l1(from - offsets, me - offsets);
        l1.extrapolate(100);
        PositionVector l2(me - offsets2, to - offsets2);
        l2.extrapolate(100);
        if (l1.intersects(l2)) {
            result.push_back(l1.intersectionPosition2D(l2));
        } else {
            WRITE_WARNING("Could not reconstruct shape for edge '" + edge->id + "'.");
        }
    }

    result.push_back(to);
    return result;
}


GeoConvHelper*
NIImporter_SUMO::loadLocation(const SUMOSAXAttributes& attrs) {
    // @todo refactor parsing of location since its duplicated in NLHandler and PCNetProjectionLoader
    bool ok = true;
    GeoConvHelper* result = nullptr;
    PositionVector s = attrs.get<PositionVector>(SUMO_ATTR_NET_OFFSET, nullptr, ok);
    Boundary convBoundary = attrs.get<Boundary>(SUMO_ATTR_CONV_BOUNDARY, nullptr, ok);
    Boundary origBoundary = attrs.get<Boundary>(SUMO_ATTR_ORIG_BOUNDARY, nullptr, ok);
    std::string proj = attrs.get<std::string>(SUMO_ATTR_ORIG_PROJ, nullptr, ok);
    if (ok) {
        Position networkOffset = s[0];
        result = new GeoConvHelper(proj, networkOffset, origBoundary, convBoundary);
        GeoConvHelper::setLoaded(*result);
    }
    return result;
}


Position
NIImporter_SUMO::readPosition(const SUMOSAXAttributes& attrs, const std::string& id, bool& ok) {
    const double x = attrs.get<double>(SUMO_ATTR_X, id.c_str(), ok);
    const double y = attrs.get<double>(SUMO_ATTR_Y, id.c_str(), ok);
    const double z = attrs.getOpt<double>(SUMO_ATTR_Z, id.c_str(), ok, 0.);
    return Position(x, y, z);
}


void
NIImporter_SUMO::parseProhibitionConnection(const std::string& attr, std::string& from, std::string& to, bool& ok) {
    // split from/to
    const std::string::size_type div = attr.find("->");
    if (div == std::string::npos) {
        WRITE_ERROR("Missing connection divider in prohibition attribute '" + attr + "'");
        ok = false;
    }
    from = attr.substr(0, div);
    to = attr.substr(div + 2);
    // check whether the definition includes a lane information and discard it
    if (from.find('_') != std::string::npos) {
        from = from.substr(0, from.find('_'));
    }
    if (to.find('_') != std::string::npos) {
        to = to.substr(0, to.find('_'));
    }
    // check whether the edges are known
    if (myEdges.count(from) == 0) {
        WRITE_ERROR("Unknown edge prohibition '" + from + "'");
        ok = false;
    }
    if (myEdges.count(to) == 0) {
        WRITE_ERROR("Unknown edge prohibition '" + to + "'");
        ok = false;
    }
}


void
NIImporter_SUMO::addRoundabout(const SUMOSAXAttributes& attrs) {
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        myRoundabouts.push_back(attrs.getStringVector(SUMO_ATTR_EDGES));
    } else {
        WRITE_ERROR("Empty edges in roundabout.");
    }
}


/****************************************************************************/
