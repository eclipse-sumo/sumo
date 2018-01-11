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
    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] icon GUIIcon associated to the type of object
     */
    GNEAttributeCarrier(SumoXMLTag tag, GUIIcon icon);

    /// @brief Destructor
    virtual ~GNEAttributeCarrier() {};

    /// @brief This functions has to be implemented in all GNEAttributeCarriers
    /// @{
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

    /// @brief method for getting the attribute in the context of object selection
    virtual std::string getAttributeForSelection(SumoXMLAttr key) const;

    /// @brief get XML Tag assigned to this object
    SumoXMLTag getTag() const;

    /// @brief get FXIcon assigned to this object
    FXIcon* getIcon() const;

    /// @brief get GUI icon assigned to this object
    GUIIcon getGUIIcon() const;

    /**@brief get const pointer to GUIGlObject vinculated with this Attribute Carrier
     * @throw ProcessError if AC cannot be casted to const GUIGlObject
     */
    const GUIGlObject* getGUIGLObject() const;

    /**@brief get pointer to GUIGlObject vinculated with this Attribute Carrier
    * @throw ProcessError if AC cannot be casted to GUIGlObject
    */
    GUIGlObject* getGUIGLObject();

    /// @brief get vector of attributes
    std::vector<SumoXMLAttr> getAttrs() const;

    /// @brief function to support debugging
    const std::string getID() const;

    /// @brief get type of attribute
    static std::string getAttributeType(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief get all editable attributes for tag and their default values.
    static const std::vector<std::pair<SumoXMLAttr, std::string> >& allowedAttributes(SumoXMLTag tag);

    /// @brief get all editable for tag elements of all types
    static std::vector<SumoXMLTag> allowedTags();

    /// @brief get all editable for tag net elements
    static const std::vector<SumoXMLTag>& allowedNetElementsTags();

    /// @brief get all editable for tag additional elements
    static const std::vector<SumoXMLTag>& allowedAdditionalTags();

    /// @brief get all editable for tag shape elements
    static const std::vector<SumoXMLTag>& allowedShapeTags();

    /// @brief return true if element tag can block their movement
    static bool canBlockMovement(SumoXMLTag tag);

    /// @brief return true if element tag can block their shape
    static bool canBlockShape(SumoXMLTag tag);

    /// @brief return true if element tag can open a values editor
    static bool canOpenDialog(SumoXMLTag tag);

    /// @brief whether an attribute is numerical (int or float)
    static bool isNumerical(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is numerical or type int
    static bool isInt(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is numerical of type float
    static bool isFloat(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is time
    static bool isTime(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is of type bool for a certain tag
    static bool isBool(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is of type color for a certain tag
    static bool isColor(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is of type string
    static bool isString(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is of type bool
    static bool isList(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is unique (may not be edited for a multi-selection and don't have a default value)
    static bool isUnique(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is Discrete
    static bool isDiscrete(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is only Positive (i.e. cannot take negative values)
    static bool isPositive(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is a probability (i.e. oly can values between [0, 1])
    static bool isProbability(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether a string attribute is a filename
    static bool isFilename(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether a string attribute is a list of Vehicle Classes
    static bool isVClass(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief whether an attribute is non editable
    static bool isNonEditable(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief check if an element with certain tag has a certain attribute
    static bool hasAttribute(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief check if attribute of an element has a default avlue
    static bool hasDefaultValue(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief return a list of discrete choices for this attribute or an empty vector
    static const std::vector<std::string>& discreteChoices(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief return whether the given attribute allows for a combination of discrete values
    static bool discreteCombinableChoices(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief return definition of a certain SumoXMLAttr
    static std::string getDefinition(SumoXMLTag tag, SumoXMLAttr attr);

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

    /// @brief true if value is a valid sumo ID
    static bool isValidID(const std::string& value);

    /// @brief true if value is a valid file value
    static bool isValidFilename(const std::string& value);

    /// @brief feature is still unchanged after being loaded (implies approval)
    static const std::string LOADED;

    /// @brief feature has been reguessed (may still be unchanged be we can't tell (yet)
    static const std::string GUESSED;

    /// @brief feature has been manually modified (implies approval)
    static const std::string MODIFIED;

    /// @brief feature has been approved but not changed (i.e. after being reguessed)
    static const std::string APPROVED;

    /// @brief default value for invalid positions (used by POIs and Polygons)
    static const double INVALID_POSITION;

    /// @brief Parse attribute from XML and show warnings if there are problems parsing it
    template <typename T>
    static T parseAttributeFromXML(const SUMOSAXAttributes& attrs, const std::string& objectID, const SumoXMLTag tag, const SumoXMLAttr attribute, bool& abort, bool report = true) {
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
        if (isInt(tag, attribute) || isFloat(tag, attribute) || isTime(tag, attribute)) {
            defaultValue = "0";
        } else if (isColor(tag, attribute)) {
            defaultValue = "BLACK";
        }
        // first check that attribute exists in XML
        if (attrs.hasAttribute(attribute)) {
            // First check if attribute can be parsed to string
            parsedAttribute = attrs.get<std::string>(attribute, objectID.c_str(), parsedOk, false);
            // check that sucesfully parsed attribute can be converted to type T
            if (parsedOk && !canParse<T>(parsedAttribute)) {
                parsedOk = false;
                parsedAttribute = defaultValue;
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
            if (isInt(tag, attribute)) {
                if (canParse<int>(parsedAttribute)) {
                    // parse to int and check if can be negative
                    int parsedIntAttribute = parse<int>(parsedAttribute);
                    if (isPositive(tag, attribute) && parsedIntAttribute < 0) {
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
            if (isFloat(tag, attribute)) {
                if (canParse<double>(parsedAttribute)) {
                    // parse to double and check if can be negative
                    if (isPositive(tag, attribute) && parse<double>(parsedAttribute) < 0) {
                        errorFormat = "Cannot be negative; ";
                        parsedOk = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to float; ";
                    parsedOk = false;
                }
            }
            // set extra check for time(double) values
            if (isTime(tag, attribute)) {
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
            // set extra check for color values
            if (isColor(tag, attribute) && !canParse<RGBColor>(parsedAttribute)) {
                errorFormat = "Invalid RGB format or named color; ";
                parsedOk = false;
            }
            // set extra check for filename values
            if (isFilename(tag, attribute) && (isValidFilename(parsedAttribute) == false)) {
                errorFormat = "Filename contains invalid characters; ";
                parsedOk = false;
            }
            // set extra check for filename values
            if (isVClass(tag, attribute) && (canParseVehicleClasses(parsedAttribute) == false)) {
                errorFormat = "List of VClasses isn't valid; ";
                parsedOk = false;
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
                // if attribute has a default value, take it as string. In other case, abort.
                if (hasDefaultValue(tag, attribute)) {
                    parsedAttribute = toString(getDefaultValue<T>(tag, attribute));
                    // report warning of default value
                    if (report) {
                        WRITE_WARNING("Format of optional " + getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
                                      additionalOfWarningMessage + " is invalid; " + errorFormat + "Default value '" + toString(parsedAttribute) + "' will be used.");
                    }
                } else {
                    WRITE_WARNING("Format of essential " + getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
                                  additionalOfWarningMessage +  " is invalid; " + errorFormat + toString(tag) + " cannot be created");
                    // abort parsing of element
                    abort = true;
                    // set default value
                    parsedAttribute = defaultValue;
                }
            }
        } else {
            // if attribute has a default value, take it. In other case, abort.
            if (hasDefaultValue(tag, attribute)) {
                parsedAttribute = toString(getDefaultValue<T>(tag, attribute));
                // report warning of default value
                if (report) {
                    WRITE_WARNING("Optional " + getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
                                  additionalOfWarningMessage + " is missing; Default value '" + toString(parsedAttribute) + "' will be used.");
                }
            } else {
                WRITE_WARNING("Essential " + getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
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

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief the xml tag to which this attribute carrier corresponds
    const SumoXMLTag myTag;

    /// @brief icon associated to this AC
    GUIIcon myIcon;

    /// @brief map with the allowed attributes and their default values
    static std::map<SumoXMLTag, std::vector<std::pair <SumoXMLAttr, std::string> > > _allowedAttributes;

    /// @brief vector with the allowed tags of netElements
    static std::vector<SumoXMLTag> myAllowedNetElementTags;

    /// @brief vector with the allowed tags of additionals
    static std::vector<SumoXMLTag> myAllowedAdditionalTags;

    /// @brief vector with the allowed tags of shapes
    static std::vector<SumoXMLTag> myAllowedShapeTags;

    /// @brief vector with the allowed tags  that can block their movement
    static std::vector<SumoXMLTag> myBlockMovementTags;

    /// @brief vector with the allowed tags that can block their shapes
    static std::vector<SumoXMLTag> myBlockShapeTags;

    /// @brief vector with the allowed tags that has a editor values
    static std::vector<SumoXMLTag> myDialogTags;

    /// @brief map with the numerical attributes of type Int
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myNumericalIntAttrs;

    /// @brief map with the numerical attributes of type Float
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myNumericalFloatAttrs;

    /// @brief map with the attributes of type time
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myTimeAttrs;

    /// @brief map with the boolean attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myBoolAttrs;

    /// @brief map with the color attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myColorAttrs;

    /// @brief map with the attributes of type list
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myListAttrs;

    /// @brief map with the unique attributes (i.e. attributes without default values)
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myUniqueAttrs;

    /// @brief map with the non-editable attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myNonEditableAttrs;

    /// @brief map with the positive attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myPositiveAttrs;

    /// @brief map with the probability attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myProbabilityAttrs;

    /// @brief map with the file attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myFileAttrs;

    /// @brief map with the Vehicle Class attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myVClassAttrs;

    /// @brief map with the allowed tags of additionals with parent and their parent
    static std::map<SumoXMLTag, SumoXMLTag> myAllowedAdditionalWithParentTags;

    /// @brief map with the values of discrete choices
    static std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > myDiscreteChoices;

    /// @brief map with the definition of attributes
    static std::map<SumoXMLTag, std::map<SumoXMLAttr, std::string> > myAttrDefinitions;

    /// @brief maximum number of attributes of all tags
    static int myMaxNumAttribute;

    /// @brief Invalidated assignment operator
    GNEAttributeCarrier& operator=(const GNEAttributeCarrier& src) = delete;
};

#endif

/****************************************************************************/

