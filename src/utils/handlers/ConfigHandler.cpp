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
                // currently configuration doesn't have attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CONFIGURATION);
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
    // check tag (only parse SUMOBaseObject after ending configuration)
    if (obj->getTag() == SUMO_TAG_CONFIGURATION) {
        parseSumoBaseObject(obj);
        // delete object (and all of their childrens)
        delete obj;
    }
}


void
ConfigHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        case SUMO_TAG_NETFILE:
            loadNetFile(obj->getStringAttribute(SUMO_ATTR_VALUE));
            break;
        case SUMO_TAG_ADDITIONALFILES:
            loadAdditionalFiles(obj->getStringAttribute(SUMO_ATTR_VALUE));
            break;
        case SUMO_TAG_ROUTEFILES:
            loadRouteFiles(obj->getStringAttribute(SUMO_ATTR_VALUE));
            break;
        default:
            break;
    }
    // now iterate over childrens
    for (const auto& child : obj->getSumoBaseObjectChildren()) {
        // call this function recursively
        parseSumoBaseObject(child);
    }
}


void 
ConfigHandler::parseNetFile(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string value = attrs.get<std::string>(SUMO_ATTR_VALUE, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_NETFILE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_VALUE, value);
    }
}


void 
ConfigHandler::parseAdditionalFiles(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::vector<std::string> value = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VALUE, "", parsedOk, std::vector<std::string>());
    // continue if flag is ok
    if (parsedOk) {
        // avoid empty files
        if (value.empty()) {
            WRITE_ERROR("Additional files cannot be empty");
        } else {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_NETFILE);
            // add all attributes
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VALUE, value);
        }
    }
}


void
ConfigHandler::parseRouteFiles(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::vector<std::string> value = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VALUE, "", parsedOk, std::vector<std::string>());
    // continue if flag is ok
    if (parsedOk) {
        // avoid empty files
        if (value.empty()) {
            WRITE_ERROR("Route files cannot be empty");
        } else {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_NETFILE);
            // add all attributes
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VALUE, value);
        }
    }
}

/****************************************************************************/
