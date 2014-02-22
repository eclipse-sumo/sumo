/****************************************************************************/
/// @file    NIXMLTypesHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for edge type information stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/common/ToString.h>
#include <utils/common/SUMOVehicleClass.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLTypesHandler::NIXMLTypesHandler(NBTypeCont& tc)
    : SUMOSAXHandler("xml-types - file"),
      myTypeCont(tc) {}


NIXMLTypesHandler::~NIXMLTypesHandler() {}


void
NIXMLTypesHandler::myStartElement(int element,
                                  const SUMOSAXAttributes& attrs) {
    if (element != SUMO_TAG_TYPE) {
        return;
    }
    bool ok = true;
    // get the id, report a warning if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    int priority = attrs.getOpt<int>(SUMO_ATTR_PRIORITY, id.c_str(), ok, myTypeCont.getPriority(""));
    int noLanes = myTypeCont.getNumLanes("");
    noLanes = attrs.getOpt<int>(SUMO_ATTR_NUMLANES, id.c_str(), ok, noLanes);
    SUMOReal speed = attrs.getOpt<SUMOReal>(SUMO_ATTR_SPEED, id.c_str(), ok, (SUMOReal) myTypeCont.getSpeed(""));
    std::string allowS = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id.c_str(), ok, "");
    std::string disallowS = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    bool oneway = attrs.getOpt<bool>(SUMO_ATTR_ONEWAY, id.c_str(), ok, false);
    bool discard = attrs.getOpt<bool>(SUMO_ATTR_DISCARD, id.c_str(), ok, false);
    SUMOReal width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, id.c_str(), ok, NBEdge::UNSPECIFIED_WIDTH);
    if (!ok) {
        return;
    }
    // build the type
    SVCPermissions permissions = parseVehicleClasses(allowS, disallowS);
    if (!myTypeCont.insert(id, noLanes, speed, priority, permissions, width, oneway)) {
        WRITE_ERROR("Duplicate type occured. ID='" + id + "'");
    } else {
        if (discard) {
            myTypeCont.markAsToDiscard(id);
        }
    }
}



/****************************************************************************/

