/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIXMLEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @author  Laura Bieker
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for network edges stored in XML
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
#include <iostream>
#include <map>
#include <cmath>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include "NIXMLNodesHandler.h"
#include "NIXMLEdgesHandler.h"


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
    myCurrentEdge(0), myHaveReportedAboutOverwriting(false),
    myHaveReportedAboutTypeOverride(false),
    myHaveWarnedAboutDeprecatedLaneId(false),
    myKeepEdgeShape(!options.getBool("plain.extend-edge-shape")) {
}


NIXMLEdgesHandler::~NIXMLEdgesHandler() {}


void
NIXMLEdgesHandler::myStartElement(int element,
                                  const SUMOSAXAttributes& attrs) {
    switch (element) {
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
        case SUMO_TAG_DELETE:
            deleteEdge(attrs);
            break;
        case SUMO_TAG_ROUNDABOUT:
            addRoundabout(attrs);
            break;
        case SUMO_TAG_PARAM:
            if (myLastParameterised.size() != 0) {
                bool ok = true;
                const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, 0, ok);
                // circumventing empty string test
                const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
                myLastParameterised.back()->setParameter(key, val);
            }
        default:
            break;
    }
}


void
NIXMLEdgesHandler::addEdge(const SUMOSAXAttributes& attrs) {
    myIsUpdate = false;
    bool ok = true;
    // initialise the edge
    myCurrentEdge = 0;
    mySplits.clear();
    // get the id, report an error if not given or empty...
    myCurrentID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    myCurrentEdge = myEdgeCont.retrieve(myCurrentID);
    // check deprecated (unused) attributes
    // use default values, first
    myCurrentPriority = myTypeCont.getPriority("");
    myCurrentLaneNo = myTypeCont.getNumLanes("");
    myCurrentEndOffset = NBEdge::UNSPECIFIED_OFFSET;
    if (myCurrentEdge != 0) {
        // update existing edge. only update lane-specific settings when explicitly requested
        myIsUpdate = true;
        myCurrentSpeed = NBEdge::UNSPECIFIED_SPEED;
        myPermissions = SVC_UNSPECIFIED;
        myCurrentWidth = NBEdge::UNSPECIFIED_WIDTH;
    } else {
        // this is a completely new edge. get the type specific defaults
        myCurrentSpeed = myTypeCont.getSpeed("");
        myPermissions = myTypeCont.getPermissions("");
        myCurrentWidth = myTypeCont.getWidth("");
    }
    myCurrentType = "";
    myShape = PositionVector();
    myLanesSpread = LANESPREAD_RIGHT;
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
            WRITE_ERROR("Type '" + myCurrentType + "' used by edge '" + myCurrentID + "' was not defined (ignore with option --ignore-errors.edge-type).");
            return;
        }
        myCurrentSpeed = myTypeCont.getSpeed(myCurrentType);
        myCurrentPriority = myTypeCont.getPriority(myCurrentType);
        myCurrentLaneNo = myTypeCont.getNumLanes(myCurrentType);
        myPermissions = myTypeCont.getPermissions(myCurrentType);
        myCurrentWidth = myTypeCont.getWidth(myCurrentType);
        mySidewalkWidth = myTypeCont.getSidewalkWidth(myCurrentType);
        myBikeLaneWidth = myTypeCont.getBikeLaneWidth(myCurrentType);
    }
    // use values from the edge to overwrite if existing, then
    if (myIsUpdate) {
        if (!myHaveReportedAboutOverwriting) {
            WRITE_MESSAGE("Duplicate edge id occurred ('" + myCurrentID + "'); assuming overwriting is wished.");
            myHaveReportedAboutOverwriting = true;
        }
        if (attrs.hasAttribute(SUMO_ATTR_TYPE) && myCurrentType != myCurrentEdge->getTypeID()) {
            if (!myHaveReportedAboutTypeOverride) {
                WRITE_MESSAGE("Edge '" + myCurrentID + "' changed it's type; assuming type override is wished.");
                myHaveReportedAboutTypeOverride = true;
            }
        }
        if (attrs.getOpt<bool>(SUMO_ATTR_REMOVE, myCurrentID.c_str(), ok, false)) {
            myEdgeCont.erase(myDistrictCont, myCurrentEdge);
            myCurrentEdge = 0;
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
        return;
    }
    // check whether a previously defined edge shall be overwritten
    if (myCurrentEdge != 0) {
        myCurrentEdge->reinit(myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                              myCurrentLaneNo, myCurrentPriority, myShape,
                              myCurrentWidth, myCurrentEndOffset,
                              myCurrentStreetName, myLanesSpread,
                              myReinitKeepEdgeShape);
    } else {
        // the edge must be allocated in dependence to whether a shape is given
        if (myShape.size() == 0) {
            myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                                       myCurrentLaneNo, myCurrentPriority, myCurrentWidth, myCurrentEndOffset,
                                       myCurrentStreetName, myLanesSpread);
        } else {
            myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                                       myCurrentLaneNo, myCurrentPriority, myCurrentWidth, myCurrentEndOffset,
                                       myShape, myCurrentStreetName, "", myLanesSpread,
                                       myKeepEdgeShape);
        }
    }
    myCurrentEdge->setLoadedLength(myLength);
    if (myPermissions != SVC_UNSPECIFIED) {
        myCurrentEdge->setPermissions(myPermissions);
    }
    myLastParameterised.push_back(myCurrentEdge);
}


void
NIXMLEdgesHandler::addLane(const SUMOSAXAttributes& attrs) {
    if (myCurrentEdge == 0) {
        if (!OptionsCont::getOptions().isInStringVector("remove-edges.explicit", myCurrentID)) {
            WRITE_ERROR("Additional lane information could not be set - the edge with id '" + myCurrentID + "' is not known.");
        }
        return;
    }
    bool ok = true;
    int lane;
    if (attrs.hasAttribute(SUMO_ATTR_ID)) {
        lane = attrs.get<int>(SUMO_ATTR_ID, myCurrentID.c_str(), ok);
        if (!myHaveWarnedAboutDeprecatedLaneId) {
            myHaveWarnedAboutDeprecatedLaneId = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_ID) + "' is deprecated, please use '" + toString(SUMO_ATTR_INDEX) + "' instead.");
        }
    } else {
        lane = attrs.get<int>(SUMO_ATTR_INDEX, myCurrentID.c_str(), ok);
    }
    if (!ok) {
        return;
    }
    // check whether this lane exists
    if (lane >= myCurrentEdge->getNumLanes()) {
        WRITE_ERROR("Lane index is larger than number of lanes (edge '" + myCurrentID + "').");
        return;
    }
    // set information about allowed / disallowed vehicle classes (if specified)
    if (attrs.hasAttribute(SUMO_ATTR_ALLOW) || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
        const std::string allowed = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, 0, ok, "");
        const std::string disallowed = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, 0, ok, "");
        myCurrentEdge->setPermissions(parseVehicleClasses(allowed, disallowed), lane);
    }
    if (attrs.hasAttribute(SUMO_ATTR_PREFER)) {
        const std::string preferred  = attrs.get<std::string>(SUMO_ATTR_PREFER, 0, ok);
        myCurrentEdge->setPreferredVehicleClass(parseVehicleClasses(preferred), lane);
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
    // check whether this is an acceleration lane
    if (attrs.hasAttribute(SUMO_ATTR_ACCELERATION)) {
        myCurrentEdge->setAcceleration(lane, attrs.get<bool>(SUMO_ATTR_ACCELERATION, myCurrentID.c_str(), ok));
    }

    // check whether this is an acceleration lane
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, myCurrentID.c_str(), ok);
        if (!NBNetBuilder::transformCoordinates(shape)) {
            const std::string laneID = myCurrentID + "_" + toString(lane);
            WRITE_ERROR("Unable to project coordinates for lane '" + laneID + "'.");
        }
        myCurrentEdge->setLaneShape(lane, shape);
    }
    myLastParameterised.push_back(&myCurrentEdge->getLaneStruct(lane));
}


void NIXMLEdgesHandler::addSplit(const SUMOSAXAttributes& attrs) {
    if (myCurrentEdge == 0) {
        if (!OptionsCont::getOptions().isInStringVector("remove-edges.explicit", myCurrentID)) {
            WRITE_WARNING("Ignoring 'split' because it cannot be assigned to an edge");
        }
        return;
    }
    bool ok = true;
    Split e;
    e.pos = attrs.get<double>(SUMO_ATTR_POSITION, 0, ok);
    if (ok) {
        if (fabs(e.pos) > myCurrentEdge->getGeometry().length()) {
            WRITE_ERROR("Edge '" + myCurrentID + "' has a split at invalid position " + toString(e.pos) + ".");
            return;
        }
        std::vector<Split>::iterator i = find_if(mySplits.begin(), mySplits.end(), split_by_pos_finder(e.pos));
        if (i != mySplits.end()) {
            WRITE_ERROR("Edge '" + myCurrentID + "' has already a split at position " + toString(e.pos) + ".");
            return;
        }
        e.nameID = myCurrentID + "." + toString((int)e.pos);
        if (e.pos < 0) {
            e.pos += myCurrentEdge->getGeometry().length();
        }
        std::vector<std::string> lanes;
        SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LANES, 0, ok, ""), lanes);
        for (std::vector<std::string>::iterator i = lanes.begin(); i != lanes.end(); ++i) {
            try {
                int lane = TplConvert::_2int((*i).c_str());
                e.lanes.push_back(lane);
            } catch (NumberFormatException&) {
                WRITE_ERROR("Error on parsing a split (edge '" + myCurrentID + "').");
            } catch (EmptyData&) {
                WRITE_ERROR("Error on parsing a split (edge '" + myCurrentID + "').");
            }
        }
        if (e.lanes.empty()) {
            for (int l = 0; l < myCurrentEdge->getNumLanes(); ++l) {
                e.lanes.push_back(l);
            }
        }
        e.speed = attrs.getOpt(SUMO_ATTR_SPEED, 0, ok, myCurrentEdge->getSpeed());
        if (attrs.hasAttribute(SUMO_ATTR_SPEED) && myOptions.getBool("speed-in-kmh")) {
            e.speed /= (double) 3.6;
        }
        e.idBefore = attrs.getOpt(SUMO_ATTR_ID_BEFORE, 0, ok, std::string(""));
        e.idAfter = attrs.getOpt(SUMO_ATTR_ID_AFTER, 0, ok, std::string(""));
        if (!ok) {
            return;
        }
        const std::string nodeID = attrs.getOpt(SUMO_ATTR_ID, 0, ok, e.nameID);
        e.node = myNodeCont.retrieve(nodeID);
        if (e.node == 0) {
            e.node = new NBNode(nodeID, myCurrentEdge->getGeometry().positionAtOffset(e.pos));
        }
        NIXMLNodesHandler::processNodeType(attrs, e.node, e.node->getID(), e.node->getPosition(), false,
                                           myNodeCont, myTLLogicCont);
        mySplits.push_back(e);
    }
}


bool
NIXMLEdgesHandler::setNodes(const SUMOSAXAttributes& attrs) {
    // the names and the coordinates of the beginning and the end node
    // may be found, try
    bool ok = true;
    std::string begNodeID = myIsUpdate ? myCurrentEdge->getFromNode()->getID() : "";
    std::string endNodeID = myIsUpdate ? myCurrentEdge->getToNode()->getID() : "";
    std::string oldBegID = begNodeID;
    std::string oldEndID = endNodeID;
    if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
        begNodeID = attrs.get<std::string>(SUMO_ATTR_FROM, 0, ok);
    } else if (!myIsUpdate) {
        WRITE_ERROR("The from-node is not given for edge '" + myCurrentID + "'.");
        ok = false;
    }
    if (attrs.hasAttribute(SUMO_ATTR_TO)) {
        endNodeID = attrs.get<std::string>(SUMO_ATTR_TO, 0, ok);
    } else if (!myIsUpdate) {
        WRITE_ERROR("The to-node is not given for edge '" + myCurrentID + "'.");
        ok = false;
    }
    if (!ok) {
        return false;
    }
    myFromNode = myNodeCont.retrieve(begNodeID);
    myToNode = myNodeCont.retrieve(endNodeID);
    if (myFromNode == 0) {
        WRITE_ERROR("Edge's '" + myCurrentID + "' from-node '" + begNodeID + "' is not known.");
    }
    if (myToNode == 0) {
        WRITE_ERROR("Edge's '" + myCurrentID + "' to-node '" + endNodeID + "' is not known.");
    }
    if (myFromNode != 0 && myToNode != 0) {
        if (myIsUpdate && (myFromNode->getID() != oldBegID || myToNode->getID() != oldEndID)) {
            myShape = PositionVector();
        }
    }
    return myFromNode != 0 && myToNode != 0;
}


PositionVector
NIXMLEdgesHandler::tryGetShape(const SUMOSAXAttributes& attrs) {
    if (!attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        return myShape;
    }
    // try to build shape
    bool ok = true;
    if (!attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        myReinitKeepEdgeShape = false;
        return PositionVector();
    }
    PositionVector shape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, 0, ok, PositionVector());
    if (!NBNetBuilder::transformCoordinates(shape)) {
        WRITE_ERROR("Unable to project coordinates for edge '" + myCurrentID + "'.");
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
        WRITE_WARNING("Ignoring unknown spreadType '" + lsfS + "' for edge '" + myCurrentID + "'.");
    }
    return result;
}


void
NIXMLEdgesHandler::deleteEdge(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    NBEdge* edge = myEdgeCont.retrieve(myCurrentID);
    if (edge == 0) {
        WRITE_WARNING("Ignoring tag '" + toString(SUMO_TAG_DELETE) + "' for unknown edge '" +
                      myCurrentID + "'");
        return;
    }
    const int lane = attrs.getOpt<int>(SUMO_ATTR_INDEX, myCurrentID.c_str(), ok, -1);
    if (lane < 0) {
        myEdgeCont.extract(myDistrictCont, edge, true);
    } else {
        edge->removeFromConnections(0, lane, -1, false, true);
        const EdgeVector incoming = edge->getIncomingEdges();
        for (EdgeVector::const_iterator e = incoming.begin(); e != incoming.end(); ++e) {
            (*e)->removeFromConnections(edge, -1, lane, false, true);
        }
        edge->deleteLane(lane, false, true);
    }
}


void
NIXMLEdgesHandler::myEndElement(int element) {
    if (element == SUMO_TAG_EDGE && myCurrentEdge != 0) {
        myLastParameterised.pop_back();
        // add bike lane, wait until lanes are loaded to avoid building if it already exists
        if (myBikeLaneWidth != NBEdge::UNSPECIFIED_WIDTH) {
            myCurrentEdge->addBikeLane(myBikeLaneWidth);
        }
        // add sidewalk, wait until lanes are loaded to avoid building if it already exists
        if (mySidewalkWidth != NBEdge::UNSPECIFIED_WIDTH) {
            myCurrentEdge->addSidewalk(mySidewalkWidth);
        }
        if (!myIsUpdate) {
            try {
                if (!myEdgeCont.insert(myCurrentEdge)) {
                    WRITE_ERROR("Duplicate edge occured. ID='" + myCurrentID + "'");
                    delete myCurrentEdge;
                }
            } catch (InvalidArgument& e) {
                WRITE_ERROR(e.what());
                throw;
            } catch (...) {
                WRITE_ERROR("An important information is missing in edge '" + myCurrentID + "'.");
            }
        }
        if (mySplits.size() != 0) {
            std::vector<Split>::iterator i;
            NBEdge* e = myCurrentEdge;
            sort(mySplits.begin(), mySplits.end(), split_sorter());
            int noLanesMax = e->getNumLanes();
            // compute the node positions and sort the lanes
            for (i = mySplits.begin(); i != mySplits.end(); ++i) {
                sort((*i).lanes.begin(), (*i).lanes.end());
                noLanesMax = MAX2(noLanesMax, (int)(*i).lanes.size());
            }
            // split the edge
            std::vector<int> currLanes;
            for (int l = 0; l < e->getNumLanes(); ++l) {
                currLanes.push_back(l);
            }
            if (e->getNumLanes() != (int)mySplits.back().lanes.size()) {
                // invalidate traffic light definitions loaded from a SUMO network
                e->getToNode()->invalidateTLS(myTLLogicCont, true, true);
                // if the number of lanes changes the connections should be
                // recomputed
                e->invalidateConnections(true);
            }

            std::string firstID = "";
            double seen = 0;
            for (i = mySplits.begin(); i != mySplits.end(); ++i) {
                const Split& exp = *i;
                assert(exp.lanes.size() != 0);
                if (exp.pos > 0 && e->getGeometry().length() + seen > exp.pos && exp.pos > seen) {
                    myNodeCont.insert(exp.node);
                    myNodeCont.markAsSplit(exp.node);
                    //  split the edge
                    std::string idBefore = exp.idBefore == "" ? e->getID() : exp.idBefore;
                    std::string idAfter = exp.idAfter == "" ? exp.nameID : exp.idAfter;
                    if (firstID == "") {
                        firstID = idBefore;
                    }
                    const bool ok = myEdgeCont.splitAt(myDistrictCont, e, exp.pos - seen, exp.node,
                                                       idBefore, idAfter, e->getNumLanes(), (int) exp.lanes.size(), exp.speed);
                    if (!ok) {
                        WRITE_WARNING("Error on parsing a split (edge '" + myCurrentID + "').");
                    }
                    seen = exp.pos;
                    std::vector<int> newLanes = exp.lanes;
                    NBEdge* pe = myEdgeCont.retrieve(idBefore);
                    NBEdge* ne = myEdgeCont.retrieve(idAfter);
                    // reconnect lanes
                    pe->invalidateConnections(true);
                    //  new on right
                    int rightMostP = currLanes[0];
                    int rightMostN = newLanes[0];
                    for (int l = 0; l < (int) rightMostP - (int) rightMostN; ++l) {
                        pe->addLane2LaneConnection(0, ne, l, NBEdge::L2L_VALIDATED, true);
                    }
                    //  new on left
                    int leftMostP = currLanes.back();
                    int leftMostN = newLanes.back();
                    for (int l = 0; l < (int) leftMostN - (int) leftMostP; ++l) {
                        pe->addLane2LaneConnection(pe->getNumLanes() - 1, ne, leftMostN - l - rightMostN, NBEdge::L2L_VALIDATED, true);
                    }
                    //  all other connected
                    for (int l = 0; l < noLanesMax; ++l) {
                        if (find(currLanes.begin(), currLanes.end(), l) == currLanes.end()) {
                            continue;
                        }
                        if (find(newLanes.begin(), newLanes.end(), l) == newLanes.end()) {
                            continue;
                        }
                        pe->addLane2LaneConnection(l - rightMostP, ne, l - rightMostN, NBEdge::L2L_VALIDATED, true);
                    }
                    // move to next
                    e = ne;
                    currLanes = newLanes;
                }  else if (exp.pos == 0) {
                    const int laneCountDiff = e->getNumLanes() - (int)exp.lanes.size();
                    if (laneCountDiff < 0) {
                        e->incLaneNo(-laneCountDiff);
                    } else {
                        e->decLaneNo(laneCountDiff);
                    }
                    currLanes = exp.lanes;
                    // invalidate traffic light definition loaded from a SUMO network
                    // XXX it would be preferable to reconstruct the phase definitions heuristically
                    e->getFromNode()->invalidateTLS(myTLLogicCont, true, true);
                } else {
                    WRITE_WARNING("Split at '" + toString(exp.pos) + "' lies beyond the edge's length (edge '" + myCurrentID + "').");
                }
            }
            // patch lane offsets
            e = myEdgeCont.retrieve(firstID);
            if (mySplits.front().pos != 0) {
                // add a dummy split at the beginning to ensure correct offset
                Split start;
                start.pos = 0;
                for (int lane = 0; lane < (int)e->getNumLanes(); ++lane) {
                    start.lanes.push_back(lane);
                }
                mySplits.insert(mySplits.begin(), start);
            }
            i = mySplits.begin();
            for (; i != mySplits.end(); ++i) {
                int maxLeft = (*i).lanes.back();
                double offset = 0;
                if (maxLeft < noLanesMax) {
                    if (e->getLaneSpreadFunction() == LANESPREAD_RIGHT) {
                        offset = SUMO_const_laneWidthAndOffset * (noLanesMax - 1 - maxLeft);
                    } else {
                        offset = SUMO_const_halfLaneAndOffset * (noLanesMax - 1 - maxLeft);
                    }
                }
                int maxRight = (*i).lanes.front();
                if (maxRight > 0 && e->getLaneSpreadFunction() == LANESPREAD_CENTER) {
                    offset -= SUMO_const_halfLaneAndOffset * maxRight;
                }
                if (offset != 0) {
                    PositionVector g = e->getGeometry();
                    g.move2side(offset);
                    e->setGeometry(g);
                }
                if (e->getToNode()->getOutgoingEdges().size() != 0) {
                    e = e->getToNode()->getOutgoingEdges()[0];
                }
            }
        }
        myCurrentEdge = 0;
    } else if (element == SUMO_TAG_LANE) {
        myLastParameterised.pop_back();
    }
}


void
NIXMLEdgesHandler::addRoundabout(const SUMOSAXAttributes& attrs) {
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        std::vector<std::string> edgeIDs = attrs.getStringVector(SUMO_ATTR_EDGES);
        EdgeSet roundabout;
        for (std::vector<std::string>::iterator it = edgeIDs.begin(); it != edgeIDs.end(); ++it) {
            NBEdge* edge = myEdgeCont.retrieve(*it);
            if (edge == 0) {
                if (!myEdgeCont.wasIgnored(*it)) {
                    WRITE_ERROR("Unknown edge '" + (*it) + "' in roundabout");
                }
            } else {
                roundabout.insert(edge);
            }
        }
        myEdgeCont.addRoundabout(roundabout);
    } else {
        WRITE_ERROR("Empty edges in roundabout.");
    }
}



/****************************************************************************/

