/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAttributeProperties.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for tag properties used in GNEAttributeCarrier
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include "GNEAttributeProperties.h"
#include "GNETagProperties.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAttributeProperties - methods
// ---------------------------------------------------------------------------

GNEAttributeProperties::GNEAttributeProperties() :
    myAttribute(SUMO_ATTR_NOTHING),
    myTagPropertyParent(nullptr),
    myAttrStr(toString(SUMO_ATTR_NOTHING)),
    myAttributeProperty(ATTRPROPERTY_STRING),
    myDefinition(""),
    myDefaultValue(""),
    myAttrSynonym(SUMO_ATTR_NOTHING),
    myMinimumRange(0),
    myMaximumRange(0) {}


GNEAttributeProperties::GNEAttributeProperties(const SumoXMLAttr attribute, const int attributeProperty, const std::string& definition, std::string defaultValue) :
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
    if ((attributeProperty & ATTRPROPERTY_OPTIONAL) && !((attributeProperty & ATTRPROPERTY_DEFAULTVALUESTATIC) || (attributeProperty & ATTRPROPERTY_DEFAULTVALUEMUTABLE))) {
        throw FormatException("Attribute '" + toString(attribute) + "' requires a either static or mutable default value");
    }
    // Attributes cannot be flowdefinition and enabilitablet at the same time
    if ((attributeProperty & ATTRPROPERTY_FLOWDEFINITION) && (attributeProperty & ATTRPROPERTY_ACTIVATABLE)) {
        throw FormatException("Attribute '" + toString(attribute) + "' cannot be flowdefinition and activatable at the same time");
    }
}


GNEAttributeProperties::~GNEAttributeProperties() {}


void
GNEAttributeProperties::checkAttributeIntegrity() {
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
        if (myMinimumRange == myMaximumRange) {
            throw FormatException("empty range");
        } else if ((myMinimumRange == 0) && (myMaximumRange == 0)) {
            throw FormatException("non-defined range");
        } else if ((myMaximumRange - myMinimumRange) <= 0) {
            throw FormatException("invalid range");
        }
    }
    // check that positive attributes correspond only to a int, floats or SUMOTimes
    if (isOptional() && !(hasStaticDefaultValue() || hasMutableDefaultValue())) {
        throw FormatException("if attribute is optional, must have either a static or dynamic default value");
    }
}


void
GNEAttributeProperties::setDiscreteValues(const std::vector<std::string>& discreteValues) {
    if (isDiscrete()) {
        myDiscreteValues = discreteValues;
    } else {
        throw FormatException("AttributeProperty doesn't support discrete values values");
    }
}


void
GNEAttributeProperties::setSynonym(const SumoXMLAttr synonym) {
    if (hasAttrSynonym()) {
        myAttrSynonym = synonym;
    } else {
        throw FormatException("AttributeProperty doesn't support synonyms");
    }
}


void
GNEAttributeProperties::setRange(const double minimum, const double maximum) {
    if (hasAttrRange()) {
        myMinimumRange = minimum;
        myMaximumRange = maximum;
        // check that given range is valid
        if (myMinimumRange == myMaximumRange) {
            throw FormatException("empty range");
        } else if ((myMinimumRange == 0) && (myMaximumRange == 0)) {
            throw FormatException("non-defined range");
        } else if ((myMaximumRange - myMinimumRange) <= 0) {
            throw FormatException("invalid range");
        }
    } else {
        throw FormatException("AttributeProperty doesn't support ranges");
    }
}


void
GNEAttributeProperties::setTagPropertyParent(GNETagProperties* tagPropertyParent) {
    myTagPropertyParent = tagPropertyParent;
}


SumoXMLAttr
GNEAttributeProperties::getAttr() const {
    return myAttribute;
}


const std::string&
GNEAttributeProperties::getAttrStr() const {
    return myAttrStr;
}


const GNETagProperties&
GNEAttributeProperties::getTagPropertyParent() const {
    return *myTagPropertyParent;
}


int
GNEAttributeProperties::getPositionListed() const {
    for (auto i = myTagPropertyParent->begin(); i != myTagPropertyParent->end(); i++) {
        if (i->getAttr() == myAttribute) {
            return (int)(i - myTagPropertyParent->begin());
        }
    }
    throw ProcessError("Attribute wasn't found in myTagPropertyParent");
}


const std::string&
GNEAttributeProperties::getDefinition() const {
    return myDefinition;
}


const std::string&
GNEAttributeProperties::getDefaultValue() const {
    return myDefaultValue;
}


std::string
GNEAttributeProperties::getDescription() const {
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
    if ((myAttributeProperty & ATTRPROPERTY_DISCRETE) != 0) {
        pre += "discrete ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_OPTIONAL) != 0) {
        pre += "optional ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_UNIQUE) != 0) {
        pre += "unique ";
    }
    if ((myAttributeProperty & ATTRPROPERTY_VCLASSES) != 0) {
        pre += "vclasses ";
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
GNEAttributeProperties::getDiscreteValues() const {
    return myDiscreteValues;
}


SumoXMLAttr
GNEAttributeProperties::getAttrSynonym() const {
    if (hasAttrSynonym()) {
        return myAttrSynonym;
    } else {
        throw ProcessError("Attr doesn't support synonym");
    }
}


double
GNEAttributeProperties::getMinimumRange() const {
    if (hasAttrRange()) {
        return myMinimumRange;
    } else {
        throw ProcessError("Attr doesn't support range");
    }
}


double
GNEAttributeProperties::getMaximumRange() const {
    if (hasAttrRange()) {
        return myMaximumRange;
    } else {
        throw ProcessError("Attr doesn't support range");
    }
}


bool
GNEAttributeProperties::hasStaticDefaultValue() const {
    return (myAttributeProperty & ATTRPROPERTY_DEFAULTVALUESTATIC) != 0;
}


bool
GNEAttributeProperties::hasMutableDefaultValue() const {
    return (myAttributeProperty & ATTRPROPERTY_DEFAULTVALUEMUTABLE) != 0;
}


bool
GNEAttributeProperties::hasAttrSynonym() const {
    return (myAttributeProperty & ATTRPROPERTY_SYNONYM) != 0;
}

bool
GNEAttributeProperties::hasAttrRange() const {
    return (myAttributeProperty & ATTRPROPERTY_RANGE) != 0;
}


bool
GNEAttributeProperties::isInt() const {
    return (myAttributeProperty & ATTRPROPERTY_INT) != 0;
}


bool
GNEAttributeProperties::isFloat() const {
    return (myAttributeProperty & ATTRPROPERTY_FLOAT) != 0;
}


bool
GNEAttributeProperties::isSUMOTime() const {
    return (myAttributeProperty & ATTRPROPERTY_SUMOTIME) != 0;
}


bool
GNEAttributeProperties::isBool() const {
    return (myAttributeProperty & ATTRPROPERTY_BOOL) != 0;
}


bool
GNEAttributeProperties::isString() const {
    return (myAttributeProperty & ATTRPROPERTY_STRING) != 0;
}


bool
GNEAttributeProperties::isposition() const {
    return (myAttributeProperty & ATTRPROPERTY_POSITION) != 0;
}


bool
GNEAttributeProperties::isProbability() const {
    return (myAttributeProperty & ATTRPROPERTY_PROBABILITY) != 0;
}


bool
GNEAttributeProperties::isNumerical() const {
    return (myAttributeProperty & (ATTRPROPERTY_INT | ATTRPROPERTY_FLOAT | ATTRPROPERTY_SUMOTIME)) != 0;
}


bool
GNEAttributeProperties::isPositive() const {
    return (myAttributeProperty & ATTRPROPERTY_POSITIVE) != 0;
}


bool
GNEAttributeProperties::isColor() const {
    return (myAttributeProperty & ATTRPROPERTY_COLOR) != 0;
}


bool
GNEAttributeProperties::isFilename() const {
    return (myAttributeProperty & ATTRPROPERTY_FILENAME) != 0;
}


bool
GNEAttributeProperties::isVClass() const {
    return (myAttributeProperty & ATTRPROPERTY_VCLASS) != 0;
}


bool
GNEAttributeProperties::isSVCPermission() const {
    return ((myAttributeProperty & ATTRPROPERTY_LIST) != 0) && ((myAttributeProperty & ATTRPROPERTY_VCLASS) != 0);
}


bool
GNEAttributeProperties::isList() const {
    return (myAttributeProperty & ATTRPROPERTY_LIST) != 0;
}


bool
GNEAttributeProperties::isSecuential() const {
    return (myAttributeProperty & ATTRPROPERTY_SECUENCIAL) != 0;
}


bool
GNEAttributeProperties::isUnique() const {
    return (myAttributeProperty & ATTRPROPERTY_UNIQUE) != 0;
}


bool
GNEAttributeProperties::isOptional() const {
    return (myAttributeProperty & ATTRPROPERTY_OPTIONAL) != 0;
}


bool
GNEAttributeProperties::isDiscrete() const {
    return (myAttributeProperty & ATTRPROPERTY_DISCRETE) != 0;
}


bool
GNEAttributeProperties::isVClasses() const {
    return (myAttributeProperty & ATTRPROPERTY_VCLASSES) != 0;
}


bool
GNEAttributeProperties::isExtended() const {
    return (myAttributeProperty & ATTRPROPERTY_EXTENDED) != 0;
}


bool
GNEAttributeProperties::requireUpdateGeometry() const {
    return (myAttributeProperty & ATTRPROPERTY_UPDATEGEOMETRY) != 0;
}


bool
GNEAttributeProperties::isActivatable() const {
    return (myAttributeProperty & ATTRPROPERTY_ACTIVATABLE) != 0;
}


bool
GNEAttributeProperties::isComplex() const {
    return (myAttributeProperty & ATTRPROPERTY_COMPLEX) != 0;
}


bool
GNEAttributeProperties::isFlowDefinition() const {
    return (myAttributeProperty & ATTRPROPERTY_FLOWDEFINITION) != 0;
}

/****************************************************************************/
