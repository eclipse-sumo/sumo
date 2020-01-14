/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAttributeCarrier.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include "GNETagProperties.h"


// ===========================================================================
// static members
// ===========================================================================

const size_t GNETagProperties::MAXNUMBEROFATTRIBUTES = 128;

// ===========================================================================
// method definitions
// ===========================================================================

GNETagProperties::GNETagProperties() :
    myTag(SUMO_TAG_NOTHING),
    myTagType(0),
    myTagProperty(0),
    myIcon(ICON_EMPTY),
    myParentTag(SUMO_TAG_NOTHING),
    myTagSynonym(SUMO_TAG_NOTHING) {
}


GNETagProperties::GNETagProperties(SumoXMLTag tag, int tagType, int tagProperty, GUIIcon icon, SumoXMLTag parentTag, SumoXMLTag tagSynonym) :
    myTag(tag),
    myTagStr(toString(tag)),
    myTagType(tagType),
    myTagProperty(tagProperty),
    myIcon(icon),
    myParentTag(parentTag),
    myTagSynonym(tagSynonym) {
}


GNETagProperties::~GNETagProperties() {}


SumoXMLTag
GNETagProperties::getTag() const {
    return myTag;
}


const std::string&
GNETagProperties::getTagStr() const {
    return myTagStr;
}


void
GNETagProperties::checkTagIntegrity() const {
    // check that element must ist at least networkElement, Additional, or shape
    if (!isNetworkElement() && !isAdditionalElement() && !isShape() && !isTAZ() && !isDemandElement()) {
        throw ProcessError("element must be at leas networkElement, additional, TAZ, shape or demandElement");
    }
    // check that element only is networkElement, Additional, or shape at the same time
    if ((isNetworkElement() + isAdditionalElement() + isShape() + isTAZ() + isDemandElement()) > 1) {
        throw ProcessError("element can be only a networkElement, additional, shape or demandElement at the same time");
    }
    // if element can mask the start and end position, check that bot attributes exist
    if (canMaskStartEndPos() && (!hasAttribute(SUMO_ATTR_STARTPOS) || !hasAttribute(SUMO_ATTR_ENDPOS))) {
        throw ProcessError("If attribute mask the start and end position, bot attribute has to be defined");
    }
    // check that synonym tag isn't nothing
    if (hasTagSynonym() && (myTagSynonym == SUMO_TAG_NOTHING)) {
        throw FormatException("Synonym tag cannot be nothing");
    }
    // check that synonym was defined
    if (!hasTagSynonym() && (myTagSynonym != SUMO_TAG_NOTHING)) {
        throw FormatException("Tag doesn't support synonyms");
    }
    // check integrity of all attributes
    for (auto i : myAttributeProperties) {
        i.checkAttributeIntegrity();
        // check that if attribute is vehicle classes, own a combination of Allow/disallow attibute
        if (i.isVClasses()) {
            if ((i.getAttr() != SUMO_ATTR_ALLOW) && (i.getAttr() != SUMO_ATTR_DISALLOW)) {
                throw ProcessError("Attributes aren't combinables");
            } else if ((i.getAttr() == SUMO_ATTR_ALLOW) && !hasAttribute(SUMO_ATTR_DISALLOW)) {
                throw ProcessError("allow need a disallow attribute in the same tag");
            } else if ((i.getAttr() == SUMO_ATTR_DISALLOW) && !hasAttribute(SUMO_ATTR_ALLOW)) {
                throw ProcessError("disallow need an allow attribute in the same tag");
            }
        }
    }
}


const std::string&
GNETagProperties::getDefaultValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& i : myAttributeProperties) {
        if (i.getAttr() == attr) {
            if (!i.hasStaticDefaultValue()) {
                throw ProcessError("attribute '" + i.getAttrStr() + "' doesn't have a default value");
            } else {
                return i.getDefaultValue();
            }
        }
    }
    throw ProcessError("Attribute '" + toString(attr) + "' not defined");
}


void
GNETagProperties::addAttribute(const GNEAttributeProperties& attributeProperty) {
    if (isAttributeDeprecated(attributeProperty.getAttr())) {
        throw ProcessError("Attribute '" + attributeProperty.getAttrStr() + "' is deprecated and cannot be inserted");
    } else if ((myAttributeProperties.size() + 1) >= MAXNUMBEROFATTRIBUTES) {
        throw ProcessError("Maximum number of attributes for tag " + attributeProperty.getAttrStr() + " exceeded");
    } else {
        // Check that attribute wasn't already inserted
        for (auto i : myAttributeProperties) {
            if (i.getAttr() == attributeProperty.getAttr()) {
                throw ProcessError("Attribute '" + attributeProperty.getAttrStr() + "' already inserted");
            }
        }
        // insert AttributeProperties in vector
        myAttributeProperties.push_back(attributeProperty);
        myAttributeProperties.back().setTagPropertyParent(this);
    }
}


void
GNETagProperties::addDeprecatedAttribute(SumoXMLAttr attr) {
    // Check that attribute wasn't already inserted
    for (auto i : myAttributeProperties) {
        if (i.getAttr() == attr) {
            throw ProcessError("Attribute '" + toString(attr) + "' is deprecated but was inserted in list of attributes");
        }
    }
    // add it into myDeprecatedAttributes
    myDeprecatedAttributes.push_back(attr);
}


const GNEAttributeProperties&
GNETagProperties::getAttributeProperties(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& i : myAttributeProperties) {
        if ((i.getAttr() == attr) || (i.hasAttrSynonym() && (i.getAttrSynonym() == attr))) {
            return i;
        }
    }
    // throw error if these attribute doesn't exist
    throw ProcessError("Attribute '" + toString(attr) + "' doesn't exist");
}


std::vector<GNEAttributeProperties>::const_iterator
GNETagProperties::begin() const {
    return myAttributeProperties.begin();
}


std::vector<GNEAttributeProperties>::const_iterator
GNETagProperties::end() const {
    return myAttributeProperties.end();
}


int
GNETagProperties::getNumberOfAttributes() const {
    return (int)myAttributeProperties.size();
}


GUIIcon
GNETagProperties::getGUIIcon() const {
    return myIcon;
}


SumoXMLTag
GNETagProperties::getParentTag() const {
    if (hasParent()) {
        return myParentTag;
    } else {
        throw ProcessError("Tag doesn't have parent");
    }
}


SumoXMLTag
GNETagProperties::getTagSynonym() const {
    if (hasTagSynonym()) {
        return myTagSynonym;
    } else {
        throw ProcessError("Tag doesn't have synonym");
    }
}


bool
GNETagProperties::hasAttribute(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& i : myAttributeProperties) {
        if (i.getAttr() == attr) {
            return true;
        }
    }
    return false;
}


bool
GNETagProperties::isNetworkElement() const {
    return (myTagType & TAGTYPE_NETWORKELEMENT) != 0;
}


bool
GNETagProperties::isAdditionalElement() const {
    return (myTagType & TAGTYPE_ADDITIONALELEMENT) != 0;
}

bool
GNETagProperties::isShape() const {
    return (myTagType & TAGTYPE_SHAPE) != 0;
}


bool
GNETagProperties::isTAZ() const {
    return (myTagType & TAGTYPE_TAZ) != 0;
}


bool
GNETagProperties::isDemandElement() const {
    return (myTagType & TAGTYPE_DEMANDELEMENT) != 0;
}


bool
GNETagProperties::isDataElement() const {
    return (myTagType & TAGTYPE_DATAELEMENT) != 0;
}


bool
GNETagProperties::isStoppingPlace() const {
    return (myTagType & TAGTYPE_STOPPINGPLACE) != 0;
}


bool
GNETagProperties::isDetector() const {
    return (myTagType & TAGTYPE_DETECTOR) != 0;
}


bool
GNETagProperties::isVehicleType() const {
    return (myTagType & TAGTYPE_VTYPE) != 0;
}


bool
GNETagProperties::isVehicle() const {
    return (myTagType & TAGTYPE_VEHICLE) != 0;
}

bool
GNETagProperties::isRoute() const {
    return (myTagType & TAGTYPE_ROUTE) != 0;
}


bool
GNETagProperties::isStop() const {
    return (myTagType & TAGTYPE_STOP) != 0;
}


bool
GNETagProperties::isPerson() const {
    return (myTagType & TAGTYPE_PERSON) != 0;
}


bool
GNETagProperties::isPersonPlan() const {
    return (myTagType & TAGTYPE_PERSONPLAN) != 0;
}


bool
GNETagProperties::isPersonTrip() const {
    return (myTagType & TAGTYPE_PERSONTRIP) != 0;
}


bool
GNETagProperties::isWalk() const {
    return (myTagType & TAGTYPE_WALK) != 0;
}


bool
GNETagProperties::isRide() const {
    return (myTagType & TAGTYPE_RIDE) != 0;
}


bool
GNETagProperties::isPersonStop() const {
    return (myTagType & TAGTYPE_PERSONSTOP) != 0;
}


bool
GNETagProperties::isDrawable() const {
    return (myTagProperty & TAGPROPERTY_DRAWABLE) != 0;
}


bool
GNETagProperties::isSelectable() const {
    return (myTagProperty & TAGPROPERTY_SELECTABLE) != 0;
}


bool
GNETagProperties::canBlockMovement() const {
    return (myTagProperty & TAGPROPERTY_BLOCKMOVEMENT) != 0;
}


bool
GNETagProperties::canBlockShape() const {
    return (myTagProperty & TAGPROPERTY_BLOCKSHAPE) != 0;
}


bool
GNETagProperties::canCloseShape() const {
    return (myTagProperty & TAGPROPERTY_CLOSESHAPE) != 0;
}


bool
GNETagProperties::hasGEOPosition() const {
    return (myTagProperty & TAGPROPERTY_GEOPOSITION) != 0;
}


bool
GNETagProperties::hasGEOShape() const {
    return (myTagProperty & TAGPROPERTY_GEOSHAPE) != 0;
}


bool
GNETagProperties::hasParent() const {
    return (myTagProperty & TAGPROPERTY_PARENT) != 0;
}


bool
GNETagProperties::hasTagSynonym() const {
    return (myTagProperty & TAGPROPERTY_SYNONYM) != 0;
}


bool
GNETagProperties::hasDialog() const {
    return (myTagProperty & TAGPROPERTY_DIALOG) != 0;
}


bool
GNETagProperties::hasMinimumNumberOfChildren() const {
    return (myTagProperty & TAGPROPERTY_MINIMUMCHILDREN) != 0;
}


bool
GNETagProperties::hasParameters() const {
    // note: By default all Tags supports parameters, except Tags with "TAGPROPERTY_NOPARAMETERS"
    return (myTagProperty & TAGPROPERTY_NOPARAMETERS) == 0;
}


bool
GNETagProperties::isPlacedInRTree() const {
    return (myTagProperty & TAGPROPERTY_RTREE) != 0;
}


bool
GNETagProperties::canBeSortedManually() const {
    return (myTagProperty & TAGPROPERTY_SORTINGCHILDREN) != 0;
}


bool
GNETagProperties::canBeReparent() const {
    return (myTagProperty & TAGPROPERTY_REPARENT) != 0;
}


bool
GNETagProperties::canAutomaticSortChildren() const {
    return (myTagProperty & TAGPROPERTY_AUTOMATICSORTING) != 0;
}


bool
GNETagProperties::canWriteChildrenSeparate() const {
    return (myTagProperty & TAGPROPERTY_WRITECHILDRENSEPARATE) != 0;
}


bool
GNETagProperties::canMaskStartEndPos() const {
    return (myTagProperty & TAGPROPERTY_MASKSTARTENDPOS) != 0;
}


bool
GNETagProperties::canMaskXYZPositions() const {
    return (myTagProperty & TAGPROPERTY_MASKXYZPOSITION) != 0;
}


bool
GNETagProperties::canCenterCameraAfterCreation() const {
    return (myTagProperty & TAGPROPERTY_CENTERAFTERCREATION) != 0;
}


bool
GNETagProperties::isAttributeDeprecated(SumoXMLAttr attr) const {
    return (std::find(myDeprecatedAttributes.begin(), myDeprecatedAttributes.end(), attr) != myDeprecatedAttributes.end());
}

/****************************************************************************/
