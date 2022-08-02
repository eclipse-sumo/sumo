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
#include <utils/xml/XMLSubSys.h>

#include "ConfigHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

ConfigHandler::ConfigHandler(const std::string& file) :
    SUMOSAXHandler(file) {
}


ConfigHandler::~ConfigHandler() {}


bool
ConfigHandler::parse() {
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void 
ConfigHandler::parseConfigFile() {
    // open SUMOBaseOBject and set tag
    myCommonXMLStructure.openSUMOBaseOBject();
    myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CONFIGURATION);
    myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_CONFIGFILE, getFileName());
}


void
ConfigHandler::parseNetFile(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // network file
    const std::string value = attrs.get<std::string>(SUMO_ATTR_VALUE, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        if (value.empty()) {
            WRITE_ERROR("Network file cannot be empty");
        } else if (myCommonXMLStructure.getCurrentSumoBaseObject() == nullptr) {
            WRITE_ERROR("Network file must be loaded within a configuration");
        } else {
            // add it in SUMOConfig parent
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NETFILE, value);
        }
    }
}


void 
ConfigHandler::parseAdditionalFiles(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // additional file
    const std::string value = attrs.get<std::string>(SUMO_ATTR_VALUE, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // avoid empty files
        if (value.empty()) {
            WRITE_ERROR("Additional files cannot be empty");
        } else if (myCommonXMLStructure.getCurrentSumoBaseObject() == nullptr) {
            WRITE_ERROR("Additional files must be loaded within a configuration");
        } else {
            // add it in SUMOConfig parent
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ADDITIONALFILES, value);
        }
    }
}


void
ConfigHandler::parseRouteFiles(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // route file
    const std::string value = attrs.get<std::string>(SUMO_ATTR_VALUE, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // avoid empty files
        if (value.empty()) {
            WRITE_ERROR("Route files cannot be empty");
        } else if (myCommonXMLStructure.getCurrentSumoBaseObject() == nullptr) {
            WRITE_ERROR("Route files must be loaded within a configuration");
        } else {
            // add it in SUMOConfig parent
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ROUTEFILES, value);
        }
    }
}


void
ConfigHandler::parseDataFiles(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // data file
    const std::string value = attrs.get<std::string>(SUMO_ATTR_VALUE, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // avoid empty files
        if (value.empty()) {
            WRITE_ERROR("Data files cannot be empty");
        } else if (myCommonXMLStructure.getCurrentSumoBaseObject() == nullptr) {
            WRITE_ERROR("Data files must be loaded within a configuration");
        } else {
            // add it in SUMOConfig parent
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DATAFILES, value);
        }
    }
}


void
ConfigHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // check tag
    try {
        switch (tag) {
            // Stopping Places
            case SUMO_TAG_CONFIGURATION:
                parseConfigFile();
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
            case SUMO_TAG_DATAFILES:
                parseDataFiles(attrs);
                break;
            default:
                // tag cannot be parsed in ConfigHandler
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void
ConfigHandler::myEndElement(int element) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // check tag (only load after ending configuration)
    if (tag == SUMO_TAG_CONFIGURATION) {
        // close SUMOBaseOBject
        myCommonXMLStructure.closeSUMOBaseOBject();
        // load config
        loadConfig(obj);
        // delete object (and all of their childrens)
        delete obj;
    }
}

/****************************************************************************/
