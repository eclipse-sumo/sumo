/***************************************************************************
                          NIXMLNodesHandler.h
              Used to load the XML-description of the nodes given in a
           XML-format
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.14  2006/01/11 12:02:08  dkrajzew
// debugged node type specification (unfinished)
//
// Revision 1.13  2005/10/17 09:18:44  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.12  2005/10/07 11:41:16  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.11  2005/09/23 06:04:00  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 12:03:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.8  2004/11/23 10:23:51  dkrajzew
// debugging
//
// Revision 1.7  2004/08/02 12:44:28  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.6  2004/01/12 15:36:36  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.5  2003/07/07 08:33:15  dkrajzew
// further attribute added: 1:N-definition between node and tl; adapted the importer to the new node type description
//
// Revision 1.4  2003/06/19 10:59:34  dkrajzew
// error output patched
//
// Revision 1.3  2003/06/18 11:17:29  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/13 15:55:16  dkrajzew
// xml-loaders now use new options
//
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
// Revision 1.2  2002/10/17 13:30:01  dkrajzew
// possibility to specify the type of the junction added
//
// Revision 1.1  2002/10/16 15:45:36  dkrajzew
// initial commit for xml-importing classes
//
// Revision 1.7  2002/06/21 10:13:28  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/17 15:19:30  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 16:00:42  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/10 06:56:14  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:42:57  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:13  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:21:25  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:04  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIXMLNodesHandler::NIXMLNodesHandler(NBNodeCont &nc,
									 NBTrafficLightLogicCont &tlc,
									 OptionsCont &options)
    : SUMOSAXHandler("xml-nodes - file"),
    _options(options),
    myNodeCont(nc), myTLLogicCont(tlc)
{
}


NIXMLNodesHandler::~NIXMLNodesHandler()
{
}



void
NIXMLNodesHandler::myStartElement(int element, const std::string &tag,
                                  const Attributes &attrs)
{
    if(tag!="node") {
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
    if(!setPosition(attrs)) {
        return;
    }
    // get the type
    myType = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
    NBNode::BasicNodeType type = NBNode::NODETYPE_UNKNOWN;
    if(myType=="priority") {
        type = NBNode::NODETYPE_PRIORITY_JUNCTION;
    } else if(myType=="right_before_left"||myType=="right_for_left") {
        type = NBNode::NODETYPE_RIGHT_BEFORE_LEFT;
    } else if(myType=="traffic_light") {
        type = NBNode::NODETYPE_PRIORITY_JUNCTION;
    }
    // check whether there is a traffic light to assign this node to
    // build the node
    NBNode *node = new NBNode(myID, myPosition, type);
    // insert the node
    if(!myNodeCont.insert(node)) {
        if(myNodeCont.retrieve(myPosition)!=0) {
            addError(string("Duplicate node occured. ID='") + myID + string("'"));
        }
    }
    // process traffic light definition
    if(myType=="traffic_light") {
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
        myPosition.set(x, y);
    } catch (NumberFormatException) {
        addError(string("Not numeric value for position (at node ID='") + myID
            + string("')."));
        return false;
    } catch (EmptyData) {
        addError(string("Node position (at node ID='") + myID
            + string("') is not given."));
        return false;
    }
    // check whether the y-axis shall be flipped
    if(_options.getBool("flip-y")) {
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
        if(tlDef==0) {
            // this traffic light is visited the first time
            NBTrafficLightDefinition *tlDef = new NBOwnTLDef(tlID, currentNode);
            if(!myTLLogicCont.insert(tlID, tlDef)) {
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
        if(!myTLLogicCont.insert(myID, tlDef)) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

