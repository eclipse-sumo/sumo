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
/// @file    GNEPerson.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// Representation of persons in netedit
/****************************************************************************/
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIBasePersonHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEPerson.h"
#include "GNERouteHandler.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEPerson::GNEPersonPopupMenu) personPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PERSON_TRANSFORM,   GNEPerson::GNEPersonPopupMenu::onCmdTransform),
};

FXDEFMAP(GNEPerson::GNESelectedPersonsPopupMenu) selectedPersonsPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PERSON_TRANSFORM,   GNEPerson::GNESelectedPersonsPopupMenu::onCmdTransform),
};

// Object implementation
FXIMPLEMENT(GNEPerson::GNEPersonPopupMenu,          GUIGLObjectPopupMenu, personPopupMenuMap,           ARRAYNUMBER(personPopupMenuMap))
FXIMPLEMENT(GNEPerson::GNESelectedPersonsPopupMenu, GUIGLObjectPopupMenu, selectedPersonsPopupMenuMap,  ARRAYNUMBER(selectedPersonsPopupMenuMap))

// ===========================================================================
// GNEPerson::GNEPersonPopupMenu
// ===========================================================================

GNEPerson::GNEPersonPopupMenu::GNEPersonPopupMenu(GNEPerson* person, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *person),
    myPerson(person),
    myTransformToPerson(nullptr),
    myTransformToPersonFlow(nullptr) {
    // build header
    myPerson->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    myPerson->buildCenterPopupEntry(this);
    myPerson->buildPositionCopyEntry(this, app);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myPerson->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myPerson->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    myPerson->getNet()->getViewNet()->buildSelectionACPopupEntry(this, myPerson);
    myPerson->buildShowParamsPopupEntry(this);
    // add transform functions only in demand mode
    if (myPerson->getNet()->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu comands for all transformations
        myTransformToPerson = GUIDesigns::buildFXMenuCommand(transformOperation, "Person", GUIIconSubSys::getIcon(GUIIcon::PERSON), this, MID_GNE_PERSON_TRANSFORM);
        myTransformToPersonFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "PersonFlow", GUIIconSubSys::getIcon(GUIIcon::PERSONFLOW), this, MID_GNE_PERSON_TRANSFORM);
        // check what menu command has to be disabled
        if (myPerson->getTagProperty().getTag() == SUMO_TAG_PERSON) {
            myTransformToPerson->disable();
        } else if (myPerson->getTagProperty().getTag() == SUMO_TAG_PERSONFLOW) {
            myTransformToPersonFlow->disable();
        }
    }
}


GNEPerson::GNEPersonPopupMenu::~GNEPersonPopupMenu() {}


long
GNEPerson::GNEPersonPopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    if (obj == myTransformToPerson) {
        GNERouteHandler::transformToPerson(myPerson);
    } else if (obj == myTransformToPersonFlow) {
        GNERouteHandler::transformToPersonFlow(myPerson);
    }
    return 1;
}


// ===========================================================================
// GNEPerson::GNESelectedPersonsPopupMenu
// ===========================================================================

GNEPerson::GNESelectedPersonsPopupMenu::GNESelectedPersonsPopupMenu(GNEPerson* person, const std::vector<GNEPerson*>& selectedPerson, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *person),
    myPersonTag(person->getTagProperty().getTag()),
    mySelectedPersons(selectedPerson),
    myTransformToPerson(nullptr),
    myTransformToPersonFlow(nullptr) {
    // build header
    person->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    person->buildCenterPopupEntry(this);
    person->buildPositionCopyEntry(this, app);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + person->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + person->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    person->getNet()->getViewNet()->buildSelectionACPopupEntry(this, person);
    person->buildShowParamsPopupEntry(this);
    // add transform functions only in demand mode
    if (person->getNet()->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu comands for all transformations
        myTransformToPerson = GUIDesigns::buildFXMenuCommand(transformOperation, "Person", GUIIconSubSys::getIcon(GUIIcon::PERSON), this, MID_GNE_PERSON_TRANSFORM);
        myTransformToPersonFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "PersonFlow", GUIIconSubSys::getIcon(GUIIcon::PERSONFLOW), this, MID_GNE_PERSON_TRANSFORM);
    }
}


GNEPerson::GNESelectedPersonsPopupMenu::~GNESelectedPersonsPopupMenu() {}


long
GNEPerson::GNESelectedPersonsPopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    // iterate over all selected persons
    for (const auto& i : mySelectedPersons) {
        if ((obj == myTransformToPerson) &&
                (i->getTagProperty().getTag() == myPersonTag)) {
            GNERouteHandler::transformToPerson(i);
        } else if ((obj == myTransformToPersonFlow) &&
                   (i->getTagProperty().getTag() == myPersonTag)) {
            GNERouteHandler::transformToPerson(i);
        }
    }
    return 1;
}

// ===========================================================================
// member method definitions
// ===========================================================================

GNEPerson::GNEPerson(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_PERSON, tag, GUIIconSubSys::getIcon(GUIIcon::PERSON),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
GNEDemandElementFlow(this) {
    // reset default values
    resetDefaultValues();
    // enable set and persons per hour as default flow values
    toggleAttribute(SUMO_ATTR_END, true);
    toggleAttribute(SUMO_ATTR_PERSONSPERHOUR, true);
}


GNEPerson::GNEPerson(SumoXMLTag tag, GNENet* net, GNEDemandElement* pType, const SUMOVehicleParameter& personparameters) :
    GNEDemandElement(personparameters.id, net, (tag == SUMO_TAG_PERSONFLOW) ? GLO_PERSONFLOW : GLO_PERSON, tag,
                     (tag == SUMO_TAG_PERSONFLOW) ? GUIIconSubSys::getIcon(GUIIcon::PERSONFLOW) : GUIIconSubSys::getIcon(GUIIcon::PERSON),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {pType}, {}),
GNEDemandElementFlow(this, personparameters) {
    // set manually vtypeID (needed for saving)
    vtypeid = pType->getID();
}


GNEPerson::~GNEPerson() {}


GNEMoveOperation*
GNEPerson::getMoveOperation() {
    // check first person plan
    if (getChildDemandElements().front()->getTagProperty().isPlanStopPerson()) {
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


void
GNEPerson::writeDemandElement(OutputDevice& device) const {
    // attribute VType musn't be written if is DEFAULT_PEDTYPE_ID
    if (getTypeParent()->getID() == DEFAULT_PEDTYPE_ID) {
        // unset VType parameter
        parametersSet &= ~VEHPARS_VTYPE_SET;
        // write person attributes (VType will not be written)
        write(device, OptionsCont::getOptions(), myTagProperty.getXMLTag());
        // set VType parameter again
        parametersSet |= VEHPARS_VTYPE_SET;
    } else {
        // write person attributes, including VType
        write(device, OptionsCont::getOptions(), myTagProperty.getXMLTag(), getTypeParent()->getID());
    }
    // write flow attributes
    writeFlowAttributes(this, device);
    // write parameters
    writeParams(device);
    // write child demand elements associated to this person (Rides, Walks...)
    for (const auto& i : getChildDemandElements()) {
        i->writeDemandElement(device);
    }
    // close person tag
    device.closeTag();
}


GNEDemandElement::Problem
GNEPerson::isDemandElementValid() const {
    if (getChildDemandElements().size() == 0) {
        return Problem::NO_PLANS;
    } else {
        return Problem::OK;
    }
}


std::string
GNEPerson::getDemandElementProblem() const {
    if (getChildDemandElements().size() == 0) {
        return "Person needs at least one plan";
    } else {
        return "";
    }
}


void
GNEPerson::fixDemandElementProblem() {
    // nothing to fix
}


SUMOVehicleClass
GNEPerson::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEPerson::getColor() const {
    return color;
}


void
GNEPerson::updateGeometry() {
    // only update geometry of childrens
    for (const auto& demandElement : getChildDemandElements()) {
        demandElement->updateGeometry();
    }
}


Position
GNEPerson::getPositionInView() const {
    return getAttributePosition(SUMO_ATTR_DEPARTPOS);
}


GUIGLObjectPopupMenu*
GNEPerson::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // return a GNEPersonPopupMenu
    return new GNEPersonPopupMenu(this, app, parent);
}


std::string
GNEPerson::getParentName() const {
    return getParentDemandElements().front()->getID();
}


double
GNEPerson::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.personSize.getExaggeration(s, this, 80);
}


Boundary
GNEPerson::getCenteringBoundary() const {
    Boundary personBoundary;
    if (getChildDemandElements().size() > 0) {
        if (getChildDemandElements().front()->getTagProperty().isPlanStopPerson()) {
            // use boundary of stop center
            return getChildDemandElements().front()->getCenteringBoundary();
        } else {
            personBoundary.add(getPositionInView());
        }
    } else {
        personBoundary = Boundary(-0.1, -0.1, 0.1, 0.1);
    }
    personBoundary.grow(20);
    return personBoundary;
}


void
GNEPerson::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEPerson::drawGL(const GUIVisualizationSettings& s) const {
    bool drawPerson = true;
    const auto personColor = setColor(s);
    // check if person can be drawn
    if (personColor.alpha() == 0) {
        drawPerson = false;
    } else if (!myNet->getViewNet()->getNetworkViewOptions().showDemandElements()) {
        drawPerson = false;
    } else if (!myNet->getViewNet()->getDataViewOptions().showDemandElements()) {
        drawPerson = false;
    } else if (!myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this)) {
        drawPerson = false;
    } else if (getChildDemandElements().empty()) {
        drawPerson = false;
    }
    // continue if person can be drawn
    if (drawPerson) {
        // obtain exaggeration (and add the special personExaggeration)
        const double exaggeration = getExaggeration(s) + s.detailSettings.personExaggeration;
        // obtain width and length
        const double length = getTypeParent()->getAttributeDouble(SUMO_ATTR_LENGTH);
        const double width = getTypeParent()->getAttributeDouble(SUMO_ATTR_WIDTH);
        // obtain img file
        const std::string file = getTypeParent()->getAttribute(SUMO_ATTR_IMGFILE);
        // obtain position
        const Position personPosition = getAttributePosition(SUMO_ATTR_DEPARTPOS);
        // push GL ID
        GLHelper::pushName(getGlID());
        // push draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // translate and rotate
        glTranslated(personPosition.x(), personPosition.y(), 0);
        glRotated(90, 0, 0, 1);
        // set person color
        GLHelper::setColor(personColor);
        // set scale
        glScaled(exaggeration, exaggeration, 1);
        // draw person depending of detail level
        if (s.personQuality >= 2) {
            GUIBasePersonHelper::drawAction_drawAsImage(0, length, width, file, SUMOVehicleShape::PEDESTRIAN, exaggeration);
        } else if (s.personQuality == 1) {
            GUIBasePersonHelper::drawAction_drawAsCenteredCircle(length / 2, width / 2, s.scale * exaggeration);
        } else if (s.personQuality == 0) {
            GUIBasePersonHelper::drawAction_drawAsTriangle(0, length, width);
        }
        // pop matrix
        GLHelper::popMatrix();
        // draw stack label
        if (myStackedLabelNumber > 0) {
            drawStackLabel(myStackedLabelNumber, "person", Position(personPosition.x() - 2.5, personPosition.y()), -90, 1.3, 5, getExaggeration(s));
        } else if ((getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_STOPPERSON_BUSSTOP) ||
                    (getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_STOPPERSON_TRAINSTOP)) {
            // declare counter for stacked persons over stops
            int stackedCounter = 0;
            // get stoppingPlace
            const auto stoppingPlace = getChildDemandElements().front()->getParentAdditionals().front();
            // get stacked persons
            for (const auto& stopPerson : stoppingPlace->getChildDemandElements()) {
                if ((stopPerson->getTagProperty().getTag() == GNE_TAG_STOPPERSON_BUSSTOP) ||
                        (stopPerson->getTagProperty().getTag() == GNE_TAG_STOPPERSON_TRAINSTOP)) {
                    // get person parent
                    const auto personParent = stopPerson->getParentDemandElements().front();
                    // check if the stop if the first person plan parent
                    if (stopPerson->getPreviousChildDemandElement(personParent) == nullptr) {
                        stackedCounter++;
                    }
                }
            }
            // if we have more than two stacked elements, draw label
            if (stackedCounter > 1) {
                drawStackLabel(stackedCounter, "person", Position(personPosition.x() - 2.5, personPosition.y()), -90, 1.3, 5, getExaggeration(s));
            }
        }
        // draw flow label
        if (myTagProperty.isFlow()) {
            drawFlowLabel(Position(personPosition.x() - 1, personPosition.y() - 0.25), -90, 1.8, 2, getExaggeration(s));
        }
        // pop name
        GLHelper::popName();
        // draw name
        drawName(personPosition, s.scale, s.personName, s.angle);
        if (s.personValue.show(this)) {
            Position personValuePosition = personPosition + Position(0, 0.6 * s.personName.scaledSize(s.scale));
            const double value = getColorValue(s, s.personColorer.getActive());
            GLHelper::drawTextSettings(s.personValue, toString(value), personValuePosition, s.scale, s.angle, GLO_MAX - getType());
        }
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), personPosition, exaggeration, s.dottedContourSettings.segmentWidth);
        // draw dotted contour
        myContour.drawDottedContourRectangle(s, personPosition, 0.5, 0.5, 0, 0, 0, exaggeration,
                                                s.dottedContourSettings.segmentWidth);
    }
}


void
GNEPerson::computePathElement() {
    // compute all person plan children (because aren't computed in "computeDemandElements()")
    for (const auto& demandElement : getChildDemandElements()) {
        demandElement->computePathElement();
    }
}


void
GNEPerson::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawJunctionPartialGL
}


void
GNEPerson::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawJunctionPartialGL
}


GNELane*
GNEPerson::getFirstPathLane() const {
    // use path lane of first person plan
    return getChildDemandElements().front()->getFirstPathLane();
}


GNELane*
GNEPerson::getLastPathLane() const {
    // use path lane of first person plan
    return getChildDemandElements().front()->getLastPathLane();
}


std::string
GNEPerson::getAttribute(SumoXMLAttr key) const {
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
                return myTagProperty.getDefaultValue(SUMO_ATTR_COLOR);
            }
        case SUMO_ATTR_DEPARTPOS:
            if (wasSet(VEHPARS_DEPARTPOS_SET)) {
                return getDepartPos();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTPOS);
            }
        // Others
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            return getFlowAttribute(key);
    }
}


double
GNEPerson::getAttributeDouble(SumoXMLAttr key) const {
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
GNEPerson::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS: {
            // first check number of child demand elements
            if (getChildDemandElements().empty()) {
                return Position();
            }
            // get person plan
            const GNEDemandElement* personPlan = getChildDemandElements().front();
            // first check if first person plan is a stop
            if (personPlan->getTagProperty().isPlanStopPerson()) {
                // stop center
                return personPlan->getPositionInView();
            } else if (personPlan->getTagProperty().planFromTAZ()) {
                // TAZ
                return personPlan->getParentAdditionals().front()->getPositionInView();
            } else if (personPlan->getTagProperty().planFromJunction()) {
                // juncrtion
                return personPlan->getParentJunctions().front()->getPositionInView();
            } else {
                return personPlan->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS);
            }
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a Position attribute of type '" + toString(key) + "'");
    }
}


void
GNEPerson::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTPOS:
        // Other
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SELECTED:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setFlowAttribute(this, key, value, undoList);
            break;
    }
}


bool
GNEPerson::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(NamespaceIDs::persons, value);
        case SUMO_ATTR_TYPE:
            return (myNet->getAttributeCarriers()->retrieveDemandElements(NamespaceIDs::types, value, false) == nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_PERSON), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Other
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            return isValidFlowAttribute(this, key, value);
    }
}


void
GNEPerson::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    enableFlowAttribute(this, key, undoList);
}


void
GNEPerson::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    disableFlowAttribute(this, key, undoList);
}


bool
GNEPerson::isAttributeEnabled(SumoXMLAttr key) const {
    return isFlowAttributeEnabled(key);
}


std::string
GNEPerson::getPopUpID() const {
    return getTagStr();
}


std::string
GNEPerson::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID);
}


const Parameterised::Map&
GNEPerson::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// protected
// ===========================================================================

RGBColor
GNEPerson::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.personColorer;
    /*
        if (!setFunctionalColor(c.getActive())) {
            return c.getScheme().getColor(getColorValue(s, c.getActive()));
        }
    */
    return c.getScheme().getColor(getColorValue(s, c.getActive()));
}


bool
GNEPerson::setFunctionalColor(int /* activeScheme */) const {
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

GNEPerson::personPlanSegment::personPlanSegment(GNEDemandElement* _personPlan) :
    personPlan(_personPlan),
    edge(nullptr),
    arrivalPos(-1) {
}


GNEPerson::personPlanSegment::personPlanSegment() :
    personPlan(nullptr),
    edge(nullptr),
    arrivalPos(-1) {
}


void
GNEPerson::setAttribute(SumoXMLAttr key, const std::string& value) {
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
                parseDepartPos(value, toString(SUMO_TAG_PERSON), id, departPos, departPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOS_SET;
            } else {
                // set default value
                parseDepartPos(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_PERSON), id, departPos, departPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOS_SET;
            }
            // compute person
            updateGeometry();
            break;
        // Other
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
            setFlowAttribute(this, key, value);
            break;
    }
}


void
GNEPerson::toggleAttribute(SumoXMLAttr key, const bool value) {
    // toggle flow attributes
    toggleFlowAttribute(key, value);
}


void GNEPerson::setMoveShape(const GNEMoveResult& moveResult) {
    // change departPos
    departPosProcedure = DepartPosDefinition::GIVEN;
    departPos = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNEPerson::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(this, "departPos of " + getTagStr());
    // now set departPos
    setAttribute(SUMO_ATTR_DEPARTPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
