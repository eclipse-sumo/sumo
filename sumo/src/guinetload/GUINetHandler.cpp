//---------------------------------------------------------------------------//
//                        GUINetHandler.cpp -
//  The XML-Handler for building networks within the gui-version derived
//      from NLNetHandler
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2003/07/16 15:21:16  dkrajzew
// conversion tools splitted and relocated to avoid mandatory inclusion of unused files
//
// Revision 1.4  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.3  2003/06/18 11:08:05  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/13 15:59:00  dkrajzew
// unnecessary output of build edges id removed
//
// Revision 1.1  2003/02/07 10:38:19  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <netload/NLNetHandler.h>
#include <netload/NLContainer.h>
#include <utils/convert/TplConvert.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "GUIContainer.h"
#include "GUINetHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINetHandler::GUINetHandler(const std::string &file,
                             NLContainer &container)
    : NLNetHandler(file, container)
{
}


GUINetHandler::~GUINetHandler()
{
}


void
GUINetHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    NLNetHandler::myStartElement(element, name, attrs);
    if(wanted(LOADFILTER_NET) && element==SUMO_TAG_EDGEPOS) {
        addSourceDestinationInformation(attrs);
    }
}


void
GUINetHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
    NLNetHandler::myCharacters(element, name, chars);
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case SUMO_TAG_SHAPE:
            addJunctionShape(chars);
            break;
        case SUMO_TAG_LANE:
            addLaneShape(chars);
            break;
        default:
            break;
        }
    }
}


void
GUINetHandler::addSourceDestinationInformation(const Attributes &attrs) {
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        string from = getString(attrs, SUMO_ATTR_FROM);
        string to = getString(attrs, SUMO_ATTR_TO);
        static_cast<GUIContainer&>(myContainer).addSrcDestInfo(id, from, to);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: An edge has no information about the from/to-node");
    }
}


void
GUINetHandler::addJunctionShape(const std::string &chars)
{
    Position2DVector shape = GeomConvHelper::parseShape(chars);
    static_cast<GUIContainer&>(myContainer).addJunctionShape(shape);
}


void
GUINetHandler::addLaneShape(const std::string &chars)
{
    Position2DVector shape = GeomConvHelper::parseShape(chars);
    static_cast<GUIContainer&>(myContainer).addLaneShape(shape);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINetHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:







