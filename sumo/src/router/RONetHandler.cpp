/****************************************************************************/
/// @file    RONetHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The handler for SUMO-Networks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RONetHandler::RONetHandler(RONet &net,
                           ROAbstractEdgeBuilder &eb)
        : SUMOSAXHandler("sumo-network"),
        myNet(net), myCurrentName(),
        myCurrentEdge(0), myEdgeBuilder(eb)
{}


RONetHandler::~RONetHandler() throw()
{}


void
RONetHandler::myStartElement(SumoXMLTag element,
                             const SUMOSAXAttributes &attrs) throw(ProcessError)
{
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
    default:
        break;
    }
}


void
RONetHandler::parseEdge(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    if(!attrs.setIDFromAttributes("edge", myCurrentName)) {
        throw ProcessError();
    }
    // get the edge
    myCurrentEdge = 0;
    if (myCurrentName[0]==':') {
        // this is an internal edge - we will not use it
        //  !!! recheck this; internal edges may be of importance during the dua
        return;
    }
    myCurrentEdge = myEdgeBuilder.buildEdge(myCurrentName);
    myNet.addEdge(myCurrentEdge); // !!! where is the edge deleted when failing? 
    // !!! secure??
    // get the type of the edge
    try {
        string type = attrs.getString(SUMO_ATTR_FUNCTION);
        myProcess = true;
        if (type=="normal") {
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
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing type in edge '" + myCurrentName + "'.");
        return;
    }
    // get the from-junction
    RONode *fromNode = 0;
    try {
        string from = attrs.getString(SUMO_ATTR_FROM);
        if (from=="") {
            throw EmptyData();
        }
        fromNode = myNet.getNode(from);
        if (fromNode==0) {
            fromNode = new RONode(from);
            myNet.addNode(fromNode);
        }
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing from-node in edge '" + myCurrentName + "'.");
        return;
    }
    // get the to-junction
    RONode *toNode = 0;
    try {
        string to = attrs.getString(SUMO_ATTR_TO);
        if (to=="") {
            throw EmptyData();
        }
        toNode = myNet.getNode(to);
        if (toNode==0) {
            toNode = new RONode(to);
            myNet.addNode(toNode);
        }
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing to-node in edge '" + myCurrentName + "'.");
        return;
    }
    // add the edge
    myCurrentEdge->setNodes(fromNode, toNode);
}


void
RONetHandler::parseLane(const SUMOSAXAttributes &attrs)
{
    if (myCurrentEdge==0) {
        // was an internal edge to skip or an error occured
        return;
    }
    SUMOReal maxSpeed = -1;
    SUMOReal length = -1;
    std::vector<SUMOVehicleClass> allowed, disallowed;
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttributes("lane", id)) {
        return;
    }
    // get the speed
    try {
        maxSpeed = attrs.getFloat(SUMO_ATTR_MAXSPEED);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing maxspeed definition in lane '" + id + "'.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Not numerical maxspeed definition in lane '" + id + "'.");
        return;
    }
    // get the length
    try {
        length = attrs.getFloat(SUMO_ATTR_LENGTH);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing length definition in lane '" + id + "'.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Not numerical length definition in lane '" + id + "'.");
        return;
    }
    // get the vehicle classes
    string allowedS = attrs.getStringSecure(SUMO_ATTR_VCLASSES , "");
    if (allowedS.length()!=0) {
        StringTokenizer st(allowedS, ";");
        while (st.hasNext()) {
            string next = st.next();
            if (next[0]=='-') {
                disallowed.push_back(getVehicleClassID(next.substr(1)));
                myNet.setRestrictionFound();
            } else {
                allowed.push_back(getVehicleClassID(next));
                myNet.setRestrictionFound();
            }
        }
    }
    // add when both values are valid
    if (maxSpeed>0&&length>0&&id.length()>0) {
        myCurrentEdge->addLane(new ROLane(id, length, maxSpeed, allowed, disallowed));
    }
}


void
RONetHandler::parseJunction(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttributes("junction", id)) {
        return;
    }
    // get the position of the node
    bool ok = true;
    SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_X, "junction", id.c_str(), ok);
    SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_Y, "junction", id.c_str(), ok);
    if(ok) {
        RONode *n = myNet.getNode(id);
        if(n==0) {
            n = new RONode(id);
            myNet.addNode(n);
        }
        n->setPosition(Position2D(x, y));
    } else {
        throw ProcessError();
    }
}


void
RONetHandler::parseConnectingEdge(const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    string id = attrs.getString(SUMO_ATTR_EDGE);
    myCurrentEdge = myNet.getEdge(id);
    if(myCurrentEdge==0) {
        throw ProcessError("An unknown edge occured (id='" + id + "').");
    }
}


void
RONetHandler::parseConnectedEdge(const SUMOSAXAttributes &attrs)
{
    string id = attrs.getString(SUMO_ATTR_LANE);
    if(id=="SUMO_NO_DESTINATION") {
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



/****************************************************************************/

