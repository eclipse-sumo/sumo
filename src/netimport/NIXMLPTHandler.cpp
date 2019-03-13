/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIXMLPTHandler.cpp
/// @author  Jakob Erdmann
/// @date    Sat, 28 Jul 2018
/// @version $Id$
///
// Importer for static public transport information
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
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
    myCurrentLine(nullptr) {
}


NIXMLPTHandler::~NIXMLPTHandler() {}


void
NIXMLPTHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
            if (myCurrentLine == nullptr) {
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
            break;
    }
}

void
NIXMLPTHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
            myCurrentStop = nullptr;
            break;
        case SUMO_TAG_PT_LINE:
            myCurrentLine->setMyNumOfStops((int)(myCurrentLine->getStops().size() / myCurrentCompletion));
            myCurrentLine = nullptr;
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
    //const std::string lines = attrs.get<std::string>(SUMO_ATTR_LINES, id.c_str(), ok);
    const int laneIndex = NBEdge::getLaneIndexFromLaneID(laneID);
    const std::string edgeID = SUMOXMLDefinitions::getEdgeIDFromLane(laneID);
    NBEdge* edge = myEdgeCont.retrieve(edgeID);
    if (edge == nullptr) {
        WRITE_ERROR("Edge '" + edgeID + "' for stop '" + id + "' not found");
        return;
    }
    if (edge->getNumLanes() <= laneIndex) {
        WRITE_ERROR("Lane '" + laneID + "' for stop '" + id + "' not found");
        return;
    }
    SVCPermissions permissions = edge->getPermissions(laneIndex);
    if (ok) {
        Position pos = edge->getGeometry().positionAtOffset2D((startPos + endPos) / 2);
        myCurrentStop = new NBPTStop(id, pos, edgeID, edgeID, endPos - startPos, name, permissions);
        if (!myStopCont.insert(myCurrentStop)) {
            WRITE_ERROR("Could not add public transport stop '" + id + "' (already exists)");
        }
    }
}

void
NIXMLPTHandler::addAccess(const SUMOSAXAttributes& attrs) {
    if (myCurrentStop == nullptr) {
        throw InvalidArgument("Could not add access outside a stopping place.");
    }
    bool ok = true;
    const std::string lane = attrs.get<std::string>(SUMO_ATTR_LANE, "access", ok);
    const double pos = attrs.get<double>(SUMO_ATTR_POSITION, "access", ok);
    const double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, "access", ok, -1);
    myCurrentStop->addAccess(lane, pos, length);
}


void
NIXMLPTHandler::addPTLine(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "ptLine", ok);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_ID, id.c_str(), ok, "");
    const std::string line = attrs.get<std::string>(SUMO_ATTR_LINE, id.c_str(), ok);
    const std::string type = attrs.get<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok);
    const int intervalS = attrs.getOpt<int>(SUMO_ATTR_PERIOD, id.c_str(), ok, -1);
    const std::string nightService = attrs.getStringSecure("nightService", "");
    myCurrentCompletion = StringUtils::toDouble(attrs.getStringSecure("completeness", "1"));
    /// XXX parse route child
    //if (!myRoute.empty()) {
    //    device.openTag(SUMO_TAG_ROUTE);
    //    device.writeAttr(SUMO_ATTR_EDGES, validEdgeIDs);
    //    device.closeTag();
    //}
    if (ok) {
        myCurrentLine = new NBPTLine(id, name, type, line, intervalS / 60, nightService);
        myLineCont.insert(myCurrentLine);
    }
}


void
NIXMLPTHandler::addPTLineStop(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "ptLine", ok);
    NBPTStop* stop = myStopCont.get(id);
    if (stop == nullptr) {
        WRITE_ERROR("Stop '" + id + "' within line '" + toString(myCurrentLine->getLineID()) + "' not found");
        return;
    }
    myCurrentLine->addPTStop(stop);
}


/****************************************************************************/

