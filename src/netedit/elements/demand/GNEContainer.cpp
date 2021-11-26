/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
// Representation of containers in NETEDIT
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
    myContainer->buildPositionCopyEntry(this, false);
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
        myTransformToContainerFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "Container (embedded route)", GUIIconSubSys::getIcon(GUIIcon::CONTAINERFLOW), this, MID_GNE_CONTAINER_TRANSFORM);
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
    container->buildPositionCopyEntry(this, false);
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
    for (const auto& i : mySelectedContainers) {
        if ((obj == myTransformToContainer) &&
                (i->getTagProperty().getTag() == myContainerTag)) {
            GNERouteHandler::transformToContainer(i);
        } else if ((obj == myTransformToContainerFlow) &&
                   (i->getTagProperty().getTag() == myContainerTag)) {
            GNERouteHandler::transformToContainer(i);
        }
    }
    return 1;
}

// ===========================================================================
// member method definitions
// ===========================================================================

GNEContainer::GNEContainer(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_CONTAINER, tag, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {}, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNEContainer::GNEContainer(SumoXMLTag tag, GNENet* net, GNEDemandElement* pType, const SUMOVehicleParameter& containerparameters) :
    GNEDemandElement(containerparameters.id, net, (tag == SUMO_TAG_CONTAINERFLOW) ? GLO_CONTAINERFLOW : GLO_CONTAINER, tag, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {}, {}, {}, {}, {}, {pType}, {}),
    SUMOVehicleParameter(containerparameters) {
    // set manually vtypeID (needed for saving)
    vtypeid = pType->getID();
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
    // obtain depart depending if is a Container, trip or routeFlow
    std::string departStr;
    if (myTagProperty.getTag() == SUMO_TAG_CONTAINERFLOW) {
        departStr = toString(depart);
    } else {
        departStr = getDepart();
    }
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
    if (getParentDemandElements().at(0)->getID() == DEFAULT_PEDTYPE_ID) {
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
        if (isAttributeEnabled(SUMO_ATTR_PROB)) {
            device.writeAttr(SUMO_ATTR_PROB, repetitionProbability);
        }
    }
    // write parameters
    writeParams(device);
    // write child demand elements associated to this container (Rides, Walks...)
    for (const auto& i : getChildDemandElements()) {
        i->writeDemandElement(device);
    }
    // close container tag
    device.closeTag();
}


bool
GNEContainer::isDemandElementValid() const {
    // a single container is always valid
    return true;
}


std::string
GNEContainer::getDemandElementProblem() const {
    // A single container cannot habe problem (but their children)
    return "";
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
    if (!myNet->getViewNet()->getNetworkViewOptions().showDemandElements()) {
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
            setColor(s);
            // set scale
            glScaled(exaggeration, exaggeration, 1);
            // draw container depending of detail level
            if (s.drawDetail(s.detailSettings.personShapes, exaggeration)) {
                GUIBasePersonHelper::drawAction_drawAsImage(0, length, width, file, SVS_PEDESTRIAN, exaggeration);
            } else if (s.drawDetail(s.detailSettings.personCircles, exaggeration)) {
                GUIBasePersonHelper::drawAction_drawAsCircle(length, width, s.scale * exaggeration);
            } else if (s.drawDetail(s.detailSettings.personTriangles, exaggeration)) {
                GUIBasePersonHelper::drawAction_drawAsTriangle(0, length, width);
            }
            // pop matrix
            GLHelper::popMatrix();
            // pop name
            GLHelper::popName();
            // draw name
            drawName(containerPosition, s.scale, s.containerName, s.angle);
            if (s.personValue.show(this)) {
                Position containerValuePosition = containerPosition + Position(0, 0.6 * s.containerName.scaledSize(s.scale));
                const double value = getColorValue(s, s.containerColorer.getActive());
                GLHelper::drawTextSettings(s.personValue, toString(value), containerValuePosition, s.scale, s.angle, GLO_MAX - getType());
            }
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), exaggeration);
            // check if dotted contours has to be drawn
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                // draw using drawDottedSquaredShape
                GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::INSPECT, s, containerPosition, 0.5, 0.5, 0, 0, 0, exaggeration);
            }
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                // draw using drawDottedSquaredShape
                GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::FRONT, s, containerPosition, 0.5, 0.5, 0, 0, 0, exaggeration);
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
            return getID();
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
            return toString(depart);
        // Specific of containerFlows
        case SUMO_ATTR_BEGIN:
            return time2string(depart);
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_CONTAINERSPERHOUR:
            return toString(3600 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PERIOD:
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
            if (departPosProcedure == DepartPosDefinition::GIVEN) {
                return departPos;
            } else {
                return 0;
            }
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
                GNELane* lane = nullptr;
                // update lane
                if (containerPlan->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE) {
                    lane = containerPlan->getParentDemandElements().at(1)->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
                } else {
                    lane = containerPlan->getParentEdges().front()->getLaneByAllowedVClass(getVClass());
                }
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
        case SUMO_ATTR_DEPART: {
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        }
        // Specific of containerflows
        case SUMO_ATTR_BEGIN:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
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
        case SUMO_ATTR_PROB:
            undoList->add(new GNEChange_EnableAttribute(this, key, true, parametersSet), true);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEContainer::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // nothing to disable
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


const std::map<std::string, std::string>&
GNEContainer::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEContainer::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.containerColorer;
    if (!setFunctionalColor(c.getActive())) {
        GLHelper::setColor(c.getScheme().getColor(getColorValue(s, c.getActive())));
    }
}


bool
GNEContainer::setFunctionalColor(int /* activeScheme */) const {
    /*
    switch (activeScheme) {
        case 0: {
            if (getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(getParameter().color);
                return true;
            }
            if (getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(getVehicleType().getColor());
                return true;
            }
            return false;
        }
        case 2: {
            if (getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(getParameter().color);
                return true;
            }
            return false;
        }
        case 3: {
            if (getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(getVehicleType().getColor());
                return true;
            }
            return false;
        }
        case 8: { // color by angle
            double hue = GeomHelper::naviDegree(getAngle());
            GLHelper::setColor(RGBColor::fromHSV(hue, 1., 1.));
            return true;
        }
        case 9: { // color randomly (by pointer)
            const double hue = (long)this % 360; // [0-360]
            const double sat = (((long)this / 360) % 67) / 100.0 + 0.33; // [0.33-1]
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        default:
            return false;
    }
    */
    return false;
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
            // Change IDs of all container plans children
            for (const auto& containerPlans : getChildDemandElements()) {
                containerPlans->setMicrosimID(getID());
            }
            break;
        case SUMO_ATTR_TYPE:
            replaceDemandElementParent(SUMO_TAG_PTYPE, value, 0);
            // set manually vtypeID (needed for saving)
            vtypeid = value;
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
        case SUMO_ATTR_DEPART: {
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            break;
        }
        // Specific of containerFlows
        case SUMO_ATTR_BEGIN: {
            depart = string2time(value);
            break;
        }
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_CONTAINERSPERHOUR:
            repetitionOffset = TIME2STEPS(3600 / parse<double>(value));
            break;
        case SUMO_ATTR_PERIOD:
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
GNEContainer::toogleAttribute(SumoXMLAttr key, const bool value, const int previousParameters) {
    if (value) {
        GNERouteHandler::setFlowParameters(key, parametersSet);
    } else {
        parametersSet = previousParameters;
    }
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
