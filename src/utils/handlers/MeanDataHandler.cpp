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

MeanDataHandler::MeanDataHandler() {
}


MeanDataHandler::~MeanDataHandler() {}


bool
MeanDataHandler::beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
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
                // tag cannot be parsed in MeanDataHandler
                return false;
                break;
        }
    } catch (InvalidArgument& e) {
        writeError(e.what());
    }
    return true;
}


void
MeanDataHandler::endParseAttributes() {
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // close SUMOBaseOBject
    myCommonXMLStructure.closeSUMOBaseOBject();
    // check tag
    switch (obj->getTag()) {
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


void
MeanDataHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        case SUMO_TAG_MEANDATA_EDGE:
            buildEdgeMeanData(obj,
                              obj->getStringAttribute(SUMO_ATTR_ID),
                              obj->getStringAttribute(SUMO_ATTR_FILE),
                              obj->getTimeAttribute(SUMO_ATTR_PERIOD),
                              obj->getTimeAttribute(SUMO_ATTR_BEGIN),
                              obj->getTimeAttribute(SUMO_ATTR_END),
                              obj->getBoolAttribute(SUMO_ATTR_TRACK_VEHICLES),
                              obj->getStringListAttribute(SUMO_ATTR_WRITE_ATTRIBUTES),
                              obj->getBoolAttribute(SUMO_ATTR_AGGREGATE),
                              obj->getStringListAttribute(SUMO_ATTR_EDGES),
                              obj->getStringAttribute(SUMO_ATTR_EDGESFILE),
                              obj->getStringAttribute(SUMO_ATTR_EXCLUDE_EMPTY),
                              obj->getBoolAttribute(SUMO_ATTR_WITH_INTERNAL),
                              obj->getStringListAttribute(SUMO_ATTR_DETECT_PERSONS),
                              obj->getDoubleAttribute(SUMO_ATTR_MIN_SAMPLES),
                              obj->getDoubleAttribute(SUMO_ATTR_MAX_TRAVELTIME),
                              obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                              obj->getDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD));


            break;
        case SUMO_TAG_MEANDATA_LANE:
            buildLaneMeanData(obj,
                              obj->getStringAttribute(SUMO_ATTR_ID),
                              obj->getStringAttribute(SUMO_ATTR_FILE),
                              obj->getTimeAttribute(SUMO_ATTR_PERIOD),
                              obj->getTimeAttribute(SUMO_ATTR_BEGIN),
                              obj->getTimeAttribute(SUMO_ATTR_END),
                              obj->getBoolAttribute(SUMO_ATTR_TRACK_VEHICLES),
                              obj->getStringListAttribute(SUMO_ATTR_WRITE_ATTRIBUTES),
                              obj->getBoolAttribute(SUMO_ATTR_AGGREGATE),
                              obj->getStringListAttribute(SUMO_ATTR_EDGES),
                              obj->getStringAttribute(SUMO_ATTR_EDGESFILE),
                              obj->getStringAttribute(SUMO_ATTR_EXCLUDE_EMPTY),
                              obj->getBoolAttribute(SUMO_ATTR_WITH_INTERNAL),
                              obj->getStringListAttribute(SUMO_ATTR_DETECT_PERSONS),
                              obj->getDoubleAttribute(SUMO_ATTR_MIN_SAMPLES),
                              obj->getDoubleAttribute(SUMO_ATTR_MAX_TRAVELTIME),
                              obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                              obj->getDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD));
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
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk);
    // optional attributes
    const SUMOTime period = attrs.getOptSUMOTimeReporting(SUMO_ATTR_PERIOD, id.c_str(), parsedOk, -1);
    const SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), parsedOk, -1);
    const SUMOTime end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), parsedOk, -1);
    const bool trackVehicles = attrs.getOpt<bool>(SUMO_ATTR_TRACK_VEHICLES, id.c_str(), parsedOk, false);
    const std::vector<std::string> writeAttributes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_WRITE_ATTRIBUTES, id.c_str(), parsedOk, {});
    const bool aggregate = attrs.getOpt<bool>(SUMO_ATTR_AGGREGATE, id.c_str(), parsedOk, false);
    const std::vector<std::string> edges = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_EDGES, id.c_str(), parsedOk, {});
    const std::string edgeFile = attrs.getOpt<std::string>(SUMO_ATTR_EDGESFILE, id.c_str(), parsedOk, "");
    const std::string excludeEmpty = attrs.getOpt<std::string>(SUMO_ATTR_EXCLUDE_EMPTY, id.c_str(), parsedOk, "default");
    const bool withInternal = attrs.getOpt<bool>(SUMO_ATTR_WITH_INTERNAL, id.c_str(), parsedOk, false);
    const std::vector<std::string> detectPersons = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_DETECT_PERSONS, id.c_str(), parsedOk, {});
    const double minSamples = attrs.getOpt<double>(SUMO_ATTR_MIN_SAMPLES, id.c_str(), parsedOk, 0);
    const double maxTravel = attrs.getOpt<double>(SUMO_ATTR_MAX_TRAVELTIME, id.c_str(), parsedOk, 100000);
    const std::vector<std::string> vTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, {});
    const double speedThreshold = attrs.getOpt<double>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), parsedOk, 0.1);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_MEANDATA_EDGE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_PERIOD, period);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_BEGIN, begin);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_END, end);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_TRACK_VEHICLES, trackVehicles);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_WRITE_ATTRIBUTES, writeAttributes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_AGGREGATE, aggregate);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_EDGESFILE, edgeFile);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_EXCLUDE_EMPTY, excludeEmpty);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_WITH_INTERNAL, withInternal);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_DETECT_PERSONS, detectPersons);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_MIN_SAMPLES, minSamples);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_MAX_TRAVELTIME, maxTravel);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD, speedThreshold);
    }
}


void
MeanDataHandler::parseLaneMeanData(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk);
    // optional attributes
    const SUMOTime period = attrs.getOptSUMOTimeReporting(SUMO_ATTR_PERIOD, id.c_str(), parsedOk, -1);
    const SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), parsedOk, -1);
    const SUMOTime end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), parsedOk, -1);
    const bool trackVehicles = attrs.getOpt<bool>(SUMO_ATTR_TRACK_VEHICLES, id.c_str(), parsedOk, false);
    const std::vector<std::string> writeAttributes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_WRITE_ATTRIBUTES, id.c_str(), parsedOk, {});
    const bool aggregate = attrs.getOpt<bool>(SUMO_ATTR_AGGREGATE, id.c_str(), parsedOk, false);
    const std::vector<std::string> edges = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_EDGES, id.c_str(), parsedOk, {});
    const std::string edgeFile = attrs.getOpt<std::string>(SUMO_ATTR_EDGESFILE, id.c_str(), parsedOk, "");
    const std::string excludeEmpty = attrs.getOpt<std::string>(SUMO_ATTR_EXCLUDE_EMPTY, id.c_str(), parsedOk, "default");
    const bool withInternal = attrs.getOpt<bool>(SUMO_ATTR_WITH_INTERNAL, id.c_str(), parsedOk, false);
    const std::vector<std::string> detectPersons = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_DETECT_PERSONS, id.c_str(), parsedOk, {});
    const double minSamples = attrs.getOpt<double>(SUMO_ATTR_MIN_SAMPLES, id.c_str(), parsedOk, 0);
    const double maxTravel = attrs.getOpt<double>(SUMO_ATTR_MAX_TRAVELTIME, id.c_str(), parsedOk, 100000);
    const std::vector<std::string> vTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, {});
    const double speedThreshold = attrs.getOpt<double>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), parsedOk, 0.1);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_MEANDATA_LANE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_PERIOD, period);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_BEGIN, begin);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_END, end);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_TRACK_VEHICLES, trackVehicles);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_WRITE_ATTRIBUTES, writeAttributes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_AGGREGATE, aggregate);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_EDGESFILE, edgeFile);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_EXCLUDE_EMPTY, excludeEmpty);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_WITH_INTERNAL, withInternal);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_DETECT_PERSONS, detectPersons);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_MIN_SAMPLES, minSamples);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_MAX_TRAVELTIME, maxTravel);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD, speedThreshold);
    }
}

/****************************************************************************/
