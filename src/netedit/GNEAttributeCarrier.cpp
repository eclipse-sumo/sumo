/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/options/OptionsCont.h>

#include "GNEAttributeCarrier.h"
#include "GNENet.h"


// ===========================================================================
// static members
// ===========================================================================

std::map<SumoXMLTag, GNEAttributeCarrier::TagProperties> GNEAttributeCarrier::myTagProperties;
GNEAttributeCarrier::TagProperties GNEAttributeCarrier::dummyTagProperty;

const std::string GNEAttributeCarrier::FEATURE_LOADED = "loaded";
const std::string GNEAttributeCarrier::FEATURE_GUESSED = "guessed";
const std::string GNEAttributeCarrier::FEATURE_MODIFIED = "modified";
const std::string GNEAttributeCarrier::FEATURE_APPROVED = "approved";
const double GNEAttributeCarrier::INVALID_POSITION(-1000000);


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAttributeCarrier::AttributeProperties - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrier::AttributeProperties::AttributeProperties() :
    myAttribute(SUMO_ATTR_NOTHING),
    myAttributeProperty(ATTRPROPERTY_STRING),
    myPositionListed(0),
    myDefinition(""),
    myDefaultValue(""),
    myAttrSynonym(SUMO_ATTR_NOTHING),
    myMinimumRange(0),
    myMaximumRange(0) {}


GNEAttributeCarrier::AttributeProperties::AttributeProperties(SumoXMLAttr attribute, int attributeProperty, const std::string& definition, std::string defaultValue) :
    myAttribute(attribute),
    myAttributeProperty(attributeProperty),
    myPositionListed(0),
    myDefinition(definition),
    myDefaultValue(defaultValue),
    myAttrSynonym(SUMO_ATTR_NOTHING),
    myMinimumRange(0),
    myMaximumRange(0) {
    // empty definition aren't valid
    if(definition.empty()) {
        throw FormatException("Every AttributeProperty needs a definition");
    }
    // if default value isn't empty, but attribute doesn't support default values, throw exception.
    if(!defaultValue.empty() && !(attributeProperty & ATTRPROPERTY_DEFAULTVALUE)) {
        throw FormatException("AttributeProperty doesn't support default values");
    }
}


GNEAttributeCarrier::AttributeProperties::~AttributeProperties() {}


void
GNEAttributeCarrier::AttributeProperties::checkAttributeIntegrity() {
    // Check that color attributes always owns an default value
    if (isColor() && myDefaultValue.empty()) {
        throw FormatException("Color attributes must own always a default color");
    }
    // check that secuential attributes correspond to a list
    if (isSecuential() && !isList()) {
        throw FormatException("Secuential property only is compatible with list properties");
    }
    // check that synonym attribute isn't nothing
    if (hasAttrSynonym() && (myAttrSynonym == SUMO_ATTR_NOTHING)) {
        throw FormatException("synonym attribute cannot be nothing");
    }
    // check that ranges are valid
    if (hasAttrRange()) {
        if ((myMinimumRange == 0) && (myMaximumRange == 0)) {
            throw FormatException("non-defined range");
        } else if ((myMaximumRange - myMinimumRange) <= 0) {
            throw FormatException("invalid range");
        }
    }
}


void 
GNEAttributeCarrier::AttributeProperties::setDiscreteValues(const std::vector<std::string>& discreteValues) {
    if (isDiscrete()) {
        myDiscreteValues = discreteValues;
    } else {
        throw FormatException("AttributeProperty doesn't support discrete values values");
    }
}
            

void 
GNEAttributeCarrier::AttributeProperties::setSynonym(SumoXMLAttr synonym) {
    if (hasAttrSynonym()) {
        myAttrSynonym = synonym;
    } else {
        throw FormatException("AttributeProperty doesn't support synonyms");
    }
}


void 
GNEAttributeCarrier::AttributeProperties::setRange(double minimum, double maximum) {
    if (hasAttrRange()) {
        myMinimumRange = minimum;
        myMaximumRange = maximum;
    } else {
        throw FormatException("AttributeProperty doesn't support ranges");
    }
}


void 
GNEAttributeCarrier::AttributeProperties::setPositionListed(int positionListed) {
    myPositionListed = positionListed;
}


SumoXMLAttr 
GNEAttributeCarrier::AttributeProperties::getAttribute() const {
    return myAttribute;
}


int
GNEAttributeCarrier::AttributeProperties::getPositionListed() const {
    return myPositionListed;
}


const std::string&
GNEAttributeCarrier::AttributeProperties::getDefinition() const {
    return myDefinition;
}


const std::string&
GNEAttributeCarrier::AttributeProperties::getDefaultValue() const {
    return myDefaultValue;
}


std::string
GNEAttributeCarrier::AttributeProperties::getDescription() const {
    std::string pre;
    std::string type;
    std::string plural;
    std::string last;
    // pre type
    if ((myAttributeProperty & ATTRPROPERTY_LIST) != 0) {
        pre += "list of ";
        if ((myAttributeProperty & ATTRPROPERTY_VCLASS) != 0) {
            plural = "es";
        } else {
            plural = "s";
        }
    }
    if ((myAttributeProperty & ATTRPROPERTY_POSITIVE) != 0) {
        pre += "positive ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_NONEDITABLE) != 0) {
        pre += "non editable ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_DISCRETE) != 0) {
        pre += "discrete ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_OPTIONAL) != 0) {
        pre += "optional ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_UNIQUE) != 0) {
        pre += "unique ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_COMBINABLE) != 0) {
        pre += "combinable ";
    }
    // type
    if ((myAttributeProperty & ATTRPROPERTY_INT) != 0) {
        type = "integer";
    }
    if ((myAttributeProperty & ATTRPROPERTY_FLOAT) != 0) {
        type = "float";
    }
    if ((myAttributeProperty & ATTRPROPERTY_BOOL) != 0) {
        type = "boolean";
    }
    if ((myAttributeProperty & ATTRPROPERTY_STRING) != 0) {
        type = "string";
    }
    if ((myAttributeProperty & ATTRPROPERTY_POSITION) != 0) {
        type = "position";
    }
    if ((myAttributeProperty & ATTRPROPERTY_COLOR) != 0) {
        type = "color";
    }
    if ((myAttributeProperty & ATTRPROPERTY_VCLASS) != 0) {
        type = "VClass";
    }
    if ((myAttributeProperty & ATTRPROPERTY_FILENAME) != 0) {
        type = "filename";
    }
    if ((myAttributeProperty & ATTRPROPERTY_PROBABILITY) != 0) {
        type = "probability";
        last = "[0, 1]";
    }
    if ((myAttributeProperty & ATTRPROPERTY_TIME) != 0) {
        type = "time";
    }
    if ((myAttributeProperty & ATTRPROPERTY_ANGLE) != 0) {
        type = "angle";
        last = "[0, 360]";
    }
    return pre + type + plural + last;
}


const std::vector<std::string>&
GNEAttributeCarrier::AttributeProperties::getDiscreteValues() const {
    return myDiscreteValues;
}


SumoXMLAttr
GNEAttributeCarrier::AttributeProperties::getAttrSynonym() const {
    if (hasAttrSynonym()) {
        return myAttrSynonym;
    } else {
        throw ProcessError("Attr doesn't support synonym");
    }
}


double
GNEAttributeCarrier::AttributeProperties::getMinimumRange() const {
    if (hasAttrRange()) {
        return myMinimumRange;
    } else {
        throw ProcessError("Attr doesn't support range");
    }
}


double
GNEAttributeCarrier::AttributeProperties::getMaximumRange() const {
    if (hasAttrRange()) {
        return myMaximumRange;
    } else {
        throw ProcessError("Attr doesn't support range");
    }
}


bool
GNEAttributeCarrier::AttributeProperties::hasDefaultValue() const {
    return (myAttributeProperty & ATTRPROPERTY_DEFAULTVALUE) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::hasAttrSynonym() const {
    return (myAttributeProperty & ATTRPROPERTY_SYNONYM) != 0;
}

bool
GNEAttributeCarrier::AttributeProperties::hasAttrRange() const {
    return (myAttributeProperty & ATTRPROPERTY_RANGE) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isInt() const {
    return (myAttributeProperty & ATTRPROPERTY_INT) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isFloat() const {
    return (myAttributeProperty & ATTRPROPERTY_FLOAT) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isBool() const {
    return (myAttributeProperty & ATTRPROPERTY_BOOL) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isString() const {
    return (myAttributeProperty & ATTRPROPERTY_STRING) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isposition() const {
    return (myAttributeProperty & ATTRPROPERTY_POSITION) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isProbability() const {
    return (myAttributeProperty & ATTRPROPERTY_PROBABILITY) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isNumerical() const {
    return (myAttributeProperty & (ATTRPROPERTY_INT | ATTRPROPERTY_FLOAT)) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isTime() const {
    return (myAttributeProperty & ATTRPROPERTY_TIME) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isPositive() const {
    return (myAttributeProperty & ATTRPROPERTY_POSITIVE) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::cannotBeZero() const {
    return (myAttributeProperty & ATTRPROPERTY_NOTZERO) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isColor() const {
    return (myAttributeProperty & ATTRPROPERTY_COLOR) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isFilename() const {
    return (myAttributeProperty & ATTRPROPERTY_FILENAME) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isVClass() const {
    return (myAttributeProperty & ATTRPROPERTY_VCLASS) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isSVCPermission() const {
    return ((myAttributeProperty & ATTRPROPERTY_LIST) != 0) && ((myAttributeProperty & ATTRPROPERTY_VCLASS) != 0);
}


bool
GNEAttributeCarrier::AttributeProperties::isList() const {
    return (myAttributeProperty & ATTRPROPERTY_LIST) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isSecuential() const {
    return (myAttributeProperty & ATTRPROPERTY_SECUENCIAL) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isUnique() const {
    return (myAttributeProperty & ATTRPROPERTY_UNIQUE) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isOptional() const {
    return (myAttributeProperty & ATTRPROPERTY_OPTIONAL) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isDiscrete() const {
    return (myAttributeProperty & ATTRPROPERTY_DISCRETE) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isCombinable() const {
    return (myAttributeProperty & ATTRPROPERTY_COMBINABLE) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isNonEditable() const {
    return (myAttributeProperty & ATTRPROPERTY_NONEDITABLE) != 0;
}


bool 
GNEAttributeCarrier::AttributeProperties::isExtended() const {
    return (myAttributeProperty & ATTRPROPERTY_EXTENDED) != 0;
}

// ---------------------------------------------------------------------------
// GNEAttributeCarrier::TagProperties - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrier::TagProperties::TagProperties() :
    myTag(SUMO_TAG_NOTHING),
    myTagType(0),
    myTagProperty(0),
    myIcon(ICON_EMPTY),
    myParentTag(SUMO_TAG_NOTHING),
    myTagSynonym(SUMO_TAG_NOTHING) {
}


GNEAttributeCarrier::TagProperties::TagProperties(SumoXMLTag tag, int tagType, int tagProperty, GUIIcon icon, SumoXMLTag parentTag, SumoXMLTag tagSynonym) :
    myTag(tag),
    myTagStr(toString(tag)),
    myTagType(tagType),
    myTagProperty(tagProperty),
    myIcon(icon),
    myParentTag(parentTag),
    myTagSynonym(tagSynonym) {
}


GNEAttributeCarrier::TagProperties::~TagProperties() {}


SumoXMLTag
GNEAttributeCarrier::TagProperties::getTag() const {
    return myTag;
}


const std::string&
GNEAttributeCarrier::TagProperties::getTagStr() const {
    return myTagStr;
}


void
GNEAttributeCarrier::TagProperties::checkTagIntegrity() const {
    // check that element must ist at least netElement, Additional, or shape
    if (!isNetElement() && !isAdditional() && !isShape() && !isTAZ() && !isDemandElement()) {
        throw ProcessError("element must be at leas netElement, Additional, shape or demandElement");
    }
    // check that element only is netElement, Additional, or shape at the same time
    if ((isNetElement() + isAdditional() + isShape() + isTAZ() + isDemandElement()) > 1) {
        throw ProcessError("element can be only a netElement, Additional, shape or demandElement at the same time");
    }
    // If element is drawable, chek that at least one placeover is defined
    if (isDrawable() && !(canBePlacedOverView() || canBePlacedOverEdge() || canBePlacedOverLane() || canBePlacedOverJunction() || canBePlacedOverEdges() || canBePlacedOverLanes() || canBePlacedOverRoute())) {
        throw ProcessError("If attribute is drawable a PLACEDOVER attribute must be defined");
    }
    // if element can mask the start and end position, check that bot attributes exist
    if (canMaskStartEndPos() && (!hasAttribute(SUMO_ATTR_STARTPOS) || !hasAttribute(SUMO_ATTR_ENDPOS))) {
        throw ProcessError("If attribute mask the start and end position, bot attribute has to be defined");
    }
    // check that synonym tag isn't nothing
    if (hasTagSynonym() && (myTagSynonym == SUMO_TAG_NOTHING)) {
        throw FormatException("synonym tag cannot be nothing");
    }
    // check integrity of all attributes
    for (auto i : myAttributeProperties) {
        i.second.checkAttributeIntegrity();
        // check that if attribute is combinable, own a combination of Allow/disallow attibute
        if (i.second.isCombinable()) {
            if ((i.first != SUMO_ATTR_ALLOW) && (i.first != SUMO_ATTR_DISALLOW)) {
                throw ProcessError("Attributes aren't combinables");
            } else if ((i.first == SUMO_ATTR_ALLOW) && !hasAttribute(SUMO_ATTR_DISALLOW)) {
                throw ProcessError("allow need a disallow attribute in the same tag");
            } else if ((i.first == SUMO_ATTR_DISALLOW) && !hasAttribute(SUMO_ATTR_ALLOW)) {
                throw ProcessError("disallow need an allow attribute in the same tag");
            }
        }
    }
    // check that all position listed are consecutives
    for (int i = 0; i < (int)myAttributeProperties.size(); i++) {
        bool found = false;
        for (auto j : myAttributeProperties) {
            if (j.second.getPositionListed() == i) {
                found = true;
            }
        }
        if (!found) {
            throw FormatException("There is no position listed consecutive");
        }
    }
}


const std::string&
GNEAttributeCarrier::TagProperties::getDefaultValue(SumoXMLAttr attr) const {
    if (myAttributeProperties.count(attr) == 0) {
        throw ProcessError("Attribute '" + toString(attr) + "' not defined");
    } else if (!myAttributeProperties.at(attr).hasDefaultValue()) {
        throw ProcessError("attribute '" + toString(attr) + "' doesn't have a default value");
    } else {
        return myAttributeProperties.at(attr).getDefaultValue();
    }
}


void
GNEAttributeCarrier::TagProperties::addAttribute(const AttributeProperties &attributeProperty) {
    if (isAttributeDeprecated(attributeProperty.getAttribute())) {
        throw ProcessError("Attribute '" + toString(attributeProperty.getAttribute()) + "' is deprecated and cannot be inserted");
    } else if (myAttributeProperties.count(attributeProperty.getAttribute()) != 0) {
        throw ProcessError("Attribute '" + toString(attributeProperty.getAttribute()) + "' already inserted");
    } else {
        // insert AttributeProperties in map
        myAttributeProperties[attributeProperty.getAttribute()] = attributeProperty;
        // update position listed 
        myAttributeProperties[attributeProperty.getAttribute()].setPositionListed((int)myAttributeProperties.size()-1);
    }
}


void
GNEAttributeCarrier::TagProperties::addDeprecatedAttribute(SumoXMLAttr attr) {
    // Check that attribute wasn't already inserted
    for (auto i : myAttributeProperties) {
        if (i.first == attr) {
            throw ProcessError("Attribute '" + toString(attr) + "' is deprecated but was inserted in list of attributes");
        }
    }
    // add it into myDeprecatedAttributes
    myDeprecatedAttributes.push_back(attr);
}


const GNEAttributeCarrier::AttributeProperties&
GNEAttributeCarrier::TagProperties::getAttributeProperties(SumoXMLAttr attr) const {
    if (myAttributeProperties.count(attr) != 0) {
        return myAttributeProperties.at(attr);
    } else {
        // check if we're try to loading an synonym
        for (auto i : myAttributeProperties) {
            if (i.second.hasAttrSynonym() && i.second.getAttrSynonym() == attr) {
                return myAttributeProperties.at(i.first);
            }
        }
        // throw error if these attribute doesn't exist
        throw ProcessError("Attribute '" + toString(attr) + "' doesn't exist");
    }
}


std::map<SumoXMLAttr, GNEAttributeCarrier::AttributeProperties>::const_iterator
GNEAttributeCarrier::TagProperties::begin() const {
    return myAttributeProperties.begin();
}


std::map<SumoXMLAttr, GNEAttributeCarrier::AttributeProperties>::const_iterator
GNEAttributeCarrier::TagProperties::end() const {
    return myAttributeProperties.end();
}


int
GNEAttributeCarrier::TagProperties::getNumberOfAttributes() const {
    return (int)myAttributeProperties.size();
}


GUIIcon
GNEAttributeCarrier::TagProperties::getGUIIcon() const {
    return myIcon;
}


SumoXMLTag
GNEAttributeCarrier::TagProperties::getParentTag() const {
    if (hasParent()) {
        return myParentTag;
    } else {
        throw ProcessError("Tag doesn't have parent");
    }
}


SumoXMLTag
GNEAttributeCarrier::TagProperties::getTagSynonym() const {
    if (hasTagSynonym()) {
        return myTagSynonym;
    } else {
        throw ProcessError("Tag doesn't have synonym");
    }
}


bool
GNEAttributeCarrier::TagProperties::hasAttribute(SumoXMLAttr attr) const {
    return (myAttributeProperties.count(attr) == 1);
}


bool
GNEAttributeCarrier::TagProperties::isNetElement() const {
    return (myTagType & TAGTYPE_NETELEMENT) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isAdditional() const {
    return (myTagType & TAGTYPE_ADDITIONAL) != 0;
}

bool
GNEAttributeCarrier::TagProperties::isShape() const {
    return (myTagType & TAGTYPE_SHAPE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isTAZ() const {
    return (myTagType & TAGTYPE_TAZ) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isDemandElement() const {
    return (myTagType & TAGTYPE_DEMANDELEMENT) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isStoppingPlace() const {
    return (myTagType & TAGTYPE_STOPPINGPLACE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isDetector() const {
    return (myTagType & TAGTYPE_DETECTOR) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isVehicle() const {
    return (myTagType & TAGTYPE_VEHICLE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isDrawable() const {
    return (myTagProperty & TAGPROPERTY_DRAWABLE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isSelectable() const {
    return (myTagProperty & TAGPROPERTY_SELECTABLE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBlockMovement() const {
    return (myTagProperty & TAGPROPERTY_BLOCKMOVEMENT) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBlockShape() const {
    return (myTagProperty & TAGPROPERTY_BLOCKSHAPE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canCloseShape() const {
    return (myTagProperty & TAGPROPERTY_CLOSESHAPE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasGEOPosition() const {
    return (myTagProperty & TAGPROPERTY_GEOPOSITION) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasGEOShape() const {
    return (myTagProperty & TAGPROPERTY_GEOSHAPE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasParent() const {
    return (myTagProperty & TAGPROPERTY_PARENT) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasTagSynonym() const {
    return (myTagProperty & TAGPROPERTY_SYNONYM) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasDialog() const {
    return (myTagProperty & TAGPROPERTY_DIALOG) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasMinimumNumberOfChilds() const {
    return (myTagProperty & TAGPROPERTY_MINIMUMCHILDS) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasGenericParameters() const {
    // note: By default all Tags supports generic parameters, except Tags with "TAGPROPERTY_NOGENERICPARAMETERS"
    return (myTagProperty & TAGPROPERTY_NOGENERICPARAMETERS) == 0;
}


bool
GNEAttributeCarrier::TagProperties::canBeReparent() const {
    return (myTagProperty & TAGPROPERTY_REPARENT) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canAutomaticSortChilds() const {
    return (myTagProperty & TAGPROPERTY_AUTOMATICSORTING) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canWriteChildsSeparate() const {
    return (myTagProperty & TAGPROPERTY_WRITECHILDSSEPARATE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBePlacedOverView() const {
    return (myTagProperty & TAGPROPERTY_PLACEDOVER_VIEW) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBePlacedOverEdge() const {
    return (myTagProperty & TAGPROPERTY_PLACEDOVER_EDGE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBePlacedOverLane() const {
    return (myTagProperty & TAGPROPERTY_PLACEDOVER_LANE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBePlacedOverJunction() const {
    return (myTagProperty & TAGPROPERTY_PLACEDOVER_JUNCTION) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBePlacedOverEdges() const {
    return (myTagProperty & TAGPROPERTY_PLACEDOVER_EDGES) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBePlacedOverLanes() const {
    return (myTagProperty & TAGPROPERTY_PLACEDOVER_LANES) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBePlacedOverRoute() const {
    return (myTagProperty & TAGPROPERTY_PLACEDOVER_ROUTE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canMaskStartEndPos() const {
    return (myTagProperty & TAGPROPERTY_MASKSTARTENDPOS) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canMaskXYZPositions() const {
    return (myTagProperty & TAGPROPERTY_MASKXYZPOSITION) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isAttributeDeprecated(SumoXMLAttr attr) const {
    return (std::find(myDeprecatedAttributes.begin(), myDeprecatedAttributes.end(), attr) != myDeprecatedAttributes.end());
}

// ---------------------------------------------------------------------------
// GNEAttributeCarrier - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrier::GNEAttributeCarrier(SumoXMLTag tag) :
    myTagProperty(getTagProperties(tag)),
    mySelected(false) {
}


template<> int
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toInt(string);
}


template<> double
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toDouble(string);
}


template<> bool
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toBool(string);
}


template<> std::string
GNEAttributeCarrier::parse(const std::string& string) {
    return string;
}


template<> SUMOVehicleClass
GNEAttributeCarrier::parse(const std::string& string) {
    if (string.size() == 0) {
        throw EmptyData();
    } else if (!SumoVehicleClassStrings.hasString(string)) {
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
        PositionVector pos = GeomConvHelper::parseShapeReporting(string, "user-supplied position", 0, ok, false, false);
        if (!ok || (pos.size() != 1)) {
            throw NumberFormatException("(Position) " + string);
        } else {
            return pos[0];
        }
    }
}


template<> PositionVector
GNEAttributeCarrier::parse(const std::string& string) {
    PositionVector posVector;
    // empty string are allowed (It means empty position vector)
    if (string.empty()) {
        return posVector;
    } else {
        bool ok = true;
        posVector = GeomConvHelper::parseShapeReporting(string, "user-supplied shape", 0, ok, false, true);
        if (!ok) {
            throw NumberFormatException("(Position List) " + string);
        } else {
            return posVector;
        }
    }
}


template<> SUMOVehicleShape
GNEAttributeCarrier::parse(const std::string& string) {
    if ((string == "unknown") || (!SumoVehicleShapeStrings.hasString(string))) {
        return SVS_UNKNOWN;
    } else {
        return SumoVehicleShapeStrings.get(string);
    }
}


template<> std::vector<std::string>
GNEAttributeCarrier::parse(const std::string& string) {
    return StringTokenizer(string).getVector();
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


template<> std::vector<GNEEdge*>
GNEAttributeCarrier::parse(GNENet* net, const std::string& value) {
    // Declare string vector
    std::vector<std::string> edgeIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    std::vector<GNEEdge*> parsedEdges;
    // Iterate over edges IDs, retrieve Edges and add it into parsedEdges
    for (auto i : edgeIds) {
        GNEEdge* retrievedEdge = net->retrieveEdge(i, false);
        if (retrievedEdge) {
            parsedEdges.push_back(net->retrieveEdge(i));
        } else {
            throw FormatException("Error parsing parameter " + toString(SUMO_ATTR_EDGES) + ". " + toString(SUMO_TAG_EDGE) + " '" + i + "' doesn't exist");
        }
    }
    return parsedEdges;
}


template<> std::vector<GNELane*>
GNEAttributeCarrier::parse(GNENet* net, const std::string& value) {
    // Declare string vector
    std::vector<std::string> laneIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    std::vector<GNELane*> parsedLanes;
    // Iterate over lanes IDs, retrieve Lanes and add it into parsedLanes
    for (auto i : laneIds) {
        GNELane* retrievedLane = net->retrieveLane(i, false);
        if (retrievedLane) {
            parsedLanes.push_back(net->retrieveLane(i));
        } else {
            throw FormatException("Error parsing parameter " + toString(SUMO_ATTR_LANES) + ". " + toString(SUMO_TAG_LANE) + " '" + i + "'  doesn't exist");
        }
    }
    return parsedLanes;
}


template<> std::string
GNEAttributeCarrier::parseIDs(const std::vector<GNEEdge*>& ACs) {
    // obtain ID's of edges and return their join
    std::vector<std::string> edgeIDs;
    for (auto i : ACs) {
        edgeIDs.push_back(i->getID());
    }
    return joinToString(edgeIDs, " ");
}


template<> std::string
GNEAttributeCarrier::parseIDs(const std::vector<GNELane*>& ACs) {
    // obtain ID's of lanes and return their join
    std::vector<std::string> laneIDs;
    for (auto i : ACs) {
        laneIDs.push_back(i->getID());
    }
    return joinToString(laneIDs, " ");
}


bool
GNEAttributeCarrier::lanesConsecutives(const std::vector<GNELane*>& lanes) {
    // we need at least two lanes
    if (lanes.size() > 1) {
        // now check that lanes are consecutives (not neccesary connected)
        int currentLane = 0;
        while (currentLane < ((int)lanes.size() - 1)) {
            int nextLane = -1;
            // iterate over outgoing edges of destiny juntion of edge's lane
            for (int i = 0; (i < (int)lanes.at(currentLane)->getParentEdge().getGNEJunctionDestiny()->getGNEOutgoingEdges().size()) && (nextLane == -1); i++) {
                // iterate over lanes of outgoing edges of destiny juntion of edge's lane
                for (int j = 0; (j < (int)lanes.at(currentLane)->getParentEdge().getGNEJunctionDestiny()->getGNEOutgoingEdges().at(i)->getLanes().size()) && (nextLane == -1); j++) {
                    // check if lane correspond to the next lane of "lanes"
                    if (lanes.at(currentLane)->getParentEdge().getGNEJunctionDestiny()->getGNEOutgoingEdges().at(i)->getLanes().at(j) == lanes.at(currentLane + 1)) {
                        nextLane = currentLane;
                    }
                }
            }
            if (nextLane == -1) {
                return false;
            } else {
                currentLane++;
            }
        }
        return true;
    } else {
        return false;
    }
}


std::string
GNEAttributeCarrier::getAttributeForSelection(SumoXMLAttr key) const {
    return getAttribute(key);
}

/*
SumoXMLTag
GNEAttributeCarrier::getTag() const {
    return myTagProperty.getTag();
}
*/

const std::string&
GNEAttributeCarrier::getTagStr() const {
    return myTagProperty.getTagStr();
}


const GNEAttributeCarrier::TagProperties&
GNEAttributeCarrier::getTagProperty() const {
    return myTagProperty;
}


FXIcon*
GNEAttributeCarrier::getIcon() const {
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    return GUIIconSubSys::getIcon(myTagProperty.getGUIIcon());
}


const std::string
GNEAttributeCarrier::getID() const {
    return getAttribute(SUMO_ATTR_ID);
}

// ===========================================================================
// static methods
// ===========================================================================

const GNEAttributeCarrier::TagProperties&
GNEAttributeCarrier::getTagProperties(SumoXMLTag tag) {
    if (tag == SUMO_TAG_NOTHING) {
        return dummyTagProperty;
    }
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    // check that tag is defined
    if (myTagProperties.count(tag) == 0) {
        throw ProcessError("Attributes for tag '" + toString(tag) + "' not defined");
    } else {
        return myTagProperties.at(tag);
    }
}


std::vector<SumoXMLTag>
GNEAttributeCarrier::allowedTags(bool onlyDrawables) {
    std::vector<SumoXMLTag> allTags;
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    // fill all tags
    for (const auto& i : myTagProperties) {
        if (!onlyDrawables || i.second.isDrawable()) {
            allTags.push_back(i.first);
        }
    }
    return allTags;
}


std::vector<SumoXMLTag>
GNEAttributeCarrier::allowedTagsByCategory(int tagPropertyCategory, bool onlyDrawables) {
    std::vector<SumoXMLTag> netElementTags;
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    switch (tagPropertyCategory) {
        case TAGTYPE_NETELEMENT:
            // fill netElements tags
            for (const auto& i : myTagProperties) {
                if (i.second.isNetElement() && (!onlyDrawables || i.second.isDrawable())) {
                    netElementTags.push_back(i.first);
                }
            }
            break;
        case TAGTYPE_ADDITIONAL:
            // fill additional tags
            for (const auto& i : myTagProperties) {
                if (i.second.isAdditional() && (!onlyDrawables || i.second.isDrawable())) {
                    netElementTags.push_back(i.first);
                }
            }
            break;
        case TAGTYPE_SHAPE:
            // fill shape tags
            for (const auto& i : myTagProperties) {
                if (i.second.isShape() && (!onlyDrawables || i.second.isDrawable())) {
                    netElementTags.push_back(i.first);
                }
            }
            break;
        case TAGTYPE_TAZ:
            // fill taz tags
            for (const auto& i : myTagProperties) {
                if (i.second.isTAZ() && (!onlyDrawables || i.second.isDrawable())) {
                    netElementTags.push_back(i.first);
                }
            }
            break;
        case TAGTYPE_DEMANDELEMENT:
            // fill demand tags
            for (const auto& i : myTagProperties) {
                if (i.second.isDemandElement() && (!onlyDrawables || i.second.isDrawable())) {
                    netElementTags.push_back(i.first);
                }
            }
            break;
        case TAGTYPE_VEHICLE:
            // fill demand tags
            for (const auto& i : myTagProperties) {
                if (i.second.isVehicle() && (!onlyDrawables || i.second.isDrawable())) {
                    netElementTags.push_back(i.first);
                }
            }
            break;
        default:
            throw ProcessError("Category isn't defined");
    }
    return netElementTags;
}


int
GNEAttributeCarrier::getHigherNumberOfAttributes() {
    int maxNumAttribute = 0;
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    // get max num attributes
    for (const auto& i : myTagProperties) {
        maxNumAttribute = MAX2(maxNumAttribute, i.second.getNumberOfAttributes());
    }
    return maxNumAttribute;
}


bool
GNEAttributeCarrier::isGenericParametersValid(const std::string& value) {
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two
        if (parsedParameters.size() == 2) {
            // check that key and value contains valid characters
            if (!SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) || !SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
                return false;
            }
        } else {
            return false;
        }
    }
    // all ok, then return true
    return true;
}


int
GNEAttributeCarrier::getCircleResolution(const GUIVisualizationSettings& settings) {
    if (settings.drawForSelecting) {
        return 8;
    } else if (settings.scale >= 10) {
        return 32;
    } else if (settings.scale >= 2) {
        return 16;
    } else {
        return 8;
    }
}


void
GNEAttributeCarrier::fillAttributeCarriers() {
    // fill all groups of ACs
    fillNetElements();
    fillAdditionals();
    fillShapes();
    fillDemandElements();
    // check integrity of all Tags (function checkTagIntegrity() throw an exception if there is an inconsistency)
    for (const auto& i : myTagProperties) {
        i.second.checkTagIntegrity();
    }
}


void 
GNEAttributeCarrier::fillNetElements() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // obtain Node Types except NODETYPE_DEAD_END_DEPRECATED
    const OptionsCont& oc = OptionsCont::getOptions();
    std::vector<std::string> nodeTypes = SUMOXMLDefinitions::NodeTypes.getStrings();
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(NODETYPE_DEAD_END_DEPRECATED)));
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(NODETYPE_DEAD_END)));
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(NODETYPE_NOJUNCTION)));
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(NODETYPE_INTERNAL)));
    // fill netElement ACs
    SumoXMLTag currentTag = SUMO_TAG_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE, ICON_EDGE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the edge");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of a node within the nodes-file the edge shall start at" );
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of a node within the nodes-file the edge shall end at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The maximum speed allowed on the edge in m/s");
                toString(oc.getFloat("default.speed"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PRIORITY,
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE,
            "The priority of the edge");
                toString(oc.getInt("default.priority"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NUMLANES,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The number of lanes of the edge");
                toString(oc.getInt("default.lanenumber"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE,
            "The name of a type within the SUMO edge type file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE,
            "Explicitly allows the given vehicle classes (not given will be not allowed)",
            "all");
                attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE,
            "Explicitly disallows the given vehicle classes (not given will be allowed)");
                attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "If the shape is given it should start and end with the positions of the from-node and to-node");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE,
            "The length of the edge in meter");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPREADTYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE,
            "Lane width for all lanes of this edge in meters (used for visualization)",
            "right");
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::LaneSpreadFunctions.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE,
            "street name (need not be unique, used for visualization)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Lane width for all lanes of this edge in meters (used for visualization)",
            "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDOFFSET,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Move the stop line back from the intersection by the given amount",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(GNE_ATTR_SHAPE_START,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_DEFAULTVALUE, // virtual attribute used to define an endPoint
            "Custom position in which shape start (by default position of junction from)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(GNE_ATTR_SHAPE_END,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_DEFAULTVALUE, // virtual attribute from to define an endPoint
            "Custom position in which shape end (by default position of junction from)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(GNE_ATTR_BIDIR,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_NONEDITABLE, // virtual attribute to check of this edge is part of a bidirectional railway (cannot be edited)
            "Show if edge is bidireccional",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_JUNCTION;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE, ICON_JUNCTION);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the node");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_POSITION, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
            "The x-y-z position of the node on the plane in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE,
            "An optional type for the node");
                attrProperty.setDiscreteValues(nodeTypes);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "A custom shape for that node");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_RADIUS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Optional turning radius (for all corners) for that node in meters",
            "1.5");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_KEEP_CLEAR,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE,
            "Whether the junction-blocking-heuristic should be activated at this node",
            "1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_RIGHT_OF_WAY,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE,
            "How to compute right of way rules at this node",
                SUMOXMLDefinitions::RightOfWayValues.getString(RIGHT_OF_WAY_DEFAULT));
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::RightOfWayValues.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLTYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE,
            "An optional type for the traffic light algorithm");
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::TrafficLightTypes.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE,
            "An optional id for the traffic light program");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE, ICON_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
            "ID of lane (Automatic, non editable)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_INDEX,
            ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
            "The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Speed in meters per second",
            "13.89");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE,
            "Explicitly allows the given vehicle classes (not given will be not allowed)",
            "all");
                attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE,
            "Explicitly disallows the given vehicle classes (not given will be allowed)");
                attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Width in meters (used for visualization)",
            "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDOFFSET,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Move the stop line back from the intersection by the given amount",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ACCELERATION,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE,
            "Enable or disable lane as acceleration lane",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_UNIQUE,
            "If the shape is given it overrides the computation based on edge shape");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CROSSING;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_JUNCTION | TAGPROPERTY_SELECTABLE, ICON_CROSSING, SUMO_TAG_JUNCTION);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
            "The ID of Crossing");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE,
            "The (road) edges which are crossed");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PRIORITY,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE,
            "Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections)",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The width of the crossings",
                toString(OptionsCont::getOptions().getFloat("default.crossing-width")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLLINKINDEX,
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE,
            "sets the tls-index for this crossing"
            "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLLINKINDEX2,
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE,
            "sets the opposite-direction tls-index for this crossing"
            "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "Overrids default shape of pedestrian crossing");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CONNECTION;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE, ICON_CONNECTION, SUMO_TAG_EDGE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
            "The name of the edge the vehicles leave");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
            "The name of the edge the vehicles may reach when leaving 'from'");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FROM_LANE,
            ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
            "the lane index of the incoming lane (numbers starting with 0)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO_LANE,
            ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
            "the lane index of the outgoing lane (numbers starting with 0)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PASS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE,
            "if set, vehicles which pass this (lane-2-lane) connection) will not wait"
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_KEEP_CLEAR,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE,
            "if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection"
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CONTPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "If set to a more than 0 value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection",
                toString(NBEdge::UNSPECIFIED_CONTPOS));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_UNCONTROLLED,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE,
            "If set to true, This connection will not be TLS-controlled despite its node being controlled"
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VISIBILITY_DISTANCE,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Vision distance between vehicles",
                toString(NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLLINKINDEX,
            ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUE,
            "sets the distance to the connection at which all relevant foes are visible",
            "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "sets custom speed limit for the connection",
                toString(NBEdge::UNSPECIFIED_SPEED));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "sets custom shape for the connection");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void 
GNEAttributeCarrier::fillAdditionals() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // fill additional elements
    SumoXMLTag currentTag = SUMO_TAG_BUS_STOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_CONTAINERSTOP, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of bus stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the lane the bus stop shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The begin position on the lane (the lower position on the lane) in meters");

        myTagProperties[currentTag].addAttribute(attrProperty);
        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LINES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ACCESS;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_ACCESS, SUMO_TAG_BUS_STOP);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the lane the stop access shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The position on the lane (the lower position on the lane) in meters"
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The walking length of the access in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1"
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_CONTAINER_STOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_CONTAINERSTOP, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of container stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the lane the container stop shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The begin position on the lane (the lower position on the lane) in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LINES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CHARGING_STATION;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_CHARGINGSTATION, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of charging station");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "Lane of the charging station location");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Begin position in the specified lane");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "End position in the specified lane");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CHARGINGPOWER,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Charging power in W",
            "22000.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EFFICIENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_RANGE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Charging efficiency [0,1]",
            "0.95");
                attrProperty.setRange(0, 1);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CHARGEINTRANSIT,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CHARGEDELAY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_PARKING_AREA;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_PARKINGAREA, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of ParkingArea");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the lane the Parking Area shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The begin position on the lane (the lower position on the lane) in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROADSIDE_CAPACITY,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            " The number of parking spaces for road-side parking",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ONROAD,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, vehicles will park on the road lane and thereby reducing capacity",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The width of the road-side parking spaces",
            "3.20");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_OPTIONAL | ATTRPROPERTY_DEFAULTVALUE,
            "The length of the road-side parking spaces. By default (endPos - startPos) / roadsideCapacity");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise"
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_PARKING_SPACE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_MASKXYZPOSITION | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_REPARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_PARKINGSPACE, SUMO_TAG_PARKING_AREA);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_POSITION, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
            "The x-y-z position of the parking vehicle on the plane");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The width of the road-side parking spaces",
            "3.20");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The length of the road-side parking spaces",
            "5.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_E1DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_E1, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of E1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "The aggregation period the values the detector collects shall be summed up",
            "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE,
            "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_E2DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_E2, SUMO_TAG_LANE);
        // set "file" as deprecated attribute
        myTagProperties[currentTag].addDeprecatedAttribute(SUMO_ATTR_CONT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of E2");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The length of the detector in meters",
            "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "The aggregation period the values the detector collects shall be summed up",
            "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE,
            "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Space separated list of vehicle type ids to consider");

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s",
            "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m",
            "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_E2DETECTOR_MULTILANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANES | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SYNONYM | TAGPROPERTY_BLOCKMOVEMENT, ICON_E2, SUMO_TAG_LANE, SUMO_TAG_E2DETECTOR);
        // set "file" as deprecated attribute
        myTagProperties[currentTag].addDeprecatedAttribute(SUMO_ATTR_CONT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of Multilane E2");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_SECUENCIAL | ATTRPROPERTY_UNIQUE,
            "The list of secuencial lane ids in which the detector shall be laid on");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The end position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "The aggregation period the values the detector collects shall be summed up",
            "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE,
            "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s",
            "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m",
            "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_E3DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MINIMUMCHILDS | TAGPROPERTY_AUTOMATICSORTING, ICON_E3);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of E3");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "X-Y position of detector in editor (Only used in NETEDIT)",
            "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "The aggregation period the values the detector collects shall be summed up",
            "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE,
            "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) in s",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s",
            "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_DET_ENTRY;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_REPARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_E3ENTRY, SUMO_TAG_E3DETECTOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_DET_EXIT;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_REPARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_E3EXIT, SUMO_TAG_E3DETECTOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_INSTANT_INDUCTION_LOOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_E1INSTANT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of Instant Induction Loop (E1Instant)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE,
            "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_VSS;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_DIALOG, ICON_VARIABLESPEEDSIGN);
        // set "file" as deprecated attribute
        myTagProperties[currentTag].addDeprecatedAttribute(SUMO_ATTR_FILE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "The id of Variable Speed Signal");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "X-Y position of detector in editor (Only used in NETEDIT)",
            "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE,
            "list of lanes of Variable Speed Sign");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_STEP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_VSSSTEP, SUMO_TAG_VSS);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_TIME,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME,
            "Time");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE,
            "Speed"
            "13.89");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CALIBRATOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_EDGE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_DIALOG, ICON_CALIBRATOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of Calibrator");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of edge in the simulation network");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The position of the calibrator on the specified lane",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The aggregation interval in which to calibrate the flows. default is step-length",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTEPROBE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The id of the routeProbe element from which to determine the route distribution for generated vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_OUTPUT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The output file for writing calibrator information or NULL");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_LANECALIBRATOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_SYNONYM | TAGPROPERTY_DIALOG, ICON_CALIBRATOR, SUMO_TAG_NOTHING, SUMO_TAG_CALIBRATOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of Calibrator");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of lane in the simulation network");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The position of the calibrator on the specified lane",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The aggregation interval in which to calibrate the flows. default is step-length",
            "100.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTEPROBE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The id of the routeProbe element from which to determine the route distribution for generated vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_OUTPUT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The output file for writing calibrator information or NULL");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CALIBRATORFLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_FLOW, SUMO_TAG_CALIBRATOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "The id of the vehicle type to use for this vehicle",
                DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the route the vehicle shall drive along");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VEHSPERHOUR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Number of vehicles per hour, equally spaced");

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Speed of vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "This vehicle's color",
            "yellow");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "First vehicle departure time",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_END,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "End of departure interval",
            "3600.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lane on which the vehicle shall be inserted",
            "first");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The position at which the vehicle shall enter the net",
            "base");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed with which the vehicle shall enter the network",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lane at which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The position at which the vehicle shall leave the network",
            "max");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed with which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LINE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A string specifying the id of a public transport line which can be used when specifying person rides");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PERSON_NUMBER,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of occupied seats when the vehicle is inserted",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_NUMBER,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of occupied container places when the vehicle is inserted",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_REROUTE,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Whether the vehicle should be equipped with a rerouting device",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS_LAT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lateral position on the departure lane at which the vehicle shall enter the net",
            "center");
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS_LAT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lateral position on the arrival lane at which the vehicle shall arrive",
            "center");
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_REROUTER;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_DIALOG | TAGPROPERTY_WRITECHILDSSEPARATE, ICON_REROUTER);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of Rerouter");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE,
            "An edge id or a list of edge ids where vehicles shall be rerouted");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "X,Y position in editor (Only used in NETEDIT)",
            "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The path to the definition file (alternatively, the intervals may defined as children of the rerouter)");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The probability for vehicle rerouting (0-1)",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The waiting time threshold (in s) that must be reached to activate rerouting (default -1 which disables the threshold)",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The list of vehicle types that shall be affected by this rerouter (empty to affect all types)");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_OFF,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Whether the router should be inactive initially (and switched on in the gui)",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_INTERVAL;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_REROUTERINTERVAL, SUMO_TAG_REROUTER);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "Begin",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_END,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "End",
            "3600.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CLOSING_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_CLOSINGREROUTE, SUMO_TAG_INTERVAL);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM,
            "Edge ID");
                attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_OPTIONAL,
            "allowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_OPTIONAL,
            "disallowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CLOSING_LANE_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_CLOSINGLANEREROUTE, SUMO_TAG_INTERVAL);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM,
            "Lane ID");
                attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_OPTIONAL,
            "allowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_OPTIONAL,
            "disallowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_DEST_PROB_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_DESTPROBREROUTE, SUMO_TAG_INTERVAL);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM,
            "Edge ID");
                attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "SUMO Probability",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_PARKING_ZONE_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_PARKINGZONEREROUTE, SUMO_TAG_INTERVAL);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_PARKING,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM,
            "ParkingArea ID");
                attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "SUMO Probability",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_VISIBLE,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_OPTIONAL | ATTRPROPERTY_DEFAULTVALUE,
            "Enable or disable visibility for parking area reroutes",
            "1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ROUTE_PROB_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_ROUTEPROBREROUTE, SUMO_TAG_INTERVAL);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM,
            "Route");
                attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "SUMO Probability",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ROUTEPROBE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_EDGE | TAGPROPERTY_SELECTABLE, ICON_ROUTEPROBE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of RouteProbe");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of an edge in the simulation network");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "The frequency in which to report the distribution",
            "3600");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE,
            "The file for generated output");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The time at which to start generating output",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VAPORIZER;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_EDGE | TAGPROPERTY_SELECTABLE, ICON_VAPORIZER);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "Edge in which vaporizer is placed");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "Start Time",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_END,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "End Time",
            "3600.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TAZ;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_TAZ, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_BLOCKSHAPE | TAGPROPERTY_AUTOMATICSORTING, ICON_TAZ);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the TAZ");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The shape of the TAZ");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The RGBA color with which the TAZ shall be displayed",
            "red");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TAZSOURCE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_TAZ, TAGPROPERTY_PARENT, ICON_TAZEDGE, SUMO_TAG_TAZ);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_SYNONYM,
            "The id of edge in the simulation network");
                attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_WEIGHT,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Depart weight associated to this Edge",
            "1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TAZSINK;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_TAZ, TAGPROPERTY_PARENT, ICON_TAZEDGE, SUMO_TAG_TAZ);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_SYNONYM,
            "The id of edge in the simulation network");
                attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_WEIGHT,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "Arrival weight associated to this Edget",
            "1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}
     

void 
GNEAttributeCarrier::fillShapes() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // fill shape ACs
    SumoXMLTag currentTag = SUMO_TAG_POLY;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_SHAPE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_BLOCKSHAPE | TAGPROPERTY_CLOSESHAPE | TAGPROPERTY_GEOSHAPE, ICON_LOCATEPOLY /* temporal */);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the polygon");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_SHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE,
            "The shape of the polygon");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The RGBA color with which the polygon shall be displayed",
            "red");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_FILL,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "An information whether the polygon shall be filled",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LINEWIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The default line width for drawing an unfilled polygon",
            "1");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LAYER,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The layer in which the polygon lies",
                toString(Shape::DEFAULT_LAYER));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A typename for the polygon",
                toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A bitmap to use for rendering this polygon",
                toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_RELATIVEPATH,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Enable or disable use image file as a relative path",
                toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Angle of rendered image in degree",
                toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_POI;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_SHAPE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_VIEW | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKXYZPOSITION | TAGPROPERTY_GEOPOSITION, ICON_LOCATEPOI /* temporal */);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the POI");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
            "The position in view");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The color with which the poi shall be displayed",
            "red");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A typename for the poi",
                toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LAYER,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The layer of the poi for drawing and selecting",
                toString(Shape::DEFAULT_LAYER_POI));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Width of rendered image in meters",
                toString(Shape::DEFAULT_IMG_WIDTH));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_HEIGHT,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Height of rendered image in meters",
                toString(Shape::DEFAULT_IMG_HEIGHT));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A bitmap to use for rendering this poi",
                toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_RELATIVEPATH,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Enable or disable use image file as a relative path",
                toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Angle of rendered image in degree",
                toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_POILANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_SHAPE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_LOCATEPOI /* temporal */);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the POI");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the lane the poi is located at");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE,
            "The position on the named lane or in the net in meters at which the poi is located at");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_POSITION_LAT,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lateral offset on the named lane at which the poi is located at",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The color with which the poi shall be displayed",
            "red");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A typename for the poi",
                toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LAYER,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The layer of the poi for drawing and selecting",
                toString(Shape::DEFAULT_LAYER_POI));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Width of rendered image in meters",
                toString(Shape::DEFAULT_IMG_WIDTH));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_HEIGHT,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Height of rendered image in meters",
                toString(Shape::DEFAULT_IMG_HEIGHT));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A bitmap to use for rendering this poi",
                toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_RELATIVEPATH,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Enable or disable use image file as a relative path",
                toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Angle of rendered image in degree",
                toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void 
GNEAttributeCarrier::fillDemandElements() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // obtain a vector string with the emissions
    std::vector<std::string> emissions = { "zero", "LDV", "LDV_G_EU0", "LDV_G_EU1", "LDV_G_EU2", "LDV_G_EU3", "LDV_G_EU4", "LDV_G_EU5",
                                           "LDV_G_EU6", "LDV_G_East", "LDV_D_EU0", "LDV_D_EU1", "LDV_D_EU2", "LDV_D_EU3", "LDV_D_EU4", "LDV_D_EU5", "LDV_D_EU6",
                                           "PC", "PC_Alternative", "PC_G_EU0", "PC_G_EU1", "PC_G_EU2", "PC_G_EU3", "PC_G_EU4", "PC_G_EU5", "PC_G_EU6", "PC_G_East",
                                           "PC_D_EU0", "PC_D_EU1", "PC_D_EU2", "PC_D_EU3", "PC_D_EU4", "PC_D_EU5", "PC_D_EU6", "Bus", "Coach", "HDV", "HDV_G", "HDV_D_EU0",
                                           "HDV_D_EU1", "HDV_D_EU2", "HDV_D_EU3", "HDV_D_EU4", "HDV_D_EU5", "HDV_D_EU6", "HDV_D_East"
                                         };
    // fill demand elements
    SumoXMLTag  currentTag = SUMO_TAG_ROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PLACEDOVER_EDGES, ICON_ROUTE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of Route");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE,
            "The edges the vehicle shall drive along, given as their ids, separated using spaces");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "This route's color",
            "yellow");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VTYPE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PLACEDOVER_VIEW, ICON_VTYPE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of VehicleType");
        myTagProperties[currentTag].addAttribute(attrProperty);
 
        attrProperty = AttributeProperties(SUMO_ATTR_ACCEL,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The acceleration ability of vehicles of this type [m/s^2]",
            "2.60");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DECEL,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The deceleration ability of vehicles of this type [m/s^2]",
            "4.50");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_APPARENTDECEL,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The apparent deceleration of the vehicle as used by the standard model [m/s^2]",
            "4.50");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EMERGENCYDECEL,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The maximal physically possible deceleration for the vehicle [m/s^2]",
            "4.50");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SIGMA,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_RANGE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Car-following model parameter",
            "0.50");
                attrProperty.setRange(0, 1);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_TAU,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Car-following model parameter"
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The vehicle's netto-length (length) [m]",
            "5.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_MINGAP,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Empty space after leader [m]",
            "2.50");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_MAXSPEED,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The vehicle's maximum velocity [m/s]",
            "55.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_SPEEDFACTOR,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The vehicles expected multiplicator for lane speed limits",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_SPEEDDEV,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The deviation of the speedFactor",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "This vehicle type's color",
            "1,1,0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_VCLASS,
            ATTRPROPERTY_VCLASS | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "An abstract vehicle class",
            "passenger");
                attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_EMISSIONCLASS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "An abstract emission class",
            "PC_G_EU4");
                attrProperty.setDiscreteValues(emissions);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_GUISHAPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "How this vehicle is rendered");
                attrProperty.setDiscreteValues(SumoVehicleShapeStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The vehicle's width [m] (only used for drawing)",
            "1.8");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_IMPATIENCE,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Willingess of drivers to impede vehicles with higher priority",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LANE_CHANGE_MODEL,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The model used for changing lanes",
            "LC2013");
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::LaneChangeModels.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_CAR_FOLLOW_MODEL,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The model used for car following",
            "Krauss");
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::CarFollowModels.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PERSON_CAPACITY,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of persons (excluding an autonomous driver) the vehicle can transport",
            "4");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_CAPACITY,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of containers the vehicle can transport",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_BOARDING_DURATION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The time required by a person to board the vehicle",
            "0.50");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LOADING_DURATION,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The time required to load a container onto the vehicle",
            "90.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LATALIGNMENT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The preferred lateral alignment when using the sublane-model",
            "center");
                attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_MINGAP_LAT,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The minimum lateral gap at a speed difference of 50km/h when using the sublane-model",
            "0.12");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_MAXSPEED_LAT,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The maximum lateral speed when using the sublane-model",
            "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ACTIONSTEPLENGTH,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The interval length for which vehicle performs its decision logic (acceleration and lane-changing)",
            toString(OptionsCont::getOptions().getFloat("default.action-step-length")));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VEHICLE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_ROUTE, ICON_VEHICLE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the vehicle");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "The id of the vehicle type to use for this vehicle",
                DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the route the vehicle shall drive along");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE,
            "This vehicle's color",
            "yellow");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPART,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The time step at which the vehicle shall enter the network",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lane on which the vehicle shall be inserted",
            "first");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The position at which the vehicle shall enter the net",
            "base");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The speed with which the vehicle shall enter the network",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The lane at which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The position at which the vehicle shall leave the network",
            "max");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The speed with which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LINE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A string specifying the id of a public transport line which can be used when specifying person rides");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PERSON_NUMBER,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of occupied seats when the vehicle is inserted",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_NUMBER,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of occupied container places when the vehicle is inserted",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_REROUTE,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Whether the vehicle should be equipped with a rerouting device",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_VIA,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST |ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "List of intermediate edges that shall be passed on rerouting");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS_LAT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The lateral position on the departure lane at which the vehicle shall enter the net",
            "center");
                /*attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());*/
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS_LAT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The lateral position on the arrival lane at which the vehicle shall arrive",
            "center");
                /*attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());*/
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_FLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE, ICON_FLOW);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the vehicle");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "The id of the vehicle type to use for this vehicle",
                DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The id of the route the vehicle shall drive along");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "This vehicle's color",
            "yellow");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lane on which the vehicle shall be inserted",
            "first");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The position at which the vehicle shall enter the net",
            "base");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed with which the vehicle shall enter the network"
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lane at which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The position at which the vehicle shall leave the network",
            "max");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed with which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_LINE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "A string specifying the id of a public transport line which can be used when specifying person rides");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PERSON_NUMBER,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of occupied seats when the vehicle is inserted",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_NUMBER,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The number of occupied container places when the vehicle is inserted",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_REROUTE,
            ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Whether the vehicle should be equipped with a rerouting device",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS_LAT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The lateral position on the departure lane at which the vehicle shall enter the net",
            "center");
                /*attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());*/
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS_LAT,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
            "The lateral position on the arrival lane at which the vehicle shall arrive",
            "center");
                /*attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());*/
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "First vehicle departure time",
            "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_END,
            ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_TIME | ATTRPROPERTY_DEFAULTVALUE,
            "End of departure interval",
            "3600.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_VEHSPERHOUR,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Number of vehicles per hour, equally spaced (not together with period or probability)",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PERIOD,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "Insert equally spaced vehicles at that period (not together with vehsPerHour or probability)",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "probability for emitting a vehicle each second (not together with vehsPerHour or period)",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_NUMBER,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "probability for emitting a vehicle each second (not together with vehsPerHour or period)", 
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TRIP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_PLACEDOVER_LANE, ICON_TRIP);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUE,
            "The name of vehicles that will be generated using this trip definition");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPART,
            ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE,
            "The departure time of the (first) vehicle which is generated using this trip definition",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the edge the route starts at; the edge must be a part of the used network");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_TO,
            ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
            "The name of the edge the route ends at; the edge must be a part of the used network");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VIA,
            ATTRPROPERTY_STRING | ATTRPROPERTY_LIST,
            "List of intermediate edge ids which shall be part of the route; the edges must be a part of the used network");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_FROM_TAZ,
            ATTRPROPERTY_STRING,
            "The name of the district the route starts at. TAZ edges are selected so that travel time is minimized");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_TO_TAZ,
            ATTRPROPERTY_STRING,
            "The name of the district the route ends at. TAZ edges are selected so that travel time is minimized");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
            ATTRPROPERTY_COLOR | ATTRPROPERTY_OPTIONAL | ATTRPROPERTY_DEFAULTVALUE,
            "This generated vehicle's color",
            "yellow");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lane on which the vehicle shall be inserted",
            "first");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The position at which the vehicle shall enter the net",
            "base");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_DEPARTSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed with which the vehicle shall enter the network",
            "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALLANE,
            ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The lane at which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The position at which the vehicle shall leave the network",
            "max");
        myTagProperties[currentTag].addAttribute(attrProperty);
        
        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALSPEED,
            ATTRPROPERTY_STRING | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUE | ATTRPROPERTY_OPTIONAL,
            "The speed with which the vehicle shall leave the network",
            "current");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


bool 
GNEAttributeCarrier::checkParsedAttribute(const TagProperties& tagProperties, 
                                          const AttributeProperties& attrProperties, const SumoXMLAttr attribute, bool& parsedOk, 
                                          std::string &defaultValue, std::string &parsedAttribute, std::string &warningMessage) {
    // declare a string for details about error formats
    std::string errorFormat;
    // set extra check for ID Values
    if (attribute == SUMO_ATTR_ID) {
        if (parsedAttribute.empty()) {
            errorFormat = "ID cannot be empty; ";
            parsedOk = false;
        } else if (tagProperties.isDetector()) {
            // special case for detectors (because in this case empty spaces are allowed)
            if (SUMOXMLDefinitions::isValidDetectorID(parsedAttribute) == false) {
                errorFormat = "Detector ID contains invalid characters; ";
                parsedOk = false;
            }
        } else if (SUMOXMLDefinitions::isValidNetID(parsedAttribute) == false) {
            errorFormat = "ID contains invalid characters; ";
            parsedOk = false;
        }
    }
    // Set extra checks for int values
    if (attrProperties.isInt()) {
        if (canParse<int>(parsedAttribute)) {
            // obtain int value
            int parsedIntAttribute = parse<int>(parsedAttribute);
            // check if attribute can be negative or zero
            if (attrProperties.isPositive() && (parsedIntAttribute < 0)) {
                errorFormat = "Cannot be negative; ";
                parsedOk = false;
            } else if (attrProperties.cannotBeZero() && (parsedIntAttribute == 0)) {
                errorFormat = "Cannot be zero; ";
                parsedOk = false;
            }
        } else if (canParse<double>(parsedAttribute)) {
            errorFormat = "Float cannot be reinterpreted as int; ";
            parsedOk = false;
        } else {
            errorFormat = "Cannot be parsed to int; ";
            parsedOk = false;
        }
    }
    // Set extra checks for float(double) values
    if (attrProperties.isFloat()) {
        if (canParse<double>(parsedAttribute)) {
            // obtain double value
            double parsedDoubleAttribute = parse<double>(parsedAttribute);
            //check if can be negative and Zero
            if (attrProperties.isPositive() && (parsedDoubleAttribute < 0)) {
                errorFormat = "Cannot be negative; ";
                parsedOk = false;
            } else if (attrProperties.cannotBeZero() && (parsedDoubleAttribute == 0)) {
                errorFormat = "Cannot be zero; ";
                parsedOk = false;
            }
        } else {
            errorFormat = "Cannot be parsed to float; ";
            parsedOk = false;
        }
    }
    // Set extra checks for position values
    if (attrProperties.isposition()) {
        // check if we're parsing a single position or an entire shape
        if (attrProperties.isList()) {
            // check if parsed attribute can be parsed to Position Vector
            if (!canParse<PositionVector>(parsedAttribute)) {
                errorFormat = "List of Positions aren't neither x,y nor x,y,z; ";
                parsedOk = false;
            }
        } else if (!canParse<Position>(parsedAttribute)) {
            errorFormat = "Position is neither x,y nor x,y,z; ";
            parsedOk = false;
        }
    }
    // set extra check for time(double) values
    if (attrProperties.isTime()) {
        if (canParse<double>(parsedAttribute)) {
            // parse to SUMO Real and check if is negative
            if (parse<double>(parsedAttribute) < 0) {
                errorFormat = "Time cannot be negative; ";
                parsedOk = false;
            }
        } else {
            errorFormat = "Cannot be parsed to time; ";
            parsedOk = false;
        }
    }
    // set extra check for probability values
    if (attrProperties.isProbability()) {
        if (canParse<double>(parsedAttribute)) {
            // parse to double and check if is between [0,1]
            double probability = parse<double>(parsedAttribute);
            if (probability < 0) {
                errorFormat = "Probability cannot be smaller than 0; ";
                parsedOk = false;
            } else if (probability > 1) {
                errorFormat = "Probability cannot be greather than 1; ";
                parsedOk = false;
            }
        } else {
            errorFormat = "Cannot be parsed to probability; ";
            parsedOk = false;
        }
    }
    // set extra check for range values
    if (attrProperties.hasAttrRange()) {
        if (canParse<double>(parsedAttribute)) {
            // parse to double and check if is in range
            double range = parse<double>(parsedAttribute);
            if (range < attrProperties.getMinimumRange()) {
                errorFormat = "Float cannot be smaller than " + toString(attrProperties.getMinimumRange()) + "; ";
                parsedOk = false;
            } else if (range > attrProperties.getMaximumRange()) {
                errorFormat = "Float cannot be greather than " + toString(attrProperties.getMaximumRange()) + "; ";
                parsedOk = false;
            }
        } else {
            errorFormat = "Cannot be parsed to float; ";
            parsedOk = false;
        }
    }
    // set extra check for discrete values
    if (attrProperties.isDiscrete()) {
        // search value in the list of discretes values of attribute properties
        auto finder = std::find(attrProperties.getDiscreteValues().begin(), attrProperties.getDiscreteValues().end(), parsedAttribute);
        // check if attribute is valid
        if (finder == attrProperties.getDiscreteValues().end()) {
            errorFormat = "value is not within the set of allowed values for attribute '" + toString(attribute) + "'";
            parsedOk = false;
        }
    }
    // set extra check for color values
    if (attrProperties.isColor() && !canParse<RGBColor>(parsedAttribute)) {
        errorFormat = "Invalid RGB format or named color; ";
        parsedOk = false;
    }
    // set extra check for filename values
    if (attrProperties.isFilename()) {
        if (SUMOXMLDefinitions::isValidFilename(parsedAttribute) == false) {
            errorFormat = "Filename contains invalid characters; ";
            parsedOk = false;
        } else if (parsedAttribute.empty()) {
            errorFormat = "Filename cannot be empty; ";
            parsedOk = false;
        }
    }
    // set extra check for name values
    if ((attribute == SUMO_ATTR_NAME) && !SUMOXMLDefinitions::isValidAttribute(parsedAttribute)) {
        errorFormat = "name contains invalid characters; ";
        parsedOk = false;
    }
    // set extra check for SVCPermissions values
    if (attrProperties.isVClass()) {
        if (!canParseVehicleClasses(parsedAttribute)) {
            errorFormat = "List of VClasses isn't valid; ";
            parsedAttribute = defaultValue;
            parsedOk = false;
        }
    }
    // set extra check for Vehicle Classes
    if ((!parsedOk) && (attribute == SUMO_ATTR_VCLASS)) {
        errorFormat = "Is not a part of defined set of Vehicle Classes; ";
    }
    // set extra check for Vehicle Classes
    if ((!parsedOk) && (attribute == SUMO_ATTR_GUISHAPE)) {
        errorFormat = "Is not a part of defined set of Gui Vehicle Shapes; ";
    }
    // set extra check for RouteProbes
    if ((attribute == SUMO_ATTR_ROUTEPROBE) && !SUMOXMLDefinitions::isValidNetID(parsedAttribute)) {
        errorFormat = "RouteProbe ID contains invalid characters; ";
        parsedOk = false;
    }
    // set extra check for list of edges
    if ((attribute == SUMO_ATTR_EDGES) && parsedAttribute.empty()) {
        errorFormat = "List of edges cannot be empty; ";
        parsedOk = false;
    }
    // set extra check for list of lanes
    if ((attribute == SUMO_ATTR_LANES) && parsedAttribute.empty()) {
        errorFormat = "List of lanes cannot be empty; ";
        parsedOk = false;
    }
    // set extra check for list of VTypes
    if ((attribute == SUMO_ATTR_VTYPES) && !parsedAttribute.empty() && !SUMOXMLDefinitions::isValidListOfTypeID(parsedAttribute)) {
        errorFormat = "List of vTypes contains invalid characters; ";
        parsedOk = false;
    }
    // set extra check for list of RouteProbe
    if ((attribute == SUMO_ATTR_ROUTEPROBE) && !parsedAttribute.empty() && !SUMOXMLDefinitions::isValidNetID(parsedAttribute)) {
        errorFormat = "RouteProbe ID contains invalid characters; ";
        parsedOk = false;
    }
    // If attribute has an invalid format
    if (!parsedOk) {
        // if attribute is optional and has a default value, obtain it as string. In other case, abort.
        if (attrProperties.isOptional() && attrProperties.hasDefaultValue()) {
            parsedAttribute = attrProperties.getDefaultValue();
        } else {
            WRITE_WARNING("Format of essential " + attrProperties.getDescription() + " attribute '" + toString(attribute) + "' of " +
                            warningMessage +  " is invalid; " + errorFormat + tagProperties.getTagStr() + " cannot be created");
            // set default value (To avoid errors in parse<T>(parsedAttribute))
            parsedAttribute = defaultValue;
            // return false to abort creation of element
            return false;
        }
    }
    // return true to continue creation of element
    return true;
}


bool 
GNEAttributeCarrier::parseMaskedPositionAttribute(const SUMOSAXAttributes& attrs, const std::string& objectID, const TagProperties& tagProperties, 
                                                  const AttributeProperties& attrProperties, bool& parsedOk, 
                                                  std::string &parsedAttribute, std::string &warningMessage) {
    // if element can mask their XYPosition, then must be extracted X Y coordiantes separeted
    std::string x, y, z;
    // give a default value to parsedAttribute to avoid problem parsing invalid positions
    parsedAttribute = "0,0";
    if (attrs.hasAttribute(SUMO_ATTR_X)) {
        x = attrs.get<std::string>(SUMO_ATTR_X, objectID.c_str(), parsedOk, false);
        // check that X attribute is valid
        if (!canParse<double>(x)) {
            WRITE_WARNING("Format of essential " + attrProperties.getDescription() + " attribute '" + toString(SUMO_ATTR_X) + "' of " +
                            warningMessage +  " is invalid; Cannot be parsed to float; " + tagProperties.getTagStr() + " cannot be created");
            // abort parsing (and creation) of element
            return false;
        }
    } else {
        WRITE_WARNING("Essential " + attrProperties.getDescription() + " attribute '" + toString(SUMO_ATTR_X) + "' of " +
                        warningMessage +  " is missing; " + tagProperties.getTagStr() + " cannot be created");
        // abort parsing (and creation) of element
        return false;
    }
    if (attrs.hasAttribute(SUMO_ATTR_Y)) {
        y = attrs.get<std::string>(SUMO_ATTR_Y, objectID.c_str(), parsedOk, false);
        // check that X attribute is valid
        if (!canParse<double>(y)) {
            WRITE_WARNING("Format of essential " + attrProperties.getDescription() + " attribute '" + toString(SUMO_ATTR_Y) + "' of " +
                            warningMessage + " is invalid; Cannot be parsed to float; " + tagProperties.getTagStr() + " cannot be created");
            // abort parsing (and creation) of element
            return false;
        }
    } else {
        WRITE_WARNING("Essential " + attrProperties.getDescription() + " attribute '" + toString(SUMO_ATTR_Y) + "' of " +
                        warningMessage +  " is missing; " + tagProperties.getTagStr() + " cannot be created");
        // abort parsing (and creation) of element
        return false;
    }
    // Z attribute is optional
    if (attrs.hasAttribute(SUMO_ATTR_Z)) {
        z = attrs.get<std::string>(SUMO_ATTR_Z, objectID.c_str(), parsedOk, false);
        // check that Z attribute is valid
        if (!canParse<double>(z)) {
            WRITE_WARNING("Format of optional " + attrProperties.getDescription() + " attribute '" + toString(SUMO_ATTR_Z) + "' of " +
                            warningMessage + " is invalid; Cannot be parsed to float; " + tagProperties.getTagStr() + " cannot be created");
            // leave Z attribute empty
            z.clear();
        }
    }
    // create Position attribute using parsed coordinates X, Y and, optionally, Z
    if (z.empty()) {
        parsedAttribute = x + "," + y;
    } else {
        parsedAttribute = x + "," + y + "," + z;
    }
    // continue creation of element
    return true;
}
/****************************************************************************/
