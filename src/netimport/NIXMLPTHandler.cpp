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
/// @file    NIXMLPTHandler.cpp
/// @author  Jakob Erdmann
/// @date    Sat, 28 Jul 2018
///
// Importer for static public transport information
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBPTStop.h>
#include "NIImporter_OpenStreetMap.h"
#include "NIXMLNodesHandler.h"
#include "NIXMLPTHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLPTHandler::NIXMLPTHandler(NBEdgeCont& ec, NBPTStopCont& sc, NBPTLineCont& lc) :
    SUMOSAXHandler("public transport - file"),
    myEdgeCont(ec),
    myStopCont(sc),
    myLineCont(lc),
    myCurrentStop(nullptr),
    myCurrentLine(nullptr),
    myCurrentCompletion(0),
    myCurrentStopWasIgnored(false)
{ }


NIXMLPTHandler::~NIXMLPTHandler() {}


void
NIXMLPTHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
        case SUMO_TAG_STOP:
            if (myCurrentRouteID != "") {
                addRouteStop(attrs);
            } else if (myCurrentLine == nullptr) {
                addPTStop(attrs);
            } else {
                addPTLineStop(attrs);
            }
            break;
        case SUMO_TAG_ACCESS:
            addAccess(attrs);
            break;
        case SUMO_TAG_PT_LINE:
            addPTLine(attrs);
            break;
        case SUMO_TAG_ROUTE:
            if (myCurrentLine == nullptr) {
                addRoute(attrs);
            } else {
                addPTLineRoute(attrs);
            }
            break;
        case SUMO_TAG_FLOW:
        case SUMO_TAG_TRIP:
            addPTLineFromFlow(attrs);
            break;
        case SUMO_TAG_PARAM: {
            bool ok = true;
            const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
            if (myCurrentLine != nullptr) {
                if (key == "completeness") {
                    myCurrentCompletion = attrs.get<double>(SUMO_ATTR_VALUE, nullptr, ok);
                } else if (key == "name") {
                    myCurrentLine->setName(attrs.get<std::string>(SUMO_ATTR_VALUE, nullptr, ok));
                }
            } else if (myCurrentStop != nullptr) {
                const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
                myCurrentStop->setParameter(key, val);
            }
        }
        break;
        default:
            break;
    }
}

void
NIXMLPTHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
            myCurrentStop = nullptr;
            myCurrentStopWasIgnored = false;
            break;
        case SUMO_TAG_PT_LINE:
        case SUMO_TAG_FLOW:
        case SUMO_TAG_TRIP:
            myCurrentLine->setMyNumOfStops((int)((double)myCurrentLine->getStops().size() / myCurrentCompletion));
            myCurrentLine = nullptr;
            break;
        case SUMO_TAG_ROUTE:
            myCurrentRouteID = "";
            break;
        default:
            break;
    }
}


void
NIXMLPTHandler::addPTStop(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "busStop", ok);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, "");
    const std::string laneID = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    const double startPos = attrs.get<double>(SUMO_ATTR_STARTPOS, id.c_str(), ok);
    const double endPos = attrs.get<double>(SUMO_ATTR_ENDPOS, id.c_str(), ok);
    const double parkingLength = attrs.getOpt<double>(SUMO_ATTR_PARKING_LENGTH, id.c_str(), ok, 0);
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok, RGBColor(false));
    //const std::string lines = attrs.get<std::string>(SUMO_ATTR_LINES, id.c_str(), ok);
    const int laneIndex = NBEdge::getLaneIndexFromLaneID(laneID);
    const std::string edgeID = SUMOXMLDefinitions::getEdgeIDFromLane(laneID);
    NBEdge* edge = myEdgeCont.retrieve(edgeID);
    if (edge == nullptr) {
        if (!myEdgeCont.wasIgnored(edgeID)) {
            WRITE_ERRORF(TL("Edge '%' for stop '%' not found"), edgeID, id);
        } else {
            myCurrentStopWasIgnored = true;
            NBPTStopCont::addIgnored(id);
        }
        return;
    }
    if (edge->getNumLanes() <= laneIndex) {
        WRITE_ERRORF(TL("Lane '%' for stop '%' not found"), laneID, id);
        return;
    }
    SVCPermissions permissions = edge->getPermissions(laneIndex);
    // possibly the stops were written for a different network. If the lane is not a typical public transport stop lane, assume bus as the default
    if (!isRailway(permissions) && permissions != SVC_SHIP && permissions != SVC_TAXI) {
        permissions = SVC_BUS;
    }
    if (ok) {
        Position pos = edge->geometryPositionAtOffset((startPos + endPos) / 2);
        myCurrentStop = std::make_shared<NBPTStop>(id, pos, edgeID, edgeID, endPos - startPos, name, permissions, parkingLength, color, startPos);
        if (!myStopCont.insert(myCurrentStop)) {
            WRITE_ERRORF(TL("Could not add public transport stop '%' (already exists)"), id);
        }
    }
}

void
NIXMLPTHandler::addAccess(const SUMOSAXAttributes& attrs) {
    if (myCurrentStop == nullptr) {
        if (myCurrentStopWasIgnored) {
            return;
        } else {
            throw InvalidArgument("Could not add access outside a stopping place.");
        }
    }
    bool ok = true;
    const std::string laneID = attrs.get<std::string>(SUMO_ATTR_LANE, "access", ok);
    const std::string edgeID = SUMOXMLDefinitions::getEdgeIDFromLane(laneID);
    if (myEdgeCont.retrieve(edgeID) == nullptr) {
        if (!myEdgeCont.wasIgnored(edgeID)) {
            WRITE_ERRORF(TL("Edge '%' for access to stop '%' not found"), edgeID, myCurrentStop->getID());
        }
        return;
    }
    const double pos = attrs.get<double>(SUMO_ATTR_POSITION, "access", ok);
    const double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, "access", ok, -1);
    if (ok) {
        myCurrentStop->addAccess(laneID, pos, length);
    }
}


void
NIXMLPTHandler::addPTLine(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "ptLine", ok);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, "");
    const std::string line = attrs.get<std::string>(SUMO_ATTR_LINE, id.c_str(), ok);
    const std::string type = attrs.get<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok);
    SUMOVehicleClass vClass = NIImporter_OpenStreetMap::interpretTransportType(type);
    if (attrs.hasAttribute(SUMO_ATTR_VCLASS)) {
        vClass = getVehicleClassID(attrs.get<std::string>(SUMO_ATTR_VCLASS, id.c_str(), ok));
    }
    RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok, RGBColor(false));
    const int intervalS = attrs.getOpt<int>(SUMO_ATTR_PERIOD, id.c_str(), ok, -1);
    const std::string nightService = attrs.getStringSecure("nightService", "");
    myCurrentCompletion = StringUtils::toDouble(attrs.getStringSecure("completeness", "1"));
    if (ok) {
        myCurrentLine = new NBPTLine(id, name, type, line, intervalS / 60, nightService, vClass, color);
        if (!myLineCont.insert(myCurrentLine)) {
            WRITE_WARNINGF(TL("Ignoring duplicate PT line '%'."), id);
            delete myCurrentLine;
        }
    }
}


void
NIXMLPTHandler::addPTLineFromFlow(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "flow", ok);
    const std::string line = attrs.get<std::string>(SUMO_ATTR_LINE, id.c_str(), ok);
    const std::string type = attrs.get<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok);
    const std::string route = attrs.get<std::string>(SUMO_ATTR_ROUTE, id.c_str(), ok);
    SUMOVehicleClass vClass = NIImporter_OpenStreetMap::interpretTransportType(type);
    const int intervalS = attrs.getOpt<int>(SUMO_ATTR_PERIOD, id.c_str(), ok, -1);
    RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok, RGBColor(false));
    if (ok) {
        myCurrentLine = new NBPTLine(id, "", type, line, intervalS / 60, "", vClass, color);
        myCurrentLine->setEdges(myRouteEdges[route]);
        for (std::shared_ptr<NBPTStop> stop : myRouteStops[route]) {
            myCurrentLine->addPTStop(stop);
        }
        myLineCont.insert(myCurrentLine);
    }
}


void
NIXMLPTHandler::addPTLineRoute(const SUMOSAXAttributes& attrs) {
    if (myCurrentLine == nullptr) {
        WRITE_ERROR(TL("Found route outside line definition"));
        return;
    }
    bool ok = true;
    const std::vector<std::string>& edgeIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, nullptr, ok);
    EdgeVector edges;
    for (const std::string& edgeID : edgeIDs) {
        NBEdge* edge = myEdgeCont.retrieve(edgeID);
        if (edge == nullptr) {
            if (!myEdgeCont.wasIgnored(edgeID)) {
                WRITE_ERRORF(TL("Edge '%' in route of line '%' not found"), edgeID, myCurrentLine->getName());
            }
        } else {
            edges.push_back(edge);
        }
    }
    myCurrentLine->setEdges(edges);
}


void
NIXMLPTHandler::addRoute(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentRouteID = attrs.get<std::string>(SUMO_ATTR_ID, "route", ok);
    const std::vector<std::string>& edgeIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, myCurrentRouteID.c_str(), ok);
    EdgeVector edges;
    for (const std::string& edgeID : edgeIDs) {
        NBEdge* edge = myEdgeCont.retrieve(edgeID);
        if (edge == nullptr) {
            if (!myEdgeCont.wasIgnored(edgeID)) {
                WRITE_ERRORF(TL("Edge '%' in route of line '%' not found"), edgeID, myCurrentLine->getName());
            }
        } else {
            edges.push_back(edge);
        }
    }
    myRouteEdges[myCurrentRouteID] = edges;
}


void
NIXMLPTHandler::addPTLineStop(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string id = attrs.hasAttribute(SUMO_ATTR_ID)
                           ? attrs.get<std::string>(SUMO_ATTR_ID, "ptLine", ok)
                           : attrs.get<std::string>(SUMO_ATTR_BUS_STOP, "ptline", ok);
    std::shared_ptr<NBPTStop> stop = myStopCont.get(id);
    if (stop == nullptr) {
        if (!NBPTStopCont::wasIgnored(id)) {
            WRITE_ERRORF(TL("Stop '%' within line '%' not found"), id, toString(myCurrentLine->getLineID()));
        }
        return;
    }
    myCurrentLine->addPTStop(stop);
}

void
NIXMLPTHandler::addRouteStop(const SUMOSAXAttributes& attrs) {
    assert(myCurrentRouteID != "");
    bool ok = true;
    const std::string id = attrs.hasAttribute(SUMO_ATTR_ID)
                           ? attrs.get<std::string>(SUMO_ATTR_ID, "ptLine", ok)
                           : attrs.get<std::string>(SUMO_ATTR_BUS_STOP, "ptline", ok);
    std::shared_ptr<NBPTStop> stop = myStopCont.get(id);
    if (stop == nullptr) {
        WRITE_ERRORF(TL("Stop '%' within route '%' not found"), id, toString(myCurrentRouteID));
        return;
    }
    myRouteStops[myCurrentRouteID].push_back(stop);
}


/****************************************************************************/
