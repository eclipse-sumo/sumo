/****************************************************************************/
/// @file    RONetHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The handler for SUMO-Networks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
RONetHandler::RONetHandler(RONet &net,
                           ROAbstractEdgeBuilder &eb)
        : SUMOSAXHandler("sumo-network"),
        myNet(net), myCurrentName(),
        myCurrentEdge(0), myEdgeBuilder(eb), myHaveWarnedAboutDeprecatedVClass(false) {}


RONetHandler::~RONetHandler() throw() {}


void
RONetHandler::myStartElement(SumoXMLTag element,
                             const SUMOSAXAttributes &attrs) throw(ProcessError) {
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
    case SUMO_TAG_SUCC:
        parseConnectingEdge(attrs);
        break;
    case SUMO_TAG_SUCCLANE:
        parseConnectedEdge(attrs);
        break;
    case SUMO_TAG_DISTRICT:
        parseDistrict(attrs);
        break;
    case SUMO_TAG_DSOURCE:
        parseDistrictEdge(attrs, true);
        break;
    case SUMO_TAG_DSINK:
        parseDistrictEdge(attrs, false);
        break;
    default:
        break;
    }
}


void
RONetHandler::parseEdge(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    if (!attrs.setIDFromAttributes("edge", myCurrentName)) {
        throw ProcessError();
    }
    // get the edge
    myCurrentEdge = 0;
    if (myCurrentName[0]==':') {
        // this is an internal edge - we will not use it
        //  !!! recheck this; internal edges may be of importance during the dua
        return;
    }
    bool ok = true;
    std::string from = attrs.getStringReporting(SUMO_ATTR_FROM, "edge", myCurrentName.c_str(), ok);
    std::string to = attrs.getStringReporting(SUMO_ATTR_TO, "edge", myCurrentName.c_str(), ok);
    std::string type = attrs.getStringReporting(SUMO_ATTR_FUNCTION, "edge", myCurrentName.c_str(), ok);
    if (!ok) {
        return;
    }
    RONode *fromNode = myNet.getNode(from);
    if (fromNode==0) {
        fromNode = new RONode(from);
        myNet.addNode(fromNode);
    }
    RONode *toNode = myNet.getNode(to);
    if (toNode==0) {
        toNode = new RONode(to);
        myNet.addNode(toNode);
    }
    // build the edge
    myCurrentEdge = myEdgeBuilder.buildEdge(myCurrentName, fromNode, toNode);
	if(myNet.addEdge(myCurrentEdge)) {
	    // get the type of the edge
	    myProcess = true;
		if (type=="normal"||type=="connector") {
			myCurrentEdge->setType(ROEdge::ET_NORMAL);
		} else if (type=="source") {
			myCurrentEdge->setType(ROEdge::ET_SOURCE);
		} else if (type=="sink") {
			myCurrentEdge->setType(ROEdge::ET_SINK);
		} else if (type=="internal") {
			myProcess = false;
		} else {
			MsgHandler::getErrorInstance()->inform("Edge '" + myCurrentName + "' has an unknown type.");
			return;
		}
	} else {
		myCurrentEdge = 0;
	}
}


void
RONetHandler::parseLane(const SUMOSAXAttributes &attrs) {
    if (myCurrentEdge==0) {
        // was an internal edge to skip or an error occured
        return;
    }
    std::vector<SUMOVehicleClass> allowed, disallowed;
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("lane", id)) {
        return;
    }
    // get the speed
    bool ok = true;
    SUMOReal maxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED, "lane", id.c_str(), ok);
    SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, "lane", id.c_str(), ok);
    std::string vclasses = attrs.getOptStringReporting(SUMO_ATTR_VCLASSES, "lane", id.c_str(), ok, "");
    std::string allow = attrs.getOptStringReporting(SUMO_ATTR_ALLOW, "lane", id.c_str(), ok, "");
    std::string disallow = attrs.getOptStringReporting(SUMO_ATTR_DISALLOW, "lane", id.c_str(), ok, "");
    if (!ok) {
        return;
    }
    // get the length
    // get the vehicle classes
    parseVehicleClasses(vclasses, allow, disallow,
                        allowed, disallowed, myHaveWarnedAboutDeprecatedVClass);
    if (allowed.size()!=0 || disallowed.size() != 0) {
        myNet.setRestrictionFound();
    }
    // add when both values are valid
    if (maxSpeed>0&&length>0&&id.length()>0) {
        myCurrentEdge->addLane(new ROLane(id, length, maxSpeed, allowed, disallowed));
    }
}


void
RONetHandler::parseJunction(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("junction", id)) {
        return;
    }
    // get the position of the node
    bool ok = true;
    SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_X, "junction", id.c_str(), ok);
    SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_Y, "junction", id.c_str(), ok);
    if (ok) {
        RONode *n = myNet.getNode(id);
        if (n==0) {
            n = new RONode(id);
            myNet.addNode(n);
        }
        n->setPosition(Position2D(x, y));
    } else {
        throw ProcessError();
    }
}


void
RONetHandler::parseConnectingEdge(const SUMOSAXAttributes &attrs) throw(ProcessError) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_EDGE, 0,0, ok);
    if (id[0]==':') {
        myCurrentEdge = 0;
        return;
    }
    myCurrentEdge = myNet.getEdge(id);
    if (myCurrentEdge==0) {
        throw ProcessError("An unknown edge occured (id='" + id + "').");
    }
}


void
RONetHandler::parseConnectedEdge(const SUMOSAXAttributes &attrs) {
    if (myCurrentEdge==0) {
        // earlier error or internal link
        return;
    }
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_LANE, "lane", myCurrentName.c_str(), ok);
    if (id=="SUMO_NO_DESTINATION") {
        return;
    }
    ROEdge *succ = myNet.getEdge(id.substr(0, id.rfind('_')));
    if (succ!=0) {
        // connect edge
        myCurrentEdge->addFollower(succ);
    } else {
        MsgHandler::getErrorInstance()->inform("At edge '" + myCurrentName + "': succeeding edge '" + id + "' does not exist.");
    }
}


void
RONetHandler::parseDistrict(const SUMOSAXAttributes &attrs) throw(ProcessError) {
	myCurrentEdge = 0;
    if (!attrs.setIDFromAttributes("district", myCurrentName)) {
        return;
    }
    ROEdge *sink = myEdgeBuilder.buildEdge(myCurrentName + "-sink", 0, 0);
    sink->setType(ROEdge::ET_DISTRICT);
    myNet.addEdge(sink);
    ROEdge *source = myEdgeBuilder.buildEdge(myCurrentName + "-source", 0, 0);
    source->setType(ROEdge::ET_DISTRICT);
    myNet.addEdge(source);
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        std::vector<std::string> desc = StringTokenizer(attrs.getString(SUMO_ATTR_EDGES)).getVector();
        for (std::vector<std::string>::const_iterator i=desc.begin(); i!=desc.end(); ++i) {
            ROEdge *edge = myNet.getEdge(*i);
            // check whether the edge exists
            if (edge==0) {
                throw ProcessError("The edge '" + *i + "' within district '" + myCurrentName + "' is not known.");
            }
            source->addFollower(edge);
            edge->addFollower(sink);
        }
    }
}


void
RONetHandler::parseDistrictEdge(const SUMOSAXAttributes &attrs, bool isSource) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, "district", myCurrentName.c_str(), ok);
    ROEdge *succ = myNet.getEdge(id);
    if (succ!=0) {
        // connect edge
        if (isSource) {
            myNet.getEdge(myCurrentName+"-source")->addFollower(succ);
        } else {
            succ->addFollower(myNet.getEdge(myCurrentName+"-sink"));
        }
    } else {
        MsgHandler::getErrorInstance()->inform("At district '" + myCurrentName + "': succeeding edge '" + id + "' does not exist.");
    }
}



/****************************************************************************/

