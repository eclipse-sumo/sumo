/****************************************************************************/
/// @file    NISUMOHandlerEdges.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:NISUMOHandlerEdges.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for SUMO edges
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "NISUMOHandlerEdges.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS
// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

NISUMOHandlerEdges::NISUMOHandlerEdges(NBEdgeCont &ec, NBNodeCont &nc)
        : SUMOSAXHandler("sumo-network"),
        myEdgeCont(ec), myNodeCont(nc)
{}


NISUMOHandlerEdges::~NISUMOHandlerEdges() throw()
{}


void
NISUMOHandlerEdges::myStartElement(SumoXMLTag element,
                                   const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    if (element==SUMO_TAG_EDGE) {
        addEdge(attrs);
    }
}


void
NISUMOHandlerEdges::addEdge(const SUMOSAXAttributes &attrs)
{
    string id;
    try {
        // get the id
        id = attrs.getString(SUMO_ATTR_ID);
        // get the name
        string name;
        try {
            name = attrs.getString(SUMO_ATTR_NAME);
        } catch (EmptyData &) {}
        // get the type
        string type;
        try {
            type = attrs.getString(SUMO_ATTR_TYPE);
        } catch (EmptyData &) {}
        // get the origin and the destination node
        NBNode *from = getNode(attrs, SUMO_ATTR_FROMNODE, "from", id);
        NBNode *to = getNode(attrs, SUMO_ATTR_TONODE, "to", id);
        if (from==0||to==0) {
            return;
        }
        // get some other parameter
        SUMOReal speed = attrs.getFloat(SUMO_ATTR_SPEED);
        SUMOReal length = attrs.getFloat(SUMO_ATTR_LENGTH);
        int nolanes = attrs.getInt(SUMO_ATTR_NOLANES);
        int priority = attrs.getInt(SUMO_ATTR_PRIORITY);
        if (speed>0&&length>0&&nolanes>0&&priority>0) {
            myEdgeCont.insert(new NBEdge(id, name, from, to, type, speed,
                                         nolanes, priority));
        }
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("An edge with an unknown id occured.");
    }
}

NBNode *
NISUMOHandlerEdges::getNode(const SUMOSAXAttributes &attrs, SumoXMLAttr id,
                            const std::string &dir, const std::string &name)
{
    try {
        string nodename = attrs.getString(id);
        NBNode *node = myNodeCont.retrieve(nodename);
        if (node==0) {
            MsgHandler::getErrorInstance()->inform("The " + dir + "-node '" + nodename + "' used within edge '" + name + "' is not known.");
        }
        return node;
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing " + dir + "-node name for edge with id '" + name + "'");
    }
    return 0;
}



/****************************************************************************/

