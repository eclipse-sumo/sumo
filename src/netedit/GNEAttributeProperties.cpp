/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEAttributeProperties.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for tag properties used in GNEAttributeCarrier
/****************************************************************************/

#include <netedit/elements/GNEAttributeCarrier.h>

#include "GNETagProperties.h"
#include "GNEAttributeProperties.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributeProperties::GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const int attributeProperty,
        const int editProperty, const std::string& definition) :
    myTagPropertyParent(tagProperties),
    myAttribute(attribute),
    myAttrStr(toString(attribute)),
    myAttributeProperty(attributeProperty),
    myEditProperty(editProperty),
    myDefinition(definition) {
    // check build conditions (only in debug mode)
    checkBuildConstraints();
    // add attribute in tag properties vector
    tagProperties->myAttributeProperties.push_back(this);
}


GNEAttributeProperties::GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const int attributeProperty, const int editProperty,
        const std::string& definition, const std::string& defaultValue) :
    myTagPropertyParent(tagProperties),
    myAttribute(attribute),
    myAttrStr(toString(attribute)),
    myAttributeProperty(attributeProperty),
    myEditProperty(editProperty),
    myDefinition(definition),
    myDefaultStringValue(defaultValue) {
    // check build conditions (only in debug mode)
    checkBuildConstraints();
    // parse default values
    parseDefaultValues(defaultValue, true);
    // add attribute in tag properties vector
    tagProperties->myAttributeProperties.push_back(this);
}


GNEAttributeProperties::GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const int attributeProperty, const int editProperty,
        const std::string& definition, const std::string& defaultValueMask, const std::string& defaultValue) :
    myTagPropertyParent(tagProperties),
    myAttribute(attribute),
    myAttrStr(toString(attribute)),
    myAttributeProperty(attributeProperty),
    myEditProperty(editProperty),
    myDefinition(definition),
    myDefaultStringValue(defaultValueMask) {
    // check build conditions (only in debug mode)
    checkBuildConstraints();
    // parse default values
    parseDefaultValues(defaultValue, false);
    // add attribute in tag properties vector
    tagProperties->myAttributeProperties.push_back(this);
}


GNEAttributeProperties::GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const std::string& definition) :
    myTagPropertyParent(tagProperties),
    myAttribute(attribute),
    myAttrStr(toString(attribute)),
    myDefinition(definition) {
    // check build conditions (only in debug mode)
    checkBuildConstraints();
    // add attribute in tag properties vector
    tagProperties->myAttributeProperties.push_back(this);
}


GNEAttributeProperties::~GNEAttributeProperties() {}


void
GNEAttributeProperties::checkAttributeIntegrity() const {
    // check integrity only in debug mode
#ifdef DEBUG
    // check that there are properties
    if (myAttributeProperty == 0) {
        throw FormatException("Attr properties cannot be empty");
    }
    if (myEditProperty == 0) {
        throw FormatException("Attr edition properties cannot be empty");
    }
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
    // check that synonym attribute isn't nothing
    if ((isFileOpen() || isFileSave()) && myFilenameExtensions.empty()) {
        throw FormatException("Files requieres at least one extension");
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
    // check that unique attributes aren't copyables
    if (isUnique() && isCopyable()) {
        throw FormatException("Unique attributes aren't copyables");
    }
#endif // DEBUG
}


void
GNEAttributeProperties::setDiscreteValues(const std::vector<std::string>& discreteValues) {
    if (isDiscrete()) {
        myDiscreteValues = discreteValues;
    } else {
        throw FormatException("AttributeProperty doesn't support discrete values");
    }
}


void
GNEAttributeProperties::setFilenameExtensions(const std::string& extensions) {
    if (isFileOpen() || isFileSave()) {
        myFilenameExtensions = extensions;
    } else {
        throw FormatException("AttributeProperty doesn't support extensions values");
    }
}


void
GNEAttributeProperties::setDefaultActivated(const bool value) {
    if (isActivatable()) {
        myDefaultActivated = value;
    } else {
        throw FormatException("AttributeProperty doesn't support default activated");
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


const GNETagProperties*
GNEAttributeProperties::getTagPropertyParent() const {
    return myTagPropertyParent;
}


int
GNEAttributeProperties::getPositionListed() const {
    for (auto it = myTagPropertyParent->getAttributeProperties().begin(); it != myTagPropertyParent->getAttributeProperties().end(); it++) {
        if ((*it)->getAttr() == myAttribute) {
            return (int)(it - myTagPropertyParent->getAttributeProperties().begin());
        }
    }
    throw ProcessError("Attribute wasn't found in myTagPropertyParent");
}


const std::string&
GNEAttributeProperties::getDefinition() const {
    return myDefinition;
}


const std::string&
GNEAttributeProperties::getDefaultStringValue() const {
    return myDefaultStringValue;
}


int
GNEAttributeProperties::getDefaultIntValue() const {
    return myDefaultIntValue;
}


double
GNEAttributeProperties::getDefaultDoubleValue() const {
    return myDefaultDoubleValue;
}


SUMOTime
GNEAttributeProperties::getDefaultTimeValue() const {
    return myDefaultTimeValue;
}


bool
GNEAttributeProperties::getDefaultBoolValue() const {
    return myDefaultBoolValue;
}


const RGBColor&
GNEAttributeProperties::getDefaultColorValue() const {
    return myDefaultColorValue;
}


const Position&
GNEAttributeProperties::getDefaultPositionValue() const {
    return myDefaultPositionValue;
}


bool
GNEAttributeProperties::getDefaultActivated() const {
    return myDefaultActivated;
}


std::string
GNEAttributeProperties::getCategory() const {
    if (isExtendedEditor()) {
        return TL("Extended");
    } else if (isGeoEditor()) {
        return TL("GEO");
    } else if (isFlowEditor()) {
        return TL("Flow");
    } else if (isNeteditEditor()) {
        return TL("Netedit");
    } else {
        return TL("Internal");
    }
}


std::string
GNEAttributeProperties::getDescription() const {
    std::string pre;
    std::string type;
    std::string plural;
    std::string last;
    // pre type
    if (isList()) {
        pre += TL("list of ");
        if (isVClass()) {
            plural = "es";
        } else {
            plural = "s";
        }
    }
    if (isPositive()) {
        pre += TL("non-negative ");
    }
    if (isDiscrete()) {
        pre += TL("discrete ");
    }
    if (isUnique()) {
        pre += TL("unique ");
    }
    // type
    if (isInt()) {
        type = TL("integer");
    }
    if (isFloat()) {
        type = TL("float");
    }
    if (isSUMOTime()) {
        type = TL("SUMOTime");
    }
    if (isBool()) {
        type = TL("boolean");
    }
    if (isString()) {
        type = TL("string");
    }
    if (isPosition()) {
        type = TL("position");
    }
    if (isColor()) {
        type = TL("color");
    }
    if (isVClass()) {
        type = TL("vClass");
    }
    if (isFileOpen()) {
        type = TL("filename");
        last = TL("(Existent)");
    }
    if (isFileSave()) {
        type = TL("filename");
    }
    if (isProbability()) {
        type = TL("probability");
        last = TL("[0, 1]");
    }
    if (isAngle()) {
        type = TL("angle");
        last = TL("[0, 360]");
    }
    return pre + type + plural + last;
}


const std::vector<std::string>&
GNEAttributeProperties::getDiscreteValues() const {
    return myDiscreteValues;
}


const std::string&
GNEAttributeProperties::getFilenameExtensions() const {
    return myFilenameExtensions;
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
GNEAttributeProperties::hasDefaultValue() const {
    return (myAttributeProperty & DEFAULTVALUE) != 0;
}


bool
GNEAttributeProperties::hasAttrSynonym() const {
    return (myAttributeProperty & SYNONYM) != 0;
}

bool
GNEAttributeProperties::hasAttrRange() const {
    return (myAttributeProperty & RANGE) != 0;
}


bool
GNEAttributeProperties::isInt() const {
    return (myAttributeProperty & INT) != 0;
}


bool
GNEAttributeProperties::isFloat() const {
    return (myAttributeProperty & FLOAT) != 0;
}


bool
GNEAttributeProperties::isSUMOTime() const {
    return (myAttributeProperty & SUMOTIME) != 0;
}


bool
GNEAttributeProperties::isBool() const {
    return (myAttributeProperty & BOOL) != 0;
}


bool
GNEAttributeProperties::isString() const {
    return (myAttributeProperty & STRING) != 0;
}


bool
GNEAttributeProperties::isPosition() const {
    return (myAttributeProperty & POSITION) != 0;
}


bool
GNEAttributeProperties::isProbability() const {
    return (myAttributeProperty & PROBABILITY) != 0;
}


bool
GNEAttributeProperties::isAngle() const {
    return (myAttributeProperty & ANGLE) != 0;
}


bool
GNEAttributeProperties::isNumerical() const {
    return (myAttributeProperty & (INT | FLOAT | SUMOTIME)) != 0;
}


bool
GNEAttributeProperties::isPositive() const {
    return (myAttributeProperty & POSITIVE) != 0;
}


bool
GNEAttributeProperties::isColor() const {
    return (myAttributeProperty & COLOR) != 0;
}


bool
GNEAttributeProperties::isVType() const {
    return (myAttributeProperty & VTYPE) != 0;
}


bool
GNEAttributeProperties::isFileOpen() const {
    return (myAttributeProperty & FILEOPEN) != 0;
}


bool
GNEAttributeProperties::isFileSave() const {
    return (myAttributeProperty & FILESAVE) != 0;
}


bool
GNEAttributeProperties::isVClass() const {
    return (myAttributeProperty & VCLASS) != 0;
}


bool
GNEAttributeProperties::isSVCPermission() const {
    return ((myAttributeProperty & LIST) != 0) && ((myAttributeProperty & VCLASS) != 0);
}


bool
GNEAttributeProperties::isList() const {
    return (myAttributeProperty & LIST) != 0;
}


bool
GNEAttributeProperties::isSecuential() const {
    return (myAttributeProperty & SECUENCIAL) != 0;
}


bool
GNEAttributeProperties::isUnique() const {
    return (myAttributeProperty & UNIQUE) != 0;
}


bool
GNEAttributeProperties::isDiscrete() const {
    return (myAttributeProperty & DISCRETE) != 0;
}


bool
GNEAttributeProperties::requireUpdateGeometry() const {
    return (myAttributeProperty & UPDATEGEOMETRY) != 0;
}


bool
GNEAttributeProperties::isActivatable() const {
    return (myAttributeProperty & ACTIVATABLE) != 0;
}


bool
GNEAttributeProperties::isFlow() const {
    return (myAttributeProperty & FLOW) != 0;
}


bool
GNEAttributeProperties::isCopyable() const {
    return (myAttributeProperty & COPYABLE) != 0;
}


bool
GNEAttributeProperties::isAlwaysEnabled() const {
    return (myAttributeProperty & ALWAYSENABLED) != 0;
}


bool
GNEAttributeProperties::isBasicEditor() const {
    return !isExtendedEditor() && !isGeoEditor() && !isFlowEditor() && !isNeteditEditor();
}


bool
GNEAttributeProperties::isExtendedEditor() const {
    return (myEditProperty & EXTENDEDEDITOR) != 0;
}


bool
GNEAttributeProperties::isGeoEditor() const {
    return (myEditProperty & GEOEDITOR) != 0;
}


bool
GNEAttributeProperties::isFlowEditor() const {
    return (myEditProperty & FLOWEDITOR) != 0;
}


bool
GNEAttributeProperties::isNeteditEditor() const {
    return (myEditProperty & NETEDITEDITOR) != 0;
}


bool
GNEAttributeProperties::isCreateMode() const {
    return (myEditProperty & CREATEMODE) != 0;
}


bool
GNEAttributeProperties::isEditMode() const {
    return (myEditProperty & EDITMODE) != 0;
}


void
GNEAttributeProperties::checkBuildConstraints() const {
// check integrity only in debug mode
#ifdef DEBUG
    // empty definition aren't valid
    if (myDefinition.empty()) {
        throw FormatException("Missing definition for AttributeProperty '" + toString(myAttribute) + "'");
    }
    // if default value isn't empty, but attribute doesn't support default values, throw exception.
    if (!myDefaultStringValue.empty() && !(myAttributeProperty & DEFAULTVALUE)) {
        throw FormatException("AttributeProperty for '" + toString(myAttribute) + "' doesn't support default values");
    }
    // Attributes cannot be flowdefinition and enabilitablet at the same time
    if ((myAttributeProperty & FLOW) && (myAttributeProperty & ACTIVATABLE)) {
        throw FormatException("Attribute '" + toString(myAttribute) + "' cannot be flow definition and activatable at the same time");
    }
    // Check that attribute wasn't already inserted
    for (const auto& attrProperty : myTagPropertyParent->myAttributeProperties) {
        if (attrProperty->getAttr() == myAttribute) {
            throw ProcessError(TLF("Attribute '%' already inserted", toString(myAttribute)));
        }
    }
#endif // DEBUG
}


void
GNEAttributeProperties::parseDefaultValues(const std::string& defaultValue, const bool overWritteDefaultString) {
    // in every case parse default value back (to avoid problems during comparations)
    if (isInt()) {
        myDefaultIntValue = GNEAttributeCarrier::parse<int>(defaultValue);
        if (overWritteDefaultString) {
            myDefaultStringValue = toString(myDefaultIntValue);
        }
    } else if (isFloat()) {
        myDefaultDoubleValue = GNEAttributeCarrier::parse<double>(defaultValue);
        if (overWritteDefaultString) {
            myDefaultStringValue = toString(myDefaultDoubleValue);
        }
    } else if (isSUMOTime()) {
        myDefaultTimeValue = GNEAttributeCarrier::parse<SUMOTime>(defaultValue);
        if (overWritteDefaultString) {
            myDefaultStringValue = time2string(myDefaultTimeValue);
        }
    } else if (isBool()) {
        myDefaultBoolValue = GNEAttributeCarrier::parse<bool>(defaultValue);
        if (overWritteDefaultString) {
            myDefaultStringValue = myDefaultBoolValue ? GNEAttributeCarrier::True : GNEAttributeCarrier::False;
        }
    } else if (isBool()) {
        myDefaultColorValue = GNEAttributeCarrier::parse<RGBColor>(defaultValue);
        if (overWritteDefaultString) {
            myDefaultStringValue = toString(myDefaultColorValue);
        }
    } else if (isPosition()) {
        myDefaultPositionValue = GNEAttributeCarrier::parse<Position>(defaultValue);
        if (overWritteDefaultString) {
            myDefaultStringValue = toString(myDefaultPositionValue);
        }
    }
}

/****************************************************************************/
