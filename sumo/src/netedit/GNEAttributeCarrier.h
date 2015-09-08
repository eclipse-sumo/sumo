/****************************************************************************/
/// @file    GNEAttributeCarrier.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/ToString.h>
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

    friend class GNEChange_Attribute;

public:

    /** @brief Constructor
     */
    GNEAttributeCarrier(SumoXMLTag tag);


    /// @brief Destructor
    virtual ~GNEAttributeCarrier() {};

    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     * @param[in] net optionally the GNENet to inform about gui updates
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    virtual bool isValid(SumoXMLAttr key, const std::string& value);

    /// @brief how should this attribute carrier be called
    virtual std::string getDescription() {
        return toString(myTag);
    }

    SumoXMLTag getTag() const {
        return myTag;
    }

    const std::vector<SumoXMLAttr>& getAttrs() const {
        return GNEAttributeCarrier::allowedAttributes(myTag);
    }

    /// @brief function to support debugging
    const std::string getID() const {
        return getAttribute(SUMO_ATTR_ID);
    }

    /// @brief get all editable attributes for tag.
    static const std::vector<SumoXMLAttr>& allowedAttributes(SumoXMLTag tag);

    /// @brief get all editable attributes for tag.
    static const std::vector<SumoXMLTag>& allowedTags();

    /// @brief whether an attribute is numerical
    static bool isNumerical(SumoXMLAttr attr);

    /// @brief whether an attribute is unique (may not be edited for a multi-selection)
    static bool isUnique(SumoXMLAttr attr);

    /// @brief return a list of discrete choices for this attribute or an empty vector
    static const std::vector<std::string>& discreteChoices(SumoXMLTag tag, SumoXMLAttr attr);

    /// @brief true if an object of type T can be parsed from string
    template <class T>
    static bool canParse(const std::string& string) {
        T tmp;
        std::istringstream buf(string);
        buf >> tmp;
        return !buf.fail() && (size_t)buf.tellg() == string.size();
    }

    /// @brief parses an object of type T from from string
    template <class T>
    static T parse(const std::string& string) {
        T result;
        std::istringstream buf(string);
        buf >> result;
        return result;
    }

    /// @brief true if a positive number of type T can be parsed from string
    template <class T>
    static bool isPositive(const std::string& string) {
        if (canParse<T>(string)) {
            return parse<T>(string) > 0;
        } else {
            return false;
        }
    }

    /// @brief true if value is a valid sumo ID
    static bool isValidID(const std::string& value);

    /// @brief feature is still unchanged after being loaded (implies approval)
    static const std::string LOADED;
    /// @brief feature has been reguessed (may still be unchanged be we can't tell (yet)
    static const std::string GUESSED;
    /// @brief feature has been manually modified (implies approval)
    static const std::string MODIFIED;
    /// @brief feature has been approved but not changed (i.e. after being reguessed)
    static const std::string APPROVED;


private:
    /* @brief method for setting the attribute and nothing else
     * (used in GNEChange_Attribute)
     * */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief the xml tag to which this carrier corresponds
    const SumoXMLTag myTag;

    static std::map<SumoXMLTag, std::vector<SumoXMLAttr> > _allowedAttributes;
    static std::vector<SumoXMLTag> _allowedTags;
    static std::set<SumoXMLAttr> _numericalAttrs;
    static std::set<SumoXMLAttr> _uniqueAttrs;
    static std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > _discreteChoices;

private:
    /// @brief Invalidated assignment operator
    GNEAttributeCarrier& operator=(const GNEAttributeCarrier& src);

};

#endif

/****************************************************************************/

