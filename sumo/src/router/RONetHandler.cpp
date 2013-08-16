/****************************************************************************/
/// @file    RONetHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The handler for SUMO-Networks
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
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "ROEdge.h"
#include "ROLane.h"
#include "RONode.h"
#include "RONet.h"
#include "RONetHandler.h"
#include "ROAbstractEdgeBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RONetHandler::RONetHandler(RONet& net,
                           ROAbstractEdgeBuilder& eb)
    : SUMOSAXHandler("sumo-network"),
      myNet(net), myCurrentName(),
      myCurrentEdge(0), myEdgeBuilder(eb) {}


RONetHandler::~RONetHandler() {}


void
RONetHandler::myStartElement(int element,
                             const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_EDGE:
            // in the first step, we do need the name to allocate the edge
            // in the second, we need it to know to which edge we have to add
            //  the following edges to
            parseEdge(attrs);
            break;
        case SUMO_TAG_LANE:
            if (myProcess) {
                parseLane(attrs);
            }
            break;
        case SUMO_TAG_JUNCTION:
            parseJunction(attrs);
            break;
        case SUMO_TAG_CONNECTION:
            parseConnection(attrs);
            break;
        case SUMO_TAG_TAZ:
            parseDistrict(attrs);
            break;
        case SUMO_TAG_TAZSOURCE:
            parseDistrictEdge(attrs, true);
            break;
        case SUMO_TAG_TAZSINK:
            parseDistrictEdge(attrs, false);
            break;
        default:
            break;
    }
}


void
RONetHandler::parseEdge(const SUMOSAXAttributes& attrs) {
    // get the id, report an error if not given or empty...
    bool ok = true;
    myCurrentName = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the edge
    myCurrentEdge = 0;
    if (myCurrentName[0] == ':') {
        // this is an internal edge - we will not use it
        //  !!! recheck this; internal edges may be of importance during the dua
        return;
    }
    const std::string from = attrs.get<std::string>(SUMO_ATTR_FROM, myCurrentName.c_str(), ok);
    const std::string to = attrs.get<std::string>(SUMO_ATTR_TO, myCurrentName.c_str(), ok);
    const std::string type = attrs.hasAttribute(SUMO_ATTR_FUNCTION) ? attrs.get<std::string>(SUMO_ATTR_FUNCTION, myCurrentName.c_str(), ok) : "";
    const int priority = attrs.get<int>(SUMO_ATTR_PRIORITY, myCurrentName.c_str(), ok);
    if (!ok) {
        return;
    }
    RONode* fromNode = myNet.getNode(from);
    if (fromNode == 0) {
        fromNode = new RONode(from);
        myNet.addNode(fromNode);
    }
    RONode* toNode = myNet.getNode(to);
    if (toNode == 0) {
        toNode = new RONode(to);
        myNet.addNode(toNode);
    }
    // build the edge
    myCurrentEdge = myEdgeBuilder.buildEdge(myCurrentName, fromNode, toNode, priority);
    if (myNet.addEdge(myCurrentEdge)) {
        // get the type of the edge
        myProcess = true;
        if (type == "" || type == "normal" || type == "connector") {
            myCurrentEdge->setType(ROEdge::ET_NORMAL);
        } else if (type == "source") {
            myCurrentEdge->setType(ROEdge::ET_SOURCE);
        } else if (type == "sink") {
            myCurrentEdge->setType(ROEdge::ET_SINK);
        } else if (type == "internal") {
            myProcess = false;
        } else {
            WRITE_ERROR("Edge '" + myCurrentName + "' has an unknown type.");
            return;
        }
    } else {
        myCurrentEdge = 0;
    }
}


void
RONetHandler::parseLane(const SUMOSAXAttributes& attrs) {
    if (myCurrentEdge == 0) {
        // was an internal edge to skip or an error occured
        return;
    }
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    // get the speed
    SUMOReal maxSpeed = attrs.get<SUMOReal>(SUMO_ATTR_SPEED, id.c_str(), ok);
    SUMOReal length = attrs.get<SUMOReal>(SUMO_ATTR_LENGTH, id.c_str(), ok);
    std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id.c_str(), ok, "");
    std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    if (!ok) {
        return;
    }
    // get the length
    // get the vehicle classes
    SVCPermissions permissions = parseVehicleClasses(allow, disallow);
    if (permissions != SVCFreeForAll) {
        myNet.setRestrictionFound();
    }
    // add when both values are valid
    if (maxSpeed > 0 && length > 0 && id.length() > 0) {
        myCurrentEdge->addLane(new ROLane(id, length, maxSpeed, permissions));
    }
}


void
RONetHandler::parseJunction(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    // get the position of the node
    SUMOReal x = attrs.get<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok);
    SUMOReal y = attrs.get<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok);
    if (ok) {
        RONode* n = myNet.getNode(id);
        if (n == 0) {
            n = new RONode(id);
            myNet.addNode(n);
        }
        n->setPosition(Position(x, y));
    } else {
        throw ProcessError();
    }
}


void
RONetHandler::parseConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, 0, ok);
    std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, 0, ok);
    std::string dir = attrs.get<std::string>(SUMO_ATTR_DIR, 0, ok);
    if (fromID[0] == ':') { // skip inner lane connections
        return;
    }
    ROEdge* from = myNet.getEdge(fromID);
    ROEdge* to = myNet.getEdge(toID);
    if (from == 0) {
        throw ProcessError("unknown from-edge '" + fromID + "' in connection");
    }
    if (to == 0) {
        throw ProcessError("unknown to-edge '" + toID + "' in connection");
    }
    from->addFollower(to, dir);
}


void
RONetHandler::parseDistrict(const SUMOSAXAttributes& attrs) {
    myCurrentEdge = 0;
    bool ok = true;
    myCurrentName = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    ROEdge* sink = myEdgeBuilder.buildEdge(myCurrentName + "-sink", 0, 0, 0);
    sink->setType(ROEdge::ET_DISTRICT);
    myNet.addEdge(sink);
    ROEdge* source = myEdgeBuilder.buildEdge(myCurrentName + "-source", 0, 0, 0);
    source->setType(ROEdge::ET_DISTRICT);
    myNet.addEdge(source);
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        std::vector<std::string> desc = attrs.getStringVector(SUMO_ATTR_EDGES);
        for (std::vector<std::string>::const_iterator i = desc.begin(); i != desc.end(); ++i) {
            ROEdge* edge = myNet.getEdge(*i);
            // check whether the edge exists
            if (edge == 0) {
                throw ProcessError("The edge '" + *i + "' within district '" + myCurrentName + "' is not known.");
            }
            source->addFollower(edge);
            edge->addFollower(sink);
        }
    }
}


void
RONetHandler::parseDistrictEdge(const SUMOSAXAttributes& attrs, bool isSource) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, myCurrentName.c_str(), ok);
    ROEdge* succ = myNet.getEdge(id);
    if (succ != 0) {
        // connect edge
        if (isSource) {
            myNet.getEdge(myCurrentName + "-source")->addFollower(succ);
        } else {
            succ->addFollower(myNet.getEdge(myCurrentName + "-sink"));
        }
    } else {
        WRITE_ERROR("At district '" + myCurrentName + "': succeeding edge '" + id + "' does not exist.");
    }
}



/****************************************************************************/

