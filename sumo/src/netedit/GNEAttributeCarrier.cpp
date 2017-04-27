/****************************************************************************/
/// @file    GNEAttributeCarrier.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNEAttributeCarrier.h"
#include "GNEUndoList.h"

// ===========================================================================
// static members
// ===========================================================================
std::map<SumoXMLTag, std::vector<std::pair <SumoXMLAttr, std::string> > > GNEAttributeCarrier::_allowedAttributes;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedNetElementTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalTags;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myNumericalIntAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myNumericalFloatAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myTimeAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myBoolAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myListAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myUniqueAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myNonEditableAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myPositiveAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myProbabilityAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myFileAttrs;
std::map<SumoXMLTag, SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalWithParentTags;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > GNEAttributeCarrier::myDiscreteChoices;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::string > > GNEAttributeCarrier::myAttrDefinitions;
int GNEAttributeCarrier::myMaxNumAttribute = 0;

const std::string GNEAttributeCarrier::LOADED = "loaded";
const std::string GNEAttributeCarrier::GUESSED = "guessed";
const std::string GNEAttributeCarrier::MODIFIED = "modified";
const std::string GNEAttributeCarrier::APPROVED = "approved";

#define NODEFAULTVALUE "<NODEFAULTVALUE>"

// ===========================================================================
// method definitions
// ===========================================================================
GNEAttributeCarrier::GNEAttributeCarrier(SumoXMLTag tag, GUIIcon icon) :
    myTag(tag),
    myIcon(icon) {
}


template<> int
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2int(string);
}


template<> double
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2double(string);
}


template<> bool
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2Bool(string);
}


template<> std::string
GNEAttributeCarrier::parse(const std::string& string) {
    return string;
}


template<> SUMOVehicleClass
GNEAttributeCarrier::parse(const std::string& string) {
    if (string.size() == 0) {
        throw EmptyData();
    } else if (SumoVehicleClassStrings.hasString(string) == false) {
        return SVC_IGNORING;
    } else {
        return SumoVehicleClassStrings.get(string);
    }
}


template<> SUMOVehicleShape
GNEAttributeCarrier::parse(const std::string& string) {
    if (string.size() == 0) {
        throw EmptyData();
    } else if ((string == "unknown") || (SumoVehicleShapeStrings.hasString(string) == false)) {
        return SVS_UNKNOWN;
    } else {
        return SumoVehicleShapeStrings.get(string);
    }
}


template<> std::vector<std::string>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues;
    SUMOSAXAttributes::parseStringVector(string, parsedValues);
    return parsedValues;
}


template<> std::vector<int>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<int> parsedIntValues;
    for (std::vector<std::string>::const_iterator i = parsedValues.begin(); i != parsedValues.end(); i++) {
        parsedIntValues.push_back(parse<int>(*i));
    }
    return parsedIntValues;
}


template<> std::vector<double>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<double> parsedDoubleValues;
    for (std::vector<std::string>::const_iterator i = parsedValues.begin(); i != parsedValues.end(); i++) {
        parsedDoubleValues.push_back(parse<double>(*i));
    }
    return parsedDoubleValues;
}


template<> std::vector<bool>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<bool> parsedBoolValues;
    for (std::vector<std::string>::const_iterator i = parsedValues.begin(); i != parsedValues.end(); i++) {
        parsedBoolValues.push_back(parse<bool>(*i));
    }
    return parsedBoolValues;
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


FXIcon*
GNEAttributeCarrier::getIcon() const {
    return GUIIconSubSys::getIcon(myIcon);
}


GUIIcon
GNEAttributeCarrier::getGUIIcon() const {
    return myIcon;
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


std::string
GNEAttributeCarrier::getAttributeType(SumoXMLTag tag, SumoXMLAttr attr) {
    if (isInt(tag, attr)) {
        return "int";
    } else if (isFloat(tag, attr)) {
        return "float";
    } else if (isTime(tag, attr)) {
        return "time";
    } else if (isBool(tag, attr)) {
        return "bool";
    } else if (isString(tag, attr)) {
        return "string";
    } else if (isList(tag, attr)) {
        return "list";
    } else {
        throw ProcessError("Undeterminted type for '" + toString(tag) + "' '" + toString(attr) + "'");
    }
}


bool
GNEAttributeCarrier::isValidID(const std::string& value) {
    return value.find_first_of(" \t\n\r@$%^&/|\\{}*'\";:<>") == std::string::npos;
}


bool
GNEAttributeCarrier::isValidFilename(const std::string& value) {
    // @note Only characteres that aren't permited in a file path or belong
    // to XML sintax
    return (value.find_first_of("\t\n\r@$%^&|\\{}*'\";:<>") == std::string::npos);
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
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, "13.89"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, "1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NUMLANES, "1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, "all"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, ""));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPREADTYPE, "right"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NAME, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, "default"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(GNE_ATTR_SHAPE_START, "")); // virtual attribute used to define an endPoint
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(GNE_ATTR_SHAPE_END, ""));   // virtual attribute from to define an endPoint
                break;
            case SUMO_TAG_JUNCTION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, NODEFAULTVALUE)); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_RADIUS, "1.5"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_KEEP_CLEAR, "true"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TLTYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TLID, ""));
                break;
            case SUMO_TAG_LANE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, "13.89"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, "all"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, ""));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, "default"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ACCELERATION, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_INDEX, NODEFAULTVALUE));
                break;
            case SUMO_TAG_POI:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, NODEFAULTVALUE)); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                break;
            case SUMO_TAG_CROSSING:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, "3"));
                break;
            case SUMO_TAG_CONNECTION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PASS, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_KEEP_CLEAR, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONTPOS, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_UNCONTROLLED, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_VISIBILITY_DISTANCE, "4.5"));
                break;
            case SUMO_TAG_BUS_STOP:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LINES, ""));
                break;
            case SUMO_TAG_CONTAINER_STOP:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LINES, ""));
                break;
            case SUMO_TAG_CHARGING_STATION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGINGPOWER, "22000"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EFFICIENCY, "0.95"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEINTRANSIT, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEDELAY, "0"));
                break;
            case SUMO_TAG_E1DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPLIT_VTYPE, "false"));
                break;
            case SUMO_TAG_E2DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, "10"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONT, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_TIME_THRESHOLD, "1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "1.39"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_JAM_DIST_THRESHOLD, "10"));
                break;
            case SUMO_TAG_E3DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_X, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_Y, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_TIME_THRESHOLD, "1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "1.39"));
                break;
            case SUMO_TAG_DET_ENTRY:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, NODEFAULTVALUE));
                break;
            case SUMO_TAG_DET_EXIT:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, NODEFAULTVALUE));
                break;
            case SUMO_TAG_VSS:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANES, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                break;
            case SUMO_TAG_CALIBRATOR:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ROUTEPROBE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OUTPUT, ""));
                break;
            case SUMO_TAG_REROUTER:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PROB, "1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OFF, "false"));
                break;
            case SUMO_TAG_ROUTEPROBE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_BEGIN, "0"));
                break;
            case SUMO_TAG_VAPORIZER:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTTIME, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_END, "10"));
                break;
            case SUMO_TAG_FLOW:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ROUTE, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_BEGIN, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_END, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_VEHSPERHOUR, "-1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PERIOD, "-1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PROB, "-1"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NUMBER, "100"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DEPARTLANE, "first"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DEPARTPOS, "base"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DEPARTSPEED, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ARRIVALLANE, "current"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ARRIVALPOS, "max"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ARRIVALSPEED, "current"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LINE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PERSON_NUMBER, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONTAINER_NUMBER, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_REROUTE, "false"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DEPARTPOS_LAT, "center"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ARRIVALPOS_LAT, ""));
                break;
            case SUMO_TAG_ROUTE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_COLOR, ""));
                break;
            case SUMO_TAG_VTYPE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ACCEL, "2.6"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DECEL, "4.5"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SIGMA, "0.5"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TAU, "1.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, "5.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_MINGAP, "2.5"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_MAXSPEED, "70.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEEDFACTOR, "1.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEEDDEV, "0.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_COLOR, "1,1,0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_VCLASS, "unknown"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EMISSIONCLASS, "P_7_7"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_GUISHAPE, "unknown"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, "2.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_IMGFILE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_IMPATIENCE, "0.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE_CHANGE_MODEL, "LC2013"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CAR_FOLLOW_MODEL, "Krauss"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PERSON_CAPACITY, "4"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONTAINER_CAPACITY, "0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_BOARDING_DURATION, "0.5"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LOADING_DURATION, "90.0"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LATALIGNMENT, "center"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_MINGAP_LAT, "0.12"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_MAXSPEED_LAT, "1.0"));
                break;
            case SUMO_TAG_STEP:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TIME, NODEFAULTVALUE));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, "50.0"));
                break;
            default:
                // Throw exception if tag isn't defined
                throw ProcessError("allowed attributes for tag '" + toString(tag) + "' not defined");
        }
    }
    return _allowedAttributes[tag];
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedTags(bool net) {
    // define on first access
    if (myAllowedNetElementTags.empty()) {
        myAllowedNetElementTags.push_back(SUMO_TAG_EDGE);
        myAllowedNetElementTags.push_back(SUMO_TAG_JUNCTION);
        myAllowedNetElementTags.push_back(SUMO_TAG_LANE);
        myAllowedNetElementTags.push_back(SUMO_TAG_CONNECTION);
        myAllowedNetElementTags.push_back(SUMO_TAG_CROSSING);
    }
    if (myAllowedAdditionalTags.empty()) {
        myAllowedAdditionalTags.push_back(SUMO_TAG_BUS_STOP);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CALIBRATOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CHARGING_STATION);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CONTAINER_STOP);
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_ENTRY);
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_EXIT);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E1DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E2DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E3DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_REROUTER);
        myAllowedAdditionalTags.push_back(SUMO_TAG_ROUTEPROBE);
        myAllowedAdditionalTags.push_back(SUMO_TAG_VAPORIZER);
        myAllowedAdditionalTags.push_back(SUMO_TAG_VSS);
    }
    return net ? myAllowedNetElementTags : myAllowedAdditionalTags;
}


bool
GNEAttributeCarrier::isNumerical(SumoXMLTag tag, SumoXMLAttr attr) {
    return (isInt(tag, attr) || isFloat(tag, attr));
}


bool
GNEAttributeCarrier::isInt(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myNumericalIntAttrs.empty()) {
        // connection
        myNumericalIntAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_FROM_LANE);
        myNumericalIntAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TO_LANE);
        // edge
        myNumericalIntAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_NUMLANES);
        myNumericalIntAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_PRIORITY);
        // lane
        myNumericalIntAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_INDEX);
        // flow
        myNumericalIntAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PROB);
        myNumericalIntAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PERSON_NUMBER);
        myNumericalIntAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_CONTAINER_NUMBER);
        myNumericalIntAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_NUMBER);
        // vehicle type
        myNumericalIntAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_PERSON_CAPACITY);
        myNumericalIntAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_CONTAINER_CAPACITY);
    }
    return myNumericalIntAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isFloat(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myNumericalFloatAttrs.empty()) {
        // bus stop
        myNumericalFloatAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_ENDPOS);
        myNumericalFloatAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_STARTPOS);
        // charging station
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGINGPOWER);
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_EFFICIENCY);
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_ENDPOS);
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_STARTPOS);
        // connection
        myNumericalFloatAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_CONTPOS);
        myNumericalFloatAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_VISIBILITY_DISTANCE);
        // container stop
        myNumericalFloatAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_ENDPOS);
        myNumericalFloatAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_STARTPOS);
        // crossing
        myNumericalFloatAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_WIDTH);
        // E2
        myNumericalFloatAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);
        myNumericalFloatAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_JAM_DIST_THRESHOLD);
        myNumericalFloatAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_LENGTH);
        // E3
        myNumericalFloatAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_X);
        myNumericalFloatAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_Y);
        myNumericalFloatAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);
        // Edge
        myNumericalFloatAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_ENDOFFSET);
        myNumericalFloatAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_LENGTH);
        myNumericalFloatAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_SPEED);
        // Junction
        myNumericalFloatAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_RADIUS);
        // Lane
        myNumericalFloatAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_ENDOFFSET);
        myNumericalFloatAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_SPEED);
        // Rerouter
        myNumericalFloatAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_PROB);
        // Calibrator
        myNumericalFloatAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_POSITION);
        // vehicle type
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_ACCEL);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_DECEL);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_SIGMA);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_TAU);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_LENGTH);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MINGAP);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MAXSPEED);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_SPEEDFACTOR);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_SPEEDDEV);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_WIDTH);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MINGAP_LAT);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MAXSPEED_LAT);
        myNumericalFloatAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_IMPATIENCE);
        // flow
        myNumericalFloatAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_VEHSPERHOUR);
        myNumericalFloatAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PERIOD);
        myNumericalFloatAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PROB);
        // step
        myNumericalFloatAttrs[SUMO_TAG_STEP].insert(SUMO_ATTR_SPEED);
    }
    return myNumericalFloatAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isTime(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myTimeAttrs.empty()) {
        // calibrator
        myTimeAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_FREQUENCY);
        // charging station
        myTimeAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGEDELAY);
        // E1
        myTimeAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_FREQUENCY);
        // E2
        myTimeAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_FREQUENCY);
        myTimeAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);
        // E3
        myTimeAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_FREQUENCY);
        myTimeAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);
        // RouteProbe
        myTimeAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_BEGIN);
        myTimeAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_FREQUENCY);
        // Vaporizer
        myTimeAttrs[SUMO_TAG_VAPORIZER].insert(SUMO_ATTR_END);
        myTimeAttrs[SUMO_TAG_VAPORIZER].insert(SUMO_ATTR_STARTTIME);
        // Vehicle type
        myTimeAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_BOARDING_DURATION);
        myTimeAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_LOADING_DURATION);
        // Flow
        myTimeAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_BEGIN);
        myTimeAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_END);
        // step
        myTimeAttrs[SUMO_TAG_STEP].insert(SUMO_ATTR_TIME);
    }
    return myTimeAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isBool(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myBoolAttrs.empty()) {
        // charging station
        myBoolAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGEINTRANSIT);
        // lane
        myBoolAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_ACCELERATION);
        // connection
        myBoolAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_PASS);
        myBoolAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_UNCONTROLLED);
        // crossing
        myBoolAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_PRIORITY);
        // E1
        myBoolAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_SPLIT_VTYPE);
        // E2
        myBoolAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_CONT);
        // junction
        myBoolAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_KEEP_CLEAR);
        // rerouter
        myBoolAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_OFF);
        // flow
        myBoolAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_REROUTE);
    }
    return myBoolAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isString(SumoXMLTag tag, SumoXMLAttr attr) {
    return (!isNumerical(tag, attr) && !isBool(tag, attr) && !isTime(tag, attr));
}


bool
GNEAttributeCarrier::isList(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myListAttrs.empty()) {
        // bus stop
        myListAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_LINES);
        // container stop
        myListAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_LINES);
        // crossing
        myListAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_EDGES);
        // rerouter
        myListAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_EDGES);
        // variable speed signal
        myListAttrs[SUMO_TAG_VSS].insert(SUMO_ATTR_LANES);
        // route
        myListAttrs[SUMO_TAG_ROUTE].insert(SUMO_ATTR_EDGES);
    }
    return myListAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isUnique(SumoXMLTag tag, SumoXMLAttr attr) {
    // ID is an atribute that always is unique
    if (attr == SUMO_ATTR_ID) {
        return true;
    } else {
        // define on first access
        if (myUniqueAttrs.empty()) {
            // connection
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_FROM_LANE);
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TO);
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TO_LANE);
            // edge
            myUniqueAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_FROM);
            myUniqueAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_TO);
            // busstop
            myUniqueAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_ENDPOS);
            myUniqueAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_STARTPOS);
            // calibrator
            myUniqueAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_OUTPUT);
            myUniqueAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_ROUTEPROBE);
            // charging station
            myUniqueAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_ENDPOS);
            myUniqueAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_STARTPOS);
            // connection
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_FROM);
            // container stop
            myUniqueAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_ENDPOS);
            myUniqueAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_STARTPOS);
            // crossing
            myUniqueAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_EDGES);
            // det entry
            myUniqueAttrs[SUMO_TAG_DET_ENTRY].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_DET_ENTRY].insert(SUMO_ATTR_POSITION);
            // det exit
            myUniqueAttrs[SUMO_TAG_DET_EXIT].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_DET_EXIT].insert(SUMO_ATTR_POSITION);
            // E1
            myUniqueAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_FILE);
            myUniqueAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_POSITION);
            // E2
            myUniqueAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_FILE);
            myUniqueAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_POSITION);
            // E3
            myUniqueAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_FILE);
            myUniqueAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_X);
            myUniqueAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_Y);
            // Edge
            myUniqueAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_SHAPE);
            // Junction
            myUniqueAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_POSITION);
            myUniqueAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_SHAPE);
            myUniqueAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_TLID);
            // POI
            myUniqueAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_POSITION);
            // Rerouter
            myUniqueAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_EDGES);
            myUniqueAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_FILE);
            // Routeprobe
            myUniqueAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_EDGE);
            myUniqueAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_FILE);
            // Vaporizer
            myUniqueAttrs[SUMO_TAG_VAPORIZER].insert(SUMO_ATTR_EDGE);
            myUniqueAttrs[SUMO_TAG_VAPORIZER].insert(SUMO_ATTR_FILE);
            // VSS
            myUniqueAttrs[SUMO_TAG_VSS].insert(SUMO_ATTR_FILE);
        }
        return myUniqueAttrs[tag].count(attr) == 1;
    }
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
GNEAttributeCarrier::isPositive(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myPositiveAttrs.empty()) {
        // edge
        myPositiveAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_SPEED);
        myPositiveAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_PRIORITY);
        myPositiveAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_NUMLANES);
        myPositiveAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_LENGTH);
        myPositiveAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_WIDTH);
        myPositiveAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_ENDOFFSET);
        // junction
        myPositiveAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_RADIUS);
        // lane
        myPositiveAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_SPEED);
        myPositiveAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_WIDTH);
        myPositiveAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_ENDOFFSET);
        // poi
        myPositiveAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_POSITION);
        // crossing
        myPositiveAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_PRIORITY);
        // connection
        myPositiveAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_CONTPOS);
        myPositiveAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_VISIBILITY_DISTANCE);
        // busstop
        myPositiveAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_STARTPOS);
        myPositiveAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_ENDPOS);
        // container stop
        myPositiveAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_STARTPOS);
        myPositiveAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_ENDPOS);
        // charging station
        myPositiveAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_STARTPOS);
        myPositiveAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_ENDPOS);
        myPositiveAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGINGPOWER);
        myPositiveAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_EFFICIENCY);
        myPositiveAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGEINTRANSIT);
        // E1
        myPositiveAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_POSITION);
        // E2
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_POSITION);
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_LENGTH);
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_JAM_DIST_THRESHOLD);
        // entry
        myPositiveAttrs[SUMO_TAG_DET_ENTRY].insert(SUMO_ATTR_POSITION);
        // exit
        myPositiveAttrs[SUMO_TAG_DET_EXIT].insert(SUMO_ATTR_POSITION);
        // calibrator
        myPositiveAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_POSITION);
        // flow
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PERSON_NUMBER);
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_CONTAINER_NUMBER);
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_VEHSPERHOUR);
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PERIOD);
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_NUMBER);
        // vehicle type
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_ACCEL);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_DECEL);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_SIGMA);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_TAU);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_LENGTH);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MINGAP);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MAXSPEED);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_SPEEDFACTOR);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_SPEEDDEV);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_WIDTH);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MINGAP_LAT);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_MAXSPEED_LAT);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_PERSON_CAPACITY);
        myPositiveAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_CONTAINER_CAPACITY);
    }
    return myPositiveAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isProbability(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myProbabilityAttrs.empty()) {
        // charging station
        myProbabilityAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_EFFICIENCY);
        // rerouter
        myProbabilityAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_PROB);
        // flow
        myProbabilityAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PROB);
    }
    return myProbabilityAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isFilename(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myFileAttrs.empty()) {
        // E1
        myFileAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_FILE);
        // E2
        myFileAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_FILE);
        // E3
        myFileAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_FILE);
        // calibrator
        myFileAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_OUTPUT);
        // rerouter
        myFileAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_FILE);
        // routeprobe
        myFileAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_FILE);
        // Variable Speed Signal
        myFileAttrs[SUMO_TAG_VSS].insert(SUMO_ATTR_FILE);
    }
    return myFileAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isNonEditable(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myNonEditableAttrs.empty()) {
        // connection
        myNonEditableAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_FROM);
        myNonEditableAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_FROM_LANE);
        myNonEditableAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TO);
        myNonEditableAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TO_LANE);
        // crossing
        myNonEditableAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_ID);
        // lane
        myNonEditableAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_INDEX);
    }
    return myNonEditableAttrs[tag].count(attr) == 1;
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



bool
GNEAttributeCarrier::hasDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const std::vector<std::pair <SumoXMLAttr, std::string> >& attrs = allowedAttributes(tag);
    for (std::vector<std::pair <SumoXMLAttr, std::string> >::const_iterator i = attrs.begin(); i != attrs.end(); i++) {
        if ((*i).first == attr) {
            if ((*i).second != NODEFAULTVALUE) {
                return true;
            } else {
                return false;
            }
        }
    }
    throw ProcessError("Attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' not defined");
}


const std::vector<std::string>&
GNEAttributeCarrier::discreteChoices(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myDiscreteChoices.empty()) {
        std::vector<std::string> choices;
        // Get type of nodes
        choices = SUMOXMLDefinitions::NodeTypes.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            if (*it != toString(NODETYPE_DEAD_END_DEPRECATED)) {
                // junction
                myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE].push_back(*it);
            }
        }
        // Get types of traffic lights
        choices = SUMOXMLDefinitions::TrafficLightTypes.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            if (*it != toString(TLTYPE_INVALID)) {
                // junction
                myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TLTYPE].push_back(*it);
            }
        }
        // get type of lane spread functions
        choices = SUMOXMLDefinitions::LaneSpreadFunctions.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            // edge
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE].push_back(*it);
        }
        // get vehicle types
        choices = SumoVehicleClassStrings.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            // edge
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_ALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_DISALLOW].push_back(*it);
            // lane
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_ALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_DISALLOW].push_back(*it);
            // vehicle type
            myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_VCLASS].push_back(*it);
        }
        // get vehicle shapes
        choices = SumoVehicleShapeStrings.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            // vehicle type
            myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_GUISHAPE].push_back(*it);
        }
        // lat alignments of vehicle types
        myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_LATALIGNMENT].push_back("left");
        myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_LATALIGNMENT].push_back("right");
        myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_LATALIGNMENT].push_back("center");
        myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_LATALIGNMENT].push_back("compact");
        myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_LATALIGNMENT].push_back("nice");
        myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_LATALIGNMENT].push_back("arbitrary");
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
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_ID] = "The id of the edge (must be unique).";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_FROM] = "The name of a node within the nodes-file the edge shall start at.";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_TO] = "The name of a node within the nodes-file the edge shall end at.";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_SPEED] = "The maximum speed allowed on the edge in m/s.";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_PRIORITY] = "The priority of the edge.";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_NUMLANES] = "The number of lanes of the edge.";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_TYPE] = "The name of a type within the SUMO edge type file";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_ALLOW] = "Explicitly allows the given vehicle classes (not given will be not allowed).";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_DISALLOW] = "Explicitly disallows the given vehicle classes (not given will be allowed).";
        //myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_PREFER] = "";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_SHAPE] = "If the shape is given it should start and end with the positions of the from-node and to-node.";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_LENGTH] = "The length of the edge in meter";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE] = "Lane width for all lanes of this edge in meters (used for visualization).";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_NAME] = "street name (need not be unique, used for visualization).";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_WIDTH] = "Lane width for all lanes of this edge in meters (used for visualization).";
        myAttrDefinitions[SUMO_TAG_EDGE][SUMO_ATTR_ENDOFFSET] = "Move the stop line back from the intersection by the given amount.";
        // Junction
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_ID] = "The name of the node (Must be unique).";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_POSITION] = "The x-y-z position of the node on the plane in meters.";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE] = "An optional type for the node.";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_SHAPE] = "A custom shape for that node.";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_RADIUS] = "Optional turning radius (for all corners) for that node in meters.";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR] = "Whether the junction-blocking-heuristic should be activated at this node.";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_TLTYPE] = "An optional type for the traffic light algorithm.";
        myAttrDefinitions[SUMO_TAG_JUNCTION][SUMO_ATTR_TLID] = "An optional id for the traffic light program.";
        // Lane
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_ID] = "ID of lane (Automatic)";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_SPEED] = "Speed in meters per second";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_ALLOW] = "Explicitly allows the given vehicle classes (not given will be not allowed).";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_DISALLOW] = "Explicitly disallows the given vehicle classes (not given will be allowed).";
        //myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_PREFER] = "";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_WIDTH] = "Width in meters (used for visualization).";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_ENDOFFSET] = "Move the stop line back from the intersection by the given amount.";
        myAttrDefinitions[SUMO_TAG_LANE][SUMO_ATTR_INDEX] = "The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one).";
        // POI
        myAttrDefinitions[SUMO_TAG_POI][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_POI][SUMO_ATTR_POSITION] = "The position of the poi along the xyz-axis in meters.";
        myAttrDefinitions[SUMO_TAG_POI][SUMO_ATTR_TYPE] = "A typename for the poi.";
        // Crossing
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_ID] = "ID (Automatic)";
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY] = "Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections).";
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_WIDTH] = "The width of the crossings.";
        myAttrDefinitions[SUMO_TAG_CROSSING][SUMO_ATTR_EDGES] = "The (road) edges which are crossed.";
        // Connection
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_FROM] = "The name of the edge the vehicles leave ";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_TO] = "The name of the edge the vehicles may reach when leaving 'from'";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_FROM_LANE] = "the lane index of the incoming lane (numbers starting with 0)";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_TO_LANE] = "the lane index of the outgoing lane (numbers starting with 0)";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_PASS] = "if set, vehicles which pass this (lane-2-lane) connection) will not wait";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_KEEP_CLEAR] = "if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection.";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_CONTPOS] = "If set to a more than 0 value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection.";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_UNCONTROLLED] = "if set to true, This connection will not be TLS-controlled despite its node being controlled.";
        myAttrDefinitions[SUMO_TAG_CONNECTION][SUMO_ATTR_VISIBILITY_DISTANCE] = "sets the distance to the connection at which all relevant foes are visible.";
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
        myAttrDefinitions[SUMO_TAG_E3DETECTOR][SUMO_ATTR_HALTING_TIME_THRESHOLD] = "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting; in s, default: 1s";
        myAttrDefinitions[SUMO_TAG_E3DETECTOR][SUMO_ATTR_HALTING_SPEED_THRESHOLD] = "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting; in m/s, default: 5/3.6m/s";
        myAttrDefinitions[SUMO_TAG_E3DETECTOR][SUMO_ATTR_X] = "X position in editor (Only used in netedit)";
        myAttrDefinitions[SUMO_TAG_E3DETECTOR][SUMO_ATTR_Y] = "Y position in editor (Only used in netedit)";
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
        myAttrDefinitions[SUMO_TAG_CALIBRATOR][SUMO_ATTR_LANE] = "The id of lane in the simulation network";
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
        // route probe
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_ID] = "ID (Must be unique)";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_EDGE] = "The id of an edge in the simulation network";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_FREQUENCY] = "The frequency in which to report the distribution";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_FILE] = "The file for generated output";
        myAttrDefinitions[SUMO_TAG_ROUTEPROBE][SUMO_ATTR_BEGIN] = "The time at which to start generating output";
        // flow
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_ID] = "The name of the vehicle (Must be unique)";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_TYPE] = "The id of the vehicle type to use for this vehicle.";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_ROUTE] = "The id of the route the vehicle shall drive along";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_BEGIN] = "First vehicle departure time";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_END] = "End of departure interval";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_VEHSPERHOUR] = "Number of vehicles per hour, equally spaced (not together with period or probability)";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_PERIOD] = "Insert equally spaced vehicles at that period (not together with vehsPerHour or probability)";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_PROB] = "Probability for emitting a vehicle each second (not together with vehsPerHour or period)";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_NUMBER] = "Total number of vehicles, equally spaced";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_DEPARTLANE] = "The lane on which the vehicle shall be inserted";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_DEPARTPOS] = "The position at which the vehicle shall enter the net";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_DEPARTSPEED] = "The speed with which the vehicle shall enter the network";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_ARRIVALLANE] = "The lane at which the vehicle shall leave the network";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_ARRIVALPOS] = "The position at which the vehicle shall leave the network";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_ARRIVALSPEED] = "The speed with which the vehicle shall leave the network";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_LINE] = "A string specifying the id of a public transport line which can be used when specifying person rides";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_PERSON_NUMBER] = "The number of occupied seats when the vehicle is inserted";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_CONTAINER_NUMBER] = "The number of occupied container places when the vehicle is inserted";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_REROUTE] = "	Whether the vehicle should be equipped with a rerouting device";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_DEPARTPOS_LAT] = "The lateral position on the departure lane at which the vehicle shall enter the net";
        myAttrDefinitions[SUMO_TAG_FLOW][SUMO_ATTR_ARRIVALPOS_LAT] = "The lateral position on the arrival lane at which the vehicle shall arrive";
        // route
        myAttrDefinitions[SUMO_TAG_ROUTE][SUMO_ATTR_ID] = "The name of the route (Must be unique)";
        myAttrDefinitions[SUMO_TAG_ROUTE][SUMO_ATTR_EDGES] = "The edges the vehicle shall drive along, given as their ids, separated using spaces";
        myAttrDefinitions[SUMO_TAG_ROUTE][SUMO_ATTR_COLOR] = "This route's color";
        // vehicle type
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_ID] = "The name of the vehicle type (Must be unique)";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_ACCEL] = "The acceleration ability of vehicles of this type [m/s^2]";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_DECEL] = "The deceleration ability of vehicles of this type [m/s^2]";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_SIGMA] = "Car-following model parameter";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_TAU] = "Car-following model parameter";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_LENGTH] = "The vehicle's netto-length (length) [m]";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_MINGAP] = "Empty space after leader [m]";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_MAXSPEED] = "The vehicle's maximum velocity [m/s]";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_SPEEDFACTOR] = "The vehicles expected multiplicator for lane speed limits";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_SPEEDDEV] = "The deviation of the speedFactor";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_COLOR] = "This vehicle type's color";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_VCLASS] = "An abstract vehicle class";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_EMISSIONCLASS] = "An abstract emission class";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_GUISHAPE] = "How this vehicle is rendered";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_WIDTH] = "The vehicle's width [m] (only used for drawing)";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_IMGFILE] = "Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_IMPATIENCE] = "Willingess of drivers to impede vehicles with higher priority";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_LANE_CHANGE_MODEL] = "The model used for changing lanes";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_CAR_FOLLOW_MODEL] = "The model used for car following";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_PERSON_CAPACITY] = "The number of persons (excluding an autonomous driver) the vehicle can transport";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_CONTAINER_CAPACITY] = "The number of containers the vehicle can transport";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_BOARDING_DURATION] = "The time required by a person to board the vehicle";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_LOADING_DURATION] = "The time required to load a container onto the vehicle";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_LATALIGNMENT] = "The preferred lateral alignment when using the sublane-model";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_MINGAP_LAT] = "The minimum lateral gap at a speed difference of 50km/h when using the sublane-model";
        myAttrDefinitions[SUMO_TAG_VTYPE][SUMO_ATTR_MAXSPEED_LAT] = "The maximum lateral speed when using the sublane-model";
    }
    return myAttrDefinitions[tag][attr];
}


int
GNEAttributeCarrier::getHigherNumberOfAttributes() {
    if (myMaxNumAttribute == 0) {
        // initialize both vectors
        GNEAttributeCarrier::allowedTags(true);
        for (std::vector<SumoXMLTag>::const_iterator i = myAllowedNetElementTags.begin(); i != myAllowedNetElementTags.end(); i++) {
            myMaxNumAttribute = MAX2(myMaxNumAttribute, (int)allowedAttributes(*i).size());
        }
        for (std::vector<SumoXMLTag>::const_iterator i = myAllowedAdditionalTags.begin(); i != myAllowedAdditionalTags.end(); i++) {
            myMaxNumAttribute = MAX2(myMaxNumAttribute, (int)allowedAttributes(*i).size());
        }
    }
    return myMaxNumAttribute;
}


template<> int
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<int>((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> double
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<double>((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> bool
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<bool>((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::string
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return (*i).second;
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> SUMOVehicleClass
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<SUMOVehicleClass>((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}



template<> SUMOVehicleShape
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<SUMOVehicleShape>((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<int>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<std::vector<int> >((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<double>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<std::vector<double> >((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<bool>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if (((*i).first == attr) && ((*i).second != NODEFAULTVALUE)) {
            return parse<std::vector<bool> >((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<std::string>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) {
        if ((*i).first == attr) {
            return parse<std::vector<std::string> >((*i).second);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}

/****************************************************************************/

