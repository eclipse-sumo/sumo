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
/// @file    GNEAdditional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class for representation of additional elements
/****************************************************************************/
#include <config.h>
#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/demand/GNEPlanParents.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEAdditional.h"
#include "GNETAZ.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, std::string additionalName,
                             const std::vector<GNEJunction*>& junctionParents,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEGenericData*>& genericDataParents) :
    GNEPathManager::PathElement(type, id, icon, GNEPathManager::PathElement::Options::ADDITIONAL_ELEMENT),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    myAdditionalName(additionalName) {
    // check if is template
    myIsTemplate = (id == "");
}


GNEAdditional::GNEAdditional(GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, std::string additionalName,
                             const std::vector<GNEJunction*>& junctionParents,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEGenericData*>& genericDataParents) :
    GNEPathManager::PathElement(type, additionalParents.front()->getID(), icon, GNEPathManager::PathElement::Options::ADDITIONAL_ELEMENT),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    myAdditionalName(additionalName) {
}


GNEAdditional::~GNEAdditional() {}


void
GNEAdditional::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // currently there isn't additionals with removable geometry points
}


GUIGlObject*
GNEAdditional::getGUIGlObject() {
    return this;
}


const GUIGlObject*
GNEAdditional::getGUIGlObject() const {
    return this;
}


const std::string
GNEAdditional::getOptionalName() const {
    try {
        return getAttribute(SUMO_ATTR_NAME);
    } catch (InvalidArgument&) {
        return "";
    }
}


const GUIGeometry&
GNEAdditional::getAdditionalGeometry() const {
    return myAdditionalGeometry;
}


void
GNEAdditional::setSpecialColor(const RGBColor* color) {
    mySpecialColor = color;
}


bool
GNEAdditional::isAdditionalValid() const {
    return true;
}


std::string
GNEAdditional::getAdditionalProblem() const {
    return "";
}


void
GNEAdditional::fixAdditionalProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void
GNEAdditional::openAdditionalDialog() {
    throw InvalidArgument(getTagStr() + " doesn't have an additional dialog");
}


double
GNEAdditional::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNEAdditional::getCenteringBoundary() const {
    if (myAdditionalBoundary.isInitialised()) {
        return myAdditionalBoundary;
    } else {
        Boundary contourBoundary = myAdditionalContour.getContourBoundary();
        if (contourBoundary.isInitialised()) {
            contourBoundary.grow(5);
            return contourBoundary;
        } else if (myAdditionalGeometry.getShape().size() > 0) {
            Boundary geometryBoundary = myAdditionalGeometry.getShape().getBoxBoundary();
            geometryBoundary.grow(5);
            return geometryBoundary;
        } else if (getParentAdditionals().size() > 0) {
            return getParentAdditionals().front()->getCenteringBoundary();
        } else {
            Boundary centerBoundary(0, 0, 0, 0);
            centerBoundary.grow(5);
            return centerBoundary;
        }
    }
}


bool
GNEAdditional::checkDrawFromContour() const {
    // get modes and viewParent (for code legibility)
    const auto& modes = myNet->getViewNet()->getEditModes();
    const auto& viewParent = myNet->getViewNet()->getViewParent();
    // continue depending of current status
    if (myNet->getViewNet()->getInspectedAttributeCarriers().size() == 1) {
        // get inspected element
        const auto inspectedAC = myNet->getViewNet()->getInspectedAttributeCarriers().front();
        // check if starts in TAZ
        if (inspectedAC->hasAttribute(SUMO_ATTR_FROM_TAZ) && (inspectedAC->getAttribute(SUMO_ATTR_FROM_TAZ) == getID())) {
            return true;
        } else if ((inspectedAC->getTagProperty().getTag() == SUMO_TAG_TAZREL) && (inspectedAC->getAttribute(SUMO_ATTR_FROM) == getID())) {
            return true;
        }
    } else if (modes.isCurrentSupermodeDemand()) {
        // get current GNEPlanCreator
        GNEPlanCreator* planCreator = nullptr;
        if (modes.demandEditMode == DemandEditMode::DEMAND_PERSON) {
            planCreator = viewParent->getPersonFrame()->getPlanCreator();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) {
            planCreator = viewParent->getPersonPlanFrame()->getPlanCreator();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) {
            planCreator = viewParent->getContainerFrame()->getPlanCreator();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) {
            planCreator = viewParent->getContainerPlanFrame()->getPlanCreator();
        }
        // continue depending of planCreator
        if (planCreator) {
            // check if this is the from additional
            const auto additionalID = getID();
            if ((planCreator->getPlanParameteres().fromBusStop == additionalID) ||
                    (planCreator->getPlanParameteres().fromTrainStop == additionalID) ||
                    (planCreator->getPlanParameteres().fromContainerStop == additionalID) ||
                    (planCreator->getPlanParameteres().fromChargingStation == additionalID) ||
                    (planCreator->getPlanParameteres().fromParkingArea == additionalID) ||
                    (planCreator->getPlanParameteres().fromTAZ == additionalID)) {
                return true;
            }
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_VEHICLE) {
            // get selected TAZs
            const auto& selectedTAZs = viewParent->getVehicleFrame()->getPathCreator()->getSelectedTAZs();
            // check if this is the first selected TAZ
            if ((selectedTAZs.size() > 0) && (selectedTAZs.front() == this)) {
                return true;
            }
        }
    } else if (modes.isCurrentSupermodeData()) {
        // get TAZRelDataFrame
        const auto& TAZRelDataFrame = viewParent->getTAZRelDataFrame();
        if (TAZRelDataFrame->shown()) {
            // check first TAZ
            if (TAZRelDataFrame->getFirstTAZ() == nullptr) {
                return gViewObjectsHandler.isElementSelected(this);
            } else if (TAZRelDataFrame->getFirstTAZ() == this) {
                return true;
            }
        }
    }
    // nothing to draw
    return false;
}


bool
GNEAdditional::checkDrawToContour() const {
    // get modes and viewParent (for code legibility)
    const auto& modes = myNet->getViewNet()->getEditModes();
    const auto& viewParent = myNet->getViewNet()->getViewParent();
    // continue depending of current status
    if (myNet->getViewNet()->getInspectedAttributeCarriers().size() == 1) {
        // get inspected element
        const auto inspectedAC = myNet->getViewNet()->getInspectedAttributeCarriers().front();
        // check if starts in TAZ
        if (inspectedAC->hasAttribute(SUMO_ATTR_TO_TAZ) && (inspectedAC->getAttribute(SUMO_ATTR_TO_TAZ) == getID())) {
            return true;
        } else if ((inspectedAC->getTagProperty().getTag() == SUMO_TAG_TAZREL) && (inspectedAC->getAttribute(SUMO_ATTR_TO) == getID())) {
            return true;
        }
    } else if (modes.isCurrentSupermodeDemand()) {
        // get current GNEPlanCreator
        GNEPlanCreator* planCreator = nullptr;
        if (modes.demandEditMode == DemandEditMode::DEMAND_PERSON) {
            planCreator = viewParent->getPersonFrame()->getPlanCreator();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) {
            planCreator = viewParent->getPersonPlanFrame()->getPlanCreator();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) {
            planCreator = viewParent->getContainerFrame()->getPlanCreator();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) {
            planCreator = viewParent->getContainerPlanFrame()->getPlanCreator();
        }
        // continue depending of planCreator
        if (planCreator) {
            // check if this is the from additional
            const auto additionalID = getID();
            if ((planCreator->getPlanParameteres().toBusStop == additionalID) ||
                    (planCreator->getPlanParameteres().toTrainStop == additionalID) ||
                    (planCreator->getPlanParameteres().toContainerStop == additionalID) ||
                    (planCreator->getPlanParameteres().toChargingStation == additionalID) ||
                    (planCreator->getPlanParameteres().toParkingArea == additionalID) ||
                    (planCreator->getPlanParameteres().toTAZ == additionalID)) {
                return true;
            }
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_VEHICLE) {
            // get selected TAZs
            const auto& selectedTAZs = viewParent->getVehicleFrame()->getPathCreator()->getSelectedTAZs();
            // check if this is the first selected TAZ
            if ((selectedTAZs.size() > 1) && (selectedTAZs.back() == this)) {
                return true;
            }
        }
    } else if (modes.isCurrentSupermodeData()) {
        // get TAZRelDataFrame
        const auto& TAZRelDataFrame = viewParent->getTAZRelDataFrame();
        if (TAZRelDataFrame->shown()) {
            // check first TAZ
            if (TAZRelDataFrame->getFirstTAZ() == nullptr) {
                return gViewObjectsHandler.isElementSelected(this);
            } else if (TAZRelDataFrame->getFirstTAZ() == this) {
                return true;
            }
        }
    }
    // nothing to draw
    return false;
}


bool
GNEAdditional::checkDrawRelatedContour() const {
    return false;
}


bool
GNEAdditional::checkDrawOverContour() const {
    // get modes and viewParent (for code legibility)
    const auto& modes = myNet->getViewNet()->getEditModes();
    const auto& viewParent = myNet->getViewNet()->getViewParent();
    if (modes.isCurrentSupermodeDemand()) {
        // get current plan selector
        GNEPlanSelector* planSelector = nullptr;
        if (modes.demandEditMode == DemandEditMode::DEMAND_PERSON) {
            planSelector = viewParent->getPersonFrame()->getPlanSelector();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) {
            planSelector = viewParent->getPersonPlanFrame()->getPlanSelector();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) {
            planSelector = viewParent->getContainerFrame()->getPlanSelector();
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) {
            planSelector = viewParent->getContainerPlanFrame()->getPlanSelector();
        }
        // continue depending of plan selector
        if (planSelector) {
            if ((myTagProperty.isStoppingPlace() && planSelector->markStoppingPlaces()) ||
                    (myTagProperty.isTAZElement() && planSelector->markTAZs())) {
                return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
            }
        } else if (modes.demandEditMode == DemandEditMode::DEMAND_VEHICLE) {
            // get current vehicle template
            const auto& vehicleTemplate = viewParent->getVehicleFrame()->getVehicleTagSelector()->getCurrentTemplateAC();
            // check if vehicle can be placed over from-to TAZs
            if (vehicleTemplate && vehicleTemplate->getTagProperty().vehicleTAZs()) {
                return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
            }
        }
    }
    return false;
}


bool
GNEAdditional::checkDrawDeleteContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in delete mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_DELETE)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEAdditional::checkDrawSelectContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in select mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_SELECT)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


GUIGLObjectPopupMenu*
GNEAdditional::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, TLF("Copy % name to clipboard", getTagStr()), nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, TLF("Copy % typed name to clipboard", getTagStr()), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    // show option to open additional dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Open ") + getTagStr() + TL(" Dialog"), getACIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    // Show position parameters
    if (myTagProperty.hasAttribute(SUMO_ATTR_LANE) && (myAdditionalGeometry.getShape().size() > 1)) {
        const GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myAdditionalGeometry.getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        GUIDesigns::buildFXMenuCommand(ret, TL("Cursor position over additional shape: ") + toString(innerPos), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command lane position
        if (myAdditionalGeometry.getShape().size() > 0) {
            const double lanePos = lane->getLaneShape().nearest_offset_to_point2D(myAdditionalGeometry.getShape().front());
            GUIDesigns::buildFXMenuCommand(ret, TL("Cursor position over lane: ") + toString(innerPos + lanePos), nullptr, nullptr, 0);
        }
    } else if (myTagProperty.hasAttribute(SUMO_ATTR_EDGE) && (myAdditionalGeometry.getShape().size() > 1)) {
        const GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
        // Show menu command inner position
        const double innerPos = myAdditionalGeometry.getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        GUIDesigns::buildFXMenuCommand(ret, TL("Cursor position over additional shape: ") + toString(innerPos), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command edge position
        if (myAdditionalGeometry.getShape().size() > 0) {
            const double edgePos = edge->getLanes().at(0)->getLaneShape().nearest_offset_to_point2D(myAdditionalGeometry.getShape().front());
            GUIDesigns::buildFXMenuCommand(ret, TL("Mouse position over edge: ") + toString(innerPos + edgePos), nullptr, nullptr, 0);
        }
    } else {
        const auto mousePos = myNet->getViewNet()->getPositionInformation();
        GUIDesigns::buildFXMenuCommand(ret, TL("Cursor position in view: ") + toString(mousePos.x()) + "," + toString(mousePos.y()), nullptr, nullptr, 0);
    }
    return ret;
}


GUIParameterTableWindow*
GNEAdditional::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& attributeProperty : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (attributeProperty.isUnique()) {
            ret->mkItem(attributeProperty.getAttrStr().c_str(), false, getAttribute(attributeProperty.getAttr()));
        } else {
            ret->mkItem(attributeProperty.getAttrStr().c_str(), true, getAttribute(attributeProperty.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string&
GNEAdditional::getOptionalAdditionalName() const {
    return myAdditionalName;
}


bool
GNEAdditional::isGLObjectLocked() const {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return myNet->getViewNet()->getLockManager().isObjectLocked(getType(), isAttributeCarrierSelected());
    } else {
        return true;
    }
}


void
GNEAdditional::markAsFrontElement() {
    myNet->getViewNet()->setFrontAttributeCarrier(this);
}


void
GNEAdditional::deleteGLObject() {
    myNet->deleteAdditional(this, myNet->getViewNet()->getUndoList());
}


void
GNEAdditional::selectGLObject() {
    if (isAttributeCarrierSelected()) {
        unselectAttributeCarrier();
    } else {
        selectAttributeCarrier();
    }
    // update information label
    myNet->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionInformation()->updateInformationLabel();
}


void GNEAdditional::updateGLObject() {
    updateGeometry();
}


void
GNEAdditional::computePathElement() {
    // Nothing to compute
}


bool
GNEAdditional::isPathElementSelected() const {
    return mySelected;
}


void
GNEAdditional::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Nothing to draw
}


void
GNEAdditional::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Nothing to draw
}

// ---------------------------------------------------------------------------
// GNEAdditional - protected methods
// ---------------------------------------------------------------------------

bool
GNEAdditional::isValidAdditionalID(const std::string& value) const {
    if (!isTemplate() && (value == getID())) {
        return true;
    } else if (SUMOXMLDefinitions::isValidAdditionalID(value)) {
        return (myNet->getAttributeCarriers()->retrieveAdditional(myTagProperty.getTag(), value, false) == nullptr);
    } else {
        return false;
    }
}


bool
GNEAdditional::isValidAdditionalID(const std::vector<SumoXMLTag>& tags, const std::string& value) const {
    if (!isTemplate() && (value == getID())) {
        return true;
    } else if (SUMOXMLDefinitions::isValidAdditionalID(value)) {
        return (myNet->getAttributeCarriers()->retrieveAdditionals(tags, value, false) == nullptr);
    } else {
        return false;
    }
}


bool
GNEAdditional::isValidDetectorID(const std::string& value) const {
    if (!isTemplate() && (value == getID())) {
        return true;
    } else if (SUMOXMLDefinitions::isValidDetectorID(value)) {
        return (myNet->getAttributeCarriers()->retrieveAdditional(myTagProperty.getTag(), value, false) == nullptr);
    } else {
        return false;
    }
}


bool
GNEAdditional::isValidDetectorID(const std::vector<SumoXMLTag>& tags, const std::string& value) const {
    if (!isTemplate() && (value == getID())) {
        return true;
    } else if (SUMOXMLDefinitions::isValidDetectorID(value)) {
        return (myNet->getAttributeCarriers()->retrieveAdditionals(tags, value, false) == nullptr);
    } else {
        return false;
    }
}


void
GNEAdditional::setAdditionalID(const std::string& newID) {
    // update ID
    if (isTemplate()) {
        setMicrosimID(newID);
    } else if ((myTagProperty.getTag() == SUMO_TAG_VAPORIZER) || !myTagProperty.hasAttribute(SUMO_ATTR_ID)) {
        setMicrosimID(newID);
    } else {
        myNet->getAttributeCarriers()->updateAdditionalID(this, newID);
    }
    // change IDs of certain children
    for (const auto& additionalChild : getChildAdditionals()) {
        // get tag
        const auto tag = additionalChild->getTagProperty().getTag();
        if ((tag == SUMO_TAG_ACCESS) || (tag == SUMO_TAG_PARKING_SPACE) ||
                (tag == SUMO_TAG_DET_ENTRY) || (tag == SUMO_TAG_DET_EXIT)) {
            additionalChild->setAdditionalID(getID());
        }
    }
    // enable save demand elements if this additional has children
    if (getChildDemandElements().size() > 0) {
        myNet->getSavingStatus()->requireSaveDemandElements();
    }
    // enable save data elements if this additional has children
    if (getChildGenericDatas().size() > 0) {
        myNet->getSavingStatus()->requireSaveDataElements();
    }
}


void
GNEAdditional::drawAdditionalID(const GUIVisualizationSettings& s) const {
    if (s.addName.show(this) && (myAdditionalGeometry.getShape().size() > 0) && !s.drawForRectangleSelection) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate position
        const Position pos = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShape().front() : myAdditionalGeometry.getShape().positionAtOffset2D(middlePoint);
        // calculate rotation
        const double rot = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShapeRotations().front() : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint);
        // draw additional ID
        if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
            GLHelper::drawText(getMicrosimID(), pos, GLO_MAX - getType(), s.addName.scaledSize(s.scale), s.addName.color, s.getTextAngle(rot - 90));
        } else {
            GLHelper::drawText(getMicrosimID(), pos, GLO_MAX - getType(), s.addName.scaledSize(s.scale), s.addName.color, 0);
        }
    }
}


void
GNEAdditional::drawAdditionalName(const GUIVisualizationSettings& s) const {
    if (s.addFullName.show(this) && (myAdditionalGeometry.getShape().size() > 0) && (myAdditionalName != "") && !s.drawForRectangleSelection) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate position
        const Position pos = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShape().front() : myAdditionalGeometry.getShape().positionAtOffset2D(middlePoint);
        // calculate rotation
        const double rot = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShapeRotations().front() : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint);
        // draw additional name
        if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
            GLHelper::drawText(myAdditionalName, pos, GLO_MAX - getType(), s.addFullName.scaledSize(s.scale), s.addFullName.color, s.getTextAngle(rot - 90));
        } else {
            GLHelper::drawText(myAdditionalName, pos, GLO_MAX - getType(), s.addFullName.scaledSize(s.scale), s.addFullName.color, 0);
        }
    }
}


void
GNEAdditional::replaceAdditionalParentEdges(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNEEdge*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalParentLanes(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNELane*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalChildEdges(const std::string& value) {
    replaceChildElements(this, parse<std::vector<GNEEdge*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalChildLanes(const std::string& value) {
    replaceChildElements(this, parse<std::vector<GNELane*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEAdditional*> parentAdditionals;
    // special case for calibrators and routeprobes
    if (value.size() > 0) {
        parentAdditionals = getParentAdditionals();
        if ((parentAdditionals.size() == 0) && (parentIndex == 0)) {
            parentAdditionals.push_back(myNet->getAttributeCarriers()->retrieveAdditional(tag, value));
        } else {
            parentAdditionals[parentIndex] = myNet->getAttributeCarriers()->retrieveAdditional(tag, value);
        }
    }
    // replace parent additionals
    replaceParentElements(this, parentAdditionals);
}


void
GNEAdditional::replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEDemandElement*> parentDemandElements = getParentDemandElements();
    parentDemandElements[parentIndex] = myNet->getAttributeCarriers()->retrieveDemandElement(tag, value);
    // replace parent demand elements
    replaceParentElements(this, parentDemandElements);
}


void
GNEAdditional::shiftLaneIndex() {
    // get new lane parent vector
    std::vector<GNELane*> newLane = {getParentLanes().front()->getParentEdge()->getLanes().at(getParentLanes().front()->getIndex() + 1)};
    // replace parent elements
    replaceParentElements(this, newLane);
}


void
GNEAdditional::calculatePerpendicularLine(const double endLaneposition) {
    if (getParentEdges().empty()) {
        throw ProcessError(TL("Invalid number of edges"));
    } else {
        // get lanes
        const GNELane* firstLane = getParentEdges().front()->getLanes().front();
        const GNELane* lastLane = getParentEdges().front()->getLanes().back();
        // get first and back lane shapes
        PositionVector firstLaneShape = firstLane->getLaneShape();
        PositionVector lastLaneShape = lastLane->getLaneShape();
        // move shapes
        firstLaneShape.move2side((firstLane->getParentEdge()->getNBEdge()->getLaneWidth(firstLane->getIndex()) * 0.5) + 1);
        lastLaneShape.move2side(lastLane->getParentEdge()->getNBEdge()->getLaneWidth(lastLane->getIndex()) * -0.5);
        // calculate lane postion
        const double lanePosition = firstLaneShape.length2D() >= endLaneposition ? endLaneposition : firstLaneShape.length2D();
        // update geometry
        myAdditionalGeometry.updateGeometry({firstLaneShape.positionAtOffset2D(lanePosition), lastLaneShape.positionAtOffset2D(lanePosition)});
    }
}


void
GNEAdditional::drawSquaredAdditional(const GUIVisualizationSettings& s, const Position& pos, const double size, GUITexture texture, GUITexture selectedTexture) const {
    // draw boundaries
    GLHelper::drawBoundary(s, getCenteringBoundary());
    // Obtain drawing exaggeration
    const double exaggeration = getExaggeration(s);
    // get detail level
    const auto d = s.getDetailLevel(exaggeration);
    // draw geometry only if we'rent in drawForObjectUnderCursor mode
    if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
        // Add layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // translate to position
        glTranslated(pos.x(), pos.y(), 0);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        // set White color
        glColor3d(1, 1, 1);
        // rotate
        glRotated(180, 0, 0, 1);
        // draw texture
        if (drawUsingSelectColor()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(selectedTexture), size);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(texture), size);
        }
        // Pop layer matrix
        GLHelper::popMatrix();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), pos, exaggeration, 0.4, 0.5, 0.5);
        // Draw additional ID
        drawAdditionalID(s);
        // draw additional name
        drawAdditionalName(s);
        // draw dotted contour
        myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
    }
    // calculate contour
    myAdditionalContour.calculateContourRectangleShape(s, d, this, pos, size, size, 0, 0, 0, exaggeration);
}


void
GNEAdditional::drawListedAdditional(const GUIVisualizationSettings& s, const Position& parentPosition, const double offsetX,
                                    const double extraOffsetY, const RGBColor baseCol, const RGBColor textCol, GUITexture texture,
                                    const std::string text) const {
    // check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // draw boundaries
        GLHelper::drawBoundary(s, getCenteringBoundary());
        // get detail level
        const auto d = s.getDetailLevel(1);
        // declare offsets
        const double lineOffset = 0.1875;
        const double baseOffsetX = 6.25;
        const double baseOffsetY = 0.6;
        // get draw position index
        const int drawPositionIndex = getDrawPositionIndex();
        // calculate lineA position (from parent to middle)
        Position positionLineA = parentPosition;
        const double positionLineA_Y = (0 - extraOffsetY + baseOffsetY);
        // set position depending of indexes
        positionLineA.add(1 + lineOffset + (baseOffsetX * offsetX), positionLineA_Y, 0);
        // calculate lineC position (From middle until current listenAdditional
        Position positionLineB = parentPosition;
        const double positionLineB_Y = ((drawPositionIndex * -1) - extraOffsetY + baseOffsetY);
        // set position depending of indexes
        positionLineB.add(1 + lineOffset + (baseOffsetX * offsetX) + (2 * lineOffset), positionLineB_Y, 0);
        // calculate signPosition position
        Position signPosition = parentPosition;
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // set position depending of indexes
            signPosition.add(4.5 + (baseOffsetX * offsetX), (drawPositionIndex * -1) - extraOffsetY + 1, 0);
            // calculate colors
            const RGBColor baseColor = isAttributeCarrierSelected() ? s.colorSettings.selectedAdditionalColor : baseCol;
            const RGBColor secondColor = baseColor.changedBrightness(-30);
            const RGBColor textColor = isAttributeCarrierSelected() ? s.colorSettings.selectedAdditionalColor.changedBrightness(30) : textCol;
            // Add layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
            // set line color
            GLHelper::setColor(s.additionalSettings.connectionColor);
            // draw both lines
            GLHelper::drawBoxLine(positionLineA, 0, 0.1, lineOffset);
            GLHelper::drawBoxLine(positionLineB, 0, 0.1, lineOffset);
            // check if draw middle lane
            if (drawPositionIndex != 0) {
                // calculate length
                const double length = std::abs(positionLineA_Y - positionLineB_Y);
                // push middle lane matrix
                GLHelper::pushMatrix();
                //move and rotate
                glTranslated(positionLineA.x() + lineOffset, positionLineA.y(), 0);
                glRotated(90, 0, 0, 1);
                glTranslated((length * -0.5), 0, 0);
                // draw line
                GLHelper::drawBoxLine(Position(0, 0), 0, 0.1, length * 0.5);
                // pop middle lane matrix
                GLHelper::popMatrix();
            }
            // draw extern rectangle
            GLHelper::setColor(secondColor);
            GLHelper::drawBoxLine(signPosition, 0, 0.96, 2.75);
            // move to front
            glTranslated(0, -0.06, 0.1);
            // draw intern rectangle
            GLHelper::setColor(baseColor);
            GLHelper::drawBoxLine(signPosition, 0, 0.84, 2.69);
            // move position down
            signPosition.add(-2, -0.43, 0);
            // draw interval
            GLHelper::drawText(adjustListedAdditionalText(text), signPosition, .1, 0.5, textColor, 0, (FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE));
            // move to icon position
            signPosition.add(-0.3, 0);
            // check if draw lock icon or rerouter interval icon
            if (GNEViewNetHelper::LockIcon::checkDrawing(d, this, getType(), 1)) {
                // pop layer matrix
                GLHelper::popMatrix();
                // draw lock icon
                GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), signPosition, 1, 0.4, 0.0, -0.05);
            } else {
                // translate to front
                glTranslated(signPosition.x(), signPosition.y(), 0.1);
                // set White color
                glColor3d(1, 1, 1);
                // rotate
                glRotated(180, 0, 0, 1);
                // draw texture
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(texture), 0.25);
                // pop layer matrix
                GLHelper::popMatrix();
            }
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour
        myAdditionalContour.calculateContourRectangleShape(s, d, this, signPosition, 0.56, 2.75, 0, -2.3, 0, 1);
    }
}


bool
GNEAdditional::drawMovingGeometryPoints(const bool ignoreShift) const {
    // get modes
    const auto& modes = myNet->getViewNet()->getEditModes();
    // check conditions
    if (modes.isCurrentSupermodeNetwork() && (modes.networkEditMode == NetworkEditMode::NETWORK_MOVE) &&
            (ignoreShift || myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed())) {
        return true;
    } else {
        return false;
    }
}


void
GNEAdditional::drawDemandElementChildren(const GUIVisualizationSettings& s) const {
    // draw child demand elements
    for (const auto& demandElement : getChildDemandElements()) {
        if (!demandElement->getTagProperty().isPlacedInRTree()) {
            demandElement->drawGL(s);
        }
    }
}


GNEMoveOperation*
GNEAdditional::getMoveOperationSingleLane(const double startPos, const double endPos) {
    // get allow change lane
    const bool allowChangeLane = myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane();
    // fist check if we're moving only extremes
    if (myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
        // get snap radius
        const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
        // get mouse position
        const Position mousePosition = myNet->getViewNet()->getPositionInformation();
        // check if we clicked over start or end position
        if (myAdditionalGeometry.getShape().front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // move only start position
            return new GNEMoveOperation(this, getParentLanes().front(), startPos, endPos,
                                        allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST);
        } else if (myAdditionalGeometry.getShape().back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // move only end position
            return new GNEMoveOperation(this, getParentLanes().front(), startPos, endPos,
                                        allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND);
        } else {
            return nullptr;
        }
    } else {
        // move both start and end positions
        return new GNEMoveOperation(this, getParentLanes().front(), startPos, endPos,
                                    allowChangeLane, GNEMoveOperation::OperationType::ONE_LANE_MOVEBOTH);
    }
}


GNEMoveOperation*
GNEAdditional::getMoveOperationMultiLane(const double startPos, const double endPos) {
    // check if shift is pressed
    const bool shift = myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed();
    // get snap radius
    const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
    // get mouse position
    const Position mousePosition = myNet->getViewNet()->getPositionInformation();
    // calculate both geometries
    GUIGeometry fromGeometry, toGeometry;
    fromGeometry.updateGeometry(getParentLanes().front()->getLaneGeometry().getShape(), startPos, 0);
    toGeometry.updateGeometry(getParentLanes().back()->getLaneGeometry().getShape(), endPos, 0);
    // check if we clicked over start or end position
    if (fromGeometry.getShape().front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
        // move using start position
        return new GNEMoveOperation(this, getParentLanes().front(), startPos, getParentLanes().back(), endPos,
                                    false, shift ? GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST : GNEMoveOperation::OperationType::TWO_LANES_MOVEBOTH_FIRST);
    } else if (toGeometry.getShape().back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
        // move using end position
        return new GNEMoveOperation(this, getParentLanes().front(), startPos, getParentLanes().back(), endPos,
                                    false, shift ? GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND : GNEMoveOperation::OperationType::TWO_LANES_MOVEBOTH_SECOND);
    } else {
        return nullptr;
    }
}


std::string
GNEAdditional::getJuPedSimType(SumoXMLTag tag) {
    // continue depending of tag
    switch (tag) {
        case GNE_TAG_JPS_WALKABLEAREA:
            return "jupedsim.walkable_area";
        case GNE_TAG_JPS_OBSTACLE:
            return "jupedsim.obstacle";
        default:
            throw InvalidArgument("Invalid JuPedSim tag");
    }
}


RGBColor
GNEAdditional::getJuPedSimColor(SumoXMLTag tag) {
    // continue depending of tag
    switch (tag) {
        case GNE_TAG_JPS_WALKABLEAREA:
            return RGBColor(179, 217, 255);
        case GNE_TAG_JPS_OBSTACLE:
            return RGBColor(255, 204, 204);
        default:
            throw InvalidArgument("Invalid JuPedSim tag");
    }
}


bool
GNEAdditional::getJuPedSimFill(SumoXMLTag tag) {
    // continue depending of tag
    switch (tag) {
        case GNE_TAG_JPS_WALKABLEAREA:
        case GNE_TAG_JPS_OBSTACLE:
            return true;
        default:
            throw InvalidArgument("Invalid JuPedSim tag");
    }
}


double
GNEAdditional::getJuPedSimLayer(SumoXMLTag tag) {
    // continue depending of tag
    switch (tag) {
        case GNE_TAG_JPS_WALKABLEAREA:
            return 1;
        case GNE_TAG_JPS_OBSTACLE:
            return 2;
        default:
            throw InvalidArgument("Invalid JuPedSim tag");
    }
}


GUIGlObjectType
GNEAdditional::getJuPedSimGLO(SumoXMLTag tag) {
    // continue depending of tag
    switch (tag) {
        case GNE_TAG_JPS_WALKABLEAREA:
            return GLO_JPS_WALKABLEAREA;
        case GNE_TAG_JPS_OBSTACLE:
            return GLO_JPS_OBSTACLE;
        default:
            throw InvalidArgument("Invalid JuPedSim tag");
    }
}


FXIcon*
GNEAdditional::getJuPedSimIcon(SumoXMLTag tag) {
    // continue depending of tag
    switch (tag) {
        case GNE_TAG_JPS_WALKABLEAREA:
            return GUIIconSubSys::getIcon(GUIIcon::JPS_WALKABLEAREA);
        case GNE_TAG_JPS_OBSTACLE:
            return GUIIconSubSys::getIcon(GUIIcon::JPS_OBSTACLE);
        default:
            throw InvalidArgument("Invalid JuPedSim tag");
    }
}


void
GNEAdditional::calculateContourPolygons(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const double exaggeration, const bool contouredShape) const {
    // calculate contour depenidng of contoured shape
    if (contouredShape) {
        myAdditionalContour.calculateContourClosedShape(s, d, this, myAdditionalGeometry.getShape(), 1);
    } else {
        myAdditionalContour.calculateContourExtrudedShape(s, d, this, myAdditionalGeometry.getShape(), s.neteditSizeSettings.polylineWidth,
                exaggeration, true, true, 0);
    }
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if draw geometry points
    if (editModes.isCurrentSupermodeNetwork() && !myNet->getViewNet()->getViewParent()->getMoveFrame()->getNetworkModeOptions()->getMoveWholePolygons()) {
        // check if we're in move mode
        const bool moveMode = (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE);
        // get geometry point radius (size depends if we're in move mode)
        const double geometryPointRaidus = s.neteditSizeSettings.polygonGeometryPointRadius * (moveMode ? 1 : 0.5);
        // calculate contour geometry points
        myAdditionalContour.calculateContourAllGeometryPoints(s, d, this, myAdditionalGeometry.getShape(), geometryPointRaidus, exaggeration, moveMode);
    }
}


GNELane*
GNEAdditional::getFirstPathLane() const {
    return getParentLanes().front();
}


GNELane*
GNEAdditional::getLastPathLane() const {
    return getParentLanes().back();
}


Position
GNEAdditional::getAttributePosition(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
}


void
GNEAdditional::drawParentChildLines(const GUIVisualizationSettings& s, const RGBColor& color, const bool onlySymbols) const {
    // check if current additional is inspected, front or selected
    const bool currentDrawEntire = myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                                   (myNet->getViewNet()->getFrontAttributeCarrier() == this) || isAttributeCarrierSelected();
    // push layer matrix
    GLHelper::pushMatrix();
    // translate to parentChildLine layer
    glTranslated(0, 0, GLO_PARENTCHILDLINE);
    // iterate over parent additionals
    for (const auto& parent : getParentAdditionals()) {
        // get inspected flag
        const bool inspected = myNet->getViewNet()->isAttributeCarrierInspected(parent);
        // draw parent lines
        GUIGeometry::drawParentLine(s, getPositionInView(), parent->getPositionInView(),
                                    (isAttributeCarrierSelected() || parent->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                    currentDrawEntire || inspected || parent->isAttributeCarrierSelected(), .05);
    }
    // special case for Parking area reroutes
    if (getTagProperty().getTag() == SUMO_TAG_REROUTER) {
        // iterate over rerouter elements
        for (const auto& rerouterInterval : getChildAdditionals()) {
            for (const auto& rerouterElement : rerouterInterval->getChildAdditionals()) {
                if (rerouterElement->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA_REROUTE) {
                    // get parking area
                    const auto parkingArea = rerouterElement->getParentAdditionals().at(1);
                    // get inspected flag
                    const bool inspected = myNet->getViewNet()->isAttributeCarrierInspected(parkingArea);
                    // draw parent lines
                    GUIGeometry::drawParentLine(s, getPositionInView(), parkingArea->getPositionInView(),
                                                (isAttributeCarrierSelected() || parkingArea->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                                currentDrawEntire || inspected || parkingArea->isAttributeCarrierSelected(), .05);
                }
            }
        }
    }
    // iterate over child additionals
    for (const auto& child : getChildAdditionals()) {
        // get inspected flag
        const bool inspected = myNet->getViewNet()->isAttributeCarrierInspected(child);
        // special case for parking zone reroute
        if (child->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA_REROUTE) {
            // draw child line between parking area and rerouter
            GUIGeometry::drawChildLine(s, getPositionInView(), child->getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView(),
                                       (isAttributeCarrierSelected() || child->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                       currentDrawEntire || inspected || child->isAttributeCarrierSelected(), .05);
        } else if (!onlySymbols || child->getTagProperty().isSymbol()) {
            // draw child line
            GUIGeometry::drawChildLine(s, getPositionInView(), child->getPositionInView(),
                                       (isAttributeCarrierSelected() || child->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                       currentDrawEntire || inspected || child->isAttributeCarrierSelected(), .05);
        }
    }
    // pop layer matrix
    GLHelper::popMatrix();
}


void
GNEAdditional::drawUpGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                                   const double rot, const RGBColor& baseColor, const bool ignoreShift) const {
    drawSemiCircleGeometryPoint(s, d, pos, rot, baseColor, -90, 90, ignoreShift);
}

void
GNEAdditional::drawDownGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                                     const double rot, const RGBColor& baseColor, const bool ignoreShift) const {
    drawSemiCircleGeometryPoint(s, d, pos, rot, baseColor, 90, 270, ignoreShift);
}

void
GNEAdditional::drawLeftGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                                     const double rot, const RGBColor& baseColor, const bool ignoreShift) const {
    drawSemiCircleGeometryPoint(s, d, pos, rot, baseColor, -90, 90, ignoreShift);
}


void
GNEAdditional::drawRightGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                                      const double rot, const RGBColor& baseColor, const bool ignoreShift) const {
    drawSemiCircleGeometryPoint(s, d, pos, rot, baseColor, 270, 90, ignoreShift);
}


int
GNEAdditional::getDrawPositionIndex() const {
    // filter symbols
    std::vector<GNEAdditional*> children;
    for (const auto& child : getParentAdditionals().front()->getChildAdditionals()) {
        if (!child->getTagProperty().isSymbol()) {
            children.push_back(child);
        }
    }
    // now get index
    for (int i = 0; i < (int)children.size(); i++) {
        if (children.at(i) == this) {
            return i;
        }
    }
    return 0;
}


bool
GNEAdditional::areLaneConsecutives(const std::vector<GNELane*>& lanes) {
    // declare lane iterator
    int laneIt = 0;
    // iterate over all lanes
    while (laneIt < ((int)lanes.size() - 1)) {
        // we assume that lanes aren't consecutive
        bool consecutiveFound = false;
        // get lanes
        const auto lane = lanes.at(laneIt);
        const auto nextLane = lanes.at(laneIt + 1);
        // if there is a connection between "from" lane and "to" lane of connection, change connectionFound to true
        for (const auto& outgoingEdge : lane->getParentEdge()->getToJunction()->getGNEOutgoingEdges()) {
            for (const auto& outgoingLane : outgoingEdge->getLanes()) {
                if (outgoingLane == nextLane) {
                    consecutiveFound = true;
                }
            }
        }
        // abort if consecutiveFound is false
        if (!consecutiveFound) {
            return false;
        }
        // update iterator
        laneIt++;
    }
    // lanes are consecutive, then return true
    return true;
}


bool
GNEAdditional::areLaneConnected(const std::vector<GNELane*>& lanes) {
    // declare lane iterator
    int laneIt = 0;
    // iterate over all lanes, and stop if myE2valid is false
    while (laneIt < ((int)lanes.size() - 1)) {
        // we assume that E2 is invalid
        bool connectionFound = false;
        // get lanes
        const auto lane = lanes.at(laneIt);
        const auto nextLane = lanes.at(laneIt + 1);
        // check if both lanes are sidewalks
        if ((lane->getAttribute(SUMO_ATTR_ALLOW) == "pedestrian") && (nextLane->getAttribute(SUMO_ATTR_ALLOW) == "pedestrian")) {
            connectionFound = true;
        }
        // if there is a connection between "from" lane and "to" lane of connection, change connectionFound to true
        for (const auto& connection : lane->getParentEdge()->getNBEdge()->getConnections()) {
            if ((connection.toEdge == nextLane->getParentEdge()->getNBEdge()) &&
                    (connection.fromLane == lane->getIndex()) &&
                    (connection.toLane == nextLane->getIndex())) {
                connectionFound = true;
            }
        }
        // abort if connectionFound is false
        if (!connectionFound) {
            return false;
        }
        // update iterator
        laneIt++;
    }
    // there are connections between all lanes, then return true
    return true;
}


bool
GNEAdditional::checkChildAdditionalRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError(StringUtils::format("Calling non-implemented function checkChildAdditionalRestriction during saving of %. It muss be reimplemented in child class", getTagStr()));
}


void
GNEAdditional::drawSemiCircleGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const Position& pos, const double rot, const RGBColor& baseColor, const double fromAngle, const double toAngle,
        const bool /* ignoreShift */) const {
    // check if draw geometry point
    if (!s.drawForViewObjectsHandler && (d <= GUIVisualizationSettings::Detail::GeometryPoint)) {
        // push matrix
        GLHelper::pushMatrix();
        // translated to front
        glTranslated(0, 0, 0.1);
        // set color depending if check if mouse is over element
        GLHelper::setColor(baseColor.changedBrightness(-50));
        // translate and rotate
        glTranslated(pos.x(), pos.y(), 0.1);
        glRotated(rot, 0, 0, 1);
        // draw geometry point
        GLHelper::drawFilledCircleDetailled(d, s.neteditSizeSettings.additionalGeometryPointRadius, fromAngle, toAngle);
        // pop geometry point matrix
        GLHelper::popMatrix();
    }
}


std::string
GNEAdditional::adjustListedAdditionalText(const std::string& text) const {
    // 10 + 3 + 10
    if (text.size() <= 23) {
        return text;
    } else {
        // get text size
        const int textPosition = (int)text.size() - 10;
        // declare strings
        std::string partA, partB;
        // resize
        partA.reserve(10);
        partB.reserve(10);
        // fill both
        for (int i = 0; i < 10; i++) {
            partA.push_back(text.at(i));
            partB.push_back(text.at(textPosition + i));
        }
        // return composition
        return (partA + "..." + partB);
    }
}

/****************************************************************************/
