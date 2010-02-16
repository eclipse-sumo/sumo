/****************************************************************************/
/// @file    NIXMLTypesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for edge type information stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
        myTypeCont(tc), myHaveReportedAboutFunctionDeprecation(false) {}


NIXMLTypesHandler::~NIXMLTypesHandler() throw() {}


void
NIXMLTypesHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element!=SUMO_TAG_TYPE) {
        return;
    }
    // get the id, report a warning if not given or empty...
    string id;
    if (!attrs.setIDFromAttributes("type", id), false) {
        WRITE_WARNING("No type id given... Skipping.");
        return;
    }
    // check deprecated (unused) attributes
    if (!myHaveReportedAboutFunctionDeprecation&&attrs.hasAttribute(SUMO_ATTR_FUNCTION)) {
        MsgHandler::getWarningInstance()->inform("While parsing type '" + id + "': 'function' is deprecated.\n All occurences are ignored.");
        myHaveReportedAboutFunctionDeprecation = true;
    }

    bool ok = true;
    int priority = attrs.getOptIntReporting(SUMO_ATTR_PRIORITY, "type", id.c_str(), ok, myTypeCont.getDefaultPriority());
    int noLanes = attrs.getOptIntReporting(SUMO_ATTR_NOLANES, "type", id.c_str(), ok, myTypeCont.getDefaultNoLanes());
    SUMOReal speed = attrs.getOptSUMORealReporting(SUMO_ATTR_SPEED, "type", id.c_str(), ok, (SUMOReal) myTypeCont.getDefaultSpeed());
    bool discard = attrs.getOptBoolReporting(SUMO_ATTR_DISCARD, 0, 0, ok, false);
    if (!ok) {
        return;
    }
    // build the type
    if (!myTypeCont.insert(id, noLanes, speed, priority)) {
        MsgHandler::getErrorInstance()->inform("Duplicate type occured. ID='" + id + "'");
    } else {
        if (discard) {
            myTypeCont.markAsToDiscard(id);
        }
    }
}



/****************************************************************************/

