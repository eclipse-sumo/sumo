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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
    enum TAGProperty {
        TAGPROPERTY_NETELEMENT =    1 << 0,     // Edges, Junctions, Lanes...
        TAGPROPERTY_ADDITIONAL =    1 << 1,     // Bus Stops, Charging Stations, Detectors...
        TAGPROPERTY_SHAPE =         1 << 2,     // POIs, Polygons
        TAGPROPERTY_ROUTEELEMENT =  1 << 3,     // VTypes, Vehicles, Flows...
        TAGPROPERTY_INTERNAL =      1 << 4,     // If set, element cannot be created using Frames (GNEAdditionalFrame, GNEPolygonFrame...)
        TAGPROPERTY_BLOCKMOVEMENT = 1 << 5,     // Element can block their movement
        TAGPROPERTY_BLOCKSHAPE =    1 << 6,     // Element can block their shape
        TAGPROPERTY_CLOSESHAPE =    1 << 7,     // Element can close their shape
        TAGPROPERTY_GEOPOSITION =   1 << 8,     // Element's position can be defined using a GEO position
        TAGPROPERTY_GEOSHAPE =      1 << 9,     // Element's shape acn be defined using a GEO Shape
        TAGPROPERTY_DIALOG =        1 << 10,    // Element can be edited using a dialog (GNECalibratorDialog, GNERerouterDialog...)
        TAGPROPERTY_PARENT =        1 << 11,    // Element will be writed in XML as child of another element (E3Entry -> E3Detector...)
    };

    /// @brief struct with the attribute Properties
    class TagValues {
    public:
        /// @brief default constructor
        TagValues();

        /// @brief parameter constructor
        TagValues(int tagProperty, int positionListed, SumoXMLTag tagParent = SUMO_TAG_NOTHING);

        /// @brief get position in list (used in frames for listing tags with certain sort)
        int getPositionListed() const;

        /// @brief if has a parent, return parent tag
        SumoXMLTag getParentTag() const;

        /// @brief return true if tag correspond to a netElement
        bool isNetElement() const;

        /// @brief return true if tag correspond to an additional
        bool isAdditional() const;

        /// @brief return true if tag correspond to a shape
        bool isShape() const;

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

    private:
        /// @brief Property of attribute
        int myTagProperty;

        /// @brief listed position
        int myPositionListed;

        /// @brief parent tag
        SumoXMLTag myParentTag;
    };


    enum AttrProperty {
        ATTRPROPERTY_INT =          1 << 0,     // Attribute is an integer (Including Zero)
        ATTRPROPERTY_FLOAT =        1 << 1,     // Attribute is a float
        ATTRPROPERTY_BOOL =         1 << 2,     // Attribute is boolean (0/1, true/false)
        ATTRPROPERTY_STRING =       1 << 3,     // Attribute is a string
        ATTRPROPERTY_POSITION =     1 << 4,     // Attribute is a position defined by doubles (x,y or x,y,z)
        ATTRPROPERTY_COLOR =        1 << 5,     // Attribute is a color defined by a specifically word (Red, green) or by a speicial format (XXX,YYY,ZZZ)
        ATTRPROPERTY_VCLASS =       1 << 6,     // Attribute is a VClass (passenger, bus, motorcicle...)
        ATTRPROPERTY_POSITIVE =     1 << 7,     // Attribute is positive (Including Zero)
        ATTRPROPERTY_UNIQUE =       1 << 8,     // Attribute is unique (cannot be edited in a selection of similar elements (ID, Position...)
        ATTRPROPERTY_FILENAME =     1 << 9,     // Attribute is a filename (string that cannot contains certain characters)
        ATTRPROPERTY_NONEDITABLE =  1 << 10,    // Attribute is non editable (index of a lane)
        ATTRPROPERTY_DISCRETE =     1 << 11,    // Attribute is discrete (only certain values are allowed)
        ATTRPROPERTY_PROBABILITY =  1 << 12,    // Attribute is probability (only allowed values between 0 and 1, including both)
        ATTRPROPERTY_TIME =         1 << 13,    // Attribute is a Time (float positive)
        ATTRPROPERTY_ANGLE =        1 << 14,    // Attribute is an angle (only takes values between 0 and 360, including both, another value will be automatically reduced
        ATTRPROPERTY_LIST =         1 << 15,    // Attribute is a list of other elements separated by spaces
        ATTRPROPERTY_OPTIONAL =     1 << 16,    // Attribute is optional
        ATTRPROPERTY_DEFAULTVALUE = 1 << 17,    // Attribute owns a default value
        ATTRPROPERTY_COMBINABLE =   1 << 18,    // Attribute is combinable with other Attribute
        ATTRPROPERTY_INHERITED =    1 << 19,    // Attribute can be inherited of another AC
    };

    /// @brief struct with the attribute Properties
    class AttributeValues {
    public:
        /// @brief default constructor
        AttributeValues();

        /// @brief parameter constructor
        AttributeValues(int attributeProperty, int positionListed, const std::string &definition, const std::string &defaultValue, std::vector<std::string> discreteValues = std::vector<std::string>());

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

        /// @brief return true if attribute owns a default value
        bool hasInheritValue() const;

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

    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] icon GUIIcon associated to the type of object
     */
    GNEAttributeCarrier(SumoXMLTag tag, GUIIcon icon);

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

    /// @brief get FXIcon assigned to this object
    FXIcon* getIcon() const;

    /// @brief get GUI icon assigned to this object
    GUIIcon getGUIIcon() const;

    /// @brief function to support debugging
    const std::string getID() const;

    /// @brief get all editable attributes for the given tag
    static const std::map<SumoXMLAttr, GNEAttributeCarrier::AttributeValues>& getAttributes(SumoXMLTag tag);

    /// @brief get Tag Properties
    static const TagValues &getTagProperties(SumoXMLTag tag);

    /// @brief get Attribute Properties (Used for simplify code)
    static const AttributeValues &getAttributeProperties(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief get all editable for tag elements of all types
    static std::vector<SumoXMLTag> allowedTags(bool includingInternals);

    /// @brief get all editable for tag net elements
    static std::vector<SumoXMLTag> allowedNetElementsTags(bool includingInternals);

    /// @brief get all editable for tag additional elements
    static std::vector<SumoXMLTag> allowedAdditionalTags(bool includingInternals);

    /// @brief get all editable for tag shape elements
    static std::vector<SumoXMLTag> allowedShapeTags(bool includingInternals);

    /// @brief check if an element with certain tag has a certain attribute
    static bool hasAttribute(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief return the number of attributes of the tag with the most highter number of attributes
    static int getHigherNumberOfAttributes();

    /// @brief return the default value of the attribute of an element
    /// @note It's advisable to check before with function hasDefaultValue if  exits a default value
    template<typename T>
    static T getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief true if a number of type T can be parsed from string
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

    /// @brief parses a number of type T from string
    template<typename T>
    static T parse(const std::string& string);

    /// @brief true if a positive number of type T can be parsed from string
    template<typename T>
    static bool isPositive(const std::string& string) {
        return canParse<T>(string) && parse<T>(string) > 0;
    }

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
        // set additionalOfWarningMessage
        std::string additionalOfWarningMessage;
        if (objectID != "") {
            additionalOfWarningMessage = toString(tag) + " with ID '" + objectID + "'";
        } else {
            additionalOfWarningMessage = toString(tag);
        }
        // first check what kind of default value has to be give if parsing isn't valid (needed to avoid exceptions)
        if (getAttributeProperties(tag, attribute).isNumerical()) {
            defaultValue = "0";
        } else if (getAttributeProperties(tag, attribute).isColor()) {
            defaultValue = "BLACK";
        }
        // first check that attribute exists in XML
        if (attrs.hasAttribute(attribute)) {
            // First check if attribute can be parsed to string
            parsedAttribute = attrs.get<std::string>(attribute, objectID.c_str(), parsedOk, false);
            // check that sucesfully parsed attribute can be converted to type T
            if (parsedOk && !canParse<T>(parsedAttribute)) {
                parsedOk = false;
                // only set default value if this isn't a SVCPermission
                if(!getAttributeProperties(tag, attribute).isVClass()) {
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
            if (getAttributeProperties(tag, attribute).isInt()) {
                if (canParse<int>(parsedAttribute)) {
                    // parse to int and check if can be negative
                    int parsedIntAttribute = parse<int>(parsedAttribute);
                    if (getAttributeProperties(tag, attribute).isPositive() && parsedIntAttribute < 0) {
                        errorFormat = "Cannot be negative; ";
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
            if (getAttributeProperties(tag, attribute).isFloat()) {
                if (canParse<double>(parsedAttribute)) {
                    // parse to double and check if can be negative
                    if (getAttributeProperties(tag, attribute).isPositive() && parse<double>(parsedAttribute) < 0) {
                        errorFormat = "Cannot be negative; ";
                        parsedOk = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to float; ";
                    parsedOk = false;
                }
            }
            // set extra check for time(double) values
            if (getAttributeProperties(tag, attribute).isTime()) {
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
            if (getAttributeProperties(tag, attribute).isProbability()) {
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
            if (getAttributeProperties(tag, attribute).isColor() && !canParse<RGBColor>(parsedAttribute)) {
                errorFormat = "Invalid RGB format or named color; ";
                parsedOk = false;
            }
            // set extra check for filename values
            if (getAttributeProperties(tag, attribute).isFilename() && (isValidFilename(parsedAttribute) == false)) {
                errorFormat = "Filename contains invalid characters; ";
                parsedOk = false;
            }
            // set extra check for SVCPermissions values
            if (getAttributeProperties(tag, attribute).isVClass()) {
                if (canParseVehicleClasses(parsedAttribute)) {
                    parsedAttribute = toString(parseVehicleClasses(parsedAttribute));
                    parsedOk = true;
                } else {
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
                // if attribute has a default value, obtain it as string. In other case, abort.
                if (getAttributeProperties(tag, attribute).hasDefaultValue()) {
                    parsedAttribute = toString(getDefaultValue<T>(tag, attribute));
                } else if (getAttributeProperties(tag, attribute).hasInheritValue()) {
                    // set default value
                    parsedAttribute = defaultValue;
                } else {
                    WRITE_WARNING("Format of essential " + getAttributeProperties(tag, attribute).getDescription() + " attribute '" + toString(attribute) + "' of " +
                                  additionalOfWarningMessage +  " is invalid; " + errorFormat + toString(tag) + " cannot be created");
                    // abort parsing of element
                    abort = true;
                    // set default value
                    parsedAttribute = defaultValue;
                }
            }
        } else {
            // if attribute has a default value, obtain it. In other case, abort.
            if (getAttributeProperties(tag, attribute).hasDefaultValue()) {
                parsedAttribute = toString(getDefaultValue<T>(tag, attribute));
            } else if (getAttributeProperties(tag, attribute).hasInheritValue()) {
                // set default value
                parsedAttribute = defaultValue;
            } else {
                WRITE_WARNING("Essential " + getAttributeProperties(tag, attribute).getDescription() + " attribute '" + toString(attribute) + "' of " +
                              additionalOfWarningMessage +  " is missing; " + toString(tag) + " cannot be created");
                // abort parsing of element
                abort = true;
                // set default value
                parsedAttribute = defaultValue;
            }
        }
        // return parsed attribute
        return parse<T>(parsedAttribute);
    }

    /// @name function used to parse GNEEdges and GNELanes
    /// @{

    /** @brief check if a list of edge IDs is valid
     * @brief value string with a list of edges
     * @brief report enable or disable show warning if edges aren't valid
     */
    static bool checkGNEEdgesValid(GNENet* net, const std::string& value, bool report);

    /**@brief check if a list of Lane IDs is valid
     * @brief value string with a list of lanes
     * @brief report enable or disable show warning if lanes aren't valid
     */
    static bool checkGNELanesValid(GNENet* net, const std::string& value, bool report);

    /**@brief parse string into vector of GNEEdges
    * @throw exception one of GNEEdges doesn't exist
    */
    static std::vector<GNEEdge*> parseGNEEdges(GNENet* net, const std::string& value);

    /**@brief parse string into vector of GNELanes
    * @throw exception one of GNELanes doesn't exist
    */
    static std::vector<GNELane*> parseGNELanes(GNENet* net, const std::string& value);

    /**@brief parse vector of GNEEdges into string
    * @throw exception one of GNEEdges doesn't exist
    */
    static std::string parseGNEEdges(const std::vector<GNEEdge*>& edges);

    /**@brief parse vector of GNELanes into string
    * @throw exception one of GNELanes doesn't exist
    */
    static std::string parseGNELanes(const std::vector<GNELane*>& lanes);

    /// @}

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

    /// @brief icon associated to this AC
    GUIIcon myIcon;

    /// @brief map with the allowed attributes and their default values
    static std::map<SumoXMLTag, std::pair<TagValues, std::map<SumoXMLAttr, AttributeValues> > > myAllowedAttributes;

    /// @brief Invalidated assignment operator
    GNEAttributeCarrier& operator=(const GNEAttributeCarrier& src) = delete;
};

#endif

/****************************************************************************/

