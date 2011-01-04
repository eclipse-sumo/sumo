/****************************************************************************/
/// @file    ODDistrictHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// An XML-Handler for districts
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
#include <utility>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "ODDistrict.h"
#include "ODDistrictCont.h"
#include "ODDistrictHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ODDistrictHandler::ODDistrictHandler(ODDistrictCont &cont,
                                     const std::string &file) throw()
        : SUMOSAXHandler(file), myContainer(cont), myCurrentDistrict(0) {}


ODDistrictHandler::~ODDistrictHandler() throw() {}


void
ODDistrictHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_DISTRICT:
        openDistrict(attrs);
        break;
    case SUMO_TAG_DSOURCE:
        addSource(attrs);
        break;
    case SUMO_TAG_DSINK:
        addSink(attrs);
        break;
    default:
        break;
    }
}


void
ODDistrictHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_DISTRICT) {
        closeDistrict();
    }
}


void
ODDistrictHandler::openDistrict(const SUMOSAXAttributes &attrs) throw() {
    myCurrentDistrict = 0;
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("district", id)) {
        return;
    }
    myCurrentDistrict = new ODDistrict(id);
}


void
ODDistrictHandler::addSource(const SUMOSAXAttributes &attrs) throw() {
    std::pair<std::string, SUMOReal> vals = parseConnection(attrs, "source");
    if (vals.second>=0) {
        myCurrentDistrict->addSource(vals.first, vals.second);
    }
}


void
ODDistrictHandler::addSink(const SUMOSAXAttributes &attrs) throw() {
    std::pair<std::string, SUMOReal> vals = parseConnection(attrs, "sink");
    if (vals.second>=0) {
        myCurrentDistrict->addSink(vals.first, vals.second);
    }
}



std::pair<std::string, SUMOReal>
ODDistrictHandler::parseConnection(const SUMOSAXAttributes &attrs, const std::string &type) throw() {
    // check the current district first
    if (myCurrentDistrict==0) {
        return std::pair<std::string, SUMOReal>("", -1);
    }
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(type.c_str(), id)) {
        return std::pair<std::string, SUMOReal>("", -1);
    }
    // get the weight
    bool ok = true;
    SUMOReal weight = attrs.getSUMORealReporting(SUMO_ATTR_WEIGHT, type.c_str(), id.c_str(), ok);
    if (ok) {
        if (weight<0) {
            MsgHandler::getErrorInstance()->inform("'probability' must be positive (in definition of " + type + " '" + id + "').");
        } else {
            return std::pair<std::string, SUMOReal>(id, weight);
        }
    }
    return std::pair<std::string, SUMOReal>("", -1);
}


void
ODDistrictHandler::closeDistrict() throw() {
    if (myCurrentDistrict!=0) {
        myContainer.add(myCurrentDistrict->getID(), myCurrentDistrict);
    }
}



/****************************************************************************/

