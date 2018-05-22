/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAttributeCarrier.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Abstract Base class for gui objects which carry attributes
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
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/GeomConvHelper.h>
#include <netbuild/NBEdge.h>

#include "GNEAttributeCarrier.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>

// ===========================================================================
// static members
// ===========================================================================

std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedNetElementTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedShapeTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myDialogTags;
std::map<SumoXMLTag, std::pair<GNEAttributeCarrier::TagValues, std::map<SumoXMLAttr, GNEAttributeCarrier::AttributeValues> > > GNEAttributeCarrier::myAllowedAttributes;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myNonEditableAttrs;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > GNEAttributeCarrier::myDiscreteChoices;
int GNEAttributeCarrier::myMaxNumAttribute = 0;

const std::string GNEAttributeCarrier::LOADED = "loaded";
const std::string GNEAttributeCarrier::GUESSED = "guessed";
const std::string GNEAttributeCarrier::MODIFIED = "modified";
const std::string GNEAttributeCarrier::APPROVED = "approved";
const double GNEAttributeCarrier::INVALID_POSITION = -1000000;

#define NODEFAULTVALUE "<NODEFAULTVALUE>"
#define OPTIONALATTRIBUTE "-1"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAttributeCarrier::TagValues - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrier::TagValues::TagValues() :
    myTagProperty(TAGPROPERTY_NETELEMENT),
    myParentTag(SUMO_TAG_NOTHING) {
}


GNEAttributeCarrier::TagValues::TagValues(int tagProperty, SumoXMLTag parentTag) :
    myTagProperty(tagProperty),
    myParentTag(parentTag) {
}


SumoXMLTag 
GNEAttributeCarrier::TagValues::getParentTag() const {
    return myParentTag;
}


bool 
GNEAttributeCarrier::TagValues::isNetElement() const {
    return (myTagProperty & TAGPROPERTY_NETELEMENT) != 0;
}


bool 
GNEAttributeCarrier::TagValues::isAdditional() const {
    return (myTagProperty & TAGPROPERTY_ADDITIONAL) != 0;
}


bool 
GNEAttributeCarrier::TagValues::isShape() const {
    return (myTagProperty & TAGPROPERTY_SHAPE) != 0;
}


bool 
GNEAttributeCarrier::TagValues::canBlockMovement() const {
    return (myTagProperty & TAGPROPERTY_BLOCKMOVEMENT) != 0;
}


bool 
GNEAttributeCarrier::TagValues::canBlockShape() const {
    return (myTagProperty & TAGPROPERTY_BLOCKSHAPE) != 0;
}


bool 
GNEAttributeCarrier::TagValues::canCloseShape() const {
    return (myTagProperty & TAGPROPERTY_CLOSESHAPE) != 0;
}


bool 
GNEAttributeCarrier::TagValues::hasGEOPosition() const {
    return (myTagProperty & TAGPROPERTY_GEOPOSITION) != 0;
}


bool 
GNEAttributeCarrier::TagValues::hasGEOShape() const {
    return (myTagProperty & TAGPROPERTY_GEOSHAPE) != 0;
}


bool 
GNEAttributeCarrier::TagValues::hasParent() const {
    return (myTagProperty & TAGPROPERTY_PARENT) != 0;
}

bool 
GNEAttributeCarrier::TagValues::hasDialog() const {
    return (myTagProperty & TAGPROPERTY_DIALOG) != 0;
}

// ---------------------------------------------------------------------------
// GNEAttributeCarrier::AttributeValues - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrier::AttributeValues::AttributeValues() :
    myAttributeProperty(ATTRPROPERTY_STRING),
    myDefinition(""),
    myDefaultValue("") {}


GNEAttributeCarrier::AttributeValues::AttributeValues(int attributeProperty, const std::string &definition, const std::string &defaultValue) :
    myAttributeProperty(attributeProperty),
    myDefinition(definition),
    myDefaultValue(defaultValue) {}


std::string 
GNEAttributeCarrier::AttributeValues::getRestriction() const {
    return "";
}


const std::string&
GNEAttributeCarrier::AttributeValues::getDefinition() const {
    return myDefinition;
}


const std::string&
GNEAttributeCarrier::AttributeValues::getDefaultValue() const {
    return myDefaultValue;
}


std::string 
GNEAttributeCarrier::AttributeValues::getType() const {
    std::string pre;
    std::string type;
    std::string plural;
    // pre type
    if((myAttributeProperty & ATTRPROPERTY_LIST) != 0) {
        pre += "list of ";
        plural = "s";
    }
    if((myAttributeProperty & ATTRPROPERTY_POSITIVE) != 0) {
        pre += "positive ";
    }
    if((myAttributeProperty & ATTRPROPERTY_NONEDITABLE) != 0) {
        pre += "non editable ";
    }
    if((myAttributeProperty & ATTRPROPERTY_DISCRETE) != 0) {
        pre += "discrete ";
    }
    if((myAttributeProperty & ATTRPROPERTY_OPTIONAL) != 0) {
        pre += "optional ";
    }
    // type
    if((myAttributeProperty & ATTRPROPERTY_INT) != 0) {
        type += "integer";
    }
    if((myAttributeProperty & ATTRPROPERTY_FLOAT) != 0) {
        type += "float";
    }
    if((myAttributeProperty & ATTRPROPERTY_BOOL) != 0) {
        type += "boolean";
    }
    if((myAttributeProperty & ATTRPROPERTY_STRING) != 0) {
        type += "string";
    }
    if((myAttributeProperty & ATTRPROPERTY_POSITION) != 0) {
        type += "position";
    }
    if((myAttributeProperty & ATTRPROPERTY_COLOR) != 0) {
        type += "color";
    }
    if((myAttributeProperty & ATTRPROPERTY_SVCPERMISSION) != 0) {
        type += "VClasses";
    }
    if((myAttributeProperty & ATTRPROPERTY_UNIQUE) != 0) {
        type += "unique";
    }
    if((myAttributeProperty & ATTRPROPERTY_FILENAME) != 0) {
        type += "filename";
    }
    if((myAttributeProperty & ATTRPROPERTY_PROBABILITY) != 0) {
        type += "probability";
    }
    if((myAttributeProperty & ATTRPROPERTY_TIME) != 0) {
        type += "time";
    }
    if((myAttributeProperty & ATTRPROPERTY_ANGLE) != 0) {
        type += "angle";
    }
    return pre + type + plural;
}

bool 
GNEAttributeCarrier::AttributeValues::isInt() const {
    return (myAttributeProperty & ATTRPROPERTY_INT) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isFloat() const {
    return (myAttributeProperty & ATTRPROPERTY_FLOAT) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isBool() const {
    return (myAttributeProperty & ATTRPROPERTY_BOOL) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isString() const {
    return (myAttributeProperty & ATTRPROPERTY_STRING) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isProbability() const {
    return (myAttributeProperty & ATTRPROPERTY_PROBABILITY) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isNumerical() const {
    return (myAttributeProperty & (ATTRPROPERTY_INT | ATTRPROPERTY_FLOAT)) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isTime() const {
    return (myAttributeProperty & ATTRPROPERTY_TIME) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isPositive() const {
    return (myAttributeProperty & ATTRPROPERTY_POSITIVE) != 0;
}

bool 
GNEAttributeCarrier::AttributeValues::isColor() const {
    return (myAttributeProperty & ATTRPROPERTY_COLOR) != 0;
}

bool 
GNEAttributeCarrier::AttributeValues::isFilename() const {
    return (myAttributeProperty & ATTRPROPERTY_FILENAME) != 0;
}

bool 
GNEAttributeCarrier::AttributeValues::isSVC() const {
    return (myAttributeProperty & ATTRPROPERTY_SVCPERMISSION) != 0;
}

bool 
GNEAttributeCarrier::AttributeValues::isList() const {
    return (myAttributeProperty & ATTRPROPERTY_LIST) != 0;
}

bool 
GNEAttributeCarrier::AttributeValues::isUnique() const {
    return (myAttributeProperty & ATTRPROPERTY_UNIQUE) != 0;
}

bool 
GNEAttributeCarrier::AttributeValues::isOptional() const {
    return (myAttributeProperty & ATTRPROPERTY_OPTIONAL) != 0;
}

// ---------------------------------------------------------------------------
// GNEAttributeCarrier - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrier::GNEAttributeCarrier(SumoXMLTag tag, GUIIcon icon) :
    myTag(tag),
    myIcon(icon),
    mySelected(false) {
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


template<> RGBColor
GNEAttributeCarrier::parse(const std::string& string) {
    return RGBColor::parseColor(string);
}


template<> Position
GNEAttributeCarrier::parse(const std::string& string) {
    if (string.size() == 0) {
        throw EmptyData();
    } else {
        bool ok = true;
        PositionVector pos = GeomConvHelper::parseShapeReporting(string, "netedit-given", 0, ok, false, false);
        if(!ok || pos.size() != 1) {
            throw NumberFormatException();
        } else {
            return pos[0];
        }
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
    for (auto i : parsedValues) {
        parsedIntValues.push_back(parse<int>(i));
    }
    return parsedIntValues;
}


template<> std::vector<double>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<double> parsedDoubleValues;
    for (auto i : parsedValues) {
        parsedDoubleValues.push_back(parse<double>(i));
    }
    return parsedDoubleValues;
}


template<> std::vector<bool>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<bool> parsedBoolValues;
    for (auto i : parsedValues) {
        parsedBoolValues.push_back(parse<bool>(i));
    }
    return parsedBoolValues;
}


bool 
GNEAttributeCarrier::parseStringToANDBool(const std::string& string) {
    // obtain boolean vector (throw exception if string is empty)
    std::vector<bool> boolValues = GNEAttributeCarrier::parse<std::vector<bool> >(string);
    // set value of checkbox
    if (boolValues.size() == 1) {
        return boolValues.front();
    } else {
        int sum = 0;
        for (auto i : boolValues) {
            sum += (int)(i);
        }
        // only return true if all values are true
        if ((sum == 0) || (sum != (int)boolValues.size())) {
            return false;
        } else {
            return true;
        }
    }
}


std::string
GNEAttributeCarrier::getAttributeForSelection(SumoXMLAttr key) const {
    return getAttribute(key);
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
    for (auto i : allowedAttributes(myTag)) {
        attr.push_back(i.first);
    }
    return attr;
}


const std::string
GNEAttributeCarrier::getID() const {
    return getAttribute(SUMO_ATTR_ID);
}


bool
GNEAttributeCarrier::isValidID(const std::string& value) {
    if(value.size() == 0) {
        return false;
    } else {
        return value.find_first_of(" \t\n\r@$%^&/|\\{}*'\";:<>") == std::string::npos;
    }
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

const std::map<SumoXMLAttr, GNEAttributeCarrier::AttributeValues>&
GNEAttributeCarrier::allowedAttributes(SumoXMLTag tag) {
    // define on first access
    if (!myAllowedAttributes.count(tag)) {
        switch (tag) {
            case SUMO_TAG_EDGE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_NETELEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_UNIQUE, 
                    "The id of the edge", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_FROM] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of a node within the nodes-file the edge shall start at", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_TO] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of a node within the nodes-file the edge shall end at", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_SPEED] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The maximum speed allowed on the edge in m/s", 
                    "13.89");
                myAllowedAttributes[tag].second[SUMO_ATTR_PRIORITY] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE, 
                    "The priority of the edge", 
                    "1");
                myAllowedAttributes[tag].second[SUMO_ATTR_NUMLANES] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE, 
                    "The number of lanes of the edge", 
                    "1");
                myAllowedAttributes[tag].second[SUMO_ATTR_TYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The name of a type within the SUMO edge type file", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_ALLOW] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_SVCPERMISSION, 
                    "Explicitly allows the given vehicle classes (not given will be not allowed)", 
                    "all");
                myAllowedAttributes[tag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_SVCPERMISSION, 
                    "Explicitly disallows the given vehicle classes (not given will be allowed)", 
                    "");
                //myAllowedAttributes[tag].second[SUMO_ATTR_PREFER, );
                myAllowedAttributes[tag].second[SUMO_ATTR_SHAPE] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
                    "If the shape is given it should start and end with the positions of the from-node and to-node", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_LENGTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The length of the edge in meter", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_SPREADTYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "Lane width for all lanes of this edge in meters (used for visualization)", 
                    "right");
                myAllowedAttributes[tag].second[SUMO_ATTR_NAME] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "street name (need not be unique, used for visualization)", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Lane width for all lanes of this edge in meters (used for visualization)", 
                    "default");
                myAllowedAttributes[tag].second[SUMO_ATTR_ENDOFFSET] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Move the stop line back from the intersection by the given amount", 
                    "0");
                myAllowedAttributes[tag].second[GNE_ATTR_SHAPE_START] = AttributeValues(
                    ATTRPROPERTY_POSITION, // virtual attribute used to define an endPoint
                    "Custom position in which shape start (by default position of junction from)", 
                    "");
                myAllowedAttributes[tag].second[GNE_ATTR_SHAPE_END] = AttributeValues(
                    ATTRPROPERTY_POSITION, // virtual attribute from to define an endPoint
                    "Custom position in which shape end (by default position of junction from)", 
                    "");
                myAllowedAttributes[tag].second[GNE_ATTR_BIDIR] = AttributeValues(
                    ATTRPROPERTY_BOOL, // virtual attribute to check of this edge is part of a bidirectional railway
                    "Enable or disable bidireccional edge", 
                    "");
                break;
            case SUMO_TAG_JUNCTION:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_NETELEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the node", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_POSITION, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                    "The x-y-z position of the node on the plane in meters", 
                    NODEFAULTVALUE); 
                myAllowedAttributes[tag].second[SUMO_ATTR_TYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "An optional type for the node", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_SHAPE] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
                    "A custom shape for that node", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_RADIUS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Optional turning radius (for all corners) for that node in meters", 
                    "1.5");
                myAllowedAttributes[tag].second[SUMO_ATTR_KEEP_CLEAR] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Whether the junction-blocking-heuristic should be activated at this node", 
                    "1");
                myAllowedAttributes[tag].second[SUMO_ATTR_TLTYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "An optional type for the traffic light algorithm", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_TLID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "An optional id for the traffic light program", 
                    "");
                break;
            case SUMO_TAG_LANE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_NETELEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "ID of lane (Automatic)", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_SPEED] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Speed in meters per second", 
                    "13.89");
                myAllowedAttributes[tag].second[SUMO_ATTR_ALLOW] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_SVCPERMISSION, 
                    "Explicitly allows the given vehicle classes (not given will be not allowed)", 
                    "all");
                myAllowedAttributes[tag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_SVCPERMISSION, 
                    "Explicitly disallows the given vehicle classes (not given will be allowed)", 
                    "");
                //myAllowedAttributes[tag].second[SUMO_ATTR_PREFER, );
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Width in meters (used for visualization)", 
                    "default");
                myAllowedAttributes[tag].second[SUMO_ATTR_ENDOFFSET] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Move the stop line back from the intersection by the given amount", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_ACCELERATION] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Enable or disable lane as acceleration lane", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_CUSTOMSHAPE] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST, 
                    "If the shape is given it overrides the computation based on edge shape", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_INDEX] = AttributeValues(
                    ATTRPROPERTY_INT,
                    "The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one)", 
                    NODEFAULTVALUE);
                break;
            case SUMO_TAG_POI:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_SHAPE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_GEOPOSITION);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the POI", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                    "The position in view", 
                    NODEFAULTVALUE); 
                myAllowedAttributes[tag].second[SUMO_ATTR_COLOR] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR, 
                    "The color with which the poi shall be displayed", 
                    "red");
                myAllowedAttributes[tag].second[SUMO_ATTR_TYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "A typename for the poi", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_LAYER] = AttributeValues(
                    ATTRPROPERTY_INT, 
                    "The layer of the poi for drawing and selecting", 
                    "4"); // needed to draw it over lane
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Width of rendered image in meters", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_HEIGHT] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Height of rendered image in meters", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "A bitmap to use for rendering this poi", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_RELATIVEPATH] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Enable or disbale use image file as a relative path", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_ANGLE] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "Angle of rendered image in degree", 
                    "0");
                break;
            case SUMO_TAG_POILANE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_SHAPE | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the POI", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of the lane the poi is located at); the lane must be a part of the loaded network", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The position on the named lane or in the net in meters at which the poi is located at", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION_LAT] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "The lateral offset on the named lane at which the poi is located at", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_COLOR] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR, 
                    "The color with which the poi shall be displayed", 
                    "red");
                myAllowedAttributes[tag].second[SUMO_ATTR_TYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "A typename for the poi", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_LAYER] = AttributeValues(
                    ATTRPROPERTY_INT, 
                    "The layer of the poi for drawing and selecting", 
                    "4"); // needed to draw it over lane
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Width of rendered image in meters", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_HEIGHT] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Height of rendered image in meters", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "A bitmap to use for rendering this poi", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_RELATIVEPATH] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Enable or disbale use image file as a relative path", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_ANGLE] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "Angle of rendered image in degree", 
                    "0");
                break;
            case SUMO_TAG_POLY:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_SHAPE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_BLOCKSHAPE | TAGPROPERTY_CLOSESHAPE | TAGPROPERTY_GEOSHAPE);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the polygon", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_SHAPE] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
                    "The shape of the polygon", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_COLOR] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR, 
                    "The RGBA color with which the polygon shall be displayed", 
                    "green");
                myAllowedAttributes[tag].second[SUMO_ATTR_FILL] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "An information whether the polygon shall be filled", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_LAYER] = AttributeValues(
                    ATTRPROPERTY_INT, 
                    "The layer in which the polygon lies", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_TYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "A typename for the polygon", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "A bitmap to use for rendering this polygon", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_RELATIVEPATH] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Enable or disbale use image file as a relative path", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_ANGLE] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "Angle of rendered image in degree", 
                    "0");
                break;
            case SUMO_TAG_CROSSING:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_NETELEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The ID of Crossing", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_EDGES] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
                    "The (road) edges which are crossed", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_PRIORITY] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections)", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The width of the crossings", 
                    toString(OptionsCont::getOptions().getFloat("default.crossing-width")));
                myAllowedAttributes[tag].second[SUMO_ATTR_TLLINKINDEX] = AttributeValues(
                    ATTRPROPERTY_INT, 
                    "sets the tls-index for this crossing", 
                    "-1");
                myAllowedAttributes[tag].second[SUMO_ATTR_TLLINKINDEX2] = AttributeValues(
                    ATTRPROPERTY_INT, 
                    "sets the opposite-direction tls-index for this crossing", 
                    "-1");
                myAllowedAttributes[tag].second[SUMO_ATTR_CUSTOMSHAPE] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
                    "Overrids default shape of pedestrian crossing", 
                    "");
                break;
            case SUMO_TAG_CONNECTION:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_NETELEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_FROM] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of the edge the vehicles leave", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_TO] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of the edge the vehicles may reach when leaving 'from'", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_FROM_LANE] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE, 
                    "the lane index of the incoming lane (numbers starting with 0)", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_TO_LANE] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE, 
                    "the lane index of the outgoing lane (numbers starting with 0)", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_PASS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "if set, vehicles which pass this (lane-2-lane) connection) will not wait", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_KEEP_CLEAR] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_CONTPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "If set to a more than 0 value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection", 
                    toString(NBEdge::UNSPECIFIED_CONTPOS));
                myAllowedAttributes[tag].second[SUMO_ATTR_UNCONTROLLED] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set to true, This connection will not be TLS-controlled despite its node being controlled", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_VISIBILITY_DISTANCE] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "", 
                    toString(NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE));
                myAllowedAttributes[tag].second[SUMO_ATTR_TLLINKINDEX] = AttributeValues(
                    ATTRPROPERTY_INT, 
                    "sets the distance to the connection at which all relevant foes are visible", 
                    "-1");
                myAllowedAttributes[tag].second[SUMO_ATTR_SPEED] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "sets custom speed limit for the connection", 
                    toString(NBEdge::UNSPECIFIED_SPEED));
                myAllowedAttributes[tag].second[SUMO_ATTR_CUSTOMSHAPE] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
                    "sets custom shape for the connection", 
                    "");
                break;
            case SUMO_TAG_BUS_STOP:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of bus stop", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of the lane the bus stop shall be located at", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The begin position on the lane (the lower position on the lane) in meters", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_NAME] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "Name of bus stop", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_LINES] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_LIST, 
                    "Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes", 
                    "");
                break;
            case SUMO_TAG_ACCESS:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of the lane the stop access shall be located at", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The position on the lane (the lower position on the lane) in meters", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                break;
            case SUMO_TAG_CONTAINER_STOP:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of container stop", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of the lane the container stop shall be located at", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The begin position on the lane (the lower position on the lane) in meters", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_NAME] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "Name of Container Stop", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_LINES] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_LIST, 
                    "meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes", 
                    "");
                break;
            case SUMO_TAG_CHARGING_STATION:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of charging station", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "Lane of the charging station location", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "Begin position in the specified lane", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "End position in the specified lane", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_NAME] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "Name of Charging Station", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_CHARGINGPOWER] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Charging power in W", 
                    "22000.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_EFFICIENCY] = AttributeValues(
                    ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE, 
                    "Charging efficiency [0,1]", 
                    "0.95");
                myAllowedAttributes[tag].second[SUMO_ATTR_CHARGEINTRANSIT] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_CHARGEDELAY] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME,
                    "Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins", 
                    "0.00");
                break;
            case SUMO_TAG_E1DETECTOR:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of E1", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The aggregation period the values the detector collects shall be summed up", 
                    "100.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_FILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "The path to the output file", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_VTYPES] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "Space separated list of vehicle type ids to consider", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                break;
            case SUMO_TAG_E2DETECTOR:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of E2", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The position on the lane the detector shall be laid on in meters", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LENGTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The length of the detector in meters", 
                    "10.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The aggregation period the values the detector collects shall be summed up", 
                    "100.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_FILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "The path to the output file", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_CONT] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Holds the information whether detectors longer than a lane shall be cut off or continued (set it to true for the second case))", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_HALTING_TIME_THRESHOLD] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)", 
                    "1.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_HALTING_SPEED_THRESHOLD] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s", 
                    "1.39");
                myAllowedAttributes[tag].second[SUMO_ATTR_JAM_DIST_THRESHOLD] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m", 
                    "10.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                break;
            case SUMO_TAG_E3DETECTOR:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of E3", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE, 
                    "X-Y position of detector in editor (Only used in NETEDIT)", 
                    "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                myAllowedAttributes[tag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The aggregation period the values the detector collects shall be summed up", 
                    "100.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_FILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "The path to the output file", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_HALTING_TIME_THRESHOLD] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) in s", 
                    "1.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_HALTING_SPEED_THRESHOLD] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s", 
                    "1.39");
                break;
            case SUMO_TAG_DET_ENTRY:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, SUMO_TAG_E3DETECTOR);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The position on the lane the detector shall be laid on in meters", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                break;
            case SUMO_TAG_DET_EXIT:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, SUMO_TAG_E3DETECTOR);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The position on the lane the detector shall be laid on in meters", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                break;
            case SUMO_TAG_VSS:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                    "The id of Variable Speed Signal", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE, 
                    "X-Y position of detector in editor (Only used in NETEDIT)", 
                    "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                myAllowedAttributes[tag].second[SUMO_ATTR_LANES] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_LIST, 
                    "list of lanes of Variable Speed Sign", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_FILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_UNIQUE, 
                    "The path to the output file", 
                    "");
                break;
            case SUMO_TAG_CALIBRATOR:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of Calibrator", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_EDGE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of edge in the simulation network", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The position of the calibrator on the specified lane", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The aggregation interval in which to calibrate the flows. default is step-length", 
                    "1.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_ROUTEPROBE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The id of the routeProbe element from which to determine the route distribution for generated vehicles", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_OUTPUT] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "The output file for writing calibrator information or NULL", 
                    "");
                break;
            case SUMO_TAG_LANECALIBRATOR:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of Calibrator", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of lane in the simulation network", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The position of the calibrator on the specified lane", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The aggregation interval in which to calibrate the flows. default is step-length", 
                    "100.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_ROUTEPROBE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The id of the routeProbe element from which to determine the route distribution for generated vehicles", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_OUTPUT] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "The output file for writing calibrator information or NULL", 
                    "");
                break;
            case SUMO_TAG_REROUTER:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of Rerouter", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_EDGES] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
                    "An edge id or a list of edge ids where vehicles shall be rerouted", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_POSITION] = AttributeValues(
                    ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE, 
                    "X,Y position in editor (Only used in NETEDIT)", 
                    "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                myAllowedAttributes[tag].second[SUMO_ATTR_FILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "The path to the definition file (alternatively, the intervals may defined as children of the rerouter)", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_PROB] = AttributeValues(
                    ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE, 
                    "The probability for vehicle rerouting (0-1)", 
                    "1.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_HALTING_TIME_THRESHOLD] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The waiting time threshold (in s) that must be reached to activate rerouting (default -1 which disables the threshold)", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_OFF] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Whether the router should be inactive initially (and switched on in the gui)", 
                    "0");
                break;
            case SUMO_TAG_ROUTEPROBE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of RouteProbe", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_EDGE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of an edge in the simulation network", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The frequency in which to report the distribution", 
                    OPTIONALATTRIBUTE);                
                myAllowedAttributes[tag].second[SUMO_ATTR_FILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "The file for generated output", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_BEGIN] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The time at which to start generating output", 
                    "0");
                break;
            case SUMO_TAG_VAPORIZER:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_EDGE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "Edge in which vaporizer is placed", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_STARTTIME] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "Start Time", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_END] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "End Time", 
                    "10.00");
                break;
            case SUMO_TAG_PARKING_AREA:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of ParkingArea", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The name of the lane the Parking Area shall be located at", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The begin position on the lane (the lower position on the lane) in meters", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ROADSIDE_CAPACITY] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE, 
                    " The number of parking spaces for road-side parking ", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_NAME] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "Name of Parking Area", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The width of the road-side parking spaces", 
                    "3.2");
                myAllowedAttributes[tag].second[SUMO_ATTR_LENGTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The length of the road-side parking spaces", 
                    "5.0");
                myAllowedAttributes[tag].second[SUMO_ATTR_ANGLE] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise", 
                    "0.00");
                break;
            case SUMO_TAG_PARKING_SPACE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, SUMO_TAG_PARKING_AREA);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_X] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The X position in meters of the parking vehicle", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_Y] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
                    "The Y position in meters of the parking vehicle", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_Z] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "The Z position in meters of the parking vehicle", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The width of the road-side parking spaces", 
                    "3.2");
                myAllowedAttributes[tag].second[SUMO_ATTR_LENGTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The length of the road-side parking spaces", 
                    "5.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_ANGLE] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise", 
                    "0");
                break;
            case SUMO_TAG_FLOW:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of the vehicle type to use for this vehicle", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_TYPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The id of the vehicle type to use for this vehicle", 
                    DEFAULT_VTYPE_ID);
                myAllowedAttributes[tag].second[SUMO_ATTR_ROUTE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The id of the route the vehicle shall drive along", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_VEHSPERHOUR] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Number of vehicles per hour, equally spaced", 
                    OPTIONALATTRIBUTE);
                myAllowedAttributes[tag].second[SUMO_ATTR_SPEED] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Speed of vehicles", 
                    OPTIONALATTRIBUTE);
                myAllowedAttributes[tag].second[SUMO_ATTR_COLOR] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR, 
                    "This vehicle's color", 
                    "yellow");
                myAllowedAttributes[tag].second[SUMO_ATTR_BEGIN] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "First vehicle departure time",  
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_END] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "End of departure interval", 
                    "100.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_DEPARTLANE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The lane on which the vehicle shall be inserted", 
                    "first");
                myAllowedAttributes[tag].second[SUMO_ATTR_DEPARTPOS] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE, 
                    "The position at which the vehicle shall enter the net", 
                    "base");
                myAllowedAttributes[tag].second[SUMO_ATTR_DEPARTSPEED] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE, 
                    "The speed with which the vehicle shall enter the network", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_ARRIVALLANE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The lane at which the vehicle shall leave the network", 
                    "current");
                myAllowedAttributes[tag].second[SUMO_ATTR_ARRIVALPOS] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE, 
                    "The position at which the vehicle shall leave the network", 
                    "max");
                myAllowedAttributes[tag].second[SUMO_ATTR_ARRIVALSPEED] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE, 
                    "The speed with which the vehicle shall leave the network", 
                    "current");
                myAllowedAttributes[tag].second[SUMO_ATTR_LINE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "A string specifying the id of a public transport line which can be used when specifying person rides", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_PERSON_NUMBER] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE,
                    "The number of occupied seats when the vehicle is inserted", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_CONTAINER_NUMBER] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE, 
                    "The number of occupied container places when the vehicle is inserted",  
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_REROUTE] = AttributeValues(
                    ATTRPROPERTY_BOOL, 
                    "Whether the vehicle should be equipped with a rerouting device", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_DEPARTPOS_LAT] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The lateral position on the departure lane at which the vehicle shall enter the net", 
                    "center");
                myAllowedAttributes[tag].second[SUMO_ATTR_ARRIVALPOS_LAT] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The lateral position on the arrival lane at which the vehicle shall arrive", 
                    "");
                break;
            case SUMO_TAG_ROUTE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of Route", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_EDGES] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_LIST, 
                    "The edges the vehicle shall drive along, given as their ids, separated using spaces", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_COLOR] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR, 
                    "This route's color", 
                    "yellow");
                break;
            case SUMO_TAG_VTYPE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "The id of VehicleType", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ACCEL] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The acceleration ability of vehicles of this type [m/s^2]", 
                    "2.60");
                myAllowedAttributes[tag].second[SUMO_ATTR_DECEL] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The deceleration ability of vehicles of this type [m/s^2]", 
                    "4.50");
                myAllowedAttributes[tag].second[SUMO_ATTR_SIGMA] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Car-following model parameter", 
                    "0.50");
                myAllowedAttributes[tag].second[SUMO_ATTR_TAU] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Car-following model parameter", 
                    "1.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_LENGTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The vehicle's netto-length (length) [m]", 
                    "5.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_MINGAP] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Empty space after leader [m]", 
                    "2.50");
                myAllowedAttributes[tag].second[SUMO_ATTR_MAXSPEED] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The vehicle's maximum velocity [m/s]", 
                    "70.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_SPEEDFACTOR] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The vehicles expected multiplicator for lane speed limits", 
                    "1.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_SPEEDDEV] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The deviation of the speedFactor", 
                    "0.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_COLOR] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR, 
                    "This vehicle type's color", 
                    "1,1,0");
                myAllowedAttributes[tag].second[SUMO_ATTR_VCLASS] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "An abstract vehicle class", 
                    "passenger");
                myAllowedAttributes[tag].second[SUMO_ATTR_EMISSIONCLASS] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "An abstract emission class", 
                    "P_7_7");
                myAllowedAttributes[tag].second[SUMO_ATTR_GUISHAPE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "How this vehicle is rendered", 
                    "passenger");
                myAllowedAttributes[tag].second[SUMO_ATTR_WIDTH] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The vehicle's width [m] (only used for drawing)", 
                    "2.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME, 
                    "Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)", 
                    "");
                myAllowedAttributes[tag].second[SUMO_ATTR_IMPATIENCE] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "Willingess of drivers to impede vehicles with higher priority", 
                    "0.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_LANE_CHANGE_MODEL] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The model used for changing lanes", 
                    "LC2013");
                myAllowedAttributes[tag].second[SUMO_ATTR_CAR_FOLLOW_MODEL] = AttributeValues(
                    ATTRPROPERTY_STRING, 
                    "The model used for car following", 
                    "Krauss");
                myAllowedAttributes[tag].second[SUMO_ATTR_PERSON_CAPACITY] = AttributeValues(
                    ATTRPROPERTY_INT, 
                    "The number of persons (excluding an autonomous driver) the vehicle can transport", 
                    "4");
                myAllowedAttributes[tag].second[SUMO_ATTR_CONTAINER_CAPACITY] = AttributeValues(
                    ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE,
                    "The number of containers the vehicle can transport", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_BOARDING_DURATION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The time required by a person to board the vehicle", 
                    "0.50");
                myAllowedAttributes[tag].second[SUMO_ATTR_LOADING_DURATION] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "The time required to load a container onto the vehicle", 
                    "90.00");
                myAllowedAttributes[tag].second[SUMO_ATTR_LATALIGNMENT] = AttributeValues(
                    ATTRPROPERTY_FLOAT, 
                    "The preferred lateral alignment when using the sublane-model", 
                    "center");
                myAllowedAttributes[tag].second[SUMO_ATTR_MINGAP_LAT] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The minimum lateral gap at a speed difference of 50km/h when using the sublane-model", 
                    "0.12");
                myAllowedAttributes[tag].second[SUMO_ATTR_MAXSPEED_LAT] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "The maximum lateral speed when using the sublane-model", 
                    "1.00");
                break;
            case SUMO_TAG_STEP:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_TIME] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "Time", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_SPEED] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
                    "Speed", 
                    "50.00");
                break;
            case SUMO_TAG_INTERVAL:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_BEGIN] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "Begin", 
                    "0");
                myAllowedAttributes[tag].second[SUMO_ATTR_END] = AttributeValues(
                    ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
                    "End", 
                    "100.00");
                break;
            case SUMO_TAG_CLOSING_REROUTE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "Edge ID", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ALLOW] = AttributeValues(
                    ATTRPROPERTY_SVCPERMISSION, 
                    "allowed vehicles", 
                    "all");
                myAllowedAttributes[tag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
                    ATTRPROPERTY_SVCPERMISSION, 
                    "disallowed vehicles", 
                    "");
                break;
            case SUMO_TAG_CLOSING_LANE_REROUTE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "Lane ID", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_ALLOW] = AttributeValues(
                    ATTRPROPERTY_SVCPERMISSION, 
                    "allowed vehicles", 
                    "all");
                myAllowedAttributes[tag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
                    ATTRPROPERTY_SVCPERMISSION, 
                    "disallowed vehicles", 
                    "");
                break;
            case SUMO_TAG_DEST_PROB_REROUTE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "Edge ID", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_PROB] = AttributeValues(
                    ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE, 
                    "probability", 
                    "1.00");
                break;
            case SUMO_TAG_PARKING_ZONE_REROUTE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "ParkingArea ID", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_PROB] = AttributeValues(
                    ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE, 
                    "probability", 
                    "1.00");
                break;
            case SUMO_TAG_ROUTE_PROB_REROUTE:
                // set values of tag
                myAllowedAttributes[tag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
                // set values of attributes
                myAllowedAttributes[tag].second[SUMO_ATTR_ID] = AttributeValues(
                    ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
                    "Route", 
                    NODEFAULTVALUE);
                myAllowedAttributes[tag].second[SUMO_ATTR_PROB] = AttributeValues(
                    ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE, 
                    "probability", 
                    "1.00");
                break;
            default:
                // Throw exception if tag isn't defined
                throw ProcessError("allowed attributes for tag '" + toString(tag) + "' not defined");
        }
    }
    return myAllowedAttributes[tag].second;
}


const GNEAttributeCarrier::TagValues & 
GNEAttributeCarrier::getTagProperties(SumoXMLTag tag) {
    if(myAllowedAttributes.size() == 0) {
        allowedAttributes(tag);
    }
    return myAllowedAttributes.at(tag).first;
}


std::vector<SumoXMLTag>
GNEAttributeCarrier::allowedTags() {
    std::vector<SumoXMLTag> tags;
    // append all net elements to tags
    tags.insert(std::end(tags), std::begin(allowedNetElementsTags()), std::end(allowedNetElementsTags()));
    tags.insert(std::end(tags), std::begin(allowedAdditionalTags()), std::end(allowedAdditionalTags()));
    tags.insert(std::end(tags), std::begin(allowedShapeTags()), std::end(allowedShapeTags()));
    return tags;
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedNetElementsTags() {
    // define on first access
    if (myAllowedNetElementTags.empty()) {
        myAllowedNetElementTags.push_back(SUMO_TAG_EDGE);
        myAllowedNetElementTags.push_back(SUMO_TAG_JUNCTION);
        myAllowedNetElementTags.push_back(SUMO_TAG_LANE);
        myAllowedNetElementTags.push_back(SUMO_TAG_CONNECTION);
        myAllowedNetElementTags.push_back(SUMO_TAG_CROSSING);
    }
    return myAllowedNetElementTags;
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedAdditionalTags() {
    // define on first access
    if (myAllowedAdditionalTags.empty()) {
        // Stopping Places
        myAllowedAdditionalTags.push_back(SUMO_TAG_BUS_STOP);
        myAllowedAdditionalTags.push_back(SUMO_TAG_ACCESS);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CONTAINER_STOP);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CHARGING_STATION);
        myAllowedAdditionalTags.push_back(SUMO_TAG_PARKING_AREA);
        myAllowedAdditionalTags.push_back(SUMO_TAG_PARKING_SPACE);
        // Detectors
        myAllowedAdditionalTags.push_back(SUMO_TAG_E1DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E2DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_E3DETECTOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_ENTRY);
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_EXIT);
        // Related with GNERoute Elements
        myAllowedAdditionalTags.push_back(SUMO_TAG_REROUTER);
        myAllowedAdditionalTags.push_back(SUMO_TAG_VSS);
        myAllowedAdditionalTags.push_back(SUMO_TAG_CALIBRATOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_LANECALIBRATOR);
        myAllowedAdditionalTags.push_back(SUMO_TAG_ROUTEPROBE);
        myAllowedAdditionalTags.push_back(SUMO_TAG_VAPORIZER);
    }
    return myAllowedAdditionalTags;
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedShapeTags() {
    // define on first access
    if (myAllowedShapeTags.empty()) {
        myAllowedShapeTags.push_back(SUMO_TAG_POLY);
        myAllowedShapeTags.push_back(SUMO_TAG_POI);
        myAllowedShapeTags.push_back(SUMO_TAG_POILANE);
    }
    return myAllowedShapeTags;
}


bool
GNEAttributeCarrier::canOpenDialog(SumoXMLTag tag) {
    // define on first access
    if (myDialogTags.empty()) {
        myDialogTags.push_back(SUMO_TAG_CALIBRATOR);
        myDialogTags.push_back(SUMO_TAG_LANECALIBRATOR);
        myDialogTags.push_back(SUMO_TAG_REROUTER);
        myDialogTags.push_back(SUMO_TAG_VSS);
    }
    return std::find(myDialogTags.begin(), myDialogTags.end(), tag) != myDialogTags.end();
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
    for (auto i : allowedAttributes(tag)) {
        if (i.first == attr) {
            return true;
        }
    }
    return false;
}


bool
GNEAttributeCarrier::hasDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if (i.first == attr) {
            return (i.second.getDefaultValue() != NODEFAULTVALUE);
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
        for (auto it : choices) {
            if (it != toString(NODETYPE_DEAD_END_DEPRECATED)) {
                // junction
                myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE].push_back(it);
            }
        }
        // Get types of traffic lights
        choices = SUMOXMLDefinitions::TrafficLightTypes.getStrings();
        for (auto it : choices) {
            if (it != toString(TLTYPE_INVALID)) {
                // junction
                myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TLTYPE].push_back(it);
            }
        }
        // get type of lane spread functions
        choices = SUMOXMLDefinitions::LaneSpreadFunctions.getStrings();
        for (auto it : choices) {
            // edge
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE].push_back(it);
        }
        // get vehicle types
        choices = SumoVehicleClassStrings.getStrings();
        for (auto it : choices) {
            // edge
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_ALLOW].push_back(it);
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_DISALLOW].push_back(it);
            // lane
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_ALLOW].push_back(it);
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_DISALLOW].push_back(it);
            // vehicle type
            myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_VCLASS].push_back(it);
        }
        // get vehicle shapes
        choices = SumoVehicleShapeStrings.getStrings();
        for (auto it : choices) {
            // vehicle type
            myDiscreteChoices[SUMO_TAG_VTYPE][SUMO_ATTR_GUISHAPE].push_back(it);
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
GNEAttributeCarrier::discreteCombinableChoices(SumoXMLAttr attr) {
    return (attr == SUMO_ATTR_ALLOW || attr == SUMO_ATTR_DISALLOW);
}


int
GNEAttributeCarrier::getHigherNumberOfAttributes() {
    if (myMaxNumAttribute == 0) {
        for (auto i : allowedNetElementsTags()) {
            myMaxNumAttribute = MAX2(myMaxNumAttribute, (int)allowedAttributes(i).size());
        }
        for (auto i : allowedAdditionalTags()) {
            myMaxNumAttribute = MAX2(myMaxNumAttribute, (int)allowedAttributes(i).size());
        }
        for (auto i : allowedShapeTags()) {
            myMaxNumAttribute = MAX2(myMaxNumAttribute, (int)allowedAttributes(i).size());
        }
    }
    return myMaxNumAttribute;
}


template<> int
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<int>(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> double
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<double>(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> bool
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<bool>(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::string
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return (i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> Position
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<Position>(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> SUMOVehicleClass
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<SUMOVehicleClass>(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> SUMOVehicleShape
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<SUMOVehicleShape>(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> RGBColor
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<RGBColor>(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<int>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<std::vector<int> >(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<double>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<std::vector<double> >(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<bool>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<std::vector<bool> >(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<std::string>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.getDefaultValue() != NODEFAULTVALUE)) {
            return parse<std::vector<std::string> >(i.second.getDefaultValue());
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


bool
GNEAttributeCarrier::checkGNEEdgesValid(GNENet* net, const std::string& value, bool report) {
    // Declare string vector
    std::vector<std::string> edgeIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    // Iterate over edges IDs and check if edge exist
    for (auto i : edgeIds) {
        if (net->retrieveEdge(i, false) == nullptr) {
            if (report) {
                WRITE_WARNING("Error parsing parameter " + toString(SUMO_ATTR_EDGES) + ". " + toString(SUMO_TAG_EDGE) + " '" + i + "'  doesn't exist.");
            }
            return false;
        }
    }
    // all edges exist, then return true
    return true;
}


bool
GNEAttributeCarrier::checkGNELanesValid(GNENet* net, const std::string& value, bool report) {
    // Declare string vector
    std::vector<std::string> laneIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    // Iterate over lanes IDs and check if lane exist
    for (auto i : laneIds) {
        if (net->retrieveLane(i, false) == nullptr) {
            if (report) {
                WRITE_WARNING("Error parsing parameter " + toString(SUMO_ATTR_LANES) + ". " + toString(SUMO_TAG_LANE) + " '" + i + "'  doesn't exist.");
            }
            return false;
        }
    }
    // all lanes exist, then return true
    return true;
}


std::vector<GNEEdge*>
GNEAttributeCarrier::parseGNEEdges(GNENet* net, const std::string& value) {
    // Declare string vector
    std::vector<std::string> edgeIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    std::vector<GNEEdge*> parsedEdges;
    // Iterate over edges IDs, retrieve Edges and add it into parsedEdges
    for (auto i : edgeIds) {
        parsedEdges.push_back(net->retrieveEdge(i));
    }
    return parsedEdges;
}


std::vector<GNELane*>
GNEAttributeCarrier::parseGNELanes(GNENet* net, const std::string& value) {
    // Declare string vector
    std::vector<std::string> laneIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    std::vector<GNELane*> parsedLanes;
    // Iterate over lanes IDs, retrieve Lanes and add it into parsedLanes
    for (auto i : laneIds) {
        parsedLanes.push_back(net->retrieveLane(i));
    }
    return parsedLanes;
}


std::string
GNEAttributeCarrier::parseGNEEdges(const std::vector<GNEEdge*>& edges) {
    // obtain ID's of edges and return their join
    std::vector<std::string> edgeIDs;
    for (auto i : edges) {
        edgeIDs.push_back(i->getID());
    }
    return joinToString(edgeIDs, " ");
}


std::string
GNEAttributeCarrier::parseGNELanes(const std::vector<GNELane*>& lanes) {
    // obtain ID's of lanes and return their join
    std::vector<std::string> laneIDs;
    for (auto i : lanes) {
        laneIDs.push_back(i->getID());
    }
    return joinToString(laneIDs, " ");
}


int 
GNEAttributeCarrier::getCircleResolution(const GUIVisualizationSettings& settings) {
    if(settings.drawForSelecting) {
        return 8;
    } else if (settings.scale >= 10) {
        return 32;
    } else if (settings.scale >= 2) {
        return 16;
    } else if (settings.scale >= 1) {
        return 8;
    } else {
        return 4;
    }
}


void 
GNEAttributeCarrier::writeAttribute(OutputDevice& device, SumoXMLAttr key) const {
    std::string attribute = getAttribute(key);
    if(key == GNE_ATTR_BLOCK_MOVEMENT) {
        // only write Block Movement if is enabled
        if(attribute == "1") {
            device.writeAttr(key, attribute);
        }
    } else if(hasDefaultValue(getTag(), key)) {
        // only write optional attributes (i.e attributes with default value) if are differents
        if(getDefaultValue<std::string>(getTag(), key) != attribute) {
            device.writeAttr(key, attribute);
        }
    } else {
        device.writeAttr(key, attribute);
    }
}


/****************************************************************************/

