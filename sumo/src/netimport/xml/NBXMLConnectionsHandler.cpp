/***************************************************************************
                          NBXMLConnectionsHandler.cpp
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
// Revision 1.1  2002/10/17 13:28:11  dkrajzew
// initial commit of classes to import connection definitions
//
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
#include "NBXMLConnectionsHandler.h"
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/TplConvertSec.h>
#include <utils/xml/XMLBuildingExceptions.h>

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
NBXMLConnectionsHandler::NBXMLConnectionsHandler(bool warn, bool verbose)
    : SUMOSAXHandler(warn, verbose)
{
}


NBXMLConnectionsHandler::~NBXMLConnectionsHandler()
{
}


void
NBXMLConnectionsHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    if(name=="connection") {
        string from = getStringSecure(attrs, SUMO_ATTR_FROM, "");
        string to = getStringSecure(attrs, SUMO_ATTR_TO, "");
        if(from.length()==0||to.length()==0) {
            SErrorHandler::add(
                "Either a from-edge or a to-edge is not specified within one of the connections");
            return;
        }
        // extract edges
        NBEdge *fromEdge = NBEdgeCont::retrieve(from);
        NBEdge *toEdge = NBEdgeCont::retrieve(to);
        // check whether they are valid
        if(fromEdge==0) {
            SErrorHandler::add(
                string("The connection-source edge '") 
                + from + string("' is not known."));
            return;
        }
        if(toEdge==0) {
            SErrorHandler::add(
                string("The connection-destination edge '") 
                + to + string("' is not known."));
            return;
        }
        // parse the id
        string type = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
        if(type=="edgebound") {
            parseEdgeBound(attrs, fromEdge, toEdge);
        } else if(type=="lanebound") {
            parseLaneBound(attrs, fromEdge, toEdge);
        } else {
            SErrorHandler::add("Unknown type of connection");
        }
    }
}

void
NBXMLConnectionsHandler::parseEdgeBound(const Attributes &attrs,
                                        NBEdge *from,
                                        NBEdge *to) 
{
    int noLanes;
    try {
        noLanes = getIntSecure(attrs, SUMO_ATTR_NOLANES, -1);
    } catch (NumberFormatException e) {
        SErrorHandler::add(
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
    }
}


void
NBXMLConnectionsHandler::parseLaneBound(const Attributes &attrs,
                                        NBEdge *from,
                                        NBEdge *to) 
{
    string laneConn = getStringSecure(attrs, SUMO_ATTR_LANE, "");
    if(laneConn.length()==0) {
        SErrorHandler::add("Not specified lane to lane connection");
        return;
    } else {
        // split the information
        StringTokenizer st(laneConn, ':');
        if(st.size()!=2) {
            SErrorHandler::add("False lane to lane connection occured.");
            return;
        }
        // get the begin and the end lane
        int fromLane;
        int toLane;
        try {
            fromLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            toLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            from->addLane2LaneConnection(fromLane, to, toLane);
        } catch (NumberFormatException) {
            SErrorHandler::add(
                string("At least one of the defined lanes was not numeric"));
        }
    }
}


void
NBXMLConnectionsHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
}


void
NBXMLConnectionsHandler::myEndElement(int element, const std::string &name)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBXMLConnectionsHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:

