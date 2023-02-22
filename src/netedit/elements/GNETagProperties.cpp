/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNETagProperties.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for tag properties used in GNEAttributeCarrier
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
    myIcon(GUIIcon::EMPTY),
    myXMLTag(SUMO_TAG_NOTHING),
    myBackgroundColor(0) {
}


GNETagProperties::GNETagProperties(const SumoXMLTag tag, const int tagType, const int tagProperty, const GUIIcon icon, const SumoXMLTag XMLTag,
                                   const std::vector<SumoXMLTag> parentTags, const unsigned int backgroundColor) :
    myTag(tag),
    myTagStr(toString(tag)),
    myTagType(tagType),
    myTagProperty(tagProperty),
    myIcon(icon),
    myXMLTag(XMLTag),
    myParentTags(parentTags),
    myFieldString(toString(tag)),
    myBackgroundColor(backgroundColor) {
}


GNETagProperties::~GNETagProperties() {}


SumoXMLTag
GNETagProperties::getTag() const {
    return myTag;
}


Supermode
GNETagProperties::getSupermode() const {
    if (isDemandElement()) {
        return Supermode::DEMAND;
    } else if (isDataElement() || isMeanData()) {
        return Supermode::DATA;
    } else {
        return Supermode::NETWORK;
    }
}


const std::string&
GNETagProperties::getTagStr() const {
    return myTagStr;
}


void
GNETagProperties::checkTagIntegrity() const {
    // check integrity only in debug mode
#ifdef DEBUG
    // check that element must ist at least networkElement, Additional, or shape
    if (!isNetworkElement() && !isAdditionalElement() && !isDemandElement() && !isDataElement() && !isMeanData() && !isInternalLane()) {
        throw ProcessError(TL("element must be at least networkElement, additional, TAZ, demandElement, dataElement or meanData"));
    }
    // check that element only is networkElement, Additional, or shape at the same time
    if ((isNetworkElement() + isAdditionalElement() + isDemandElement() + isDataElement() + isMeanData()) > 1) {
        throw ProcessError(TL("element can be only a networkElement, additional, demandElement, dataElement or meanData at the same time"));
    }
    // check that element only is shape, TAZ, or wire at the same time
    if ((isShapeElement() + isTAZElement() + isWireElement()) > 1) {
        throw ProcessError(TL("element can be only a shape, TAZ or wire element at the same time"));
    }
    // if element can mask the start and end position, check that bot attributes exist
    if (canMaskStartEndPos() && (!hasAttribute(SUMO_ATTR_STARTPOS) || !hasAttribute(SUMO_ATTR_ENDPOS))) {
        throw ProcessError(TL("If attributes mask the start and end position, both attributes have to be defined"));
    }
    // check that master tag is valid
    if (isChild() && myParentTags.empty()) {
        throw FormatException("Master tags cannot be empty");
    }
    // check that master was defined
    if (!isChild() && !myParentTags.empty()) {
        throw FormatException("Tag doesn't support master elements");
    }
    // check reparent
    if (!isChild() && canBeReparent()) {
        throw FormatException("Only Child elements can be reparent");
    }
    // check vClass icons
    if (vClassIcon() && !hasAttribute(SUMO_ATTR_VCLASS)) {
        throw FormatException("Tag require attribute SUMO_ATTR_VCLASS");
    }
    // check integrity of all attributes
    for (const auto& attributeProperty : myAttributeProperties) {
        attributeProperty.checkAttributeIntegrity();
        // check that if attribute is vehicle classes, own a combination of Allow/disallow attribute
        if (attributeProperty.isVClasses()) {
            if ((attributeProperty.getAttr() != SUMO_ATTR_ALLOW) && (attributeProperty.getAttr() != SUMO_ATTR_DISALLOW) &&
                    (attributeProperty.getAttr() != SUMO_ATTR_CHANGE_LEFT) && (attributeProperty.getAttr() != SUMO_ATTR_CHANGE_RIGHT) &&
                    (attributeProperty.getAttr() != GNE_ATTR_STOPOEXCEPTION)) {
                throw ProcessError(TL("Attributes aren't combinables"));
            } else if ((attributeProperty.getAttr() == SUMO_ATTR_ALLOW) && !hasAttribute(SUMO_ATTR_DISALLOW)) {
                throw ProcessError(TL("allow need a disallow attribute in the same tag"));
            } else if ((attributeProperty.getAttr() == SUMO_ATTR_DISALLOW) && !hasAttribute(SUMO_ATTR_ALLOW)) {
                throw ProcessError(TL("disallow need an allow attribute in the same tag"));
            }
        }
    }
#endif // DEBUG
}


const std::string&
GNETagProperties::getDefaultValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty.getAttr() == attr) {
            if (!attributeProperty.hasDefaultValue()) {
                throw ProcessError(TLF("attribute '%' doesn't have a default value", attributeProperty.getAttrStr()));
            } else {
                return attributeProperty.getDefaultValue();
            }
        }
    }
    throw ProcessError(TLF("Attribute '%' not defined", toString(attr)));
}


void
GNETagProperties::addAttribute(const GNEAttributeProperties& attributeProperty) {
    if ((myAttributeProperties.size() + 1) >= MAXNUMBEROFATTRIBUTES) {
        throw ProcessError(TLF("Maximum number of attributes for tag % exceeded", attributeProperty.getAttrStr()));
    } else {
        // Check that attribute wasn't already inserted
        for (const auto& attrProperty : myAttributeProperties) {
            if (attributeProperty.getAttr() == attrProperty.getAttr()) {
                throw ProcessError(TLF("Attribute '%' already inserted", attributeProperty.getAttrStr()));
            }
        }
        // insert AttributeProperties in vector
        myAttributeProperties.push_back(attributeProperty);
        myAttributeProperties.back().setTagPropertyParent(this);
    }
}


const std::string&
GNETagProperties::getFieldString() const {
    return myFieldString;
}


void
GNETagProperties::setFieldString(const std::string& fieldString) {
    myFieldString = fieldString;
}


unsigned int
GNETagProperties::getBackGroundColor() const {
    return myBackgroundColor;
}


const GNEAttributeProperties&
GNETagProperties::getAttributeProperties(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if ((attributeProperty.getAttr() == attr) || (attributeProperty.hasAttrSynonym() && (attributeProperty.getAttrSynonym() == attr))) {
            return attributeProperty;
        }
    }
    // throw error if these attribute doesn't exist
    throw ProcessError(TLF("Attribute '%' doesn't exist", toString(attr)));
}


std::vector<GNEAttributeProperties>::const_iterator
GNETagProperties::begin() const {
    return myAttributeProperties.begin();
}


std::vector<GNEAttributeProperties>::const_iterator
GNETagProperties::end() const {
    return myAttributeProperties.end();
}


const GNEAttributeProperties&
GNETagProperties::at(int index) const {
    return myAttributeProperties.at(index);
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
GNETagProperties::getXMLTag() const {
    return myXMLTag;
}


const std::vector<SumoXMLTag>&
GNETagProperties::getParentTags() const {
    return myParentTags;
}


bool
GNETagProperties::hasAttribute(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty.getAttr() == attr) {
            return true;
        }
    }
    return false;
}


bool
GNETagProperties::isNetworkElement() const {
    return (myTagType & NETWORKELEMENT) != 0;
}


bool
GNETagProperties::isAdditionalElement() const {
    return (myTagType & ADDITIONALELEMENT) != 0;
}


bool
GNETagProperties::isAdditionalPureElement() const {
    return (isAdditionalElement() && !isShapeElement() && !isTAZElement() && !isWireElement());
}


bool
GNETagProperties::isDemandElement() const {
    return (myTagType & DEMANDELEMENT) != 0;
}


bool
GNETagProperties::isDataElement() const {
    return (myTagType & DATAELEMENT) != 0;
}


bool
GNETagProperties::isStoppingPlace() const {
    return (myTagType & STOPPINGPLACE) != 0;
}


bool
GNETagProperties::isDetector() const {
    return (myTagType & DETECTOR) != 0;
}


bool
GNETagProperties::isCalibrator() const {
    return (myTagType & CALIBRATOR) != 0;
}


bool
GNETagProperties::isShapeElement() const {
    return (myTagType & SHAPE) != 0;
}


bool
GNETagProperties::isTAZElement() const {
    return (myTagType & TAZELEMENT) != 0;
}


bool
GNETagProperties::isWireElement() const {
    return (myTagType & WIRE) != 0;
}


bool
GNETagProperties::isVehicleType() const {
    return (myTagType & VTYPE) != 0;
}


bool
GNETagProperties::isVehicle() const {
    return (myTagType & VEHICLE) != 0;
}


bool
GNETagProperties::isRoute() const {
    return (myTagType & ROUTE) != 0;
}


bool
GNETagProperties::isStop() const {
    return (myTagType & STOP) != 0;
}


bool
GNETagProperties::isWaypoint() const {
    return (myTagType & WAYPOINT) != 0;
}


bool
GNETagProperties::isFlow() const {
    return (myTagType & FLOW) != 0;
}


bool
GNETagProperties::isPerson() const {
    return (myTagType & PERSON) != 0;
}


bool
GNETagProperties::isPersonPlan() const {
    return (myTagType & PERSONPLAN) != 0;
}


bool
GNETagProperties::isPersonTrip() const {
    return (myTagType & PERSONTRIP) != 0;
}


bool
GNETagProperties::isWalk() const {
    return (myTagType & WALK) != 0;
}


bool
GNETagProperties::isRide() const {
    return (myTagType & RIDE) != 0;
}


bool
GNETagProperties::isStopPerson() const {
    return (myTagType & STOPPERSON) != 0;
}


bool
GNETagProperties::isContainer() const {
    return (myTagType & CONTAINER) != 0;
}


bool
GNETagProperties::isContainerPlan() const {
    return (myTagType & CONTAINERPLAN) != 0;
}


bool
GNETagProperties::isTransportPlan() const {
    return (myTagType & TRANSPORT) != 0;
}

bool
GNETagProperties::isTranshipPlan() const {
    return (myTagType & TRANSHIP) != 0;
}



bool
GNETagProperties::isStopContainer() const {
    return (myTagType & STOPCONTAINER) != 0;
}


bool
GNETagProperties::isGenericData() const {
    return (myTagType & GENERICDATA) != 0;
}


bool
GNETagProperties::isMeanData() const {
    return (myTagType & MEANDATA) != 0;
}


bool
GNETagProperties::isChild() const {
    return (myTagProperty & CHILD) != 0;
}


bool
GNETagProperties::isSymbol() const {
    return (myTagType & SYMBOL) != 0;
}


bool
GNETagProperties::isInternalLane() const {
    return (myTagType & INTERNALLANE) != 0;
}


bool
GNETagProperties::isDrawable() const {
    return (myTagProperty & NOTDRAWABLE) == 0;
}


bool
GNETagProperties::isSelectable() const {
    // note: By default all elements can be selected, except Tags with "NOTSELECTABLE"
    return (myTagProperty & NOTSELECTABLE) == 0;
}


bool
GNETagProperties::canCloseShape() const {
    return (myTagProperty & CLOSESHAPE) != 0;
}


bool
GNETagProperties::hasGEOShape() const {
    return (myTagProperty & GEOSHAPE) != 0;
}


bool
GNETagProperties::hasDialog() const {
    return (myTagProperty & DIALOG) != 0;
}


bool
GNETagProperties::hasParameters() const {
    // note: By default all elements support parameters, except Tags with "NOPARAMETERS"
    return (myTagProperty & NOPARAMETERS) == 0;
}


bool
GNETagProperties::isPlacedInRTree() const {
    return (myTagProperty & RTREE) != 0;
}


bool
GNETagProperties::canBeReparent() const {
    return (myTagProperty & REPARENT) != 0;
}


bool
GNETagProperties::canMaskStartEndPos() const {
    return (myTagProperty & MASKSTARTENDPOS) != 0;
}


bool
GNETagProperties::canCenterCameraAfterCreation() const {
    return (myTagProperty & CENTERAFTERCREATION) != 0;
}


bool
GNETagProperties::hasEmbeddedRoute() const {
    return (myTagProperty & EMBEDDED_ROUTE) != 0;
}


bool
GNETagProperties::requireProj() const {
    return (myTagProperty & REQUIRE_PROJ) != 0;
}


bool
GNETagProperties::vClassIcon() const {
    return (myTagProperty & VCLASS_ICON) != 0;
}

/****************************************************************************/
