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
/// @file    NIImporter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    Mon, 14.04.2008
///
// Importer for networks stored in SUMO format
/****************************************************************************/
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
      myNetworkVersion(0, 0),
      myHaveSeenInternalEdge(false),
      myAmLefthand(false),
      myChangeLefthand(false),
      myCornerDetail(0),
      myLinkDetail(-1),
      myRectLaneCut(false),
      myWalkingAreas(false),
      myLimitTurnSpeed(-1),
      myCheckLaneFoesAll(false),
      myCheckLaneFoesRoundabout(true),
      myTlsIgnoreInternalJunctionJam(false),
      myDefaultSpreadType(toString(LaneSpreadFunction::RIGHT)),
      myGeomAvoidOverlap(true),
      myJunctionsHigherSpeed(false),
      myInternalJunctionsVehicleWidth(OptionsCont::getOptions().getFloat("internal-junctions.vehicle-width")) {
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
    const std::vector<std::string> discardableParams = oc.getStringVector("discard-params");
    myDiscardableParams.insert(discardableParams.begin(), discardableParams.end());
    // parse file(s)
    const std::vector<std::string> files = oc.getStringVector("sumo-net-file");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::isReadable(*file)) {
            WRITE_ERRORF(TL("Could not open sumo-net-file '%'."), *file);
            return;
        }
        setFileName(*file);
        const long before = PROGRESS_BEGIN_TIME_MESSAGE("Parsing sumo-net from '" + *file + "'");
        XMLSubSys::runParser(*this, *file, true);
        PROGRESS_TIME_MESSAGE(before);
    }
    // build edges
    const double maxSegmentLength = oc.getFloat("geometry.max-segment-length");
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        // skip internal edges
        if (ed->func == SumoXMLEdgeFunc::INTERNAL || ed->func == SumoXMLEdgeFunc::CROSSING || ed->func == SumoXMLEdgeFunc::WALKINGAREA) {
            continue;
        }
        // get and check the nodes
        NBNode* from = myNodeCont.retrieve(ed->fromNode);
        NBNode* to = myNodeCont.retrieve(ed->toNode);
        if (from == nullptr) {
            WRITE_ERRORF(TL("Edge's '%' from-node '%' is not known."), ed->id, ed->fromNode);
            continue;
        }
        if (to == nullptr) {
            WRITE_ERRORF(TL("Edge's '%' to-node '%' is not known."), ed->id, ed->toNode);
            continue;
        }
        if (from == to) {
            WRITE_ERRORF(TL("Edge's '%' from-node and to-node '%' are identical."), ed->id, ed->toNode);
            continue;
        }
        if (ed->shape.size() == 0 && maxSegmentLength > 0) {
            ed->shape.push_back(from->getPosition());
            ed->shape.push_back(to->getPosition());
            // shape is already cartesian but we must use a copy because the original will be modified
            NBNetBuilder::addGeometrySegments(ed->shape, PositionVector(ed->shape), maxSegmentLength);
        }
        // build and insert the edge
        NBEdge* e = new NBEdge(ed->id, from, to,
                               ed->type, ed->maxSpeed, NBEdge::UNSPECIFIED_FRICTION,
                               (int) ed->lanes.size(),
                               ed->priority, NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET,
                               ed->shape, ed->lsf, ed->streetName, "", true); // always use tryIgnoreNodePositions to keep original shape
        e->setLoadedLength(ed->length);
        e->updateParameters(ed->getParametersMap());
        e->setDistance(ed->distance);
        if (!myNetBuilder.getEdgeCont().insert(e)) {
            WRITE_ERRORF(TL("Could not insert edge '%'."), ed->id);
            delete e;
            continue;
        }
        ed->builtEdge = myNetBuilder.getEdgeCont().retrieve(ed->id);
        if (ed->builtEdge != nullptr) {
            ed->builtEdge->setEdgeStopOffset(-1, ed->edgeStopOffset);
            ed->builtEdge->setBidi(ed->bidi != "");
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
                    WRITE_ERRORF(TL("Unknown edge '%' given in connection."), c.toEdgeID);
                    continue;
                }
                NBEdge* toEdge = myEdges[c.toEdgeID]->builtEdge;
                if (toEdge == nullptr) { // removed by explicit list, vclass, ...
                    continue;
                }
                if (toEdge->getFromNode() != nbe->getToNode()) { // inconsistency may occur when merging networks
                    WRITE_WARNINGF("Removing invalid connection from edge '%' to edge '%'", nbe->getID(), toEdge->getID());
                    continue;
                }
                // patch attribute uncontrolled for legacy networks where it is not set explicitly
                bool uncontrolled = c.uncontrolled;

                if ((NBNode::isTrafficLight(toType) || toType == SumoXMLNodeType::RAIL_SIGNAL)
                        && c.tlLinkIndex == NBConnection::InvalidTlIndex) {
                    uncontrolled = true;
                }
                nbe->addLane2LaneConnection(
                    fromLaneIndex, toEdge, c.toLaneIdx, NBEdge::Lane2LaneInfoType::VALIDATED,
                    true, c.mayDefinitelyPass, c.keepClear ? KEEPCLEAR_TRUE : KEEPCLEAR_FALSE,
                    c.contPos, c.visibility, c.speed, c.friction, c.customLength, c.customShape, uncontrolled, c.permissions, c.indirectLeft, c.edgeType, c.changeLeft, c.changeRight);
                if (c.getParametersMap().size() > 0) {
                    nbe->getConnectionRef(fromLaneIndex, toEdge, c.toLaneIdx).updateParameters(c.getParametersMap());
                }
                // maybe we have a tls-controlled connection
                if (c.tlID != "" && myRailSignals.count(c.tlID) == 0) {
                    const std::map<std::string, NBTrafficLightDefinition*>& programs = myTLLCont.getPrograms(c.tlID);
                    if (programs.size() > 0) {
                        std::map<std::string, NBTrafficLightDefinition*>::const_iterator it;
                        for (it = programs.begin(); it != programs.end(); it++) {
                            NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it->second);
                            if (tlDef) {
                                tlDef->addConnection(nbe, toEdge, fromLaneIndex, c.toLaneIdx, c.tlLinkIndex, c.tlLinkIndex2, false);
                            } else {
                                throw ProcessError("Corrupt traffic light definition '" + c.tlID + "' (program '" + it->first + "')");
                            }
                        }
                    } else {
                        WRITE_ERRORF(TL("The traffic light '%' is not known."), c.tlID);
                    }
                }
            }
            // allow/disallow XXX preferred
            if (!dismissVclasses) {
                nbe->setPermissions(parseVehicleClasses(lane->allow, lane->disallow, myNetworkVersion), fromLaneIndex);
            }
            nbe->setPermittedChanging(fromLaneIndex, parseVehicleClasses(lane->changeLeft, ""), parseVehicleClasses(lane->changeRight, ""));
            // width, offset
            nbe->setLaneWidth(fromLaneIndex, lane->width);
            nbe->setEndOffset(fromLaneIndex, lane->endOffset);
            nbe->setSpeed(fromLaneIndex, lane->maxSpeed);
            nbe->setFriction(fromLaneIndex, lane->friction);
            nbe->setAcceleration(fromLaneIndex, lane->accelRamp);
            nbe->getLaneStruct(fromLaneIndex).oppositeID = lane->oppositeID;
            nbe->getLaneStruct(fromLaneIndex).type = lane->type;
            nbe->getLaneStruct(fromLaneIndex).updateParameters(lane->getParametersMap());
            if (lane->customShape) {
                nbe->setLaneShape(fromLaneIndex, lane->shape);
            }
            // stop offset for lane
            bool stopOffsetSet = false;
            if (lane->laneStopOffset.isDefined() || nbe->getEdgeStopOffset().isDefined()) {
                // apply lane-specific stopOffset (might be none as well)
                stopOffsetSet = nbe->setEdgeStopOffset(fromLaneIndex, lane->laneStopOffset);
            }
            if (!stopOffsetSet) {
                // apply default stop offset to lane
                nbe->setEdgeStopOffset(fromLaneIndex, nbe->getEdgeStopOffset());
            }
        }
        nbe->declareConnectionsAsLoaded();
        if (!nbe->hasLaneSpecificWidth() && nbe->getLanes()[0].width != NBEdge::UNSPECIFIED_WIDTH) {
            nbe->setLaneWidth(-1, nbe->getLaneWidth(0));
        }
        if (!nbe->hasLaneSpecificEndOffset() && nbe->getEndOffset(0) != NBEdge::UNSPECIFIED_OFFSET) {
            nbe->setEndOffset(-1, nbe->getEndOffset(0));
        }
        if (!nbe->hasLaneSpecificStopOffsets() && nbe->getEdgeStopOffset().isDefined()) {
            nbe->setEdgeStopOffset(-1, nbe->getEdgeStopOffset());
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
            WRITE_WARNINGF(TL("Edge '%' in prohibition was not built."), it->prohibitedFrom);
        } else if (prohibitedTo == nullptr) {
            WRITE_WARNINGF(TL("Edge '%' in prohibition was not built."), it->prohibitedTo);
        } else if (prohibitorFrom == nullptr) {
            WRITE_WARNINGF(TL("Edge '%' in prohibition was not built."), it->prohibitorFrom);
        } else if (prohibitorTo == nullptr) {
            WRITE_WARNINGF(TL("Edge '%' in prohibition was not built."), it->prohibitorTo);
        } else {
            NBNode* n = prohibitedFrom->getToNode();
            n->addSortedLinkFoes(
                NBConnection(prohibitorFrom, prohibitorTo),
                NBConnection(prohibitedFrom, prohibitedTo));
        }
    }
    if (!myHaveSeenInternalEdge && oc.isWriteable("no-internal-links")) {
        oc.set("no-internal-links", "true");
    }
    if (oc.isWriteable("lefthand")) {
        oc.set("lefthand", toString(myAmLefthand));
    }
    if (oc.isWriteable("junctions.corner-detail")) {
        oc.set("junctions.corner-detail", toString(myCornerDetail));
    }
    if (oc.isWriteable("junctions.internal-link-detail") && myLinkDetail > 0) {
        oc.set("junctions.internal-link-detail", toString(myLinkDetail));
    }
    if (oc.isWriteable("rectangular-lane-cut")) {
        oc.set("rectangular-lane-cut", toString(myRectLaneCut));
    }
    if (oc.isWriteable("walkingareas")) {
        oc.set("walkingareas", toString(myWalkingAreas));
    }
    if (oc.isWriteable("junctions.limit-turn-speed")) {
        oc.set("junctions.limit-turn-speed", toString(myLimitTurnSpeed));
    }
    if (oc.isWriteable("check-lane-foes.all") && oc.getBool("check-lane-foes.all") != myCheckLaneFoesAll) {
        oc.set("check-lane-foes.all", toString(myCheckLaneFoesAll));
    }
    if (oc.isWriteable("check-lane-foes.roundabout") && oc.getBool("check-lane-foes.roundabout") != myCheckLaneFoesRoundabout) {
        oc.set("check-lane-foes.roundabout", toString(myCheckLaneFoesRoundabout));
    }
    if (oc.isWriteable("tls.ignore-internal-junction-jam") && oc.getBool("tls.ignore-internal-junction-jam") != myTlsIgnoreInternalJunctionJam) {
        oc.set("tls.ignore-internal-junction-jam", toString(myTlsIgnoreInternalJunctionJam));
    }
    if (oc.isWriteable("default.spreadtype") && oc.getString("default.spreadtype") != myDefaultSpreadType) {
        oc.set("default.spreadtype", myDefaultSpreadType);
    }
    if (oc.isWriteable("geometry.avoid-overlap") && oc.getBool("geometry.avoid-overlap") != myGeomAvoidOverlap) {
        oc.set("geometry.avoid-overlap", toString(myGeomAvoidOverlap));
    }
    if (oc.isWriteable("junctions.higher-speed") && oc.getBool("junctions.higher-speed") != myJunctionsHigherSpeed) {
        oc.set("junctions.higher-speed", toString(myJunctionsHigherSpeed));
    }
    if (oc.isWriteable("internal-junctions.vehicle-width") && oc.getFloat("internal-junctions.vehicle-width") != myInternalJunctionsVehicleWidth) {
        oc.set("internal-junctions.vehicle-width", toString(myInternalJunctionsVehicleWidth));
    }
    if (!deprecatedVehicleClassesSeen.empty()) {
        WRITE_WARNINGF(TL("Deprecated vehicle class(es) '%' in input network."), toString(deprecatedVehicleClassesSeen));
        deprecatedVehicleClassesSeen.clear();
    }
    if (!oc.getBool("no-internal-links")) {
        // add loaded crossings
        for (const auto& crossIt : myPedestrianCrossings) {
            NBNode* const node = myNodeCont.retrieve(crossIt.first);
            for (const Crossing& crossing : crossIt.second) {
                EdgeVector edges;
                for (const std::string& edgeID : crossing.crossingEdges) {
                    NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(edgeID);
                    // edge might have been removed due to options
                    if (edge != nullptr) {
                        edges.push_back(edge);
                    }
                }
                if (!edges.empty()) {
                    node->addCrossing(edges, crossing.width, crossing.priority,
                                      crossing.customTLIndex, crossing.customTLIndex2, crossing.customShape, true);
                }
            }
        }
        // add walking area custom shapes
        for (const auto& item : myWACustomShapes) {
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
            for (const std::string& edgeID : edgeIDs) {
                NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(edgeID);
                // edge might have been removed due to options
                if (edge != nullptr) {
                    edges.push_back(edge);
                }
            }
            if (edges.size() > 0) {
                node->addWalkingAreaShape(edges, item.second.shape, item.second.width);
            }
        }
    }
    // add roundabouts
    for (const std::vector<std::string>& ra : myRoundabouts) {
        EdgeSet roundabout;
        for (const std::string& edgeID : ra) {
            NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(edgeID);
            if (edge == nullptr) {
                if (!myNetBuilder.getEdgeCont().wasIgnored(edgeID)) {
                    WRITE_ERRORF(TL("Unknown edge '%' in roundabout"), (edgeID));
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
     *      netedit. We will probably have to patch NBNetBuilder to contain them
     *      and hand them over to netedit
     *    alternative idea: those shouldn't really be contained within the
     *    network but rather in separate files. teach netedit how to open those
     *    (POI?)
     * 4) those which are of concern neither to NBNetBuilder nor netedit and
     *    must be copied over - need to patch NBNetBuilder for this.
     *    copy unknown by default
     */
    switch (element) {
        case SUMO_TAG_NET: {
            bool ok;
            myNetworkVersion = StringUtils::toVersion(attrs.get<std::string>(SUMO_ATTR_VERSION, nullptr, ok, false));
            myAmLefthand = attrs.getOpt<bool>(SUMO_ATTR_LEFTHAND, nullptr, ok, false);
            myCornerDetail = attrs.getOpt<int>(SUMO_ATTR_CORNERDETAIL, nullptr, ok, 0);
            myLinkDetail = attrs.getOpt<int>(SUMO_ATTR_LINKDETAIL, nullptr, ok, -1);
            myRectLaneCut = attrs.getOpt<bool>(SUMO_ATTR_RECTANGULAR_LANE_CUT, nullptr, ok, false);
            myWalkingAreas = attrs.getOpt<bool>(SUMO_ATTR_WALKINGAREAS, nullptr, ok, false);
            myLimitTurnSpeed = attrs.getOpt<double>(SUMO_ATTR_LIMIT_TURN_SPEED, nullptr, ok, -1);
            myCheckLaneFoesAll = attrs.getOpt<bool>(SUMO_ATTR_CHECKLANEFOES_ALL, nullptr, ok, false);
            myCheckLaneFoesRoundabout = attrs.getOpt<bool>(SUMO_ATTR_CHECKLANEFOES_ROUNDABOUT, nullptr, ok, true);
            myTlsIgnoreInternalJunctionJam = attrs.getOpt<bool>(SUMO_ATTR_TLS_IGNORE_INTERNAL_JUNCTION_JAM, nullptr, ok, false);
            myDefaultSpreadType = attrs.getOpt<std::string>(SUMO_ATTR_SPREADTYPE, nullptr, ok, myDefaultSpreadType);
            myGeomAvoidOverlap = attrs.getOpt<bool>(SUMO_ATTR_AVOID_OVERLAP, nullptr, ok, myGeomAvoidOverlap);
            myJunctionsHigherSpeed = attrs.getOpt<bool>(SUMO_ATTR_HIGHER_SPEED, nullptr, ok, myJunctionsHigherSpeed);
            myInternalJunctionsVehicleWidth = attrs.getOpt<double>(SUMO_ATTR_INTERNAL_JUNCTIONS_VEHICLE_WIDTH, nullptr, ok, myInternalJunctionsVehicleWidth);
            // derived
            const OptionsCont& oc = OptionsCont::getOptions();
            myChangeLefthand = !oc.isDefault("lefthand") && (oc.getBool("lefthand") != myAmLefthand);

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
            delete myLocation;
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
                if (myDiscardableParams.count(key) == 0) {
                    // circumventing empty string test
                    const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
                    myLastParameterised.back()->setParameter(key, val);
                }
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
                    WRITE_WARNINGF(TL("Edge '%' occurred at least twice in the input."), myCurrentEdge->id);
                    for (LaneAttrs* const lane : myCurrentEdge->lanes) {
                        delete lane;
                    }
                    delete myCurrentEdge;
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
            if (myCurrentTL == nullptr) {
                WRITE_ERROR(TL("Unmatched closing tag for tl-logic."));
            } else {
                if (!myTLLCont.insert(myCurrentTL)) {
                    WRITE_WARNINGF(TL("Could not add program '%' for traffic light '%'"), myCurrentTL->getProgramID(), myCurrentTL->getID());
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
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    myCurrentEdge = new EdgeAttrs();
    myLastParameterised.push_back(myCurrentEdge);
    myCurrentEdge->builtEdge = nullptr;
    myCurrentEdge->id = id;
    // get the function
    myCurrentEdge->func = attrs.getOpt<SumoXMLEdgeFunc>(SUMO_ATTR_FUNCTION, id.c_str(), ok, SumoXMLEdgeFunc::NORMAL);
    if (myCurrentEdge->func == SumoXMLEdgeFunc::CROSSING) {
        // add the crossing but don't do anything else
        Crossing c(id);
        c.crossingEdges = attrs.get<std::vector<std::string> >(SUMO_ATTR_CROSSING_EDGES, nullptr, ok);
        myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(id)].push_back(c);
        return;
    } else if (myCurrentEdge->func == SumoXMLEdgeFunc::INTERNAL || myCurrentEdge->func == SumoXMLEdgeFunc::WALKINGAREA) {
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
    myCurrentEdge->bidi = attrs.getOpt<std::string>(SUMO_ATTR_BIDI, id.c_str(), ok, "");
    if (myCurrentEdge->streetName != "" && OptionsCont::getOptions().isDefault("output.street-names")) {
        OptionsCont::getOptions().set("output.street-names", "true");
    }

    std::string lsfS = attrs.getOpt<std::string>(SUMO_ATTR_SPREADTYPE, id.c_str(), ok, myDefaultSpreadType);
    if (SUMOXMLDefinitions::LaneSpreadFunctions.hasString(lsfS)) {
        myCurrentEdge->lsf = SUMOXMLDefinitions::LaneSpreadFunctions.get(lsfS);
    } else {
        WRITE_ERRORF(TL("Unknown spreadType '%' for edge '%'."), lsfS, id);
    }
}


void
NIImporter_SUMO::addLane(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    if (myCurrentEdge == nullptr) {
        WRITE_ERRORF(TL("Found lane '%' not within edge element."), id);
        return;
    }
    const std::string expectedID = myCurrentEdge->id + "_" + toString(myCurrentEdge->lanes.size());
    if (id != expectedID) {
        WRITE_WARNINGF(TL("Renaming lane '%' to '%'."), id, expectedID);
    }
    myCurrentLane = new LaneAttrs();
    myLastParameterised.push_back(myCurrentLane);
    myCurrentLane->customShape = attrs.getOpt<bool>(SUMO_ATTR_CUSTOMSHAPE, nullptr, ok, false);
    myCurrentLane->shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    myCurrentLane->width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, id.c_str(), ok, (double) NBEdge::UNSPECIFIED_WIDTH);
    myCurrentLane->type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    if (myCurrentEdge->func == SumoXMLEdgeFunc::CROSSING) {
        // save the width and the lane id of the crossing but don't do anything else
        std::vector<Crossing>& crossings = myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(myCurrentEdge->id)];
        assert(crossings.size() > 0);
        crossings.back().width = attrs.get<double>(SUMO_ATTR_WIDTH, id.c_str(), ok);
        if (myCurrentLane->customShape) {
            crossings.back().customShape = myCurrentLane->shape;
            NBNetBuilder::transformCoordinates(crossings.back().customShape, true, myLocation);
        }
    } else if (myCurrentEdge->func == SumoXMLEdgeFunc::WALKINGAREA) {
        // save custom shape if needed but don't do anything else
        if (myCurrentLane->customShape) {
            WalkingAreaParsedCustomShape wacs;
            wacs.shape = myCurrentLane->shape;
            wacs.width = myCurrentLane->width;
            NBNetBuilder::transformCoordinates(wacs.shape, true, myLocation);
            myWACustomShapes[myCurrentEdge->id] = wacs;
        }
        return;
    } else if (myCurrentEdge->func == SumoXMLEdgeFunc::INTERNAL) {
        return; // skip internal edges
    }
    if (attrs.hasAttribute("maxspeed")) {
        // !!! deprecated
        myCurrentLane->maxSpeed = attrs.getFloat("maxspeed");
    } else {
        myCurrentLane->maxSpeed = attrs.get<double>(SUMO_ATTR_SPEED, id.c_str(), ok);
    }
    myCurrentLane->friction = attrs.getOpt<double>(SUMO_ATTR_FRICTION, id.c_str(), ok, NBEdge::UNSPECIFIED_FRICTION, false); //sets 1 on empty
    try {
        myCurrentLane->allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id.c_str(), ok, "", false);
    } catch (EmptyData&) {
        // !!! deprecated
        myCurrentLane->allow = "";
    }
    myCurrentLane->disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    myCurrentLane->endOffset = attrs.getOpt<double>(SUMO_ATTR_ENDOFFSET, id.c_str(), ok, (double) NBEdge::UNSPECIFIED_OFFSET);
    myCurrentLane->accelRamp = attrs.getOpt<bool>(SUMO_ATTR_ACCELERATION, id.c_str(), ok, false);
    myCurrentLane->changeLeft = attrs.getOpt<std::string>(SUMO_ATTR_CHANGE_LEFT, id.c_str(), ok, "");
    myCurrentLane->changeRight = attrs.getOpt<std::string>(SUMO_ATTR_CHANGE_RIGHT, id.c_str(), ok, "");
    if (myChangeLefthand) {
        std::swap(myCurrentLane->changeLeft, myCurrentLane->changeRight);
    }

    // lane coordinates are derived (via lane spread) do not include them in convex boundary
    NBNetBuilder::transformCoordinates(myCurrentLane->shape, false, myLocation);
}


void
NIImporter_SUMO::addStopOffsets(const SUMOSAXAttributes& attrs, bool& ok) {
    const StopOffset offset(attrs, ok);
    if (!ok) {
        return;
    }
    // Admissibility of value will be checked in _loadNetwork(), when lengths are known
    if (myCurrentLane == nullptr) {
        if (myCurrentEdge->edgeStopOffset.isDefined()) {
            WRITE_WARNINGF(TL("Duplicate definition of stopOffset for edge %.\nIgnoring duplicate specification."), myCurrentEdge->id);
        } else {
            myCurrentEdge->edgeStopOffset = offset;
        }
    } else {
        if (myCurrentLane->laneStopOffset.isDefined()) {
            WRITE_WARNINGF(TL("Duplicate definition of lane's stopOffset on edge %.\nIgnoring duplicate specifications."), myCurrentEdge->id);
        } else {
            myCurrentLane->laneStopOffset = offset;
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
    SumoXMLNodeType type = attrs.getOpt<SumoXMLNodeType>(SUMO_ATTR_TYPE, id.c_str(), ok, SumoXMLNodeType::UNKNOWN);
    if (ok) {
        if (type == SumoXMLNodeType::DEAD_END_DEPRECATED || type == SumoXMLNodeType::DEAD_END) {
            // dead end is a computed status. Reset this to unknown so it will
            // be corrected if additional connections are loaded
            type = SumoXMLNodeType::UNKNOWN;
        } else if (type == SumoXMLNodeType::INTERNAL) {
            WRITE_WARNINGF("Invalid node type '%' for junction '%' in input network", toString(SumoXMLNodeType::INTERNAL), id);
            type = SumoXMLNodeType::UNKNOWN;
        }
    }
    Position pos = readPosition(attrs, id, ok);
    NBNetBuilder::transformCoordinate(pos, true, myLocation);
    NBNode* node = new NBNode(id, pos, type);
    if (!myNodeCont.insert(node)) {
        WRITE_WARNINGF(TL("Junction '%' occurred at least twice in the input."), id);
        delete node;
        myLastParameterised.push_back(myNodeCont.retrieve(id));
        return;
    } else {
        myLastParameterised.push_back(node);
    }
    myCurrentJunction.node = node;
    myCurrentJunction.intLanes = attrs.get<std::vector<std::string> >(SUMO_ATTR_INTLANES, nullptr, ok, false);
    // set optional radius
    if (attrs.hasAttribute(SUMO_ATTR_RADIUS)) {
        node->setRadius(attrs.get<double>(SUMO_ATTR_RADIUS, id.c_str(), ok));
    }
    // handle custom shape
    if (attrs.getOpt<bool>(SUMO_ATTR_CUSTOMSHAPE, id.c_str(), ok, false)) {
        PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
        NBNetBuilder::transformCoordinates(shape, true, myLocation);
        node->setCustomShape(shape);
    }
    if (type == SumoXMLNodeType::RAIL_SIGNAL || type == SumoXMLNodeType::RAIL_CROSSING) {
        // both types of nodes come without a tlLogic
        myRailSignals.insert(id);
    }
    node->setRightOfWay(attrs.getOpt<RightOfWay>(SUMO_ATTR_RIGHT_OF_WAY, id.c_str(), ok, node->getRightOfWay()));
    node->setFringeType(attrs.getOpt<FringeType>(SUMO_ATTR_FRINGE, id.c_str(), ok, node->getFringeType()));
    if (attrs.hasAttribute(SUMO_ATTR_NAME)) {
        node->setName(attrs.get<std::string>(SUMO_ATTR_NAME, id.c_str(), ok));
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
        WRITE_ERRORF(TL("Unknown edge '%' given in connection."), fromID);
        return;
    }
    EdgeAttrs* from = myEdges[fromID];
    if (from->func == SumoXMLEdgeFunc::INTERNAL) {
        // internal junction connection
        return;
    }
    Connection conn;
    conn.toEdgeID = attrs.get<std::string>(SUMO_ATTR_TO, nullptr, ok);
    int fromLaneIdx = attrs.get<int>(SUMO_ATTR_FROM_LANE, nullptr, ok);
    conn.toLaneIdx = attrs.get<int>(SUMO_ATTR_TO_LANE, nullptr, ok);
    conn.tlID = attrs.getOpt<std::string>(SUMO_ATTR_TLID, nullptr, ok, "");
    conn.mayDefinitelyPass = attrs.getOpt<bool>(SUMO_ATTR_PASS, nullptr, ok, false);
    conn.keepClear = attrs.getOpt<bool>(SUMO_ATTR_KEEP_CLEAR, nullptr, ok, true);
    conn.indirectLeft = attrs.getOpt<bool>(SUMO_ATTR_INDIRECT, nullptr, ok, false);
    conn.edgeType = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, nullptr, ok, "");
    conn.contPos = attrs.getOpt<double>(SUMO_ATTR_CONTPOS, nullptr, ok, NBEdge::UNSPECIFIED_CONTPOS);
    conn.visibility = attrs.getOpt<double>(SUMO_ATTR_VISIBILITY_DISTANCE, nullptr, ok, NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE);
    std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, nullptr, ok, "", false);
    std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, nullptr, ok, "", false);
    if (allow == "" && disallow == "") {
        conn.permissions = SVC_UNSPECIFIED;
    } else {
        conn.permissions = parseVehicleClasses(allow, disallow, myNetworkVersion);
    }
    if (attrs.hasAttribute(SUMO_ATTR_CHANGE_LEFT)) {
        conn.changeLeft = parseVehicleClasses(attrs.get<std::string>(SUMO_ATTR_CHANGE_LEFT, nullptr, ok), "");
    } else {
        conn.changeLeft = SVC_UNSPECIFIED;
    }
    if (attrs.hasAttribute(SUMO_ATTR_CHANGE_RIGHT)) {
        conn.changeRight = parseVehicleClasses(attrs.get<std::string>(SUMO_ATTR_CHANGE_RIGHT, nullptr, ok), "");
    } else {
        conn.changeRight = SVC_UNSPECIFIED;
    }
    if (myChangeLefthand) {
        std::swap(conn.changeLeft, conn.changeRight);
    }
    conn.speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, nullptr, ok, NBEdge::UNSPECIFIED_SPEED);
    conn.friction = attrs.getOpt<double>(SUMO_ATTR_FRICTION, nullptr, ok, NBEdge::UNSPECIFIED_FRICTION);
    conn.customLength = attrs.getOpt<double>(SUMO_ATTR_LENGTH, nullptr, ok, NBEdge::UNSPECIFIED_LOADED_LENGTH);
    conn.customShape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, nullptr, ok, PositionVector::EMPTY);
    NBNetBuilder::transformCoordinates(conn.customShape, false, myLocation);
    conn.uncontrolled = attrs.getOpt<bool>(SUMO_ATTR_UNCONTROLLED, nullptr, ok, NBEdge::UNSPECIFIED_CONNECTION_UNCONTROLLED, false);
    if (conn.tlID != "") {
        conn.tlLinkIndex = attrs.get<int>(SUMO_ATTR_TLLINKINDEX, nullptr, ok);
        conn.tlLinkIndex2 = attrs.getOpt<int>(SUMO_ATTR_TLLINKINDEX2, nullptr, ok, -1);
    } else {
        conn.tlLinkIndex = NBConnection::InvalidTlIndex;
    }
    if ((int)from->lanes.size() <= fromLaneIdx) {
        WRITE_ERRORF(TL("Invalid lane index '%' for connection from '%'."), toString(fromLaneIdx), fromID);
        return;
    }
    from->lanes[fromLaneIdx]->connections.push_back(conn);
    myLastParameterised.push_back(&from->lanes[fromLaneIdx]->connections.back());

    // determine crossing priority and tlIndex
    if (myPedestrianCrossings.size() > 0) {
        if (from->func == SumoXMLEdgeFunc::WALKINGAREA && myEdges[conn.toEdgeID]->func == SumoXMLEdgeFunc::CROSSING) {
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
        } else if (from->func == SumoXMLEdgeFunc::CROSSING && myEdges[conn.toEdgeID]->func == SumoXMLEdgeFunc::WALKINGAREA) {
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
        if (from->func == SumoXMLEdgeFunc::WALKINGAREA) {
            std::map<std::string, WalkingAreaParsedCustomShape>::iterator it = myWACustomShapes.find(fromID);
            if (it != myWACustomShapes.end()) {
                if (to->func == SumoXMLEdgeFunc::NORMAL) {
                    // add target sidewalk as reference
                    it->second.toEdges.push_back(conn.toEdgeID);
                } else if (to->func == SumoXMLEdgeFunc::CROSSING) {
                    // add target crossing edges as reference
                    for (Crossing crossing : myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(fromID)]) {
                        if (conn.toEdgeID == crossing.edgeID) {
                            it->second.toCrossed.insert(it->second.toCrossed.end(), crossing.crossingEdges.begin(), crossing.crossingEdges.end());
                        }
                    }
                }
            }
        } else if (to->func == SumoXMLEdgeFunc::WALKINGAREA) {
            std::map<std::string, WalkingAreaParsedCustomShape>::iterator it = myWACustomShapes.find(conn.toEdgeID);
            if (it != myWACustomShapes.end()) {
                if (from->func == SumoXMLEdgeFunc::NORMAL) {
                    // add origin sidewalk as reference
                    it->second.fromEdges.push_back(fromID);
                } else if (from->func == SumoXMLEdgeFunc::CROSSING) {
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


NBLoadedSUMOTLDef*
NIImporter_SUMO::initTrafficLightLogic(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL) {
    if (currentTL) {
        WRITE_ERRORF(TL("Definition of tl-logic '%' was not finished."), currentTL->getID());
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
        WRITE_ERRORF(TL("Unknown traffic light type '%' for tlLogic '%'."), typeS, id);
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
        WRITE_ERROR(TL("found phase without tl-logic"));
        return;
    }
    const std::string& id = currentTL->getID();
    bool ok = true;
    std::string state = attrs.get<std::string>(SUMO_ATTR_STATE, id.c_str(), ok);
    SUMOTime duration = TIME2STEPS(attrs.get<double>(SUMO_ATTR_DURATION, id.c_str(), ok));
    if (duration < 0) {
        WRITE_ERRORF(TL("Phase duration for tl-logic '%/%' must be positive."), id, currentTL->getProgramID());
        return;
    }
    // if the traffic light is an actuated traffic light, try to get the minimum and maximum durations and ends
    std::vector<int> nextPhases = attrs.getOpt<std::vector<int> >(SUMO_ATTR_NEXT, id.c_str(), ok);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, nullptr, ok);
    // Specific from actuated
    const SUMOTime minDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MINDURATION, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    const SUMOTime maxDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MAXDURATION, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    const SUMOTime earliestEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_EARLIEST_END, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    const SUMOTime latestEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_LATEST_END, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    // specific von NEMA
    const SUMOTime vehExt = attrs.getOptSUMOTimeReporting(SUMO_ATTR_VEHICLEEXTENSION, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    const SUMOTime yellow = attrs.getOptSUMOTimeReporting(SUMO_ATTR_YELLOW, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    const SUMOTime red = attrs.getOptSUMOTimeReporting(SUMO_ATTR_RED, id.c_str(), ok, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
    if (ok) {
        currentTL->addPhase(duration, state, minDuration, maxDuration, earliestEnd, latestEnd, vehExt, yellow, red, nextPhases, name);
    }
}


GeoConvHelper*
NIImporter_SUMO::loadLocation(const SUMOSAXAttributes& attrs, bool setLoaded) {
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
        result->resolveAbstractProjection();
        if (setLoaded) {
            GeoConvHelper::setLoaded(*result);
        }
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
        WRITE_ERRORF(TL("Missing connection divider in prohibition attribute '%'"), attr);
        ok = false;
    }
    from = attr.substr(0, div);
    to = attr.substr(div + 2);
    // check whether the edges are known
    if (myEdges.count(from) == 0) {
        WRITE_ERRORF(TL("Unknown edge prohibition '%'"), from);
        ok = false;
    }
    if (myEdges.count(to) == 0) {
        WRITE_ERRORF(TL("Unknown edge prohibition '%'"), to);
        ok = false;
    }
}


void
NIImporter_SUMO::addRoundabout(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::vector<std::string>& edgeIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, nullptr, ok);
    if (ok) {
        myRoundabouts.push_back(edgeIDs);
    }
}


/****************************************************************************/
