/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    OptionHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// The XML-Handler for loading options
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/shapes/Shape.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>

#include "OptionHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

OptionHandler::OptionHandler() {}


OptionHandler::~OptionHandler() {}


bool
OptionHandler::beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
    // open SUMOBaseOBject
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            // Stopping Places
            case SUMO_TAG_BUS_STOP:
                //parseBusStopAttributes(attrs);
                break;
            default:
                // tag cannot be parsed in OptionHandler
                return false;
                break;
        }
    } catch (InvalidArgument& e) {
        writeError(e.what());
    }
    return true;
}


void
OptionHandler::endParseAttributes() {
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // close SUMOBaseOBject
    myCommonXMLStructure.closeSUMOBaseOBject();
    // check tag
    switch (obj->getTag()) {
        // Stopping Places
        case SUMO_TAG_BUS_STOP:
       
        case SUMO_TAG_POI:
            // parse object and all their childrens
            parseSumoBaseObject(obj);
            // delete object (and all of their childrens)
            delete obj;
            break;
        default:
            break;
    }
}


void
OptionHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
/*
    // switch tag
    switch (obj->getTag()) {
        // Stopping Places
        case SUMO_TAG_BUS_STOP:
            buildBusStop(obj,
                         obj->getStringAttribute(SUMO_ATTR_ID),
                         obj->getStringAttribute(SUMO_ATTR_LANE),
                         obj->getDoubleAttribute(SUMO_ATTR_STARTPOS),
                         obj->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                         obj->getStringAttribute(SUMO_ATTR_NAME),
                         obj->getStringListAttribute(SUMO_ATTR_LINES),
                         obj->getIntAttribute(SUMO_ATTR_PERSON_CAPACITY),
                         obj->getDoubleAttribute(SUMO_ATTR_PARKING_LENGTH),
                         obj->getColorAttribute(SUMO_ATTR_COLOR),
                         obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                         obj->getParameters());
            break;
        default:
        
        break;
    }
    // now iterate over childrens
    for (const auto& child : obj->getSumoBaseObjectChildren()) {
        // call this function recursively
        parseSumoBaseObject(child);
    }
*/
}


bool
OptionHandler::isErrorCreatingElement() const {
    return myErrorCreatingElement;
}


void
OptionHandler::writeError(const std::string& error) {
    WRITE_ERROR(error);
    myErrorCreatingElement = true;
}

/****************************************************************************/
