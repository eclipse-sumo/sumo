/***************************************************************************
                          NIXMLConnectionsHandler.cpp
			  Used to parse the XML-descriptions of connections between
              edges or lanes given in a XML-format
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Thu, 17 Oct 2002
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
// Revision 1.5  2005/07/12 12:35:23  dkrajzew
// elmar2 importer included; debugging
//
// Revision 1.4  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2003/06/18 11:17:29  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/03/17 14:23:26  dkrajzew
//
// unneeded connection-type parameter usage removed
//
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
// Revision 1.1  2002/10/17 13:28:11  dkrajzew
// initial commit of classes to import connection definitions
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NIXMLConnectionsHandler.h"
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <utils/common/StringTokenizer.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/convert/ToString.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/TplConvertSec.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIXMLConnectionsHandler::NIXMLConnectionsHandler(NBEdgeCont &ec)
    : SUMOSAXHandler("xml-connection-description"), myEdgeCont(ec)
{
}


NIXMLConnectionsHandler::~NIXMLConnectionsHandler()
{
}


void
NIXMLConnectionsHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    if(name=="connection") {
        string from = getStringSecure(attrs, SUMO_ATTR_FROM, "");
        string to = getStringSecure(attrs, SUMO_ATTR_TO, "");
        if(from.length()==0||to.length()==0) {
            addError(
                "Either a from-edge or a to-edge is not specified within one of the connections");
            return;
        }
        // extract edges
        NBEdge *fromEdge = myEdgeCont.retrieve(from);
        NBEdge *toEdge = myEdgeCont.retrieve(to);
        // check whether they are valid
        if(fromEdge==0) {
            addError(
                string("The connection-source edge '")
                + from + string("' is not known."));
            return;
        }
        if(toEdge==0) {
            addError(
                string("The connection-destination edge '")
                + to + string("' is not known."));
            return;
        }
        // parse the id
        string type = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
        string laneConn = getStringSecure(attrs, SUMO_ATTR_LANE, "");
        if(type=="edgebound"||laneConn=="") {
            parseEdgeBound(attrs, fromEdge, toEdge);
        } else if(type=="lanebound"||laneConn.size()!=0) {
            parseLaneBound(attrs, fromEdge, toEdge);
        } else {
            addError("Unknown type of connection");
        }
    }
}

void
NIXMLConnectionsHandler::parseEdgeBound(const Attributes &attrs,
                                        NBEdge *from,
                                        NBEdge *to)
{
    from->addEdge2EdgeConnection(to);
/*    int noLanes;
    try {
        noLanes = getIntSecure(attrs, SUMO_ATTR_NOLANES, -1);
    } catch (NumberFormatException e) {
        addError(
            string("Not numeric lane in connection"));
        return;
    }
    if(noLanes<0) {
        // !!! (what to do??)
    } else {
        // add connection
        for(size_t i=0; i<noLanes; i++) {
            from->addLane2LaneConnection(i, to, i);
        }
    }*/
}


void
NIXMLConnectionsHandler::parseLaneBound(const Attributes &attrs,
                                        NBEdge *from,
                                        NBEdge *to)
{
    string laneConn = getStringSecure(attrs, SUMO_ATTR_LANE, "");
    if(laneConn.length()==0) {
        addError("Not specified lane to lane connection");
        return;
    } else {
        // split the information
        StringTokenizer st(laneConn, ':');
        if(st.size()!=2) {
            addError("False lane to lane connection occured.");
            return;
        }
        // get the begin and the end lane
        int fromLane;
        int toLane;
        try {
            fromLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            toLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            if(!from->addLane2LaneConnection(fromLane, to, toLane)) {
                MsgHandler::getWarningInstance()->inform(
                    "Could not set loaded connection from '"
                    + from->getID() + "_" + toString<int>(fromLane)
                    + "' to '"
                    + to->getID() + "_" + toString<int>(toLane)
                    + "'.");
            }
        } catch (NumberFormatException) {
            addError(
                string("At least one of the defined lanes was not numeric"));
        }
    }
}


void
NIXMLConnectionsHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
}


void
NIXMLConnectionsHandler::myEndElement(int element, const std::string &name)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

