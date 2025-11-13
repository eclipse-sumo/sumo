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

GNETagProperties::GNETagProperties(const SumoXMLTag tag, GNETagProperties* parent, const GNETagProperties::Type type,
                                   const GNETagProperties::Property property, const GNETagProperties::Over over,
                                   const FileBucket::Type bucketType, const GNETagProperties::Conflicts conflicts,
                                   const GUIIcon icon, const GUIGlObjectType GLType, const SumoXMLTag XMLTag,
                                   const std::string tooltipText, const std::vector<SumoXMLTag> XMLParentTags,
                                   const unsigned int backgroundColor, const std::string selectorText) :
    myTag(tag),
    myTagStr(toString(tag)),
    myParent(parent),
    myType(type),
    myProperty(property),
    myOver(over),
    myBucketType(bucketType),
    myConflicts(conflicts),
    myIcon(icon),
    myGLType(GLType),
    myXMLTag(XMLTag),
    myTooltipText(tooltipText),
    myXMLParentTags(XMLParentTags),
    mySelectorText(selectorText.empty() ? toString(tag) : selectorText),
    myBackgroundColor(backgroundColor) {
    if (parent) {
        parent->addChild(this);
    }
}


GNETagProperties::GNETagProperties(const SumoXMLTag tag, GNETagProperties* parent, const GUIIcon icon, const std::string tooltipText,
                                   const unsigned int backgroundColor, const std::string selectorText) :
    myTag(tag),
    myTagStr(toString(tag)),
    myParent(parent),
    myProperty(Property::HIERARCHICAL),
    myIcon(icon),
    myXMLTag(tag),
    myTooltipText(tooltipText),
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
    // check drawable
    if (!isDrawable() && isPlacedInRTree()) {
        throw FormatException("Non-drawable elements cannot be placed in RTREE");
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
    return myProperty & Property::HIERARCHICAL;
}


bool
GNETagProperties::isNetworkElement() const {
    return myType & Type::NETWORKELEMENT;
}


bool
GNETagProperties::isAdditionalElement() const {
    return myType & Type::ADDITIONALELEMENT;
}


bool
GNETagProperties::isAdditionalPureElement() const {
    return (isAdditionalElement() && !isShapeElement() && !isTAZElement() && !isWireElement());
}


bool
GNETagProperties::isDemandElement() const {
    return myType & Type::DEMANDELEMENT;
}


bool
GNETagProperties::isDataElement() const {
    return myType & Type::DATAELEMENT;
}


bool
GNETagProperties::isOtherElement() const {
    return myType & Type::OTHER;
}


bool
GNETagProperties::isStoppingPlace() const {
    return myType & Type::STOPPINGPLACE;
}


bool
GNETagProperties::isDetector() const {
    return myType & Type::DETECTOR;
}


bool
GNETagProperties::isCalibrator() const {
    return myType & Type::CALIBRATOR;
}


bool
GNETagProperties::isShapeElement() const {
    return myType & Type::SHAPE;
}


bool
GNETagProperties::isTAZElement() const {
    return myType & Type::TAZELEMENT;
}


bool
GNETagProperties::isWireElement() const {
    return myType & Type::WIRE;
}


bool
GNETagProperties::isJuPedSimElement() const {
    return myType & Type::JUPEDSIM;
}


bool
GNETagProperties::isType() const {
    return myType & Type::VTYPE;
}


bool
GNETagProperties::isVehicle() const {
    return myType & Type::VEHICLE;
}


bool
GNETagProperties::isRoute() const {
    return myType & Type::ROUTE;
}


bool
GNETagProperties::isVehicleStop() const {
    return myType & Type::STOP_VEHICLE;
}


bool
GNETagProperties::isVehicleWaypoint() const {
    return myType & Type::WAYPOINT_VEHICLE;
}


bool
GNETagProperties::isFlow() const {
    return myType & Type::FLOW;
}


bool
GNETagProperties::isPerson() const {
    return myType & Type::PERSON;
}


bool
GNETagProperties::isContainer() const {
    return myType & Type::CONTAINER;
}


bool
GNETagProperties::hasTypeParent() const {
    return isVehicle() || isPerson() || isContainer();
}


bool
GNETagProperties::isDistribution() const {
    return myType & Type::DISTRIBUTION;
}


bool
GNETagProperties::isDistributionReference() const {
    return myType & Type::DISTRIBUTIONREF;
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
    return myType & Type::PERSONPLAN;
}


bool
GNETagProperties::isPlanContainer() const {
    return myType & Type::CONTAINERPLAN;
}


bool
GNETagProperties::isPlanPersonTrip() const {
    return myType & Type::PERSONTRIP;
}


bool
GNETagProperties::isPlanWalk() const {
    return myType & Type::WALK;
}


bool
GNETagProperties::isPlanRide() const {
    return myType & Type::RIDE;
}


bool
GNETagProperties::isPlanTransport() const {
    return myType & Type::TRANSPORT;
}


bool
GNETagProperties::isPlanTranship() const {
    return myType & Type::TRANSHIP;
}


bool
GNETagProperties::isPlanStop() const {
    return isPlanStopPerson() || isPlanStopContainer();
}


bool
GNETagProperties::isPlanStopPerson() const {
    return myType & Type::STOP_PERSON;
}


bool
GNETagProperties::isPlanStopContainer() const {
    return myType & Type::STOP_CONTAINER;
}


bool
GNETagProperties::isGenericData() const {
    return myType & Type::GENERICDATA;
}


bool
GNETagProperties::isMeanData() const {
    return myType & Type::MEANDATA;
}


bool
GNETagProperties::vehicleRoute() const {
    return isVehicle() && (myOver & Over::ROUTE);
}


bool
GNETagProperties::vehicleRouteEmbedded() const {
    return isVehicle() && (myOver & Over::ROUTE_EMBEDDED);
}


bool
GNETagProperties::vehicleEdges() const {
    return isVehicle() && (myOver & Over::FROM_EDGE) && (myOver & Over::TO_EDGE);
}


bool
GNETagProperties::vehicleJunctions() const {
    return isVehicle() && (myOver & Over::FROM_JUNCTION) && (myOver & Over::TO_JUNCTION);
}


bool
GNETagProperties::vehicleTAZs() const {
    return isVehicle() && (myOver & Over::FROM_TAZ) && (myOver & Over::TO_TAZ);
}


bool
GNETagProperties::planConsecutiveEdges() const {
    return isPlan() && (myOver & Over::CONSECUTIVE_EDGES);
}


bool
GNETagProperties::planRoute() const {
    return isPlan() && (myOver & Over::ROUTE);
}


bool
GNETagProperties::planEdge() const {
    return isPlan() && (myOver & Over::EDGE);
}


bool
GNETagProperties::planBusStop() const {
    return isPlan() && (myOver & Over::BUSSTOP);
}


bool
GNETagProperties::planTrainStop() const {
    return isPlan() && (myOver & Over::TRAINSTOP);
}


bool
GNETagProperties::planContainerStop() const {
    return isPlan() && (myOver & Over::CONTAINERSTOP);
}


bool
GNETagProperties::planChargingStation() const {
    return isPlan() && (myOver & Over::CHARGINGSTATION);
}


bool
GNETagProperties::planParkingArea() const {
    return isPlan() && (myOver & Over::PARKINGAREA);
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
    return (myOver & Over::FROM_EDGE);
}


bool
GNETagProperties::planFromTAZ() const {
    return isPlan() && (myOver & Over::FROM_TAZ);
}


bool
GNETagProperties::planFromJunction() const {
    return isPlan() && (myOver & Over::FROM_JUNCTION);
}


bool
GNETagProperties::planFromBusStop() const {
    return isPlan() && (myOver & Over::FROM_BUSSTOP);
}


bool
GNETagProperties::planFromTrainStop() const {
    return isPlan() && (myOver & Over::FROM_TRAINSTOP);
}


bool
GNETagProperties::planFromContainerStop() const {
    return isPlan() && (myOver & Over::FROM_CONTAINERSTOP);
}


bool
GNETagProperties::planFromChargingStation() const {
    return isPlan() && (myOver & Over::FROM_CHARGINGSTATION);
}


bool
GNETagProperties::planFromParkingArea() const {
    return isPlan() && (myOver & Over::FROM_PARKINGAREA);
}


bool
GNETagProperties::planFromStoppingPlace() const {
    return planFromBusStop() || planFromTrainStop() || planFromContainerStop() ||
           planFromChargingStation() || planFromParkingArea();
}


bool
GNETagProperties::planToEdge() const {
    return isPlan() && (myOver & Over::TO_EDGE);
}


bool
GNETagProperties::planToTAZ() const {
    return isPlan() && (myOver & Over::TO_TAZ);
}


bool
GNETagProperties::planToJunction() const {
    return isPlan() && (myOver & Over::TO_JUNCTION);
}

bool
GNETagProperties::planToBusStop() const {
    return isPlan() && (myOver & Over::TO_BUSSTOP);
}


bool
GNETagProperties::planToTrainStop() const {
    return isPlan() && (myOver & Over::TO_TRAINSTOP);
}


bool
GNETagProperties::planToContainerStop() const {
    return isPlan() && (myOver & Over::TO_CONTAINERSTOP);
}


bool
GNETagProperties::planToChargingStation() const {
    return isPlan() && (myOver & Over::TO_CHARGINGSTATION);
}


bool
GNETagProperties::planToParkingArea() const {
    return isPlan() && (myOver & Over::TO_PARKINGAREA);
}


bool
GNETagProperties::planToStoppingPlace() const {
    return planToBusStop() || planToTrainStop() || planToContainerStop() ||
           planToChargingStation() || planToParkingArea();
}


bool
GNETagProperties::isChild() const {
    return (myProperty & Property::XMLCHILD);
}


bool
GNETagProperties::isSymbol() const {
    return (myProperty & Property::SYMBOL);
}


bool
GNETagProperties::isInternalLane() const {
    return (myType & Type::INTERNALLANE);
}


bool
GNETagProperties::isDrawable() const {
    return (myProperty & Property::NOTDRAWABLE) == false;
}


bool
GNETagProperties::isSelectable() const {
    // note: By default all elements can be selected, except Tags with "NOTSELECTABLE"
    return (myProperty & Property::NOTSELECTABLE) == false;
}


bool
GNETagProperties::hasGEOShape() const {
    return (myProperty & Property::GEOSHAPE);
}


bool
GNETagProperties::hasDialog() const {
    return (myProperty & Property::DIALOG);
}


bool
GNETagProperties::hasExtendedAttributes() const {
    return (myProperty & Property::EXTENDED);
}


bool
GNETagProperties::hasParameters() const {
    // note: By default all elements support parameters, except Tags with "NOPARAMETERS"
    return (myProperty & Property::NOPARAMETERS) == false;
}


bool
GNETagProperties::isPlacedInRTree() const {
    return (myProperty & Property::RTREE);
}


bool
GNETagProperties::isListedElement() const {
    return (myProperty & Property::LISTED);
}


bool
GNETagProperties::canBeReparent() const {
    return (myProperty & Property::REPARENT);
}


bool
GNETagProperties::canCenterCameraAfterCreation() const {
    return (myProperty & Property::CENTERAFTERCREATION);
}


bool
GNETagProperties::requireProj() const {
    return (myProperty & Property::REQUIRE_PROJ);
}


bool
GNETagProperties::vClassIcon() const {
    return (myProperty & Property::VCLASS_ICON);
}


bool
GNETagProperties::isFileCompatible(FileBucket::Type file) const {
    return (myBucketType & file);
}


bool
GNETagProperties::saveInNetworkFile() const {
    return (myBucketType & FileBucket::Type::NETWORK);
}


bool
GNETagProperties::saveInAdditionalFile() const {
    return (myBucketType & FileBucket::Type::ADDITIONAL);
}


bool
GNETagProperties::saveInDemandFile() const {
    return (myBucketType & FileBucket::Type::DEMAND);
}


bool
GNETagProperties::saveInDataFile() const {
    return (myBucketType & FileBucket::Type::DATA);
}


bool
GNETagProperties::saveInMeanDataFile() const {
    return (myBucketType & FileBucket::Type::MEANDATA);
}


bool
GNETagProperties::saveInParentFile() const {
    return (myBucketType & FileBucket::Type::NOTHING);
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
