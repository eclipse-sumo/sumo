/****************************************************************************/
/// @file    GNEAttributeCarrier.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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

#include <string>
#include <vector>
#include <map>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
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
class GNEUndoList;

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

    /// @brief This functions has to be implemented in allGNEAttributeCarrier
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
    static T parseAttributeFromXML(const SUMOSAXAttributes& attrs, const char* objectid, const SumoXMLTag tag, const SumoXMLAttr attribute, bool& abort, bool report = true) {
        bool ok = true;
        std::string parsedAttribute = "0";
        // only show one warning for every error/warning loading additional
        if (!abort) {
            // set additionalOfWarningMessage
            std::string additionalOfWarningMessage;
            if (objectid) {
                additionalOfWarningMessage = toString(tag) + " with ID '" + toString(objectid) + "'";
            } else {
                additionalOfWarningMessage = toString(tag);
            }
            // first check that attribute exists
            if (attrs.hasAttribute(attribute)) {
                // Parse attribute as string
                parsedAttribute = attrs.get<std::string>(attribute, objectid, ok, false);
                // check that parsed attribute can be converted to type T
                if (ok && !canParse<T>(parsedAttribute)) {
                    ok = false;
                }
                std::string errorFormat;
                // Set extra checks for int values
                if (isInt(tag, attribute)) {
                    if (canParse<int>(parsedAttribute)) {
                        // parse to int and check if can be negative
                        int parsedIntAttribute = parse<int>(parsedAttribute);
                        if (isPositive(tag, attribute) && parsedIntAttribute < 0) {
                            errorFormat = "Cannot be negative; ";
                            ok = false;
                        }
                    } else {
                        errorFormat = "Cannot be parsed to int; ";
                        ok = false;
                    }
                }
                // Set extra checks for float(double) values
                if (isFloat(tag, attribute)) {
                    if (canParse<double>(parsedAttribute)) {
                        // parse to double and check if can be negative
                        double parsedSumoRealAttribute = parse<double>(parsedAttribute);
                        if (isPositive(tag, attribute) && parsedSumoRealAttribute < 0) {
                            errorFormat = "Cannot be negative; ";
                            ok = false;
                        }
                    } else {
                        errorFormat = "Cannot be parsed to float; ";
                        ok = false;
                    }
                }
                // set extra check for time(double) values
                if (isTime(tag, attribute)) {
                    if (canParse<double>(parsedAttribute)) {
                        // parse to SUMO Real and check if is negative
                        double parsedSumoRealAttribute = parse<double>(parsedAttribute);
                        if (parsedSumoRealAttribute < 0) {
                            errorFormat = "Time cannot be negative; ";
                            ok = false;
                        }
                    } else {
                        errorFormat = "Cannot be parsed to time; ";
                        ok = false;
                    }
                }
                // set extra check for filename values
                if (isFilename(tag, attribute) && (isValidFilename(parsedAttribute) == false)) {
                    errorFormat = "Filename contains invalid characters; ";
                    ok = false;
                }
                // set extra check for color values
                if (isColor(tag, attribute) && (isValidFilename(parsedAttribute) == false)) {
                    errorFormat = "Invalid RGB or named color; ";
                    ok = false;
                }
                // set extra check for Vehicle Classes
                if ((!ok) && (attribute == SUMO_ATTR_VCLASS)) {
                    errorFormat = "Is not a part of defined set of Vehicle Classes; ";
                }
                // set extra check for Vehicle Classes
                if ((!ok) && (attribute == SUMO_ATTR_GUISHAPE)) {
                    errorFormat = "Is not a part of defined set of Gui Vehicle Shapes; ";
                }
                // If attribute has an invalid format
                if (!ok) {
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
                                      additionalOfWarningMessage +  " is invalid; " + errorFormat + "" + toString(tag) + " cannot be created");
                        // set default value of parsedAttribute (to avoid exceptions during conversions)
                        parsedAttribute = "0";
                        abort = true;
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
                    abort = true;
                }
            }
        }
        // return parsed attribute
        return parse<T>(parsedAttribute);
    }


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

    /// @brief map with the empty attributes
    static std::map<SumoXMLTag, std::set<SumoXMLAttr> > myFileAttrs;

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

