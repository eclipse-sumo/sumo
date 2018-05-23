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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>

#include "GNEAttributeCarrier.h"
#include "GNEUndoList.h"
#include "GNENet.h"


// ===========================================================================
// static members
// ===========================================================================

std::map<SumoXMLTag, std::pair<GNEAttributeCarrier::TagValues, std::map<SumoXMLAttr, GNEAttributeCarrier::AttributeValues> > > GNEAttributeCarrier::myAllowedAttributes;

const std::string GNEAttributeCarrier::FEATURE_LOADED = "loaded";
const std::string GNEAttributeCarrier::FEATURE_GUESSED = "guessed";
const std::string GNEAttributeCarrier::FEATURE_MODIFIED = "modified";
const std::string GNEAttributeCarrier::FEATURE_APPROVED = "approved";
const double GNEAttributeCarrier::INVALID_POSITION = -1000000;

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
GNEAttributeCarrier::TagValues::isInternal() const {
    return (myTagProperty & TAGPROPERTY_INTERNAL) != 0;
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


GNEAttributeCarrier::AttributeValues::AttributeValues(int attributeProperty, const std::string &definition, const std::string &defaultValue, std::vector<std::string> discreteValues) :
    myAttributeProperty(attributeProperty),
    myDefinition(definition),
    myDefaultValue(defaultValue),
    myDiscreteValues(discreteValues) {}


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
GNEAttributeCarrier::AttributeValues::getDescription() const {
    std::string pre;
    std::string type;
    std::string plural;
    std::string last;
    // pre type
    if((myAttributeProperty & ATTRPROPERTY_LIST) != 0) {
        pre += "list of ";
        if((myAttributeProperty & ATTRPROPERTY_VCLASS) != 0) {
            plural = "es";
        } else {
            plural = "s";
        }
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
    if((myAttributeProperty & ATTRPROPERTY_UNIQUE) != 0) {
        pre += "unique ";
    }
    if((myAttributeProperty & ATTRPROPERTY_COMBINABLE) != 0) {
        pre += "combinable ";
    }
    // type
    if((myAttributeProperty & ATTRPROPERTY_INT) != 0) {
        type = "integer";
    }
    if((myAttributeProperty & ATTRPROPERTY_FLOAT) != 0) {
        type = "float";
    }
    if((myAttributeProperty & ATTRPROPERTY_BOOL) != 0) {
        type = "boolean";
    }
    if((myAttributeProperty & ATTRPROPERTY_STRING) != 0) {
        type = "string";
    }
    if((myAttributeProperty & ATTRPROPERTY_POSITION) != 0) {
        type = "position";
    }
    if((myAttributeProperty & ATTRPROPERTY_COLOR) != 0) {
        type = "color";
    }
    if((myAttributeProperty & ATTRPROPERTY_VCLASS) != 0) {
        type = "VClass";
    }
    if((myAttributeProperty & ATTRPROPERTY_FILENAME) != 0) {
        type = "filename";
    }
    if((myAttributeProperty & ATTRPROPERTY_PROBABILITY) != 0) {
        type = "probability";
        last = "[0, 1]";
    }
    if((myAttributeProperty & ATTRPROPERTY_TIME) != 0) {
        type = "time";
    }
    if((myAttributeProperty & ATTRPROPERTY_ANGLE) != 0) {
        type = "angle";
        last = "[0, 360]";
    }
    return pre + type + plural + last;
}


const std::vector<std::string> &
GNEAttributeCarrier::AttributeValues::getDiscreteValues() const {
    return myDiscreteValues;
}


bool 
GNEAttributeCarrier::AttributeValues::hasDefaultValue() const {
    return (myAttributeProperty & ATTRPROPERTY_DEFAULTVALUE) != 0;
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
    return (myAttributeProperty & ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST) != 0;
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


bool 
GNEAttributeCarrier::AttributeValues::isDiscrete() const {
    return (myAttributeProperty & ATTRPROPERTY_DISCRETE) != 0;
}


bool 
GNEAttributeCarrier::AttributeValues::isCombinable() const {
    return (myAttributeProperty & ATTRPROPERTY_COMBINABLE) != 0;
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
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // iterate over attributes and save it in attr vector
    for (auto i : myAllowedAttributes.at(myTag).second) {
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
GNEAttributeCarrier::getAttributes(SumoXMLTag tag) {
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // check that tag is defined
    if(myAllowedAttributes.count(tag) == 0) {
        throw ProcessError("Attributes for tag '" + toString(tag) + "' not defined");
    } else {
        return myAllowedAttributes.at(tag).second;
    }
}

const GNEAttributeCarrier::TagValues & 
GNEAttributeCarrier::getTagProperties(SumoXMLTag tag) {
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // check that tag is defined
    if(myAllowedAttributes.count(tag) == 0) {
        throw ProcessError("Attributes for tag '" + toString(tag) + "' not defined");
    } else {
        return myAllowedAttributes.at(tag).first;
    }
}


const GNEAttributeCarrier::AttributeValues &
GNEAttributeCarrier::getAttributeProperties(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // check that tag is defined
    if(myAllowedAttributes.count(tag) == 0) {
        throw ProcessError("Attributes for tag '" + toString(tag) + "' not defined");
    } else if(myAllowedAttributes.at(tag).second.count(attr) == 0) {
        throw ProcessError("Attributes for tag '" + toString(tag) + "' and attribute '" + toString(attr) + "' not defined");
    } else {
        return myAllowedAttributes.at(tag).second.at(attr);
    }
}


std::vector<SumoXMLTag>
GNEAttributeCarrier::allowedTags(bool includingInternals) {
    std::vector<SumoXMLTag> tags;
    // append all net elements to tags
    tags.insert(std::end(tags), std::begin(allowedNetElementsTags(includingInternals)), std::end(allowedNetElementsTags(includingInternals)));
    tags.insert(std::end(tags), std::begin(allowedAdditionalTags(includingInternals)), std::end(allowedAdditionalTags(includingInternals)));
    tags.insert(std::end(tags), std::begin(allowedShapeTags(includingInternals)), std::end(allowedShapeTags(includingInternals)));
    return tags;
}


std::vector<SumoXMLTag>
GNEAttributeCarrier::allowedNetElementsTags(bool includingInternals) {
    std::vector<SumoXMLTag> netElementTags;
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // fill additional tags
    for (auto i : myAllowedAttributes) {
        if (i.second.first.isNetElement() && (includingInternals || !i.second.first.isInternal())) {
            netElementTags.push_back(i.first);
        }
    }
    return netElementTags;
}


std::vector<SumoXMLTag>
GNEAttributeCarrier::allowedAdditionalTags(bool includingInternals) {
    std::vector<SumoXMLTag> additionalTags;
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // fill additional tags
    for (auto i : myAllowedAttributes) {
        if (i.second.first.isAdditional() && (includingInternals || !i.second.first.isInternal())) {
            additionalTags.push_back(i.first);
        }
    }
    return additionalTags;
}


std::vector<SumoXMLTag>
GNEAttributeCarrier::allowedShapeTags(bool includingInternals) {
    std::vector<SumoXMLTag> shapeTags;
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // fill shape tags
    for (auto i : myAllowedAttributes) {
        if (i.second.first.isShape() && (includingInternals || !i.second.first.isInternal())) {
            shapeTags.push_back(i.first);
        }
    }
    return shapeTags;
}


bool
GNEAttributeCarrier::hasAttribute(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    return (myAllowedAttributes.count(tag) != 0) && (myAllowedAttributes.at(tag).second.count(attr) != 0);
}


int
GNEAttributeCarrier::getHigherNumberOfAttributes() {
    int maxNumAttribute = 0;
    // define on first access
    if (myAllowedAttributes.size() == 0) {
        fillAttributeCarriers();
    }
    // get max num attributes
    for (auto i : myAllowedAttributes) {
        maxNumAttribute = MAX2(maxNumAttribute, (int)i.second.second.size());
    }
    return maxNumAttribute;
}


template<> int
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<int>(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> double
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<double>(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> bool
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<bool>(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> std::string
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return attrValue.getDefaultValue();
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> Position
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<Position>(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> SUMOVehicleClass
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<SUMOVehicleClass>(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> SUMOVehicleShape
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<SUMOVehicleShape>(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> RGBColor
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<RGBColor>(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> std::vector<int>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<std::vector<int> >(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> std::vector<double>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<std::vector<double> >(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> std::vector<bool>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<std::vector<bool> >(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
}


template<> std::vector<std::string>
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {
    const AttributeValues & attrValue = getAttributeProperties(tag, attr);
    if(attrValue.hasDefaultValue()) {
        return parse<std::vector<std::string> >(attrValue.getDefaultValue());
    } else {
        // throw exception if attribute doesn't have a default value and return a empty value to avoid warnings
        throw ProcessError("attribute '" + toString(attr) + "' for tag '" + toString(tag) + "' doesn't have a default value");
    }
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
    } else if(getAttributeProperties(getTag(), key).hasDefaultValue()) {
        // only write optional attributes (i.e attributes with default value) if are differents
        if(getDefaultValue<std::string>(getTag(), key) != attribute) {
            device.writeAttr(key, attribute);
        }
    } else {
        device.writeAttr(key, attribute);
    }
}


void 
GNEAttributeCarrier::fillAttributeCarriers() {
    // obtain Node Types except NODETYPE_DEAD_END_DEPRECATED
    std::vector<std::string> nodeTypes = SUMOXMLDefinitions::NodeTypes.getStrings();
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(NODETYPE_DEAD_END_DEPRECATED)));
    // fill all ACs
    SumoXMLTag currentTag = SUMO_TAG_EDGE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_NETELEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_UNIQUE, 
            "The id of the edge", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FROM] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The name of a node within the nodes-file the edge shall start at", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TO] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The name of a node within the nodes-file the edge shall end at", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPEED] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The maximum speed allowed on the edge in m/s", 
            "13.89");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PRIORITY] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The priority of the edge", 
            "1");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_NUMLANES] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The number of lanes of the edge", 
            "1");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The name of a type within the SUMO edge type file", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "Explicitly allows the given vehicle classes (not given will be not allowed)", 
            "all",
            SumoVehicleClassStrings.getStrings());
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "Explicitly disallows the given vehicle classes (not given will be allowed)", 
            "",
            SumoVehicleClassStrings.getStrings());
        //myAllowedAttributes[currentTag].second[SUMO_ATTR_PREFER, );
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SHAPE] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "If the shape is given it should start and end with the positions of the from-node and to-node", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LENGTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
            "The length of the edge in meter", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPREADTYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE, 
            "Lane width for all lanes of this edge in meters (used for visualization)", 
            "right",
            SUMOXMLDefinitions::LaneSpreadFunctions.getStrings());
        myAllowedAttributes[currentTag].second[SUMO_ATTR_NAME] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "street name (need not be unique, used for visualization)", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Lane width for all lanes of this edge in meters (used for visualization)", 
            "default");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ENDOFFSET] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Move the stop line back from the intersection by the given amount", 
            "0");
        myAllowedAttributes[currentTag].second[GNE_ATTR_SHAPE_START] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_DEFAULTVALUE, // virtual attribute used to define an endPoint
            "Custom position in which shape start (by default position of junction from)", 
            "");
        myAllowedAttributes[currentTag].second[GNE_ATTR_SHAPE_END] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_DEFAULTVALUE, // virtual attribute from to define an endPoint
            "Custom position in which shape end (by default position of junction from)", 
            "");
        myAllowedAttributes[currentTag].second[GNE_ATTR_BIDIR] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, // virtual attribute to check of this edge is part of a bidirectional railway
            "Enable or disable bidireccional edge", 
            "false");
    }
    currentTag = SUMO_TAG_JUNCTION;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_NETELEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the node", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_POSITION, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
            "The x-y-z position of the node on the plane in meters", 
            ""); 
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE, 
            "An optional type for the node", 
            "",
            nodeTypes);
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SHAPE] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "A custom shape for that node", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_RADIUS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
            "Optional turning radius (for all corners) for that node in meters", 
            "1.5");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_KEEP_CLEAR] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Whether the junction-blocking-heuristic should be activated at this node", 
            "1");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TLTYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE, 
            "An optional type for the traffic light algorithm", 
            "",
            SUMOXMLDefinitions::TrafficLightTypes.getStrings());
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TLID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "An optional id for the traffic light program", 
            "");
    }
    currentTag = SUMO_TAG_LANE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_NETELEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "ID of lane (Automatic)", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPEED] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Speed in meters per second", 
            "13.89");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "Explicitly allows the given vehicle classes (not given will be not allowed)", 
            "all",
            SumoVehicleClassStrings.getStrings());
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "Explicitly disallows the given vehicle classes (not given will be allowed)", 
            "",
            SumoVehicleClassStrings.getStrings());
        //myAllowedAttributes[currentTag].second[SUMO_ATTR_PREFER, );
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Width in meters (used for visualization)", 
            "default");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ENDOFFSET] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Move the stop line back from the intersection by the given amount", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ACCELERATION] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Enable or disable lane as acceleration lane", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CUSTOMSHAPE] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE, 
            "If the shape is given it overrides the computation based on edge shape", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_INDEX] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_NONEDITABLE,
            "The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one)", 
            "");
    }
    currentTag = SUMO_TAG_POI;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_SHAPE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_GEOPOSITION);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the POI", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
            "The position in view", 
            ""); 
        myAllowedAttributes[currentTag].second[SUMO_ATTR_COLOR] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE, 
            "The color with which the poi shall be displayed", 
            "red");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "A typename for the poi", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LAYER] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE, 
            "The layer of the poi for drawing and selecting", 
            "4"); // needed to draw it over lane
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Width of rendered image in meters", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_HEIGHT] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Height of rendered image in meters", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "A bitmap to use for rendering this poi", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_RELATIVEPATH] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Enable or disbale use image file as a relative path", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ANGLE] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "Angle of rendered image in degree", 
            "0");
    }
    currentTag = SUMO_TAG_POILANE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_SHAPE | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the POI", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The name of the lane the poi is located at); the lane must be a part of the loaded network", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The position on the named lane or in the net in meters at which the poi is located at", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION_LAT] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "The lateral offset on the named lane at which the poi is located at", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_COLOR] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE, 
            "The color with which the poi shall be displayed", 
            "red");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "A typename for the poi", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LAYER] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE, 
            "The layer of the poi for drawing and selecting", 
            "4"); // needed to draw it over lane
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Width of rendered image in meters", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_HEIGHT] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Height of rendered image in meters", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "A bitmap to use for rendering this poi", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_RELATIVEPATH] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Enable or disbale use image file as a relative path", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ANGLE] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "Angle of rendered image in degree", 
            "0");
    }
    currentTag = SUMO_TAG_POLY;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_SHAPE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_BLOCKSHAPE | TAGPROPERTY_CLOSESHAPE | TAGPROPERTY_GEOSHAPE);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "The id of the polygon", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SHAPE] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
            "The shape of the polygon", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_COLOR] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE, 
            "The RGBA color with which the polygon shall be displayed", 
            "green");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FILL] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "An information whether the polygon shall be filled", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LAYER] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE, 
            "The layer in which the polygon lies", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "A typename for the polygon", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "A bitmap to use for rendering this polygon", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_RELATIVEPATH] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Enable or disbale use image file as a relative path", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ANGLE] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "Angle of rendered image in degree", 
            "0");
    }
    currentTag = SUMO_TAG_CROSSING;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_NETELEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE, 
            "The ID of Crossing", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EDGES] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
            "The (road) edges which are crossed", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PRIORITY] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections)", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The width of the crossings", 
            toString(OptionsCont::getOptions().getFloat("default.crossing-width")));
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TLLINKINDEX] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE, 
            "sets the tls-index for this crossing", 
            "-1");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TLLINKINDEX2] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE, 
            "sets the opposite-direction tls-index for this crossing", 
            "-1");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CUSTOMSHAPE] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "Overrids default shape of pedestrian crossing", 
            "");
    }
    currentTag = SUMO_TAG_CONNECTION;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_NETELEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FROM] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE, 
            "The name of the edge the vehicles leave", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TO] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE, 
            "The name of the edge the vehicles may reach when leaving 'from'", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FROM_LANE] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE, 
            "the lane index of the incoming lane (numbers starting with 0)", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TO_LANE] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE, 
            "the lane index of the outgoing lane (numbers starting with 0)", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PASS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "if set, vehicles which pass this (lane-2-lane) connection) will not wait", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_KEEP_CLEAR] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CONTPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "If set to a more than 0 value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection", 
            toString(NBEdge::UNSPECIFIED_CONTPOS));
        myAllowedAttributes[currentTag].second[SUMO_ATTR_UNCONTROLLED] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set to true, This connection will not be TLS-controlled despite its node being controlled", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_VISIBILITY_DISTANCE] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "", 
            toString(NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE));
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TLLINKINDEX] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE, 
            "sets the distance to the connection at which all relevant foes are visible", 
            "-1");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPEED] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "sets custom speed limit for the connection", 
            toString(NBEdge::UNSPECIFIED_SPEED));
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CUSTOMSHAPE] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "sets custom shape for the connection", 
            "");
    }
    currentTag = SUMO_TAG_BUS_STOP;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of bus stop", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The name of the lane the bus stop shall be located at", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The begin position on the lane (the lower position on the lane) in meters", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_NAME] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "Name of bus stop", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LINES] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE, 
            "Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes", 
            "");
    }
    currentTag = SUMO_TAG_ACCESS;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The name of the lane the stop access shall be located at", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The position on the lane (the lower position on the lane) in meters", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
    }
    currentTag = SUMO_TAG_CONTAINER_STOP;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of container stop", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The name of the lane the container stop shall be located at", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The begin position on the lane (the lower position on the lane) in meters", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_NAME] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "Name of Container Stop", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LINES] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE, 
            "meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes", 
            "");
    }
    currentTag = SUMO_TAG_CHARGING_STATION;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of charging station", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "Lane of the charging station location", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "Begin position in the specified lane", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "End position in the specified lane", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_NAME] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "Name of Charging Station", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CHARGINGPOWER] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Charging power in W", 
            "22000.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EFFICIENCY] = AttributeValues(
            ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Charging efficiency [0,1]", 
            "0.95");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CHARGEINTRANSIT] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CHARGEDELAY] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins", 
            "0.00");
    }
    currentTag = SUMO_TAG_E1DETECTOR;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of E1", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The aggregation period the values the detector collects shall be summed up", 
            "100.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "The path to the output file", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_VTYPES] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "Space separated list of vehicle type ids to consider", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
    }
    currentTag = SUMO_TAG_E2DETECTOR;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of E2", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The position on the lane the detector shall be laid on in meters", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LENGTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The length of the detector in meters", 
            "10.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The aggregation period the values the detector collects shall be summed up", 
            "100.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "The path to the output file", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CONT] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Holds the information whether detectors longer than a lane shall be cut off or continued (set it to true for the second case))", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_HALTING_TIME_THRESHOLD] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)", 
            "1.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_HALTING_SPEED_THRESHOLD] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s", 
            "1.39");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_JAM_DIST_THRESHOLD] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m", 
            "10.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
    }
    currentTag = SUMO_TAG_E3DETECTOR;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of E3", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "X-Y position of detector in editor (Only used in NETEDIT)", 
            "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The aggregation period the values the detector collects shall be summed up", 
            "100.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "The path to the output file", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_HALTING_TIME_THRESHOLD] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) in s", 
            "1.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_HALTING_SPEED_THRESHOLD] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s", 
            "1.39");
    }
    currentTag = SUMO_TAG_DET_ENTRY;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, SUMO_TAG_E3DETECTOR);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The position on the lane the detector shall be laid on in meters", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
    }
    currentTag = SUMO_TAG_DET_EXIT;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, SUMO_TAG_E3DETECTOR);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The position on the lane the detector shall be laid on in meters", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
    }
    currentTag = SUMO_TAG_VSS;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_DIALOG);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "The id of Variable Speed Signal", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "X-Y position of detector in editor (Only used in NETEDIT)", 
            "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANES] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE, 
            "list of lanes of Variable Speed Sign", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "The path to the output file", 
            "");
    }
    currentTag = SUMO_TAG_CALIBRATOR;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_DIALOG);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of Calibrator", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EDGE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of edge in the simulation network", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The position of the calibrator on the specified lane", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The aggregation interval in which to calibrate the flows. default is step-length", 
            "1.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ROUTEPROBE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The id of the routeProbe element from which to determine the route distribution for generated vehicles", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_OUTPUT] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "The output file for writing calibrator information or NULL", 
            "");
    }
    currentTag = SUMO_TAG_LANECALIBRATOR;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_DIALOG);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of Calibrator", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of lane in the simulation network", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The position of the calibrator on the specified lane", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The aggregation interval in which to calibrate the flows. default is step-length", 
            "100.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ROUTEPROBE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The id of the routeProbe element from which to determine the route distribution for generated vehicles", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_OUTPUT] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "The output file for writing calibrator information or NULL", 
            "");
    }
    currentTag = SUMO_TAG_REROUTER;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_DIALOG);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of Rerouter", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EDGES] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE, 
            "An edge id or a list of edge ids where vehicles shall be rerouted", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_POSITION] = AttributeValues(
            ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE, 
            "X,Y position in editor (Only used in NETEDIT)", 
            "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "The path to the definition file (alternatively, the intervals may defined as children of the rerouter)", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PROB] = AttributeValues(
            ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The probability for vehicle rerouting (0-1)", 
            "1.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_HALTING_TIME_THRESHOLD] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The waiting time threshold (in s) that must be reached to activate rerouting (default -1 which disables the threshold)", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_OFF] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Whether the router should be inactive initially (and switched on in the gui)", 
            "0");
    }
    currentTag = SUMO_TAG_ROUTEPROBE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of RouteProbe", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EDGE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of an edge in the simulation network", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FREQUENCY] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL, 
            "The frequency in which to report the distribution", 
            OPTIONALATTRIBUTE);                
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "The file for generated output", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_BEGIN] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The time at which to start generating output", 
            "0");
    }
    currentTag = SUMO_TAG_VAPORIZER;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EDGE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "Edge in which vaporizer is placed", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_STARTTIME] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "Start Time", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_END] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "End Time", 
            "10.00");
    }
    currentTag = SUMO_TAG_PARKING_AREA;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_BLOCKMOVEMENT);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of ParkingArea", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The name of the lane the Parking Area shall be located at", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_STARTPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The begin position on the lane (the lower position on the lane) in meters", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ENDPOS] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ROADSIDE_CAPACITY] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            " The number of parking spaces for road-side parking ", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_FRIENDLY_POS] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_NAME] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "Name of Parking Area", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The width of the road-side parking spaces", 
            "3.2");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LENGTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The length of the road-side parking spaces", 
            "5.0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ANGLE] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise", 
            "0.00");
    }
    currentTag = SUMO_TAG_PARKING_SPACE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, SUMO_TAG_PARKING_AREA);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_X] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The X position in meters of the parking vehicle", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_Y] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE, 
            "The Y position in meters of the parking vehicle", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_Z] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "The Z position in meters of the parking vehicle", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The width of the road-side parking spaces", 
            "3.2");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LENGTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The length of the road-side parking spaces", 
            "5.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ANGLE] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise", 
            "0");
    }
    currentTag = SUMO_TAG_FLOW;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of the vehicle type to use for this vehicle", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TYPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The id of the vehicle type to use for this vehicle", 
            DEFAULT_VTYPE_ID);
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ROUTE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The id of the route the vehicle shall drive along", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_VEHSPERHOUR] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
            "Number of vehicles per hour, equally spaced", 
            OPTIONALATTRIBUTE);
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPEED] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE, 
            "Speed of vehicles", 
            OPTIONALATTRIBUTE);
        myAllowedAttributes[currentTag].second[SUMO_ATTR_COLOR] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE, 
            "This vehicle's color", 
            "yellow");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_BEGIN] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "First vehicle departure time",  
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_END] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "End of departure interval", 
            "100.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DEPARTLANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The lane on which the vehicle shall be inserted", 
            "first");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DEPARTPOS] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The position at which the vehicle shall enter the net", 
            "base");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DEPARTSPEED] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The speed with which the vehicle shall enter the network", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ARRIVALLANE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The lane at which the vehicle shall leave the network", 
            "current");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ARRIVALPOS] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The position at which the vehicle shall leave the network", 
            "max");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ARRIVALSPEED] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The speed with which the vehicle shall leave the network", 
            "current");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LINE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "A string specifying the id of a public transport line which can be used when specifying person rides", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PERSON_NUMBER] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The number of occupied seats when the vehicle is inserted", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CONTAINER_NUMBER] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The number of occupied container places when the vehicle is inserted",  
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_REROUTE] = AttributeValues(
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE, 
            "Whether the vehicle should be equipped with a rerouting device", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DEPARTPOS_LAT] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The lateral position on the departure lane at which the vehicle shall enter the net", 
            "center");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ARRIVALPOS_LAT] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The lateral position on the arrival lane at which the vehicle shall arrive", 
            "");
    }
    currentTag = SUMO_TAG_ROUTE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of Route", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EDGES] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE, 
            "The edges the vehicle shall drive along, given as their ids, separated using spaces", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_COLOR] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE, 
            "This route's color", 
            "yellow");
    }
    currentTag = SUMO_TAG_VTYPE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "The id of VehicleType", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ACCEL] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The acceleration ability of vehicles of this type [m/s^2]", 
            "2.60");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DECEL] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The deceleration ability of vehicles of this type [m/s^2]", 
            "4.50");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SIGMA] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Car-following model parameter", 
            "0.50");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TAU] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Car-following model parameter", 
            "1.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LENGTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The vehicle's netto-length (length) [m]", 
            "5.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_MINGAP] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Empty space after leader [m]", 
            "2.50");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_MAXSPEED] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The vehicle's maximum velocity [m/s]", 
            "70.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPEEDFACTOR] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The vehicles expected multiplicator for lane speed limits", 
            "1.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPEEDDEV] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The deviation of the speedFactor", 
            "0.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_COLOR] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE, 
            "This vehicle type's color", 
            "1,1,0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_VCLASS] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE, 
            "An abstract vehicle class", 
            "passenger",
            SumoVehicleShapeStrings.getStrings());
        myAllowedAttributes[currentTag].second[SUMO_ATTR_EMISSIONCLASS] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "An abstract emission class", 
            "P_7_7");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_GUISHAPE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE, 
            "How this vehicle is rendered", 
            "passenger",
            SumoVehicleShapeStrings.getStrings());
        myAllowedAttributes[currentTag].second[SUMO_ATTR_WIDTH] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The vehicle's width [m] (only used for drawing)", 
            "2.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_IMGFILE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE, 
            "Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_IMPATIENCE] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "Willingess of drivers to impede vehicles with higher priority", 
            "0.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LANE_CHANGE_MODEL] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The model used for changing lanes", 
            "LC2013");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CAR_FOLLOW_MODEL] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE, 
            "The model used for car following", 
            "Krauss");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PERSON_CAPACITY] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE, 
            "The number of persons (excluding an autonomous driver) the vehicle can transport", 
            "4");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_CONTAINER_CAPACITY] = AttributeValues(
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The number of containers the vehicle can transport", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_BOARDING_DURATION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "The time required by a person to board the vehicle", 
            "0.50");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LOADING_DURATION] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME| ATTRPROPERTY_DEFAULTVALUE, 
            "The time required to load a container onto the vehicle", 
            "90.00");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_LATALIGNMENT] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE, 
            "The preferred lateral alignment when using the sublane-model", 
            "center");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_MINGAP_LAT] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The minimum lateral gap at a speed difference of 50km/h when using the sublane-model", 
            "0.12");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_MAXSPEED_LAT] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "The maximum lateral speed when using the sublane-model", 
            "1.00");
    }
    currentTag = SUMO_TAG_STEP;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_TIME] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME, 
            "Time", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_SPEED] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "Speed", 
            "50.00");
    }
    currentTag = SUMO_TAG_INTERVAL;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_BEGIN] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "Begin", 
            "0");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_END] = AttributeValues(
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE, 
            "End", 
            "100.00");
    }
    currentTag = SUMO_TAG_CLOSING_REROUTE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "Edge ID", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "allowed vehicles", 
            "all");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "disallowed vehicles", 
            "");
    }
    currentTag = SUMO_TAG_CLOSING_LANE_REROUTE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "Lane ID", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "allowed vehicles", 
            "all");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_DISALLOW] = AttributeValues(
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE, 
            "disallowed vehicles", 
            "");
    }
    currentTag = SUMO_TAG_DEST_PROB_REROUTE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "Edge ID", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PROB] = AttributeValues(
            ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "probability", 
            "1.00");
    }
    currentTag = SUMO_TAG_PARKING_ZONE_REROUTE;
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "ParkingArea ID", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PROB] = AttributeValues(
            ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "probability", 
            "1.00");
    }
    currentTag = SUMO_TAG_ROUTE_PROB_REROUTE; 
    {
        // set values of tag
        myAllowedAttributes[currentTag].first = TagValues(TAGPROPERTY_ADDITIONAL | TAGPROPERTY_INTERNAL);
        // set values of attributes
        myAllowedAttributes[currentTag].second[SUMO_ATTR_ID] = AttributeValues(
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE, 
            "Route", 
            "");
        myAllowedAttributes[currentTag].second[SUMO_ATTR_PROB] = AttributeValues(
            ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE, 
            "probability", 
            "1.00");
    }
}

/****************************************************************************/

