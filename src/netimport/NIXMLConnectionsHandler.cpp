/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    NIXMLConnectionsHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Thu, 17 Oct 2002
///
// Importer for edge connections stored in XML
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include "NIImporter_SUMO.h"
#include "NIXMLConnectionsHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLConnectionsHandler::NIXMLConnectionsHandler(NBEdgeCont& ec, NBNodeCont& nc, NBTrafficLightLogicCont& tlc) :
    SUMOSAXHandler("xml-connection-description"),
    myEdgeCont(ec),
    myNodeCont(nc),
    myTLLogicCont(tlc),
    myHaveWarnedAboutDeprecatedLanes(false),
    myErrorMsgHandler(OptionsCont::getOptions().getBool("ignore-errors.connections") ?
                      MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
    myLocation(nullptr),
    myLastParameterised(nullptr) {
}


NIXMLConnectionsHandler::~NIXMLConnectionsHandler() {
    delete myLocation;
}


void
NIXMLConnectionsHandler::myStartElement(int element,
                                        const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_CONNECTIONS:
            // infer location for legacy networks that don't have location information
            myLocation = GeoConvHelper::getLoadedPlain(getFileName(), ".con.xml");
            break;
        case SUMO_TAG_LOCATION:
            delete myLocation;
            myLocation = NIImporter_SUMO::loadLocation(attrs, false);
            break;
        case SUMO_TAG_DEL:
            delConnection(attrs);
            break;
        case SUMO_TAG_CONNECTION:
            parseConnection(attrs);
            break;
        case SUMO_TAG_PROHIBITION:
            addProhibition(attrs);
            break;
        case SUMO_TAG_CROSSING:
            addCrossing(attrs);
            break;
        case SUMO_TAG_WALKINGAREA:
            addWalkingArea(attrs);
            break;
        case SUMO_TAG_PARAM:
            if (myLastParameterised != nullptr) {
                bool ok = true;
                const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
                // circumventing empty string test
                const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
                myLastParameterised->setParameter(key, val);
            }
            break;
        default:
            break;
    }
}


void
NIXMLConnectionsHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_CONNECTION:
        case SUMO_TAG_CROSSING:
            myLastParameterised = nullptr;
            break;
        default:
            break;
    }
}



NBConnection
NIXMLConnectionsHandler::parseConnectionDef(const std::string& defRole, const std::string& def) {
    // split from/to
    const std::string::size_type div = def.find("->");
    if (div == std::string::npos) {
        myErrorMsgHandler->inform("Missing connection divider in " + defRole + " '" + def + "'");
        return NBConnection::InvalidConnection;
    }
    std::string fromDef = def.substr(0, div);
    std::string toDef = def.substr(div + 2);

    // retrieve them now
    NBEdge* fromE = myEdgeCont.retrieve(fromDef);
    NBEdge* toE = myEdgeCont.retrieve(toDef);
    // check
    if (fromE == nullptr) {
        myErrorMsgHandler->inform("Could not find edge '" + fromDef + "' in " + defRole + " '" + def + "'");
        return NBConnection::InvalidConnection;
    }
    if (toE == nullptr) {
        myErrorMsgHandler->inform("Could not find edge '" + toDef + "' in " + defRole + " '" + def + "'");
        return NBConnection::InvalidConnection;
    }
    return NBConnection(fromE, toE);
}


void
NIXMLConnectionsHandler::parseLaneBound(const SUMOSAXAttributes& attrs, NBEdge* from, NBEdge* to) {
    if (to == nullptr) {
        // do nothing if it's a dead end
        return;
    }
    bool ok = true;
    // get the begin and the end lane
    int fromLane;
    int toLane;
    try {
        if (!parseLaneInfo(attrs, from, to, &fromLane, &toLane)) {
            return;
        }
        if (fromLane < 0) {
            myErrorMsgHandler->informf("Invalid value '%' for " + toString(SUMO_ATTR_FROM_LANE) +
                                       " in connection from '%' to '%'.", fromLane, from->getID(), to->getID());
            return;
        }
        if (toLane < 0) {
            myErrorMsgHandler->informf("Invalid value '%' for " + toString(SUMO_ATTR_TO_LANE) +
                                       " in connection from '%' to '%'.", toLane, from->getID(), to->getID());
            return;
        }

        NBEdge::Connection defaultCon(fromLane, to, toLane);
        if (from->getStep() == NBEdge::EdgeBuildingStep::LANES2LANES_USER) {
            // maybe we are patching an existing connection
            std::vector<NBEdge::Connection> existing = from->getConnectionsFromLane(fromLane, to, toLane);
            if (existing.size() > 0) {
                assert(existing.size() == 1);
                defaultCon = existing.front();
                // remove the original so we can insert the replacement
                from->removeFromConnections(defaultCon);
            } else {
                from->getToNode()->invalidateTLS(myTLLogicCont, true, false);
            }
        }
        if (OptionsCont::getOptions().isSet("default.connection.cont-pos")) {
            defaultCon.contPos = OptionsCont::getOptions().getFloat("default.connection.cont-pos");
        }
        const bool mayDefinitelyPass = attrs.getOpt<bool>(SUMO_ATTR_PASS, nullptr, ok, defaultCon.mayDefinitelyPass);
        KeepClear keepClear = defaultCon.keepClear;
        if (attrs.hasAttribute(SUMO_ATTR_KEEP_CLEAR)) {
            keepClear = attrs.get<bool>(SUMO_ATTR_KEEP_CLEAR, nullptr, ok) ? KEEPCLEAR_TRUE : KEEPCLEAR_FALSE;
        }
        const double contPos = attrs.getOpt<double>(SUMO_ATTR_CONTPOS, nullptr, ok, defaultCon.contPos);
        const double visibility = attrs.getOpt<double>(SUMO_ATTR_VISIBILITY_DISTANCE, nullptr, ok, defaultCon.visibility);
        const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, nullptr, ok, defaultCon.speed);
        const double friction = attrs.getOpt<double>(SUMO_ATTR_FRICTION, nullptr, ok, defaultCon.friction);
        const double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, nullptr, ok, defaultCon.customLength);
        const bool uncontrolled = attrs.getOpt<bool>(SUMO_ATTR_UNCONTROLLED, nullptr, ok, defaultCon.uncontrolled);
        const bool indirectLeft = attrs.getOpt<bool>(SUMO_ATTR_INDIRECT, nullptr, ok, false);
        const std::string edgeType = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, nullptr, ok, "");
        PositionVector customShape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, nullptr, ok, defaultCon.customShape);
        std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, nullptr, ok, "");
        std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, nullptr, ok, "");
        SVCPermissions permissions;
        if (allow == "" && disallow == "") {
            permissions = SVC_UNSPECIFIED;
        } else {
            permissions = parseVehicleClasses(attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, nullptr, ok, ""), attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, nullptr, ok, ""));
        }
        SVCPermissions changeLeft = SVC_UNSPECIFIED;
        SVCPermissions changeRight = SVC_UNSPECIFIED;
        if (attrs.hasAttribute(SUMO_ATTR_CHANGE_LEFT)) {
            changeLeft = parseVehicleClasses(attrs.get<std::string>(SUMO_ATTR_CHANGE_LEFT, nullptr, ok), "");
        }
        if (attrs.hasAttribute(SUMO_ATTR_CHANGE_RIGHT)) {
            changeRight = parseVehicleClasses(attrs.get<std::string>(SUMO_ATTR_CHANGE_RIGHT, nullptr, ok), "");
        }
        if (attrs.hasAttribute(SUMO_ATTR_SHAPE) && !NBNetBuilder::transformCoordinates(customShape, true, myLocation)) {
            WRITE_ERRORF(TL("Unable to project shape for connection from edge '%' to edge '%'."), from->getID(), to->getID());
        }
        if (!ok) {
            return;
        }
        if (!from->addLane2LaneConnection(fromLane, to, toLane, NBEdge::Lane2LaneInfoType::USER, true, mayDefinitelyPass,
                                          keepClear, contPos, visibility, speed, friction, length, customShape, uncontrolled, permissions, indirectLeft, edgeType, changeLeft, changeRight)) {
            if (OptionsCont::getOptions().getBool("show-errors.connections-first-try")) {
                WRITE_WARNINGF(TL("Could not set loaded connection from lane '%' to lane '%'."), from->getLaneID(fromLane), to->getLaneID(toLane));
            }
            // set as to be re-applied after network processing
            myEdgeCont.addPostProcessConnection(from->getID(), fromLane, to->getID(), toLane, mayDefinitelyPass, keepClear, contPos, visibility,
                                                speed, friction, length, customShape, uncontrolled, false, permissions, indirectLeft, edgeType, changeLeft, changeRight);
        }
    } catch (NumberFormatException&) {
        myErrorMsgHandler->inform("At least one of the defined lanes was not numeric");
    }
}

bool
NIXMLConnectionsHandler::parseLaneInfo(const SUMOSAXAttributes& attributes, NBEdge* fromEdge, NBEdge* toEdge,
                                       int* fromLane, int* toLane) {
    if (attributes.hasAttribute(SUMO_ATTR_LANE)) {
        return parseDeprecatedLaneDefinition(attributes, fromEdge, toEdge, fromLane, toLane);
    } else {
        return parseLaneDefinition(attributes, fromLane, toLane);
    }
}


inline bool
NIXMLConnectionsHandler::parseDeprecatedLaneDefinition(const SUMOSAXAttributes& attributes,
        NBEdge* from, NBEdge* to,
        int* fromLane, int* toLane) {
    bool ok = true;
    if (!myHaveWarnedAboutDeprecatedLanes) {
        myHaveWarnedAboutDeprecatedLanes = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_LANE) + "' is deprecated, please use '" +
                      toString(SUMO_ATTR_FROM_LANE) + "' and '" + toString(SUMO_ATTR_TO_LANE) +
                      "' instead.");
    }

    std::string laneConn = attributes.get<std::string>(SUMO_ATTR_LANE, nullptr, ok);
    StringTokenizer st(laneConn, ':');
    if (!ok || st.size() != 2) {
        myErrorMsgHandler->inform("Invalid lane to lane connection from '" +
                                  from->getID() + "' to '" + to->getID() + "'.");
        return false; // There was an error.
    }

    *fromLane = StringUtils::toIntSecure(st.next(), -1);
    *toLane = StringUtils::toIntSecure(st.next(), -1);

    return true; // We succeeded.
}


inline bool
NIXMLConnectionsHandler::parseLaneDefinition(const SUMOSAXAttributes& attributes,
        int* fromLane,
        int* toLane) {
    bool ok = true;
    *fromLane = attributes.get<int>(SUMO_ATTR_FROM_LANE, nullptr, ok);
    *toLane = attributes.get<int>(SUMO_ATTR_TO_LANE, nullptr, ok);
    return ok;
}

void
NIXMLConnectionsHandler::delConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string from = attrs.get<std::string>(SUMO_ATTR_FROM, nullptr, ok);
    std::string to = attrs.get<std::string>(SUMO_ATTR_TO, nullptr, ok);
    if (!ok) {
        return;
    }
    // these connections were removed when the edge was deleted
    if (myEdgeCont.wasRemoved(from) || myEdgeCont.wasRemoved(to)) {
        return;
    }
    NBEdge* fromEdge = myEdgeCont.retrieve(from);
    NBEdge* toEdge = myEdgeCont.retrieve(to);
    if (fromEdge == nullptr) {
        myErrorMsgHandler->informf("The connection-source edge '%' to reset is not known.", from);
        return;
    }
    if (toEdge == nullptr) {
        myErrorMsgHandler->informf("The connection-destination edge '%' to reset is not known.", to);
        return;
    }
    if (!fromEdge->isConnectedTo(toEdge) && fromEdge->getStep() >= NBEdge::EdgeBuildingStep::EDGE2EDGES) {
        WRITE_WARNINGF(TL("Target edge '%' is not connected with '%'; the connection cannot be reset."), toEdge->getID(), fromEdge->getID());
        return;
    }
    int fromLane = -1; // Assume all lanes are to be reset.
    int toLane = -1;
    if (attrs.hasAttribute(SUMO_ATTR_LANE)
            || attrs.hasAttribute(SUMO_ATTR_FROM_LANE)
            || attrs.hasAttribute(SUMO_ATTR_TO_LANE)) {
        if (!parseLaneInfo(attrs, fromEdge, toEdge, &fromLane, &toLane)) {
            return;
        }
        // we could be trying to reset a connection loaded from a sumo net and which has become obsolete.
        // In this case it's ok to encounter invalid lance indices
        if (!fromEdge->hasConnectionTo(toEdge, toLane) && fromEdge->getStep() >= NBEdge::EdgeBuildingStep::LANES2EDGES) {
            WRITE_WARNINGF(TL("Edge '%' has no connection to lane '%'; the connection cannot be reset."), fromEdge->getID(), toEdge->getLaneID(toLane));
        }
    }
    fromEdge->removeFromConnections(toEdge, fromLane, toLane, true);
}

void
NIXMLConnectionsHandler::parseConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string from = attrs.get<std::string>(SUMO_ATTR_FROM, "connection", ok);
    std::string to = attrs.getOpt<std::string>(SUMO_ATTR_TO, "connection", ok, "");
    if (!ok || myEdgeCont.wasIgnored(from) || myEdgeCont.wasIgnored(to)) {
        return;
    }
    // extract edges
    NBEdge* fromEdge = myEdgeCont.retrieve(from);
    NBEdge* toEdge = to.length() != 0 ? myEdgeCont.retrieve(to) : nullptr;
    // check whether they are valid
    if (fromEdge == nullptr) {
        myErrorMsgHandler->inform("The connection-source edge '" + from + "' is not known.");
        return;
    }
    if (toEdge == nullptr && to.length() != 0) {
        myErrorMsgHandler->inform("The connection-destination edge '" + to + "' is not known.");
        return;
    }
    // parse optional lane information
    if (attrs.hasAttribute(SUMO_ATTR_LANE) || attrs.hasAttribute(SUMO_ATTR_FROM_LANE) || attrs.hasAttribute(SUMO_ATTR_TO_LANE)) {
        parseLaneBound(attrs, fromEdge, toEdge);
    } else {
        fromEdge->addEdge2EdgeConnection(toEdge);
        fromEdge->getToNode()->invalidateTLS(myTLLogicCont, true, false);
        if (attrs.hasAttribute(SUMO_ATTR_PASS)
                || attrs.hasAttribute(SUMO_ATTR_KEEP_CLEAR)
                || attrs.hasAttribute(SUMO_ATTR_CONTPOS)
                || attrs.hasAttribute(SUMO_ATTR_VISIBILITY_DISTANCE)
                || attrs.hasAttribute(SUMO_ATTR_SPEED)
                || attrs.hasAttribute(SUMO_ATTR_LENGTH)
                || attrs.hasAttribute(SUMO_ATTR_UNCONTROLLED)
                || attrs.hasAttribute(SUMO_ATTR_SHAPE)
                || attrs.hasAttribute(SUMO_ATTR_ALLOW)
                || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
            WRITE_ERROR("No additional connection attributes are permitted in connection from edge '" + fromEdge->getID() + "' unless '"
                        + toString(SUMO_ATTR_FROM_LANE) + "' and '" + toString(SUMO_ATTR_TO_LANE) + "' are set.");
        }
    }
}

void
NIXMLConnectionsHandler::addCrossing(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    EdgeVector edges;
    const std::string nodeID = attrs.get<std::string>(SUMO_ATTR_NODE, nullptr, ok);
    double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, nodeID.c_str(), ok, NBEdge::UNSPECIFIED_WIDTH, true);
    const bool discard = attrs.getOpt<bool>(SUMO_ATTR_DISCARD, nodeID.c_str(), ok, false, true);
    int tlIndex = attrs.getOpt<int>(SUMO_ATTR_TLLINKINDEX, nullptr, ok, -1);
    int tlIndex2 = attrs.getOpt<int>(SUMO_ATTR_TLLINKINDEX2, nullptr, ok, -1);
    NBNode* node = myNodeCont.retrieve(nodeID);
    if (node == nullptr) {
        if (!discard && myNodeCont.wasRemoved(nodeID)) {
            WRITE_ERRORF(TL("Node '%' in crossing is not known."), nodeID);
        }
        return;
    }
    if (!attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        if (discard) {
            node->discardAllCrossings(true);
            return;
        } else {
            WRITE_ERRORF(TL("No edges specified for crossing at node '%'."), nodeID);
            return;
        }
    }
    for (const std::string& id : attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, nodeID.c_str(), ok)) {
        NBEdge* edge = myEdgeCont.retrieve(id);
        if (edge == nullptr) {
            if (!(discard && myEdgeCont.wasRemoved(id))) {
                WRITE_ERRORF(TL("Edge '%' for crossing at node '%' is not known."), id, nodeID);
                return;
            } else {
                edge = myEdgeCont.retrieve(id, true);
            }
        } else {
            if (edge->getToNode() != node && edge->getFromNode() != node) {
                if (!discard) {
                    WRITE_ERRORF(TL("Edge '%' does not touch node '%'."), id, nodeID);
                    return;
                }
            }
        }
        edges.push_back(edge);
    }
    if (!ok) {
        return;
    }
    bool priority = attrs.getOpt<bool>(SUMO_ATTR_PRIORITY, nodeID.c_str(), ok, node->isTLControlled(), true);
    if (node->isTLControlled() && !priority) {
        // traffic_light nodes should always have priority crossings
        WRITE_WARNINGF(TL("Crossing at controlled node '%' must be prioritized"), nodeID);
        priority = true;
    }
    PositionVector customShape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, nullptr, ok, PositionVector::EMPTY);
    if (!NBNetBuilder::transformCoordinates(customShape, true, myLocation)) {
        WRITE_ERRORF(TL("Unable to project shape for crossing at node '%'."), node->getID());
    }
    if (discard) {
        node->removeCrossing(edges);
    } else {
        if (node->checkCrossingDuplicated(edges)) {
            // possibly a diff
            NBNode::Crossing* existing =  node->getCrossing(edges);
            if (!(
                        (attrs.hasAttribute(SUMO_ATTR_WIDTH) && width != existing->width)
                        || (attrs.hasAttribute(SUMO_ATTR_TLLINKINDEX) && tlIndex != existing->customTLIndex)
                        || (attrs.hasAttribute(SUMO_ATTR_TLLINKINDEX2) && tlIndex2 != existing->customTLIndex2)
                        || (attrs.hasAttribute(SUMO_ATTR_PRIORITY) && priority != existing->priority))) {
                WRITE_ERRORF(TL("Crossing with edges '%' already exists at node '%'."), toString(edges), node->getID());
                return;
            } else {
                // replace existing, keep old attributes
                if (!attrs.hasAttribute(SUMO_ATTR_WIDTH)) {
                    width = existing->width;
                }
                if (!attrs.hasAttribute(SUMO_ATTR_TLLINKINDEX)) {
                    tlIndex = existing->customTLIndex;
                }
                if (!attrs.hasAttribute(SUMO_ATTR_TLLINKINDEX2)) {
                    tlIndex2 = existing->customTLIndex2;
                }
                if (!attrs.hasAttribute(SUMO_ATTR_PRIORITY)) {
                    priority = existing->priority;
                }
                node->removeCrossing(edges);
            }
        }
        NBNode::Crossing* c = node->addCrossing(edges, width, priority, tlIndex, tlIndex2, customShape);
        myLastParameterised = c;
    }
}


void
NIXMLConnectionsHandler::addWalkingArea(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    NBNode* node = nullptr;
    EdgeVector edges;
    const std::string nodeID = attrs.get<std::string>(SUMO_ATTR_NODE, nullptr, ok);
    std::vector<std::string> edgeIDs;
    if (!attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        WRITE_ERRORF(TL("No edges specified for walkingArea at node '%'."), nodeID);
        return;
    }
    for (const std::string& id : attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, nodeID.c_str(), ok)) {
        NBEdge* edge = myEdgeCont.retrieve(id);
        if (edge == nullptr) {
            WRITE_ERRORF(TL("Edge '%' for walkingArea at node '%' is not known."), id, nodeID);
            return;
        }
        if (node == nullptr) {
            if (edge->getToNode()->getID() == nodeID) {
                node = edge->getToNode();
            } else if (edge->getFromNode()->getID() == nodeID) {
                node = edge->getFromNode();
            } else {
                WRITE_ERRORF(TL("Edge '%' does not touch node '%'."), id, nodeID);
                return;
            }
        } else {
            if (edge->getToNode() != node && edge->getFromNode() != node) {
                WRITE_ERRORF(TL("Edge '%' does not touch node '%'."), id, nodeID);
                return;
            }
        }
        edges.push_back(edge);
    }
    if (!ok) {
        return;
    }
    PositionVector customShape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, nullptr, ok, PositionVector::EMPTY);
    double customWidth = attrs.getOpt<double>(SUMO_ATTR_WIDTH, nullptr, ok, NBEdge::UNSPECIFIED_WIDTH);
    if (!NBNetBuilder::transformCoordinates(customShape, true, myLocation)) {
        WRITE_ERRORF(TL("Unable to project shape for walkingArea at node '%'."), node->getID());
    }
    node->addWalkingAreaShape(edges, customShape, customWidth);
}

void
NIXMLConnectionsHandler::addProhibition(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string prohibitor = attrs.getOpt<std::string>(SUMO_ATTR_PROHIBITOR, nullptr, ok, "");
    std::string prohibited = attrs.getOpt<std::string>(SUMO_ATTR_PROHIBITED, nullptr, ok, "");
    if (!ok) {
        return;
    }
    NBConnection prohibitorC = parseConnectionDef("prohibitor", prohibitor);
    NBConnection prohibitedC = parseConnectionDef("prohibited", prohibited);
    if (prohibitorC == NBConnection::InvalidConnection || prohibitedC == NBConnection::InvalidConnection) {
        // something failed
        return;
    }
    NBNode* n = prohibitorC.getFrom()->getToNode();
    n->addSortedLinkFoes(prohibitorC, prohibitedC);
}

/****************************************************************************/
