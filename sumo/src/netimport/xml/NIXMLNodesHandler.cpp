/****************************************************************************/
/// @file    NIXMLNodesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// }
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "NIXMLNodesHandler.h"
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBOwnTLDef.h>
#include <utils/geoconv/GeoConvHelper.h>

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
NIXMLNodesHandler::NIXMLNodesHandler(NBNodeCont &nc,
                                     NBTrafficLightLogicCont &tlc,
                                     OptionsCont &options)
        : SUMOSAXHandler("xml-nodes - file"),
        _options(options),
        myNodeCont(nc), myTLLogicCont(tlc)
{}


NIXMLNodesHandler::~NIXMLNodesHandler()
{}



void
NIXMLNodesHandler::myStartElement(int /*element*/, const std::string &tag,
                                  const Attributes &attrs)
{
    if (tag!="node") {
        return;
    }
    try {
        // retrieve the id of the node
        myID = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        WRITE_WARNING("No node id given... Skipping.");
        return;
    }
    // retrieve the name of the node
    string name = getStringSecure(attrs, SUMO_ATTR_NAME, myID);
    // retrieve the position of the node
    if (!setPosition(attrs)) {
        return;
    }
    // get the type
    myType = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
    NBNode::BasicNodeType type = NBNode::NODETYPE_UNKNOWN;
    if (myType=="priority") {
        type = NBNode::NODETYPE_PRIORITY_JUNCTION;
    } else if (myType=="right_before_left"||myType=="right_for_left") {
        type = NBNode::NODETYPE_RIGHT_BEFORE_LEFT;
    } else if (myType=="traffic_light") {
        type = NBNode::NODETYPE_TRAFFIC_LIGHT;
    }
    // check whether there is a traffic light to assign this node to
    // build the node
    NBNode *node = new NBNode(myID, myPosition, type);
    // insert the node
    if (!myNodeCont.insert(node)) {
        if (myNodeCont.retrieve(myPosition)!=0) {
            addError("Duplicate node occured. ID='" + myID + "'");
        }
    }
    // process traffic light definition
    if (type==NBNode::NODETYPE_TRAFFIC_LIGHT) {
        processTrafficLightDefinitions(attrs, node);
    }
}



bool
NIXMLNodesHandler::setPosition(const Attributes &attrs)
{
    // retrieve the positions
    try {
        SUMOReal x = getFloat(attrs, SUMO_ATTR_X);
        SUMOReal y = getFloat(attrs, SUMO_ATTR_Y);
        myNodeCont.addGeoreference(Position2D((SUMOReal)(x / 100000.0), (SUMOReal)(y / 100000.0)));
        myPosition.set(x, y);
        GeoConvHelper::remap(myPosition);
    } catch (NumberFormatException) {
        addError("Not numeric value for position (at node ID='" + myID + "').");
        return false;
    } catch (EmptyData) {
        addError("Node position (at node ID='" + myID + "') is not given.");
        return false;
    }
    // check whether the y-axis shall be flipped
    if (_options.getBool("flip-y")) {
        myPosition.mul(1.0, -1.0);
    }
    return true;
}


void
NIXMLNodesHandler::processTrafficLightDefinitions(const Attributes &attrs,
        NBNode *currentNode)
{
    NBTrafficLightDefinition *tlDef = 0;
    try {
        string tlID = getString(attrs, SUMO_ATTR_TLID);
        // ok, the traffic light has a name
        tlDef = myTLLogicCont.getDefinition(tlID);
        if (tlDef==0) {
            // this traffic light is visited the first time
            NBTrafficLightDefinition *tlDef = new NBOwnTLDef(tlID, currentNode);
            if (!myTLLogicCont.insert(tlID, tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError();
            }
        } else {
            tlDef->addNode(currentNode);
        }
    } catch (EmptyData) {
        // ok, this node is a traffic light node where no other nodes
        //  participate
        NBTrafficLightDefinition *tlDef = new NBOwnTLDef(myID, currentNode);
        if (!myTLLogicCont.insert(myID, tlDef)) {
            // actually, nothing should fail here
            delete tlDef;
            throw ProcessError();
        }
    }
    // inform the node
    currentNode->addTrafficLight(tlDef);
}


void
NIXMLNodesHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
    myCharactersDump(element, name, chars);
}


void
NIXMLNodesHandler::myEndElement(int element, const std::string &name)
{
    myEndElementDump(element, name);
}



/****************************************************************************/

