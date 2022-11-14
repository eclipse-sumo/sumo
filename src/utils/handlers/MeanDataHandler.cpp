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
/// @file    MeanDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// The XML-Handler for meanMeanData elements loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/XMLSubSys.h>

#include "MeanDataHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

MeanDataHandler::MeanDataHandler(const std::string& file) :
    SUMOSAXHandler(file) {
}


MeanDataHandler::~MeanDataHandler() {}


bool
MeanDataHandler::parse() {
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void
MeanDataHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        case SUMO_TAG_MEANDATA_EDGE:
            buildEdgeMeanData(obj,
                obj->getStringAttribute(SUMO_ATTR_ID),
                obj->getStringAttribute(SUMO_ATTR_FILE));
            break;
        case SUMO_TAG_MEANDATA_LANE:
            buildLaneMeanData(obj,
                obj->getStringAttribute(SUMO_ATTR_ID),
                obj->getStringAttribute(SUMO_ATTR_FILE));
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
MeanDataHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = (element == 0) ? SUMO_TAG_ROOTFILE : static_cast<SumoXMLTag>(element);
    // open SUMOBaseOBject
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            case SUMO_TAG_MEANDATA_EDGE:
                parseEdgeMeanData(attrs);
                break;
            case SUMO_TAG_MEANDATA_LANE:
                parseLaneMeanData(attrs);
                break;
            case SUMO_TAG_PARAM:
                WRITE_WARNING(TL("MeanData elements cannot load attributes as params"));
                break;
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        writeError(e.what());
    }
}


void
MeanDataHandler::myEndElement(int element) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // close SUMOBaseOBject
    myCommonXMLStructure.closeSUMOBaseOBject();
    // check tag
    switch (tag) {
        case SUMO_TAG_MEANDATA_EDGE:
        case SUMO_TAG_MEANDATA_LANE:
            parseSumoBaseObject(obj);
            // delete object
            delete obj;
            break;
        default:
            break;
    }
}


bool
MeanDataHandler::isErrorCreatingElement() const {
    return true;
}


void
MeanDataHandler::writeError(const std::string& error) {
    WRITE_ERROR(error);
}


void
MeanDataHandler::parseEdgeMeanData(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string edgeID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_MEANDATA_EDGE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, edgeID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
    }
}


void
MeanDataHandler::parseLaneMeanData(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string laneID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_MEANDATA_LANE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, laneID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
    }
}

/****************************************************************************/
