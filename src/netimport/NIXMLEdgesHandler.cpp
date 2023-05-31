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
/// @file    NIXMLEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @author  Laura Bieker
/// @author  Leonhard Luecken
/// @date    Tue, 20 Nov 2001
///
// Importer for network edges stored in XML
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include "NIXMLNodesHandler.h"
#include "NIXMLEdgesHandler.h"
#include "NIImporter_SUMO.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLEdgesHandler::NIXMLEdgesHandler(NBNodeCont& nc,
                                     NBEdgeCont& ec,
                                     NBTypeCont& tc,
                                     NBDistrictCont& dc,
                                     NBTrafficLightLogicCont& tlc,
                                     OptionsCont& options) :
    SUMOSAXHandler("xml-edges - file"),
    myOptions(options),
    myNodeCont(nc),
    myEdgeCont(ec),
    myTypeCont(tc),
    myDistrictCont(dc),
    myTLLogicCont(tlc),
    myCurrentEdge(nullptr),
    myCurrentLaneIndex(-1),
    myHaveReportedAboutOverwriting(false),
    myHaveReportedAboutTypeOverride(false),
    myHaveWarnedAboutDeprecatedLaneId(false),
    myKeepEdgeShape(!options.getBool("plain.extend-edge-shape")) {
}


NIXMLEdgesHandler::~NIXMLEdgesHandler() {
    delete myLocation;
}


void
NIXMLEdgesHandler::myStartElement(int element,
                                  const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_VIEWSETTINGS_EDGES:
            // infer location for legacy networks that don't have location information
            myLocation = GeoConvHelper::getLoadedPlain(getFileName());
            break;
        case SUMO_TAG_LOCATION:
            delete myLocation;
            myLocation = NIImporter_SUMO::loadLocation(attrs, false);
            break;
        case SUMO_TAG_EDGE:
            addEdge(attrs);
            break;
        case SUMO_TAG_LANE:
            addLane(attrs);
            break;
        case SUMO_TAG_NEIGH:
            myCurrentEdge->getLaneStruct((int)myCurrentEdge->getNumLanes() - 1).oppositeID = attrs.getString(SUMO_ATTR_LANE);
            break;
        case SUMO_TAG_SPLIT:
            addSplit(attrs);
            break;
        case SUMO_TAG_DEL:
            deleteEdge(attrs);
            break;
        case SUMO_TAG_ROUNDABOUT:
            addRoundabout(attrs);
            break;
        case SUMO_TAG_PARAM:
            if (myLastParameterised.size() != 0 && myCurrentEdge != nullptr) {
                bool ok = true;
                const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
                // circumventing empty string test
                const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
                myLastParameterised.back()->setParameter(key, val);
            }
            break;
        case SUMO_TAG_STOPOFFSET: {
            bool ok = true;
            const StopOffset stopOffset(attrs, ok);
            if (!ok) {
                std::stringstream ss;
                ss << "(Error encountered at lane " << myCurrentLaneIndex << " of edge '" << myCurrentID << "' while parsing stopOffsets.)";
                WRITE_ERROR(ss.str());
            } else {
                if (myCurrentEdge->getLaneStopOffset(myCurrentLaneIndex).isDefined()) {
                    std::stringstream ss;
                    ss << "Duplicate definition of stopOffset for ";
                    if (myCurrentLaneIndex != -1) {
                        ss << "lane " << myCurrentLaneIndex << " on ";
                    }
                    ss << "edge " << myCurrentEdge->getID() << ". Ignoring duplicate specification.";
                    WRITE_WARNING(ss.str());
                } else if ((stopOffset.getOffset() > myCurrentEdge->getLength()) || (stopOffset.getOffset() < 0)) {
                    std::stringstream ss;
                    ss << "Ignoring invalid stopOffset for ";
                    if (myCurrentLaneIndex != -1) {
                        ss << "lane " << myCurrentLaneIndex << " on ";
                    }
                    ss << "edge " << myCurrentEdge->getID();
                    if (stopOffset.getOffset() > myCurrentEdge->getLength()) {
                        ss << " (offset larger than the edge length).";
                    } else {
                        ss << " (negative offset).";
                    }
                    WRITE_WARNING(ss.str());
                } else {
                    myCurrentEdge->setEdgeStopOffset(myCurrentLaneIndex, stopOffset);
                }
            }
        }
        break;
        default:
            break;
    }
}


void
NIXMLEdgesHandler::addEdge(const SUMOSAXAttributes& attrs) {
    myIsUpdate = false;
    bool ok = true;
    // initialise the edge
    myCurrentEdge = nullptr;
    mySplits.clear();
    // get the id, report an error if not given or empty...
    myCurrentID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    myCurrentEdge = myEdgeCont.retrieve(myCurrentID);
    // check deprecated (unused) attributes
    // use default values, first
    myCurrentType = myOptions.getString("default.type");
    myCurrentPriority = myTypeCont.getEdgeTypePriority(myCurrentType);
    myCurrentLaneNo = myTypeCont.getEdgeTypeNumLanes(myCurrentType);
    myCurrentEndOffset = NBEdge::UNSPECIFIED_OFFSET;
    if (myCurrentEdge != nullptr) {
        // update existing edge. only update lane-specific settings when explicitly requested
        myIsUpdate = true;
        myCurrentSpeed = NBEdge::UNSPECIFIED_SPEED;
        myCurrentFriction = NBEdge::UNSPECIFIED_FRICTION;
        myPermissions = SVC_UNSPECIFIED;
        myCurrentWidth = NBEdge::UNSPECIFIED_WIDTH;
        myCurrentType = myCurrentEdge->getTypeID();
        myLanesSpread = SUMOXMLDefinitions::LaneSpreadFunctions.get(myOptions.getString("default.spreadtype"));
    } else {
        // this is a completely new edge. get the type specific defaults
        myCurrentSpeed = myTypeCont.getEdgeTypeSpeed(myCurrentType);
        myCurrentFriction = myTypeCont.getEdgeTypeFriction(myCurrentType);
        myPermissions = myTypeCont.getEdgeTypePermissions(myCurrentType);
        myCurrentWidth = myTypeCont.getEdgeTypeWidth(myCurrentType);
        myLanesSpread = myTypeCont.getEdgeTypeSpreadType(myCurrentType);
    }
    myShape = PositionVector();
    myLength = NBEdge::UNSPECIFIED_LOADED_LENGTH;
    myCurrentStreetName = "";
    myReinitKeepEdgeShape = false;
    mySidewalkWidth = NBEdge::UNSPECIFIED_WIDTH;
    myBikeLaneWidth = NBEdge::UNSPECIFIED_WIDTH;
    // check whether a type's values shall be used
    if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        myCurrentType = attrs.get<std::string>(SUMO_ATTR_TYPE, myCurrentID.c_str(), ok);
        if (!ok) {
            return;
        }
        if (!myTypeCont.knows(myCurrentType) && !myOptions.getBool("ignore-errors.edge-type")) {
            WRITE_ERRORF("Type '%' used by edge '%' was not defined (ignore with option --ignore-errors.edge-type).", myCurrentType, myCurrentID);
            return;
        }
        myCurrentSpeed = myTypeCont.getEdgeTypeSpeed(myCurrentType);
        myCurrentPriority = myTypeCont.getEdgeTypePriority(myCurrentType);
        myCurrentLaneNo = myTypeCont.getEdgeTypeNumLanes(myCurrentType);
        myPermissions = myTypeCont.getEdgeTypePermissions(myCurrentType);
        myCurrentWidth = myTypeCont.getEdgeTypeWidth(myCurrentType);
        myLanesSpread = myTypeCont.getEdgeTypeSpreadType(myCurrentType);
        mySidewalkWidth = myTypeCont.getEdgeTypeSidewalkWidth(myCurrentType);
        myBikeLaneWidth = myTypeCont.getEdgeTypeBikeLaneWidth(myCurrentType);
    }
    // use values from the edge to overwrite if existing, then
    if (myIsUpdate) {
        if (!myHaveReportedAboutOverwriting) {
            WRITE_MESSAGEF(TL("Duplicate edge id occurred ('%'); assuming overwriting is wished."), myCurrentID);
            myHaveReportedAboutOverwriting = true;
        }
        if (attrs.hasAttribute(SUMO_ATTR_TYPE) && myCurrentType != myCurrentEdge->getTypeID()) {
            if (!myHaveReportedAboutTypeOverride) {
                WRITE_MESSAGEF(TL("Edge '%' changed it's type; assuming type override is wished."), myCurrentID);
                myHaveReportedAboutTypeOverride = true;
            }
        }
        if (attrs.getOpt<bool>(SUMO_ATTR_REMOVE, myCurrentID.c_str(), ok, false)) {
            myEdgeCont.erase(myDistrictCont, myCurrentEdge);
            myCurrentEdge = nullptr;
            return;
        }
        myCurrentPriority = myCurrentEdge->getPriority();
        myCurrentLaneNo = myCurrentEdge->getNumLanes();
        if (!myCurrentEdge->hasDefaultGeometry()) {
            myShape = myCurrentEdge->getGeometry();
            myReinitKeepEdgeShape = true;
        }
        myLanesSpread = myCurrentEdge->getLaneSpreadFunction();
        if (myCurrentEdge->hasLoadedLength()) {
            myLength = myCurrentEdge->getLoadedLength();
        }
        myCurrentStreetName = myCurrentEdge->getStreetName();
    }
    // speed, priority and the number of lanes have now default values;
    // try to read the real values from the file
    if (attrs.hasAttribute(SUMO_ATTR_SPEED)) {
        myCurrentSpeed = attrs.get<double>(SUMO_ATTR_SPEED, myCurrentID.c_str(), ok);
    }
    if (myOptions.getBool("speed-in-kmh") && myCurrentSpeed != NBEdge::UNSPECIFIED_SPEED) {
        myCurrentSpeed = myCurrentSpeed / (double) 3.6;
    }
    // try to read the friction value from file
    if (attrs.hasAttribute(SUMO_ATTR_FRICTION)) {
        myCurrentFriction = attrs.get<double>(SUMO_ATTR_FRICTION, myCurrentID.c_str(), ok);
    }
    // try to get the number of lanes
    if (attrs.hasAttribute(SUMO_ATTR_NUMLANES)) {
        myCurrentLaneNo = attrs.get<int>(SUMO_ATTR_NUMLANES, myCurrentID.c_str(), ok);
    }
    // try to get the priority
    if (attrs.hasAttribute(SUMO_ATTR_PRIORITY)) {
        myCurrentPriority = attrs.get<int>(SUMO_ATTR_PRIORITY, myCurrentID.c_str(), ok);
    }
    // try to get the width
    if (attrs.hasAttribute(SUMO_ATTR_WIDTH)) {
        myCurrentWidth = attrs.get<double>(SUMO_ATTR_WIDTH, myCurrentID.c_str(), ok);
    }
    // try to get the offset of the stop line from the intersection
    if (attrs.hasAttribute(SUMO_ATTR_ENDOFFSET)) {
        myCurrentEndOffset = attrs.get<double>(SUMO_ATTR_ENDOFFSET, myCurrentID.c_str(), ok);
    }
    // try to get the street name
    if (attrs.hasAttribute(SUMO_ATTR_NAME)) {
        myCurrentStreetName = attrs.get<std::string>(SUMO_ATTR_NAME, myCurrentID.c_str(), ok);
        if (myCurrentStreetName != "" && myOptions.isDefault("output.street-names")) {
            myOptions.set("output.street-names", "true");
        }
    }

    // try to get the allowed/disallowed classes
    if (attrs.hasAttribute(SUMO_ATTR_ALLOW) || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
        std::string allowS = attrs.hasAttribute(SUMO_ATTR_ALLOW) ? attrs.getStringSecure(SUMO_ATTR_ALLOW, "") : "";
        std::string disallowS = attrs.hasAttribute(SUMO_ATTR_DISALLOW) ? attrs.getStringSecure(SUMO_ATTR_DISALLOW, "") : "";
        // XXX matter of interpretation: should updated permissions replace or extend previously set permissions?
        myPermissions = parseVehicleClasses(allowS, disallowS);
    }
    // try to set the nodes
    if (!setNodes(attrs)) {
        // return if this failed
        myCurrentEdge = nullptr;
        return;
    }
    // try to get the shape
    myShape = tryGetShape(attrs);
    // try to get the spread type
    myLanesSpread = tryGetLaneSpread(attrs);
    // try to get the length
    myLength = attrs.getOpt<double>(SUMO_ATTR_LENGTH, myCurrentID.c_str(), ok, myLength);
    // try to get the sidewalkWidth
    mySidewalkWidth = attrs.getOpt<double>(SUMO_ATTR_SIDEWALKWIDTH, myCurrentID.c_str(), ok, mySidewalkWidth);
    // try to get the bikeLaneWidth
    myBikeLaneWidth = attrs.getOpt<double>(SUMO_ATTR_BIKELANEWIDTH, myCurrentID.c_str(), ok, myBikeLaneWidth);
    // insert the parsed edge into the edges map
    if (!ok) {
        myCurrentEdge = nullptr;
        return;
    }
    if (myFromNode == myToNode) {
        // this might as well be an error. We make this a warning mostly for
        // backward compatibility
        WRITE_WARNINGF(TL("Ignoring self-looped edge '%' at junction '%'"), myCurrentID, myFromNode->getID());
        myCurrentEdge = nullptr;
        return;
    }
    // check whether a previously defined edge shall be overwritten
    const bool applyLaneType = myCurrentEdge == nullptr;
    if (myCurrentEdge != nullptr) {
        myCurrentEdge->reinit(myFromNode, myToNode, myCurrentType, myCurrentSpeed, myCurrentFriction,
                              myCurrentLaneNo, myCurrentPriority, myShape,
                              myCurrentWidth, myCurrentEndOffset,
                              myCurrentStreetName, myLanesSpread,
                              myReinitKeepEdgeShape);
    } else {
        // the edge must be allocated in dependence to whether a shape is given
        if (myShape.size() == 0) {
            myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed, myCurrentFriction,
                                       myCurrentLaneNo, myCurrentPriority, myCurrentWidth, myCurrentEndOffset,
                                       myLanesSpread, myCurrentStreetName);
        } else {
            myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed, myCurrentFriction,
                                       myCurrentLaneNo, myCurrentPriority, myCurrentWidth, myCurrentEndOffset,
                                       myShape, myLanesSpread, myCurrentStreetName, "",
                                       myKeepEdgeShape);
        }
    }
    myCurrentEdge->setLoadedLength(myLength);
    if (myPermissions != SVC_UNSPECIFIED) {
        myCurrentEdge->setPermissions(myPermissions);
    }
    // apply laneType if given
    if (applyLaneType && myCurrentType != "" && myTypeCont.knows(myCurrentType)) {
        const NBTypeCont::EdgeTypeDefinition* eType = myTypeCont.getEdgeType(myCurrentType);
        if (eType->needsLaneType()) {
            int lane = 0;
            for (const NBTypeCont::LaneTypeDefinition& laneType : eType->laneTypeDefinitions) {
                if (lane >= myCurrentLaneNo) {
                    break;
                }
                if (laneType.attrs.count(SUMO_ATTR_SPEED) > 0) {
                    myCurrentEdge->setSpeed(lane, laneType.speed);
                }
                if (laneType.attrs.count(SUMO_ATTR_FRICTION) > 0) {
                    myCurrentEdge->setFriction(lane, laneType.friction);
                }
                if (laneType.attrs.count(SUMO_ATTR_DISALLOW) > 0 || laneType.attrs.count(SUMO_ATTR_ALLOW) > 0) {
                    myCurrentEdge->setPermissions(laneType.permissions, lane);
                }
                if (laneType.attrs.count(SUMO_ATTR_WIDTH) > 0) {
                    myCurrentEdge->setLaneWidth(lane, laneType.width);
                }
                lane++;
            }
        }
    }
    // try to get the kilometrage/mileage
    myCurrentEdge->setDistance(attrs.getOpt<double>(SUMO_ATTR_DISTANCE, myCurrentID.c_str(), ok, myCurrentEdge->getDistance()));
    // preserve bidi edge (only as boo, the actual edge will be recomputed)
    const std::string bidi = attrs.getOpt<std::string>(SUMO_ATTR_BIDI, myCurrentID.c_str(), ok, "");
    myCurrentEdge->setBidi(myCurrentEdge->getBidiEdge() != nullptr || bidi != "");

    myLastParameterised.push_back(myCurrentEdge);
}


void
NIXMLEdgesHandler::addLane(const SUMOSAXAttributes& attrs) {
    if (myCurrentEdge == nullptr) {
        if (!OptionsCont::getOptions().isInStringVector("remove-edges.explicit", myCurrentID)) {
            WRITE_ERRORF("Additional lane information could not be set - the edge with id '%s' is not known.", myCurrentID);
        }
        return;
    }
    bool ok = true;
    int lane;
    if (attrs.hasAttribute(SUMO_ATTR_ID)) {
        lane = attrs.get<int>(SUMO_ATTR_ID, myCurrentID.c_str(), ok);
        if (!myHaveWarnedAboutDeprecatedLaneId) {
            myHaveWarnedAboutDeprecatedLaneId = true;
            WRITE_WARNINGF(TL("'%' is deprecated, please use '%' instead."), toString(SUMO_ATTR_ID), toString(SUMO_ATTR_INDEX));
        }
    } else {
        lane = attrs.get<int>(SUMO_ATTR_INDEX, myCurrentID.c_str(), ok);
    }
    if (!ok) {
        return;
    }
    // check whether this lane exists
    if (lane >= myCurrentEdge->getNumLanes()) {
        WRITE_ERRORF(TL("Lane index is larger than number of lanes (edge '%')."), myCurrentID);
        return;
    }
    myCurrentLaneIndex = lane;
    // set information about allowed / disallowed vehicle classes (if specified)
    if (attrs.hasAttribute(SUMO_ATTR_ALLOW) || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
        const std::string allowed = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, nullptr, ok, "");
        const std::string disallowed = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, nullptr, ok, "");
        myCurrentEdge->setPermissions(parseVehicleClasses(allowed, disallowed), lane);
    }
    if (attrs.hasAttribute(SUMO_ATTR_PREFER)) {
        const std::string preferred  = attrs.get<std::string>(SUMO_ATTR_PREFER, nullptr, ok);
        myCurrentEdge->setPreferredVehicleClass(parseVehicleClasses(preferred), lane);
    }
    if (attrs.hasAttribute(SUMO_ATTR_CHANGE_LEFT) || attrs.hasAttribute(SUMO_ATTR_CHANGE_RIGHT)) {
        const std::string changeLeft = attrs.getOpt<std::string>(SUMO_ATTR_CHANGE_LEFT, nullptr, ok, "");
        const std::string changeRight = attrs.getOpt<std::string>(SUMO_ATTR_CHANGE_RIGHT, nullptr, ok, "");
        myCurrentEdge->setPermittedChanging(lane, parseVehicleClasses(changeLeft, ""), parseVehicleClasses(changeRight, ""));
    }
    // try to get the width
    if (attrs.hasAttribute(SUMO_ATTR_WIDTH)) {
        myCurrentEdge->setLaneWidth(lane, attrs.get<double>(SUMO_ATTR_WIDTH, myCurrentID.c_str(), ok));
    }
    // try to get the end-offset (lane shortened due to pedestrian crossing etc..)
    if (attrs.hasAttribute(SUMO_ATTR_ENDOFFSET)) {
        myCurrentEdge->setEndOffset(lane, attrs.get<double>(SUMO_ATTR_ENDOFFSET, myCurrentID.c_str(), ok));
    }
    // try to get lane specific speed (should not occur for german networks)
    if (attrs.hasAttribute(SUMO_ATTR_SPEED)) {
        myCurrentEdge->setSpeed(lane, attrs.get<double>(SUMO_ATTR_SPEED, myCurrentID.c_str(), ok));
    }
    // try to get lane specific friction
    if (attrs.hasAttribute(SUMO_ATTR_FRICTION)) {
        myCurrentEdge->setFriction(lane, attrs.get<double>(SUMO_ATTR_FRICTION, myCurrentID.c_str(), ok));
    }
    // check whether this is an acceleration lane
    if (attrs.hasAttribute(SUMO_ATTR_ACCELERATION)) {
        myCurrentEdge->setAcceleration(lane, attrs.get<bool>(SUMO_ATTR_ACCELERATION, myCurrentID.c_str(), ok));
    }
    // check whether this lane has a custom shape
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, myCurrentID.c_str(), ok);
        if (!NBNetBuilder::transformCoordinates(shape, true, myLocation)) {
            const std::string laneID = myCurrentID + "_" + toString(lane);
            WRITE_ERRORF(TL("Unable to project coordinates for lane '%'."), laneID);
        }
        if (shape.size() == 1) {
            // lane shape of length 1 is not permitted
            shape.push_front(myCurrentEdge->getFromNode()->getPosition());
            shape.push_back(myCurrentEdge->getToNode()->getPosition());
        }
        shape.removeDoublePoints();
        if (shape.size() < 2) {
            // ignore lane shape for very short lanes
            shape.clear();
        }
        myCurrentEdge->setLaneShape(lane, shape);
    }
    // set custom lane type
    if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        myCurrentEdge->setLaneType(lane, attrs.get<std::string>(SUMO_ATTR_TYPE, myCurrentID.c_str(), ok));
    }
    myLastParameterised.push_back(&myCurrentEdge->getLaneStruct(lane));
}


void NIXMLEdgesHandler::addSplit(const SUMOSAXAttributes& attrs) {
    if (myCurrentEdge == nullptr) {
        if (!OptionsCont::getOptions().isInStringVector("remove-edges.explicit", myCurrentID)) {
            WRITE_WARNING(TL("Ignoring 'split' because it cannot be assigned to an edge"));
        }
        return;
    }
    bool ok = true;
    NBEdgeCont::Split e;
    e.pos = attrs.get<double>(SUMO_ATTR_POSITION, nullptr, ok);
    if (ok) {
        if (fabs(e.pos) > myCurrentEdge->getLoadedLength()) {
            WRITE_ERRORF(TL("Edge '%' has a split at invalid position %."), myCurrentID, toString(e.pos));
            return;
        }
        std::vector<NBEdgeCont::Split>::iterator i = find_if(mySplits.begin(), mySplits.end(), split_by_pos_finder(e.pos));
        if (i != mySplits.end()) {
            WRITE_ERRORF(TL("Edge '%' has already a split at position %."), myCurrentID, toString(e.pos));
            return;
        }
        // XXX rounding to int may duplicate the id of another split
        e.nameID = myCurrentID + "." + toString((int)e.pos);
        if (e.pos < 0) {
            e.pos += myCurrentEdge->getGeometry().length();
        }
        for (const std::string& id : attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LANES, myCurrentID.c_str(), ok)) {
            try {
                int lane = StringUtils::toInt(id);
                e.lanes.push_back(lane);
            } catch (NumberFormatException&) {
                WRITE_ERRORF(TL("Error on parsing a split (edge '%')."), myCurrentID);
            } catch (EmptyData&) {
                WRITE_ERRORF(TL("Error on parsing a split (edge '%')."), myCurrentID);
            }
        }
        if (e.lanes.empty()) {
            for (int l = 0; l < myCurrentEdge->getNumLanes(); ++l) {
                e.lanes.push_back(l);
            }
        }
        e.speed = attrs.getOpt(SUMO_ATTR_SPEED, nullptr, ok, myCurrentEdge->getSpeed());
        if (attrs.hasAttribute(SUMO_ATTR_SPEED) && myOptions.getBool("speed-in-kmh")) {
            e.speed /= 3.6;
        }
        e.idBefore = attrs.getOpt(SUMO_ATTR_ID_BEFORE, nullptr, ok, std::string(""));
        e.idAfter = attrs.getOpt(SUMO_ATTR_ID_AFTER, nullptr, ok, std::string(""));
        if (!ok) {
            return;
        }
        const std::string nodeID = attrs.getOpt(SUMO_ATTR_ID, nullptr, ok, e.nameID);
        if (nodeID == myCurrentEdge->getFromNode()->getID() || nodeID == myCurrentEdge->getToNode()->getID()) {
            WRITE_ERRORF(TL("Invalid split node id for edge '%' (from- and to-node are forbidden)"), myCurrentEdge->getID());
            return;
        }
        e.node = myNodeCont.retrieve(nodeID);
        e.offsetFactor = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;
        if (e.node == nullptr) {
            double geomPos = e.pos;
            if (myCurrentEdge->hasLoadedLength()) {
                geomPos *= myCurrentEdge->getGeometry().length() / myCurrentEdge->getLoadedLength();
            }
            e.node = new NBNode(nodeID, myCurrentEdge->getGeometry().positionAtOffset(geomPos));
            myNodeCont.insert(e.node);
        }
        NIXMLNodesHandler::processNodeType(attrs, e.node, e.node->getID(), e.node->getPosition(), false,
                                           myNodeCont, myEdgeCont, myTLLogicCont);
        mySplits.push_back(e);
    }
}


bool
NIXMLEdgesHandler::setNodes(const SUMOSAXAttributes& attrs) {
    // the names and the coordinates of the beginning and the end node
    // may be found, try
    bool ok = true;
    if (myIsUpdate) {
        myFromNode = myCurrentEdge->getFromNode();
        myToNode = myCurrentEdge->getToNode();
    }
    if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
        const std::string begNodeID = attrs.get<std::string>(SUMO_ATTR_FROM, nullptr, ok);
        if (begNodeID != "") {
            myFromNode = myNodeCont.retrieve(begNodeID);
            if (myFromNode == nullptr) {
                WRITE_ERRORF(TL("Edge's '%' from-node '%' is not known."), myCurrentID, begNodeID);
            }
        }
    } else if (!myIsUpdate) {
        WRITE_ERRORF(TL("The from-node is not given for edge '%'."), myCurrentID);
        ok = false;
    }
    if (attrs.hasAttribute(SUMO_ATTR_TO)) {
        const std::string endNodeID = attrs.get<std::string>(SUMO_ATTR_TO, nullptr, ok);
        if (endNodeID != "") {
            myToNode = myNodeCont.retrieve(endNodeID);
            if (myToNode == nullptr) {
                WRITE_ERRORF(TL("Edge's '%' to-node '%' is not known."), myCurrentID, endNodeID);
            }
        }
    } else if (!myIsUpdate) {
        WRITE_ERRORF(TL("The to-node is not given for edge '%'."), myCurrentID);
        ok = false;
    }
    return ok && myFromNode != nullptr && myToNode != nullptr;
}


PositionVector
NIXMLEdgesHandler::tryGetShape(const SUMOSAXAttributes& attrs) {
    if (!attrs.hasAttribute(SUMO_ATTR_SHAPE) && myShape.size() > 0) {
        return myShape;
    }
    // try to build shape
    bool ok = true;
    if (!attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        const double maxSegmentLength = OptionsCont::getOptions().getFloat("geometry.max-segment-length");
        if (maxSegmentLength > 0) {
            PositionVector shape;
            shape.push_back(myFromNode->getPosition());
            shape.push_back(myToNode->getPosition());
            // shape is already cartesian but we must use a copy because the original will be modified
            NBNetBuilder::addGeometrySegments(shape, PositionVector(shape), maxSegmentLength);
            return shape;
        } else {
            myReinitKeepEdgeShape = false;
            return PositionVector();
        }
    }
    PositionVector shape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, nullptr, ok, PositionVector());
    if (!NBNetBuilder::transformCoordinates(shape, true, myLocation)) {
        WRITE_ERRORF(TL("Unable to project coordinates for edge '%'."), myCurrentID);
    }
    myReinitKeepEdgeShape = myKeepEdgeShape;
    return shape;
}


LaneSpreadFunction
NIXMLEdgesHandler::tryGetLaneSpread(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    LaneSpreadFunction result = myLanesSpread;
    std::string lsfS = toString(result);
    lsfS = attrs.getOpt<std::string>(SUMO_ATTR_SPREADTYPE, myCurrentID.c_str(), ok, lsfS);
    if (SUMOXMLDefinitions::LaneSpreadFunctions.hasString(lsfS)) {
        result = SUMOXMLDefinitions::LaneSpreadFunctions.get(lsfS);
    } else {
        WRITE_WARNINGF(TL("Ignoring unknown spreadType '%' for edge '%'."), lsfS, myCurrentID);
    }
    return result;
}


void
NIXMLEdgesHandler::deleteEdge(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    NBEdge* edge = myEdgeCont.retrieve(myCurrentID);
    if (edge == nullptr) {
        WRITE_WARNING("Ignoring tag '" + toString(SUMO_TAG_DEL) + "' for unknown edge '" +
                      myCurrentID + "'");
        return;
    }
    const int lane = attrs.getOpt<int>(SUMO_ATTR_INDEX, myCurrentID.c_str(), ok, -1);
    if (lane < 0) {
        myEdgeCont.extract(myDistrictCont, edge, true);
    } else {
        edge->deleteLane(lane, false, true);
    }
}


void
NIXMLEdgesHandler::myEndElement(int element) {
    if (element == SUMO_TAG_VIEWSETTINGS_EDGES) {
        delete myLocation;
        myLocation = nullptr;
        return;
    }
    if (myCurrentEdge == nullptr) {
        return;
    }
    if (element == SUMO_TAG_EDGE) {
        myLastParameterised.pop_back();
        // add bike lane, wait until lanes are loaded to avoid building if it already exists
        if (myBikeLaneWidth != NBEdge::UNSPECIFIED_WIDTH) {
            myCurrentEdge->addBikeLane(myBikeLaneWidth);
        }
        // add sidewalk, wait until lanes are loaded to avoid building if it already exists
        if (mySidewalkWidth != NBEdge::UNSPECIFIED_WIDTH) {
            myCurrentEdge->addSidewalk(mySidewalkWidth);
        }
        // apply default stopOffsets of edge to all lanes without specified stopOffset.
        const StopOffset stopOffsets = myCurrentEdge->getEdgeStopOffset();
        if (stopOffsets.isDefined()) {
            for (int i = 0; i < (int)myCurrentEdge->getLanes().size(); i++) {
                myCurrentEdge->setEdgeStopOffset(i, stopOffsets, false);
            }
        }
        if (!myIsUpdate) {
            try {
                if (!myEdgeCont.insert(myCurrentEdge)) {
                    WRITE_ERRORF(TL("Duplicate edge '%' occurred."), myCurrentID);
                    delete myCurrentEdge;
                    myCurrentEdge = nullptr;
                    return;
                }
            } catch (InvalidArgument& e) {
                WRITE_ERROR(e.what());
                throw;
            } catch (...) {
                WRITE_ERRORF(TL("An important information is missing in edge '%'."), myCurrentID);
            }
        }
        myEdgeCont.processSplits(myCurrentEdge, mySplits, myNodeCont, myDistrictCont, myTLLogicCont);
        myCurrentEdge = nullptr;
    } else if (element == SUMO_TAG_LANE && myCurrentLaneIndex != -1) {
        myLastParameterised.pop_back();
        myCurrentLaneIndex = -1;
    }
}


void
NIXMLEdgesHandler::addRoundabout(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::vector<std::string>& edgeIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, nullptr, ok);
    if (ok) {
        EdgeSet roundabout;
        for (const std::string& eID : edgeIDs) {
            NBEdge* edge = myEdgeCont.retrieve(eID);
            if (edge == nullptr) {
                if (!myEdgeCont.wasIgnored(eID)) {
                    WRITE_ERRORF(TL("Unknown edge '%' in roundabout."), eID);
                }
            } else {
                roundabout.insert(edge);
            }
        }
        myEdgeCont.addRoundabout(roundabout);
    }
}


/****************************************************************************/
