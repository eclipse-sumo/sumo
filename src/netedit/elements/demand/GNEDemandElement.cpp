/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEDemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// A abstract class for demand elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEDemandElement.h"
#include "GNERouteHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandElement::GNEDemandElement(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const int options,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GNEPathManager::PathElement(type, id, icon, options),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    GNEDemandElementDistribution(this),
    myStackedLabelNumber(0) {
    // check if is template
    myIsTemplate = id.empty();
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* demandElementParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const int options,
                                   const std::vector<GNEJunction*>& junctionParents,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEGenericData*>& genericDataParents) :
    GNEPathManager::PathElement(type, demandElementParent->getID(), icon, options),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    GNEDemandElementDistribution(this),
    myStackedLabelNumber(0) {
}


GNEDemandElement::~GNEDemandElement() {}


void
GNEDemandElement::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // currently there isn't demand elements with removable geometry points
}


GUIGlObject*
GNEDemandElement::getGUIGlObject() {
    return this;
}


const GUIGlObject*
GNEDemandElement::getGUIGlObject() const {
    return this;
}


const GUIGeometry&
GNEDemandElement::getDemandElementGeometry() {
    return myDemandElementGeometry;
}


GNEDemandElement*
GNEDemandElement::getPreviousChildDemandElement(const GNEDemandElement* demandElement) const {
    // first check if there are demand elements
    if (getChildDemandElements().empty()) {
        return nullptr;
    } else {
        // find child demand element
        auto it = std::find(getChildDemandElements().begin(), getChildDemandElements().end(), demandElement);
        // return element or null depending of iterator
        if (it == getChildDemandElements().end()) {
            // in this case, we assume that the last child is the previos child
            return getChildDemandElements().back();
        } else if (it == getChildDemandElements().begin()) {
            return nullptr;
        } else {
            return *(it - 1);
        }
    }
}


GNEDemandElement*
GNEDemandElement::getNextChildDemandElement(const GNEDemandElement* demandElement) const {
    // find child demand element
    auto it = std::find(getChildDemandElements().begin(), getChildDemandElements().end(), demandElement);
    // return element or null depending of iterator
    if (it == getChildDemandElements().end()) {
        return nullptr;
    } else if (it == (getChildDemandElements().end() - 1)) {
        return nullptr;
    } else {
        return *(it + 1);
    }
}


void
GNEDemandElement::updateDemandElementGeometry(const GNELane* lane, const double posOverLane) {
    myDemandElementGeometry.updateGeometry(lane->getLaneShape(), posOverLane, myMoveElementLateralOffset);
}


void
GNEDemandElement::updateDemandElementStackLabel(const int stack) {
    myStackedLabelNumber = stack;
}


void
GNEDemandElement::updateDemandElementSpreadGeometry(const GNELane* lane, const double posOverLane) {
    mySpreadGeometry.updateGeometry(lane->getLaneShape(), posOverLane, myMoveElementLateralOffset);
}


const GUIGeometry&
GNEDemandElement::getDemandElementGeometry() const {
    return myDemandElementGeometry;
}


bool
GNEDemandElement::checkDrawFromContour() const {
    return false;
}


bool
GNEDemandElement::checkDrawToContour() const {
    return false;
}


bool
GNEDemandElement::checkDrawRelatedContour() const {
    return false;
}


bool
GNEDemandElement::checkDrawOverContour() const {
    // get modes
    const auto& modes = myNet->getViewNet()->getEditModes();
    // get frames
    const auto& personFramePlanSelector = myNet->getViewNet()->getViewParent()->getPersonFrame()->getPlanSelector();
    const auto& personPlanFramePlanSelector = myNet->getViewNet()->getViewParent()->getPersonPlanFrame()->getPlanSelector();
    const auto& containerFramePlanSelector = myNet->getViewNet()->getViewParent()->getContainerFrame()->getPlanSelector();
    const auto& containerPlanFramePlanSelector = myNet->getViewNet()->getViewParent()->getContainerPlanFrame()->getPlanSelector();
    // special case for Route
    if (myTagProperty.getTag() == SUMO_TAG_ROUTE) {
        // get vehicle frame
        const auto& vehicleFrame = myNet->getViewNet()->getViewParent()->getVehicleFrame();
        // check if we're in vehicle mode
        if (vehicleFrame->shown()) {
            // get current vehicle template
            const auto& vehicleTemplate = vehicleFrame->getVehicleTagSelector()->getCurrentTemplateAC();
            // check if vehicle can be placed over route
            if (vehicleTemplate && vehicleTemplate->getTagProperty().vehicleRoute()) {
                return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
            }
        } else if (modes.isCurrentSupermodeDemand()) {
            // check if we're in person or personPlan modes
            if (((modes.demandEditMode == DemandEditMode::DEMAND_PERSON) && personFramePlanSelector->markRoutes()) ||
                    ((modes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) && personPlanFramePlanSelector->markRoutes()) ||
                    ((modes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) && containerFramePlanSelector->markRoutes()) ||
                    ((modes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) && containerPlanFramePlanSelector->markRoutes())) {
                return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
            }
        }
    }
    return false;
}


bool
GNEDemandElement::checkDrawDeleteContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in delete mode
    if (editModes.isCurrentSupermodeDemand() && (editModes.demandEditMode == DemandEditMode::DEMAND_DELETE)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEDemandElement::checkDrawSelectContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in select mode
    if (editModes.isCurrentSupermodeDemand() && (editModes.demandEditMode == DemandEditMode::DEMAND_SELECT)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEDemandElement::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check first set of conditions
    if (!myNet->getViewNet()->isCurrentlyMovingElements() &&                            // another elements are not currently moved
            editModes.isCurrentSupermodeDemand() &&                                         // supermode demand
            (editModes.demandEditMode == DemandEditMode::DEMAND_MOVE) &&                    // move mode
            myNet->getViewNet()->checkOverLockedElement(this, mySelected) &&                // no locked
            myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this) {  // first element
        // continue depending of subtype
        if (myTagProperty.isVehicle()) {
            // only vehicles over edges can be moved
            if (myTagProperty.vehicleEdges() || myTagProperty.vehicleRoute() || myTagProperty.vehicleRouteEmbedded()) {
                return true;
            } else {
                return false;
            }
        } else if ((myTagProperty.isPerson() || myTagProperty.isContainer()) && (getChildDemandElements().size() > 0)) {
            // only persons/containers with their first plan over edge can be moved
            return getChildDemandElements().front()->getTagProperty().planFromEdge();
        } else {
            return false;
        }
    } else {
        return false;
    }
}


void
GNEDemandElement::openDemandElementDialog() {
    throw InvalidArgument(getTagStr() + " doesn't have a demand element dialog");
}


GUIGLObjectPopupMenu*
GNEDemandElement::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " name to clipboard", nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " typed name to clipboard", nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getACIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y()), nullptr, nullptr, 0);
    return ret;
}


GUIParameterTableWindow*
GNEDemandElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.isUnique()) {
            ret->mkItem(i.getAttrStr().c_str(), false, getAttribute(i.getAttr()));
        } else {
            ret->mkItem(i.getAttrStr().c_str(), true, getAttribute(i.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


bool
GNEDemandElement::isGLObjectLocked() const {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        return myNet->getViewNet()->getLockManager().isObjectLocked(getType(), isAttributeCarrierSelected());
    } else {
        return true;
    }
}


void
GNEDemandElement::markAsFrontElement() {
    myNet->getViewNet()->setFrontAttributeCarrier(this);
}


void
GNEDemandElement::deleteGLObject() {
    // we need an special checks due hierarchies
    if (myTagProperty.isPlan()) {
        // get person/container plarent
        GNEDemandElement* planParent = getParentDemandElements().front();
        // if this is the last person/container plan element, remove parent instead plan
        if (planParent->getChildDemandElements().size() == 1) {
            planParent->deleteGLObject();
        } else {
            myNet->deleteDemandElement(this, myNet->getViewNet()->getUndoList());
        }
    } else if (myTagProperty.getTag() == GNE_TAG_ROUTE_EMBEDDED) {
        // remove parent demand element
        getParentDemandElements().front()->deleteGLObject();
    } else {
        myNet->deleteDemandElement(this, myNet->getViewNet()->getUndoList());
    }
}


void
GNEDemandElement::selectGLObject() {
    if (isAttributeCarrierSelected()) {
        unselectAttributeCarrier();
    } else {
        selectAttributeCarrier();
    }
    // update information label
    myNet->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionInformation()->updateInformationLabel();
}


void
GNEDemandElement::updateGLObject() {
    updateGeometry();
}


bool
GNEDemandElement::isPathElementSelected() const {
    return mySelected;
}

// ---------------------------------------------------------------------------
// GNEDemandElement - protected methods
// ---------------------------------------------------------------------------

bool
GNEDemandElement::isValidDemandElementID(const std::string& value) const {
    if (!isTemplate() && (value == getID())) {
        return true;
    } else if (SUMOXMLDefinitions::isValidVehicleID(value)) {
        return (myNet->getAttributeCarriers()->retrieveDemandElement(myTagProperty.getTag(), value, false) == nullptr);
    } else {
        return false;
    }
}


bool
GNEDemandElement::isValidDemandElementID(const std::vector<SumoXMLTag>& tags, const std::string& value) const {
    if (value == getID()) {
        return true;
    } else if (SUMOXMLDefinitions::isValidVehicleID(value)) {
        return (myNet->getAttributeCarriers()->retrieveDemandElements(tags, value, false) == nullptr);
    } else {
        return false;
    }
}


void
GNEDemandElement::setDemandElementID(const std::string& newID) {
    // update ID
    if (isTemplate() || !myTagProperty.hasAttribute(SUMO_ATTR_ID)) {
        setMicrosimID(newID);
    } else {
        myNet->getAttributeCarriers()->updateDemandElementID(this, newID);
    }
    // check if update ids of child elements
    if (myTagProperty.isPerson() || myTagProperty.isContainer()) {
        // Change IDs of all person plans children (stops, embedded routes...)
        for (const auto& childDemandElement : getChildDemandElements()) {
            childDemandElement->setDemandElementID(getID());
        }
    }
}


GNEDemandElement*
GNEDemandElement::getTypeParent() const {
    if (getParentDemandElements().size() < 1) {
        throw InvalidArgument("This demand element doesn't have a type parent");
    } else if (!getParentDemandElements().at(0)->getTagProperty().isType()
               && !getParentDemandElements().at(0)->getTagProperty().isTypeDist()) {
        throw InvalidArgument("The first parent isn't a type");
    } else if (getParentDemandElements().at(0)->getTagProperty().getTag() == SUMO_TAG_VTYPE) {
        return getParentDemandElements().at(0);
    } else {
        // get typeDistribution ID
        const auto typeDistributionID = getParentDemandElements().at(0)->getID();
        // obtain all types with the given typeDistribution sorted by ID
        std::map<std::string, GNEDemandElement*> sortedTypes;
        for (const auto& type : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
            if (type.second->getAttribute(GNE_ATTR_VTYPE_DISTRIBUTION) == typeDistributionID) {
                sortedTypes[type.second->getID()] = type.second;
            }
        }
        // return first type, or default vType
        if (sortedTypes.size() > 0) {
            return sortedTypes.begin()->second;
        } else if (myNet->getAttributeCarriers()->getDemandElements().size() > 0) {
            return myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE).begin()->second;
        } else {
            throw InvalidArgument("no vTypes");
        }
    }
}


GNEDemandElement*
GNEDemandElement::getRouteParent() const {
    if (getParentDemandElements().size() < 2) {
        throw InvalidArgument("This demand element doesn't have two parent");
    } else if (getParentDemandElements().at(1)->getTagProperty().getTag() != SUMO_TAG_ROUTE) {
        throw InvalidArgument("This demand element doesn't have a route parent");
    } else {
        return getParentDemandElements().at(1);
    }
}


std::vector<GNEDemandElement*>
GNEDemandElement::getInvalidStops() const {
    if (myTagProperty.isVehicleStop()) {
        // get stops
        std::vector<GNEDemandElement*> invalidStops;
        // get edge stop index
        const auto edgeStopIndex = getEdgeStopIndex();
        // take all stops/waypoints with index = -1
        for (const auto& edgeStop : edgeStopIndex) {
            if (edgeStop.stopIndex == -1) {
                for (const auto& stop : edgeStop.stops) {
                    invalidStops.push_back(stop);
                }
            }
        }
        return invalidStops;
    } else {
        return {};
    }
}


void
GNEDemandElement::drawJunctionLine(const GNEDemandElement* element) const {
    // get two points
    const Position posA = element->getParentJunctions().front()->getPositionInView();
    const Position posB = element->getParentJunctions().back()->getPositionInView();
    const double rot = ((double)atan2((posB.x() - posA.x()), (posA.y() - posB.y())) * (double) 180.0 / (double)M_PI);
    const double len = posA.distanceTo2D(posB);
    // push draw matrix
    GLHelper::pushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, element->getType() + 0.1);
    // set trip color
    GLHelper::setColor(RGBColor::RED);
    // draw line
    GLHelper::drawBoxLine(posA, rot, len, 0.25);
    // pop draw matrix
    GLHelper::popMatrix();
}


void
GNEDemandElement::drawStackLabel(const int number, const std::string& element, const Position& position, const double rotation, const double width, const double length, const double exaggeration) const {
    // declare contour width
    const double contourWidth = (0.05 * exaggeration);
    // Push matrix
    GLHelper::pushMatrix();
    // Traslate to  top
    glTranslated(position.x(), position.y(), GLO_ROUTE + getType() + 0.1 + GLO_PERSONFLOW);
    glRotated(rotation, 0, 0, -1);
    glTranslated((width * exaggeration * 0.5) + (0.35 * exaggeration), 0, 0);
    // draw external box
    GLHelper::setColor(RGBColor::GREY);
    GLHelper::drawBoxLine(Position(), 0, (length * exaggeration), 0.3 * exaggeration);
    // draw internal box
    glTranslated(0, 0, 0.1);
    GLHelper::setColor(RGBColor(0, 128, 0));
    GLHelper::drawBoxLine(Position(0, -contourWidth), Position(0, -contourWidth), 0, (length * exaggeration) - (contourWidth * 2), (0.3 * exaggeration) - contourWidth);
    // draw stack label
    GLHelper::drawText(element + "s stacked: " + toString(number), Position(0, length * exaggeration * -0.5), (.1 * exaggeration), (0.6 * exaggeration), RGBColor::WHITE, 90, 0, -1);
    // pop draw matrix
    GLHelper::popMatrix();
}


void
GNEDemandElement::replaceDemandParentEdges(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNEEdge*> >(getNet(), value));
}


void
GNEDemandElement::replaceDemandParentLanes(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNELane*> >(getNet(), value));
}


void
GNEDemandElement::replaceFirstParentJunction(const std::string& value) {
    std::vector<GNEJunction*> parentJunctions = getParentJunctions();
    parentJunctions[0] = myNet->getAttributeCarriers()->retrieveJunction(value);
    // replace parent junctions
    replaceParentElements(this, parentJunctions);
}


void
GNEDemandElement::replaceLastParentJunction(const std::string& value) {
    std::vector<GNEJunction*> parentJunctions = getParentJunctions();
    parentJunctions[(int)parentJunctions.size() - 1] = myNet->getAttributeCarriers()->retrieveJunction(value);
    // replace parent junctions
    replaceParentElements(this, parentJunctions);
}


void
GNEDemandElement::replaceFirstParentEdge(const std::string& value) {
    std::vector<GNEEdge*> parentEdges = getParentEdges();
    parentEdges[0] = myNet->getAttributeCarriers()->retrieveEdge(value);
    // replace parent edges
    replaceParentElements(this, parentEdges);
}


void
GNEDemandElement::replaceLastParentEdge(const std::string& value) {
    std::vector<GNEEdge*> parentEdges = getParentEdges();
    parentEdges[(int)parentEdges.size() - 1] = myNet->getAttributeCarriers()->retrieveEdge(value);
    // replace parent edges
    replaceParentElements(this, parentEdges);
}


void
GNEDemandElement::replaceFirstParentAdditional(SumoXMLTag tag, const std::string& value) {
    std::vector<GNEAdditional*> parentAdditionals = getParentAdditionals();
    parentAdditionals[0] = myNet->getAttributeCarriers()->retrieveAdditional(tag, value);
    // replace parent additionals
    replaceParentElements(this, parentAdditionals);
}


void
GNEDemandElement::replaceLastParentAdditional(SumoXMLTag tag, const std::string& value) {
    std::vector<GNEAdditional*> parentAdditionals = getParentAdditionals();
    parentAdditionals[(int)parentAdditionals.size() - 1] = myNet->getAttributeCarriers()->retrieveAdditional(tag, value);
    // replace parent additionals
    replaceParentElements(this, parentAdditionals);
}


void
GNEDemandElement::replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEDemandElement*> parentDemandElements = getParentDemandElements();
    parentDemandElements[parentIndex] = myNet->getAttributeCarriers()->retrieveDemandElement(tag, value);
    // replace parent demand elements
    replaceParentElements(this, parentDemandElements);
}


void
GNEDemandElement::setVTypeDistributionParent(const std::string& value) {
    std::vector<GNEDemandElement*> parents;
    if (value.size() > 0) {
        parents.push_back(myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, value));
    }
    replaceParentElements(this, parents);
}


bool
GNEDemandElement::checkChildDemandElementRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError(StringUtils::format("Calling non-implemented function checkChildDemandElementRestriction during saving of %. It muss be reimplemented in child class", getTagStr()));
}


std::vector<GNEDemandElement::EdgeStopIndex>
GNEDemandElement::getEdgeStopIndex() const {
    std::vector<GNEDemandElement::EdgeStopIndex> edgeStopIndex;
    // first check that this stop has parent
    if (getParentDemandElements().size() > 0) {
        // get path edges depending of parent
        std::vector<GNEEdge*> pathEdges;
        // get parent demand element
        const auto parent = getParentDemandElements().front();
        // continue depending of parent
        if (parent->getTagProperty().hasAttribute(SUMO_ATTR_EDGES)) {
            pathEdges = parent->getParentEdges();
        } else if (parent->getTagProperty().vehicleRoute()) {
            // get route edges
            if (parent->getParentDemandElements().size() > 1) {
                pathEdges = parent->getParentDemandElements().at(1)->getParentEdges();
            }
        } else if (parent->getTagProperty().vehicleRouteEmbedded()) {
            // get embedded route edges
            pathEdges = parent->getChildDemandElements().front()->getParentEdges();
        } else {
            // get last parent edge
            const auto lastEdge = parent->getParentEdges().back();
            bool stop = false;
            const auto& pathElementSegments = myNet->getPathManager()->getPathElementSegments(parent);
            // extract all edges from pathElement parent
            for (auto it = pathElementSegments.begin(); (it != pathElementSegments.end()) && !stop; it++) {
                if ((*it)->getLane()) {
                    pathEdges.push_back((*it)->getLane()->getParentEdge());
                    // stop if path correspond to last edge
                    if (pathEdges.back() == lastEdge) {
                        stop = true;
                    }
                }
            }
        }
        // get all parent's stops and waypoints sorted by position
        for (const auto& demandElement : parent->getChildDemandElements()) {
            if (demandElement->getTagProperty().isVehicleStop()) {
                // get stop/waypoint edge
                GNEEdge* edge = nullptr;
                if (demandElement->getParentAdditionals().size() > 0) {
                    edge = demandElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
                } else {
                    edge = demandElement->getParentLanes().front()->getParentEdge();
                }
                // check if add a new edgeStopIndex or update last
                if ((edgeStopIndex.size() > 0) && (edgeStopIndex.back().edge == edge)) {
                    edgeStopIndex.back().stops.push_back(demandElement);
                } else {
                    edgeStopIndex.push_back(EdgeStopIndex(edge, demandElement));
                }
            }
        }
        // declare index for current stop
        int currentEdgeStopIndex = 0;
        for (int i = 0; (i < (int)pathEdges.size()) && (currentEdgeStopIndex < (int)edgeStopIndex.size()); i++) {
            // check if current edge stop index is in the path
            if (edgeStopIndex[currentEdgeStopIndex].edge == pathEdges.at(i)) {
                edgeStopIndex[currentEdgeStopIndex].stopIndex = i;
                currentEdgeStopIndex++;
            } else {
                // check if edge exist in the rest of the path
                bool next = false;
                for (int j = (i + 1); j < (int)pathEdges.size(); j++) {
                    if (edgeStopIndex[currentEdgeStopIndex].edge == pathEdges.at(j)) {
                        next = true;
                    }
                }
                if (!next) {
                    // ignore current stops (because is out of path)
                    currentEdgeStopIndex++;
                }
            }
        }
    }
    // sort stops by position
    for (auto& edgeStop : edgeStopIndex) {
        if (edgeStop.stops.size() > 1) {
            // copy all stops to a map to sort it by endPos
            std::map<double, std::vector<GNEDemandElement*> > sortedStops;
            for (const auto& stop : edgeStop.stops) {
                if (sortedStops.count(stop->getAttributeDouble(SUMO_ATTR_ENDPOS)) == 0) {
                    sortedStops[stop->getAttributeDouble(SUMO_ATTR_ENDPOS)] = {stop};
                } else {
                    sortedStops[stop->getAttributeDouble(SUMO_ATTR_ENDPOS)].push_back(stop);
                }
            }
            // update stops with sorted stops
            edgeStop.stops.clear();
            for (const auto& sortedStop : sortedStops) {
                edgeStop.stops.insert(edgeStop.stops.end(), sortedStop.second.begin(), sortedStop.second.end());
            }
        }
    }
    return edgeStopIndex;
}


RGBColor
GNEDemandElement::getColorByScheme(const GUIColorer& c, const SUMOVehicleParameter* parameters) const {
    // set color depending of color active
    switch (c.getActive()) {
        case 0: {
            // test for emergency vehicle
            if (getTypeParent()->getAttribute(SUMO_ATTR_GUISHAPE) == "emergency") {
                return RGBColor::WHITE;
            }
            // test for firebrigade
            if (getTypeParent()->getAttribute(SUMO_ATTR_GUISHAPE) == "firebrigade") {
                return RGBColor::RED;
            }
            // test for police car
            if (getTypeParent()->getAttribute(SUMO_ATTR_GUISHAPE) == "police") {
                return RGBColor::BLUE;
            }
            if (getTypeParent()->getAttribute(SUMO_ATTR_GUISHAPE) == "scooter") {
                return RGBColor::WHITE;
            }
            // check if color was set
            if (parameters->wasSet(VEHPARS_COLOR_SET)) {
                return parameters->color;
            } else {
                // take their parent's color)
                return getTypeParent()->getColor();
            }
        }
        case 2: {
            if (parameters->wasSet(VEHPARS_COLOR_SET)) {
                return parameters->color;
            } else {
                return c.getScheme().getColor(0);
            }
        }
        case 3: {
            if (getTypeParent()->isAttributeEnabled(SUMO_ATTR_COLOR)) {
                return getTypeParent()->getColor();
            } else {
                return c.getScheme().getColor(0);
            }
            break;
        }
        case 4: {
            if (getRouteParent()->getColor() != RGBColor::DEFAULT_COLOR) {
                return getRouteParent()->getColor();
            } else {
                return c.getScheme().getColor(0);
            }
        }
        case 5: {
            Position p = getRouteParent()->getParentEdges().at(0)->getLanes().at(0)->getLaneShape()[0];
            const Boundary& b = myNet->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            return RGBColor::fromHSV(hue, sat, 1.);
        }
        case 6: {
            Position p = getRouteParent()->getParentEdges().back()->getLanes().at(0)->getLaneShape()[-1];
            const Boundary& b = myNet->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            return RGBColor::fromHSV(hue, sat, 1.);
        }
        case 7: {
            Position pb = getRouteParent()->getParentEdges().at(0)->getLanes().at(0)->getLaneShape()[0];
            Position pe = getRouteParent()->getParentEdges().back()->getLanes().at(0)->getLaneShape()[-1];
            const Boundary& b = myNet->getBoundary();
            double hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / M_PI;
            Position minp(b.xmin(), b.ymin());
            Position maxp(b.xmax(), b.ymax());
            double sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
            return RGBColor::fromHSV(hue, sat, 1.);
        }
        case 35: { // color randomly (by pointer hash)
            std::hash<const GNEDemandElement*> ptr_hash;
            const double hue = (double)(ptr_hash(this) % 360); // [0-360]
            const double sat = (double)((ptr_hash(this) / 360) % 67) / 100. + 0.33; // [0.33-1]
            return RGBColor::fromHSV(hue, sat, 1.);
        }
        default: {
            return c.getScheme().getColor(0);
        }
    }
}


std::string
GNEDemandElement::getDistributionParents() const {
    SumoXMLTag tagDistribution = SUMO_TAG_NOTHING;
    if (myTagProperty.getTag() == SUMO_TAG_VTYPE) {
        tagDistribution = SUMO_TAG_VTYPE_DISTRIBUTION;
    } else if (myTagProperty.getTag() == SUMO_TAG_ROUTE) {
        tagDistribution = SUMO_TAG_ROUTE_DISTRIBUTION;
    } else {
        return "";
    }
    // check if the current element is in the distributions
    std::vector<std::string> distributionParents;
    for (const auto& distribution : myNet->getAttributeCarriers()->getDemandElements().at(tagDistribution)) {
        if (distribution.second->keyExists(this)) {
            distributionParents.push_back(distribution.second->getID());
        }
    }
    return toString(distributionParents);
}


void
GNEDemandElement::buildMenuCommandRouteLength(GUIGLObjectPopupMenu* ret) const {
    std::vector<GNEEdge*> edges;
    if (myTagProperty.isRoute()) {
        edges = getParentEdges();
    } else if (myTagProperty.vehicleRoute()) {
        edges = getParentDemandElements().at(1)->getParentEdges();
    } else if (myTagProperty.vehicleRouteEmbedded()) {
        edges = getChildDemandElements().front()->getParentEdges();
    } else if (myTagProperty.vehicleEdges()) {
        edges = getParentEdges();
    }
    // calculate path
    const auto path = myNet->getPathManager()->getPathCalculator()->calculateDijkstraPath(getVClass(), edges);
    // check path size
    if (path.size() > 0) {
        double length = 0;
        for (const auto& edge : path) {
            length += edge->getNBEdge()->getFinalLength();
        }
        for (int i = 0; i < ((int)path.size() - 1); i++) {
            length += path.at(i)->getLanes().front()->getLane2laneConnections().getLane2laneGeometry(path.at(i + 1)->getLanes().front()).getShape().length();
        }
        GUIDesigns::buildFXMenuCommand(ret, TL("Route length: ") + toString(length), nullptr, ret, MID_COPY_NAME);
    }
}


void
GNEDemandElement::buildMenuAddReverse(GUIGLObjectPopupMenu* ret) const {
    // create menu pane for transform operations
    FXMenuPane* transformOperation = new FXMenuPane(ret);
    ret->insertMenuPaneChild(transformOperation);
    auto reverseMenuCascade = new FXMenuCascade(ret, TL("reverse"), nullptr, transformOperation);
    // build menu commands
    GUIDesigns::buildFXMenuCommand(transformOperation, TLF("reverse current %", myTagProperty.getTagStr()), nullptr, myNet->getViewNet(), MID_GNE_REVERSE);
    GUIDesigns::buildFXMenuCommand(transformOperation, TLF("Add reverse %", myTagProperty.getTagStr()), nullptr, myNet->getViewNet(), MID_GNE_ADDREVERSE);
    // check if reverse can be added
    if (GNERouteHandler::canReverse(this)) {
        reverseMenuCascade->enable();
    } else {
        reverseMenuCascade->disable();
    }
}

/****************************************************************************/
