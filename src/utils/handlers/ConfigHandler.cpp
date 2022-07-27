/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    ConfigHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2022
///
// The XML-Handler for SUMOConfig loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/shapes/Shape.h>
#include <utils/options/OptionsCont.h>

#include "ConfigHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

ConfigHandler::ConfigHandler() {}


ConfigHandler::~ConfigHandler() {}


bool
ConfigHandler::beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
    // open SUMOBaseOBject
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            // Stopping Places
            case SUMO_TAG_CONFIGURATION:
                parseConfiguration(attrs);
                break;
            case SUMO_TAG_NETFILE:
                parseNetFile(attrs);
                break;
            case SUMO_TAG_ADDITIONALFILES:
                parseAdditionalFiles(attrs);
                break;
            case SUMO_TAG_ROUTEFILES:
                parseRouteFiles(attrs);
                break;
            default:
                // tag cannot be parsed in ConfigHandler
                return false;
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
    return true;
}


void
ConfigHandler::endParseAttributes() {
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // close SUMOBaseOBject
    myCommonXMLStructure.closeSUMOBaseOBject();
    // check tag
    if (obj->getTag() == SUMO_TAG_CONFIGURATION) {
        parseConfigObject(obj);
        // delete object (and all of their childrens)
        delete obj;
    }
}


void 
ConfigHandler::parseConfigObject(CommonXMLStructure::SumoBaseObject* obj) {
    //
}


void
ConfigHandler::parseBusStopAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    // optional attributes
    const double startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const double endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, std::vector<std::string>());
    const int personCapacity = attrs.getOpt<int>(SUMO_ATTR_PERSON_CAPACITY, id.c_str(), parsedOk, 6);
    const double parkingLength = attrs.getOpt<double>(SUMO_ATTR_PARKING_LENGTH, id.c_str(), parsedOk, 0);
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk, RGBColor::INVISIBLE);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_BUS_STOP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_PERSON_CAPACITY, personCapacity);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PARKING_LENGTH, parkingLength);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}

/****************************************************************************/
