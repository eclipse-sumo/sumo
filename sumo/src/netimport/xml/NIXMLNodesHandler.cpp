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
// Revision 1.7  2004/08/02 12:44:28  dkrajzew
// using Position2D instead of two doubles
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
// Windows eol removed; minor double to int conversions removed;
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
 * included modules
 * ======================================================================= */
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
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBOwnTLDef.h>


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC       // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIXMLNodesHandler::NIXMLNodesHandler(OptionsCont &options)
    : SUMOSAXHandler("xml-nodes - file"),
    _options(options)
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
        MsgHandler::getWarningInstance()->inform(
            "No node id given... Skipping.");
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
    // check whether there is a traffic light to assign this node to
    // build the node
    NBNode *node = new NBNode(myID, myPosition);
    // insert the node
    if(!NBNodeCont::insert(node)) {
        if(NBNodeCont::retrieve(myPosition)!=0) {
            addError(string("Duplicate node occured. ID='") + myID
                + string("'"));
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
        double x = getFloat(attrs, SUMO_ATTR_X);
        double y = getFloat(attrs, SUMO_ATTR_Y);
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
        tlDef = NBTrafficLightLogicCont::getDefinition(tlID);
        if(tlDef==0) {
            // this traffic light is visited the first time
            NBTrafficLightDefinition *tlDef =
                new NBOwnTLDef(tlID, currentNode);
            if(!NBTrafficLightLogicCont::insert(tlID, tlDef)) {
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
        NBTrafficLightDefinition *tlDef =
            new NBOwnTLDef(myID, currentNode);
        if(!NBTrafficLightLogicCont::insert(myID, tlDef)) {
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

