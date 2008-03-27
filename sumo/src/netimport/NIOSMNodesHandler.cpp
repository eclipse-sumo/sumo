/****************************************************************************/
/// @file    NIOSMNodesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIOSMNodesHandler.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include "NIOSMNodesHandler.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NBNodeCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBOwnTLDef.h>
#include <utils/geom/GeoConvHelper.h>

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
NIOSMNodesHandler::NIOSMNodesHandler(std::map<int, NIOSMNode*> &toFill,
                                     OptionsCont &options)
        : SUMOSAXHandler("osm-nodes - file"),
        myOptions(options),
        myToFill(toFill)
{}


NIOSMNodesHandler::~NIOSMNodesHandler() throw()
{}



void
NIOSMNodesHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    if (element!=SUMO_TAG_NODE) {
        return;
    }
    int id;
    try {
        // retrieve the id of the node
        id = attrs.getInt(SUMO_ATTR_ID);
    } catch (EmptyData &) {
        WRITE_WARNING("No node id given... Skipping.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A node id is not numeric.");
        return;
    }
    NIOSMNode *toAdd = new NIOSMNode();
    toAdd->id = id;
    try {
        toAdd->lon = attrs.getFloat(SUMO_ATTR_LON);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Node '" + toString(id) + "' has no lon information.");
        delete toAdd;
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Node's '" + toString(id) + "' lon information is not numeric.");
        delete toAdd;
        return;
    }
    try {
        toAdd->lat = attrs.getFloat(SUMO_ATTR_LAT);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Node '" + toString(id) + "' has no lat information.");
        delete toAdd;
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Node's '" + toString(id) + "' lat information is not numeric.");
        delete toAdd;
        return;
    }
    if (myToFill.find(id)!=myToFill.end()) {
        MsgHandler::getErrorInstance()->inform("Node id '" + toString(id) + "' is duplicate.");
        delete toAdd;
        return;
    }
    myToFill[toAdd->id] = toAdd;
}



/****************************************************************************/

