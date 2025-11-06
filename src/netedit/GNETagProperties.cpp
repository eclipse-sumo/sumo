/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include "GNETagProperties.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNETagProperties::GNETagProperties(const SumoXMLTag tag, GNETagProperties* parent, const Type tagType,
                                   const Property tagProperty, const Over myTagOver, const Conflicts conflicts,
                                   const GUIIcon icon, const GUIGlObjectType GLType, const SumoXMLTag XMLTag,
                                   const std::string tooltip, const std::vector<SumoXMLTag> XMLParentTags,
                                   const unsigned int backgroundColor, const std::string selectorText) :
    myTag(tag),
    myTagStr(toString(tag)),
    myParent(parent),
    myTagType(tagType),
    myTagProperty(tagProperty),
    myTagOver(myTagOver),
    myConflicts(conflicts),
    myIcon(icon),
    myGLType(GLType),
    myXMLTag(XMLTag),
    myTooltipText(tooltip),
    myXMLParentTags(XMLParentTags),
    mySelectorText(selectorText.empty() ? toString(tag) : selectorText),
    myBackgroundColor(backgroundColor) {
    if (parent) {
        parent->addChild(this);
    }
}


GNETagProperties::GNETagProperties(const SumoXMLTag tag, GNETagProperties* parent, const GUIIcon icon, const std::string tooltip,
                                   const unsigned int backgroundColor, const std::string selectorText) :
    myTag(tag),
    myTagStr(toString(tag)),
    myParent(parent),
    myTagProperty(Property::HIERARCHICAL),
    myIcon(icon),
    myXMLTag(tag),
    myTooltipText(tooltip),
    mySelectorText(selectorText.empty() ? toString(tag) : selectorText),
    myBackgroundColor(backgroundColor) {
    if (parent) {
        parent->addChild(this);
    }
}


GNETagProperties::~GNETagProperties() {
    for (const auto& attrProperties : myAttributeProperties) {
        delete attrProperties;
    }
}


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
    // check integrity only in debug mode
#ifdef DEBUG
    // check that this edge has parents (Except supermodes)
    if (myTag == SUMO_TAG_ROOTFILE) {
        if (myParent != nullptr) {
            throw ProcessError("Root parent must be empty");
        }
    } else if (myParent == nullptr) {
        throw ProcessError("No parent defined");
    }
    // check network parents
    if (isNetworkElement() && (myParent->getTag() != SUMO_TAG_NET)) {
        throw ProcessError("Invalid network element parent");
    }
    // check additional parents
    if (isStoppingPlace()) {
        if (myParent->getTag() != GNE_TAG_STOPPINGPLACES) {
            throw ProcessError("Invalid stoppingPlace parent");
        }
    } else if (isDetector()) {
        if (myParent->getTag() != GNE_TAG_DETECTORS) {
            throw ProcessError("Invalid detector parent");
        }
    } else if (isWireElement()) {
        if (myParent->getTag() != GNE_TAG_WIRES) {
            throw ProcessError("Invalid wire parent");
        }
    } else if (isJuPedSimElement()) {
        if (myParent->getTag() != GNE_TAG_JUPEDSIM) {
            throw ProcessError("Invalid juPedSim parent");
        }
    } else if (isTAZElement()) {
        if (myParent->getTag() != GNE_TAG_TAZS) {
            throw ProcessError("Invalid TAZ parent");
        }
    } else if (isShapeElement()) {
        if (myParent->getTag() != GNE_TAG_SHAPES) {
            throw ProcessError("Invalid shape parent");
        }
    } else if (isAdditionalElement()) {
        if (myParent->getTag() != SUMO_TAG_VIEWSETTINGS_ADDITIONALS) {
            throw ProcessError("Invalid additional parent");
        }
    }
    // check demand parents
    if (isVehicle()) {
        if (myParent->getTag() != SUMO_TAG_VIEWSETTINGS_VEHICLES) {
            throw ProcessError("Invalid vehicle parent");
        }
    } else if (isVehicleStop()) {
        if (myParent->getTag() != GNE_TAG_STOPS) {
            throw ProcessError("Invalid vehicle stop parent");
        }
    } else if (isPlanPersonTrip()) {
        if (myParent->getTag() != GNE_TAG_PERSONTRIPS) {
            throw ProcessError("Invalid person trip parent");
        }
    } else if (isPlanRide()) {
        if (myParent->getTag() != GNE_TAG_RIDES) {
            throw ProcessError("Invalid ride parent");
        }
    } else if (isPlanWalk()) {
        if (myParent->getTag() != GNE_TAG_WALKS) {
            throw ProcessError("Invalid walk parent");
        }
    } else if (isPlanStopPerson()) {
        if (myParent->getTag() != GNE_TAG_PERSONSTOPS) {
            throw ProcessError("Invalid person stop parent");
        }
    } else if (isPlanPerson()) {
        if (myParent->getTag() != GNE_TAG_PERSONPLANS) {
            throw ProcessError("Invalid person plan parent");
        }
    } else if (isPlanTransport()) {
        if (myParent->getTag() != GNE_TAG_TRANSPORTS) {
            throw ProcessError("Invalid ride parent");
        }
    } else if (isPlanTranship()) {
        if (myParent->getTag() != GNE_TAG_TRANSHIPS) {
            throw ProcessError("Invalid walk parent");
        }
    } else if (isPlanStopContainer()) {
        if (myParent->getTag() != GNE_TAG_CONTAINERSTOPS) {
            throw ProcessError("Invalid container stop parent");
        }
    } else if (isPlanContainer()) {
        if (myParent->getTag() != GNE_TAG_CONTAINERPLANS) {
            throw ProcessError("Invalid container plan parent");
        }
    } else if (isDemandElement()) {
        if (myParent->getTag() != GNE_TAG_SUPERMODE_DEMAND) {
            throw ProcessError("Invalid supermode demand parent");
        }
    }
    // check data parents
    if (isGenericData()) {
        if (myParent->getTag() != GNE_TAG_DATAS) {
            throw ProcessError("Invalid generic data parent");
        }
    } else if (isDataElement()) {
        if (myParent->getTag() != GNE_TAG_SUPERMODE_DATA) {
            throw ProcessError("Invalid supermode data parent");
        }
    }
    // check that element must ist at least networkElement, Additional, or shape
    if (!isNetworkElement() && !isAdditionalElement() && !isDemandElement() && !isDataElement() && !isMeanData() && !isInternalLane() && !isOtherElement()) {
        throw ProcessError("no basic type property defined");
    }
    // check that element only is networkElement, Additional, or shape at the same time
    if ((isNetworkElement() + isAdditionalElement() + isDemandElement() + isDataElement() + isMeanData() + isOtherElement()) > 1) {
        throw ProcessError("multiple basic type properties defined");
    }
    // check that element only is shape, TAZ, or wire at the same time
    if ((isShapeElement() + isTAZElement() + isWireElement()) > 1) {
        throw ProcessError("element can be either shape or TAZ or wire element at the same time");
    }
    // check that master tag is valid
    if (isChild() && myXMLParentTags.empty()) {
        throw FormatException("Parent tags cannot be empty");
    }
    // check that master was defined
    if (!isChild() && !myXMLParentTags.empty()) {
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
    // check glType
    if (!isHierarchicalTag() && (myGLType == GUIGlObjectType::GLO_MAX)) {
        throw FormatException("Only hierarchical tags can have a GLType GLO_MAX");
    }
    // check integrity of all attributes
    for (const auto& attributeProperty : myAttributeProperties) {
        attributeProperty->checkAttributeIntegrity();
        // check that if attribute is vehicle classes, own a combination of Allow/disallow attribute
        if (attributeProperty->isSVCPermission()) {
            if ((attributeProperty->getAttr() != SUMO_ATTR_ALLOW) && (attributeProperty->getAttr() != SUMO_ATTR_DISALLOW) &&
                    (attributeProperty->getAttr() != SUMO_ATTR_CHANGE_LEFT) && (attributeProperty->getAttr() != SUMO_ATTR_CHANGE_RIGHT) &&
                    (attributeProperty->getAttr() != GNE_ATTR_STOPOEXCEPTION)) {
                throw ProcessError("Attributes aren't combinables");
            } else if ((attributeProperty->getAttr() == SUMO_ATTR_ALLOW) && !hasAttribute(SUMO_ATTR_DISALLOW)) {
                throw ProcessError("allow need a disallow attribute in the same tag");
            } else if ((attributeProperty->getAttr() == SUMO_ATTR_DISALLOW) && !hasAttribute(SUMO_ATTR_ALLOW)) {
                throw ProcessError("disallow need an allow attribute in the same tag");
            }
        }
    }
#endif // DEBUG
}


const std::string&
GNETagProperties::getDefaultStringValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty->getAttr() == attr) {
            return attributeProperty->getDefaultStringValue();
        }
    }
    throw ProcessError(TLF("Attribute '%' not defined", toString(attr)));
}


int
GNETagProperties::getDefaultIntValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty->getAttr() == attr) {
            return attributeProperty->getDefaultIntValue();
        }
    }
    throw ProcessError(TLF("Attribute '%' not defined", toString(attr)));
}


double
GNETagProperties::getDefaultDoubleValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty->getAttr() == attr) {
            return attributeProperty->getDefaultDoubleValue();
        }
    }
    throw ProcessError(TLF("Attribute '%' not defined", toString(attr)));
}


SUMOTime
GNETagProperties::getDefaultTimeValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty->getAttr() == attr) {
            return attributeProperty->getDefaultTimeValue();
        }
    }
    throw ProcessError(TLF("Attribute '%' not defined", toString(attr)));
}


bool
GNETagProperties::getDefaultBoolValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty->getAttr() == attr) {
            return attributeProperty->getDefaultBoolValue();
        }
    }
    throw ProcessError(TLF("Attribute '%' not defined", toString(attr)));
}


const RGBColor&
GNETagProperties::getDefaultColorValue(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty->getAttr() == attr) {
            return attributeProperty->getDefaultColorValue();
        }
    }
    throw ProcessError(TLF("Attribute '%' not defined", toString(attr)));
}


const std::string&
GNETagProperties::getSelectorText() const {
    return mySelectorText;
}


const std::string&
GNETagProperties::getTooltipText() const {
    return myTooltipText;
}


unsigned int
GNETagProperties::getBackGroundColor() const {
    return myBackgroundColor;
}


const std::vector<const GNEAttributeProperties*>&
GNETagProperties::getAttributeProperties() const {
    return myAttributeProperties;
}


const GNEAttributeProperties*
GNETagProperties::getAttributeProperties(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if ((attributeProperty->getAttr() == attr) || (attributeProperty->hasAttrSynonym() && (attributeProperty->getAttrSynonym() == attr))) {
            return attributeProperty;
        }
    }
    // throw error if these attribute doesn't exist
    throw ProcessError(TLF("Attribute '%' doesn't exist", toString(attr)));
}


const GNEAttributeProperties*
GNETagProperties::getAttributeProperties(const int index) const {
    if (index < 0 || index >= (int)myAttributeProperties.size()) {
        throw ProcessError(TLF("Invalid index '%' used in getAttributeProperties(int)", toString(index)));
    } else {
        return myAttributeProperties.at(index);
    }
}


const GNEAttributeProperties*
GNETagProperties::at(int index) const {
    return myAttributeProperties.at(index);
}


bool
GNETagProperties::hasAttribute(SumoXMLAttr attr) const {
    // iterate over attribute properties
    for (const auto& attributeProperty : myAttributeProperties) {
        if (attributeProperty->getAttr() == attr) {
            return true;
        }
    }
    return false;
}


int
GNETagProperties::getNumberOfAttributes() const {
    return (int)myAttributeProperties.size();
}


GUIIcon
GNETagProperties::getGUIIcon() const {
    return myIcon;
}


GUIGlObjectType
GNETagProperties::getGLType() const {
    return myGLType;
}


SumoXMLTag
GNETagProperties::getXMLTag() const {
    return myXMLTag;
}


const std::vector<SumoXMLTag>&
GNETagProperties::getXMLParentTags() const {
    return myXMLParentTags;
}


const GNETagProperties*
GNETagProperties::getHierarchicalParent() const {
    return myParent;
}


const std::vector<const GNETagProperties*>
GNETagProperties::getHierarchicalParentsRecuersively() const {
    // get the list of all roots
    std::vector<const GNETagProperties*> parents;
    parents.push_back(this);
    while (parents.back()->myParent != nullptr) {
        parents.push_back(parents.back()->myParent);
    }
    std::reverse(parents.begin(), parents.end());
    return parents;
}


const std::vector<const GNETagProperties*>&
GNETagProperties::getHierarchicalChildren() const {
    return myChildren;
}


std::vector<const GNETagProperties*>
GNETagProperties::getHierarchicalChildrenRecursively() const {
    std::vector<const GNETagProperties*> children;
    // obtain all tags recursively (including this)
    getChildrenTagProperties(this, children);
    return children;
}


std::map<std::string, const GNEAttributeProperties*>
GNETagProperties::getHierarchicalChildrenAttributesRecursively(const bool onlyCommon, const bool onlyDrawables) const {
    std::map<std::string, const GNEAttributeProperties*> allChildrenAttributes;
    // obtain all children attributes recursively (including this)
    getChildrenAttributes(this, allChildrenAttributes, onlyDrawables);
    // check if get only commons
    if (onlyCommon) {
        std::map<std::string, const GNEAttributeProperties*> commonChildrenAttributes;
        // get all tag children and take only the common attributes
        const auto tagChildren = getHierarchicalChildrenRecursively();
        // iterate over all children and check if exist in child tag
        for (const auto& attributeChild : allChildrenAttributes) {
            bool isCommon = true;
            for (const auto tagChild : tagChildren) {
                if ((!onlyDrawables || tagChild->isDrawable()) &&   // filter only drawables
                        !tagChild->isHierarchicalTag() &&               // hierarchical tags doesn't have attirbutes
                        !tagChild->hasAttribute(attributeChild.second->getAttr())) {
                    isCommon = false;
                }
            }
            if (isCommon) {
                commonChildrenAttributes.insert(attributeChild);
            }
        }
        return commonChildrenAttributes;
    } else {
        return allChildrenAttributes;
    }
}


Supermode
GNETagProperties::getSupermode() const {
    if (myParent == nullptr) {
        throw ProcessError("Root doesn't have an associated supermode");
    } else {
        auto parents = getHierarchicalParentsRecuersively();
        // continue depending of supermode
        if (parents.at(1)->getTag() == GNE_TAG_SUPERMODE_NETWORK) {
            return Supermode::NETWORK;
        } else if (parents.at(1)->getTag() == GNE_TAG_SUPERMODE_DEMAND) {
            return Supermode::DEMAND;
        } else if (parents.at(1)->getTag() == GNE_TAG_SUPERMODE_DATA) {
            return Supermode::DATA;
        } else {
            throw ProcessError("Invalid supermode");
        }
    }
}


bool
GNETagProperties::isHierarchicalTag() const {
    return myTagProperty & Property::HIERARCHICAL;
}


bool
GNETagProperties::isNetworkElement() const {
    return myTagType & Type::NETWORKELEMENT;
}


bool
GNETagProperties::isAdditionalElement() const {
    return myTagType & Type::ADDITIONALELEMENT;
}


bool
GNETagProperties::isAdditionalPureElement() const {
    return (isAdditionalElement() && !isShapeElement() && !isTAZElement() && !isWireElement());
}


bool
GNETagProperties::isDemandElement() const {
    return myTagType & Type::DEMANDELEMENT;
}


bool
GNETagProperties::isDataElement() const {
    return myTagType & Type::DATAELEMENT;
}


bool
GNETagProperties::isOtherElement() const {
    return myTagType & Type::OTHER;
}


bool
GNETagProperties::isStoppingPlace() const {
    return myTagType & Type::STOPPINGPLACE;
}


bool
GNETagProperties::isDetector() const {
    return myTagType & Type::DETECTOR;
}


bool
GNETagProperties::isCalibrator() const {
    return myTagType & Type::CALIBRATOR;
}


bool
GNETagProperties::isShapeElement() const {
    return myTagType & Type::SHAPE;
}


bool
GNETagProperties::isTAZElement() const {
    return myTagType & Type::TAZELEMENT;
}


bool
GNETagProperties::isWireElement() const {
    return myTagType & Type::WIRE;
}


bool
GNETagProperties::isJuPedSimElement() const {
    return myTagType & Type::JUPEDSIM;
}


bool
GNETagProperties::isType() const {
    return myTagType & Type::VTYPE;
}


bool
GNETagProperties::isVehicle() const {
    return myTagType & Type::VEHICLE;
}


bool
GNETagProperties::isRoute() const {
    return myTagType & Type::ROUTE;
}


bool
GNETagProperties::isVehicleStop() const {
    return myTagType & Type::STOP_VEHICLE;
}


bool
GNETagProperties::isVehicleWaypoint() const {
    return myTagType & Type::WAYPOINT_VEHICLE;
}


bool
GNETagProperties::isFlow() const {
    return myTagType & Type::FLOW;
}


bool
GNETagProperties::isPerson() const {
    return myTagType & Type::PERSON;
}


bool
GNETagProperties::isContainer() const {
    return myTagType & Type::CONTAINER;
}


bool
GNETagProperties::hasTypeParent() const {
    return isVehicle() || isPerson() || isContainer();
}


bool
GNETagProperties::isDistribution() const {
    return myTagType & Type::DISTRIBUTION;
}


bool
GNETagProperties::isDistributionReference() const {
    return myTagType & Type::DISTRIBUTIONREF;
}


bool
GNETagProperties::isTypeDistribution() const {
    return isType() && isDistribution();
}


bool
GNETagProperties::isRouteDistribution() const {
    return isRoute() && isDistribution();
}


bool
GNETagProperties::isPlan() const {
    return isPlanPerson() || isPlanContainer();
}


bool
GNETagProperties::isPlanPerson() const {
    return myTagType & Type::PERSONPLAN;
}


bool
GNETagProperties::isPlanContainer() const {
    return myTagType & Type::CONTAINERPLAN;
}


bool
GNETagProperties::isPlanPersonTrip() const {
    return myTagType & Type::PERSONTRIP;
}


bool
GNETagProperties::isPlanWalk() const {
    return myTagType & Type::WALK;
}


bool
GNETagProperties::isPlanRide() const {
    return myTagType & Type::RIDE;
}


bool
GNETagProperties::isPlanTransport() const {
    return myTagType & Type::TRANSPORT;
}


bool
GNETagProperties::isPlanTranship() const {
    return myTagType & Type::TRANSHIP;
}


bool
GNETagProperties::isPlanStop() const {
    return isPlanStopPerson() || isPlanStopContainer();
}


bool
GNETagProperties::isPlanStopPerson() const {
    return myTagType & Type::STOP_PERSON;
}


bool
GNETagProperties::isPlanStopContainer() const {
    return myTagType & Type::STOP_CONTAINER;
}


bool
GNETagProperties::isGenericData() const {
    return myTagType & Type::GENERICDATA;
}


bool
GNETagProperties::isMeanData() const {
    return myTagType & Type::MEANDATA;
}


bool
GNETagProperties::vehicleRoute() const {
    return isVehicle() && (myTagOver & Over::ROUTE);
}


bool
GNETagProperties::vehicleRouteEmbedded() const {
    return isVehicle() && (myTagOver & Over::ROUTE_EMBEDDED);
}


bool
GNETagProperties::vehicleEdges() const {
    return isVehicle() && (myTagOver & Over::FROM_EDGE) && (myTagOver & Over::TO_EDGE);
}


bool
GNETagProperties::vehicleJunctions() const {
    return isVehicle() && (myTagOver & Over::FROM_JUNCTION) && (myTagOver & Over::TO_JUNCTION);
}


bool
GNETagProperties::vehicleTAZs() const {
    return isVehicle() && (myTagOver & Over::FROM_TAZ) && (myTagOver & Over::TO_TAZ);
}


bool
GNETagProperties::planConsecutiveEdges() const {
    return isPlan() && (myTagOver & Over::CONSECUTIVE_EDGES);
}


bool
GNETagProperties::planRoute() const {
    return isPlan() && (myTagOver & Over::ROUTE);
}


bool
GNETagProperties::planEdge() const {
    return isPlan() && (myTagOver & Over::EDGE);
}


bool
GNETagProperties::planBusStop() const {
    return isPlan() && (myTagOver & Over::BUSSTOP);
}


bool
GNETagProperties::planTrainStop() const {
    return isPlan() && (myTagOver & Over::TRAINSTOP);
}


bool
GNETagProperties::planContainerStop() const {
    return isPlan() && (myTagOver & Over::CONTAINERSTOP);
}


bool
GNETagProperties::planChargingStation() const {
    return isPlan() && (myTagOver & Over::CHARGINGSTATION);
}


bool
GNETagProperties::planParkingArea() const {
    return isPlan() && (myTagOver & Over::PARKINGAREA);
}


bool
GNETagProperties::planStoppingPlace() const {
    return planBusStop() || planTrainStop() || planContainerStop() ||
           planChargingStation() || planParkingArea();
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
    return (myTagOver & Over::FROM_EDGE);
}


bool
GNETagProperties::planFromTAZ() const {
    return isPlan() && (myTagOver & Over::FROM_TAZ);
}


bool
GNETagProperties::planFromJunction() const {
    return isPlan() && (myTagOver & Over::FROM_JUNCTION);
}


bool
GNETagProperties::planFromBusStop() const {
    return isPlan() && (myTagOver & Over::FROM_BUSSTOP);
}


bool
GNETagProperties::planFromTrainStop() const {
    return isPlan() && (myTagOver & Over::FROM_TRAINSTOP);
}


bool
GNETagProperties::planFromContainerStop() const {
    return isPlan() && (myTagOver & Over::FROM_CONTAINERSTOP);
}


bool
GNETagProperties::planFromChargingStation() const {
    return isPlan() && (myTagOver & Over::FROM_CHARGINGSTATION);
}


bool
GNETagProperties::planFromParkingArea() const {
    return isPlan() && (myTagOver & Over::FROM_PARKINGAREA);
}


bool
GNETagProperties::planFromStoppingPlace() const {
    return planFromBusStop() || planFromTrainStop() || planFromContainerStop() ||
           planFromChargingStation() || planFromParkingArea();
}


bool
GNETagProperties::planToEdge() const {
    return isPlan() && (myTagOver & Over::TO_EDGE);
}


bool
GNETagProperties::planToTAZ() const {
    return isPlan() && (myTagOver & Over::TO_TAZ);
}


bool
GNETagProperties::planToJunction() const {
    return isPlan() && (myTagOver & Over::TO_JUNCTION);
}

bool
GNETagProperties::planToBusStop() const {
    return isPlan() && (myTagOver & Over::TO_BUSSTOP);
}


bool
GNETagProperties::planToTrainStop() const {
    return isPlan() && (myTagOver & Over::TO_TRAINSTOP);
}


bool
GNETagProperties::planToContainerStop() const {
    return isPlan() && (myTagOver & Over::TO_CONTAINERSTOP);
}


bool
GNETagProperties::planToChargingStation() const {
    return isPlan() && (myTagOver & Over::TO_CHARGINGSTATION);
}


bool
GNETagProperties::planToParkingArea() const {
    return isPlan() && (myTagOver & Over::TO_PARKINGAREA);
}


bool
GNETagProperties::planToStoppingPlace() const {
    return planToBusStop() || planToTrainStop() || planToContainerStop() ||
           planToChargingStation() || planToParkingArea();
}


bool
GNETagProperties::isChild() const {
    return (myTagProperty & Property::XMLCHILD);
}


bool
GNETagProperties::isSymbol() const {
    return (myTagProperty & Property::SYMBOL);
}


bool
GNETagProperties::isInternalLane() const {
    return (myTagType & Type::INTERNALLANE);
}


bool
GNETagProperties::isDrawable() const {
    return (myTagProperty & Property::NOTDRAWABLE) == false;
}


bool
GNETagProperties::isSelectable() const {
    // note: By default all elements can be selected, except Tags with "NOTSELECTABLE"
    return (myTagProperty & Property::NOTSELECTABLE) == false;
}


bool
GNETagProperties::hasGEOShape() const {
    return (myTagProperty & Property::GEOSHAPE);
}


bool
GNETagProperties::hasDialog() const {
    return (myTagProperty & Property::DIALOG);
}


bool
GNETagProperties::hasExtendedAttributes() const {
    return (myTagProperty & Property::EXTENDED);
}


bool
GNETagProperties::hasParameters() const {
    // note: By default all elements support parameters, except Tags with "NOPARAMETERS"
    return (myTagProperty & Property::NOPARAMETERS) == false;
}


bool
GNETagProperties::isPlacedInRTree() const {
    return (myTagProperty & Property::RTREE);
}


bool
GNETagProperties::isListedElement() const {
    return (myTagProperty & Property::LISTED);
}


bool
GNETagProperties::canBeReparent() const {
    return (myTagProperty & Property::REPARENT);
}


bool
GNETagProperties::canCenterCameraAfterCreation() const {
    return (myTagProperty & Property::CENTERAFTERCREATION);
}


bool
GNETagProperties::requireProj() const {
    return (myTagProperty & Property::REQUIRE_PROJ);
}


bool
GNETagProperties::vClassIcon() const {
    return (myTagProperty & Property::VCLASS_ICON);
}


void
GNETagProperties::addChild(const GNETagProperties* child) {
    myChildren.push_back(child);
}


void
GNETagProperties::getChildrenTagProperties(const GNETagProperties* tagProperties, std::vector<const GNETagProperties*>& result) const {
    result.push_back(tagProperties);
    // call it iterative for all children
    for (const auto& child : tagProperties->myChildren) {
        getChildrenTagProperties(child, result);
    }
}


void
GNETagProperties::getChildrenAttributes(const GNETagProperties* tagProperties, std::map<std::string, const GNEAttributeProperties*>& result, const bool onlyDrawables) const {
    // add every attribute only once
    if (!onlyDrawables || tagProperties->isDrawable()) {
        for (const auto& attributeProperty : tagProperties->myAttributeProperties) {
            result[attributeProperty->getAttrStr()] = attributeProperty;
        }
    }
    // call it iterative for all children
    for (const auto& child : tagProperties->myChildren) {
        getChildrenAttributes(child, result, onlyDrawables);
    }
}

/****************************************************************************/
