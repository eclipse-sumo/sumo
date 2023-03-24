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
/// @file    GNEContainer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// Representation of containers in netedit
/****************************************************************************/
#include <cmath>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_EnableAttribute.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIBasePersonHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalPostDrawing.h>

#include "GNEContainer.h"
#include "GNERouteHandler.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEContainer::GNEContainerPopupMenu) containerPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CONTAINER_TRANSFORM,   GNEContainer::GNEContainerPopupMenu::onCmdTransform),
};

FXDEFMAP(GNEContainer::GNESelectedContainersPopupMenu) selectedContainersPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CONTAINER_TRANSFORM,   GNEContainer::GNESelectedContainersPopupMenu::onCmdTransform),
};

// Object implementation
FXIMPLEMENT(GNEContainer::GNEContainerPopupMenu,          GUIGLObjectPopupMenu, containerPopupMenuMap,           ARRAYNUMBER(containerPopupMenuMap))
FXIMPLEMENT(GNEContainer::GNESelectedContainersPopupMenu, GUIGLObjectPopupMenu, selectedContainersPopupMenuMap,  ARRAYNUMBER(selectedContainersPopupMenuMap))

// ===========================================================================
// GNEContainer::GNEContainerPopupMenu
// ===========================================================================

GNEContainer::GNEContainerPopupMenu::GNEContainerPopupMenu(GNEContainer* container, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *container),
    myContainer(container),
    myTransformToContainer(nullptr),
    myTransformToContainerFlow(nullptr) {
    // build header
    myContainer->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    myContainer->buildCenterPopupEntry(this);
    myContainer->buildPositionCopyEntry(this, app);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myContainer->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myContainer->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    myContainer->getNet()->getViewNet()->buildSelectionACPopupEntry(this, myContainer);
    myContainer->buildShowParamsPopupEntry(this);
    // add transform functions only in demand mode
    if (myContainer->getNet()->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu comands for all transformations
        myTransformToContainer = GUIDesigns::buildFXMenuCommand(transformOperation, "Container", GUIIconSubSys::getIcon(GUIIcon::CONTAINER), this, MID_GNE_CONTAINER_TRANSFORM);
        myTransformToContainerFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "ContainerFlow", GUIIconSubSys::getIcon(GUIIcon::CONTAINERFLOW), this, MID_GNE_CONTAINER_TRANSFORM);
        // check what menu command has to be disabled
        if (myContainer->getTagProperty().getTag() == SUMO_TAG_CONTAINER) {
            myTransformToContainer->disable();
        } else if (myContainer->getTagProperty().getTag() == SUMO_TAG_CONTAINERFLOW) {
            myTransformToContainerFlow->disable();
        }
    }
}


GNEContainer::GNEContainerPopupMenu::~GNEContainerPopupMenu() {}


long
GNEContainer::GNEContainerPopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    if (obj == myTransformToContainer) {
        GNERouteHandler::transformToContainer(myContainer);
    } else if (obj == myTransformToContainerFlow) {
        GNERouteHandler::transformToContainerFlow(myContainer);
    }
    return 1;
}


// ===========================================================================
// GNEContainer::GNESelectedContainersPopupMenu
// ===========================================================================

GNEContainer::GNESelectedContainersPopupMenu::GNESelectedContainersPopupMenu(GNEContainer* container, const std::vector<GNEContainer*>& selectedContainer, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *container),
    myContainerTag(container->getTagProperty().getTag()),
    mySelectedContainers(selectedContainer),
    myTransformToContainer(nullptr),
    myTransformToContainerFlow(nullptr) {
    // build header
    container->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    container->buildCenterPopupEntry(this);
    container->buildPositionCopyEntry(this, app);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + container->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + container->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    container->getNet()->getViewNet()->buildSelectionACPopupEntry(this, container);
    container->buildShowParamsPopupEntry(this);
    // add transform functions only in demand mode
    if (container->getNet()->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu comands for all transformations
        myTransformToContainer = GUIDesigns::buildFXMenuCommand(transformOperation, "Container", GUIIconSubSys::getIcon(GUIIcon::CONTAINER), this, MID_GNE_CONTAINER_TRANSFORM);
        myTransformToContainerFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "ContainerFlow", GUIIconSubSys::getIcon(GUIIcon::CONTAINERFLOW), this, MID_GNE_CONTAINER_TRANSFORM);
    }
}


GNEContainer::GNESelectedContainersPopupMenu::~GNESelectedContainersPopupMenu() {}


long
GNEContainer::GNESelectedContainersPopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    // iterate over all selected containers
    for (const auto& container : mySelectedContainers) {
        if ((obj == myTransformToContainer) &&
                (container->getTagProperty().getTag() == myContainerTag)) {
            GNERouteHandler::transformToContainer(container);
        } else if ((obj == myTransformToContainerFlow) &&
                   (container->getTagProperty().getTag() == myContainerTag)) {
            GNERouteHandler::transformToContainer(container);
        }
    }
    return 1;
}

// ===========================================================================
// member method definitions
// ===========================================================================

GNEContainer::GNEContainer(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_CONTAINER, tag, GUIIconSubSys::getIcon(GUIIcon::CONTAINER),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
    // set end and vehPerHours
    toggleAttribute(SUMO_ATTR_END, 1);
    toggleAttribute(SUMO_ATTR_CONTAINERSPERHOUR, 1);
}


GNEContainer::GNEContainer(SumoXMLTag tag, GNENet* net, GNEDemandElement* pType, const SUMOVehicleParameter& containerparameters) :
    GNEDemandElement(containerparameters.id, net, (tag == SUMO_TAG_CONTAINERFLOW) ? GLO_CONTAINERFLOW : GLO_CONTAINER, tag,
                     (tag == SUMO_TAG_CONTAINERFLOW) ? GUIIconSubSys::getIcon(GUIIcon::CONTAINERFLOW) : GUIIconSubSys::getIcon(GUIIcon::CONTAINER),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {pType}, {}),
SUMOVehicleParameter(containerparameters) {
    // set manually vtypeID (needed for saving)
    vtypeid = pType->getID();
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEContainer::~GNEContainer() {}


GNEMoveOperation*
GNEContainer::getMoveOperation() {
    // check first container plan
    if (getChildDemandElements().front()->getTagProperty().isStopContainer()) {
        return nullptr;
    } else {
        // get lane
        const GNELane* lane = getChildDemandElements().front()->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
        // declare departPos
        double posOverLane = 0;
        if (canParse<double>(getDepartPos())) {
            posOverLane = parse<double>(getDepartPos());
        }
        // return move operation
        return new GNEMoveOperation(this, lane, posOverLane, false);
    }
}


std::string
GNEContainer::getBegin() const {
    // obtain depart
    std::string departStr = depart < 0 ? "0.00" : time2string(depart);
    // we need to handle depart as a tuple of 20 numbers (format: 000000...00<departTime>)
    departStr.reserve(20 - departStr.size());
    // add 0s at the beginning of departStr until we have 20 numbers
    for (int i = (int)departStr.size(); i < 20; i++) {
        departStr.insert(departStr.begin(), '0');
    }
    return departStr;
}


void
GNEContainer::writeDemandElement(OutputDevice& device) const {
    // attribute VType musn't be written if is DEFAULT_PEDTYPE_ID
    if (getParentDemandElements().at(0)->getID() == DEFAULT_CONTAINERTYPE_ID) {
        // unset VType parameter
        parametersSet &= ~VEHPARS_VTYPE_SET;
        // write container attributes (VType will not be written)
        write(device, OptionsCont::getOptions(), myTagProperty.getXMLTag());
        // set VType parameter again
        parametersSet |= VEHPARS_VTYPE_SET;
    } else {
        // write container attributes, including VType
        write(device, OptionsCont::getOptions(), myTagProperty.getXMLTag(), getParentDemandElements().at(0)->getID());
    }
    // write specific flow attributes
    if (myTagProperty.getTag() == SUMO_TAG_CONTAINERFLOW) {
        // write routeFlow values depending if it was set
        if (isAttributeEnabled(SUMO_ATTR_END)) {
            device.writeAttr(SUMO_ATTR_END,  time2string(repetitionEnd));
        }
        if (isAttributeEnabled(SUMO_ATTR_NUMBER)) {
            device.writeAttr(SUMO_ATTR_NUMBER, repetitionNumber);
        }
        if (isAttributeEnabled(SUMO_ATTR_CONTAINERSPERHOUR)) {
            device.writeAttr(SUMO_ATTR_CONTAINERSPERHOUR, 3600. / STEPS2TIME(repetitionOffset));
        }
        if (isAttributeEnabled(SUMO_ATTR_PERIOD)) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(repetitionOffset));
        }
        if (isAttributeEnabled(GNE_ATTR_POISSON)) {
            device.writeAttr(SUMO_ATTR_PERIOD, "exp(" + time2string(repetitionOffset) + ")");
        }
        if (isAttributeEnabled(SUMO_ATTR_PROB)) {
            device.writeAttr(SUMO_ATTR_PROB, repetitionProbability);
        }
    }
    // write parameters
    writeParams(device);
    // write child demand elements associated to this container (Rides, Walks...)
    for (const auto& containerPlan : getChildDemandElements()) {
        containerPlan->writeDemandElement(device);
    }
    // close container tag
    device.closeTag();
}


GNEDemandElement::Problem
GNEContainer::isDemandElementValid() const {
    if (getChildDemandElements().size() == 0) {
        return Problem::NO_PLANS;
    } else {
        return Problem::OK;
    }
}


std::string
GNEContainer::getDemandElementProblem() const {
    if (getChildDemandElements().size() == 0) {
        return "Container needs at least one plan";
    } else {
        return "";
    }
}


void
GNEContainer::fixDemandElementProblem() {
    // nothing to fix
}


SUMOVehicleClass
GNEContainer::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEContainer::getColor() const {
    return color;
}


void
GNEContainer::updateGeometry() {
    // only update geometry of childrens
    for (const auto& demandElement : getChildDemandElements()) {
        demandElement->updateGeometry();
    }
}


Position
GNEContainer::getPositionInView() const {
    return getAttributePosition(SUMO_ATTR_DEPARTPOS);
}


GUIGLObjectPopupMenu*
GNEContainer::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // return a GNEContainerPopupMenu
    return new GNEContainerPopupMenu(this, app, parent);
}


std::string
GNEContainer::getParentName() const {
    return getParentDemandElements().front()->getID();
}


double
GNEContainer::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.containerSize.getExaggeration(s, this, 80);
}


Boundary
GNEContainer::getCenteringBoundary() const {
    Boundary containerBoundary;
    if (getChildDemandElements().size() > 0) {
        if (getChildDemandElements().front()->getTagProperty().isStopContainer()) {
            // use boundary of stop center
            return getChildDemandElements().front()->getCenteringBoundary();
        } else {
            containerBoundary.add(getPositionInView());
        }
    } else {
        containerBoundary = Boundary(-0.1, -0.1, 0.1, 0.1);
    }
    containerBoundary.grow(20);
    return containerBoundary;
}


void
GNEContainer::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEContainer::drawGL(const GUIVisualizationSettings& s) const {
    bool drawContainer = true;
    // check if container can be drawn
    if (color.alpha() == 0) {
        drawContainer = false;
    } else if (!myNet->getViewNet()->getNetworkViewOptions().showDemandElements()) {
        drawContainer = false;
    } else if (!myNet->getViewNet()->getDataViewOptions().showDemandElements()) {
        drawContainer = false;
    } else if (!myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this)) {
        drawContainer = false;
    } else if (getChildDemandElements().empty()) {
        drawContainer = false;
    }
    // continue if container can be drawn
    if (drawContainer) {
        // obtain exaggeration (and add the special containerExaggeration)
        const double exaggeration = getExaggeration(s) + s.detailSettings.personExaggeration;
        // obtain width and length
        const double length = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
        const double width = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_WIDTH);
        // obtain diameter around container (used to calculate distance bewteen cursor and container)
        const double distanceSquared = pow(exaggeration * std::max(length, width), 2);
        // obtain img file
        const std::string file = getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_IMGFILE);
        // obtain position
        const Position containerPosition = getAttributePosition(SUMO_ATTR_DEPARTPOS);
        // check if container can be drawn
        if (!(s.drawForPositionSelection && (containerPosition.distanceSquaredTo(myNet->getViewNet()->getPositionInformation()) > distanceSquared))) {
            // push GL ID
            GLHelper::pushName(getGlID());
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
            // translate and rotate
            glTranslated(containerPosition.x(), containerPosition.y(), 0);
            glRotated(90, 0, 0, 1);
            // set container color
            GLHelper::setColor(color);
            // set scale
            glScaled(exaggeration, exaggeration, 1);
            // draw container depending of detail level
            switch (s.containerQuality) {
                case 0:
                case 1:
                case 2:
                    drawAction_drawAsPoly();
                    break;
                case 3:
                default:
                    drawAction_drawAsImage(s);
                    break;
            }
            // pop matrix
            GLHelper::popMatrix();
            // draw line between junctions if container plan isn't valid
            for (const auto& containerPlan : getChildDemandElements()) {
                if (containerPlan->getTagProperty().isContainerPlan() && (containerPlan->getParentJunctions().size() > 0) && !myNet->getPathManager()->isPathValid(containerPlan)) {
                    drawJunctionLine(containerPlan);
                }
            }
            // pop name
            GLHelper::popName();
            // draw stack label
            if (myStackedLabelNumber > 0) {
                drawStackLabel("container", Position(containerPosition.x() - 2.5, containerPosition.y() - 0.8), -90, 1.3, 5, getExaggeration(s));
            }
            // draw flow label
            if (myTagProperty.isFlow()) {
                drawFlowLabel(Position(containerPosition.x() - 1, containerPosition.y() - 4.25), -90, 1.8, 2, getExaggeration(s));
            }
            // draw name
            drawName(containerPosition, s.scale, s.containerName, s.angle);
            if (s.personValue.show(this)) {
                Position containerValuePosition = containerPosition + Position(0, 0.6 * s.containerName.scaledSize(s.scale));
                const double value = getColorValue(s, s.containerColorer.getActive());
                GLHelper::drawTextSettings(s.personValue, toString(value), containerValuePosition, s.scale, s.angle, GLO_MAX - getType());
            }
            // check if mouse is over element
            mouseWithinGeometry(containerPosition, 0.5, 0.2, -2.5, 0, 0);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), exaggeration);
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                // draw using drawDottedSquaredShape
                GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::INSPECT, containerPosition, 0.5, 0.2, -2.5, 0, 0, exaggeration);
            }
            // front element contour
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                // draw using drawDottedSquaredShape
                GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::FRONT, containerPosition, 0.5, 0.2, -2.5, 0, 0, exaggeration);
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                // draw using drawDottedSquaredShape
                GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::REMOVE, containerPosition, 0.5, 0.2, -2.5, 0, 0, exaggeration);
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                // draw using drawDottedSquaredShape
                GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::SELECT, containerPosition, 0.5, 0.2, -2.5, 0, 0, exaggeration);
            }
        }
    }
}


void
GNEContainer::computePathElement() {
    // compute all container plan children (because aren't computed in "computeDemandElements()")
    for (const auto& demandElement : getChildDemandElements()) {
        demandElement->computePathElement();
    }
}


void
GNEContainer::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawPartialGL
}


void
GNEContainer::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawPartialGL
}


GNELane*
GNEContainer::getFirstPathLane() const {
    // use path lane of first container plan
    return getChildDemandElements().front()->getFirstPathLane();
}


GNELane*
GNEContainer::getLastPathLane() const {
    // use path lane of first container plan
    return getChildDemandElements().front()->getLastPathLane();
}


std::string
GNEContainer::getAttribute(SumoXMLAttr key) const {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_TYPE:
            return getParentDemandElements().at(0)->getID();
        case SUMO_ATTR_COLOR:
            if (wasSet(VEHPARS_COLOR_SET)) {
                return toString(color);
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_COLOR);
            }
        case SUMO_ATTR_DEPARTPOS:
            if (wasSet(VEHPARS_DEPARTPOS_SET)) {
                return getDepartPos();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTPOS);
            }
        // Specific of containers
        case SUMO_ATTR_DEPART:
            if (departProcedure == DepartDefinition::TRIGGERED) {
                return "triggered";
            } else if (departProcedure == DepartDefinition::CONTAINER_TRIGGERED) {
                return "containerTriggered";
            } else if (departProcedure == DepartDefinition::NOW) {
                return "now";
            } else if (departProcedure == DepartDefinition::SPLIT) {
                return "split";
            } else if (departProcedure == DepartDefinition::BEGIN) {
                return "begin";
            } else {
                return time2string(depart);
            }
        // Specific of containerFlows
        case SUMO_ATTR_BEGIN:
            return time2string(depart);
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_CONTAINERSPERHOUR:
            return toString(3600 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
            return time2string(repetitionOffset);
        case SUMO_ATTR_PROB:
            return toString(repetitionProbability);
        case SUMO_ATTR_NUMBER:
            return toString(repetitionNumber);
        //
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEContainer::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS:
            return STEPS2TIME(depart);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


Position
GNEContainer::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS: {
            // get container plan
            const GNEDemandElement* containerPlan = getChildDemandElements().front();
            // first check if first container plan is a stop
            if (containerPlan->getTagProperty().isStopContainer()) {
                return containerPlan->getPositionInView();
            } else {
                // declare lane lane
                const GNELane* lane = containerPlan->getParentEdges().front()->getLaneByAllowedVClass(SVC_IGNORING);
                // get position over lane shape
                if (departPos <= 0) {
                    return lane->getLaneShape().front();
                } else if (departPos >= lane->getLaneShape().length2D()) {
                    return lane->getLaneShape().back();
                } else {
                    return lane->getLaneShape().positionAtOffset2D(departPos);
                }
            }
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a Position attribute of type '" + toString(key) + "'");
    }
}


void
GNEContainer::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTPOS:
        // Specific of containers
        case SUMO_ATTR_DEPART:
        // Specific of containerFlows
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
        //
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEContainer::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            // Containers and containerflows share namespace
            if (SUMOXMLDefinitions::isValidVehicleID(value) &&
                    (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_CONTAINER, value, false) == nullptr) &&
                    (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_CONTAINERFLOW, value, false) == nullptr)) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Specific of containers
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_CONTAINER), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Specific of containerflows
        case SUMO_ATTR_END:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_CONTAINERSPERHOUR:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PROB:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_NUMBER:
            if (canParse<int>(value)) {
                return (parse<int>(value) >= 0);
            } else {
                return false;
            }
        //
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEContainer::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
            undoList->add(new GNEChange_EnableAttribute(this, key, true, parametersSet), true);
            return;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEContainer::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
            undoList->add(new GNEChange_EnableAttribute(this, key, false, parametersSet), true);
            return;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEContainer::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_END:
            return (parametersSet & VEHPARS_END_SET) != 0;
        case SUMO_ATTR_NUMBER:
            return (parametersSet & VEHPARS_NUMBER_SET) != 0;
        case SUMO_ATTR_CONTAINERSPERHOUR:
            return (parametersSet & VEHPARS_VPH_SET) != 0;
        case SUMO_ATTR_PERIOD:
            return (parametersSet & VEHPARS_PERIOD_SET) != 0;
        case GNE_ATTR_POISSON:
            return (parametersSet & VEHPARS_POISSON_SET) != 0;
        case SUMO_ATTR_PROB:
            return (parametersSet & VEHPARS_PROB_SET) != 0;
        default:
            return true;
    }
}


std::string
GNEContainer::getPopUpID() const {
    return getTagStr();
}


std::string
GNEContainer::getHierarchyName() const {
    // special case for Trips and flow
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check if we're inspecting a Edge
        if (myNet->getViewNet()->getInspectedAttributeCarriers().front() &&
                myNet->getViewNet()->getInspectedAttributeCarriers().front()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
            // check if edge correspond to a "from", "to" or "via" edge
            if (myNet->getViewNet()->isAttributeCarrierInspected(getParentEdges().front())) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (from)";
            } else if (myNet->getViewNet()->isAttributeCarrierInspected(getParentEdges().front())) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (to)";
            } else {
                // iterate over via
                for (const auto& i : via) {
                    if (i == myNet->getViewNet()->getInspectedAttributeCarriers().front()->getID()) {
                        return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (via)";
                    }
                }
            }
        }
    }
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID);
}


const Parameterised::Map&
GNEContainer::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEContainer::drawAction_drawAsPoly() const {
    // obtain width and length
    const double length = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
    const double width = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_WIDTH);
    // draw pedestrian shape
    glScaled(length * 0.2, width * 0.2, 1);
    glBegin(GL_QUADS);
    glVertex2d(0, 0.5);
    glVertex2d(0, -0.5);
    glVertex2d(-1, -0.5);
    glVertex2d(-1, 0.5);
    glEnd();
    GLHelper::setColor(GLHelper::getColor().changedBrightness(-30));
    glTranslated(0, 0, .045);
    glBegin(GL_QUADS);
    glVertex2d(-0.1, 0.4);
    glVertex2d(-0.1, -0.4);
    glVertex2d(-0.9, -0.4);
    glVertex2d(-0.9, 0.4);
    glEnd();
}


void
GNEContainer::drawAction_drawAsImage(const GUIVisualizationSettings& s) const {
    const std::string& file = getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_IMGFILE);
    // obtain width and length
    const double length = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
    const double width = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_WIDTH);
    if (file != "") {
        // @todo invent an option for controlling whether images should be rotated or not
        //if (getVehicleType().getGuiShape() == SVS_CONTAINER) {
        //    glRotated(RAD2DEG(getAngle() + M_PI / 2.), 0, 0, 1);
        //}
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const double exaggeration = s.personSize.getExaggeration(s, this);
            const double halfLength = length / 2.0 * exaggeration;
            const double halfWidth = width / 2.0 * exaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, -halfLength, halfWidth, halfLength);
        }
    } else {
        // fallback if no image is defined
        drawAction_drawAsPoly();
    }
}

// ===========================================================================
// private
// ===========================================================================

GNEContainer::containerPlanSegment::containerPlanSegment(GNEDemandElement* _containerPlan) :
    containerPlan(_containerPlan),
    edge(nullptr),
    arrivalPos(-1) {
}


GNEContainer::containerPlanSegment::containerPlanSegment() :
    containerPlan(nullptr),
    edge(nullptr),
    arrivalPos(-1) {
}


void
GNEContainer::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            // update id
            id = value;
            // Change IDs of all container plans children
            for (const auto& containerPlans : getChildDemandElements()) {
                containerPlans->setMicrosimID(getID());
            }
            break;
        case SUMO_ATTR_TYPE:
            if (getID().size() > 0) {
                replaceDemandElementParent(SUMO_TAG_VTYPE, value, 0);
                // set manually vtypeID (needed for saving)
                vtypeid = value;
            }
            break;
        case SUMO_ATTR_COLOR:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                color = parse<RGBColor>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_COLOR_SET;
            } else {
                // set default value
                color = parse<RGBColor>(myTagProperty.getDefaultValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_COLOR_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOS_SET;
            } else {
                // set default value
                parseDepartPos(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOS_SET;
            }
            // compute container
            updateGeometry();
            break;
        // Specific of containers
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN: {
            parseDepart(value, toString(SUMO_TAG_CONTAINER), id, depart, departProcedure, error);
            break;
        }
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_CONTAINERSPERHOUR:
            repetitionOffset = TIME2STEPS(3600 / parse<double>(value));
            break;
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
            repetitionOffset = string2time(value);
            break;
        case SUMO_ATTR_PROB:
            repetitionProbability = parse<double>(value);
            break;
        case SUMO_ATTR_NUMBER:
            repetitionNumber = parse<int>(value);
            break;
        //
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEContainer::toggleAttribute(SumoXMLAttr key, const bool value) {
    // set flow parameters
    setFlowParameters(this, key, value);
}


void GNEContainer::setMoveShape(const GNEMoveResult& moveResult) {
    // change departPos
    departPosProcedure = DepartPosDefinition::GIVEN;
    departPos = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNEContainer::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(GUIIcon::CONTAINER, "departPos of " + getTagStr());
    // now set departPos
    setAttribute(SUMO_ATTR_DEPARTPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
