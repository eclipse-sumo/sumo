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

std::map<SumoXMLTag, std::vector<std::pair <SumoXMLAttr, GNEAttributeCarrier::AttributeValues> > > GNEAttributeCarrier::myAllowedAttributes;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedNetElementTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedShapeTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myBlockMovementTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myBlockShapeTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myCloseShapeTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myGeoPositionTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myGeoShapeTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myDialogTags;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myNumericalIntAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myNumericalFloatAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myTimeAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myBoolAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myColorAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myListAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myUniqueAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myNonEditableAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myPositiveAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myProbabilityAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::myFileAttrs;
std::map<SumoXMLTag, std::set<SumoXMLAttr> > GNEAttributeCarrier::mySVCPermissionsAttrs;
std::map<SumoXMLTag, SumoXMLTag> GNEAttributeCarrier::myAdditionalsWithParent;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > GNEAttributeCarrier::myDiscreteChoices;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::pair<std::string, std::string> > > GNEAttributeCarrier::myAttrDefinitions;
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

GNEAttributeCarrier::GNEAttributeCarrier(SumoXMLTag tag, GUIIcon icon) :
    myTag(tag),
    myIcon(icon),
    mySelected(false)
{
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
    } else if (isColor(tag, attr)) {
        return "color";
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

const std::vector<std::pair <SumoXMLAttr, GNEAttributeCarrier::AttributeValues> >&
GNEAttributeCarrier::allowedAttributes(SumoXMLTag tag) {
    // define on first access
    if (!myAllowedAttributes.count(tag)) {
        std::vector<std::pair <SumoXMLAttr, AttributeValues> >& attrs = myAllowedAttributes[tag];
        switch (tag) {
            case SUMO_TAG_EDGE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "The id of the edge.", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FROM, AttributeValues(
                    ACPROPERTY_INT, 
                    "The name of a node within the nodes-file the edge shall start at.", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TO, AttributeValues(
                    ACPROPERTY_INT, 
                    "The name of a node within the nodes-file the edge shall end at.", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "The maximum speed allowed on the edge in m/s.", 
                    "13.89")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PRIORITY, AttributeValues(
                    ACPROPERTY_INT, 
                    "The priority of the edge.", 
                    "1")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_NUMLANES, AttributeValues(
                    ACPROPERTY_INT, 
                    "The number of lanes of the edge.", 
                    "1")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "The name of a type within the SUMO edge type file", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "Explicitly allows the given vehicle classes (not given will be not allowed).", 
                    "all")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DISALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "Explicitly disallows the given vehicle classes (not given will be allowed).", 
                    "")));
                //attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PREFER, )));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SHAPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "If the shape is given it should start and end with the positions of the from-node and to-node.", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LENGTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "The length of the edge in meter", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPREADTYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "Lane width for all lanes of this edge in meters (used for visualization).", 
                    "right")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_NAME, AttributeValues(
                    ACPROPERTY_INT, 
                    "street name (need not be unique, used for visualization).", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "Lane width for all lanes of this edge in meters (used for visualization).", 
                    "default")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ENDOFFSET, AttributeValues(
                    ACPROPERTY_INT, 
                    "Move the stop line back from the intersection by the given amount.", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(GNE_ATTR_SHAPE_START, AttributeValues(
                    ACPROPERTY_INT, 
                    "Custom position in which shape start (by default position of junction from)", 
                    ""))); // virtual attribute used to define an endPoint
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(GNE_ATTR_SHAPE_END, AttributeValues(
                    ACPROPERTY_INT, 
                    "Custom position in which shape end (by default position of junction from)", 
                    "")));   // virtual attribute from to define an endPoint
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(GNE_ATTR_BIDIR, AttributeValues(
                    ACPROPERTY_INT, 
                    "Enable or disable bidireccional edge", 
                    "")));       // virtual attribute to check of this edge is part of a bidirectional railway
                break;
            case SUMO_TAG_JUNCTION:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "The id of the node", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "The x-y-z position of the node on the plane in meters.", 
                    NODEFAULTVALUE))); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "An optional type for the node.", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SHAPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "A custom shape for that node.", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_RADIUS, AttributeValues(
                    ACPROPERTY_INT, 
                    "Optional turning radius (for all corners) for that node in meters.", 
                    "1.5")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_KEEP_CLEAR, AttributeValues(
                    ACPROPERTY_INT, 
                    "Whether the junction-blocking-heuristic should be activated at this node.", 
                    "1")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TLTYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "An optional type for the traffic light algorithm.", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TLID, AttributeValues(
                    ACPROPERTY_INT, 
                    "An optional id for the traffic light program.", 
                    "")));
                break;
            case SUMO_TAG_LANE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "ID of lane (Automatic)", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "Speed in meters per second", 
                    "13.89")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "Explicitly allows the given vehicle classes (not given will be not allowed).", 
                    "all")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DISALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "Explicitly disallows the given vehicle classes (not given will be allowed).", 
                    "")));
                //attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PREFER, )));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "Width in meters (used for visualization).", 
                    "default")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ENDOFFSET, AttributeValues(
                    ACPROPERTY_INT, 
                    "Move the stop line back from the intersection by the given amount.", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ACCELERATION, AttributeValues(
                    ACPROPERTY_INT, 
                    "Enable or disable lane as acceleration lane", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CUSTOMSHAPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "If the shape is given it overrides the computation based on edge shape.", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_INDEX, AttributeValues(
                    ACPROPERTY_INT,
                    "The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one).", 
                    NODEFAULTVALUE)));
                break;
            case SUMO_TAG_POI:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "The id of the POI", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "The position in view", 
                    NODEFAULTVALUE))); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_COLOR, AttributeValues(
                    ACPROPERTY_INT, 
                    "The color with which the poi shall be displayed", 
                    "red")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "A typename for the poi", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LAYER, AttributeValues(
                    ACPROPERTY_INT, 
                    "The layer of the poi for drawing and selecting", 
                    "4"))); // needed to draw it over lane
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "Width of rendered image in meters", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_HEIGHT, AttributeValues(
                    ACPROPERTY_INT, 
                    "Height of rendered image in meters", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_IMGFILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "A bitmap to use for rendering this poi", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_RELATIVEPATH, AttributeValues(
                    ACPROPERTY_INT, 
                    "Enable or disbale use image file as a relative path", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ANGLE, AttributeValues(
                    ACPROPERTY_INT, 
                    "Angle of rendered image in degree", 
                    "0")));
                break;
            case SUMO_TAG_POILANE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "The id of the POI", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "The name of the lane the poi is located at); the lane must be a part of the loaded network", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "The position on the named lane or in the net in meters at which the poi is located at", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION_LAT, AttributeValues(
                    ACPROPERTY_INT, 
                    "The lateral offset on the named lane at which the poi is located at", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_COLOR, AttributeValues(
                    ACPROPERTY_INT, 
                    "The color with which the poi shall be displayed", 
                    "red")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "A typename for the poi", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LAYER, AttributeValues(
                    ACPROPERTY_INT, 
                    "The layer of the poi for drawing and selecting", 
                    "4"))); // needed to draw it over lane
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "Width of rendered image in meters", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_HEIGHT, AttributeValues(
                    ACPROPERTY_INT, 
                    "Height of rendered image in meters", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_IMGFILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "A bitmap to use for rendering this poi", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_RELATIVEPATH, AttributeValues(
                    ACPROPERTY_INT, 
                    "Enable or disbale use image file as a relative path", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ANGLE, AttributeValues(
                    ACPROPERTY_INT, 
                    "Angle of rendered image in degree", 
                    "0")));
                break;
            case SUMO_TAG_POLY:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "The id of the polygon", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SHAPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "The shape of the polygon", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_COLOR, AttributeValues(
                    ACPROPERTY_INT, 
                    "The RGBA color with which the polygon shall be displayed", 
                    "green")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FILL, AttributeValues(
                    ACPROPERTY_INT, 
                    "An information whether the polygon shall be filled", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LAYER, AttributeValues(
                    ACPROPERTY_INT, 
                    "The layer in which the polygon lies", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "A typename for the polygon", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_IMGFILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "A bitmap to use for rendering this polygon", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_RELATIVEPATH, AttributeValues(
                    ACPROPERTY_INT, 
                    "Enable or disbale use image file as a relative path", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ANGLE, AttributeValues(
                    ACPROPERTY_INT, 
                    "Angle of rendered image in degree", 
                    "0")));
                break;
            case SUMO_TAG_CROSSING:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "The ID of Crossing", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EDGES, AttributeValues(
                    ACPROPERTY_INT, 
                    "The (road) edges which are crossed.", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PRIORITY, AttributeValues(
                    ACPROPERTY_INT, 
                    "Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections).", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "The width of the crossings.", 
                    toString(OptionsCont::getOptions().getFloat("default.crossing-width")))));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TLLINKINDEX, AttributeValues(
                    ACPROPERTY_INT, 
                    "sets the tls-index for this crossing.", 
                    "-1")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TLLINKINDEX2, AttributeValues(
                    ACPROPERTY_INT, 
                    "sets the opposite-direction tls-index for this crossing.", 
                    "-1")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CUSTOMSHAPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "Overrids default shape of pedestrian crossing.", 
                    "")));
                break;
            case SUMO_TAG_CONNECTION:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FROM, AttributeValues(
                    ACPROPERTY_INT, 
                    "The name of the edge the vehicles leave", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TO, AttributeValues(
                    ACPROPERTY_INT, 
                    "The name of the edge the vehicles may reach when leaving 'from'", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FROM_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "the lane index of the incoming lane (numbers starting with 0)", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TO_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "the lane index of the outgoing lane (numbers starting with 0)", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PASS, AttributeValues(
                    ACPROPERTY_INT, 
                    "if set, vehicles which pass this (lane-2-lane) connection) will not wait", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_KEEP_CLEAR, AttributeValues(
                    ACPROPERTY_INT, 
                    "if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection.", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CONTPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "If set to a more than 0 value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection.", 
                    toString(NBEdge::UNSPECIFIED_CONTPOS))));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_UNCONTROLLED, AttributeValues(
                    ACPROPERTY_INT, 
                    "If set to true, This connection will not be TLS-controlled despite its node being controlled.", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_VISIBILITY_DISTANCE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    toString(NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE))));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TLLINKINDEX, AttributeValues(
                    ACPROPERTY_INT, 
                    "sets the distance to the connection at which all relevant foes are visible.", 
                    "-1")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "sets custom speed limit for the connection.", 
                    toString(NBEdge::UNSPECIFIED_SPEED))));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CUSTOMSHAPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "sets custom shape for the connection.", 
                    "")));
                break;
            case SUMO_TAG_BUS_STOP:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_STARTPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ENDPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_NAME, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LINES, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_ACCESS:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_CONTAINER_STOP:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_STARTPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ENDPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_NAME, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LINES, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_CHARGING_STATION:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_STARTPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ENDPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_NAME, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CHARGINGPOWER, AttributeValues(
                    ACPROPERTY_INT, 
                    "", "22000.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EFFICIENCY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", "0.95")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CHARGEINTRANSIT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CHARGEDELAY, AttributeValues(
                    ACPROPERTY_INT,
                    "", 
                    "0.00")));
                break;
            case SUMO_TAG_E1DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FREQUENCY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "100.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_VTYPES, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_E2DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LENGTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "10.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FREQUENCY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "100.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CONT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_HALTING_TIME_THRESHOLD, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.39")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_JAM_DIST_THRESHOLD, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "10.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_E3DETECTOR:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0,0"))); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FREQUENCY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "100.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_HALTING_TIME_THRESHOLD, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.39")));
                break;
            case SUMO_TAG_DET_ENTRY:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_DET_EXIT:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_VSS:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0,0"))); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANES, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_CALIBRATOR:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EDGE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FREQUENCY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ROUTEPROBE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_OUTPUT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_LANECALIBRATOR:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FREQUENCY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "100.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ROUTEPROBE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_OUTPUT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_REROUTER:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EDGES, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_POSITION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0,0"))); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PROB, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_HALTING_TIME_THRESHOLD, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_OFF, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_ROUTEPROBE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EDGE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FREQUENCY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    OPTIONALATTRIBUTE)));                
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_BEGIN, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_VAPORIZER:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EDGE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_STARTTIME, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_END, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "10.00")));
                break;
            case SUMO_TAG_PARKING_AREA:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_STARTPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ENDPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ROADSIDE_CAPACITY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_FRIENDLY_POS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_NAME, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "3.2")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LENGTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "5.0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ANGLE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0.00")));
                break;
            case SUMO_TAG_PARKING_SPACE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_X, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_Y, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_Z, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "3.2")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LENGTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "5.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ANGLE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                break;
            case SUMO_TAG_FLOW:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TYPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    DEFAULT_VTYPE_ID)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ROUTE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_VEHSPERHOUR, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    OPTIONALATTRIBUTE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    OPTIONALATTRIBUTE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_COLOR, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "yellow")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_BEGIN, AttributeValues(
                    ACPROPERTY_INT, 
                    "",  
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_END, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "100.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DEPARTLANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "first")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DEPARTPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "base")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DEPARTSPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ARRIVALLANE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "current")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ARRIVALPOS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "max")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ARRIVALSPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "current")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LINE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PERSON_NUMBER, AttributeValues(
                    ACPROPERTY_INT,
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CONTAINER_NUMBER,AttributeValues(
                    ACPROPERTY_INT, 
                    "",  
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_REROUTE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DEPARTPOS_LAT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "center")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ARRIVALPOS_LAT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_ROUTE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EDGES, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_COLOR, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "yellow")));
                break;
            case SUMO_TAG_VTYPE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ACCEL, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "2.60")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DECEL, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "4.50")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SIGMA, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0.50")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TAU, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LENGTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "5.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_MINGAP, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "2.50")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_MAXSPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "70.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPEEDFACTOR, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPEEDDEV, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_COLOR, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1,1,0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_VCLASS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "passenger")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_EMISSIONCLASS, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "P_7_7")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_GUISHAPE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "passenger")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_WIDTH, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "2.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_IMGFILE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_IMPATIENCE, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LANE_CHANGE_MODEL, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "LC2013")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CAR_FOLLOW_MODEL, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "Krauss")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PERSON_CAPACITY, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "4")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_CONTAINER_CAPACITY, AttributeValues(
                    ACPROPERTY_INT,
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_BOARDING_DURATION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0.50")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LOADING_DURATION, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "90.00")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_LATALIGNMENT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "center")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_MINGAP_LAT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0.12")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_MAXSPEED_LAT, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                break;
            case SUMO_TAG_STEP:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_TIME, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_SPEED, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "50.00")));
                break;
            case SUMO_TAG_INTERVAL:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_BEGIN, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "0")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_END, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "100.00")));
                break;
            case SUMO_TAG_CLOSING_REROUTE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "all")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DISALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_CLOSING_LANE_REROUTE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "all")));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_DISALLOW, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "")));
                break;
            case SUMO_TAG_DEST_PROB_REROUTE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PROB, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                break;
            case SUMO_TAG_PARKING_ZONE_REROUTE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PROB, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                break;
            case SUMO_TAG_ROUTE_PROB_REROUTE:
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_ID, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    NODEFAULTVALUE)));
                attrs.push_back(std::pair<SumoXMLAttr, AttributeValues>(SUMO_ATTR_PROB, AttributeValues(
                    ACPROPERTY_INT, 
                    "", 
                    "1.00")));
                break;
            default:
                // Throw exception if tag isn't defined
                throw ProcessError("allowed attributes for tag '" + toString(tag) + "' not defined");
        }
    }
    return myAllowedAttributes[tag];
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
GNEAttributeCarrier::canBlockMovement(SumoXMLTag tag) {
    // define on first access
    if (myBlockMovementTags.empty()) {
        myBlockMovementTags.push_back(SUMO_TAG_BUS_STOP);
        myBlockMovementTags.push_back(SUMO_TAG_ACCESS);
        myBlockMovementTags.push_back(SUMO_TAG_CONTAINER_STOP);
        myBlockMovementTags.push_back(SUMO_TAG_CHARGING_STATION);
        myBlockMovementTags.push_back(SUMO_TAG_E1DETECTOR);
        myBlockMovementTags.push_back(SUMO_TAG_E2DETECTOR);
        myBlockMovementTags.push_back(SUMO_TAG_E3DETECTOR);
        myBlockMovementTags.push_back(SUMO_TAG_DET_ENTRY);
        myBlockMovementTags.push_back(SUMO_TAG_DET_EXIT);
        myBlockMovementTags.push_back(SUMO_TAG_REROUTER);
        myBlockMovementTags.push_back(SUMO_TAG_VSS);
        myBlockMovementTags.push_back(SUMO_TAG_POI);
        myBlockMovementTags.push_back(SUMO_TAG_POILANE);
        myBlockMovementTags.push_back(SUMO_TAG_POLY);
        myBlockMovementTags.push_back(SUMO_TAG_PARKING_AREA);
        myBlockMovementTags.push_back(SUMO_TAG_PARKING_SPACE);
    }
    return std::find(myBlockMovementTags.begin(), myBlockMovementTags.end(), tag) != myBlockMovementTags.end();
}


bool
GNEAttributeCarrier::canBlockShape(SumoXMLTag tag) {
    // define on first access
    if (myBlockShapeTags.empty()) {
        myBlockShapeTags.push_back(SUMO_TAG_POLY);
    }
    return std::find(myBlockShapeTags.begin(), myBlockShapeTags.end(), tag) != myBlockShapeTags.end();
}


bool 
GNEAttributeCarrier::canCloseShape(SumoXMLTag tag) {
    // define on first access
    if (myCloseShapeTags.empty()) {
        myCloseShapeTags.push_back(SUMO_TAG_POLY);
    }
    return std::find(myCloseShapeTags.begin(), myCloseShapeTags.end(), tag) != myCloseShapeTags.end();
}


bool 
GNEAttributeCarrier::canHaveParent(SumoXMLTag tag) {
    // define on first access
    if (myAdditionalsWithParent.empty()) {
        myAdditionalsWithParent[SUMO_TAG_DET_ENTRY] = SUMO_TAG_E3DETECTOR;
        myAdditionalsWithParent[SUMO_TAG_DET_EXIT] = SUMO_TAG_E3DETECTOR;
        myAdditionalsWithParent[SUMO_TAG_PARKING_SPACE] = SUMO_TAG_PARKING_AREA;
    }
    return myAdditionalsWithParent.count(tag) == 1;
}


bool 
GNEAttributeCarrier::canUseGeoPosition(SumoXMLTag tag) {
    // define on first access
    if (myGeoPositionTags.empty()) {
        myGeoPositionTags.push_back(SUMO_TAG_POI);
    }
    return std::find(myGeoPositionTags.begin(), myGeoPositionTags.end(), tag) != myGeoPositionTags.end();
}


bool 
GNEAttributeCarrier::canUseGeoShape(SumoXMLTag tag) {
    // define on first access
    if (myGeoShapeTags.empty()) {
        myGeoShapeTags.push_back(SUMO_TAG_POLY);
    }
    return std::find(myGeoShapeTags.begin(), myGeoShapeTags.end(), tag) != myGeoShapeTags.end();
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
        myNumericalIntAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TLLINKINDEX);
        // edge
        myNumericalIntAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_NUMLANES);
        myNumericalIntAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_PRIORITY);
        // lane
        myNumericalIntAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_INDEX);
        // crossing
        myNumericalIntAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_TLLINKINDEX);
        myNumericalIntAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_TLLINKINDEX2);
        // flow
        myNumericalIntAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PERSON_NUMBER);
        myNumericalIntAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_CONTAINER_NUMBER);
        // vehicle type
        myNumericalIntAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_PERSON_CAPACITY);
        myNumericalIntAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_CONTAINER_CAPACITY);
        // POI
        myNumericalIntAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_LAYER);
        // POILane
        myNumericalIntAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_LAYER);
        // Layer
        myNumericalIntAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_LAYER);
        // ParkingArea
        myNumericalIntAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_ROADSIDE_CAPACITY);
    }
    return myNumericalIntAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isFloat(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myNumericalFloatAttrs.empty()) {
        // bus stop
        myNumericalFloatAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_STARTPOS);
        myNumericalFloatAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_ENDPOS);
        // stop access
        myNumericalFloatAttrs[SUMO_TAG_ACCESS].insert(SUMO_ATTR_POSITION);
        // charging station
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_STARTPOS);
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_ENDPOS);
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGINGPOWER);
        myNumericalFloatAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_EFFICIENCY);
        // connection
        myNumericalFloatAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_CONTPOS);
        myNumericalFloatAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_VISIBILITY_DISTANCE);
        myNumericalFloatAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_SPEED);
        // container stop
        myNumericalFloatAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_STARTPOS);
        myNumericalFloatAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_ENDPOS);
        // crossing
        myNumericalFloatAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_WIDTH);
        // E1
        myNumericalFloatAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_POSITION);
        // E2
        myNumericalFloatAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_POSITION);
        myNumericalFloatAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);
        myNumericalFloatAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_JAM_DIST_THRESHOLD);
        myNumericalFloatAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_LENGTH);
        // E3
        myNumericalFloatAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);
        // Entry
        myNumericalFloatAttrs[SUMO_TAG_DET_ENTRY].insert(SUMO_ATTR_POSITION);
        // Exit
        myNumericalFloatAttrs[SUMO_TAG_DET_EXIT].insert(SUMO_ATTR_POSITION);
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
        myNumericalFloatAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);
        // Calibrator (edge)
        myNumericalFloatAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_POSITION);
        // Calibrator (lane
        myNumericalFloatAttrs[SUMO_TAG_LANECALIBRATOR].insert(SUMO_ATTR_POSITION);
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
        myNumericalFloatAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_SPEED);
        // step
        myNumericalFloatAttrs[SUMO_TAG_STEP].insert(SUMO_ATTR_SPEED);
        // POLY
        myNumericalFloatAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_ANGLE);
        // POI
        myNumericalFloatAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_ANGLE);
        myNumericalFloatAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_WIDTH);
        myNumericalFloatAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_HEIGHT);
        // POILane
        myNumericalFloatAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_POSITION);
        myNumericalFloatAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_POSITION_LAT);
        myNumericalFloatAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_ANGLE);
        myNumericalFloatAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_WIDTH);
        myNumericalFloatAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_HEIGHT);
        // offset value
        myNumericalFloatAttrs[SUMO_TAG_STOPOFFSET].insert(SUMO_ATTR_VALUE);
        // Parking Area
        myNumericalFloatAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_STARTPOS);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_ENDPOS);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_ANGLE);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_WIDTH);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_LENGTH);
        // Parking Space
        myNumericalFloatAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_X);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_Y);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_Z);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_ANGLE);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_WIDTH);
        myNumericalFloatAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_LENGTH);

    }
    return myNumericalFloatAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isTime(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myTimeAttrs.empty()) {
        // calibrator (edge)
        myTimeAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_FREQUENCY);
        // calibrator (lane)
        myTimeAttrs[SUMO_TAG_LANECALIBRATOR].insert(SUMO_ATTR_FREQUENCY);
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
        // edge
        myBoolAttrs[SUMO_TAG_EDGE].insert(GNE_ATTR_BIDIR);
        // bus stop
        myBoolAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_FRIENDLY_POS);
        // access
        myBoolAttrs[SUMO_TAG_ACCESS].insert(SUMO_ATTR_FRIENDLY_POS);
        // container stop
        myBoolAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_FRIENDLY_POS);
        // charging station
        myBoolAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGEINTRANSIT);
        myBoolAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_FRIENDLY_POS);
        // lane
        myBoolAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_ACCELERATION);
        // connection
        myBoolAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_PASS);
        myBoolAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_UNCONTROLLED);
        myBoolAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_KEEP_CLEAR);
        // crossing
        myBoolAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_PRIORITY);
        // E1
        myBoolAttrs[SUMO_TAG_E1DETECTOR].insert(SUMO_ATTR_FRIENDLY_POS);
        // E2
        myBoolAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_CONT);
        myBoolAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_FRIENDLY_POS);
        // Entry
        myBoolAttrs[SUMO_TAG_DET_ENTRY].insert(SUMO_ATTR_FRIENDLY_POS);
        // Exit
        myBoolAttrs[SUMO_TAG_DET_EXIT].insert(SUMO_ATTR_FRIENDLY_POS);
        // junction
        myBoolAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_KEEP_CLEAR);
        // rerouter
        myBoolAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_OFF);
        // flow
        myBoolAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_REROUTE);
        // Poly
        myBoolAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_FILL);
        myBoolAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_GEO);
        myBoolAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_RELATIVEPATH);
        // POI
        myBoolAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_GEO);
        myBoolAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_RELATIVEPATH);
        // POILane
        myBoolAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_GEO);
        myBoolAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_RELATIVEPATH);
        // Parking Area
        myBoolAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_FRIENDLY_POS);
    }
    return myBoolAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isColor(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myColorAttrs.empty()) {
        // POI
        myColorAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_COLOR);
        // POILane
        myColorAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_COLOR);
        // Poly
        myColorAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_COLOR);
        // Route
        myColorAttrs[SUMO_TAG_ROUTE].insert(SUMO_ATTR_COLOR);
        // Color
        myColorAttrs[SUMO_TAG_VTYPE].insert(SUMO_ATTR_COLOR);
    }
    return myColorAttrs[tag].count(attr) == 1;
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
        // POLY
        myListAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_SHAPE);
        // vClasses
        myListAttrs[SUMO_TAG_STOPOFFSET].insert(SUMO_ATTR_VCLASSES);
        // Exception vClasses
        myListAttrs[SUMO_TAG_STOPOFFSET].insert(SUMO_ATTR_EXCEPTIONS);
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
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_FROM);
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_FROM_LANE);
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TO);
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_TO_LANE);
            myUniqueAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_CUSTOMSHAPE);
            // edge
            myUniqueAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_FROM);
            myUniqueAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_TO);
            // busstop
            myUniqueAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_STARTPOS);
            myUniqueAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_ENDPOS);
            myUniqueAttrs[SUMO_TAG_BUS_STOP].insert(SUMO_ATTR_LANE);
            // access
            myUniqueAttrs[SUMO_TAG_ACCESS].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_ACCESS].insert(SUMO_ATTR_POSITION);
            // container stop
            myUniqueAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_STARTPOS);
            myUniqueAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_ENDPOS);
            myUniqueAttrs[SUMO_TAG_CONTAINER_STOP].insert(SUMO_ATTR_LANE);
            // charging station
            myUniqueAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_STARTPOS);
            myUniqueAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_ENDPOS);
            myUniqueAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_LANE);
            // calibrator (edge)
            myUniqueAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_EDGE);
            // calibrator (lane)
            myUniqueAttrs[SUMO_TAG_LANECALIBRATOR].insert(SUMO_ATTR_LANE);
            // crossing
            myUniqueAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_EDGES);
            myUniqueAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_CUSTOMSHAPE);
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
            myUniqueAttrs[SUMO_TAG_E3DETECTOR].insert(SUMO_ATTR_POSITION);
            // Edge
            myUniqueAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_SHAPE);
            // Junction
            myUniqueAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_POSITION);
            myUniqueAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_SHAPE);
            myUniqueAttrs[SUMO_TAG_JUNCTION].insert(SUMO_ATTR_TLID);
            // Rerouter
            myUniqueAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_EDGES);
            myUniqueAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_FILE);
            myUniqueAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_POSITION);
            // Routeprobe
            myUniqueAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_EDGE);
            myUniqueAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_FILE);
            // Vaporizer
            myUniqueAttrs[SUMO_TAG_VAPORIZER].insert(SUMO_ATTR_EDGE);
            myUniqueAttrs[SUMO_TAG_VAPORIZER].insert(SUMO_ATTR_FILE);
            // VSS
            myUniqueAttrs[SUMO_TAG_VSS].insert(SUMO_ATTR_POSITION);
            myUniqueAttrs[SUMO_TAG_VSS].insert(SUMO_ATTR_FILE);
            // POI
            myUniqueAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_POSITION);
            myUniqueAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_GEOPOSITION);
            // POILane
            myUniqueAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_LANE);
            myUniqueAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_POSITION);
            // POLY
            myUniqueAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_SHAPE);
            myUniqueAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_GEOSHAPE);
            // Parking Area
            myUniqueAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_STARTPOS);
            myUniqueAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_ENDPOS);
            myUniqueAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_LANE);
            // Parking Space
            myUniqueAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_X);
            myUniqueAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_Y);
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
        // crossing
        myPositiveAttrs[SUMO_TAG_CROSSING].insert(SUMO_ATTR_PRIORITY);
        // connection
        myPositiveAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_CONTPOS);
        myPositiveAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_VISIBILITY_DISTANCE);
        myPositiveAttrs[SUMO_TAG_CONNECTION].insert(SUMO_ATTR_SPEED);
        // charging station
        myPositiveAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGINGPOWER);
        myPositiveAttrs[SUMO_TAG_CHARGING_STATION].insert(SUMO_ATTR_CHARGEINTRANSIT);
        // E2
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_LENGTH);
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);
        myPositiveAttrs[SUMO_TAG_E2DETECTOR].insert(SUMO_ATTR_JAM_DIST_THRESHOLD);
        // calibrator (edge)
        myPositiveAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_POSITION);
        // calibrator (lane)
        myPositiveAttrs[SUMO_TAG_LANECALIBRATOR].insert(SUMO_ATTR_POSITION);
        // flow
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_PERSON_NUMBER);
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_CONTAINER_NUMBER);
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_VEHSPERHOUR);
        myPositiveAttrs[SUMO_TAG_FLOW].insert(SUMO_ATTR_SPEED);
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
        // POI
        myPositiveAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_WIDTH);
        myPositiveAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_HEIGHT);
        // POILane
        myPositiveAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_POSITION);
        // offset value
        myPositiveAttrs[SUMO_TAG_STOPOFFSET].insert(SUMO_ATTR_VALUE);
        // Parking Area
        myPositiveAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_ROADSIDE_CAPACITY);
        myPositiveAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_WIDTH);
        myPositiveAttrs[SUMO_TAG_PARKING_AREA].insert(SUMO_ATTR_LENGTH);
        // Parking Space
        myPositiveAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_WIDTH);
        myPositiveAttrs[SUMO_TAG_PARKING_SPACE].insert(SUMO_ATTR_LENGTH);
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
        // destiny probability reroute
        myProbabilityAttrs[SUMO_TAG_DEST_PROB_REROUTE].insert(SUMO_ATTR_PROB);
        // destiny probability reroute
        myProbabilityAttrs[SUMO_TAG_PARKING_ZONE_REROUTE].insert(SUMO_ATTR_PROB);
        // route prob reroute
        myProbabilityAttrs[SUMO_TAG_ROUTE_PROB_REROUTE].insert(SUMO_ATTR_PROB);
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
        // calibrator (edge)
        myFileAttrs[SUMO_TAG_CALIBRATOR].insert(SUMO_ATTR_OUTPUT);
        // calibrator (lane)
        myFileAttrs[SUMO_TAG_LANECALIBRATOR].insert(SUMO_ATTR_OUTPUT);
        // rerouter
        myFileAttrs[SUMO_TAG_REROUTER].insert(SUMO_ATTR_FILE);
        // routeprobe
        myFileAttrs[SUMO_TAG_ROUTEPROBE].insert(SUMO_ATTR_FILE);
        // Variable Speed Signal
        myFileAttrs[SUMO_TAG_VSS].insert(SUMO_ATTR_FILE);
        // POI
        myFileAttrs[SUMO_TAG_POI].insert(SUMO_ATTR_IMGFILE);
        // POILane
        myFileAttrs[SUMO_TAG_POILANE].insert(SUMO_ATTR_IMGFILE);
        // POLY
        myFileAttrs[SUMO_TAG_POLY].insert(SUMO_ATTR_IMGFILE);
    }
    return myFileAttrs[tag].count(attr) == 1;
}


bool
GNEAttributeCarrier::isSVCPermissions(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (mySVCPermissionsAttrs.empty()) {
        // Edge
		mySVCPermissionsAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_ALLOW);
		mySVCPermissionsAttrs[SUMO_TAG_EDGE].insert(SUMO_ATTR_DISALLOW);
        // Lane
		mySVCPermissionsAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_ALLOW);
		mySVCPermissionsAttrs[SUMO_TAG_LANE].insert(SUMO_ATTR_DISALLOW);
        // Closing Reroute
		mySVCPermissionsAttrs[SUMO_TAG_CLOSING_REROUTE].insert(SUMO_ATTR_ALLOW);
		mySVCPermissionsAttrs[SUMO_TAG_CLOSING_REROUTE].insert(SUMO_ATTR_DISALLOW);
        // Closing Lane Reroute
		mySVCPermissionsAttrs[SUMO_TAG_CLOSING_LANE_REROUTE].insert(SUMO_ATTR_ALLOW);
		mySVCPermissionsAttrs[SUMO_TAG_CLOSING_LANE_REROUTE].insert(SUMO_ATTR_DISALLOW);
    }
    return mySVCPermissionsAttrs[tag].count(attr) == 1;
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
            return (i.second.defaultValue != NODEFAULTVALUE);
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


SumoXMLTag 
GNEAttributeCarrier::getAdditionalParentTag(SumoXMLTag tag) {
    if(canHaveParent(tag)) {
        return myAdditionalsWithParent[tag];
    } else {
        return SUMO_TAG_NOTHING;
    }
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
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<int>(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> double
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<double>(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> bool
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<bool>(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::string
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return (i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> Position
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<Position>(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> SUMOVehicleClass
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<SUMOVehicleClass>(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> SUMOVehicleShape
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<SUMOVehicleShape>(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> RGBColor
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<RGBColor>(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<int>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<std::vector<int> >(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<double>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<std::vector<double> >(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<bool>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<std::vector<bool> >(i.second.defaultValue);
        }
    }
    // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
    throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
}


template<> std::vector<std::string>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    for (auto i : allowedAttributes(tag)) {
        if ((i.first == attr) && (i.second.defaultValue != NODEFAULTVALUE)) {
            return parse<std::vector<std::string> >(i.second.defaultValue);
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


std::pair<std::string, std::string> 
GNEAttributeCarrier::setAttrDefinition(const std::string &definition, const std::string &restriction) {
    return std::pair<std::string, std::string>(definition, restriction);
}

/****************************************************************************/

