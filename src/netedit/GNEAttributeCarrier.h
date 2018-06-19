/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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

#include <fx.h>
#include <string>
#include <vector>
#include <map>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/gui/images/GUIIcons.h>

#include "GNEReferenceCounter.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEEdge;
class GNELane;
class GNEUndoList;
class GUIGlObject;

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

public:

    enum AttrProperty {
        ATTRPROPERTY_INT =          1 << 0,     // Attribute is an integer (Including Zero)
        ATTRPROPERTY_FLOAT =        1 << 1,     // Attribute is a float
        ATTRPROPERTY_BOOL =         1 << 2,     // Attribute is boolean (0/1, true/false)
        ATTRPROPERTY_STRING =       1 << 3,     // Attribute is a string
        ATTRPROPERTY_POSITION =     1 << 4,     // Attribute is a position defined by doubles (x,y or x,y,z)
        ATTRPROPERTY_COLOR =        1 << 5,     // Attribute is a color defined by a specifically word (Red, green) or by a special format (XXX,YYY,ZZZ)
        ATTRPROPERTY_VCLASS =       1 << 6,     // Attribute is a VClass (passenger, bus, motorcicle...)
        ATTRPROPERTY_POSITIVE =     1 << 7,     // Attribute is positive (Including Zero)
        ATTRPROPERTY_NOTZERO =      1 << 8,     // Attribute cannot be 0 (only for numerical attributes)
        ATTRPROPERTY_UNIQUE =       1 << 9,     // Attribute is unique (cannot be edited in a selection of similar elements (ID, Position...)
        ATTRPROPERTY_FILENAME =     1 << 10,     // Attribute is a filename (string that cannot contains certain characters)
        ATTRPROPERTY_NONEDITABLE =  1 << 11,    // Attribute is non editable (index of a lane)
        ATTRPROPERTY_DISCRETE =     1 << 12,    // Attribute is discrete (only certain values are allowed)
        ATTRPROPERTY_PROBABILITY =  1 << 13,    // Attribute is probability (only allowed values between 0 and 1, including both)
        ATTRPROPERTY_TIME =         1 << 14,    // Attribute is a Time (float positive)
        ATTRPROPERTY_ANGLE =        1 << 15,    // Attribute is an angle (only takes values between 0 and 360, including both, another value will be automatically reduced
        ATTRPROPERTY_LIST =         1 << 16,    // Attribute is a list of other elements separated by spaces
        ATTRPROPERTY_OPTIONAL =     1 << 17,    // Attribute is optional
        ATTRPROPERTY_DEFAULTVALUE = 1 << 18,    // Attribute owns a default value
        ATTRPROPERTY_COMBINABLE =   1 << 19,    // Attribute is combinable with other Attribute
    };

    /// @brief struct with the attribute Properties
    class AttributeValues {
    public:
        /// @brief default constructor
        AttributeValues();

        /// @brief parameter constructor
        AttributeValues(int attributeProperty, int positionListed, const std::string &definition, const std::string &defaultValue, const std::vector<std::string> &discreteValues);

        /// @brief get position in list (used in frames for listing attributes with certain sort)
        int getPositionListed() const;

        /// @brief get default value
        const std::string &getDefinition() const;

        /// @brief get default value
        const std::string &getDefaultValue() const;

        /// @brief return a description of attribute
        std::string getDescription() const;

        /// @brief get discrete values
        const std::vector<std::string> &getDiscreteValues() const;

        /// @brief return true if attribute owns a default value
        bool hasDefaultValue() const;

        /// @brief return true if atribute is an integer
        bool isInt() const;

        /// @brief return true if atribute is a float
        bool isFloat() const;

        /// @brief return true if atribute is boolean
        bool isBool() const;

        /// @brief return true if atribute is a string
        bool isString() const;

        /// @brief return true if atribute is a probability
        bool isProbability() const;

        /// @brief return true if atribute is numerical (int or float)
        bool isNumerical() const;

        /// @brief return true if atribute is time
        bool isTime() const;

        /// @brief return true if atribute is positive
        bool isPositive() const;

        /// @brief return true if atribute cannot be zero
        bool isntZero() const;

        /// @brief return true if atribute is a color
        bool isColor() const;

        /// @brief return true if atribute is a filename
        bool isFilename() const;

        /// @brief return true if atribute is a VehicleClass
        bool isVClass() const;

        /// @brief return true if atribute is a list
        bool isList() const;

        /// @brief return true if atribute is unique
        bool isUnique() const;

        /// @brief return true if atribute is optional
        bool isOptional() const;

        /// @brief return true if atribute is discrete
        bool isDiscrete() const;

        /// @brief return true if atribute is combinable with other Attribute
        bool isCombinable() const;

    private:
        /// @brief Property of attribute
        int myAttributeProperty;

        /// @brief listed position
        int myPositionListed;

        /// @brief text with a definition of attribute
        std::string myDefinition;

        /// @brief default value (by default empty)
        std::string myDefaultValue;

        /// @brief discrete values that can take this Attribute (by default empty)
        std::vector<std::string> myDiscreteValues;
    };

    enum TAGProperty {
        TAGPROPERTY_NETELEMENT =    1 << 0,     // Edges, Junctions, Lanes...
        TAGPROPERTY_ADDITIONAL =    1 << 1,     // Bus Stops, Charging Stations, Detectors...
        TAGPROPERTY_SHAPE =         1 << 2,     // POIs, Polygons
        TAGPROPERTY_STOPPINGPLACE = 1 << 3,     // StoppingPlaces (BusStops, ChargingStations...)
        TAGPROPERTY_DETECTOR =      1 << 4,     // Detectors (E1, E2...)
        TAGPROPERTY_ROUTEELEMENT =  1 << 5,     // VTypes, Vehicles, Flows...
        TAGPROPERTY_INTERNAL =      1 << 6,     // If set, element cannot be created using Frames (GNEAdditionalFrame, GNEPolygonFrame...)
        TAGPROPERTY_BLOCKMOVEMENT = 1 << 7,     // Element can block their movement
        TAGPROPERTY_BLOCKSHAPE =    1 << 8,     // Element can block their shape
        TAGPROPERTY_CLOSESHAPE =    1 << 9,     // Element can close their shape
        TAGPROPERTY_GEOPOSITION =   1 << 10,    // Element's position can be defined using a GEO position
        TAGPROPERTY_GEOSHAPE =      1 << 11,    // Element's shape acn be defined using a GEO Shape
        TAGPROPERTY_DIALOG =        1 << 12,    // Element can be edited using a dialog (GNECalibratorDialog, GNERerouterDialog...)
        TAGPROPERTY_PARENT =        1 << 13,    // Element will be writed in XML as child of another element (E3Entry -> E3Detector...)
        TAGPROPERTY_LIMITEDCHILDS = 1 << 14,    // Element can only have a certain number of childs (0 -> unlimited)
        TAGPROPERTY_REPARENT =      1 << 15,    // Element can be reparent
    };

    /// @brief struct with the attribute Properties
    class TagValues {
    public:
        /// @brief default constructor
        TagValues();

        /// @brief parameter constructor
        TagValues(int tagProperty, int positionListed, GUIIcon icon, SumoXMLTag tagParent = SUMO_TAG_NOTHING, int maxNumberOfChilds = 0);

        /// @brief add attribute (duplicated attributed aren't allowed)
        void addAttribute(SumoXMLAttr attr, int attributeProperty, const std::string &definition, const std::string &defaultValue, std::vector<std::string> discreteValues = std::vector<std::string>());

        /// @brief get attribute (throw error if doesn't exist)
        const AttributeValues &getAttribute(SumoXMLAttr attr) const;

        /// @brief get begin of attribute values (used for iterate)
        std::map<SumoXMLAttr, AttributeValues>::const_iterator begin() const;

        /// @brief get end of attribute values (used for iterate)
        std::map<SumoXMLAttr, AttributeValues>::const_iterator end() const;

        /// @brief get number of attributes
        int getNumberOfAttributes() const;

        /// @brief return the default value of the attribute of an element
        const std::string &getDefaultValue(SumoXMLAttr attr) const;

        /// @brief get GUI icon associated to this Tag
        GUIIcon getGUIIcon() const;

        /// @brief get position in list (used in frames for listing tags with certain sort)
        int getPositionListed() const;

        /// @brief if Tag owns a parent, return parent tag
        SumoXMLTag getParentTag() const;

        /// @brief get maximum number of childs
        int getMaxNumberOfChilds() const;

        /// @brief check if current TagValues owns the attribute attr
        bool hasAttribute(SumoXMLAttr attr) const;

        /// @brief return true if tag correspond to a netElement
        bool isNetElement() const;

        /// @brief return true if tag correspond to an additional
        bool isAdditional() const;

        /// @brief return true if tag correspond to a shape
        bool isShape() const;

        /// @brief return true if tag correspond to a detector (Only used to group all stoppingPlaces in the output XML)
        bool isStoppingPlace() const;

        /// @brief return true if tag correspond to a shape (Only used to group all detectors in the XML)
        bool isDetector() const;

        /// @brief return true if tag correspond to an internal element (i.e. Cannot be created using frames)
        bool isInternal() const;

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

        /// @brief return true if tag correspond to an element that can be edited using a dialog
        bool hasDialog() const;

        /// @brief return true if tag correspond to an element that only have a limited number of childs
        bool hasLimitedNumberOfChilds() const;

        /// @brief return true if tag correspond to an element that can be reparent
        bool canBeReparent() const;

    private:
        /// @brief Property of attribute
        int myTagProperty;

        /// @brief map with the attribute values vinculated with this Tag
        std::map<SumoXMLAttr, AttributeValues> myAttributeValues;

        /// @brief icon associated to this Tag
        GUIIcon myIcon;

        /// @brief listed position
        int myPositionListed;

        /// @brief parent tag
        SumoXMLTag myParentTag;

        /// @brief maximun number of childs (0 -> unlimited)
        int myMaxNumberOfChilds;
    };

    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] icon GUIIcon associated to the type of object
     */
    GNEAttributeCarrier(SumoXMLTag tag);

    /// @brief Destructor
    virtual ~GNEAttributeCarrier() {};

    /// @name This functions has to be implemented in all GNEAttributeCarriers
    /// @{
    /// @brief select attribute carrier using GUIGlobalSelection
    virtual void selectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief unselect attribute carrier using GUIGlobalSelection
    virtual void unselectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief check if attribute carrier is selected
    virtual bool isAttributeCarrierSelected() const = 0;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     * @param[in] net optionally the GNENet to inform about gui updates
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;
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

    /// @brief method for getting the attribute in the context of object selection
    virtual std::string getAttributeForSelection(SumoXMLAttr key) const;

    /// @brief get XML Tag assigned to this object
    SumoXMLTag getTag() const;

    /// @brief get FXIcon associated to this AC
    FXIcon* getIcon() const;

    /// @brief function to support debugging
    const std::string getID() const;

    /// @brief get Tag Properties
    static const TagValues &getTagProperties(SumoXMLTag tag);

    /// @brief get all editable for tag elements of all types
    static std::vector<SumoXMLTag> allowedTags(bool includingInternals);

    /// @brief get all editable for tag net elements
    static std::vector<SumoXMLTag> allowedNetElementsTags(bool includingInternals);

    /// @brief get all editable for tag additional elements
    static std::vector<SumoXMLTag> allowedAdditionalTags(bool includingInternals);

    /// @brief get all editable for tag shape elements
    static std::vector<SumoXMLTag> allowedShapeTags(bool includingInternals);

    /// @brief return the number of attributes of the tag with the most highter number of attributes
    static int getHigherNumberOfAttributes();

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
            if(report) {
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
    static std::string parseIDs(const std::vector<T> &ACs);

    /// @brief parse a string of booleans (1 0 1 1....) using AND operation
    static bool parseStringToANDBool(const std::string& string);

    /// @brief true if value is a valid sumo ID
    static bool isValidID(const std::string& value);

    /// @brief true if value is a valid file value
    static bool isValidFilename(const std::string& value);

    /// @brief default value for invalid positions (used by POIs and Polygons)
    static const double INVALID_POSITION;

    /// @brief Parse attribute from XML and show warnings if there are problems parsing it
    template <typename T>
    static T parseAttributeFromXML(const SUMOSAXAttributes& attrs, const std::string& objectID, const SumoXMLTag tag, const SumoXMLAttr attribute, bool& abort/*, T optional*/) {
        bool parsedOk = true;
        std::string defaultValue, parsedAttribute;
        // obtain attribute properties (Only for improving efficiency)
        const auto &attrProperties = getTagProperties(tag).getAttribute(attribute);
        // set additionalOfWarningMessage
        std::string additionalOfWarningMessage;
        if (objectID != "") {
            additionalOfWarningMessage = toString(tag) + " with ID '" + objectID + "'";
        } else {
            additionalOfWarningMessage = toString(tag);
        }
        // set a special default value for numerical and boolean attributes (To avoid errors parsing)
        if(attrProperties.isNumerical() || attrProperties.isBool()) {
            defaultValue = "0";
        }
        // first check that attribute exists in XML
        if (attrs.hasAttribute(attribute)) {
            // First check if attribute can be parsed to string
            parsedAttribute = attrs.get<std::string>(attribute, objectID.c_str(), parsedOk, false);
            // check that sucesfully parsed attribute can be converted to type T
            if (parsedOk && !canParse<T>(parsedAttribute)) {
                parsedOk = false;
                // only set default value if this isn't a SVCPermission
                if(!attrProperties.isVClass()) {
                    parsedAttribute = defaultValue;
                }
            }
            // declare a string for details about error formats
            std::string errorFormat;
            // set extra check for ID Values
            if (attribute == SUMO_ATTR_ID) {
                if (parsedAttribute.empty()) {
                    errorFormat = "ID cannot be empty; ";
                    parsedOk = false;
                } else if (isValidID(parsedAttribute) == false) {
                    errorFormat = "'" + parsedAttribute + "' contains invalid characters; ";
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
                    } else if (attrProperties.isntZero() && (parsedIntAttribute == 0)) {
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
                    } else if (attrProperties.isntZero() && (parsedDoubleAttribute == 0)) {
                        errorFormat = "Cannot be zero; ";
                        parsedOk = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to float; ";
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
                    // parse to SUMO Real and check if is negative
                    if (parse<double>(parsedAttribute) < 0) {
                        errorFormat = "Probability cannot be smaller than 0; ";
                        parsedOk = false;
                    } else if (parse<double>(parsedAttribute) > 1) {
                        errorFormat = "Probability cannot be greather than 1; ";
                        parsedOk = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to probability; ";
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
                if(isValidFilename(parsedAttribute) == false) {
                    errorFormat = "Filename contains invalid characters; ";
                    parsedOk = false;
                } else if (parsedAttribute.empty()) {
                    errorFormat = "Filename cannot be empty; ";
                    parsedOk = false;
                }
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
            if ((attribute == SUMO_ATTR_ROUTEPROBE) && !isValidID(parsedAttribute)) {
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
                                  additionalOfWarningMessage +  " is invalid; " + errorFormat + toString(tag) + " cannot be created");
                    // abort parsing (and creation) of element
                    abort = true;
                    // set default value (To avoid errors in parse<T>(parsedAttribute))
                    parsedAttribute = defaultValue;
                }
            }
        } else {
            // if attribute is optional and has a default value, obtain it. In other case, abort.
            if (attrProperties.isOptional() && attrProperties.hasDefaultValue()) {
                parsedAttribute = attrProperties.getDefaultValue();
            } else {
                WRITE_WARNING("Essential " + attrProperties.getDescription() + " attribute '" + toString(attribute) + "' of " +
                              additionalOfWarningMessage +  " is missing; " + toString(tag) + " cannot be created");
                // abort parsing (and creation) of element
                abort = true;
                // set default value (To avoid errors in parse<T>(parsedAttribute))
                parsedAttribute = defaultValue;
            }
        }
        // return parsed attribute
        return parse<T>(parsedAttribute);
    }

    /// @brief function to calculate circle resolution for all circles drawn in drawGL(...) functions
    static int getCircleResolution(const GUIVisualizationSettings& settings);

    /**@brief write attribute if is essential or if is optional AND is different of default value 
     * (Note: This solution is temporal, see #4049)
     */
    void writeAttribute(OutputDevice& device, SumoXMLAttr key) const;

protected:
    /// @brief boolean to check if this AC is selected (instead of GUIGlObjectStorage)
    bool mySelected;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief fill Attribute Carriers
    static void fillAttributeCarriers();

    /// @brief the xml tag to which this attribute carrier corresponds
    const SumoXMLTag myTag;

    /// @brief map with the tags values
    static std::map<SumoXMLTag, TagValues> myAllowedTags;

    /// @brief Invalidated copy constructor.
    GNEAttributeCarrier(const GNEAttributeCarrier&) = delete;

    /// @brief Invalidated assignment operator
    GNEAttributeCarrier& operator=(const GNEAttributeCarrier& src) = delete;
};

#endif

/****************************************************************************/

