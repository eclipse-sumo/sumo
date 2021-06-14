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
/// @file    DataHandler.cpp
/// @author  Jakob Erdmann
/// @date    Jun 2021
///
// The XML-Handler for data elements loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/RGBColor.h>
#include <utils/shapes/Shape.h>

#include "DataHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

DataHandler::DataHandler(const std::string& file) :
    SUMOSAXHandler(file) {
}


DataHandler::~DataHandler() {}


bool
DataHandler::parse() {
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void 
DataHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        // Stopping Places
        case SUMO_TAG_INTERVAL:
            buildDataInterval(obj,
                obj->getStringAttribute(SUMO_ATTR_ID),
                obj->getDoubleAttribute(SUMO_ATTR_BEGIN),
                obj->getDoubleAttribute(SUMO_ATTR_END));
            break;
        case SUMO_TAG_EDGE:
            buildEdgeData(obj,
                obj->getStringAttribute(SUMO_ATTR_ID),
                obj->getParameters());
            break;
        case SUMO_TAG_EDGEREL:
            buildEdgeRelationData(obj,
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getParameters());
            break;
        case SUMO_TAG_TAZREL:
            buildTAZRelationData(obj,
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getParameters());
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
DataHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // open SUMOBaseOBject 
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            case SUMO_TAG_INTERVAL:
                parseInterval(attrs);
                break;
            case SUMO_TAG_EDGE:
                parseEdgeData(attrs);
                break;
            case SUMO_TAG_EDGEREL:
                parseEdgeRelationData(attrs);
                break;
            case SUMO_TAG_TAZREL:
                parseTAZRelationData(attrs);
                break;
            // parameters
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
DataHandler::myEndElement(int element) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // close SUMOBaseOBject 
    myCommonXMLStructure.closeSUMOBaseOBject();
    // check tag
    switch (tag) {
        case SUMO_TAG_INTERVAL:
            // parse object and all their childrens
            parseSumoBaseObject(obj);
            // delete object (and all of their childrens)
            delete obj;
            break;
        default:
            break;
    }
}

/*
void
DataHandler::parseBusStopAttributes(const SUMOSAXAttributes& attrs) {
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
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}
*/


void
DataHandler::parseInterval(const SUMOSAXAttributes& attrs) {

}


void
DataHandler::parseEdgeData(const SUMOSAXAttributes& attrs) {

}


void
DataHandler::parseEdgeRelationData(const SUMOSAXAttributes& attrs) {

}


void
DataHandler::parseTAZRelationData(const SUMOSAXAttributes& attrs) {

}


void 
DataHandler::parseParameters(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // get key
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, parsedOk);
    // get SumoBaseObject parent
    CommonXMLStructure::SumoBaseObject* SumoBaseObjectParent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
    // check parent
    if (SumoBaseObjectParent == nullptr) {
        WRITE_ERROR("Parameters must be defined within an object");
    }
    // check tag
    if (SumoBaseObjectParent->getTag() == SUMO_TAG_NOTHING) {
        WRITE_ERROR("Parameters cannot be defined in either the data file's root nor another parameter");
    }
    // continue if key was sucesfully loaded
    if (parsedOk) {
        // get tag str
        const std::string parentTagStr = toString(SumoBaseObjectParent->getTag());
        // circumventing empty string value
        const std::string value = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // show warnings if values are invalid
        if (key.empty()) {
            WRITE_WARNING("Error parsing key from " + parentTagStr + " generic parameter. Key cannot be empty");
        } else if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
            WRITE_WARNING("Error parsing key from " + parentTagStr + " generic parameter. Key contains invalid characters");
        } else {
            WRITE_DEBUG("Inserting generic parameter '" + key + "|" + value + "' into " + parentTagStr);
            // insert parameter in SumoBaseObjectParent
            SumoBaseObjectParent->addParameter(key, value);
        }
    }
}


void
DataHandler::checkParent(const SumoXMLTag currentTag, const SumoXMLTag parentTag, bool& ok) const {
    // check that parent SUMOBaseObject's tag is the parentTag
    if ((myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject() && 
        (myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject()->getTag() == parentTag)) == false) {
        WRITE_ERROR(toString(currentTag) + " must be defined within the definition of a " + toString(parentTag));
        ok = false;
    }
}

/****************************************************************************/
