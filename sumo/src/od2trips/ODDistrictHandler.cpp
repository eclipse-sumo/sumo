/****************************************************************************/
/// @file    ODDistrictHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// An XML-Handler for districts
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ODDistrictHandler::ODDistrictHandler(ODDistrictCont &cont,
                                     const std::string &file) throw()
        : SUMOSAXHandler(file), myContainer(cont), myCurrentDistrict(0)
{}


ODDistrictHandler::~ODDistrictHandler() throw()
{}


void
ODDistrictHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError)
{
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
ODDistrictHandler::myEndElement(SumoXMLTag element) throw(ProcessError)
{
    if (element==SUMO_TAG_DISTRICT) {
        closeDistrict();
    }
}


void
ODDistrictHandler::openDistrict(const SUMOSAXAttributes &attrs) throw()
{
    myCurrentDistrict = 0;
    try {
        myCurrentDistrict = new ODDistrict(attrs.getString(SUMO_ATTR_ID));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("A district without an id occured.");
    }
}


void
ODDistrictHandler::addSource(const SUMOSAXAttributes &attrs) throw()
{
    std::pair<std::string, SUMOReal> vals = parseConnection(attrs, "source");
    if (vals.second>=0) {
        myCurrentDistrict->addSource(vals.first, vals.second);
    }
}


void
ODDistrictHandler::addSink(const SUMOSAXAttributes &attrs) throw()
{
    std::pair<std::string, SUMOReal> vals = parseConnection(attrs, "sink");
    if (vals.second>=0) {
        myCurrentDistrict->addSink(vals.first, vals.second);
    }
}



std::pair<std::string, SUMOReal>
ODDistrictHandler::parseConnection(const SUMOSAXAttributes &attrs, const std::string &type) throw()
{
    // check the current district first
    if (myCurrentDistrict==0) {
        return std::pair<std::string, SUMOReal>("", -1);
    }
    // get the id first
    string id;
    try {
        id = attrs.getString(SUMO_ATTR_ID);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("A " + type + " without an id occured within district '" + myCurrentDistrict->getID() + "'.");
        return std::pair<std::string, SUMOReal>("", -1);
    }
    // get the weight
    try {
        SUMOReal weight = attrs.getFloatSecure(SUMO_ATTR_WEIGHT, -1);
        if (weight==-1) {
            MsgHandler::getErrorInstance()->inform("The weight of the " + type + " '" + id + "' within district '" + myCurrentDistrict->getID() + "' is not given or <0.");
            return std::pair<std::string, SUMOReal>("", -1);
        }
        // return the values
        return std::pair<std::string, SUMOReal>(id, weight);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The weight of the " + type + " '" + id + "' within district '" + myCurrentDistrict->getID() + "' is not numeric.");
    }
    return std::pair<std::string, SUMOReal>("", -1);
}


void
ODDistrictHandler::closeDistrict() throw()
{
    if (myCurrentDistrict!=0) {
        myContainer.add(myCurrentDistrict->getID(), myCurrentDistrict);
    }
}



/****************************************************************************/

