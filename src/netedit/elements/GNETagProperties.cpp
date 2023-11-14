/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
// defines
// ===========================================================================

#define MAXNUMBEROFATTRIBUTES 128

// ===========================================================================
// included modules
// ===========================================================================

#include "GNETagProperties.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNETagProperties::GNETagProperties() {
}


GNETagProperties::GNETagProperties(const SumoXMLTag tag, const int tagType, const int tagProperty, const int tagParents,
                                   const int conflicts, const GUIIcon icon, const SumoXMLTag XMLTag, const std::string tooltip,
                                   const std::vector<SumoXMLTag> parentTags, const unsigned int backgroundColor,
                                   const std::string fieldString) :
    myTag(tag),
    myTagStr(toString(tag)),
    myTagType(tagType),
    myTagProperty(tagProperty),
    myTagParents(tagParents),
    myConflicts(conflicts),
    myIcon(icon),
    myXMLTag(XMLTag),
    myTooltipText(tooltip),
    myParentTags(parentTags),
    myFieldString(fieldString.empty() ? toString(tag) : fieldString),
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
    if (myTagType == -1) {
        throw ProcessError(TL("no tag type defined"));
    }
    if (myTagProperty == -1) {
        throw ProcessError(TL("no tag property defined"));
    }
    if (myTagParents == -1) {
        throw ProcessError(TL("no tag parent defined"));
    }
    if (myConflicts == -1) {
        throw ProcessError(TL("no conflict defined"));
    }
    // check that element must ist at least networkElement, Additional, or shape
    if (!isNetworkElement() && !isAdditionalElement() && !isDemandElement() && !isDataElement() && !isMeanData() && !isInternalLane()) {
        throw ProcessError(TL("no basic type property defined"));
    }
    // check that element only is networkElement, Additional, or shape at the same time
    if ((isNetworkElement() + isAdditionalElement() + isDemandElement() + isDataElement() + isMeanData()) > 1) {
        throw ProcessError(TL("multiple basic type properties defined"));
    }
    // check that element only is shape, TAZ, or wire at the same time
    if ((isShapeElement() + isTAZElement() + isWireElement()) > 1) {
        throw ProcessError(TL("element can be either shape or TAZ or wire element at the same time"));
    }
    // if element can mask the start and end position, check that bot attributes exist
    if (canMaskStartEndPos() && (!hasAttribute(SUMO_ATTR_STARTPOS) || !hasAttribute(SUMO_ATTR_ENDPOS))) {
        throw ProcessError(TL("If attributes mask the start and end position, both attributes have to be defined"));
    }
    // check that master tag is valid
    if (isChild() && myParentTags.empty()) {
        throw FormatException("Parent tags cannot be empty");
    }
    // check that master was defined
    if (!isChild() && !myParentTags.empty()) {
        throw FormatException("Element doesn't support parent elements");
    }
    // check reparent
    if (!isChild() && canBeReparent()) {
        throw FormatException("Only Child elements can be reparent");
    }
    // check vClass icons
    if (vClassIcon() && !hasAttribute(SUMO_ATTR_VCLASS)) {
        throw FormatException("Element require attribute SUMO_ATTR_VCLASS");
    }
    // check integrity of all attributes
    for (const auto& attributeProperty : myAttributeProperties) {
        attributeProperty.checkAttributeIntegrity();
        // check that if attribute is vehicle classes, own a combination of Allow/disallow attribute
        if (attributeProperty.isSVCPermission()) {
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


const std::string&
GNETagProperties::getTooltipText() const {
    return myTooltipText;
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
GNETagProperties::isJuPedSimElement() const {
    return (myTagType & JUPEDSIM) != 0;
}


bool
GNETagProperties::isType() const {
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
GNETagProperties::isVehicleStop() const {
    return (myTagType & VEHICLESTOP) != 0;
}


bool
GNETagProperties::isVehicleWaypoint() const {
    return (myTagType & VEHICLEWAYPOINT) != 0;
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
GNETagProperties::isContainer() const {
    return (myTagType & CONTAINER) != 0;
}


bool
GNETagProperties::isPlan() const {
    return isPlanPerson() || isPlanContainer();
}


bool
GNETagProperties::isPlanPerson() const {
    return (myTagType & PERSONPLAN) != 0;
}


bool
GNETagProperties::isPlanContainer() const {
    return (myTagType & CONTAINERPLAN) != 0;
}


bool
GNETagProperties::isPersonTrip() const {
    return (myTagType & PERSONTRIP) != 0;
}


bool
GNETagProperties::isPlanWalk() const {
    return (myTagType & WALK) != 0;
}


bool
GNETagProperties::isPlanRide() const {
    return (myTagType & RIDE) != 0;
}


bool
GNETagProperties::isPlanTransport() const {
    return (myTagType & TRANSPORT) != 0;
}


bool
GNETagProperties::isPlanTranship() const {
    return (myTagType & TRANSHIP) != 0;
}


bool
GNETagProperties::isPlanStop() const {
    return isPlanStopPerson() || isPlanStopContainer();
}


bool
GNETagProperties::isPlanStopPerson() const {
    return (myTagType & STOPPERSON) != 0;
}


bool
GNETagProperties::isPlanStopContainer() const {
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
GNETagProperties::vehicleRoute() const {
    return (myTagParents & VEHICLE_ROUTE) != 0;
}


bool
GNETagProperties::vehicleRouteEmbedded() const {
    return (myTagParents & VEHICLE_ROUTE_EMBEDDED) != 0;
}


bool
GNETagProperties::vehicleEdges() const {
    return (myTagParents & VEHICLE_EDGES) != 0;
}


bool
GNETagProperties::vehicleJunctions() const {
    return (myTagParents & VEHICLE_JUNCTIONS) != 0;
}


bool
GNETagProperties::vehicleTAZs() const {
    return (myTagParents & VEHICLE_TAZS) != 0;
}


bool
GNETagProperties::planConsecutiveEdges() const {
    return (myTagParents & PLAN_CONSECUTIVE_EDGES) != 0;
}


bool
GNETagProperties::planRoute() const {
    return (myTagParents & PLAN_ROUTE) != 0;
}


bool
GNETagProperties::planEdge() const {
    return (myTagParents & PLAN_EDGE) != 0;
}


bool
GNETagProperties::planBusStop() const {
    return (myTagParents & PLAN_BUSSTOP) != 0;
}


bool
GNETagProperties::planTrainStop() const {
    return (myTagParents & PLAN_TRAINSTOP) != 0;
}


bool
GNETagProperties::planContainerStop() const {
    return (myTagParents & PLAN_CONTAINERSTOP) != 0;
}


bool
GNETagProperties::planStoppingPlace() const {
    return planBusStop() || planTrainStop() || planContainerStop();
}


bool
GNETagProperties::planFromTo() const {
    return planFromEdge() || planToEdge() ||
           planFromJunction() || planToJunction() ||
           planFromTAZ() || planToTAZ() ||
           planFromStoppingPlace() || planToStoppingPlace();
}


bool
GNETagProperties::planFromEdge() const {
    return (myTagParents & PLAN_FROM_EDGE) != 0;
}


bool
GNETagProperties::planFromTAZ() const {
    return (myTagParents & PLAN_FROM_TAZ) != 0;
}


bool
GNETagProperties::planFromJunction() const {
    return (myTagParents & PLAN_FROM_JUNCTION) != 0;
}


bool
GNETagProperties::planFromStoppingPlace() const {
    return planFromBusStop() || planFromTrainStop() || planFromContainerStop();
}


bool
GNETagProperties::planFromBusStop() const {
    return (myTagParents & PLAN_FROM_BUSSTOP) != 0;
}


bool
GNETagProperties::planFromTrainStop() const {
    return (myTagParents & PLAN_FROM_TRAINSTOP) != 0;
}


bool
GNETagProperties::planFromContainerStop() const {
    return (myTagParents & PLAN_FROM_CONTAINERSTOP) != 0;
}


bool
GNETagProperties::planToEdge() const {
    return (myTagParents & PLAN_TO_EDGE) != 0;
}


bool
GNETagProperties::planToTAZ() const {
    return (myTagParents & PLAN_TO_TAZ) != 0;
}


bool
GNETagProperties::planToJunction() const {
    return (myTagParents & PLAN_TO_JUNCTION) != 0;
}


bool
GNETagProperties::planToStoppingPlace() const {
    return planToBusStop() || planToTrainStop() || planToContainerStop();
}


bool
GNETagProperties::planToBusStop() const {
    return (myTagParents & PLAN_TO_BUSSTOP) != 0;
}


bool
GNETagProperties::planToTrainStop() const {
    return (myTagParents & PLAN_TO_TRAINSTOP) != 0;
}


bool
GNETagProperties::planToContainerStop() const {
    return (myTagParents & PLAN_TO_CONTAINERSTOP) != 0;
}


bool
GNETagProperties::isChild() const {
    return (myTagProperty & CHILD) != 0;
}


bool
GNETagProperties::isSymbol() const {
    return (myTagProperty & SYMBOL) != 0;
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
GNETagProperties::requireProj() const {
    return (myTagProperty & REQUIRE_PROJ) != 0;
}


bool
GNETagProperties::vClassIcon() const {
    return (myTagProperty & VCLASS_ICON) != 0;
}

/****************************************************************************/
