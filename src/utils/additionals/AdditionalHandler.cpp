/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    AdditionalHandler.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2015
///
// The XML-Handler for network loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>

#include "AdditionalHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

AdditionalHandler::AdditionalHandler(const std::string& file) :
    SUMOSAXHandler(file) {
}


AdditionalHandler::~AdditionalHandler() {}


bool
AdditionalHandler::parse() {
    // run parser and save result
    const bool parserResult = XMLSubSys::runParser(*this, getFileName());
    // now parse over SumoBaseObjects
    if (myCommonXMLStructure.getSumoBaseObjectRoot()) {
        parseSumoBaseObject(myCommonXMLStructure.getSumoBaseObjectRoot());
    }

    return parserResult;
}


void 
AdditionalHandler::buildE1Detector(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
    const std::string &id, const std::string &laneId, const double position,
    const SUMOTime frequency, const std::string &file, const std::string &vehicleTypes,
    const std::string &name, const bool friendlyPos) {

}


void
AdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag> (element);
    // check tag
    try {
        switch (tag) {
            case SUMO_TAG_E1DETECTOR:
                parseE1Attributes(attrs);
                break;
            /* case SUMO_TAG_BUSSTOP
               case SUMO_TAG_CHARGINGSTATION
               .....
            */
            case SUMO_TAG_PARAM:
                parseParameters(attrs);
                break;
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void
AdditionalHandler::myEndElement(int /*element*/) {
    // just close node
    myCommonXMLStructure.closeTag();
}


void 
AdditionalHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        case SUMO_TAG_E1DETECTOR:
            // build E1
            buildE1Detector(obj,
                obj->getStringAttribute(SUMO_ATTR_ID),
                obj->getStringAttribute(SUMO_ATTR_LANE),
                obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                obj->getSUMOTimeAttribute(SUMO_ATTR_FREQUENCY),
                obj->getStringAttribute(SUMO_ATTR_FILE),
                obj->getStringAttribute(SUMO_ATTR_VTYPES),
                obj->getStringAttribute(SUMO_ATTR_NAME),
                obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS));
            break;
        default:
            break;
    }
    // now iterate over childrens
    for (const auto &child : obj->getSumoBaseObjectChildren()) {
        // call this function recursively
        parseSumoBaseObject(child);
    }
}


void
AdditionalHandler::parseE1Attributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // now obtain attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk, false);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk, false);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, id.c_str(), parsedOk, false);
    const SUMOTime frequency = attrs.get<SUMOTime>(SUMO_ATTR_FREQUENCY, id.c_str(), parsedOk, false);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk, false);
    const std::string vehicleTypes = attrs.get<std::string>(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, false);
    const std::string name = attrs.get<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, false);
    const bool friendlyPos = attrs.get<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk && myCommonXMLStructure.getLastInsertedSumoBaseObject()) {
        // first open tag
        myCommonXMLStructure.openTag(SUMO_TAG_E1DETECTOR);
        // add all attributes
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addSUMOTimeAttribute(SUMO_ATTR_FREQUENCY, frequency);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void 
AdditionalHandler::parseParameters(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // get key
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, parsedOk);
    // continue if key was sucesfully loaded
    if (parsedOk && myCommonXMLStructure.getLastInsertedSumoBaseObject()) {
        // get tag str
        const std::string tagStr = toString(myCommonXMLStructure.getLastInsertedSumoBaseObject()->getTag());
        // circumventing empty string value
        const std::string value = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // show warnings if values are invalid
        if (key.empty()) {
            WRITE_WARNING("Error parsing key from " + tagStr + " generic parameter. Key cannot be empty");
        } else if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
            WRITE_WARNING("Error parsing key from " + tagStr + " generic parameter. Key contains invalid characters");
        } else {
            WRITE_DEBUG("Inserting generic parameter '" + key + "|" + value + "' into " + tagStr);
            // insert parameter
            myCommonXMLStructure.getLastInsertedSumoBaseObject()->addParameter(key, value);
        }
    }
}

/****************************************************************************/
