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
/// @file    GNEContainer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// Representation of containers in netedit
/****************************************************************************/

#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementPlanParent.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/xml/NamespaceIDs.h>

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
    GUIGLObjectPopupMenu(app, parent, container),
    myContainer(container),
    myTransformToContainer(nullptr),
    myTransformToContainerFlow(nullptr) {
    // build common options
    container->buildPopUpMenuCommonOptions(this, app, container->myNet->getViewNet(), container->getTagProperty()->getTag(), container->isAttributeCarrierSelected());
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
        if (myContainer->getTagProperty()->getTag() == SUMO_TAG_CONTAINER) {
            myTransformToContainer->disable();
        } else if (myContainer->getTagProperty()->getTag() == SUMO_TAG_CONTAINERFLOW) {
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
    GUIGLObjectPopupMenu(app, parent, container),
    myContainerTag(container->getTagProperty()->getTag()),
    mySelectedContainers(selectedContainer),
    myTransformToContainer(nullptr),
    myTransformToContainerFlow(nullptr) {
    // build common options
    container->buildPopUpMenuCommonOptions(this, app, container->myNet->getViewNet(), container->getTagProperty()->getTag(), container->isAttributeCarrierSelected());
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
                (container->getTagProperty()->getTag() == myContainerTag)) {
            GNERouteHandler::transformToContainer(container);
        } else if ((obj == myTransformToContainerFlow) &&
                   (container->getTagProperty()->getTag() == myContainerTag)) {
            GNERouteHandler::transformToContainer(container);
        }
    }
    return 1;
}

// ===========================================================================
// member method definitions
// ===========================================================================

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GNEContainer::GNEContainer(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, "", tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementFlow(this),
    myMoveElementPlanParent(new GNEMoveElementPlanParent(this, departPos, departPosProcedure)) {
    // set end and container per hours as default flow values
    toggleAttribute(SUMO_ATTR_END, 1);
    toggleAttribute(SUMO_ATTR_CONTAINERSPERHOUR, 1);
}


GNEContainer::GNEContainer(SumoXMLTag tag, GNENet* net, const std::string& filename, GNEDemandElement* pType, const SUMOVehicleParameter& containerparameters) :
    GNEDemandElement(containerparameters.id, net, filename, tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementFlow(this, containerparameters),
    myMoveElementPlanParent(new GNEMoveElementPlanParent(this, departPos, departPosProcedure)) {
    // set parents
    setParent<GNEDemandElement*>(pType);
    // set manually vtypeID (needed for saving)
    vtypeid = pType->getID();
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

GNEContainer::~GNEContainer() {}


GNEMoveElement*
GNEContainer::getMoveElement() const {
    return myMoveElementPlanParent;
}


Parameterised*
GNEContainer::getParameters() {
    return this;
}


const Parameterised*
GNEContainer::getParameters() const {
    return this;
}


void
GNEContainer::writeDemandElement(OutputDevice& device) const {
    // attribute VType musn't be written if is DEFAULT_PEDTYPE_ID
    if (getTypeParent()->getID() == DEFAULT_CONTAINERTYPE_ID) {
        // unset VType parameter
        parametersSet &= ~VEHPARS_VTYPE_SET;
        // write container attributes (VType will not be written)
        write(device, OptionsCont::getOptions(), myTagProperty->getXMLTag());
        // set VType parameter again
        parametersSet |= VEHPARS_VTYPE_SET;
    } else {
        // write container attributes, including VType
        write(device, OptionsCont::getOptions(), myTagProperty->getXMLTag(), getTypeParent()->getID());
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
        if (getChildDemandElements().front()->getTagProperty()->isPlanStopContainer()) {
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
GNEContainer::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/,
                                const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
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
        const double exaggeration = getExaggeration(s) + 10;
        // get detail level
        const auto d = s.getDetailLevel(exaggeration);
        // obtain position
        const Position containerPosition = getAttributePosition(SUMO_ATTR_DEPARTPOS);
        if (containerPosition == Position::INVALID) {
            return;
        }
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawContainer(d, isAttributeCarrierSelected())) {
            // obtain img file
            const std::string file = getTypeParent()->getAttribute(SUMO_ATTR_IMGFILE);
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            drawInLayer(getType());
            // translate and rotate
            glTranslated(containerPosition.x(), containerPosition.y(), 0);
            glRotated(90, 0, 0, 1);
            // set container color
            GLHelper::setColor(getDrawingColor(s));
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
                if (containerPlan->getTagProperty()->isPlanContainer() && (containerPlan->getParentJunctions().size() > 0) &&
                        !myNet->getDemandPathManager()->isPathValid(containerPlan)) {
                    drawJunctionLine(containerPlan);
                }
            }
            // draw stack label
            if (myStackedLabelNumber > 0) {
                drawStackLabel(myStackedLabelNumber, "container", Position(containerPosition.x() - 2.5, containerPosition.y() - 0.8), -90, 1.3, 5, getExaggeration(s));
            } else if (getChildDemandElements().front()->getTagProperty()->getTag() == GNE_TAG_STOPCONTAINER_CONTAINERSTOP) {
                // declare counter for stacked containers over stops
                int stackedCounter = 0;
                // get stoppingPlace
                const auto stoppingPlace = getChildDemandElements().front()->getParentAdditionals().front();
                // get stacked containers
                for (const auto& stopContainer : stoppingPlace->getChildDemandElements()) {
                    if (stopContainer->getTagProperty()->getTag() == GNE_TAG_STOPCONTAINER_CONTAINERSTOP) {
                        // get container parent
                        const auto containerParent = stopContainer->getParentDemandElements().front();
                        // check if the stop if the first container plan parent
                        if (stopContainer->getPreviousChildDemandElement(containerParent) == nullptr) {
                            stackedCounter++;
                        }
                    }
                }
                // if we have more than two stacked elements, draw label
                if (stackedCounter > 1) {
                    drawStackLabel(stackedCounter, "container", Position(containerPosition.x() - 2.5, containerPosition.y() - 0.8), -90, 1.3, 5, getExaggeration(s));
                }
            }
            // draw flow label
            if (myTagProperty->isFlow()) {
                drawFlowLabel(Position(containerPosition.x() - 1, containerPosition.y() - 4.25), -90, 1.8, 2, getExaggeration(s));
            }
            // draw name
            drawName(containerPosition, s.scale, s.containerName, s.angle);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), exaggeration);
            // draw dotted contour
            myContainerContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour
        myContainerContour.calculateContourRectangleShape(s, d, this, containerPosition, 0.5, 0.18, getType(), -5.5, 0, 0, exaggeration, nullptr);
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
GNEContainer::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawJunctionPartialGL
}


void
GNEContainer::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawJunctionPartialGL
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
            return getTypeParent()->getID();
        case SUMO_ATTR_COLOR:
            if (wasSet(VEHPARS_COLOR_SET)) {
                return toString(color);
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_COLOR);
            }
        case SUMO_ATTR_DEPARTPOS:
            if (wasSet(VEHPARS_DEPARTPOS_SET)) {
                return getDepartPos();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_DEPARTPOS);
            }
        default:
            return getFlowAttribute(this, key);
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
            return getFlowAttributeDouble(key);
    }
}


Position
GNEContainer::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS: {
            // first check number of child demand elements
            if (getChildDemandElements().empty()) {
                return Position();
            }
            // get container plan
            const GNEDemandElement* containerPlan = getChildDemandElements().front();
            // first check if first container plan is a stop
            if (containerPlan->getTagProperty()->isPlanStopContainer()) {
                // stop center
                return containerPlan->getPositionInView();
            } else if (containerPlan->getTagProperty()->planFromTAZ()) {
                // TAZ
                if (containerPlan->getParentAdditionals().front()->getAttribute(SUMO_ATTR_CENTER).empty()) {
                    return containerPlan->getParentAdditionals().front()->getAttributePosition(GNE_ATTR_TAZ_CENTROID);
                } else {
                    return containerPlan->getParentAdditionals().front()->getAttributePosition(SUMO_ATTR_CENTER);
                }
            } else if (containerPlan->getTagProperty()->planFromJunction()) {
                // juncrtion
                return containerPlan->getParentJunctions().front()->getPositionInView();
            } else {
                return containerPlan->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS);
            }
        }
        default:
            return getCommonAttributePosition(key);
    }
}


void
GNEContainer::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTPOS:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setFlowAttribute(this, key, value, undoList);
            break;
    }
}


bool
GNEContainer::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(NamespaceIDs::containers, value);
        case SUMO_ATTR_TYPE:
            return (myNet->getAttributeCarriers()->retrieveDemandElements(NamespaceIDs::types, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, myTagProperty->getTagStr(), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        default:
            return isValidFlowAttribute(this, key, value);
    }
}


void
GNEContainer::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    enableFlowAttribute(this, key, undoList);
}


void
GNEContainer::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    disableFlowAttribute(this, key, undoList);
}


bool
GNEContainer::isAttributeEnabled(SumoXMLAttr key) const {
    return isFlowAttributeEnabled(key);
}


std::string
GNEContainer::getPopUpID() const {
    return getTagStr();
}


std::string
GNEContainer::getHierarchyName() const {
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
    // special case for Trips and flow
    if ((myTagProperty->getTag() == SUMO_TAG_TRIP) || (myTagProperty->getTag() == SUMO_TAG_FLOW)) {
        // check if we're inspecting an Edge
        if (inspectedElements.getFirstAC() && (inspectedElements.getFirstAC()->getTagProperty()->getTag() == SUMO_TAG_EDGE)) {
            // check if edge correspond to a "from", "to" or "via" edge
            if (inspectedElements.isACInspected(getParentEdges().front())) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (from)";
            } else if (inspectedElements.isACInspected(getParentEdges().front())) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (to)";
            } else {
                // iterate over via
                for (const auto& viaEdgeID : via) {
                    if (viaEdgeID == inspectedElements.getFirstAC()->getID()) {
                        return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (via)";
                    }
                }
            }
        }
    }
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID);
}

// ===========================================================================
// protected
// ===========================================================================

RGBColor
GNEContainer::getDrawingColor(const GUIVisualizationSettings& s) const {
    // change color
    if (drawUsingSelectColor()) {
        return s.colorSettings.selectedContainerColor;
    } else {
        return getColorByScheme(s.containerColorer, this);
    }
}


void
GNEContainer::drawAction_drawAsPoly() const {
    // obtain width and length
    const double length = getTypeParent()->getAttributeDouble(SUMO_ATTR_LENGTH);
    const double width = getTypeParent()->getAttributeDouble(SUMO_ATTR_WIDTH);
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
    const std::string& file = getTypeParent()->getAttribute(SUMO_ATTR_IMGFILE);
    // obtain width and length
    const double length = getTypeParent()->getAttributeDouble(SUMO_ATTR_LENGTH);
    const double width = getTypeParent()->getAttributeDouble(SUMO_ATTR_WIDTH);
    if (file != "") {
        // @todo invent an option for controlling whether images should be rotated or not
        //if (getVehicleType().getGuiShape() == SVS_CONTAINER) {
        //    glRotated(RAD2DEG(getAngle() + M_PI / 2.), 0, 0, 1);
        //}
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const double exaggeration = s.containerSize.getExaggeration(s, this);
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
            setDemandElementID(value);
            // update id
            id = value;
            break;
        case SUMO_ATTR_TYPE:
            if (getID().size() > 0) {
                replaceDemandElementParent(SUMO_TAG_VTYPE, value, 0);
                // set manually vtypeID (needed for saving)
                vtypeid = value;
            }
            break;
        case SUMO_ATTR_COLOR:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                color = parse<RGBColor>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_COLOR_SET;
            } else {
                // set default value
                color = parse<RGBColor>(myTagProperty->getDefaultStringValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_COLOR_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseDepartPos(value, myTagProperty->getTagStr(), id, departPos, departPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOS_SET;
            } else {
                // set default value
                parseDepartPos(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, departPos, departPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOS_SET;
            }
            // compute container
            updateGeometry();
            break;
        default:
            setFlowAttribute(this, key, value);
            break;
    }
}

void
GNEContainer::toggleAttribute(SumoXMLAttr key, const bool value) {
    // toggle flow attributes
    toggleFlowAttribute(key, value);
}

/****************************************************************************/
