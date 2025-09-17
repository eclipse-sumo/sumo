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
/// @file    GNEAttributeProperties.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for attribute properties used in GNEAttributeCarrier
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/Position.h>
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

    /// @brief enum class with all attribute properties
    enum class Property : int {
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
        FILEOPEN =          1 << 11,    // Attribute is a filename that opens an existent file
        FILESAVE =          1 << 12,    // Attribute is a filename that can create a new file
        DISCRETE =          1 << 13,    // Attribute is discrete (only certain values are allowed)
        PROBABILITY =       1 << 14,    // Attribute is probability (only allowed values between 0 and 1, including both)
        ANGLE =             1 << 15,    // Attribute is an angle (only takes values between 0 and 360, including both, another value will be automatically reduced
        LIST =              1 << 16,    // Attribute is a list of other elements separated by spaces
        SECUENCIAL =        1 << 17,    // Attribute is a special sequence of elements (for example: secuencial lanes in Multi Lane E2 detectors)
        DEFAULTVALUE =      1 << 18,    // Attribute owns a static default value
        SYNONYM =           1 << 19,    // Attribute will be written with a different name in der XML
        RANGE =             1 << 20,    // Attribute only accept a range of elements (example: Probability [0,1])
        UPDATEGEOMETRY =    1 << 21,    // Attribute require update geometry at the end of function setAttribute(...)
        ACTIVATABLE =       1 << 22,    // Attribute can be switch on/off using a checkbox in frame
        FLOW =              1 << 23,    // Attribute is part of a flow definition (Number, vehsPerHour...)
        COPYABLE =          1 << 24,    // Attribute can be copied over other element with the same tagProperty (used for edge/lane templates)
        ALWAYSENABLED =     1 << 25,    // Attribute cannot be disabled
        NO_PROPERTY =       1 << 26,    // No property defined
    };

    /// @brief enum class with all edit modes
    enum class Edit : int {
        CREATEMODE =        1 << 0,    // Attribute can be modified in create mode
        EDITMODE =          1 << 1,    // Attribute can be modified in edit mode
        NETEDITEDITOR =     1 << 2,    // Attribute can be edited only in netedit editor
        EXTENDEDEDITOR =    1 << 3,    // Attribute cannot be edited in editor, but is editable in extended Dialog
        GEOEDITOR =         1 << 4,    // Attribute can be edited only in geo editor
        FLOWEDITOR =        1 << 5,    // Attribute can be edited only in flow editor
        DIALOGEDITOR =      1 << 6,    // Attribute can be edited in dialog editor
        NO_EDIT =           1 << 7,    // No edit property defined
    };

    /// @brief parameter constructor for attribute properties without default values
    GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const Property attributeProperty,
                           const Edit editProperty, const std::string& definition);

    /// @brief parameter constructor for attribute properties with default values specific
    GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const Property attributeProperty,
                           const Edit editProperty, const std::string& definition, const std::string& defaultValue);

    /// @brief parameter constructor for attribute properties with default values generic
    GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const Property attributeProperty,
                           const Edit editProperty, const std::string& definition, const std::string& defaultValueMask,
                           const std::string& defaultValue);

    /// @brief parameter constructor for special attribute properties (ej: no common)
    GNEAttributeProperties(GNETagProperties* tagProperties, const SumoXMLAttr attribute, const std::string& definition);

    /// @brief destructor
    ~GNEAttributeProperties();

    /// @brief check Attribute integrity (For example, throw an exception if tag has a Float default value, but given default value cannot be parse to float)
    void checkAttributeIntegrity() const;

    /// @brief set discrete values
    void setDiscreteValues(const std::vector<std::string>& discreteValues);

    /// @brief set discrete values
    void setFilenameExtensions(const std::vector<std::string>& extensions);

    /// @brief set default activated value
    void setDefaultActivated(const bool value);

    /// @brief set synonim
    void setSynonym(const SumoXMLAttr synonym);

    /// @brief set range
    void setRange(const double minimum, const double maximum);

    /// @brief set tag property parent
    void setTagPropertyParent(GNETagProperties* tagPropertyParent);

    /// @brief set alternative name
    void setAlternativeName(const std::string& alternativeName);

    /// @brief get XML Attribute
    SumoXMLAttr getAttr() const;

    /// @brief get XML Attribute in string format (can be updated using alternative name)
    const std::string& getAttrStr() const;

    /// @brief get reference to tagProperty parent
    const GNETagProperties* getTagPropertyParent() const;

    /// @brief get position in list (used in frames for listing attributes with certain sort)
    int getPositionListed() const;

    /// @brief get default value
    const std::string& getDefinition() const;

    /// @brief get default value in string format
    const std::string& getDefaultStringValue() const;

    /// @brief get default int value
    int getDefaultIntValue() const;

    /// @brief get default double value
    double getDefaultDoubleValue() const;

    /// @brief get default time value
    SUMOTime getDefaultTimeValue() const;

    /// @brief get default bool value
    bool getDefaultBoolValue() const;

    /// @brief get default bool value
    const RGBColor& getDefaultColorValue() const;

    /// @brief get default position value
    const Position& getDefaultPositionValue() const;

    /// @brief get default active value
    bool getDefaultActivated() const;

    /// @brief return category (based on Edit)
    std::string getCategory() const;

    /// @brief return a description of attribute
    std::string getDescription() const;

    /// @brief get discrete values
    const std::vector<std::string>& getDiscreteValues() const;

    /// @brief get filename extensions in string format used in open dialogs
    const std::vector<std::string>& getFilenameExtensions() const;

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

    /// @brief return true if attribute is an angle
    bool isAngle() const;

    /// @brief return true if attribute is numerical (int or float)
    bool isNumerical() const;

    /// @brief return true if attribute is positive
    bool isPositive() const;

    /// @brief return true if attribute is a color
    bool isColor() const;

    /// @brief return true if attribute is a VType or vTypeDistribution
    bool isVType() const;

    /// @brief return true if attribute is a filename open
    bool isFileOpen() const;

    /// @brief return true if attribute is a filename save
    bool isFileSave() const;

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

    /// @brief return true if attribute requires a update geometry in setAttribute(...)
    bool requireUpdateGeometry() const;

    /// @brief return true if attribute is activatable
    bool isActivatable() const;

    /// @brief return true if attribute is part of a flow definition
    bool isFlow() const;

    /// @brief return true if attribute is copyable
    bool isCopyable() const;

    /// @brief return true if attribute is always enabled
    bool isAlwaysEnabled() const;

    /// @name edit modes
    /// @{

    /// @brief return true if this attribute can be edited in basic editor
    bool isBasicEditor() const;

    /// @brief return true if this attribute cannot be edited in editor
    bool isExtendedEditor() const;

    /// @brief return true if this attribute can be edited only in GEO editor
    bool isGeoEditor() const;

    /// @brief return true if this attribute can be edited only in flow editor
    bool isFlowEditor() const;

    /// @brief return true if this attribute can be edited only in netedit editor
    bool isNeteditEditor() const;

    /// @brief return true if attribute can be modified in create mode
    bool isCreateMode() const;

    /// @brief return true if attribute can be modified in edit mode
    bool isEditMode() const;

    /// @brief return true if attribute can be modified in dialog editor
    bool isDialogEditor() const;

    /// @}

private:
    /// @brief pointer to tagProperty parent
    const GNETagProperties* myTagPropertyParent;

    /// @brief XML Attribute
    SumoXMLAttr myAttribute = SUMO_ATTR_NOTHING;

    /// @brief string with the Attribute in text format (to avoid unnecesaries toStrings(...) calls)
    std::string myAttrStr;

    /// @brief attribute properties
    Property myAttributeProperty = Property::NO_PROPERTY;

    /// @brief edit properties
    Edit myEditProperty = Edit::NO_EDIT;

    /// @brief text with a definition of attribute
    std::string myDefinition;

    /// @brief default string value
    std::string myDefaultStringValue;

    /// @brief default int value
    int myDefaultIntValue = 0;

    /// @brief default double value
    double myDefaultDoubleValue = 0;

    /// @brief default time value
    SUMOTime myDefaultTimeValue = 0;

    /// @brief default bool value
    bool myDefaultBoolValue = false;

    /// @brief get default bool value
    RGBColor myDefaultColorValue = RGBColor::INVISIBLE;

    /// @brief get default position value
    Position myDefaultPositionValue = Position::INVALID;

    /// @brief default activated (by default false)
    bool myDefaultActivated = false;

    /// @brief discrete values that can take this Attribute (by default empty)
    std::vector<std::string> myDiscreteValues;

    /// @brief filename extensions used in open dialogs (by default empty)
    std::vector<std::string> myFilenameExtensions;

    /// @brief Attribute written in XML (If is SUMO_ATTR_NOTHING), original Attribute will be written)
    SumoXMLAttr myAttrSynonym = SUMO_ATTR_NOTHING;

    /// @brief minimun Range
    double myMinimumRange = 0;

    /// @brief maxium Range
    double myMaximumRange = 0;

    /// @brief check build constraints
    void checkBuildConstraints() const;

    /// @brief parse default values
    void parseDefaultValues(const std::string& defaultValue, const bool overWritteDefaultString);

    /// @brief invalidate default constructor
    GNEAttributeProperties() = delete;

    /// @brief Invalidated copy constructor.
    GNEAttributeProperties(const GNEAttributeProperties&) = delete;

    /// @brief Invalidated assignment operator
    GNEAttributeProperties& operator=(const GNEAttributeProperties& src) = delete;
};

/// @brief override attribute parent bit operator
constexpr GNEAttributeProperties::Property operator|(GNEAttributeProperties::Property a, GNEAttributeProperties::Property b) {
    return static_cast<GNEAttributeProperties::Property>(static_cast<int>(a) | static_cast<int>(b));
}

/// @brief override attribute parent bit operator
constexpr bool operator&(GNEAttributeProperties::Property a, GNEAttributeProperties::Property b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

/// @brief override attribute parent bit operator
constexpr GNEAttributeProperties::Edit operator|(GNEAttributeProperties::Edit a, GNEAttributeProperties::Edit b) {
    return static_cast<GNEAttributeProperties::Edit>(static_cast<int>(a) | static_cast<int>(b));
}

/// @brief override attribute parent bit operator
constexpr bool operator&(GNEAttributeProperties::Edit a, GNEAttributeProperties::Edit b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

/****************************************************************************/
