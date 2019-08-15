/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAttributeCarrier.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/
#ifndef GNEAttributeCarrier_h
#define GNEAttributeCarrier_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#include "GNEReferenceCounter.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEUndoList;
class GNELane;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAttributeCarrier
 *
 * Abstract Base class for gui objects which carry attributes
 * inherits from GNEReferenceCounter for convenience
 */
class GNEAttributeCarrier : public GNEReferenceCounter {

    /// @brief declare friend class
    friend class GNEChange_Attribute;
    friend class GNEChange_EnableAttribute;

public:

    // @brief declare class TagProperties
    class TagProperties;

    /// @brief struct with the tag Properties
    enum AttrProperty {
        ATTRPROPERTY_INT =                 1 << 0,   // Attribute is an integer (Including Zero)
        ATTRPROPERTY_FLOAT =               1 << 1,   // Attribute is a float
        ATTRPROPERTY_SUMOTIME =            1 << 2,  // Attribute is a SUMOTime
        ATTRPROPERTY_BOOL =                1 << 3,   // Attribute is boolean (0/1, true/false)
        ATTRPROPERTY_STRING =              1 << 4,   // Attribute is a string
        ATTRPROPERTY_POSITION =            1 << 5,   // Attribute is a position defined by doubles (x,y or x,y,z)
        ATTRPROPERTY_COLOR =               1 << 6,   // Attribute is a color defined by a specifically word (Red, green) or by a special format (XXX,YYY,ZZZ)
        ATTRPROPERTY_VCLASS =              1 << 7,   // Attribute is a VClass (passenger, bus, motorcicle...)
        ATTRPROPERTY_POSITIVE =            1 << 8,   // Attribute is positive (Including Zero)
        ATTRPROPERTY_NOTZERO =             1 << 9,   // Attribute cannot be 0 (only for numerical attributes)
        ATTRPROPERTY_UNIQUE =              1 << 10,  // Attribute is unique (cannot be edited in a selection of similar elements (ID, Position...)
        ATTRPROPERTY_FILENAME =            1 << 11,  // Attribute is a filename (string that cannot contains certain characters)
        ATTRPROPERTY_NONEDITABLE =         1 << 12,  // Attribute is non editable (index of a lane)
        ATTRPROPERTY_DISCRETE =            1 << 13,  // Attribute is discrete (only certain values are allowed)
        ATTRPROPERTY_PROBABILITY =         1 << 14,  // Attribute is probability (only allowed values between 0 and 1, including both)
        ATTRPROPERTY_ANGLE =               1 << 15,  // Attribute is an angle (only takes values between 0 and 360, including both, another value will be automatically reduced
        ATTRPROPERTY_LIST =                1 << 16,  // Attribute is a list of other elements separated by spaces
        ATTRPROPERTY_SECUENCIAL =          1 << 17,  // Attribute is a special sequence of elements (for example: secuencial lanes in Multi Lane E2 detectors)
        ATTRPROPERTY_WRITEXMLOPTIONAL =    1 << 18,  // Attribute will not be written in XML file if current value is the same of their default Static/Mutable value
        ATTRPROPERTY_DEFAULTVALUESTATIC =  1 << 19,  // Attribute owns a static default value
        ATTRPROPERTY_DEFAULTVALUEMUTABLE = 1 << 20,  // Attribute owns a mutable default value (Default value depends of value of other attribute)
        ATTRPROPERTY_COMBINABLE =          1 << 21,  // Attribute is combinable with other attribute (example: Allow/disallow VClasses)
        ATTRPROPERTY_SYNONYM =             1 << 22,  // Attribute will be written with a different name in der XML
        ATTRPROPERTY_RANGE =               1 << 23,  // Attribute only accept a range of elements (example: Probability [0,1]
        ATTRPROPERTY_EXTENDED =            1 << 24,  // Attribute is extended (in Frame will not be shown, see VType attributes)
        ATTRPROPERTY_UPDATEGEOMETRY =      1 << 25,  // Attribute requiere update geometry at the end of function setAttribute(...)
        ATTRPROPERTY_OPTIONAL =            1 << 26,  // Attribute is optional, i.e. can be enabled/disabled using a checkbox in frame
        ATTRPROPERTY_COMPLEX =             1 << 27,  // Attribute is complex: Requiere a special function to check if their given value is valid
        ATTRPROPERTY_ENABLITABLE =         1 << 28,  // Attribute is enablitable: Can be enabled or disabled.
    };

    /// @brief struct with the attribute Properties
    class AttributeProperties {

    public:
        /// @brief default constructor
        AttributeProperties();

        /// @brief parameter constructor
        AttributeProperties(const SumoXMLAttr attribute, const int attributeProperty, const std::string& definition, std::string defaultValue = "");

        /// @brief destructor
        ~AttributeProperties();

        /// @brief check Attribute integrity (For example, throw an exception if tag has a Float default value, but given default value cannot be parse to float)
        void checkAttributeIntegrity();

        /// @brief set discrete values
        void setDiscreteValues(const std::vector<std::string>& discreteValues);

        /// @brief set synonim
        void setSynonym(const SumoXMLAttr synonym);

        /// @brief set range
        void setRange(const double minimum, const double maximum);

        /// @brief set tag property parent
        void setTagPropertyParent(TagProperties* tagPropertyParent);

        /// @brief get XML Attribute
        SumoXMLAttr getAttr() const;

        /// @brief get XML Attribute
        const std::string& getAttrStr() const;

        /// @brief get reference to tagProperty parent
        const TagProperties& getTagPropertyParent() const;

        /// @brief get position in list (used in frames for listing attributes with certain sort)
        int getPositionListed() const;

        /// @brief get default value
        const std::string& getDefinition() const;

        /// @brief get default value
        const std::string& getDefaultValue() const;

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

        /// @brief return true if attribute owns a static default value
        bool hasStaticDefaultValue() const;

        /// @brief return true if attribute owns a mutable default value
        bool hasMutableDefaultValue() const;

        /// @brief return true if Attr correspond to an element that will be written in XML with another name
        bool hasAttrSynonym() const;

        /// @brief return true if Attr correspond to an element that only accept a range of values
        bool hasAttrRange() const;

        /// @brief return true if atribute is an integer
        bool isInt() const;

        /// @brief return true if atribute is a float
        bool isFloat() const;

        /// @brief return true if atribute is a SUMOTime
        bool isSUMOTime() const;

        /// @brief return true if atribute is boolean
        bool isBool() const;

        /// @brief return true if atribute is a string
        bool isString() const;

        /// @brief return true if atribute is a position
        bool isposition() const;

        /// @brief return true if atribute is a probability
        bool isProbability() const;

        /// @brief return true if atribute is numerical (int or float)
        bool isNumerical() const;

        /// @brief return true if atribute is positive
        bool isPositive() const;

        /// @brief return true if atribute cannot be zero
        bool cannotBeZero() const;

        /// @brief return true if atribute is a color
        bool isColor() const;

        /// @brief return true if atribute is a filename
        bool isFilename() const;

        /// @brief return true if atribute is a VehicleClass
        bool isVClass() const;

        /// @brief return true if atribute is a VehicleClass
        bool isSVCPermission() const;

        /// @brief return true if atribute is a list
        bool isList() const;

        /// @brief return true if atribute is sequential
        bool isSecuential() const;

        /// @brief return true if atribute is unique
        bool isUnique() const;

        /// @brief return true if atribute is write XML optional
        bool isWriteXMLOptional() const;

        /// @brief return true if atribute is discrete
        bool isDiscrete() const;

        /// @brief return true if atribute is combinable with other Attribute
        bool isCombinable() const;

        /// @brief return true if atribute isn't editable
        bool isNonEditable() const;

        /// @brief return true if atribute is extended
        bool isExtended() const;

        /// @brief return true if atribute requieres a update geometry in setAttribute(...)
        bool requiereUpdateGeometry() const;

        /// @brief return true if atribute is optional
        bool isOptional() const;

        /// @brief return true if atribute is complex
        bool isComplex() const;

        /// @brief return true if atribute is enablitable
        bool isEnablitable() const;

    private:
        /// @brief XML Attribute
        SumoXMLAttr myAttribute;

        /// @brief pointer to tagProperty parent
        TagProperties* myTagPropertyParent;

        /// @brief string with the Attribute in text format (to avoid unnecesaries toStrings(...) calls)
        std::string myAttrStr;

        /// @brief Property of attribute
        int myAttributeProperty;

        /// @brief text with a definition of attribute
        std::string myDefinition;

        /// @brief default value (by default empty)
        std::string myDefaultValue;

        /// @brief discrete values that can take this Attribute (by default empty)
        std::vector<std::string> myDiscreteValues;

        /// @brief Attribute written in XML (If is SUMO_ATTR_NOTHING), original Attribute will be written)
        SumoXMLAttr myAttrSynonym;

        /// @brief minimun Range
        double myMinimumRange;

        /// @brief maxium Range
        double myMaximumRange;
    };

    enum TagType {
        TAGTYPE_NETELEMENT =    1 << 0,   // Edges, Junctions, Lanes...
        TAGTYPE_ADDITIONAL =    1 << 1,   // Bus Stops, Charging Stations, Detectors...
        TAGTYPE_SHAPE =         1 << 2,   // POIs, Polygons
        TAGTYPE_DEMANDELEMENT = 1 << 3,   // Routes, Vehicles, Trips...
        TAGTYPE_TAZ =           1 << 4,   // Traffic Assignment Zones
        TAGTYPE_STOPPINGPLACE = 1 << 5,   // StoppingPlaces (BusStops, ChargingStations...)
        TAGTYPE_DETECTOR =      1 << 6,   // Detectors (E1, E2...)
        TAGTYPE_VTYPE =         1 << 7,   // Vehicle types (vType and pTye)
        TAGTYPE_VEHICLE =       1 << 8,   // Vehicles (Flows, trips...)
        TAGTYPE_ROUTE =         1 << 9,   // Routes and embedded routes
        TAGTYPE_STOP =          1 << 10,  // Stops
        TAGTYPE_PERSON =        1 << 11,  // Persons
        TAGTYPE_PERSONPLAN =    1 << 12,  // Person plans (Walks, rides, ...)
        TAGTYPE_PERSONTRIP =    1 << 13,  // Walks
        TAGTYPE_WALK =          1 << 14,  // Walks
        TAGTYPE_RIDE =          1 << 15,  // Rides
        TAGTYPE_PERSONSTOP =    1 << 16,  // Person stops
    };

    enum TAGProperty {
        TAGPROPERTY_DRAWABLE =              1 << 0,     // Element can be drawed in view
        TAGPROPERTY_BLOCKMOVEMENT =         1 << 1,     // Element can block their movement
        TAGPROPERTY_BLOCKSHAPE =            1 << 2,     // Element can block their shape
        TAGPROPERTY_CLOSESHAPE =            1 << 3,     // Element can close their shape
        TAGPROPERTY_GEOPOSITION =           1 << 4,     // Element's position can be defined using a GEO position
        TAGPROPERTY_GEOSHAPE =              1 << 5,     // Element's shape acn be defined using a GEO Shape
        TAGPROPERTY_DIALOG =                1 << 6,     // Element can be edited using a dialog (GNECalibratorDialog, GNERerouterDialog...)
        TAGPROPERTY_PARENT =                1 << 7,     // Element will be writed in XML as child of another element (E3Entry -> E3Detector...)
        TAGPROPERTY_MINIMUMCHILDREN =       1 << 8,     // Element will be only writed in XML if has a minimum number of children
        TAGPROPERTY_REPARENT =              1 << 9,     // Element can be reparent
        TAGPROPERTY_SYNONYM =               1 << 10,    // Element will be written with a different name in der XML
        TAGPROPERTY_AUTOMATICSORTING =      1 << 11,    // Element sort automatic their Children (used by Additionals)
        TAGPROPERTY_SELECTABLE =            1 << 12,    // Element is selectable
        TAGPROPERTY_MASKSTARTENDPOS =       1 << 13,    // Element mask attributes StartPos and EndPos as "lenght" (Only used in the appropiate GNEFrame)
        TAGPROPERTY_MASKXYZPOSITION =       1 << 14,    // Element mask attributes X, Y and Z as "Position"
        TAGPROPERTY_WRITECHILDRENSEPARATE = 1 << 15,    // Element writes their children in a separated filename
        TAGPROPERTY_NOGENERICPARAMETERS =   1 << 16,    // Element doesn't accept Generic Parameters (by default all tags supports generic parameters)
        TAGPROPERTY_RTREE =                 1 << 17,    // Element is placed in RTREE
        TAGPROPERTY_SORTINGCHILDREN =       1 << 18,    // Element can be sorted in their parent element manually (in ACHierarchy)
    };

    /// @brief struct with the attribute Properties
    class TagProperties {
    public:
        /// @brief default constructor
        TagProperties();

        /// @brief parameter constructor
        TagProperties(SumoXMLTag tag, int tagType, int tagProperty, GUIIcon icon, SumoXMLTag parentTag = SUMO_TAG_NOTHING, SumoXMLTag tagSynonym = SUMO_TAG_NOTHING);

        /// @brief destructor
        ~TagProperties();

        /// @brief get Tag vinculated with this attribute Property
        SumoXMLTag getTag() const;

        /// @brief get Tag vinculated with this attribute Property in String Format (used to avoid multiple calls to toString(...)
        const std::string& getTagStr() const;

        /// @brief check Tag integrity (this include all their attributes)
        void checkTagIntegrity() const;

        /// @brief add attribute (duplicated attributed aren't allowed)
        void addAttribute(const AttributeProperties& attributeProperty);

        /// @brief add deprecated Attribute
        void addDeprecatedAttribute(SumoXMLAttr attr);

        /// @brief get attribute (throw error if doesn't exist)
        const AttributeProperties& getAttributeProperties(SumoXMLAttr attr) const;

        /// @brief get begin of attribute values (used for iterate)
        std::vector<AttributeProperties>::const_iterator begin() const;

        /// @brief get end of attribute values (used for iterate)
        std::vector<AttributeProperties>::const_iterator end() const;

        /// @brief get number of attributes
        int getNumberOfAttributes() const;

        /// @brief return the default value of the attribute of an element
        const std::string& getDefaultValue(SumoXMLAttr attr) const;

        /// @brief get GUI icon associated to this Tag
        GUIIcon getGUIIcon() const;

        /// @brief if Tag owns a parent, return parent tag
        SumoXMLTag getParentTag() const;

        /// @brief get tag synonym
        SumoXMLTag getTagSynonym() const;

        /// @brief check if current TagProperties owns the attribute attr
        bool hasAttribute(SumoXMLAttr attr) const;

        /// @brief return true if tag correspond to a netElement
        bool isNetElement() const;

        /// @brief return true if tag correspond to an additional
        bool isAdditional() const;

        /// @brief return true if tag correspond to a shape
        bool isShape() const;

        /// @brief return true if tag correspond to a TAZ
        bool isTAZ() const;

        /// @brief return true if tag correspond to a demand element
        bool isDemandElement() const;

        /// @brief return true if tag correspond to a detector (Only used to group all stoppingPlaces in the output XML)
        bool isStoppingPlace() const;

        /// @brief return true if tag correspond to a shape (Only used to group all detectors in the XML)
        bool isDetector() const;

        /// @brief return true if tag correspond to a vehicle type element
        bool isVehicleType() const;

        /// @brief return true if tag correspond to a vehicle element
        bool isVehicle() const;

        /// @brief return true if tag correspond to a route element
        bool isRoute() const;

        /// @brief return true if tag correspond to a stop element
        bool isStop() const;

        /// @brief return true if tag correspond to a person element
        bool isPerson() const;

        /// @brief return true if tag correspond to a person plan
        bool isPersonPlan() const;

        /// @brief return true if tag correspond to a person trip
        bool isPersonTrip() const;

        /// @brief return true if tag correspond to a walk element
        bool isWalk() const;

        /// @brief return true if tag correspond to a ride element
        bool isRide() const;

        /// @brief return true if tag correspond to a person stop element
        bool isPersonStop() const;

        /// @brief return true if tag correspond to a drawable element
        bool isDrawable() const;

        /// @brief return true if tag correspond to a selectable element
        bool isSelectable() const;

        /// @brief return true if tag correspond to an element that can block their movement
        bool canBlockMovement() const;

        /// @brief return true if tag correspond to an element that can block their shape
        bool canBlockShape() const;

        /// @brief return true if tag correspond to an element that can close their shape
        bool canCloseShape() const;

        /// @brief return true if tag correspond to an element that can use a geo position
        bool hasGEOPosition() const;

        /// @brief return true if tag correspond to an element that can use a geo shape
        bool hasGEOShape() const;

        /// @brief return true if tag correspond to an element that can had another element as parent
        bool hasParent() const;

        /// @brief return true if tag correspond to an element that will be written in XML with another tag
        bool hasTagSynonym() const;

        /// @brief return true if tag correspond to an element that can be edited using a dialog
        bool hasDialog() const;

        /// @brief return true if tag correspond to an element that only have a limited number of children
        bool hasMinimumNumberOfChildren() const;

        /// @brief return true if Tag correspond to an element that supports generic parameters
        bool hasGenericParameters() const;

        /// @brief return true if Tag correspond to an element that has has to be placed in RTREE
        bool isPlacedInRTree() const;

        /// @brief return true if Tag correspond to an element that can be sorted within their parent
        bool canBeSortedManually() const;

        /// @brief return true if tag correspond to an element that can be reparent
        bool canBeReparent() const;

        /// @brief return true if tag correspond to an element that can sort their children automatic
        bool canAutomaticSortChildren() const;

        /// @brief return true if tag correspond to an element that can sort their children automatic
        bool canWriteChildrenSeparate() const;

        /// @brief return true if tag correspond to an element that can mask the attributes "start" and "end" position as attribute "lenght"
        bool canMaskStartEndPos() const;

        /// @brief return true if tag correspond to an element that can mask the attributes "X", "Y" and "Z" position as attribute "Position"
        bool canMaskXYZPositions() const;

        /// @brief return true if attribute of this tag is deprecated
        bool isAttributeDeprecated(SumoXMLAttr attr) const;

    private:
        /// @brief Sumo XML Tag vinculated wit this tag Property
        SumoXMLTag myTag;

        /// @brief Sumo XML Tag vinculated wit this tag Property in String format
        std::string myTagStr;

        /// @brief Attribute Type
        int myTagType;

        /// @brief Attribute properties
        int myTagProperty;

        /// @brief vector with the attribute values vinculated with this Tag
        std::vector<AttributeProperties> myAttributeProperties;

        /// @brief icon associated to this Tag
        GUIIcon myIcon;

        /// @brief parent tag
        SumoXMLTag myParentTag;

        /// @brief Tag written in XML (If is SUMO_TAG_NOTHING), original Tag name will be written)
        SumoXMLTag myTagSynonym;

        /// @brief List with the deprecated Attributes
        std::vector<SumoXMLAttr> myDeprecatedAttributes;
    };

    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] icon GUIIcon associated to the type of object
     */
    GNEAttributeCarrier(const SumoXMLTag tag);

    /// @brief Destructor
    virtual ~GNEAttributeCarrier();

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @name This functions has to be implemented in all GNEAttributeCarriers
    /// @{
    /// @brief select attribute carrier using GUIGlobalSelection
    virtual void selectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief unselect attribute carrier using GUIGlobalSelection
    virtual void unselectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief check if attribute carrier is selected
    virtual bool isAttributeCarrierSelected() const = 0;

    /// @brief check if attribute carrier must be drawn using selecting color.
    virtual bool drawUsingSelectColor() const = 0;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for check if new value for certain attribute is valid
     * @param[in] key The attribute key
     * @param[in] value The new value
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /* @brief method for enabling attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note only certain attributes can be enabled, and can produce the disabling of other attributes
     */
    virtual void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) = 0;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    virtual bool isAttributeEnabled(SumoXMLAttr key) const = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;
    /// @}

    /// @name Certain attributes and ACs (for example, connections) can be either loaded or guessed. The following static variables are used to remark it.
    /// @{
    /// @brief feature is still unchanged after being loaded (implies approval)
    static const std::string FEATURE_LOADED;

    /// @brief feature has been reguessed (may still be unchanged be we can't tell (yet)
    static const std::string FEATURE_GUESSED;

    /// @brief feature has been manually modified (implies approval)
    static const std::string FEATURE_MODIFIED;

    /// @brief feature has been approved but not changed (i.e. after being reguessed)
    static const std::string FEATURE_APPROVED;
    /// @}

    /// @brief max number of attributes allowed for every tag
    static const size_t MAXNUMBEROFATTRIBUTES;

    /// @brief invalid double position
    static const double INVALID_POSITION;

    /// @brief method for getting the attribute in the context of object selection
    virtual std::string getAttributeForSelection(SumoXMLAttr key) const;

    /// @brief get tag assigned to this object in string format
    const std::string& getTagStr() const;

    /// @brief get Tag Property assigned to this object
    const TagProperties& getTagProperty() const;

    /// @brief get FXIcon associated to this AC
    FXIcon* getIcon() const;

    /// @brief function to support debugging
    const std::string getID() const;

    /// @brief get Tag Properties
    static const TagProperties& getTagProperties(SumoXMLTag tag);

    /// @brief get tags of all editable element types
    static std::vector<SumoXMLTag> allowedTags(bool onlyDrawables);

    /// @brief get tags of all editable element types using TagProperty Type (TAGTYPE_NETELEMENT, TAGTYPE_ADDITIONAL, etc.)
    static std::vector<SumoXMLTag> allowedTagsByCategory(int tagPropertyCategory, bool onlyDrawables);

    /// @name This functions related with generic parameters has to be implemented in all GNEAttributeCarriers
    /// @{

    /// @brief return generic parameters in string format
    virtual std::string getGenericParametersStr() const = 0;

    /// @brief return generic parameters as vector of pairs format
    virtual std::vector<std::pair<std::string, std::string> > getGenericParameters() const = 0;

    /// @brief set generic parameters in string format
    virtual void setGenericParametersStr(const std::string& value) = 0;

    /// @}

    /// @brief check if given string can be parsed to a map/list of generic parameters
    static bool isGenericParametersValid(const std::string& value);

    /// @brief true if a value of type T can be parsed from string
    template<typename T>
    static bool canParse(const std::string& string) {
        try {
            parse<T>(string);
        } catch (NumberFormatException&) {
            return false;
        } catch (EmptyData&) {
            return false;
        } catch (BoolFormatException&) {
            return false;
        }
        return true;
    }

    /// @brief parses a value of type T from string (used for basic types: int, double, bool, etc.)
    template<typename T>
    static T parse(const std::string& string);

    /// @brief true if a value of type T can be parsed from string
    template<typename T>
    static bool canParse(GNENet* net, const std::string& value, bool report) {
        try {
            parse<T>(net, value);
        } catch (FormatException& exception) {
            if (report) {
                WRITE_WARNING(exception.what())
            }
            return false;
        }
        return true;
    }

    /// @brief parses a complex value of type T from string (use for list of edges, list of lanes, etc.)
    template<typename T>
    static T parse(GNENet* net, const std::string& value);

    /// @brief parses a list of specific Attribute Carriers into a string of IDs
    template<typename T>
    static std::string parseIDs(const std::vector<T>& ACs);

    /// @brief check if lanes are consecutives
    static bool lanesConsecutives(const std::vector<GNELane*>& lanes);

    /// @brief Parse attribute from XML and show warnings if there are problems parsing it
    template <typename T>
    static T parseAttributeFromXML(const SUMOSAXAttributes& attrs, const std::string& objectID, const SumoXMLTag tag, const SumoXMLAttr attribute, bool& abort) {
        bool parsedOk = true;
        // declare string values
        std::string defaultValue, parsedAttribute, warningMessage;
        // obtain tag properties
        const auto& tagProperties = getTagProperties(tag);
        // first check if attribute is deprecated
        if (tagProperties.isAttributeDeprecated(attribute)) {
            // show warning if deprecateda ttribute is in the SUMOSAXAttributes
            if (attrs.hasAttribute(attribute)) {
                WRITE_WARNING("Attribute " + toString(attribute) + "' of " + tagProperties.getTagStr() + " is deprecated and will not be loaded.");
            }
            // return a dummy value
            return parse<T>("");
        }
        // now check if we're obtaining attribute of an object with an already parsed ID
        if (objectID != "") {
            warningMessage = tagProperties.getTagStr() + " with ID '" + objectID + "'";
        } else {
            warningMessage = tagProperties.getTagStr();
        }
        // check if we're parsing block movement
        if (attribute == GNE_ATTR_BLOCK_MOVEMENT) {
            // first check if we can parse
            if (tagProperties.canBlockMovement()) {
                // First check if attribute can be parsed to bool
                parsedAttribute = attrs.get<std::string>(attribute, objectID.c_str(), parsedOk, false);
                // check that sucesfully parsed attribute can be converted to type double
                if (!canParse<bool>(parsedAttribute)) {
                    abort = true;
                    // return default value
                    return parse<T>("0");
                } else {
                    // return readed value
                    return parse<T>(parsedAttribute);
                }
            } else {
                throw ProcessError("Trying to parsing block movement attribute in an AC that cannot be moved");
            }
        }
        // now check if we're parsing a GEO Attribute
        if (tagProperties.hasGEOPosition() && ((attribute == SUMO_ATTR_LON) || (attribute == SUMO_ATTR_LAT))) {
            // first check if GEO Attribute is defined
            if (attrs.hasAttribute(attribute)) {
                // First check if attribute can be parsed to string
                parsedAttribute = attrs.get<std::string>(attribute, objectID.c_str(), parsedOk, false);
                // check that sucesfully parsed attribute can be converted to type double
                if (!canParse<double>(parsedAttribute)) {
                    WRITE_WARNING("Format of GEO attribute '" + toString(attribute) + "' of " +
                                  warningMessage + " is invalid; Cannot be parsed to float; " + tagProperties.getTagStr() + " cannot be created");
                    // return default value
                    return parse<T>("0");
                } else {
                    // return readed value
                    return parse<T>(parsedAttribute);
                }
            }
            parsedOk = false;
            // return default value
            return parse<T>("0");
        }
        // obtain attribute properties (Only for improving efficiency)
        const auto& attrProperties = tagProperties.getAttributeProperties(attribute);
        // set a special default value for numerical and boolean attributes (To avoid errors parsing)
        if (attrProperties.isNumerical() || attrProperties.isBool()) {
            defaultValue = "0";
        } else if (attrProperties.isColor()) {
            defaultValue = "black";
        } else if (attrProperties.isposition()) {
            defaultValue = "0,0";
        }
        // first check that attribute exists in XML
        if (attrs.hasAttribute(attribute)) {
            // First check if attribute can be parsed to string
            parsedAttribute = attrs.get<std::string>(attribute, objectID.c_str(), parsedOk, false);
            // check parsed attribute
            if (!checkParsedAttribute(tagProperties, attrProperties, attribute, defaultValue, parsedAttribute, warningMessage)) {
                abort = true;
            }
        } else if (tagProperties.canMaskXYZPositions() && (attribute == SUMO_ATTR_POSITION)) {
            // obtain masked position attribute
            if (!parseMaskedPositionAttribute(attrs, objectID, tagProperties, attrProperties, parsedAttribute, warningMessage)) {
                abort = true;
            }
        } else {
            // if attribute is optional and has a default value, obtain it. In other case, abort.
            if (attrProperties.isWriteXMLOptional()) {
                parsedAttribute = attrProperties.getDefaultValue();
            } else {
                WRITE_WARNING("Essential " + attrProperties.getDescription() + " attribute '" + toString(attribute) + "' of " +
                              warningMessage +  " is missing; " + tagProperties.getTagStr() + " cannot be created");
                // abort parsing (and creation) of element
                abort = true;
                // set default value (To avoid errors in parse<T>(parsedAttribute))
                parsedAttribute = defaultValue;
            }
        }
        // return parsed attribute
        return parse<T>(parsedAttribute);
    }

protected:
    /// @brief the xml tag to which this attribute carrier corresponds
    const TagProperties& myTagProperty;

    /// @brief boolean to check if this AC is selected (instead of GUIGlObjectStorage)
    bool mySelected;

    /// @brief dummy TagProperty used for reference some elements (for Example, dummyEdge)
    static TagProperties dummyTagProperty;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief method for enabling the attribute and nothing else (used in GNEChange_EnableAttribute)
    virtual void setEnabledAttribute(const int enabledAttributes) = 0;

    /// @brief fill Attribute Carriers
    static void fillAttributeCarriers();

    /// @brief fill Net Elements
    static void fillNetElements();

    /// @brief fill Additionals
    static void fillAdditionals();

    /// @brief fill Shapes
    static void fillShapes();

    /// @brief fill Demand Elements
    static void fillDemandElements();

    /// @brief fill vehicle elements
    static void fillVehicleElements();

    /// @brief fill Stop elements
    static void fillStopElements();

    /// @brief fill Person Elements
    static void fillPersonElements();

    /// @brief fill PersonStop elements
    static void fillPersonStopElements();

    /// @brief fill common vehicle attributes (used by vehicles, trips, routeFlows and flows)
    static void fillCommonVehicleAttributes(SumoXMLTag currentTag);

    /// @brief fill common flow attributes (used by flows, routeFlows and personFlows)
    static void fillCommonFlowAttributes(SumoXMLTag currentTag);

    /// @brief fill Car Following Model of Vehicle/Person Types
    static void fillCarFollowingModelAttributes(SumoXMLTag currentTag);

    /// @brief fill Junction Model Attributes of Vehicle/Person Types
    static void fillJunctionModelAttributes(SumoXMLTag currentTag);

    /// @brief fill Junction Model Attributes of Vehicle/Person Types
    static void fillLaneChangingModelAttributes(SumoXMLTag currentTag);

    /// @brief fill common person attributes (used by person and personFlows)
    static void fillCommonPersonAttributes(SumoXMLTag currentTag);

    /// @brief fill stop person attributes (used by stops and personStps)
    static void fillCommonStopAttributes(SumoXMLTag currentTag);

    /// @brief parse and check attribute (note: This function is only to improve legilibility)
    static bool checkParsedAttribute(const TagProperties& tagProperties, const AttributeProperties& attrProperties, const SumoXMLAttr attribute,
                                     std::string& defaultValue, std::string& parsedAttribute, std::string& warningMessage);

    /// @brief parse and check masked  (note: This function is only to improve legilibility)
    static bool parseMaskedPositionAttribute(const SUMOSAXAttributes& attrs, const std::string& objectID, const TagProperties& tagProperties,
            const AttributeProperties& attrProperties, std::string& parsedAttribute, std::string& warningMessage);

    /// @brief map with the tags properties
    static std::map<SumoXMLTag, TagProperties> myTagProperties;

    /// @brief Invalidated copy constructor.
    GNEAttributeCarrier(const GNEAttributeCarrier&) = delete;

    /// @brief Invalidated assignment operator
    GNEAttributeCarrier& operator=(const GNEAttributeCarrier& src) = delete;
};

#endif

/****************************************************************************/

