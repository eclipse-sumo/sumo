/****************************************************************************/
/// @file    PCTypeDefHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A handler for loading polygon type maps
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/RGBColor.h>
#include "PCTypeMap.h"
#include "PCTypeDefHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
PCTypeDefHandler::PCTypeDefHandler(OptionsCont &oc, PCTypeMap &con) throw()
        : SUMOSAXHandler("Detector-Defintion"),
        myOptions(oc),  myContainer(con) {}


PCTypeDefHandler::~PCTypeDefHandler() throw() {}


void
PCTypeDefHandler::myStartElement(SumoXMLTag element,
                                 const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element==SUMO_TAG_POLYTYPE) {
        // get the id, report an error if not given or empty...
        std::string id;
        if (!attrs.setIDFromAttributes(id)) {
            return;
        }
        bool ok = true;
        int layer = attrs.getOptIntReporting(SUMO_ATTR_LAYER, id.c_str(), ok, myOptions.getInt("layer"));
        bool discard = attrs.getOptBoolReporting(SUMO_ATTR_DISCARD, id.c_str(), ok, false);
        bool allowFill = attrs.getOptBoolReporting(SUMO_ATTR_FILL, id.c_str(), ok, true);
        std::string type = attrs.getOptStringReporting(SUMO_ATTR_NAME, id.c_str(), ok, myOptions.getString("type"));
        std::string prefix = attrs.getOptStringReporting(SUMO_ATTR_PREFIX, id.c_str(), ok, myOptions.getString("prefix"));
        std::string color = attrs.getOptStringReporting(SUMO_ATTR_COLOR, id.c_str(), ok, myOptions.getString("color"));
        // !!! what about error handling?
        if (!myContainer.add(id, type, color, prefix, layer, discard, allowFill)) {
            MsgHandler::getErrorInstance()->inform("Could not add polygon type '" + id + "' (probably the id is already used).");
        }
    }
}


/****************************************************************************/

