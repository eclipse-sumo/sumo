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
#include "ROEdgeVector.h"
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
RONetHandler::RONetHandler(OptionsCont &oc, RONet &net,
                           ROAbstractEdgeBuilder &eb)
        : SUMOSAXHandler("sumo-network"),
        _options(oc), _net(net), _currentName(),
        _currentEdge(0), myEdgeBuilder(eb)
{}


RONetHandler::~RONetHandler()
{}


void
RONetHandler::myStartElement(SumoXMLTag element, const std::string&,
                             const Attributes &attrs)
{
    switch (element) {
    case SUMO_TAG_EDGE:
        // in the first step, we do need the name to allocate the edge
        // in the second, we need it to know to which edge we have to add
        //  the following edges to
        parseEdge(attrs);
        break;
    case SUMO_TAG_LANE:
        if (_process) {
            parseLane(attrs);
        }
        break;
    case SUMO_TAG_JUNCTION:
        parseJunction(attrs);
        break;
    case SUMO_TAG_CEDGE:
        if (_process) {
            parseConnEdge(attrs);
        }
        break;
    default:
        break;
    }
}


void
RONetHandler::parseEdge(const Attributes &attrs)
{
    // get the id of the edge and the edge
    try {
        _currentName = getString(attrs, SUMO_ATTR_ID);
        if (_currentName[0]==':') {
            // this is an internal edge - we will not use it
            //  !!! recheck this; internal edges may be of importance during the dua
            _currentEdge = 0;
            return;
        }
    } catch (EmptyData &) {
        throw ProcessError("An edge without an id occured within '" + getFileName() + ".");
    }
    _currentEdge = _net.getEdge(_currentName);
    if (_currentEdge==0) {
        throw ProcessError("An unknown edge occured within '" + getFileName() + ".");
    }

    // get the type of the edge
    try {
        string type = getString(attrs, SUMO_ATTR_FUNC);
        _process = true;
        if (type=="normal") {
            _currentEdge->setType(ROEdge::ET_NORMAL);
        } else if (type=="source") {
            _currentEdge->setType(ROEdge::ET_SOURCE);
        } else if (type=="sink") {
            _currentEdge->setType(ROEdge::ET_SINK);
        } else if (type=="internal") {
            _process = false;
        } else {
            throw ProcessError("Edge '" + _currentName + "' has an unknown type.");
        }
    } catch (EmptyData &) {
        throw ProcessError("Missing type in edge '" + _currentName + "'.");
    }
    // get the from-junction
    RONode *fromNode = 0;
    try {
        string from = getString(attrs, SUMO_ATTR_FROM);
        fromNode = _net.getNode(from);
        if (fromNode==0) {
            fromNode = new RONode(from);
            _net.addNode(fromNode);
        }
    } catch (EmptyData &) {
        throw ProcessError("Missing from-node in edge '" + _currentName + "'.");
    }
    // get the to-junction
    RONode *toNode = 0;
    try {
        string to = getString(attrs, SUMO_ATTR_TO);
        toNode = _net.getNode(to);
        if (toNode==0) {
            toNode = new RONode(to);
            _net.addNode(toNode);
        }
    } catch (EmptyData &) {
        throw ProcessError("Missing to-node in edge '" + _currentName + "'.");
    }
    // add the edge
    _currentEdge->setNodes(fromNode, toNode);
}


void
RONetHandler::parseLane(const Attributes &attrs)
{
    if (_currentEdge==0) {
        // was an internal edge to skip
        return;
    }
    SUMOReal maxSpeed = -1;
    SUMOReal length = -1;
    std::vector<SUMOVehicleClass> allowed, disallowed;
    // get the speed
    try {
        maxSpeed = getFloat(attrs, SUMO_ATTR_MAXSPEED);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("A lane without a maxspeed definition occured within '" + getFileName() + "', edge '" + _currentName + "'.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A lane with a nonnumerical maxspeed definition occured within '" + getFileName() + "', edge '" + _currentName + "'.");
        return;
    }
    // get the length
    try {
        length = getFloat(attrs, SUMO_ATTR_LENGTH);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("A lane without a length definition occured within '" + getFileName() + "', edge '" + _currentName + "'.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A lane with a nonnumerical length definition occured within '" + getFileName() + "', edge '" + _currentName + "'.");
        return;
    }
    // get the id
    string id = getStringSecure(attrs, SUMO_ATTR_ID, "");
    if (id.length()==0) {
        MsgHandler::getErrorInstance()->inform("Could not retrieve the id of a lane.");
        return;
    }
    // get the vehicle classes
    string allowedS = getStringSecure(attrs, SUMO_ATTR_VCLASSES , "");
    if (allowedS.length()!=0) {
        StringTokenizer st(allowedS, ";");
        while (st.hasNext()) {
            string next = st.next();
            if (next[0]=='-') {
                disallowed.push_back(getVehicleClassID(next.substr(1)));
                _net.setRestrictionFound();
            } else {
                allowed.push_back(getVehicleClassID(next));
                _net.setRestrictionFound();
            }
        }
    }
    // add when both values are valid
    if (maxSpeed>0&&length>0&&id.length()>0) {
        _currentEdge->addLane(new ROLane(id, length, maxSpeed, allowed, disallowed));
    }
}


void
RONetHandler::parseJunction(const Attributes &attrs)
{
    try {
        _currentName = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("A junction without an id occured within '" + getFileName() + "'.");
    }
}


void
RONetHandler::parseConnEdge(const Attributes &attrs)
{
    if (_currentEdge==0) {
        // was an internal edge to skip
        return;
    }
    try {
        // get the edge to connect
        string succID = getString(attrs, SUMO_ATTR_ID);
        ROEdge *succ = _net.getEdge(succID);
        if (succ!=0) {
            // connect edge
            _currentEdge->addFollower(succ);
        } else {
            MsgHandler::getErrorInstance()->inform("At edge '" + _currentName + "': the succeding edge '" + succID + "' does not exist.");
        }
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("At edge '" + _currentName + "': a succeding edge has no id.");
    }
}


void
RONetHandler::myCharacters(SumoXMLTag element, const std::string&,
                           const std::string &chars)
{
    if (element==SUMO_TAG_EDGES) {
        preallocateEdges(chars);
    }
}


void
RONetHandler::preallocateEdges(const std::string &chars)
{
    StringTokenizer st(chars);
    while (st.hasNext()) {
        string id = st.next();
        if (id[0]==':') {
            // this is an internal edge - we will not use it
            //  !!! recheck this; internal edges may be of importance during the dua
            continue;
        }
        _net.addEdge(myEdgeBuilder.buildEdge(id)); // !!! where is the edge deleted when failing?
    }
}


void
RONetHandler::myEndElement(SumoXMLTag, const std::string&)
{}



/****************************************************************************/

