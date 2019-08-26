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
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_EnableAttribute.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIBasePersonHelper.h>

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

GNEPerson::GNEPerson(SumoXMLTag tag, GNEViewNet* viewNet, GNEDemandElement* pType, const SUMOVehicleParameter& personparameters) :
    GNEDemandElement(personparameters.id, viewNet, (tag == SUMO_TAG_PERSONFLOW) ? GLO_PERSONFLOW : GLO_PERSON, tag,
{}, {}, {}, {}, {pType}, {}, {}, {}, {}, {}),
SUMOVehicleParameter(personparameters) {
    // set manually vtypeID (needed for saving)
    vtypeid = pType->getID();
}


GNEPerson::~GNEPerson() {}


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


void
GNEPerson::writeDemandElement(OutputDevice& device) const {
    // obtain tag depending if tagProperty has a synonym
    SumoXMLTag synonymTag = myTagProperty.hasTagSynonym() ? myTagProperty.getTagSynonym() : myTagProperty.getTag();
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
        if (isAttributeEnabled(SUMO_ATTR_END)) {
            device.writeAttr(SUMO_ATTR_END,  time2string(repetitionEnd));
        }
        if (isAttributeEnabled(SUMO_ATTR_NUMBER)) {
            device.writeAttr(SUMO_ATTR_NUMBER, repetitionNumber);
        }
        if (isAttributeEnabled(SUMO_ATTR_VEHSPERHOUR)) {
            device.writeAttr(SUMO_ATTR_VEHSPERHOUR, 3600. / STEPS2TIME(repetitionOffset));
        }
        if (isAttributeEnabled(SUMO_ATTR_PERIOD)) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(repetitionOffset));
        }
        if (isAttributeEnabled(SUMO_ATTR_PROB)) {
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


GNEEdge*
GNEPerson::getFromEdge() const {
    return getDemandElementChildren().front()->getFromEdge();
}


GNEEdge*
GNEPerson::getToEdge() const {
    return getDemandElementChildren().front()->getToEdge();
}


SUMOVehicleClass
GNEPerson::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


const RGBColor&
GNEPerson::getColor() const {
    return color;
}


void
GNEPerson::compute() {
    // Nothing to compute
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
    // first check if geometry is deprecated
    if (myDemandElementSegmentGeometry.geometryDeprecated) {
        // first clear geometry
        myDemandElementSegmentGeometry.clearDemandElementSegmentGeometry();
        // only calculate new shape if there is demand element childrens
        if (getDemandElementChildren().size() > 0) {
            std::vector<personPlanSegment> personPlanSegments;
            // iterate over all demand element childrens
            for (const auto& personPlan : getDemandElementChildren()) {
                GNEAdditional* busStop = (personPlan->getAdditionalParents().size() > 0) ? personPlan->getAdditionalParents().front() : nullptr;
                // special case for person stops
                if (personPlan->getTagProperty().isPersonStop()) {
                    // declare a segment
                    personPlanSegment segment(personPlan);
                    // set stop in segment
                    segment.stops.push_back(personPlan);
                    // set edge depending of stop type
                    if (personPlan->getTagProperty().getTag() == SUMO_TAG_PERSONSTOP_LANE) {
                        segment.edge = &personPlan->getLaneParents().front()->getParentEdge();
                    } else {
                        segment.edge = &personPlan->getAdditionalParents().front()->getLaneParents().front()->getParentEdge();
                    }
                    // add segment to personPlanSegments
                    personPlanSegments.push_back(segment);
                } else if (personPlan->getTagProperty().getTag() == SUMO_TAG_WALK_ROUTE) {
                    // iterate over all demand element's route edges
                    for (const auto& j : personPlan->getDemandElementParents().at(1)->getEdgeParents()) {
                        // declare a segment
                        personPlanSegment segment(personPlan);
                        // set edge in segment
                        segment.edge = j;
                        // check if busStop can be set
                        if (busStop && (&busStop->getLaneParents().front()->getParentEdge() == segment.edge)) {
                            segment.busStops.push_back(busStop);
                        }
                        // check if arrivalPos has to be set
                        if (personPlan->getTagProperty().hasAttribute(SUMO_ATTR_ARRIVALPOS) && (personPlan->getDemandElementParents().at(1)->getEdgeParents().back() == j)) {
                            segment.arrivalPos = personPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
                        }
                        // add segment to personPlanSegments
                        personPlanSegments.push_back(segment);
                    }
                } else {
                    // iterate over all demand element's edges
                    for (const auto& j : personPlan->getEdgeParents()) {
                        // declare a segment
                        personPlanSegment segment(personPlan);
                        // set edge in segment
                        segment.edge = j;
                        // check if busStop can be set
                        if (busStop && (&busStop->getLaneParents().front()->getParentEdge() == segment.edge)) {
                            segment.busStops.push_back(busStop);
                        }
                        // check if arrivalPos has to be set
                        if (personPlan->getTagProperty().hasAttribute(SUMO_ATTR_ARRIVALPOS) && (personPlan->getEdgeParents().back() == j)) {
                            segment.arrivalPos = personPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
                        }
                        // add segment to personPlanSegments
                        personPlanSegments.push_back(segment);
                    }
                }
            }
            // now filter personPlanSegments
            auto it = personPlanSegments.begin();
            // iterate over segment plan
            while ((it != personPlanSegments.end()) && (it != (personPlanSegments.end() - 1))) {
                // check if this element and next element shares the same edge
                if (it->edge == (it + 1)->edge) {
                    // copy all busStops from next segment to previous segment
                    it->busStops.insert(it->busStops.end(), (it + 1)->busStops.begin(), (it + 1)->busStops.end());
                    // copy all stops from next segment to previous segment
                    it->stops.insert(it->stops.end(), (it + 1)->stops.begin(), (it + 1)->stops.end());
                    // erase next segment (note: don't copy arrival position)
                    personPlanSegments.erase(it + 1);
                    // start again
                    it = personPlanSegments.begin();
                } else {
                    it++;
                }
            }
            // now set shape
            for (auto personPlanSegmentsIT = personPlanSegments.begin(); personPlanSegmentsIT != personPlanSegments.end(); personPlanSegmentsIT++) {
                // obtain first lane (special case for rides)
                SUMOVehicleClass vClassOfPersonPlanSegmentsIT = personPlanSegmentsIT->personPlan->getTagProperty().isRide() ? SVC_PASSENGER : SVC_PEDESTRIAN;
                GNELane* firstLane = personPlanSegmentsIT->edge->getLaneByVClass(vClassOfPersonPlanSegmentsIT);
                // obtain next lane (special case for rides)
                GNELane* nextLane = nullptr;
                // check that next person plan segment isn't the last
                if ((personPlanSegmentsIT + 1) != personPlanSegments.end()) {
                    SUMOVehicleClass vClassOfNextPersonPlanSegmentsIT = (personPlanSegmentsIT + 1)->personPlan->getTagProperty().isRide() ? SVC_PASSENGER : SVC_PEDESTRIAN;
                    nextLane = personPlanSegmentsIT->edge->getLaneByVClass(vClassOfNextPersonPlanSegmentsIT);
                }
                if (personPlanSegmentsIT->stops.size() > 0) {
                    // iterate over all stops
                    for (const auto& stop : personPlanSegmentsIT->stops) {
                        // obtain stop shapes
                        auto shapesStop = calculatePersonPlanConnectionStop(firstLane, stop, nextLane);
                        // add first shape
                        for (const auto& shapesStopPos : shapesStop.first) {
                            // last segment must be invisible
                            if (shapesStopPos == shapesStop.first.back()) {
                                myDemandElementSegmentGeometry.insertEdgeSegment(personPlanSegmentsIT->personPlan, personPlanSegmentsIT->edge, shapesStopPos, false, true);
                            } else {
                                myDemandElementSegmentGeometry.insertEdgeSegment(personPlanSegmentsIT->personPlan, personPlanSegmentsIT->edge, shapesStopPos, true, true);
                            }
                        }
                        // check that next person plan segment isn't the last
                        if ((personPlanSegmentsIT + 1) != personPlanSegments.end()) {
                            // add second shape
                            for (const auto& shapesStopPos : shapesStop.second) {
                                myDemandElementSegmentGeometry.insertEdgeSegment((personPlanSegmentsIT + 1)->personPlan, personPlanSegmentsIT->edge, shapesStopPos, true, true);
                            }
                        }
                    }
                } else if (personPlanSegmentsIT->busStops.size() > 0) {
                    // iterate over all busStops
                    for (const auto& busStop : personPlanSegmentsIT->busStops) {
                        // obtain busStop shapes
                        auto shapesBusStop = calculatePersonPlanConnectionBusStop(firstLane, busStop, nextLane);
                        // add first shape
                        for (const auto& shapeBusStopPos : shapesBusStop.first) {
                            // last segment must be invisible
                            if (shapeBusStopPos == shapesBusStop.first.back()) {
                                myDemandElementSegmentGeometry.insertEdgeSegment(personPlanSegmentsIT->personPlan, personPlanSegmentsIT->edge, shapeBusStopPos, false, true);
                            } else {
                                myDemandElementSegmentGeometry.insertEdgeSegment(personPlanSegmentsIT->personPlan, personPlanSegmentsIT->edge, shapeBusStopPos, true, true);
                            }
                        }
                        // check that next person plan segment isn't the last
                        if ((personPlanSegmentsIT + 1) != personPlanSegments.end()) {
                            // add second shape
                            for (const auto& shapeBusStopPos : shapesBusStop.second) {
                                myDemandElementSegmentGeometry.insertEdgeSegment((personPlanSegmentsIT + 1)->personPlan, personPlanSegmentsIT->edge, shapeBusStopPos, true, true);
                            }
                        }
                    }
                } else if (personPlanSegmentsIT->arrivalPos != -1) {
                    // obtain busStop shapes
                    auto shapeArrival = calculatePersonPlanConnectionArrivalPos(firstLane, personPlanSegmentsIT->arrivalPos, nextLane);
                    // add first shape
                    for (const auto& shapeArrivalPos : shapeArrival.first) {
                        // special case for the last segment
                        if ((shapeArrivalPos == shapeArrival.first.back()) && (shapeArrival.first.size() > 0) && (shapeArrival.second.size() > 0)) {
                            myDemandElementSegmentGeometry.insertEdgeSegment((personPlanSegmentsIT + 1)->personPlan, personPlanSegmentsIT->edge, shapeArrivalPos, true, true);
                        } else {
                            myDemandElementSegmentGeometry.insertEdgeSegment(personPlanSegmentsIT->personPlan, personPlanSegmentsIT->edge, shapeArrivalPos, true, true);
                        }
                    }
                    // add second shape
                    for (const auto& shapeArrivalPos : shapeArrival.second) {
                        myDemandElementSegmentGeometry.insertEdgeSegment((personPlanSegmentsIT + 1)->personPlan, personPlanSegmentsIT->edge, shapeArrivalPos, true, true);
                    }
                } else {
                    // obtain lane (special case due rides)
                    GNELane* lane = personPlanSegmentsIT->edge->getLaneByVClass(vClassOfPersonPlanSegmentsIT);
                    // add lane shape over personPlan shape
                    for (int i = 0; i < (int)lane->getGeometry().shape.size(); i++) {
                        // insert segment
                        if (i < (int)lane->getGeometry().shape.size() - 1) {
                            myDemandElementSegmentGeometry.insertEdgeLengthRotSegment(personPlanSegmentsIT->personPlan, personPlanSegmentsIT->edge,
                                    lane->getGeometry().shape[i],
                                    lane->getGeometry().shapeLengths[i],
                                    lane->getGeometry().shapeRotations[i],
                                    true, true);
                        } else {
                            myDemandElementSegmentGeometry.insertEdgeSegment(personPlanSegmentsIT->personPlan, personPlanSegmentsIT->edge,
                                    lane->getGeometry().shape[i], true, true);
                        }
                    }
                }
                // if this isn't the last person plan segment, calculate a smooth shape connection
                if ((personPlanSegmentsIT + 1) != personPlanSegments.end()) {
                    calculateSmoothPersonPlanConnection((personPlanSegmentsIT + 1)->personPlan, personPlanSegmentsIT->edge, (personPlanSegmentsIT + 1)->edge);
                }
            }
            // calculate entire shape, rotations and lengths
            myDemandElementSegmentGeometry.calculatePartialShapeRotationsAndLengths();
        }
        // mark demand element geometry as non-deprecated
        myDemandElementSegmentGeometry.geometryDeprecated = false;
    }
}


Position
GNEPerson::getPositionInView() const {
    // Position in view depend of first child element
    if (getDemandElementChildren().size() > 0) {
        if (getDemandElementChildren().at(0)->getTagProperty().isPersonStop()) {
            return getDemandElementChildren().at(0)->getDemandElementGeometry().shape.getLineCenter();
        } else {
            // obtain lane (special case for rides)
            SUMOVehicleClass vClassEdgeFrom = getDemandElementChildren().front()->getTagProperty().isRide() ? SVC_PASSENGER : SVC_PEDESTRIAN;
            GNELane* lane = getDemandElementChildren().at(0)->getEdgeParents().at(0)->getLaneByVClass(vClassEdgeFrom);
            // return position in view depending of lane
            if (lane->getGeometry().shape.length() < 2.5) {
                return lane->getGeometry().shape.front();
            } else {
                Position A = lane->getGeometry().shape.positionAtOffset(2.5);
                Position B = lane->getGeometry().shape.positionAtOffset(2.5);
                // return Middle point
                return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
            }
        }
    } else {
        return Position(0, 0);
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
    if (myViewNet->getNetworkViewOptions().showDemandElements() && myViewNet->getDemandViewOptions().showNonInspectedDemandElements(this) && (getDemandElementChildren().size() > 0)) {
        // obtain exaggeration (and add the special personExaggeration)
        const double exaggeration = s.personSize.getExaggeration(s, this, 80) + s.detailSettings.personExaggeration;
        // obtain width and length
        const double length = getDemandElementParents().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
        const double width = getDemandElementParents().at(0)->getAttributeDouble(SUMO_ATTR_WIDTH);
        const std::string file = getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_IMGFILE);
        // push GL ID
        glPushName(getGlID());
        // push draw matrix
        glPushMatrix();
        Position personPosition;
        // obtain position depending of first PersonPlan child
        if (getDemandElementChildren().front()->getTagProperty().isPersonStop()) {
            // obtain position of stop center
            personPosition = getDemandElementChildren().front()->getPositionInView();
        } else if (getDemandElementChildren().front()->getTagProperty().getTag() == SUMO_TAG_WALK_ROUTE) {
            // obtain position of first route's edge
            personPosition = getDemandElementChildren().front()->getDemandElementParents().at(1)->getEdgeParents().front()->getLanes().front()->getGeometry().shape.front();
        } else {
            // obtain position of first edge
            personPosition = getDemandElementChildren().front()->getEdgeParents().front()->getLanes().front()->getGeometry().shape.front();
        }
        glTranslated(personPosition.x(), personPosition.y(), getType());
        glRotated(90, 0, 0, 1);
        // set person color
        setColor(s);
        // set scale
        glScaled(exaggeration, exaggeration, 1);
        // draw person depending of detail level
        if (s.drawDetail(s.detailSettings.personShapes, exaggeration)) {
            GUIBasePersonHelper::drawAction_drawAsImage(0, length, width, file, SVS_PEDESTRIAN, exaggeration);
        } else if (s.drawDetail(s.detailSettings.personCircles, exaggeration)) {
            GUIBasePersonHelper::drawAction_drawAsCircle(length, width);
        } else if (s.drawDetail(s.detailSettings.personTriangles, exaggeration)) {
            GUIBasePersonHelper::drawAction_drawAsTriangle(0, length, width);
        }
        // pop matrix
        glPopMatrix();
        drawName(personPosition, s.scale, s.personName, s.angle);
        if (s.personValue.show) {
            Position personValuePosition = personPosition + Position(0, 0.6 * s.personName.scaledSize(s.scale));
            const double value = getColorValue(s, s.personColorer.getActive());
            GLHelper::drawTextSettings(s.personValue, toString(value), personValuePosition, s.scale, s.angle, GLO_MAX - getType());
        }
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            GLHelper::drawShapeDottedContourRectangle(s, getType(), personPosition, exaggeration, exaggeration);
        }
        // pop name
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
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
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


void
GNEPerson::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int newParametersSet = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_END: {
            // give more priority to end
            newParametersSet = VEHPARS_END_SET | VEHPARS_NUMBER_SET;
            break;
        }
        case SUMO_ATTR_NUMBER:
            newParametersSet ^= VEHPARS_END_SET;
            newParametersSet |= VEHPARS_NUMBER_SET;
            break;
        case SUMO_ATTR_VEHSPERHOUR: {
            // give more priority to end
            if ((newParametersSet & VEHPARS_END_SET) && (newParametersSet & VEHPARS_NUMBER_SET)) {
                newParametersSet = VEHPARS_END_SET;
            } else if (newParametersSet & VEHPARS_END_SET) {
                newParametersSet = VEHPARS_END_SET;
            } else if (newParametersSet & VEHPARS_NUMBER_SET) {
                newParametersSet = VEHPARS_NUMBER_SET;
            }
            // set VehsPerHour
            newParametersSet |= VEHPARS_VPH_SET;
            break;
        }
        case SUMO_ATTR_PERIOD: {
            // give more priority to end
            if ((newParametersSet & VEHPARS_END_SET) && (newParametersSet & VEHPARS_NUMBER_SET)) {
                newParametersSet = VEHPARS_END_SET;
            } else if (newParametersSet & VEHPARS_END_SET) {
                newParametersSet = VEHPARS_END_SET;
            } else if (newParametersSet & VEHPARS_NUMBER_SET) {
                newParametersSet = VEHPARS_NUMBER_SET;
            }
            // set period
            newParametersSet |= VEHPARS_PERIOD_SET;
            break;
        }
        case SUMO_ATTR_PROB: {
            // give more priority to end
            if ((newParametersSet & VEHPARS_END_SET) && (newParametersSet & VEHPARS_NUMBER_SET)) {
                newParametersSet = VEHPARS_END_SET;
            } else if (newParametersSet & VEHPARS_END_SET) {
                newParametersSet = VEHPARS_END_SET;
            } else if (newParametersSet & VEHPARS_NUMBER_SET) {
                newParametersSet = VEHPARS_NUMBER_SET;
            }
            // set probability
            newParametersSet |= VEHPARS_PROB_SET;
            break;
        }
        default:
            break;
    }
    // add GNEChange_EnableAttribute
    undoList->add(new GNEChange_EnableAttribute(this, myViewNet->getNet(), parametersSet, newParametersSet), true);
}


bool
GNEPerson::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
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
                for (const auto& i : via) {
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


std::string
GNEPerson::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEPerson::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEPerson::setGenericParametersStr(const std::string& value) {
    // clear parameters
    clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            setParameter(parsedParameters.front(), parsedParameters.back());
        }
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
GNEPerson::setEnabledAttribute(const int enabledAttributes) {
    parametersSet = enabledAttributes;
}


void
GNEPerson::calculateSmoothPersonPlanConnection(const GNEDemandElement* personPlanElement, const GNEEdge* edgeFrom, const GNEEdge* edgeTo) {
    // obtain lane from (special case due rides)
    SUMOVehicleClass vClassEdgeFrom = personPlanElement->getTagProperty().isRide() ? SVC_PASSENGER : SVC_PEDESTRIAN;
    GNELane* laneFrom = edgeFrom->getLaneByVClass(vClassEdgeFrom);
    // obtain lane to (special case due rides)
    SUMOVehicleClass vClassEdgeTo = personPlanElement->getTagProperty().isRide() ? SVC_PASSENGER : SVC_PEDESTRIAN;
    GNELane* laneTo = edgeTo->getLaneByVClass(vClassEdgeTo);
    // calculate smooth shape
    PositionVector smoothShape = edgeFrom->getNBEdge()->getToNode()->computeSmoothShape(
                                     laneFrom->getGeometry().shape, laneTo->getGeometry().shape,
                                     5, false,
                                     (double) 5. * (double) edgeFrom->getNBEdge()->getNumLanes(),
                                     (double) 5. * (double) edgeTo->getNBEdge()->getNumLanes());
    // add smootshape in personPlan shape
    for (const auto& i : smoothShape) {
        myDemandElementSegmentGeometry.insertJunctionSegment(personPlanElement, edgeTo->getGNEJunctionSource(), i, true, true);
    }
}


std::pair<PositionVector, PositionVector>
GNEPerson::calculatePersonPlanConnectionBusStop(GNELane* previousLane, GNEAdditional* busStop, GNELane* nextLane) {
    // declare a pair of PositionVectors to save result
    std::pair<PositionVector, PositionVector> result;
    if (previousLane) {
        // obtain first position values of busStop shape
        const Position& firstBusStopShapePosition = busStop->getAdditionalGeometry().shape.front();
        double offsetFirstPosition = previousLane->getGeometry().shape.nearest_offset_to_point2D(firstBusStopShapePosition, false);
        // split laneShape
        auto splittedFirstLaneShape = previousLane->getGeometry().shape.splitAt(offsetFirstPosition, true);
        // fill result position vector
        for (const auto& i : splittedFirstLaneShape.first) {
            result.first.push_back(i);
        }
        // finally add first BusStop shape position
        result.first.push_back(firstBusStopShapePosition);
    }
    if (nextLane) {
        // obtain second position of busStops
        const Position& lastBusStopShapePosition = busStop->getAdditionalGeometry().shape.back();
        double offsetLastPosition = nextLane->getGeometry().shape.nearest_offset_to_point2D(lastBusStopShapePosition, false);
        // split laneShape
        auto splittedLastLaneShape = nextLane->getGeometry().shape.splitAt(offsetLastPosition, true);
        // first add last BusStop shape position
        result.second.push_back(lastBusStopShapePosition);
        // fill result position vector
        for (const auto& i : splittedLastLaneShape.second) {
            result.second.push_back(i);
        }
    }
    return result;
}


std::pair<PositionVector, PositionVector>
GNEPerson::calculatePersonPlanConnectionStop(GNELane* previousLane, GNEDemandElement* stop, GNELane* nextLane) {
    // reuse calculatePersonPlanConnectionBusStop(...) if stop is placed over a busStop
    if (stop->getTagProperty().getTag() == SUMO_TAG_PERSONSTOP_BUSSTOP) {
        return calculatePersonPlanConnectionBusStop(previousLane, stop->getAdditionalParents().front(), nextLane);
    } else {
        // declare a pair of PositionVectors to save result
        std::pair<PositionVector, PositionVector> result;
        if (previousLane) {
            // split laneShape in start position
            auto splittedFirstLaneShape = previousLane->getGeometry().shape.splitAt(stop->getAttributeDouble(SUMO_ATTR_STARTPOS), true);
            // fill result position vector
            for (const auto& i : splittedFirstLaneShape.first) {
                result.first.push_back(i);
            }
            // finally add first Stop shape position
            result.first.push_back(stop->getDemandElementGeometry().shape.front());
        }
        if (nextLane) {
            // split laneShape in end position
            auto splittedLastLaneShape = nextLane->getGeometry().shape.splitAt(stop->getAttributeDouble(SUMO_ATTR_ENDPOS), true);
            // first add last Stop shape position
            result.second.push_back(stop->getDemandElementGeometry().shape.back());
            // fill result position vector
            for (const auto& i : splittedLastLaneShape.second) {
                result.second.push_back(i);
            }
        }
        return result;
    }
}


std::pair<PositionVector, PositionVector>
GNEPerson::calculatePersonPlanConnectionArrivalPos(GNELane* previousLane, double arrivalPosPersonPlan, GNELane* nextLane) {
    // check if both lanes are similar)
    if ((previousLane == nextLane) && (previousLane != nullptr)) {
        // split laneShape in arrivalPos
        return previousLane->getGeometry().shape.splitAt(arrivalPosPersonPlan, true);
    } else {
        // declare pair of position vectors
        std::pair<PositionVector, PositionVector> solution;
        // split previousLane in arrivalPos
        if (previousLane) {
            solution.first = previousLane->getGeometry().shape.splitAt(arrivalPosPersonPlan, true).first;
        }
        // split nextLane in arrivalPos
        if (nextLane) {
            solution.second = nextLane ->getGeometry().shape.splitAt(arrivalPosPersonPlan, true).second;
        }
        // return solution
        return solution;
    }
}

/****************************************************************************/
