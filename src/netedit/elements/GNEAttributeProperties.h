/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEAttributeProperties.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for attribute properties used in GNEAttributeCarrier
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/xml/SUMOSAXAttributes.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNETagProperties;


// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeProperties {

public:

    /// @brief struct with the tag Properties
    enum AttrProperty {
        INT =               1 << 0,     // Attribute is an integer (Including Zero)
        FLOAT =             1 << 1,     // Attribute is a float
        SUMOTIME =          1 << 2,     // Attribute is a SUMOTime
        BOOL =              1 << 3,     // Attribute is boolean (0/1, true/false)
        STRING =            1 << 4,     // Attribute is a string
        POSITION =          1 << 5,     // Attribute is a position defined by doubles (x,y or x,y,z)
        COLOR =             1 << 6,     // Attribute is a color defined by a specifically word (Red, green) or by a special format (XXX,YYY,ZZZ)
        VTYPE =             1 << 7,     // Attribute corresponds to a Vtype or VTypeDistribution
        VCLASS =            1 << 8,     // Attribute is a VClass (passenger, bus, motorcicle...)
        POSITIVE =          1 << 9,     // Attribute is positive (Including Zero)
        UNIQUE =            1 << 10,    // Attribute is unique (cannot be edited in a selection of similar elements (ID, Position...)
        FILENAME =          1 << 11,    // Attribute is a filename (string that cannot contains certain characters)
        DISCRETE =          1 << 12,    // Attribute is discrete (only certain values are allowed)
        PROBABILITY =       1 << 13,    // Attribute is probability (only allowed values between 0 and 1, including both)
        ANGLE =             1 << 14,    // Attribute is an angle (only takes values between 0 and 360, including both, another value will be automatically reduced
        LIST =              1 << 15,    // Attribute is a list of other elements separated by spaces
        SECUENCIAL =        1 << 16,    // Attribute is a special sequence of elements (for example: secuencial lanes in Multi Lane E2 detectors)
        DEFAULTVALUE =      1 << 17,    // Attribute owns a static default value
        SYNONYM =           1 << 18,    // Attribute will be written with a different name in der XML
        RANGE =             1 << 19,    // Attribute only accept a range of elements (example: Probability [0,1])
        EXTENDED =          1 << 20,    // Attribute is extended (in Frame will not be shown, but is editable in a Dialog, see VType attributes)
        UPDATEGEOMETRY =    1 << 21,    // Attribute require update geometry at the end of function setAttribute(...)
        ACTIVATABLE =       1 << 22,    // Attribute can be switch on/off using a checkbox in frame
        FLOWDEFINITION =    1 << 23,    // Attribute is part of a flow definition (Number, vehsPerHour...)
        AUTOMATICID =       1 << 24,    // Attribute id can generate their own ID (used by additionals, vehicles, etc...)
        COPYABLE =          1 << 25,    // Attribute can be copied over other element with the same tagProperty (used for edge/lane templates)
    };

    /// @brief default constructor
    GNEAttributeProperties();

    /// @brief parameter constructor
    GNEAttributeProperties(const SumoXMLAttr attribute, const int attributeProperty, const std::string& definition, std::string defaultValue = "");

    /// @brief destructor
    ~GNEAttributeProperties();

    /// @brief check Attribute integrity (For example, throw an exception if tag has a Float default value, but given default value cannot be parse to float)
    void checkAttributeIntegrity() const;

    /// @brief set discrete values
    void setDiscreteValues(const std::vector<std::string>& discreteValues);

    /// @brief set default activated value
    void setDefaultActivated(const bool value);

    /// @brief set synonim
    void setSynonym(const SumoXMLAttr synonym);

    /// @brief set range
    void setRange(const double minimum, const double maximum);

    /// @brief set tag property parent
    void setTagPropertyParent(GNETagProperties* tagPropertyParent);

    /// @brief get XML Attribute
    SumoXMLAttr getAttr() const;

    /// @brief get XML Attribute
    const std::string& getAttrStr() const;

    /// @brief get reference to tagProperty parent
    const GNETagProperties& getTagPropertyParent() const;

    /// @brief get position in list (used in frames for listing attributes with certain sort)
    int getPositionListed() const;

    /// @brief get default value
    const std::string& getDefinition() const;

    /// @brief get default value
    const std::string& getDefaultValue() const;

    /// @brief get default active value
    bool getDefaultActivated() const;

    /// @brief return a description of attribute
    std::string getDescription() const;

    /// @brief get discrete values
    const std::vector<std::string>& getDiscreteValues() const;

    /// @brief get tag synonym
    SumoXMLAttr getAttrSynonym() const;

    /// @brief get minimum range
    double getMinimumRange() const;

    /// @brief get maximum range
    double getMaximumRange() const;

    /// @brief return true if attribute owns a default value
    bool hasDefaultValue() const;

    /// @brief return true if Attr correspond to an element that will be written in XML with another name
    bool hasAttrSynonym() const;

    /// @brief return true if Attr correspond to an element that only accept a range of values
    bool hasAttrRange() const;

    /// @brief return true if attribute is an integer
    bool isInt() const;

    /// @brief return true if attribute is a float
    bool isFloat() const;

    /// @brief return true if attribute is a SUMOTime
    bool isSUMOTime() const;

    /// @brief return true if attribute is boolean
    bool isBool() const;

    /// @brief return true if attribute is a string
    bool isString() const;

    /// @brief return true if attribute is a position
    bool isPosition() const;

    /// @brief return true if attribute is a probability
    bool isProbability() const;

    /// @brief return true if attribute is numerical (int or float)
    bool isNumerical() const;

    /// @brief return true if attribute is positive
    bool isPositive() const;

    /// @brief return true if attribute is a color
    bool isColor() const;

    /// @brief return true if attribute is a VType or vTypeDistribution
    bool isVType() const;

    /// @brief return true if attribute is a filename
    bool isFilename() const;

    /// @brief return true if attribute is a VehicleClass
    bool isVClass() const;

    /// @brief return true if attribute is a VehicleClass
    bool isSVCPermission() const;

    /// @brief return true if attribute is a list
    bool isList() const;

    /// @brief return true if attribute is sequential
    bool isSecuential() const;

    /// @brief return true if attribute is unique
    bool isUnique() const;

    /// @brief return true if attribute is discrete
    bool isDiscrete() const;

    /// @brief return true if attribute is extended
    bool isExtended() const;

    /// @brief return true if attribute requires a update geometry in setAttribute(...)
    bool requireUpdateGeometry() const;

    /// @brief return true if attribute is activatable
    bool isActivatable() const;

    /// @brief return true if attribute is part of a flow definition
    bool isFlowDefinition() const;

    /// @brief return true if attribute ID can generate an automatic ID
    bool hasAutomaticID() const;

    /// @brief return true if attribute ID is copyable
    bool isCopyable() const;

private:
    /// @brief XML Attribute
    SumoXMLAttr myAttribute = SUMO_ATTR_NOTHING;

    /// @brief pointer to tagProperty parent
    GNETagProperties* myTagPropertyParent = nullptr;

    /// @brief string with the Attribute in text format (to avoid unnecesaries toStrings(...) calls)
    std::string myAttrStr;

    /// @brief Property of attribute
    int myAttributeProperty = STRING;

    /// @brief text with a definition of attribute
    std::string myDefinition;

    /// @brief default value (by default empty)
    std::string myDefaultValue;

    /// @brief default activated (by default false)
    bool myDefaultActivated = false;

    /// @brief discrete values that can take this Attribute (by default empty)
    std::vector<std::string> myDiscreteValues;

    /// @brief Attribute written in XML (If is SUMO_ATTR_NOTHING), original Attribute will be written)
    SumoXMLAttr myAttrSynonym = SUMO_ATTR_NOTHING;

    /// @brief minimun Range
    double myMinimumRange = 0;

    /// @brief maxium Range
    double myMaximumRange = 0;
};

/****************************************************************************/
