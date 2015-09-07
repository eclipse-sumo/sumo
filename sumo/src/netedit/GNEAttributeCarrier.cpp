/****************************************************************************/
/// @file    GNEAttributeCarrier.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id: GNEAttributeCarrier.cpp 4567 2015-05-27 10:09:42Z erdm_ja $
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/common/MsgHandler.h>
#include "GNEAttributeCarrier.h"
#include "GNEUndoList.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
std::map<SumoXMLTag, std::vector<SumoXMLAttr> > GNEAttributeCarrier::_allowedAttributes;
std::vector<SumoXMLTag> GNEAttributeCarrier::_allowedTags;
std::set<SumoXMLAttr> GNEAttributeCarrier::_numericalAttrs;
std::set<SumoXMLAttr> GNEAttributeCarrier::_uniqueAttrs;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > GNEAttributeCarrier::_discreteChoices;

const std::string GNEAttributeCarrier::LOADED = "loaded";
const std::string GNEAttributeCarrier::GUESSED = "guessed";
const std::string GNEAttributeCarrier::MODIFIED = "modified";
const std::string GNEAttributeCarrier::APPROVED = "approved";

// ===========================================================================
// method definitions
// ===========================================================================
GNEAttributeCarrier::GNEAttributeCarrier(SumoXMLTag tag) :
    myTag(tag) {
}


bool
GNEAttributeCarrier::isValid(SumoXMLAttr key, const std::string& value) {
    UNUSED_PARAMETER(value);
    return std::find(getAttrs().begin(), getAttrs().end(), key) != getAttrs().end();
}


bool
GNEAttributeCarrier::isValidID(const std::string& value) {
    return value.find_first_of(" \t\n\r@$%^&/|\\{}*'\";:<>") == std::string::npos;
}

// ===========================================================================
// static methods
// ===========================================================================

const std::vector<SumoXMLAttr>&
GNEAttributeCarrier::allowedAttributes(SumoXMLTag tag) {
    // define on first access
    if (!_allowedAttributes.count(tag)) {
        std::vector<SumoXMLAttr>& attrs = _allowedAttributes[tag];
        switch (tag) {
            case SUMO_TAG_EDGE:
                attrs.push_back(SUMO_ATTR_ID);
                attrs.push_back(SUMO_ATTR_FROM);
                attrs.push_back(SUMO_ATTR_TO);
                attrs.push_back(SUMO_ATTR_SPEED);
                attrs.push_back(SUMO_ATTR_PRIORITY);
                attrs.push_back(SUMO_ATTR_NUMLANES);
                attrs.push_back(SUMO_ATTR_TYPE);
                attrs.push_back(SUMO_ATTR_ALLOW);
                attrs.push_back(SUMO_ATTR_DISALLOW);
                //attrs.push_back(SUMO_ATTR_PREFER);
                attrs.push_back(SUMO_ATTR_SHAPE);
                attrs.push_back(SUMO_ATTR_LENGTH);
                attrs.push_back(SUMO_ATTR_SPREADTYPE);
                attrs.push_back(SUMO_ATTR_NAME);
                attrs.push_back(SUMO_ATTR_WIDTH);
                attrs.push_back(SUMO_ATTR_ENDOFFSET);
                break;
            case SUMO_TAG_JUNCTION:
                attrs.push_back(SUMO_ATTR_ID);
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(SUMO_ATTR_POSITION);
                attrs.push_back(SUMO_ATTR_TYPE);
                attrs.push_back(SUMO_ATTR_SHAPE);
                attrs.push_back(SUMO_ATTR_RADIUS);
                break;
            case SUMO_TAG_LANE:
                attrs.push_back(SUMO_ATTR_ID);
                attrs.push_back(SUMO_ATTR_SPEED);
                attrs.push_back(SUMO_ATTR_ALLOW);
                attrs.push_back(SUMO_ATTR_DISALLOW);
                //attrs.push_back(SUMO_ATTR_PREFER);
                attrs.push_back(SUMO_ATTR_WIDTH);
                attrs.push_back(SUMO_ATTR_ENDOFFSET);
                attrs.push_back(SUMO_ATTR_INDEX); // read-only attribute
                break;
            case SUMO_TAG_POI:
                attrs.push_back(SUMO_ATTR_ID);
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(SUMO_ATTR_POSITION);
                attrs.push_back(SUMO_ATTR_TYPE);
                break;
            default:
                WRITE_WARNING("allowed attributes for tag '" +
                              toString(tag) + "' not defined");
        }
    }
    return _allowedAttributes[tag];
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedTags() {
    // define on first access
    if (_allowedTags.empty()) {
        _allowedTags.push_back(SUMO_TAG_JUNCTION);
        _allowedTags.push_back(SUMO_TAG_EDGE);
        _allowedTags.push_back(SUMO_TAG_LANE);
    }
    return _allowedTags;
}


bool
GNEAttributeCarrier::isNumerical(SumoXMLAttr attr) {
    // define on first access
    if (_numericalAttrs.empty()) {
        _numericalAttrs.insert(SUMO_ATTR_SPEED);
        _numericalAttrs.insert(SUMO_ATTR_PRIORITY);
        _numericalAttrs.insert(SUMO_ATTR_NUMLANES);
        _numericalAttrs.insert(SUMO_ATTR_LENGTH);
        _numericalAttrs.insert(SUMO_ATTR_WIDTH);
        _numericalAttrs.insert(SUMO_ATTR_ENDOFFSET);
        _numericalAttrs.insert(SUMO_ATTR_INDEX);
        _numericalAttrs.insert(SUMO_ATTR_RADIUS);
    }
    return _numericalAttrs.count(attr) == 1;
}


bool
GNEAttributeCarrier::isUnique(SumoXMLAttr attr) {
    // define on first access
    if (_uniqueAttrs.empty()) {
        _uniqueAttrs.insert(SUMO_ATTR_ID);
        _uniqueAttrs.insert(SUMO_ATTR_FROM);
        _uniqueAttrs.insert(SUMO_ATTR_TO);
        _uniqueAttrs.insert(SUMO_ATTR_SHAPE);
        _uniqueAttrs.insert(SUMO_ATTR_POSITION);
    }
    return _uniqueAttrs.count(attr) == 1;
}


const std::vector<std::string>&
GNEAttributeCarrier::discreteChoices(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (_discreteChoices.empty()) {
        std::vector<std::string> choices;
        choices = SUMOXMLDefinitions::NodeTypes.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            if (*it != toString(NODETYPE_DEAD_END_DEPRECATED)) {
                _discreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE].push_back(*it);
            }
        }

        choices = SUMOXMLDefinitions::LaneSpreadFunctions.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            _discreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE].push_back(*it);
        }
    }
    return _discreteChoices[tag][attr];
}

/****************************************************************************/

