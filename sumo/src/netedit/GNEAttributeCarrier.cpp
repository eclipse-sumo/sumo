/****************************************************************************/
/// @file    GNEAttributeCarrier.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include "GNEAttributeCarrier.h"
#include "GNEUndoList.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
std::map<SumoXMLTag, std::vector<std::pair <SumoXMLAttr, std::string> > > GNEAttributeCarrier::_allowedAttributes;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedNetElementTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalTags;
std::set<SumoXMLAttr> GNEAttributeCarrier::myNumericalIntAttrs;
std::set<SumoXMLAttr> GNEAttributeCarrier::myNumericalFloatAttrs;
std::set<SumoXMLAttr> GNEAttributeCarrier::myListAttrs;
std::set<SumoXMLAttr> GNEAttributeCarrier::myUniqueAttrs;
std::map<SumoXMLTag, SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalWithParentTags;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > GNEAttributeCarrier::myDiscreteChoices;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::string > > GNEAttributeCarrier::myAttrDefinitions;

const std::string GNEAttributeCarrier::LOADED = "loaded";
const std::string GNEAttributeCarrier::GUESSED = "guessed";
const std::string GNEAttributeCarrier::MODIFIED = "modified";
const std::string GNEAttributeCarrier::APPROVED = "approved";

// ===========================================================================
// method definitions
// ===========================================================================
GNEAttributeCarrier::GNEAttributeCarrier(SumoXMLTag tag) :
    myTag(tag) {
}


template<> int
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2int(string);
}


template<> SUMOReal
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2SUMOReal(string);
}


template<> bool
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2Bool(string);
}


bool
GNEAttributeCarrier::isValid(SumoXMLAttr key, const std::string& value) {
    UNUSED_PARAMETER(value);
    return std::find(getAttrs().begin(), getAttrs().end(), key) != getAttrs().end();
}


std::string
GNEAttributeCarrier::getDescription() {
    return toString(myTag);
}


SumoXMLTag
GNEAttributeCarrier::getTag() const {
    return myTag;
}


std::vector<SumoXMLAttr>
GNEAttributeCarrier::getAttrs() const {
    std::vector<SumoXMLAttr> attr;
    for (std::vector<std::pair <SumoXMLAttr, std::string> >::const_iterator i = allowedAttributes(myTag).begin(); i != allowedAttributes(myTag).end(); i++) {
        attr.push_back(i->first);
    }
    return attr;
}


const std::string
GNEAttributeCarrier::getID() const {
    return getAttribute(SUMO_ATTR_ID);
}


SumoXMLTag
GNEAttributeCarrier::getParentType(SumoXMLTag tag) {
    if (hasParent(tag)) {
        return myAllowedAdditionalWithParentTags[tag];
    } else {
        return SUMO_TAG_NOTHING;
    }
}


bool
GNEAttributeCarrier::isValidID(const std::string& value) {
    return value.find_first_of(" \t\n\r@$%^&/|\\{}*'\";:<>") == std::string::npos;
}


bool
GNEAttributeCarrier::isValidFileValue(const std::string& value) {
    // @note Only characteres that aren't permited in a file path or belong
    // to XML sintax
    return value.find_first_of("\t\n\r@$%^&|\\{}*'\";:<>") == std::string::npos;
}


bool
GNEAttributeCarrier::isValidStringVector(const std::string& value) {
    // 1) check if value is empty
    if (value.empty()) {
        return true;
    }
    // 2) Check if there are duplicated spaces
    for (int i = 1; i < (int)value.size(); i++) {
        if (value.at(i - 1) == ' ' && value.at(i) == ' ') {
            return false;
        }
    }
    // 3) Check if the first and last character aren't spaces
    if ((value.at(0) == ' ') || (value.at(value.size() - 1) == ' ')) {
        return false;
    }
    // 4) Check if every sub-string is valid
    int index = 0;
    std::string subString;
    while (index < (int)value.size()) {
        if (value.at(index) == ' ') {
            if (!isValidFileValue(subString)) {
                return false;
            } else {
                subString.clear();
            }
        } else {
            subString.push_back(value.at(index));
        }
        index++;
    }
    // 5) All right, then return true
    return true;
}

// ===========================================================================
// static methods
// ===========================================================================

const std::vector<std::pair <SumoXMLAttr, std::string> >&
GNEAttributeCarrier::allowedAttributes(SumoXMLTag tag) {
    // define on first access
    if (!_allowedAttributes.count(tag)) {
        std::vector<std::pair<SumoXMLAttr, std::string> >& attrs = _allowedAttributes[tag];
        switch (tag) {
            case SUMO_TAG_EDGE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NUMLANES, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, ""));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPREADTYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NAME, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, ""));
                break;
            case SUMO_TAG_JUNCTION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_RADIUS, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_KEEP_CLEAR, ""));
                break;
            case SUMO_TAG_LANE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, ""));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_INDEX, "")); // read-only attribute
                break;
            case SUMO_TAG_POI:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                break;
            case SUMO_TAG_CROSSING:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, ""));
                break;
            case SUMO_TAG_CONNECTION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PASS, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_KEEP_CLEAR, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONTPOS, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_UNCONTROLLED, "false"));
                break;
            case SUMO_TAG_BUS_STOP:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, "10"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LINES, ""));
                break;
            case SUMO_TAG_CONTAINER_STOP:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, "10"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LINES, ""));
                break;
            case SUMO_TAG_CHARGING_STATION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, "10"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGINGPOWER, "22000"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EFFICIENCY, "0.95"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEINTRANSIT, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEDELAY, "0"));
                break;
            case SUMO_TAG_E1DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPLIT_VTYPE, "false"));
                break;
            case SUMO_TAG_E2DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, "10"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONT, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_TIME_THRESHOLD, "1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "1.39"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_JAM_DIST_THRESHOLD, "10"));
                break;
            case SUMO_TAG_E3DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                break;
            case SUMO_TAG_DET_ENTRY:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                break;
            case SUMO_TAG_DET_EXIT:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                break;
            case SUMO_TAG_VSS:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANES, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                break;
            case SUMO_TAG_CALIBRATOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));
                // Currently unused attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ROUTEPROBE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OUTPUT, ""));
                break;
            case SUMO_TAG_REROUTER:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PROB, "1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OFF, "false"));
                break;
            case SUMO_TAG_ROUTEPROBE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_BEGIN, "0"));
                break;
            case SUMO_TAG_VAPORIZER:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTTIME, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_END, "10"));
                break;
            default:
                WRITE_WARNING("allowed attributes for tag '" + toString(tag) + "' not defined");
        }
    }
    return _allowedAttributes[tag];
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedTags() {
    // define on first access
    if (myAllowedTags.empty()) {
        myAllowedTags.push_back(SUMO_TAG_JUNCTION);
        myAllowedTags.push_back(SUMO_TAG_EDGE);
        myAllowedTags.push_back(SUMO_TAG_LANE);
        myAllowedTags.push_back(SUMO_TAG_CONNECTION);
        myAllowedTags.push_back(SUMO_TAG_BUS_STOP);
        myAllowedTags.push_back(SUMO_TAG_CONTAINER_STOP);
        myAllowedTags.push_back(SUMO_TAG_CHARGING_STATION);
        myAllowedTags.push_back(SUMO_TAG_E1DETECTOR);
        myAllowedTags.push_back(SUMO_TAG_E2DETECTOR);
        myAllowedTags.push_back(SUMO_TAG_E3DETECTOR);
        myAllowedTags.push_back(SUMO_TAG_DET_ENTRY);
        myAllowedTags.push_back(SUMO_TAG_DET_EXIT);
        myAllowedTags.push_back(SUMO_TAG_VSS);
        myAllowedTags.push_back(SUMO_TAG_CALIBRATOR);
        myAllowedTags.push_back(SUMO_TAG_REROUTER);
        myAllowedTags.push_back(SUMO_TAG_ROUTEPROBE);
        myAllowedTags.push_back(SUMO_TAG_VAPORIZER);
    }
    return myAllowedTags;
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedNetElementTags() {
    // define on first access
    if (myAllowedNetElementTags.empty()) {
        myAllowedNetElementTags.push_back(SUMO_TAG_JUNCTION);
        myAllowedNetElementTags.push_back(SUMO_TAG_EDGE);
        myAllowedNetElementTags.push_back(SUMO_TAG_LANE);
        myAllowedNetElementTags.push_back(SUMO_TAG_CONNECTION);
    }
    return myAllowedNetElementTags;
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedAdditionalTags() {
    // define on first access
    if (myAllowedAdditionalTags.empty()) {
        myAllowedAdditionalTags.push_back(SUMO_TAG_BUS_STOP);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CONTAINER_STOP);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CHARGING_STATION);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E1DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E2DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E3DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_ENTRY);
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_EXIT);
        myAllowedAdditionalTags.push_back(SUMO_TAG_VSS);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CALIBRATOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_REROUTER);
        myAllowedAdditionalTags.push_back(SUMO_TAG_ROUTEPROBE);
        myAllowedAdditionalTags.push_back(SUMO_TAG_VAPORIZER);
    }
    return myAllowedAdditionalTags;
}


bool
GNEAttributeCarrier::isNumerical(SumoXMLAttr attr) {
    return (isInt(attr) || isFloat(attr));
}


bool
GNEAttributeCarrier::isInt(SumoXMLAttr attr) {
    // define on first access
    if (myNumericalIntAttrs.empty()) {
        myNumericalIntAttrs.insert(SUMO_ATTR_NUMLANES);
        myNumericalIntAttrs.insert(SUMO_ATTR_PRIORITY);
        myNumericalIntAttrs.insert(SUMO_ATTR_INDEX);
        myNumericalIntAttrs.insert(SUMO_ATTR_CHARGEDELAY);
        myNumericalIntAttrs.insert(SUMO_ATTR_FREQUENCY);
        myNumericalIntAttrs.insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);
        myNumericalIntAttrs.insert(SUMO_ATTR_BEGIN);
        myNumericalIntAttrs.insert(SUMO_ATTR_FROM_LANE);
        myNumericalIntAttrs.insert(SUMO_ATTR_TO_LANE);
    }
    return myNumericalIntAttrs.count(attr) == 1;
}


bool
GNEAttributeCarrier::isFloat(SumoXMLAttr attr) {
    // define on first access
    if (myNumericalFloatAttrs.empty()) {
        myNumericalFloatAttrs.insert(SUMO_ATTR_SPEED);
        myNumericalFloatAttrs.insert(SUMO_ATTR_LENGTH);
        myNumericalFloatAttrs.insert(SUMO_ATTR_WIDTH);
        myNumericalFloatAttrs.insert(SUMO_ATTR_ENDOFFSET);
        myNumericalFloatAttrs.insert(SUMO_ATTR_RADIUS);
        myNumericalFloatAttrs.insert(SUMO_ATTR_STARTPOS);
        myNumericalFloatAttrs.insert(SUMO_ATTR_ENDPOS);
        myNumericalFloatAttrs.insert(SUMO_ATTR_CHARGINGPOWER);
        myNumericalFloatAttrs.insert(SUMO_ATTR_EFFICIENCY);
        myNumericalFloatAttrs.insert(SUMO_ATTR_LENGTH);
        myNumericalFloatAttrs.insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);
        myNumericalFloatAttrs.insert(SUMO_ATTR_JAM_DIST_THRESHOLD);
        myNumericalFloatAttrs.insert(SUMO_ATTR_PROB);
        myNumericalFloatAttrs.insert(SUMO_ATTR_CONTPOS);
    }
    return myNumericalFloatAttrs.count(attr) == 1;
}


bool
GNEAttributeCarrier::isBool(SumoXMLAttr attr) {
    // Iterate over additional tags
    for (std::vector<SumoXMLTag>::const_iterator i = allowedTags().begin(); i != allowedTags().end(); i++) {
        // Obtain choices
        std::vector<std::string> choices = discreteChoices(*i, attr);
        // CHeck if choices are exactly "true" and "false"
        if ((choices.size() == 2) && (choices.at(0) == "true") && (choices.at(1) == "false")) {
            return true;
        }
    }
    return false;
}


bool
GNEAttributeCarrier::isString(SumoXMLAttr attr) {
    return (!isNumerical(attr) && !isBool(attr) && !isFloat(attr));
}


bool
GNEAttributeCarrier::isList(SumoXMLAttr attr) {
    // define on first access
    if (myListAttrs.empty()) {
        myListAttrs.insert(SUMO_ATTR_LINES);
        myListAttrs.insert(SUMO_ATTR_EDGES);
        myListAttrs.insert(SUMO_ATTR_LANES);
    }
    return myListAttrs.count(attr) == 1;
}


bool
GNEAttributeCarrier::isUnique(SumoXMLAttr attr) {
    // define on first access
    if (myUniqueAttrs.empty()) {
        myUniqueAttrs.insert(SUMO_ATTR_ID);
        myUniqueAttrs.insert(SUMO_ATTR_FROM);
        myUniqueAttrs.insert(SUMO_ATTR_TO);
        myUniqueAttrs.insert(SUMO_ATTR_SHAPE);
        myUniqueAttrs.insert(SUMO_ATTR_POSITION);
        myUniqueAttrs.insert(SUMO_ATTR_EDGES);
        myUniqueAttrs.insert(SUMO_ATTR_STARTPOS);
        myUniqueAttrs.insert(SUMO_ATTR_ENDPOS);
        myUniqueAttrs.insert(SUMO_ATTR_LANE);
        myUniqueAttrs.insert(SUMO_ATTR_EDGE);
        myUniqueAttrs.insert(SUMO_ATTR_ROUTEPROBE);
        myUniqueAttrs.insert(SUMO_ATTR_FROM_LANE);
        myUniqueAttrs.insert(SUMO_ATTR_TO_LANE);
    }
    return myUniqueAttrs.count(attr) == 1;
}


bool
GNEAttributeCarrier::isDiscrete(SumoXMLTag tag, SumoXMLAttr attr) {
    if (discreteChoices(tag, attr).size() > 0) {
        return true;
    } else {
        return false;
    }
}


bool
GNEAttributeCarrier::hasParent(SumoXMLTag tag) {
    // define on first access
    if (myAllowedAdditionalWithParentTags.empty()) {
        myAllowedAdditionalWithParentTags[SUMO_TAG_DET_ENTRY] = SUMO_TAG_E3DETECTOR;
        myAllowedAdditionalWithParentTags[SUMO_TAG_DET_EXIT] = SUMO_TAG_E3DETECTOR;
    }
    return myAllowedAdditionalWithParentTags.count(tag) == 1;
}


bool
GNEAttributeCarrier::hasAttribute(SumoXMLTag tag, SumoXMLAttr attr) {
    const std::vector<std::pair <SumoXMLAttr, std::string> >& attrs = allowedAttributes(tag);
    for (std::vector<std::pair <SumoXMLAttr, std::string> >::const_iterator i = attrs.begin(); i != attrs.end(); i++) {
        if (i->first == attr) {
            return true;
        }
    }
    return false;
}


const std::vector<std::string>&
GNEAttributeCarrier::discreteChoices(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myDiscreteChoices.empty()) {
        std::vector<std::string> choices;
        choices = SUMOXMLDefinitions::NodeTypes.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            if (*it != toString(NODETYPE_DEAD_END_DEPRECATED)) {
                myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE].push_back(*it);
            }
        }

        myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR].push_back("true");
        myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR].push_back("false");

        choices = SUMOXMLDefinitions::LaneSpreadFunctions.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE].push_back(*it);
        }

        choices = SumoVehicleClassStrings.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_ALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_DISALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_ALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_DISALLOW].push_back(*it);
        }

        myDiscreteChoices[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY].push_back("true");
        myDiscreteChoices[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY].push_back("false");

        myDiscreteChoices[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEINTRANSIT].push_back("true");
        myDiscreteChoices[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEINTRANSIT].push_back("false");

        myDiscreteChoices[SUMO_TAG_E1DETECTOR][SUMO_ATTR_SPLIT_VTYPE].push_back("true");
        myDiscreteChoices[SUMO_TAG_E1DETECTOR][SUMO_ATTR_SPLIT_VTYPE].push_back("false");

        myDiscreteChoices[SUMO_TAG_E2DETECTOR][SUMO_ATTR_CONT].push_back("true");
        myDiscreteChoices[SUMO_TAG_E2DETECTOR][SUMO_ATTR_CONT].push_back("false");

        myDiscreteChoices[SUMO_TAG_REROUTER][SUMO_ATTR_OFF].push_back("true");
        myDiscreteChoices[SUMO_TAG_REROUTER][SUMO_ATTR_OFF].push_back("false");

        myDiscreteChoices[SUMO_TAG_CONNECTION][SUMO_ATTR_KEEP_CLEAR].push_back("true");
        myDiscreteChoices[SUMO_TAG_CONNECTION][SUMO_ATTR_KEEP_CLEAR].push_back("false");

        myDiscreteChoices[SUMO_TAG_CONNECTION][SUMO_ATTR_UNCONTROLLED].push_back("true");
        myDiscreteChoices[SUMO_TAG_CONNECTION][SUMO_ATTR_UNCONTROLLED].push_back("false");
    }

    return myDiscreteChoices[tag][attr];
}


bool
GNEAttributeCarrier::discreteCombinableChoices(SumoXMLTag, SumoXMLAttr attr) {
    return (attr == SUMO_ATTR_ALLOW || attr == SUMO_ATTR_DISALLOW);
}


std::string
GNEAttributeCarrier::getDefinition(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myAttrDefinitions.empty()) {
        // Edge
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_FROM] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_TO] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_SPEED] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_PRIORITY] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_NUMLANES] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_TYPE] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_ALLOW] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_DISALLOW] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_PREFER] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_SHAPE] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_LENGTH] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_NAME] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_WIDTH] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_ENDOFFSET] = "";
        // Junction
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_POSITION] = "";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE] = "";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_SHAPE] = "";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_RADIUS] = "";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR] = "";
        // Lane
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_SPEED] = "";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_ALLOW] = "";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_DISALLOW] = "";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_PREFER] = "";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_WIDTH] = "";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_ENDOFFSET] = "";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_INDEX] = "";
        // POI
        myAttrDefinitions[SUMO_TAG_POI][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_POI][SUMO_ATTR_POSITION] = "";
        myAttrDefinitions[SUMO_TAG_POI][SUMO_ATTR_TYPE] = "";
        // Crossing
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY] = "";
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_WIDTH] = "";
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_EDGES] = "";
        // Connection
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_FROM] = "The name of the edge the vehicles leave ";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_TO] = "The name of the edge the vehicles may reach when leaving 'from'";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_FROM_LANE] = "the lane index of the incoming lane (numbers starting with 0)";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_TO_LANE] = "the lane index of the outgoing lane (numbers starting with 0)";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_PASS] = "if set, vehicles which pass this (lane-2-lane) connection) will not wait";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_KEEP_CLEAR] = "if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection.";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_CONTPOS] = "If set to a more than 0 value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection.";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_UNCONTROLLED] = "if set to true, This connection will not be TLS-controlled despite its node being controlled.";
        // Bus Stop
        myAttrDefinitions[SUMO_TAG_BUS_STOP][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_BUS_STOP][SUMO_ATTR_LANE] = "The name of the lane the bus stop shall be located at";
        myAttrDefinitions[SUMO_TAG_BUS_STOP][SUMO_ATTR_STARTPOS] = "The begin position on the lane (the lower position on the lane) in meters";
        myAttrDefinitions[SUMO_TAG_BUS_STOP][SUMO_ATTR_ENDPOS] = "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m";
        myAttrDefinitions[SUMO_TAG_BUS_STOP][SUMO_ATTR_LINES] = "meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes";
        // container Stop
        myAttrDefinitions[SUMO_TAG_CONTAINER_STOP][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_CONTAINER_STOP][SUMO_ATTR_LANE] = "The name of the lane the container stop shall be located at";
        myAttrDefinitions[SUMO_TAG_CONTAINER_STOP][SUMO_ATTR_STARTPOS] = "The begin position on the lane (the lower position on the lane) in meters";
        myAttrDefinitions[SUMO_TAG_CONTAINER_STOP][SUMO_ATTR_ENDPOS] = "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m";
        myAttrDefinitions[SUMO_TAG_CONTAINER_STOP][SUMO_ATTR_LINES] = "meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes";
        // Charging Station
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_LANE] = "Lane of the charging station location";
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_STARTPOS] = "Begin position in the specified lane";
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_ENDPOS] = "End position in the specified lane";
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGINGPOWER] = "Charging power in W";
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_EFFICIENCY] = "Charging efficiency [0,1]";
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEINTRANSIT] = "Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging";
        myAttrDefinitions[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEDELAY] = "Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins";
        // E1
        myAttrDefinitions[SUMO_TAG_E1DETECTOR][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_E1DETECTOR][SUMO_ATTR_LANE] = "The id of the lane the detector shall be laid on. The lane must be a part of the network used";
        myAttrDefinitions[SUMO_TAG_E1DETECTOR][SUMO_ATTR_POSITION] = "The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length";
        myAttrDefinitions[SUMO_TAG_E1DETECTOR][SUMO_ATTR_FREQUENCY] = "The aggregation period the values the detector collects shall be summed up";
        myAttrDefinitions[SUMO_TAG_E1DETECTOR][SUMO_ATTR_FILE] = "The path to the output file";
        myAttrDefinitions[SUMO_TAG_E1DETECTOR][SUMO_ATTR_SPLIT_VTYPE] = "If set, the collected values will be additionally reported on per-vehicle type base, see below";
        // E2
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_LANE] = "The id of the lane the detector shall be laid on. The lane must be a part of the network used";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_POSITION] = "The position on the lane the detector shall be laid on in meters";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_LENGTH] = "The length of the detector in meters";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_FREQUENCY] = "The aggregation period the values the detector collects shall be summed up";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_FILE] = "The path to the output file";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_CONT] = "Holds the information whether detectors longer than a lane shall be cut off or continued (set it to true for the second case); default: false";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_HALTING_TIME_THRESHOLD] = "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting; in s, default: 1s";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_HALTING_SPEED_THRESHOLD] = "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting; in m/s, default: 5/3.6m/s";
        myAttrDefinitions[SUMO_TAG_E2DETECTOR][SUMO_ATTR_JAM_DIST_THRESHOLD] = "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam; in m, default: 10m";
        // E3
        myAttrDefinitions[SUMO_TAG_E3DETECTOR][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_E3DETECTOR][SUMO_ATTR_FREQUENCY] = "The aggregation period the values the detector collects shall be summed up";
        myAttrDefinitions[SUMO_TAG_E3DETECTOR][SUMO_ATTR_FILE] = "The path to the output file";
        // Entry
        myAttrDefinitions[SUMO_TAG_DET_ENTRY][SUMO_ATTR_LANE] = "The id of the lane the detector shall be laid on. The lane must be a part of the network used";
        myAttrDefinitions[SUMO_TAG_DET_ENTRY][SUMO_ATTR_POSITION] = "The position on the lane the detector shall be laid on in meters";
        // Exit
        myAttrDefinitions[SUMO_TAG_DET_EXIT][SUMO_ATTR_LANE] = "The id of the lane the detector shall be laid on. The lane must be a part of the network used";
        myAttrDefinitions[SUMO_TAG_DET_EXIT][SUMO_ATTR_POSITION] = "The position on the lane the detector shall be laid on in meters";
        // Variable Speed Signal
        myAttrDefinitions[SUMO_TAG_VSS][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_VSS][SUMO_ATTR_LANES] = "list of lanes of Variable Speed Signal";
        myAttrDefinitions[SUMO_TAG_VSS][SUMO_ATTR_FILE] = "The path to the output file";
        // Calibrator
        myAttrDefinitions[SUMO_TAG_CALIBRATOR][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_CALIBRATOR][SUMO_ATTR_LANE] = "List of lanes of calibrator";
        myAttrDefinitions[SUMO_TAG_CALIBRATOR][SUMO_ATTR_POSITION] = "The position of the calibrator on the specified lane";
        myAttrDefinitions[SUMO_TAG_CALIBRATOR][SUMO_ATTR_FREQUENCY] = "The aggregation interval in which to calibrate the flows. default is step-length";
        myAttrDefinitions[SUMO_TAG_CALIBRATOR][SUMO_ATTR_ROUTEPROBE] = "The id of the routeProbe element from which to determine the route distribution for generated vehicles";
        myAttrDefinitions[SUMO_TAG_CALIBRATOR][SUMO_ATTR_OUTPUT] = "The output file for writing calibrator information or NULL";
        // Rerouter
        myAttrDefinitions[SUMO_TAG_REROUTER][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_REROUTER][SUMO_ATTR_EDGES] = "An edge id or a list of edge ids where vehicles shall be rerouted";
        myAttrDefinitions[SUMO_TAG_REROUTER][SUMO_ATTR_FILE] = "The path to the definition file (alternatively, the intervals may defined as children of the rerouter)";
        myAttrDefinitions[SUMO_TAG_REROUTER][SUMO_ATTR_PROB] = "The probability for vehicle rerouting (0-1), default 1";
        myAttrDefinitions[SUMO_TAG_REROUTER][SUMO_ATTR_OFF] = "Whether the router should be inactive initially (and switched on in the gui), default:false";
        // SUMO_TAG_ROUTEPROBE
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_EDGE] = "The id of an edge in the simulation network";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_FREQUENCY] = "The frequency in which to report the distribution";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_FILE] = "The file for generated output";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_BEGIN] = "The time at which to start generating output";
    }
    return myAttrDefinitions[tag][attr];
}


int
GNEAttributeCarrier::getHigherNumberOfAttributes() {
    int higherNumber = 0;
    for (std::vector<SumoXMLTag>::const_iterator i = allowedTags().begin(); i != allowedTags().end(); i++) {
        if ((int)allowedAttributes(*i).size() > higherNumber) {
            higherNumber = (int)allowedAttributes(*i).size();
        }
    }
    return higherNumber;
}


template<> int
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if ((*i).first == attr) {
            return TplConvert::_str2int((*i).second);
        }
    }
    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return 0;
}


template<> SUMOReal
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if ((*i).first == attr) {
            return TplConvert::_str2SUMOReal((*i).second);
        }
    }
    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return 0;
}


template<> bool
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if ((*i).first == attr) {
            return TplConvert::_str2Bool((*i).second);
        }
    }
    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return false;
}


template<> std::string
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if ((*i).first == attr) {
            return (*i).second;
        }
    }
    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return "";
}


template<> std::vector<int>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    std::cout << "FINISH" << std::endl;

    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return std::vector<int>();
}


template<> std::vector<SUMOReal>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    std::cout << "FINISH" << std::endl;

    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return std::vector<SUMOReal>();
}


template<> std::vector<bool>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    std::cout << "FINISH" << std::endl;

    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return std::vector<bool>();
}


template<> std::vector<std::string>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if ((*i).first == attr) {
            std::vector<std::string> myVectorString;
            SUMOSAXAttributes::parseStringVector((*i).second, myVectorString);
            return myVectorString;
        }
    }
    // Write warning if attribute don't have a default value and return a empty value to avoid warnings
    WRITE_WARNING("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' don't have a default value");
    return std::vector<std::string>();
}

/****************************************************************************/

