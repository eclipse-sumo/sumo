/****************************************************************************/
/// @file    NIXMLTypesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIXMLTypesHandler.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for edge type information stored in XML
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
#include "NIXMLTypesHandler.h"
#include <netbuild/NBTypeCont.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>

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
NIXMLTypesHandler::NIXMLTypesHandler(NBTypeCont &tc)
        : SUMOSAXHandler("xml-types - file"),
        myTypeCont(tc), myHaveReportedAboutFunctionDeprecation(false)
{}


NIXMLTypesHandler::~NIXMLTypesHandler() throw()
{}


void
NIXMLTypesHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    if (element!=SUMO_TAG_TYPE) {
        return;
    }
    // get the id, report a warning if not given or empty...
    string id;
    if(!attrs.setIDFromAttributes("type", id), false) {
        WRITE_WARNING("No type id given... Skipping.");
        return;
    }
    // check deprecated (unused) attributes
    if(!myHaveReportedAboutFunctionDeprecation&&attrs.hasAttribute(SUMO_ATTR_FUNCTION)) {
        MsgHandler::getWarningInstance()->inform("While parsing type '" + id + "': 'function' is deprecated.\n All occurences are ignored.");
        myHaveReportedAboutFunctionDeprecation = true;
    }
    int priority = 0;
    int noLanes = 0;
    SUMOReal speed = 0;
    // get the priority
    try {
        priority = attrs.getIntSecure(SUMO_ATTR_PRIORITY, myTypeCont.getDefaultPriority());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Not numeric value for Priority (at tag ID='" + id + "').");
    }
    // get the number of lanes
    try {
        noLanes = attrs.getIntSecure(SUMO_ATTR_NOLANES, myTypeCont.getDefaultNoLanes());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Not numeric value for NoLanes (at tag ID='" + id + "').");
    }
    // get the speed
    try {
        speed = attrs.getFloatSecure(SUMO_ATTR_SPEED, (SUMOReal) myTypeCont.getDefaultSpeed());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Not numeric value for Speed (at tag ID='" + id + "').");
    }
    // build the type
    if (!MsgHandler::getErrorInstance()->wasInformed()) {
        if (!myTypeCont.insert(id, noLanes, speed, priority)) {
            MsgHandler::getErrorInstance()->inform("Duplicate type occured. ID='" + id + "'");
        }
    }
}



/****************************************************************************/

