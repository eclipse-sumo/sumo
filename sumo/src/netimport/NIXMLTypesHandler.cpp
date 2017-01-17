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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/SUMOVehicleClass.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTypeCont.h>
#include "NIXMLTypesHandler.h"

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
    switch (element) {
        case SUMO_TAG_TYPE: {
            bool ok = true;
            // get the id, report a warning if not given or empty...
            myCurrentTypeID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
            const char* const id = myCurrentTypeID.c_str();
            const std::string defType = myTypeCont.knows(myCurrentTypeID) ? myCurrentTypeID : "";
            const int priority = attrs.getOpt<int>(SUMO_ATTR_PRIORITY, id, ok, myTypeCont.getPriority(defType));
            const int numLanes = attrs.getOpt<int>(SUMO_ATTR_NUMLANES, id, ok, myTypeCont.getNumLanes(defType));
            const SUMOReal speed = attrs.getOpt<SUMOReal>(SUMO_ATTR_SPEED, id, ok, myTypeCont.getSpeed(defType));
            const std::string allowS = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id, ok, "");
            const std::string disallowS = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id, ok, "");
            const bool oneway = attrs.getOpt<bool>(SUMO_ATTR_ONEWAY, id, ok, myTypeCont.getIsOneWay(defType));
            const bool discard = attrs.getOpt<bool>(SUMO_ATTR_DISCARD, id, ok, false);
            const SUMOReal width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, id, ok, myTypeCont.getWidth(defType));
            const SUMOReal sidewalkWidth = attrs.getOpt<SUMOReal>(SUMO_ATTR_SIDEWALKWIDTH, id, ok, myTypeCont.getSidewalkWidth(defType));
            const SUMOReal bikeLaneWidth = attrs.getOpt<SUMOReal>(SUMO_ATTR_BIKELANEWIDTH, id, ok, myTypeCont.getBikeLaneWidth(defType));
            if (!ok) {
                return;
            }
            // build the type
            SVCPermissions permissions = myTypeCont.getPermissions(defType);
            if (allowS != "" || disallowS != "") {
                permissions = parseVehicleClasses(allowS, disallowS);
            }
            myTypeCont.insert(myCurrentTypeID, numLanes, speed, priority, permissions, width, oneway, sidewalkWidth, bikeLaneWidth);
            if (discard) {
                myTypeCont.markAsToDiscard(myCurrentTypeID);
            }
            SumoXMLAttr myAttrs[] = {SUMO_ATTR_PRIORITY, SUMO_ATTR_NUMLANES, SUMO_ATTR_SPEED,
                                     SUMO_ATTR_ALLOW, SUMO_ATTR_DISALLOW, SUMO_ATTR_ONEWAY,
                                     SUMO_ATTR_DISCARD, SUMO_ATTR_WIDTH, SUMO_ATTR_SIDEWALKWIDTH, SUMO_ATTR_BIKELANEWIDTH
                                    };
            for (int i = 0; i < 10; i++) {
                if (attrs.hasAttribute(myAttrs[i])) {
                    myTypeCont.markAsSet(myCurrentTypeID, myAttrs[i]);
                }
            }
            break;
        }
        case SUMO_TAG_RESTRICTION: {
            bool ok = true;
            const SUMOVehicleClass svc = getVehicleClassID(attrs.get<std::string>(SUMO_ATTR_VCLASS, myCurrentTypeID.c_str(), ok));
            const SUMOReal speed = attrs.get<SUMOReal>(SUMO_ATTR_SPEED, myCurrentTypeID.c_str(), ok);
            if (ok) {
                myTypeCont.addRestriction(myCurrentTypeID, svc, speed);
            }
            break;
        }
        default:
            break;
    }
}



/****************************************************************************/

