/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPerson.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
/// @version $Id$
///
// Representation of persons in NETEDIT
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <cmath>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_TLS.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>

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

GNEPerson::GNEPersonPopupMenu::GNEPersonPopupMenu(GNEPerson *person, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *person),
    myPerson(person),
    myTransformToPerson(nullptr),
    myTransformToPersonFlow(nullptr) { 
    // build header
    myPerson->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    myPerson->buildCenterPopupEntry(this);
    myPerson->buildPositionCopyEntry(this, false);
    // buld menu commands for names
    new FXMenuCommand(this, ("Copy " + myPerson->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    new FXMenuCommand(this, ("Copy " + myPerson->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    myPerson->getViewNet()->buildSelectionACPopupEntry(this, myPerson);
    myPerson->buildShowParamsPopupEntry(this);
    // add transform functions only in demand mode
    if (myPerson->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) {
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu comands for all transformations
        myTransformToPerson = new FXMenuCommand(transformOperation, "Person", GUIIconSubSys::getIcon(ICON_PERSON), this, MID_GNE_PERSON_TRANSFORM);
        myTransformToPersonFlow = new FXMenuCommand(transformOperation, "Person (embedded route)", GUIIconSubSys::getIcon(ICON_PERSONFLOW), this, MID_GNE_PERSON_TRANSFORM);
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

GNEPerson::GNESelectedPersonsPopupMenu::GNESelectedPersonsPopupMenu(GNEPerson *person, const std::vector<GNEPerson*> &selectedPerson, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *person),
    mySelectedPersons(selectedPerson),
    myPersonTag(person->getTagProperty().getTag()),
    myTransformToPerson(nullptr),
    myTransformToPersonFlow(nullptr) { 
    // build header
    person->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    person->buildCenterPopupEntry(this);
    person->buildPositionCopyEntry(this, false);
    // buld menu commands for names
    new FXMenuCommand(this, ("Copy " + person->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    new FXMenuCommand(this, ("Copy " + person->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    person->getViewNet()->buildSelectionACPopupEntry(this, person);
    person->buildShowParamsPopupEntry(this);
    // add transform functions only in demand mode
    if (person->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) {
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu comands for all transformations
        myTransformToPerson = new FXMenuCommand(transformOperation, "Person", GUIIconSubSys::getIcon(ICON_PERSON), this, MID_GNE_PERSON_TRANSFORM);
        myTransformToPersonFlow = new FXMenuCommand(transformOperation, "PersonFlow", GUIIconSubSys::getIcon(ICON_PERSONFLOW), this, MID_GNE_PERSON_TRANSFORM);
    }
}


GNEPerson::GNESelectedPersonsPopupMenu::~GNESelectedPersonsPopupMenu() {}


long
GNEPerson::GNESelectedPersonsPopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    // iterate over all selected persons
    for (const auto &i : mySelectedPersons) {
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

GNEPerson::GNEPerson(SumoXMLTag tag, GNEViewNet* viewNet, GNEDemandElement* pType, const SUMOVehicleParameter& personparameters) :
    GNEDemandElement(personparameters.id, viewNet, (tag == SUMO_TAG_PERSONFLOW) ? GLO_PERSONFLOW : GLO_PERSON, tag,
    {}, {}, {}, {}, {pType}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter(personparameters) {
    // set manually vtypeID (needed for saving)
    vtypeid = pType->getID();
}


GNEPerson::~GNEPerson() {}


SUMOVehicleClass
GNEPerson::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


std::string
GNEPerson::getBegin() const {
    // obtain depart depending if is a Person, trip or routeFlow
    std::string departStr;
    if (myTagProperty.getTag() == SUMO_TAG_PERSONFLOW) {
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


const RGBColor&
GNEPerson::getColor() const {
    return color;
}


void
GNEPerson::writeDemandElement(OutputDevice& device) const {
    // obtain tag depending if tagProperty has a synonym
    SumoXMLTag synonymTag = myTagProperty.hasTagSynonym()? myTagProperty.getTagSynonym() : myTagProperty.getTag();
    // attribute VType musn't be written if is DEFAULT_PEDTYPE_ID
    if (getDemandElementParents().at(0)->getID() == DEFAULT_PEDTYPE_ID) {
        // unset VType parameter
        parametersSet &= ~VEHPARS_VTYPE_SET;
        // write person attributes (VType will not be written)
        write(device, OptionsCont::getOptions(), synonymTag);
        // set VType parameter again
        parametersSet |= VEHPARS_VTYPE_SET;
    } else {
        // write person attributes, including VType
        write(device, OptionsCont::getOptions(), synonymTag, getDemandElementParents().at(0)->getID());
    }
    // write specific flow attributes
    if (myTagProperty.getTag() == SUMO_TAG_PERSONFLOW) {
        // write routeFlow values depending if it was set
        if (isDisjointAttributeSet(SUMO_ATTR_END)) {
            device.writeAttr(SUMO_ATTR_END,  time2string(repetitionEnd));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_NUMBER)) {
            device.writeAttr(SUMO_ATTR_NUMBER, repetitionNumber);
        }
        if (isDisjointAttributeSet(SUMO_ATTR_VEHSPERHOUR)) {
            device.writeAttr(SUMO_ATTR_VEHSPERHOUR, 3600. / STEPS2TIME(repetitionOffset));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_PERIOD)) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(repetitionOffset));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_PROB)) {
            device.writeAttr(SUMO_ATTR_PROB, repetitionProbability);
        }
    }
    // write demand element children associated to this person (Rides, Walks...)
    for (const auto& i : getDemandElementChildren()) {
        i->writeDemandElement(device);
    }
    // close person tag
    device.closeTag();
}


bool
GNEPerson::isDemandElementValid() const {
    // a single person is always valid
    return true;
}


std::string 
GNEPerson::getDemandElementProblem() const {
    // A single person cannot habe problem (but their children)
    return "";
}


void 
GNEPerson::fixDemandElementProblem() {
    // nothing to fix
}


void 
GNEPerson::startGeometryMoving() {
    // Persons cannot be moved
}


void 
GNEPerson::endGeometryMoving() {
    // Persons cannot be moved
}


void
GNEPerson::moveGeometry(const Position&) {
    // Persons cannot be moved
}


void
GNEPerson::commitGeometryMoving(GNEUndoList*) {
    // Persons cannot be moved
}


void
GNEPerson::updateGeometry() {
    // first clear geometry
    myDemandElementGeometry.shape.clear();
    // only calculate new shape if there is demand element childrens
    if (getDemandElementChildren().size() > 0) {
        // declare pointer to first person plan
        GNEDemandElement *firstPersonPlanElement = getDemandElementChildren().front();
        // declare pointer to first edge
        GNEEdge *firstEdge = firstPersonPlanElement->getEdgeParents().at(0);
        // obtain next person plan (if exist)
        GNEDemandElement *nextPersonPlanElement = (getDemandElementChildren().size() > 1)? getDemandElementChildren().at(1) : nullptr;
        // obtain pointer to busStop end (if exist)
        GNEAdditional* busStop = (firstPersonPlanElement->getAdditionalParents().size() > 0)? firstPersonPlanElement->getAdditionalParents().front() : nullptr;
        // start with the first edge of first demand element children
        if (busStop && (&busStop->getLaneParents().front()->getParentEdge() == firstEdge)) {
            // add first segment
            myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(firstPersonPlanElement, firstEdge, firstEdge->getLanes().front()->getGeometry().shape.front()));
            // calculate special shape busStop
            auto shapeBusStop = calculatePersonPlanConnectionBusStop(firstEdge, busStop);
            // add first shape busStop in geometry
            for (const auto &shapeBusStopPos : shapeBusStop.first) {
                // last segment must be invisible
                if (shapeBusStopPos == shapeBusStop.first.back()) {
                    myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(firstPersonPlanElement, firstEdge, shapeBusStopPos, false));
                } else {
                    myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(firstPersonPlanElement, firstEdge, shapeBusStopPos));
                }
            }
            // add the rest of shape in next person plan element
            if (nextPersonPlanElement) {
                // add second shape busStop in geometry referencing add first shape busStop in geometry 
                for (const auto &shapeBusStopPos : shapeBusStop.second) {
                    myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(nextPersonPlanElement, firstEdge, shapeBusStopPos));
                }
            }
        } else {
            // add lane shape in personPlan shape
            for (const auto &shapeLanePos : firstEdge->getLanes().front()->getGeometry().shape) {
                myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(firstPersonPlanElement, firstEdge, shapeLanePos));
            }
        }
        // iterate over every demand element children
        for (auto i = getDemandElementChildren().begin(); i != getDemandElementChildren().end(); i++) {
            // declare pointers with the current and next person plan elements (to improve code Legibility)
            GNEDemandElement *currentPersonPlanElement = (*i);
            GNEDemandElement *nextPersonPlanElement = ((i+1) != getDemandElementChildren().end())? *(i+1) : nullptr;
            // special case for person plan with an unique edge
            if ((currentPersonPlanElement->getEdgeParents().size() == 1) && currentPersonPlanElement->getDemandElementParents().empty()) {
                // obtain pointers to unique edge
                GNEEdge* uniqueEdge = currentPersonPlanElement->getEdgeParents().at(0);
                // add lane shape in personPlan shape
                for (const auto &shapeLanePos : uniqueEdge->getLanes().front()->getGeometry().shape) {
                    myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(currentPersonPlanElement, uniqueEdge, shapeLanePos));
                }
            } else {
                // iterate from second edge parent until final 
                for (auto j = (currentPersonPlanElement->getEdgeParents().begin() + 1); j != currentPersonPlanElement->getEdgeParents().end(); j++) {
                    // obtain pointers to previous, current and next edge
                    GNEEdge* previousEdge = *(j-1);
                    GNEEdge* currentEdge = *j;
                    GNEEdge* nextEdge = ((j+1) != currentPersonPlanElement->getEdgeParents().end())? *(j+1) : nullptr;
                    // obtain pointer to busStop end (if exist)
                    GNEAdditional* busStop = (currentPersonPlanElement->getAdditionalParents().size() > 0)? currentPersonPlanElement->getAdditionalParents().front() : nullptr;
                    // first calculate smootShape between previous and current edge
                    if (j == (currentPersonPlanElement->getEdgeParents().begin() + 1)) {
                        // calculate smootshape between the previous and the next edge and ad it in personPlan shape
                        calculateSmoothPersonPlanConnection(currentPersonPlanElement, previousEdge, currentEdge);
                    }
                    // now check if we have to draw the special lane for busStops or lane
                    if (busStop && (&busStop->getLaneParents().front()->getParentEdge() == currentEdge)) {
                        // calculate special shape busStop
                        auto shapeBusStop = calculatePersonPlanConnectionBusStop(currentEdge, busStop);
                        // add first shape busStop in geometry
                        for (const auto &shapeBusStopPos : shapeBusStop.first) {
                            // last segment must be invisible
                            if (shapeBusStopPos == shapeBusStop.first.back()) {
                                myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(currentPersonPlanElement, currentEdge, shapeBusStopPos, false));
                            } else {
                                myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(currentPersonPlanElement, currentEdge, shapeBusStopPos));
                            }
                        }
                        // add the rest of shape in next person plan element
                        if (nextPersonPlanElement) {
                            // add second shape busStop in geometry referencing add first shape busStop in geometry 
                            for (const auto &shapeBusStopPos : shapeBusStop.second) {
                                myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(nextPersonPlanElement, currentEdge, shapeBusStopPos));
                            }
                        }
                    } else {
                        // add lane shape over personPlan shape
                        for (const auto &shapeLanePos : currentEdge->getLanes().front()->getGeometry().shape) {
                            // save segment
                            myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(currentPersonPlanElement, currentEdge, shapeLanePos));
                        }
                        // check if next smootShape can be calculated
                        if (nextEdge) {
                            // calculate smootshape between the previous and the next edge and ad it in personPlan shape
                            calculateSmoothPersonPlanConnection(currentPersonPlanElement, currentEdge, nextEdge);
                        }
                    }
                }
            }
        }
    }
    // calculate shape rotations and lenghts
    myDemandElementGeometry.calculateShapeRotationsAndLengths();
}


Position
GNEPerson::getPositionInView() const {
    // Position in view depend of first child element
    if (getDemandElementChildren().size() > 0) {
        GNELane* lane = getDemandElementChildren().at(0)->getEdgeParents().at(0)->getLanes().front();
        if (lane->getGeometry().shape.length() < 2.5) {
            return lane->getGeometry().shape.front();
        } else {
            Position A = lane->getGeometry().shape.positionAtOffset(2.5);
            Position B = lane->getGeometry().shape.positionAtOffset(2.5);
            // return Middle point
            return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
        }
    } else {
        return Position(0,0);
    }
}


GUIGLObjectPopupMenu* 
GNEPerson::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // return a GNEPersonPopupMenu
    return new GNEPersonPopupMenu(this, app, parent);
}


std::string
GNEPerson::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


Boundary
GNEPerson::getCenteringBoundary() const {
    Boundary personBoundary;
    if (getDemandElementChildren().size() > 0) {
        personBoundary.add(getDemandElementChildren().at(1)->getEdgeParents().at(0)->getLanes().front()->getGeometry().shape.front());
    } else {
        personBoundary = Boundary(-0.1, -0.1, 0.1, 0.1);
    }
    personBoundary.grow(20);
    return personBoundary;
}


void
GNEPerson::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptionsNetwork().showDemandElements() && myViewNet->getViewOptionsDemand().showNonInspectedDemandElements(this)) {
        glPushName(getGlID());
        glPushMatrix();
        Position p1 = getDemandElementChildren().front()->getEdgeParents().front()->getLanes().front()->getGeometry().shape.front();
        glTranslated(p1.x(), p1.y(), getType());
        glRotated(90, 0, 0, 1);
        // set person color
        setColor(s);
        // scale
        const double upscale = s.personSize.getExaggeration(s, this, 80);
        glScaled(upscale, upscale, 1);
        // draw person as poly
        drawAction_drawAsPoly(s);
        // pop matrix
        glPopMatrix();
        drawName(p1, s.scale, s.personName, s.angle);
        if (s.personValue.show) {
            Position p2 = p1 + Position(0, 0.6 * s.personName.scaledSize(s.scale));
            const double value = getColorValue(s, s.personColorer.getActive());
            GLHelper::drawTextSettings(s.personValue, toString(value), p2, s.scale, s.angle, GLO_MAX - getType());
        }
        glPopName();
    }
}


void
GNEPerson::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(getType());
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEPerson::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(getType());
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNEPerson::getAttribute(SumoXMLAttr key) const {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_TYPE:
            return getDemandElementParents().at(0)->getID();
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
        // Specific of persons
        case SUMO_ATTR_DEPART:
            return toString(depart);
        // Specific of personFlows
        case SUMO_ATTR_BEGIN:
            return time2string(depart);
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_VEHSPERHOUR:
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
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double 
GNEPerson::getAttributeDouble(SumoXMLAttr /*key*/) const {
    return 0;
}


void
GNEPerson::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTPOS:
        // Specific of persons
        case SUMO_ATTR_DEPART:
        // Specific of personFlows
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_PROB:
        //
        case GNE_ATTR_GENERIC:
        case GNE_ATTR_SELECTED:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPerson::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            // Persons and personflows share namespace
            if (SUMOXMLDefinitions::isValidVehicleID(value) && 
                (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_PERSON, value, false) == nullptr) &&
                (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_PERSONFLOW, value, false) == nullptr)) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Specific of persons
        case SUMO_ATTR_DEPART: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Specific of personflows
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
        case SUMO_ATTR_VEHSPERHOUR:
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
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNEPerson::isAttributeSet(SumoXMLAttr /*key*/) const {
    return true;
}


bool
GNEPerson::isDisjointAttributeSet(const SumoXMLAttr attr) const {
    switch (attr) {
        case SUMO_ATTR_END:
            return (parametersSet & VEHPARS_END_SET) != 0;
        case SUMO_ATTR_NUMBER:
            return (parametersSet & VEHPARS_NUMBER_SET) != 0;
        case SUMO_ATTR_VEHSPERHOUR:
            return (parametersSet & VEHPARS_VPH_SET) != 0;
        case SUMO_ATTR_PERIOD:
            return (parametersSet & VEHPARS_PERIOD_SET) != 0;
        case SUMO_ATTR_PROB:
            return (parametersSet & VEHPARS_PROB_SET) != 0;
        default:
            return true;
    };
}


void
GNEPerson::setDisjointAttribute(const SumoXMLAttr attr, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int parametersSetCopy = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (attr) {
        case SUMO_ATTR_END: {
            // give more priority to end
            parametersSetCopy = VEHPARS_END_SET | VEHPARS_NUMBER_SET;
            break;
        }
        case SUMO_ATTR_NUMBER:
            parametersSetCopy ^= VEHPARS_END_SET;
            parametersSetCopy |= VEHPARS_NUMBER_SET;
            break;
        case SUMO_ATTR_VEHSPERHOUR: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set VehsPerHour
            parametersSetCopy |= VEHPARS_VPH_SET;
            break;
        }
        case SUMO_ATTR_PERIOD: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set period
            parametersSetCopy |= VEHPARS_PERIOD_SET;
            break;
        }
        case SUMO_ATTR_PROB: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set probability
            parametersSetCopy |= VEHPARS_PROB_SET;
            break;
        }
        default:
            break;
    }
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), parametersSet, parametersSetCopy));
}


std::string
GNEPerson::getPopUpID() const {
    return getTagStr();
}


std::string
GNEPerson::getHierarchyName() const {
    // special case for Trips and flow
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check if we're inspecting a Edge
        if (myViewNet->getNet()->getViewNet()->getDottedAC() && 
            myViewNet->getNet()->getViewNet()->getDottedAC()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
            // check if edge correspond to a "from", "to" or "via" edge
            if (getEdgeParents().front() == myViewNet->getNet()->getViewNet()->getDottedAC()) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (from)";
            } else if (getEdgeParents().front() == myViewNet->getNet()->getViewNet()->getDottedAC()) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (to)";
            } else {
                // iterate over via
                for (const auto & i : via) {
                    if (i == myViewNet->getNet()->getViewNet()->getDottedAC()->getID()) {
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

void
GNEPerson::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.personColorer;
    if (!setFunctionalColor(c.getActive())) {
        GLHelper::setColor(c.getScheme().getColor(getColorValue(s, c.getActive())));
    }
}


bool
GNEPerson::setFunctionalColor(int activeScheme) const {
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


void
GNEPerson::drawAction_drawAsPoly(const GUIVisualizationSettings& /* s */) const {
    // draw pedestrian shape
    RGBColor lighter = GLHelper::getColor().changedBrightness(51);
    glTranslated(0, 0, .045);
    // head
    glScaled(1, 0.5, 1.);
    GLHelper::drawFilledCircle(0.5);
    // nose
    glBegin(GL_TRIANGLES);
    glVertex2d(0.0, -0.2);
    glVertex2d(0.0, 0.2);
    glVertex2d(-0.6, 0.0);
    glEnd();
    glTranslated(0, 0, -.045);
    // body
    glScaled(0.9, 2.0, 1);
    glTranslated(0, 0, .04);
    GLHelper::setColor(lighter);
    GLHelper::drawFilledCircle(0.5);
    glTranslated(0, 0, -.04);
}


void
GNEPerson::drawAction_drawAsImage(const GUIVisualizationSettings& s) const {
    const std::string& file = getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_IMGFILE);
    if (file != "") {
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const double exaggeration = s.personSize.getExaggeration(s, this);
            const double halfLength = getDemandElementParents().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH) / 2.0 * exaggeration;
            const double halfWidth = getDemandElementParents().at(0)->getAttributeDouble(SUMO_ATTR_WIDTH) / 2.0 * exaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, -halfLength, halfWidth, halfLength);
        }
    } else {
        // fallback if no image is defined
        drawAction_drawAsPoly(s);
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPerson::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_TYPE:
            changeDemandElementParent(this, value, 0);
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
            break;
        // Specific of persons
        case SUMO_ATTR_DEPART: {
            std::string oldDepart = getBegin();
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            myViewNet->getNet()->updateDemandElementBegin(oldDepart, this);
            break;
        }
        // Specific of personFlows
        case SUMO_ATTR_BEGIN: {
            std::string oldBegin = getBegin();
            depart = string2time(value);
            myViewNet->getNet()->updateDemandElementBegin(oldBegin, this);
            break;
        }
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
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
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void 
GNEPerson::setDisjointAttribute(const int newParameterSet) {
    parametersSet = newParameterSet;
}


void 
GNEPerson::calculateSmoothPersonPlanConnection(GNEDemandElement* personPlanElement, const GNEEdge *edgeFrom, const GNEEdge *edgeTo) {
    // calculate smooth shape
    PositionVector smoothShape = edgeFrom->getNBEdge()->getToNode()->computeSmoothShape(
        edgeFrom->getLanes().front()->getGeometry().shape, 
        edgeTo->getLanes().front()->getGeometry().shape, 
        5, false,
        (double) 5. * (double) edgeFrom->getNBEdge()->getNumLanes(),
        (double) 5. * (double) edgeTo->getNBEdge()->getNumLanes());
    // add smootshape in personPlan shape
    for (const auto &i : smoothShape) {
        myDemandElementGeometry.shape.push_back(DemandElementGeometry::Segment(personPlanElement, edgeTo, i));
    }
}


std::pair<PositionVector, PositionVector> 
GNEPerson::calculatePersonPlanConnectionBusStop(GNEEdge* edge, GNEAdditional* busStop) {
    std::pair<PositionVector, PositionVector> result;
    // declare a reference to lane shape
    const PositionVector &laneShape = edge->getLanes().front()->getGeometry().shape;
    // obtain first position values of busStop shape
    const Position &firstBusStopShapePosition = busStop->getAdditionalGeometry().shape.front();
    double offsetFirstPosition = laneShape.nearest_offset_to_point2D(firstBusStopShapePosition, false);
    // split laneShape
    auto splittedFirstLaneShape = laneShape.splitAt(offsetFirstPosition, true);
    // fill result position vector
    for (const auto &i : splittedFirstLaneShape.first) {
        result.first.push_back(i);
    }
    // finally add first BusStop shape position
    result.first.push_back(firstBusStopShapePosition);
    // obtain second position of busStops
    const Position &lastBusStopShapePosition = busStop->getAdditionalGeometry().shape.back();
    double offsetLastPosition = laneShape.nearest_offset_to_point2D(lastBusStopShapePosition, false);
    // split laneShape
    auto splittedLastLaneShape = laneShape.splitAt(offsetLastPosition, true);
    // first add last BusStop shape position
    result.second.push_back(lastBusStopShapePosition);
    // fill result position vector
    for (const auto &i : splittedLastLaneShape.second) {
        result.second.push_back(i);
    }
    return result;
}


/****************************************************************************/
