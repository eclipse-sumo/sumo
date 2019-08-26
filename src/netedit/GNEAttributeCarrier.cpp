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
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomConvHelper.h>
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
const size_t GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES = 128;
const double GNEAttributeCarrier::INVALID_POSITION(-1000000);


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAttributeCarrier::AttributeProperties - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrier::AttributeProperties::AttributeProperties() :
    myAttribute(SUMO_ATTR_NOTHING),
    myTagPropertyParent(nullptr),
    myAttrStr(toString(SUMO_ATTR_NOTHING)),
    myAttributeProperty(ATTRPROPERTY_STRING),
    myDefinition(""),
    myDefaultValue(""),
    myAttrSynonym(SUMO_ATTR_NOTHING),
    myMinimumRange(0),
    myMaximumRange(0) {}


GNEAttributeCarrier::AttributeProperties::AttributeProperties(const SumoXMLAttr attribute, const int attributeProperty, const std::string& definition, std::string defaultValue) :
    myAttribute(attribute),
    myTagPropertyParent(nullptr),
    myAttrStr(toString(attribute)),
    myAttributeProperty(attributeProperty),
    myDefinition(definition),
    myDefaultValue(defaultValue),
    myAttrSynonym(SUMO_ATTR_NOTHING),
    myMinimumRange(0),
    myMaximumRange(0) {
    // empty definition aren't valid
    if (definition.empty()) {
        throw FormatException("Missing definition for AttributeProperty '" + toString(attribute) + "'");
    }
    // if default value isn't empty, but attribute doesn't support default values, throw exception.
    if (!defaultValue.empty() && !(attributeProperty & ATTRPROPERTY_DEFAULTVALUESTATIC)) {
        throw FormatException("AttributeProperty for '" + toString(attribute) + "' doesn't support default values");
    }
    // default value cannot be static and mutables at the same time
    if ((attributeProperty & ATTRPROPERTY_DEFAULTVALUESTATIC) && (attributeProperty & ATTRPROPERTY_DEFAULTVALUEMUTABLE)) {
        throw FormatException("Default value for attribute '" + toString(attribute) + "' cannot be static and mutable at the same time");
    }
    // Attributes that can write optionally their values in XML must have either a static or a mutable efault value
    if ((attributeProperty & ATTRPROPERTY_WRITEXMLOPTIONAL) && !((attributeProperty & ATTRPROPERTY_DEFAULTVALUESTATIC) || (attributeProperty & ATTRPROPERTY_DEFAULTVALUEMUTABLE))) {
        throw FormatException("Attribute '" + toString(attribute) + "' requieres a either static or mutable default value");
    }
}


GNEAttributeCarrier::AttributeProperties::~AttributeProperties() {}


void
GNEAttributeCarrier::AttributeProperties::checkAttributeIntegrity() {
    // check that positive attributes correspond only to a int, floats or SUMOTimes
    if (isPositive() && !(isInt() || isFloat() || isSUMOTime())) {
        throw FormatException("Only int, floats or SUMOTimes can be positive");
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
GNEAttributeCarrier::AttributeProperties::setSynonym(const SumoXMLAttr synonym) {
    if (hasAttrSynonym()) {
        myAttrSynonym = synonym;
    } else {
        throw FormatException("AttributeProperty doesn't support synonyms");
    }
}


void
GNEAttributeCarrier::AttributeProperties::setRange(const double minimum, const double maximum) {
    if (hasAttrRange()) {
        myMinimumRange = minimum;
        myMaximumRange = maximum;
    } else {
        throw FormatException("AttributeProperty doesn't support ranges");
    }
}


void
GNEAttributeCarrier::AttributeProperties::setTagPropertyParent(TagProperties* tagPropertyParent) {
    myTagPropertyParent = tagPropertyParent;
}


SumoXMLAttr
GNEAttributeCarrier::AttributeProperties::getAttr() const {
    return myAttribute;
}


const std::string&
GNEAttributeCarrier::AttributeProperties::getAttrStr() const {
    return myAttrStr;
}


const GNEAttributeCarrier::TagProperties&
GNEAttributeCarrier::AttributeProperties::getTagPropertyParent() const {
    return *myTagPropertyParent;
}


int
GNEAttributeCarrier::AttributeProperties::getPositionListed() const {
    for (auto i = myTagPropertyParent->begin(); i != myTagPropertyParent->end(); i++) {
        if (i->getAttr() == myAttribute) {
            return (int)(i - myTagPropertyParent->begin());
        }
    }
    throw ProcessError("Attribute wasn't found in myTagPropertyParent");
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
    if ((myAttributeProperty & ATTRPROPERTY_WRITEXMLOPTIONAL) != 0) {
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
    if ((myAttributeProperty & ATTRPROPERTY_SUMOTIME) != 0) {
        type = "SUMOTime";
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
GNEAttributeCarrier::AttributeProperties::hasStaticDefaultValue() const {
    return (myAttributeProperty & ATTRPROPERTY_DEFAULTVALUESTATIC) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::hasMutableDefaultValue() const {
    return (myAttributeProperty & ATTRPROPERTY_DEFAULTVALUEMUTABLE) != 0;
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
GNEAttributeCarrier::AttributeProperties::isSUMOTime() const {
    return (myAttributeProperty & ATTRPROPERTY_SUMOTIME) != 0;
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
    return (myAttributeProperty & (ATTRPROPERTY_INT | ATTRPROPERTY_FLOAT | ATTRPROPERTY_SUMOTIME)) != 0;
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
GNEAttributeCarrier::AttributeProperties::isWriteXMLOptional() const {
    return (myAttributeProperty & ATTRPROPERTY_WRITEXMLOPTIONAL) != 0;
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


bool
GNEAttributeCarrier::AttributeProperties::requiereUpdateGeometry() const {
    return (myAttributeProperty & ATTRPROPERTY_UPDATEGEOMETRY) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isOptional() const {
    return (myAttributeProperty & ATTRPROPERTY_OPTIONAL) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isComplex() const {
    return (myAttributeProperty & ATTRPROPERTY_COMPLEX) != 0;
}


bool
GNEAttributeCarrier::AttributeProperties::isEnablitable() const {
    return (myAttributeProperty & ATTRPROPERTY_ENABLITABLE) != 0;
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
        throw ProcessError("element must be at leas netElement, additional, TAZ, shape or demandElement");
    }
    // check that element only is netElement, Additional, or shape at the same time
    if ((isNetElement() + isAdditional() + isShape() + isTAZ() + isDemandElement()) > 1) {
        throw ProcessError("element can be only a netElement, additional, shape or demandElement at the same time");
    }
    // if element can mask the start and end position, check that bot attributes exist
    if (canMaskStartEndPos() && (!hasAttribute(SUMO_ATTR_STARTPOS) || !hasAttribute(SUMO_ATTR_ENDPOS))) {
        throw ProcessError("If attribute mask the start and end position, bot attribute has to be defined");
    }
    // check that synonym tag isn't nothing
    if (hasTagSynonym() && (myTagSynonym == SUMO_TAG_NOTHING)) {
        throw FormatException("Synonym tag cannot be nothing");
    }
    // check that synonym was defined
    if (!hasTagSynonym() && (myTagSynonym != SUMO_TAG_NOTHING)) {
        throw FormatException("Tag doesn't support synonyms");
    }
    // check integrity of all attributes
    for (auto i : myAttributeProperties) {
        i.checkAttributeIntegrity();
        // check that if attribute is combinable, own a combination of Allow/disallow attibute
        if (i.isCombinable()) {
            if ((i.getAttr() != SUMO_ATTR_ALLOW) && (i.getAttr() != SUMO_ATTR_DISALLOW)) {
                throw ProcessError("Attributes aren't combinables");
            } else if ((i.getAttr() == SUMO_ATTR_ALLOW) && !hasAttribute(SUMO_ATTR_DISALLOW)) {
                throw ProcessError("allow need a disallow attribute in the same tag");
            } else if ((i.getAttr() == SUMO_ATTR_DISALLOW) && !hasAttribute(SUMO_ATTR_ALLOW)) {
                throw ProcessError("disallow need an allow attribute in the same tag");
            }
        }
    }
}


const std::string&
GNEAttributeCarrier::TagProperties::getDefaultValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& i : myAttributeProperties) {
        if (i.getAttr() == attr) {
            if (!i.hasStaticDefaultValue()) {
                throw ProcessError("attribute '" + i.getAttrStr() + "' doesn't have a default value");
            } else {
                return i.getDefaultValue();
            }
        }
    }
    throw ProcessError("Attribute '" + toString(attr) + "' not defined");
}


void
GNEAttributeCarrier::TagProperties::addAttribute(const AttributeProperties& attributeProperty) {
    if (isAttributeDeprecated(attributeProperty.getAttr())) {
        throw ProcessError("Attribute '" + attributeProperty.getAttrStr() + "' is deprecated and cannot be inserted");
    } else if ((myAttributeProperties.size() + 1) >= MAXNUMBEROFATTRIBUTES) {
        throw ProcessError("Maximum number of attributes for tag " + attributeProperty.getAttrStr() + " exceeded");
    } else {
        // Check that attribute wasn't already inserted
        for (auto i : myAttributeProperties) {
            if (i.getAttr() == attributeProperty.getAttr()) {
                throw ProcessError("Attribute '" + attributeProperty.getAttrStr() + "' already inserted");
            }
        }
        // insert AttributeProperties in vector
        myAttributeProperties.push_back(attributeProperty);
        myAttributeProperties.back().setTagPropertyParent(this);
    }
}


void
GNEAttributeCarrier::TagProperties::addDeprecatedAttribute(SumoXMLAttr attr) {
    // Check that attribute wasn't already inserted
    for (auto i : myAttributeProperties) {
        if (i.getAttr() == attr) {
            throw ProcessError("Attribute '" + toString(attr) + "' is deprecated but was inserted in list of attributes");
        }
    }
    // add it into myDeprecatedAttributes
    myDeprecatedAttributes.push_back(attr);
}


const GNEAttributeCarrier::AttributeProperties&
GNEAttributeCarrier::TagProperties::getAttributeProperties(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& i : myAttributeProperties) {
        if ((i.getAttr() == attr) || (i.hasAttrSynonym() && (i.getAttrSynonym() == attr))) {
            return i;
        }
    }
    // throw error if these attribute doesn't exist
    throw ProcessError("Attribute '" + toString(attr) + "' doesn't exist");
}


std::vector<GNEAttributeCarrier::AttributeProperties>::const_iterator
GNEAttributeCarrier::TagProperties::begin() const {
    return myAttributeProperties.begin();
}


std::vector<GNEAttributeCarrier::AttributeProperties>::const_iterator
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
    // iterate over attribute properties
    for (const auto& i : myAttributeProperties) {
        if (i.getAttr() == attr) {
            return true;
        }
    }
    return false;
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
GNEAttributeCarrier::TagProperties::isVehicleType() const {
    return (myTagType & TAGTYPE_VTYPE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isVehicle() const {
    return (myTagType & TAGTYPE_VEHICLE) != 0;
}

bool
GNEAttributeCarrier::TagProperties::isRoute() const {
    return (myTagType & TAGTYPE_ROUTE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isStop() const {
    return (myTagType & TAGTYPE_STOP) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isPerson() const {
    return (myTagType & TAGTYPE_PERSON) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isPersonPlan() const {
    return (myTagType & TAGTYPE_PERSONPLAN) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isPersonTrip() const {
    return (myTagType & TAGTYPE_PERSONTRIP) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isWalk() const {
    return (myTagType & TAGTYPE_WALK) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isRide() const {
    return (myTagType & TAGTYPE_RIDE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::isPersonStop() const {
    return (myTagType & TAGTYPE_PERSONSTOP) != 0;
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
GNEAttributeCarrier::TagProperties::hasMinimumNumberOfChildren() const {
    return (myTagProperty & TAGPROPERTY_MINIMUMCHILDREN) != 0;
}


bool
GNEAttributeCarrier::TagProperties::hasGenericParameters() const {
    // note: By default all Tags supports generic parameters, except Tags with "TAGPROPERTY_NOGENERICPARAMETERS"
    return (myTagProperty & TAGPROPERTY_NOGENERICPARAMETERS) == 0;
}


bool
GNEAttributeCarrier::TagProperties::isPlacedInRTree() const {
    return (myTagProperty & TAGPROPERTY_RTREE) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBeSortedManually() const {
    return (myTagProperty & TAGPROPERTY_SORTINGCHILDREN) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canBeReparent() const {
    return (myTagProperty & TAGPROPERTY_REPARENT) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canAutomaticSortChildren() const {
    return (myTagProperty & TAGPROPERTY_AUTOMATICSORTING) != 0;
}


bool
GNEAttributeCarrier::TagProperties::canWriteChildrenSeparate() const {
    return (myTagProperty & TAGPROPERTY_WRITECHILDRENSEPARATE) != 0;
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

GNEAttributeCarrier::GNEAttributeCarrier(const SumoXMLTag tag) :
    myTagProperty(getTagProperties(tag)),
    mySelected(false) {
}


GNEAttributeCarrier::~GNEAttributeCarrier() {}


template<> int
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toInt(string);
}


template<> double
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toDouble(string);
}


template<> SUMOTime
GNEAttributeCarrier::parse(const std::string& string) {
    SUMOTime time = string2time(string);
    if (time < 0) {
        throw NumberFormatException("SUMOTIME cannot be negative");
    } else {
        return time;
    }
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


template<> std::set<std::string>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> vectorString = StringTokenizer(string).getVector();
    std::set<std::string> solution;
    for (const auto& i : vectorString) {
        solution.insert(i);
    }
    return solution;
}


template<> std::vector<int>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<int> parsedIntValues;
    for (const auto& i : parsedValues) {
        parsedIntValues.push_back(parse<int>(i));
    }
    return parsedIntValues;
}


template<> std::vector<double>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<double> parsedDoubleValues;
    for (const auto& i : parsedValues) {
        parsedDoubleValues.push_back(parse<double>(i));
    }
    return parsedDoubleValues;
}


template<> std::vector<bool>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<bool> parsedBoolValues;
    for (const auto& i : parsedValues) {
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
    for (const auto& i : edgeIds) {
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
    for (const auto& i : laneIds) {
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
    for (const auto& i : ACs) {
        edgeIDs.push_back(i->getID());
    }
    return joinToString(edgeIDs, " ");
}


template<> std::string
GNEAttributeCarrier::parseIDs(const std::vector<GNELane*>& ACs) {
    // obtain ID's of lanes and return their join
    std::vector<std::string> laneIDs;
    for (const auto& i : ACs) {
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
    std::vector<SumoXMLTag> allowedTags;
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    if (tagPropertyCategory & TAGTYPE_NETELEMENT) {
        // fill netElements tags
        for (const auto& i : myTagProperties) {
            if (i.second.isNetElement() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_ADDITIONAL) {
        // fill additional tags
        for (const auto& i : myTagProperties) {
            if (i.second.isAdditional() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_SHAPE) {
        // fill shape tags
        for (const auto& i : myTagProperties) {
            if (i.second.isShape() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_TAZ) {
        // fill taz tags
        for (const auto& i : myTagProperties) {
            if (i.second.isTAZ() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_DEMANDELEMENT) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isDemandElement() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_ROUTE) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isRoute() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_VEHICLE) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isVehicle() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_STOP) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isStop() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_PERSON) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isPerson() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_PERSONPLAN) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isPersonPlan() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_PERSONTRIP) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isPersonTrip() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_WALK) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isWalk() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_RIDE) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isRide() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    if (tagPropertyCategory & TAGTYPE_PERSONSTOP) {
        // fill demand tags
        for (const auto& i : myTagProperties) {
            if (i.second.isPersonStop() && (!onlyDrawables || i.second.isDrawable())) {
                allowedTags.push_back(i.first);
            }
        }
    }
    return allowedTags;
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

// ===========================================================================
// private
// ===========================================================================

void
GNEAttributeCarrier::fillAttributeCarriers() {
    // fill all groups of ACs
    fillNetElements();
    fillAdditionals();
    fillShapes();
    fillDemandElements();
    fillVehicleElements();
    fillStopElements();
    fillPersonElements();
    fillPersonStopElements();
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
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_EDGE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of the edge");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of a node within the nodes-file the edge shall start at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of a node within the nodes-file the edge shall end at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The maximum speed allowed on the edge in m/s");
        toString(oc.getFloat("default.speed"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PRIORITY,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The priority of the edge");
        toString(oc.getInt("default.priority"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NUMLANES,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The number of lanes of the edge");
        toString(oc.getInt("default.lanenumber"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The name of a type within the SUMO edge type file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE,
                                           "Explicitly allows the given vehicle classes (not given will be not allowed)",
                                           "all");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE,
                                           "Explicitly disallows the given vehicle classes (not given will be allowed)");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SHAPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "If the shape is given it should start and end with the positions of the from-node and to-node");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The length of the edge in meter");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPREADTYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Lane width for all lanes of this edge in meters (used for visualization)",
                                           "right");
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::LaneSpreadFunctions.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "street name (need not be unique, used for visualization)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Lane width for all lanes of this edge in meters (used for visualization)",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDOFFSET,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Move the stop line back from the intersection by the given amount",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(GNE_ATTR_SHAPE_START,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY, // virtual attribute used to define an endPoint
                                           "Custom position in which shape start (by default position of junction from)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(GNE_ATTR_SHAPE_END,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY, // virtual attribute from to define an endPoint
                                           "Custom position in which shape end (by default position of junction from)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(GNE_ATTR_BIDIR,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_NONEDITABLE, // virtual attribute to check of this edge is part of a bidirectional railway (cannot be edited)
                                           "Show if edge is bidireccional",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISTANCE,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UNIQUE,
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_JUNCTION;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_JUNCTION);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of the node");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_POSITION | ATTRPROPERTY_UPDATEGEOMETRY, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                                           "The x-y-z position of the node on the plane in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "An optional type for the node");
        attrProperty.setDiscreteValues(nodeTypes);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SHAPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "A custom shape for that node");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_RADIUS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Optional turning radius (for all corners) for that node in meters",
                                           "1.5");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_KEEP_CLEAR,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Whether the junction-blocking-heuristic should be activated at this node",
                                           "1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_RIGHT_OF_WAY,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "How to compute right of way rules at this node",
                                           SUMOXMLDefinitions::RightOfWayValues.getString(RIGHT_OF_WAY_DEFAULT));
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::RightOfWayValues.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRINGE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Whether this junction is at the fringe of the network",
                                           SUMOXMLDefinitions::FringeTypeValues.getString(FRINGE_TYPE_DEFAULT));
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::FringeTypeValues.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLTYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "An optional type for the traffic light algorithm");
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::TrafficLightTypes.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "An optional id for the traffic light program");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
                                           "ID of lane (Automatic, non editable)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_INDEX,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Speed in meters per second",
                                           "13.89");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE,
                                           "Explicitly allows the given vehicle classes (not given will be not allowed)",
                                           "all");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE,
                                           "Explicitly disallows the given vehicle classes (not given will be allowed)");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Width in meters (used for visualization)",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDOFFSET,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Move the stop line back from the intersection by the given amount",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ACCELERATION,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Enable or disable lane as acceleration lane",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "If the shape is given it overrides the computation based on edge shape");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CROSSING;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_CROSSING, SUMO_TAG_JUNCTION);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE,
                                           "The ID of Crossing");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The (road) edges which are crossed");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PRIORITY,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections)",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The width of the crossings",
                                           toString(OptionsCont::getOptions().getFloat("default.crossing-width")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLLINKINDEX,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "sets the tls-index for this crossing",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLLINKINDEX2,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "sets the opposite-direction tls-index for this crossing",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Overrids default shape of pedestrian crossing");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CONNECTION;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_NETELEMENT, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_CONNECTION, SUMO_TAG_EDGE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the vehicles leave");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the vehicles may reach when leaving 'from'");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FROM_LANE,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "the lane index of the incoming lane (numbers starting with 0)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO_LANE,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "the lane index of the outgoing lane (numbers starting with 0)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PASS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "if set, vehicles which pass this (lane-2-lane) connection) will not wait",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_KEEP_CLEAR,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CONTPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "If set to a more than 0 value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection",
                                           toString(NBEdge::UNSPECIFIED_CONTPOS));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_UNCONTROLLED,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "If set to true, This connection will not be TLS-controlled despite its node being controlled",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VISIBILITY_DISTANCE,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Vision distance between vehicles",
                                           toString(NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TLLINKINDEX,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "sets the distance to the connection at which all relevant foes are visible",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "sets custom speed limit for the connection",
                                           toString(NBEdge::UNSPECIFIED_SPEED));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "sets custom shape for the connection");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DIR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_NONEDITABLE,
                                           "turning direction for this connection (computed)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STATE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_NONEDITABLE,
                                           "link state for this connection (computed)");
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
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_CONTAINERSTOP, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of bus stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the lane the bus stop shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The begin position on the lane (the lower position on the lane) in meters");

        myTagProperties[currentTag].addAttribute(attrProperty);
        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LINES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PERSON_CAPACITY,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ACCESS;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_ACCESS, SUMO_TAG_BUS_STOP);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the lane the stop access shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the lane (the lower position on the lane) in meters",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The walking length of the access in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_CONTAINER_STOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_CONTAINERSTOP, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of container stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the lane the container stop shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The begin position on the lane (the lower position on the lane) in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LINES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CHARGING_STATION;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_CHARGINGSTATION, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of charging station");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Lane of the charging station location");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Begin position in the specified lane");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "End position in the specified lane");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CHARGINGPOWER,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Charging power in W",
                                           "22000.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EFFICIENCY,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_RANGE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Charging efficiency [0,1]",
                                           "0.95");
        attrProperty.setRange(0, 1);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CHARGEINTRANSIT,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CHARGEDELAY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins",
                                           "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_PARKING_AREA;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_STOPPINGPLACE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_RTREE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKSTARTENDPOS, ICON_PARKINGAREA, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of ParkingArea");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the lane the Parking Area shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The begin position on the lane (the lower position on the lane) in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROADSIDE_CAPACITY,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           " The number of parking spaces for road-side parking",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ONROAD,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, vehicles will park on the road lane and thereby reducing capacity",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The width of the road-side parking spaces",
                                           "3.20");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The length of the road-side parking spaces. By default (endPos - startPos) / roadsideCapacity");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise",
                                           "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_PARKING_SPACE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_RTREE | TAGPROPERTY_MASKXYZPOSITION | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_REPARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_PARKINGSPACE, SUMO_TAG_PARKING_AREA);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_POSITION | ATTRPROPERTY_UPDATEGEOMETRY, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                                           "The x-y-z position of the parking vehicle on the plane");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The width of the road-side parking spaces",
                                           "3.20");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The length of the road-side parking spaces",
                                           "5.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The angle of the road-side parking spaces relative to the lane angle, positive means clockwise",
                                           "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_E1DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_E1, SUMO_TAG_LANE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of E1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The aggregation period the values the detector collects shall be summed up",
                                           "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_E2DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_E2, SUMO_TAG_LANE);
        // set "file" as deprecated attribute
        myTagProperties[currentTag].addDeprecatedAttribute(SUMO_ATTR_CONT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of E2");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The length of the detector in meters",
                                           "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The aggregation period the values the detector collects shall be summed up",
                                           "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s",
                                           "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m",
                                           "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_E2DETECTOR_MULTILANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SYNONYM | TAGPROPERTY_BLOCKMOVEMENT, ICON_E2, SUMO_TAG_LANE, SUMO_TAG_E2DETECTOR);
        // set "file" as deprecated attribute
        myTagProperties[currentTag].addDeprecatedAttribute(SUMO_ATTR_CONT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of Multilane E2");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_SECUENCIAL | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The list of secuencial lane ids in which the detector shall be laid on");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The end position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The aggregation period the values the detector collects shall be summed up",
                                           "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s",
                                           "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m",
                                           "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_E3DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_RTREE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MINIMUMCHILDREN | TAGPROPERTY_AUTOMATICSORTING, ICON_E3);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of E3");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "X-Y position of detector in editor (Only used in NETEDIT)",
                                           "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The aggregation period the values the detector collects shall be summed up",
                                           "900.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) in s",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s",
                                           "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_DET_ENTRY;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_REPARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_E3ENTRY, SUMO_TAG_E3DETECTOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_DET_EXIT;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_REPARENT | TAGPROPERTY_BLOCKMOVEMENT, ICON_E3EXIT, SUMO_TAG_E3DETECTOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the lane the detector shall be laid on in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_INSTANT_INDUCTION_LOOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL | TAGTYPE_DETECTOR, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_E1INSTANT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of Instant Induction Loop (E1Instant)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the lane the detector shall be laid on. The lane must be a part of the network used");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The path to the output file");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Space separated list of vehicle type ids to consider");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_VSS;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_RTREE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_DIALOG, ICON_VARIABLESPEEDSIGN);
        // set "file" as deprecated attribute
        myTagProperties[currentTag].addDeprecatedAttribute(SUMO_ATTR_FILE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The id of Variable Speed Signal");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "X-Y position of detector in editor (Only used in NETEDIT)",
                                           "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "list of lanes of Variable Speed Sign");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_STEP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_VSSSTEP, SUMO_TAG_VSS);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_TIME,
                                           ATTRPROPERTY_SUMOTIME,
                                           "Time");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Speed",
                                           "13.89");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CALIBRATOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_DIALOG, ICON_CALIBRATOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of Calibrator");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of edge in the simulation network");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position of the calibrator on the specified lane",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The aggregation interval in which to calibrate the flows. Default is step-length",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTEPROBE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The id of the routeProbe element from which to determine the route distribution for generated vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_OUTPUT,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The output file for writing calibrator information or NULL");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_LANECALIBRATOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_SYNONYM | TAGPROPERTY_DIALOG, ICON_CALIBRATOR, SUMO_TAG_NOTHING, SUMO_TAG_CALIBRATOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of Calibrator");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of lane in the simulation network");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position of the calibrator on the specified lane",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The aggregation interval in which to calibrate the flows. Default is step-length",
                                           "100.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTEPROBE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The id of the routeProbe element from which to determine the route distribution for generated vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_OUTPUT,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The output file for writing calibrator information or NULL");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_FLOW_CALIBRATOR;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_FLOW, SUMO_TAG_CALIBRATOR);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the vehicle type to use for this " + toString(currentTag),
                                           DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the route the vehicle shall drive along");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common vehicle attributes
        fillCommonVehicleAttributes(currentTag);

        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "First " + toString(currentTag) + " departure time",
                                           "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_END,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "End of departure interval",
                                           "3600.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VEHSPERHOUR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_ENABLITABLE,
                                           "Number of " + toString(currentTag) + "s per hour, equally spaced");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEED,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_ENABLITABLE,
                                           "Speed of " + toString(currentTag) + "s");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_REROUTER;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_RTREE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_DIALOG | TAGPROPERTY_WRITECHILDRENSEPARATE, ICON_REROUTER);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of Rerouter");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "An edge id or a list of edge ids where vehicles shall be rerouted");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "X,Y position in editor (Only used in NETEDIT)",
                                           "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The path to the definition file (alternatively, the intervals may defined as children of the rerouter)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_PROBABILITY | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The probability for vehicle rerouting (0-1)",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The waiting time threshold (in s) that must be reached to activate rerouting (default -1 which disables the threshold)",
                                           "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The list of vehicle types that shall be affected by this rerouter (empty to affect all types)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_OFF,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
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
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Begin",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_END,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
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
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Edge ID");
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "allowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "disallowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CLOSING_LANE_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_CLOSINGLANEREROUTE, SUMO_TAG_INTERVAL);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Lane ID");
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "allowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_DISALLOW,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_COMBINABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "disallowed vehicles");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_DEST_PROB_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_PARENT, ICON_DESTPROBREROUTE, SUMO_TAG_INTERVAL);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Edge ID");
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
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
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "SUMO Probability",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VISIBLE,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_DEFAULTVALUESTATIC,
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
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_SYNONYM | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Route");
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "SUMO Probability",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ROUTEPROBE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_ROUTEPROBE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of RouteProbe");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of an edge in the simulation network");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FREQUENCY,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The frequency in which to report the distribution",
                                           "3600");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The file for generated output");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The time at which to start generating output",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VAPORIZER;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_ADDITIONAL, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_VAPORIZER);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Edge in which vaporizer is placed");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "Start Time",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_END,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "End Time",
                                           "3600.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_NAME,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Name of " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TAZ;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_TAZ, TAGPROPERTY_DRAWABLE | TAGPROPERTY_RTREE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_BLOCKSHAPE | TAGPROPERTY_AUTOMATICSORTING, ICON_TAZ);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of the TAZ");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SHAPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The shape of the TAZ");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
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
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_SYNONYM | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of edge in the simulation network");
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WEIGHT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
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
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_NONEDITABLE | ATTRPROPERTY_SYNONYM | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of edge in the simulation network");
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WEIGHT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
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
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_SHAPE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_BLOCKSHAPE | TAGPROPERTY_CLOSESHAPE | TAGPROPERTY_GEOSHAPE, ICON_LOCATEPOLY /* temporal */);
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
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The RGBA color with which the polygon shall be displayed",
                                           "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FILL,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "An information whether the polygon shall be filled",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LINEWIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The default line width for drawing an unfilled polygon",
                                           "1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LAYER,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The layer in which the polygon lies",
                                           toString(Shape::DEFAULT_LAYER));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "A typename for the polygon",
                                           toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "A bitmap to use for rendering this polygon",
                                           toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_RELATIVEPATH,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Enable or disable use image file as a relative path",
                                           toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Angle of rendered image in degree",
                                           toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_POI;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_SHAPE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_RTREE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT | TAGPROPERTY_MASKXYZPOSITION | TAGPROPERTY_GEOPOSITION, ICON_LOCATEPOI /* temporal */);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of the POI");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_POSITION | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                                           "The position in view");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The color with which the poi shall be displayed",
                                           "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "A typename for the poi",
                                           toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LAYER,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The layer of the poi for drawing and selecting",
                                           toString(Shape::DEFAULT_LAYER_POI));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Width of rendered image in meters",
                                           toString(Shape::DEFAULT_IMG_WIDTH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HEIGHT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Height of rendered image in meters",
                                           toString(Shape::DEFAULT_IMG_HEIGHT));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "A bitmap to use for rendering this poi",
                                           toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_RELATIVEPATH,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Enable or disable use image file as a relative path",
                                           toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Angle of rendered image in degree",
                                           toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_POILANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_SHAPE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_BLOCKMOVEMENT, ICON_LOCATEPOI /* temporal */);
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
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The position on the named lane or in the net in meters at which the poi is located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_POSITION_LAT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The lateral offset on the named lane at which the poi is located at",
                                           "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The color with which the poi shall be displayed",
                                           "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "A typename for the poi",
                                           toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LAYER,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The layer of the poi for drawing and selecting",
                                           toString(Shape::DEFAULT_LAYER_POI));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Width of rendered image in meters",
                                           toString(Shape::DEFAULT_IMG_WIDTH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HEIGHT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Height of rendered image in meters",
                                           toString(Shape::DEFAULT_IMG_HEIGHT));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "A bitmap to use for rendering this poi",
                                           toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_RELATIVEPATH,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Enable or disable use image file as a relative path",
                                           toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ANGLE,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_ANGLE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Angle of rendered image in degree",
                                           toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillDemandElements() {
    // first VClass separate between vehicles and persons
    std::vector<std::string> vClassesVehicles, vClassesPersons;
    auto vClasses = SumoVehicleClassStrings.getStrings();
    for (const auto& i : vClasses) {
        if (i == SumoVehicleClassStrings.getString(SVC_PEDESTRIAN)) {
            vClassesPersons.push_back(i);
        } else {
            vClassesVehicles.push_back(i);
        }
    }
    // declare empty AttributeProperties
    AttributeProperties attrProperty;

    // fill demand elements
    SumoXMLTag currentTag = SUMO_TAG_ROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_ROUTE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT, ICON_ROUTE);

        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of Route");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The edges the vehicle shall drive along, given as their ids, separated using spaces");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "This route's color",
                                           "yellow");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_EMBEDDEDROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_ROUTE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT, ICON_ROUTE);

        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The edges the vehicle shall drive along, given as their ids, separated using spaces");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "This route's color",
                                           "yellow");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VTYPE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VTYPE, 0, ICON_VTYPE);

        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of VehicleType");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VCLASS,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "An abstract vehicle class",
                                           "passenger");
        attrProperty.setDiscreteValues(vClassesVehicles);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "This vehicle type's color",
                                           "");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The vehicle's netto-length (length) [m]");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MINGAP,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Empty space after leader [m]");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MAXSPEED,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The vehicle's maximum velocity [m/s]");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEEDFACTOR,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The vehicles expected multiplicator for lane speed limits");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_SPEEDDEV,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The deviation of the speedFactor");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_EMISSIONCLASS,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "An abstract emission class");
        attrProperty.setDiscreteValues(PollutantsInterface::getAllClassesStr());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_GUISHAPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "How this vehicle is rendered");
        attrProperty.setDiscreteValues(SumoVehicleShapeStrings.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The vehicle's width [m] (only used for drawing)",
                                           "1.8");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HEIGHT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The vehicle's height [m] (only used for drawing)",
                                           "1.5");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LANE_CHANGE_MODEL,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The model used for changing lanes",
                                           "default");
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::LaneChangeModels.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CAR_FOLLOW_MODEL,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The model used for car following",
                                           "Krauss");
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::CarFollowModels.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PERSON_CAPACITY,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The number of persons (excluding an autonomous driver) the vehicle can transport");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_CAPACITY,
                                           ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The number of containers the vehicle can transport");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BOARDING_DURATION,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The time required by a person to board the vehicle",
                                           "0.50");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LOADING_DURATION,
                                           ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The time required to load a container onto the vehicle",
                                           "90.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LATALIGNMENT,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The preferred lateral alignment when using the sublane-model",
                                           "center");
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::LateralAlignments.getStrings());
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MINGAP_LAT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The minimum lateral gap at a speed difference of 50km/h when using the sublane-model",
                                           "0.12");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MAXSPEED_LAT,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The maximum lateral speed when using the sublane-model",
                                           "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ACTIONSTEPLENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The interval length for which vehicle performs its decision logic (acceleration and lane-changing)",
                                           toString(OptionsCont::getOptions().getFloat("default.action-step-length")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_PROB,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "The probability when being added to a distribution without an explicit probability",
                                           toString(DEFAULT_VEH_PROB));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_HASDRIVERSTATE,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "Whether vehicles of this type are equipped with a driver (i.e. MSDriverState))",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_OSGFILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "3D model file for this class",
                                           "");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CARRIAGE_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "Carriage lengths");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LOCOMOTIVE_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "Locomotive lengths");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_CARRIAGE_GAP,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                           "GAP between carriages",
                                           "1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill VType Car Following Model Values (implemented in a separated function to improve code legibility)
        fillCarFollowingModelAttributes(currentTag);

        // fill VType Junction Model Parameters (implemented in a separated function to improve code legibility)
        fillJunctionModelAttributes(currentTag);

        // fill VType Lane Change Model Parameters (implemented in a separated function to improve code legibility)
        fillLaneChangingModelAttributes(currentTag);
    }
    currentTag = SUMO_TAG_PTYPE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VTYPE, TAGPROPERTY_SYNONYM, ICON_PTYPE, SUMO_TAG_NOTHING, SUMO_TAG_VTYPE);

        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The id of PersonType");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VCLASS,
                                           ATTRPROPERTY_VCLASS | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "An abstract person class",
                                           "pedestrian");
        attrProperty.setDiscreteValues(vClassesPersons);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "This person type's color",
                                           "");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_WIDTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The person's width [m] (only used for drawing)");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LENGTH,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The person's netto-length (length) [m]");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MINGAP,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Empty space after leader [m]");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MAXSPEED,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUEMUTABLE | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "The person's maximum velocity [m/s]");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_JM_DRIVE_AFTER_RED_TIME,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "This value causes persons to violate a red light if the duration of the red phase is lower than the given threshold.",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_IMGFILE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_FILENAME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Image file for rendering persons of this type (should be grayscale to allow functional coloring)");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillVehicleElements() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = SUMO_TAG_VEHICLE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_VEHICLE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The name of the " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The id of the vehicle type to use for this " + toString(currentTag),
                                           DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the route the " + toString(currentTag) + " shall drive along");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        attrProperty = AttributeProperties(SUMO_ATTR_DEPART,
                                           ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The time step at which the " + toString(currentTag) + " shall enter the network",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ROUTEFLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_SYNONYM, ICON_ROUTEFLOW, SUMO_TAG_NOTHING, SUMO_TAG_FLOW);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The name of the " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the " + toString(currentTag) + " type to use for this " + toString(currentTag),
                                           DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the route the " + toString(currentTag) + " shall drive along");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag);
    }
    currentTag = SUMO_TAG_TRIP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_TRIP);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The name of " + toString(currentTag) + "s that will be generated using this trip definition");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The id of the " + toString(currentTag) + " type to use for this " + toString(currentTag),
                                           DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " starts at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " ends at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VIA,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "List of intermediate edge ids which shall be part of the " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        attrProperty = AttributeProperties(SUMO_ATTR_DEPART,
                                           ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The departure time of the (first) " + toString(currentTag) + " which is generated using this " + toString(currentTag) + " definition",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_FLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_FLOW);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                           "The name of the " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the " + toString(currentTag) + " type to use for this " + toString(currentTag),
                                           DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " starts at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " ends at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VIA,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "List of intermediate edge ids which shall be part of the " + toString(currentTag));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag);
    }
    /* currently disabled. See #5259
    currentTag = SUMO_TAG_TRIP_TAZ;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_VEHICLE, TAGPROPERTY_DRAWABLE, ICON_TRIP);
    }
    */
}


void
GNEAttributeCarrier::fillStopElements() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // fill stops ACs
    SumoXMLTag currentTag = SUMO_TAG_STOP_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_STOP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_MASKSTARTENDPOS, ICON_STOPELEMENT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the lane the stop shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The begin position on the lane (the lower position on the lane) in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag);
    }
    currentTag = SUMO_TAG_STOP_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_STOP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_STOPELEMENT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_BUS_STOP,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "BusStop associated with this stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag);
    }
    currentTag = SUMO_TAG_STOP_CONTAINERSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_STOP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_STOPELEMENT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_STOP,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "ContainerStop associated with this stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag);
    }
    currentTag = SUMO_TAG_STOP_CHARGINGSTATION;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_STOP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_STOPELEMENT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_CHARGING_STATION,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "ChargingStation associated with this stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag);
    }
    currentTag = SUMO_TAG_STOP_PARKINGAREA;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_STOP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_STOPELEMENT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_PARKING_AREA,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "ParkingArea associated with this stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag);
    }
}


void
GNEAttributeCarrier::fillPersonElements() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = SUMO_TAG_PERSON;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSON, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_PERSON);

        // add flow attributes
        fillCommonPersonAttributes(currentTag);

        // set specific attribute depart (note: Persons doesn't support triggered and containerTriggered values)
        attrProperty = AttributeProperties(SUMO_ATTR_DEPART,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "The time step at which the " + toString(currentTag) + " shall enter the network",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_PERSONFLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSON, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE, ICON_PERSONFLOW);

        // add flow attributes
        fillCommonPersonAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag);
    }
    currentTag = SUMO_TAG_PERSONTRIP_FROMTO;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_PERSONTRIP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_PERSONTRIP_FROMTO);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " starts at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " ends at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "List of possible vehicle types to take");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MODES,
                                           ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "List of possible traffic modes. Walking is always possible regardless of this value");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "arrival position on the destination edge",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_PERSONTRIP_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_PERSONTRIP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_PERSONTRIP_BUSSTOP);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " starts at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BUS_STOP,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Id of the destination " + toString(SUMO_TAG_BUS_STOP));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VTYPES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "List of possible vehicle types to take");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_MODES,
                                           ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "List of possible traffic modes. Walking is always possible regardless of this value");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_RIDE_FROMTO;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_RIDE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_RIDE_FROMTO);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " starts at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " ends at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LINES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "list of vehicle alternatives to take for the " + toString(currentTag),
                                           "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "arrival position on the destination edge",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_RIDE_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_RIDE, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_RIDE_BUSSTOP);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the edge the " + toString(currentTag) + " starts at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BUS_STOP,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Id of the destination " + toString(SUMO_TAG_BUS_STOP));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_LINES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                           "list of vehicle alternatives to take for the ride",
                                           "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_WALK_EDGES;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_WALK, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_WALK_EDGES);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_EDGES,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "id of the edges to walk");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Arrival position on the destination edge",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }

    currentTag = SUMO_TAG_WALK_ROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_WALK, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_WALK_ROUTE);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_ROUTE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The id of the route to walk");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Arrival position on the destination edge",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_WALK_FROMTO;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_WALK, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_WALK_FROMTO);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Id of the start edge");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_TO,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Id of the destination edge");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_VIA,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Ids of the intermediate edges");
        myTagProperties[currentTag].addAttribute(attrProperty);


        attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "Arrival position on the destination edge",
                                           "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }

    currentTag = SUMO_TAG_WALK_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONPLAN | TAGTYPE_WALK, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_PARENT | TAGPROPERTY_SORTINGCHILDREN, ICON_WALK_BUSSTOP);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_FROM,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Id of the start edge");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_BUS_STOP,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "Id of the destination " + toString(SUMO_TAG_BUS_STOP));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillPersonStopElements() {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = SUMO_TAG_PERSONSTOP_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONSTOP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_MASKSTARTENDPOS | TAGPROPERTY_SORTINGCHILDREN, ICON_STOPELEMENT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_LANE,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The name of the lane the stop shall be located at");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_STARTPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The begin position on the lane (the lower position on the lane) in meters");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_ENDPOS,
                                           ATTRPROPERTY_FLOAT | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = AttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                           ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                           "If set, no error will be reported if element is placed behind the lane. Instead,it will be placed 0.1 meters from the lanes end or at position 0.1, if the position was negative and larger than the lanes length after multiplication with - 1",
                                           "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag);
    }
    currentTag = SUMO_TAG_PERSONSTOP_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = TagProperties(currentTag, TAGTYPE_DEMANDELEMENT | TAGTYPE_PERSONSTOP, TAGPROPERTY_DRAWABLE | TAGPROPERTY_SELECTABLE | TAGPROPERTY_SORTINGCHILDREN, ICON_STOPELEMENT);
        // set values of attributes
        attrProperty = AttributeProperties(SUMO_ATTR_BUS_STOP,
                                           ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_UPDATEGEOMETRY,
                                           "BusStop associated with this stop");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag);
    }
}


void
GNEAttributeCarrier::fillCommonVehicleAttributes(SumoXMLTag currentTag) {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;

    attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                       "This " + toString(currentTag) + "'s color",
                                       "yellow");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_DEPARTLANE,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The lane on which the " + toString(currentTag) + " shall be inserted",
                                       "first");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
                                       "The position at which the " + toString(currentTag) + " shall enter the net",
                                       "base");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_DEPARTSPEED,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
                                       "The speed with which the " + toString(currentTag) + " shall enter the network",
                                       "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALLANE,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
                                       "The lane at which the " + toString(currentTag) + " shall leave the network",
                                       "current");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
                                       "The position at which the " + toString(currentTag) + " shall leave the network",
                                       "max");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALSPEED,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL /* ATTRPROPERTY_MULTIDISCRETE (Currently disabled) */,
                                       "The speed with which the " + toString(currentTag) + " shall leave the network",
                                       "current");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LINE,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "A string specifying the id of a public transport line which can be used when specifying person rides");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_PERSON_NUMBER,
                                       ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The number of occupied seats when the " + toString(currentTag) + " is inserted",
                                       "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_NUMBER,
                                       ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The number of occupied container places when the " + toString(currentTag) + " is inserted",
                                       "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS_LAT,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The lateral position on the departure lane at which the " + toString(currentTag) + " shall enter the net",
                                       "center");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_ARRIVALPOS_LAT,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The lateral position on the arrival lane at which the " + toString(currentTag) + " shall arrive",
                                       "center");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillCommonFlowAttributes(SumoXMLTag currentTag) {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;

    attrProperty = AttributeProperties(SUMO_ATTR_BEGIN,
                                       ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                       "First " + toString(currentTag) + " departure time",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_END,
                                       ATTRPROPERTY_SUMOTIME | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_ENABLITABLE,
                                       "End of departure interval",
                                       "3600.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_NUMBER,
                                       ATTRPROPERTY_INT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_ENABLITABLE,
                                       "probability for emitting a " + toString(currentTag) + " each second (not together with vehsPerHour or period)",
                                       "1800");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_VEHSPERHOUR,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_ENABLITABLE,
                                       "Number of " + toString(currentTag) + "s per hour, equally spaced (not together with period or probability)",
                                       "1800");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_PERIOD,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_ENABLITABLE,
                                       "Insert equally spaced " + toString(currentTag) + "s at that period (not together with vehsPerHour or probability)",
                                       "2");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_PROB,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_ENABLITABLE,
                                       "probability for emitting a " + toString(currentTag) + " each second (not together with vehsPerHour or period)",
                                       "0.5");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillCarFollowingModelAttributes(SumoXMLTag currentTag) {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;

    attrProperty = AttributeProperties(SUMO_ATTR_ACCEL,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The acceleration ability of vehicles of this type [m/s^2]",
                                       "2.60");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_DECEL,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The deceleration ability of vehicles of this type [m/s^2]",
                                       "4.50");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_APPARENTDECEL,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The apparent deceleration of the vehicle as used by the standard model [m/s^2]",
                                       "4.50");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_EMERGENCYDECEL,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The maximal physically possible deceleration for the vehicle [m/s^2]",
                                       "4.50");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_SIGMA,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_RANGE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "Car-following model parameter",
                                       "0.50");
    attrProperty.setRange(0, 1);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TAU,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "Car-following model parameter",
                                       "1.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TMP1,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "SKRAUSSX parameter 1",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TMP2,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "SKRAUSSX parameter 2",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TMP3,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "SKRAUSSX parameter 3",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TMP4,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "SKRAUSSX parameter 4",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TMP5,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "SKRAUSSX parameter 5",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_PWAGNER2009_TAULAST,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Peter Wagner 2009 parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_PWAGNER2009_APPROB,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Peter Wagner 2009 parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_IDMM_ADAPT_FACTOR,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "IDMM parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_IDMM_ADAPT_TIME,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "IDMM parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_WIEDEMANN_SECURITY,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Wiedemann parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_WIEDEMANN_ESTIMATION,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Wiedemann parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_COLLISION_MINGAP_FACTOR,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "MinGap factor parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_K,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "K parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);


    attrProperty = AttributeProperties(SUMO_ATTR_CF_KERNER_PHI,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Kerner Phi parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_IDM_DELTA,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "IDM Delta parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CF_IDM_STEPPING,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "IDM Stepping parameter",
                                       "");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TRAIN_TYPE,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DISCRETE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Train Types",
                                       "NGT400");
    attrProperty.setDiscreteValues(SUMOXMLDefinitions::TrainTypes.getStrings());
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillJunctionModelAttributes(SumoXMLTag currentTag) {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;
    attrProperty = AttributeProperties(SUMO_ATTR_JM_CROSSING_GAP,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Minimum distance to pedestrians that are walking towards the conflict point with the ego vehicle.",
                                       "10");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_IGNORE_KEEPCLEAR_TIME,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The accumulated waiting time after which a vehicle will drive onto an intersection even though this might cause jamming.",
                                       "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_DRIVE_AFTER_YELLOW_TIME,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "This value causes vehicles to violate a yellow light if the duration of the yellow phase is lower than the given threshold.",
                                       "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_DRIVE_AFTER_RED_TIME,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "This value causes vehicles to violate a red light if the duration of the red phase is lower than the given threshold.",
                                       "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_DRIVE_RED_SPEED,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "This value causes vehicles affected by jmDriveAfterRedTime to slow down when violating a red light.",
                                       "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_IGNORE_FOE_PROB,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "This value causes vehicles to ignore foe vehicles that have right-of-way with the given probability.",
                                       "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_IGNORE_FOE_SPEED,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "This value is used in conjunction with jmIgnoreFoeProb. Only vehicles with a speed below or equal to the given value may be ignored.",
                                       "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_SIGMA_MINOR,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "This value configures driving imperfection (dawdling) while passing a minor link.",
                                       "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_JM_TIMEGAP_MINOR,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "This value defines the minimum time gap when passing ahead of a prioritized vehicle. ",
                                       "1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_IMPATIENCE,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Willingess of drivers to impede vehicles with higher priority",
                                       "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillLaneChangingModelAttributes(SumoXMLTag currentTag) {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_STRATEGIC_PARAM,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The eagerness for performing strategic lane changing. Higher values result in earlier lane-changing.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_COOPERATIVE_PARAM,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The willingness for performing cooperative lane changing. Lower values result in reduced cooperation.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_SPEEDGAIN_PARAM,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The eagerness for performing lane changing to gain speed. Higher values result in more lane-changing.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_KEEPRIGHT_PARAM,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The eagerness for following the obligation to keep right. Higher values result in earlier lane-changing.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_SUBLANE_PARAM,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The eagerness for using the configured lateral alignment within the lane. Higher values result in increased willingness to sacrifice speed for alignment.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_OPPOSITE_PARAM,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The eagerness for overtaking through the opposite-direction lane. Higher values result in more lane-changing.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_PUSHY,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Willingness to encroach laterally on other drivers.",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_PUSHYGAP,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Minimum lateral gap when encroaching laterally on other drives (alternative way to define lcPushy)",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_ASSERTIVE,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Willingness to accept lower front and rear gaps on the target lane.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_IMPATIENCE,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Dynamic factor for modifying lcAssertive and lcPushy.",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_TIME_TO_IMPATIENCE,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Time to reach maximum impatience (of 1). Impatience grows whenever a lane-change manoeuvre is blocked.",
                                       "infinity");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_ACCEL_LAT,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Maximum lateral acceleration per second.",
                                       "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_LOOKAHEADLEFT,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Factor for configuring the strategic lookahead distance when a change to the left is necessary (relative to right lookahead).",
                                       "2.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_SPEEDGAINRIGHT,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Factor for configuring the treshold asymmetry when changing to the left or to the right for speed gain.",
                                       "0.1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_MAXSPEEDLATSTANDING,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Upper bound on lateral speed when standing.",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_MAXSPEEDLATFACTOR,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Upper bound on lateral speed while moving computed as lcMaxSpeedLatStanding + lcMaxSpeedLatFactor * getSpeed()",
                                       "1.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_TURN_ALIGNMENT_DISTANCE,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "Distance to an upcoming turn on the vehicles route, below which the alignment should be dynamically adapted to match the turn direction.",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_LCA_OVERTAKE_RIGHT,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "The probability for violating rules gainst overtaking on the right.",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    /*
    attrProperty = AttributeProperties(SUMO_ATTR_LCA_EXPERIMENTAL1,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL | ATTRPROPERTY_EXTENDED,
                                       "XXXXX",
                                       "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);
    */
}


void
GNEAttributeCarrier::fillCommonPersonAttributes(SumoXMLTag currentTag) {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;

    attrProperty = AttributeProperties(SUMO_ATTR_ID,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE,
                                       "The name of the " + toString(currentTag));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TYPE,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_UNIQUE | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                       "The id of the " + toString(currentTag) + " type to use for this " + toString(currentTag) +
                                       DEFAULT_VTYPE_ID);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_COLOR,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_COLOR | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                       "This " + toString(currentTag) + "'s color",
                                       "yellow");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_DEPARTPOS,
                                       ATTRPROPERTY_COMPLEX | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "The position at which the " + toString(currentTag) + " shall enter the net",
                                       "base");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillCommonStopAttributes(SumoXMLTag currentTag) {
    // declare empty AttributeProperties
    AttributeProperties attrProperty;

    attrProperty = AttributeProperties(SUMO_ATTR_DURATION,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_OPTIONAL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                       "Minimum duration for stopping",
                                       "60");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_UNTIL,
                                       ATTRPROPERTY_FLOAT | ATTRPROPERTY_POSITIVE | ATTRPROPERTY_OPTIONAL | ATTRPROPERTY_DEFAULTVALUESTATIC,
                                       "The time step at which the route continues",
                                       "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_INDEX,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "Where to insert the stop in the vehicle's list of stops",
                                       "end");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TRIGGERED,
                                       ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "Whether a person may end the stop",
                                       "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_EXPECTED,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "List of persons that must board the vehicle before it may continue");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_CONTAINER_TRIGGERED,
                                       ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "Whether a container may end the stop",
                                       "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_EXPECTED_CONTAINERS,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_LIST | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "List of containers that must be loaded onto the vehicle before it may continue");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_PARKING,
                                       ATTRPROPERTY_BOOL | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "whether the vehicle stops on the road or beside ",
                                       "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_ACTTYPE,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "Activity displayed for stopped person in GUI and output files ",
                                       "waiting");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = AttributeProperties(SUMO_ATTR_TRIP_ID,
                                       ATTRPROPERTY_STRING | ATTRPROPERTY_DEFAULTVALUESTATIC | ATTRPROPERTY_WRITEXMLOPTIONAL,
                                       "Value used for trips that uses this stop");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


bool
GNEAttributeCarrier::checkParsedAttribute(const TagProperties& tagProperties,
        const AttributeProperties& attrProperties, const SumoXMLAttr attribute,
        std::string& defaultValue, std::string& parsedAttribute, std::string& warningMessage) {
    // declare a string for details about error formats
    std::string errorFormat;
    // set extra check for ID Values
    if (attribute == SUMO_ATTR_ID) {
        if (parsedAttribute.empty()) {
            errorFormat = "ID cannot be empty; ";
        } else if (tagProperties.isDetector()) {
            // special case for detectors (because in this case empty spaces are allowed)
            if (SUMOXMLDefinitions::isValidDetectorID(parsedAttribute) == false) {
                errorFormat = "Detector ID contains invalid characters; ";
            }
        } else if (tagProperties.isDemandElement()) {
            // special case for detectors (because in this case empty spaces are allowed)
            if (SUMOXMLDefinitions::isValidVehicleID(parsedAttribute) == false) {
                errorFormat = "Demand Element ID contains invalid characters; ";
            }
        } else if (SUMOXMLDefinitions::isValidNetID(parsedAttribute) == false) {
            errorFormat = "ID contains invalid characters; ";
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
            } else if (attrProperties.cannotBeZero() && (parsedIntAttribute == 0)) {
                errorFormat = "Cannot be zero; ";
            }
        } else if (canParse<double>(parsedAttribute)) {
            errorFormat = "Float cannot be reinterpreted as int; ";
        } else {
            errorFormat = "Cannot be parsed to int; ";
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
            } else if (attrProperties.cannotBeZero() && (parsedDoubleAttribute == 0)) {
                errorFormat = "Cannot be zero; ";
            }
        } else {
            errorFormat = "Cannot be parsed to float; ";
        }
    }
    // Set extra checks for bool values
    if (attrProperties.isBool()) {
        if (!canParse<bool>(parsedAttribute)) {
            errorFormat = "Cannot be parsed to boolean; ";
        }
    }
    // Set extra checks for position values
    if (attrProperties.isposition()) {
        // check if we're parsing a single position or an entire shape
        if (attrProperties.isList()) {
            // check if parsed attribute can be parsed to Position Vector
            if (!canParse<PositionVector>(parsedAttribute)) {
                errorFormat = "List of Positions aren't neither x,y nor x,y,z; ";
            }
        } else if (!canParse<Position>(parsedAttribute)) {
            errorFormat = "Position is neither x,y nor x,y,z; ";
        }
    }
    // set extra check for time(double) values
    if (attrProperties.isSUMOTime()) {
        if (!canParse<SUMOTime>(parsedAttribute)) {
            errorFormat = "Cannot be parsed to SUMOTime; ";
        }
    }
    // set extra check for probability values
    if (attrProperties.isProbability()) {
        if (canParse<double>(parsedAttribute)) {
            // parse to double and check if is between [0,1]
            double probability = parse<double>(parsedAttribute);
            if (probability < 0) {
                errorFormat = "Probability cannot be smaller than 0; ";
            } else if (probability > 1) {
                errorFormat = "Probability cannot be greather than 1; ";
            }
        } else {
            errorFormat = "Cannot be parsed to probability; ";
        }
    }
    // set extra check for range values
    if (attrProperties.hasAttrRange()) {
        if (canParse<double>(parsedAttribute)) {
            // parse to double and check if is in range
            double range = parse<double>(parsedAttribute);
            if (range < attrProperties.getMinimumRange()) {
                errorFormat = "Float cannot be smaller than " + toString(attrProperties.getMinimumRange()) + "; ";
            } else if (range > attrProperties.getMaximumRange()) {
                errorFormat = "Float cannot be greather than " + toString(attrProperties.getMaximumRange()) + "; ";
            }
        } else {
            errorFormat = "Cannot be parsed to float; ";
        }
    }
    // set extra check for discrete values
    if (attrProperties.isDiscrete()) {
        // search value in the list of discretes values of attribute properties
        auto finder = std::find(attrProperties.getDiscreteValues().begin(), attrProperties.getDiscreteValues().end(), parsedAttribute);
        // check if attribute is valid
        if (finder == attrProperties.getDiscreteValues().end()) {
            errorFormat = "value is not within the set of allowed values for attribute '" + toString(attribute) + "'";
        }
    }
    // set extra check for color values
    if (attrProperties.isColor() && !canParse<RGBColor>(parsedAttribute)) {
        errorFormat = "Invalid RGB format or named color; ";
    }
    // set extra check for filename values
    if (attrProperties.isFilename()) {
        if (SUMOXMLDefinitions::isValidFilename(parsedAttribute) == false) {
            errorFormat = "Filename contains invalid characters; ";
        } else if (parsedAttribute.empty() && !attrProperties.isWriteXMLOptional()) {
            errorFormat = "Filename cannot be empty; ";
        }
    }
    // set extra check for name values
    if ((attribute == SUMO_ATTR_NAME) && !SUMOXMLDefinitions::isValidAttribute(parsedAttribute)) {
        errorFormat = "name contains invalid characters; ";
    }
    // set extra check for SVCPermissions values
    if (attrProperties.isVClass()) {
        if (!canParseVehicleClasses(parsedAttribute)) {
            errorFormat = "List of VClasses isn't valid; ";
            parsedAttribute = defaultValue;
        }
    }
    // set extra check for RouteProbes
    if ((attribute == SUMO_ATTR_ROUTEPROBE) && !SUMOXMLDefinitions::isValidNetID(parsedAttribute)) {
        errorFormat = "RouteProbe ID contains invalid characters; ";
    }
    // set extra check for list of edges
    if ((attribute == SUMO_ATTR_EDGES) && parsedAttribute.empty()) {
        errorFormat = "List of edges cannot be empty; ";
    }
    // set extra check for list of lanes
    if ((attribute == SUMO_ATTR_LANES) && parsedAttribute.empty()) {
        errorFormat = "List of lanes cannot be empty; ";
    }
    // set extra check for list of VTypes
    if ((attribute == SUMO_ATTR_VTYPES) && !parsedAttribute.empty() && !SUMOXMLDefinitions::isValidListOfTypeID(parsedAttribute)) {
        errorFormat = "List of vTypes contains invalid characters; ";
    }
    // set extra check for list of RouteProbe
    if ((attribute == SUMO_ATTR_ROUTEPROBE) && !parsedAttribute.empty() && !SUMOXMLDefinitions::isValidNetID(parsedAttribute)) {
        errorFormat = "RouteProbe ID contains invalid characters; ";
    }
    // If attribute has an invalid format
    if (errorFormat.size() > 0) {
        // if attribute is optional and has a default value, obtain it as string. In other case, abort.
        if (attrProperties.isWriteXMLOptional()) {
            WRITE_DEBUG("Format of optional " + attrProperties.getDescription() + " attribute '" + toString(attribute) + "' of " +
                        warningMessage +  " is invalid; " + errorFormat + "Default value will be used.");
            // set default value defined in AttrProperties
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
        const AttributeProperties& attrProperties, std::string& parsedAttribute, std::string& warningMessage) {
    // if element can mask their XYPosition, then must be extracted X Y coordiantes separeted
    std::string x, y, z;
    bool parsedOk = true;
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
