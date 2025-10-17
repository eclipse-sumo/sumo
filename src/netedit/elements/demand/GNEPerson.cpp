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
/// @file    GNEPerson.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// Representation of persons in netedit
/****************************************************************************/

#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementPlanParent.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIBasePersonHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
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
    GUIGLObjectPopupMenu(app, parent, person),
    myPerson(person),
    myTransformToPerson(nullptr),
    myTransformToPersonFlow(nullptr) {
    // build common options
    person->buildPopUpMenuCommonOptions(this, app, person->myNet->getViewNet(), person->getTagProperty()->getTag(), person->isAttributeCarrierSelected());
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
        if (myPerson->getTagProperty()->getTag() == SUMO_TAG_PERSON) {
            myTransformToPerson->disable();
        } else if (myPerson->getTagProperty()->getTag() == SUMO_TAG_PERSONFLOW) {
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
    GUIGLObjectPopupMenu(app, parent, person),
    myPersonTag(person->getTagProperty()->getTag()),
    mySelectedPersons(selectedPerson),
    myTransformToPerson(nullptr),
    myTransformToPersonFlow(nullptr) {
    // build common options
    person->buildPopUpMenuCommonOptions(this, app, person->myNet->getViewNet(), person->myTagProperty->getTag(), person->isAttributeCarrierSelected());
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
                (i->getTagProperty()->getTag() == myPersonTag)) {
            GNERouteHandler::transformToPerson(i);
        } else if ((obj == myTransformToPersonFlow) &&
                   (i->getTagProperty()->getTag() == myPersonTag)) {
            GNERouteHandler::transformToPerson(i);
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
GNEPerson::GNEPerson(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, "", tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementFlow(this),
    myMoveElementPlanParent(new GNEMoveElementPlanParent(this, departPos, departPosProcedure)) {
    // enable set and persons per hour as default flow values
    toggleAttribute(SUMO_ATTR_END, true);
    toggleAttribute(SUMO_ATTR_PERSONSPERHOUR, true);
}


GNEPerson::GNEPerson(SumoXMLTag tag, GNENet* net, const std::string& filename, GNEDemandElement* pType, const SUMOVehicleParameter& personparameters) :
    GNEDemandElement(personparameters.id, net, filename, tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementFlow(this, personparameters),
    myMoveElementPlanParent(new GNEMoveElementPlanParent(this, departPos, departPosProcedure)) {
    // set parents
    setParent<GNEDemandElement*>(pType);
    // set manually vtypeID (needed for saving)
    vtypeid = pType->getID();
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

GNEPerson::~GNEPerson() {}


GNEMoveElement*
GNEPerson::getMoveElement() const {
    return myMoveElementPlanParent;
}


Parameterised*
GNEPerson::getParameters() {
    return this;
}


const Parameterised*
GNEPerson::getParameters() const {
    return this;
}


void
GNEPerson::writeDemandElement(OutputDevice& device) const {
    // attribute VType musn't be written if is DEFAULT_PEDTYPE_ID
    if (getTypeParent()->getID() == DEFAULT_PEDTYPE_ID) {
        // unset VType parameter
        parametersSet &= ~VEHPARS_VTYPE_SET;
        // write person attributes (VType will not be written)
        write(device, OptionsCont::getOptions(), myTagProperty->getXMLTag());
        // set VType parameter again
        parametersSet |= VEHPARS_VTYPE_SET;
    } else {
        // write person attributes, including VType
        write(device, OptionsCont::getOptions(), myTagProperty->getXMLTag(), getTypeParent()->getID());
    }
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
        if (getChildDemandElements().front()->getTagProperty()->isPlanStopPerson()) {
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
    // check if person can be drawn
    if (!myNet->getViewNet()->getNetworkViewOptions().showDemandElements()) {
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
        const double exaggeration = getExaggeration(s) + 10;
        // obtain position
        const Position personPosition = getAttributePosition(SUMO_ATTR_DEPARTPOS);
        if (personPosition == Position::INVALID) {
            return;
        }
        // get detail level
        const auto d = s.getDetailLevel(exaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawPerson(d, isAttributeCarrierSelected())) {
            // obtain width and length
            const double length = getTypeParent()->getAttributeDouble(SUMO_ATTR_LENGTH);
            const double width = getTypeParent()->getAttributeDouble(SUMO_ATTR_WIDTH);
            // obtain img file
            const std::string file = getTypeParent()->getAttribute(SUMO_ATTR_IMGFILE);
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            drawInLayer(getType());
            // translate and rotate
            glTranslated(personPosition.x(), personPosition.y(), 0);
            glRotated(90, 0, 0, 1);
            // set person color
            GLHelper::setColor(getDrawingColor(s));
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
            } else if ((getChildDemandElements().front()->getTagProperty()->getTag() == GNE_TAG_STOPPERSON_BUSSTOP) ||
                       (getChildDemandElements().front()->getTagProperty()->getTag() == GNE_TAG_STOPPERSON_TRAINSTOP)) {
                // declare counter for stacked persons over stops
                int stackedCounter = 0;
                // get stoppingPlace
                const auto stoppingPlace = getChildDemandElements().front()->getParentAdditionals().front();
                // get stacked persons
                for (const auto& stopPerson : stoppingPlace->getChildDemandElements()) {
                    if ((stopPerson->getTagProperty()->getTag() == GNE_TAG_STOPPERSON_BUSSTOP) ||
                            (stopPerson->getTagProperty()->getTag() == GNE_TAG_STOPPERSON_TRAINSTOP)) {
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
            if (myTagProperty->isFlow()) {
                drawFlowLabel(Position(personPosition.x() - 1, personPosition.y() - 0.25), -90, 1.8, 2, getExaggeration(s));
            }
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), personPosition, exaggeration, s.dottedContourSettings.segmentWidth);
            // draw name
            drawName(personPosition, s.scale, s.personName, s.angle);
            if (s.personValue.show(this)) {
                Position personValuePosition = personPosition + Position(0, 0.6 * s.personName.scaledSize(s.scale));
                const double value = getColorValue(s, s.personColorer.getActive());
                GLHelper::drawTextSettings(s.personValue, toString(value), personValuePosition, s.scale, s.angle, GLO_MAX - getType());
            }
            // draw dotted contour
            myPersonContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour
        myPersonContour.calculateContourRectangleShape(s, d, this, personPosition, 0.1, 0.2, getType(), -1.1, 0, 0, exaggeration, nullptr);
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
GNEPerson::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // Stops don't use drawJunctionPartialGL
}


void
GNEPerson::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
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
            if (personPlan->getTagProperty()->isPlanStopPerson()) {
                // stop center
                return personPlan->getPositionInView();
            } else if (personPlan->getTagProperty()->planFromTAZ()) {
                // TAZ
                if (personPlan->getParentAdditionals().front()->getAttribute(SUMO_ATTR_CENTER).empty()) {
                    return personPlan->getParentAdditionals().front()->getAttributePosition(GNE_ATTR_TAZ_CENTROID);
                } else {
                    return personPlan->getParentAdditionals().front()->getAttributePosition(SUMO_ATTR_CENTER);
                }
            } else if (personPlan->getTagProperty()->planFromJunction()) {
                // juncrtion
                return personPlan->getParentJunctions().front()->getPositionInView();
            } else {
                return personPlan->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS);
            }
        }
        default:
            return getCommonAttributePosition(key);
    }
}


void
GNEPerson::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNEPerson::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(NamespaceIDs::persons, value);
        case SUMO_ATTR_TYPE:
            return (myNet->getAttributeCarriers()->retrieveDemandElements(NamespaceIDs::types, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_PERSON), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
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

// ===========================================================================
// protected
// ===========================================================================

RGBColor
GNEPerson::getDrawingColor(const GUIVisualizationSettings& s) const {
    if (isAttributeCarrierSelected()) {
        return s.colorSettings.selectedPersonColor;
    } else {
        return getColorByScheme(s.personColorer, this);
    }
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
                parseDepartPos(value, toString(SUMO_TAG_PERSON), id, departPos, departPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOS_SET;
            } else {
                // set default value
                parseDepartPos(myTagProperty->getDefaultStringValue(key), toString(SUMO_TAG_PERSON), id, departPos, departPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOS_SET;
            }
            // compute person
            updateGeometry();
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

/****************************************************************************/
