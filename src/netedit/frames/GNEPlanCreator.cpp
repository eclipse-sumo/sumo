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
/// @file    GNEPlanCreator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for create paths
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEPlanCreator.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPlanCreator) PathCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_PATHCREATOR_ABORT,           GNEPlanCreator::onCmdAbortPathCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_PATHCREATOR_FINISH,          GNEPlanCreator::onCmdCreatePath),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_PATHCREATOR_USELASTROUTE,    GNEPlanCreator::onCmdUseLastRoute),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_PATHCREATOR_USELASTROUTE,    GNEPlanCreator::onUpdUseLastRoute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_PATHCREATOR_REMOVELAST,      GNEPlanCreator::onCmdRemoveLastElement)
};

// Object implementation
FXIMPLEMENT(GNEPlanCreator,                MFXGroupBoxModule,     PathCreatorMap,                 ARRAYNUMBER(PathCreatorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEPlanCreator::PlanPath::PlanPath(const SUMOVehicleClass vClass, GNEEdge* edge) :
    mySubPath({edge}),
    myConflictVClass(false),
    myConflictDisconnected(false) {
    // check if we have to change vClass flag
    if (edge->getNBEdge()->getNumLanesThatAllow(vClass) == 0) {
        myConflictVClass = true;
    }
}


GNEPlanCreator::PlanPath::PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* edgeFrom, GNEEdge* edgeTo) :
    myConflictVClass(false),
    myConflictDisconnected(false) {
    // calculate subpath
    mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(vClass, {edgeFrom, edgeTo});
    // if subPath is empty, try it with pedestrian (i.e. ignoring vCass)
    if (mySubPath.empty()) {
        mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(SVC_PEDESTRIAN, {edgeFrom, edgeTo});
        if (mySubPath.empty()) {
            mySubPath = { edgeFrom, edgeTo };
            myConflictDisconnected = true;
        } else {
            myConflictVClass = true;
        }
    }
}


GNEPlanCreator::PlanPath::PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEJunction* junctionFrom, GNEJunction* junctionTo) :
    myConflictVClass(false),
    myConflictDisconnected(false) {
    // calculate subpath
    mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(vClass, junctionFrom, junctionTo);
    // if subPath is empty, try it with pedestrian (i.e. ignoring vCass)
    if (mySubPath.empty()) {
        mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(SVC_PEDESTRIAN, junctionFrom, junctionTo);
        if (mySubPath.empty()) {
            myConflictDisconnected = true;
        } else {
            myConflictVClass = true;
        }
    }
}


const std::vector<GNEEdge*>&
GNEPlanCreator::PlanPath::getSubPath() const {
    return mySubPath;
}


bool
GNEPlanCreator::PlanPath::isConflictVClass() const {
    return myConflictVClass;
}


bool
GNEPlanCreator::PlanPath::isConflictDisconnected() const {
    return myConflictDisconnected;
}


GNEPlanCreator::PlanPath::PlanPath() :
    myConflictVClass(false),
    myConflictDisconnected(false) {
}


GNEPlanCreator::GNEPlanCreator(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Route creator")),
    myFrameParent(frameParent),
    myVClass(SVC_PASSENGER),
    myCreationMode(0) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(getCollapsableFrame(), TL("No edges selected"), 0, GUIDesignLabelFrameInformation);
    // create button for use last route
    myUseLastRoute = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Use last route"), "", "", GUIIconSubSys::getIcon(GUIIcon::ROUTE), this, MID_GNE_PATHCREATOR_USELASTROUTE, GUIDesignButton);
    myUseLastRoute->disable();
    // create button for finish route creation
    myFinishCreationButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Finish route creation"), "", "", nullptr, this, MID_GNE_PATHCREATOR_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Abort route creation"), "", "", nullptr, this, MID_GNE_PATHCREATOR_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted edge
    myRemoveLastInsertedElement = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Remove last edge"), "", "", nullptr, this, MID_GNE_PATHCREATOR_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedElement->disable();
    // create backspace label (always shown)
    myBackSpaceLabel = new FXLabel(this, "BACKSPACE: undo click", 0, GUIDesignLabelFrameInformation);
}


GNEPlanCreator::~GNEPlanCreator() {}


void
GNEPlanCreator::showPlanCreatorModule(const GNEPlanSelector* planSelector, const GNEDemandElement *previousPlan) {
    // first abort creation
    abortPathCreation();
    // hide use last inserted route
    myUseLastRoute->hide();
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedElement->disable();
    // show info label
    myInfoRouteLabel->show();
    myBackSpaceLabel->show();
    // reset creation mode
    myCreationMode = 0;
    // set previous plan element
    myPreviousPlanElement = previousPlan;
    // continue depending of planSelector
    if (planSelector->markContinuousEdges()) {
        myCreationMode |= CONSECUTIVE_EDGES;
    } else if (planSelector->markRoutes()) {
        myCreationMode |= ROUTE;
        // show use last inserted route
        myUseLastRoute->show();
        // hide other buttons and labels
        myFinishCreationButton->hide();
        myAbortCreationButton->hide();
        myRemoveLastInsertedElement->hide();
        myInfoRouteLabel->hide();
        myBackSpaceLabel->hide();
    } else {
        if (planSelector->markSingleEdges()) {
            myCreationMode |= START_EDGE;
            myCreationMode |= END_EDGE;
        }
        if (planSelector->markJunctions()) {
            myCreationMode |= START_JUNCTION;
            myCreationMode |= END_JUNCTION;
        }
        if (planSelector->markTAZs()) {
            myCreationMode |= START_TAZ;
            myCreationMode |= END_TAZ;
        }
        if (planSelector->markBusStops()) {
            myCreationMode |= START_BUSSTOP;
            myCreationMode |= END_BUSSTOP;
        }
        if (planSelector->markTrainStops()) {
            myCreationMode |= START_TRAINSTOP;
            myCreationMode |= END_TRAINSTOP;
        }
    }
    // check if add first element
    if (myPreviousPlanElement) {
        const auto tagProperty = myPreviousPlanElement->getTagProperty();
        // add last element of previous plan
        if (tagProperty.planToEdge()) {
            addEdge(myPreviousPlanElement->getParentEdges().back());
        } else if (tagProperty.planToJunction()) {
            addJunction(myPreviousPlanElement->getParentJunctions().back());
        } else if (tagProperty.planToTAZ()) {
            addTAZ(myPreviousPlanElement->getParentAdditionals().back());
        } else if (tagProperty.planToStoppingPlace()) {
            addStoppingPlace(myPreviousPlanElement->getParentAdditionals().back());
        }
    }
    // recalc before show (to avoid graphic problems)
    recalc();
    // show modul
    show();
}


void
GNEPlanCreator::hidePathCreatorModule() {
    // clear path
    clearPath();
    // hide modul
    hide();
}


SUMOVehicleClass
GNEPlanCreator::getVClass() const {
    return myVClass;
}


void
GNEPlanCreator::setVClass(SUMOVehicleClass vClass) {
    myVClass = vClass;
}


bool
GNEPlanCreator::addJunction(GNEJunction* junction) {
    // check if junctions are allowed
    if (((myCreationMode & START_JUNCTION) == 0) && ((myCreationMode & END_JUNCTION) == 0)) {
        return false;
    }
    // avoid double junctions
    if (myFromJunction && (myFromJunction == junction)) {
        // Write warning
        WRITE_WARNING(TL("Double junctions aren't allowed"));
        // abort add junction
        return false;
    }
    // check number of selected items
    if (getNumberOfSelectedElements() == 2) {
        // Write warning
        WRITE_WARNING(TL("Only two from-to elements are allowed"));
        // abort add function
        return false;
    }
    // set junction
    if (getNumberOfSelectedElements() == 0) {
        myFromJunction = junction;
    } else {
        myToJunction = junction;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo(TL("route creation"));
    // enable or disable remove last junction button
    if (checkEnableLastItemButton()) {
        myRemoveLastInsertedElement->enable();
    } else {
        myRemoveLastInsertedElement->disable();
    }
    // recalculate path
    recalculatePath();
    // update info route label
    updateInfoRouteLabel();
    return true;
}


bool
GNEPlanCreator::addTAZ(GNEAdditional* TAZ) {
    // check if TAZs are allowed
    if (((myCreationMode & START_JUNCTION) == 0) && ((myCreationMode & END_JUNCTION) == 0)) {
        return false;
    }
    // avoid double TAZs
    if (myFromTAZ && (myFromTAZ == TAZ)) {
        // Write warning
        WRITE_WARNING(TL("Double TAZs aren't allowed"));
        // abort add TAZ
        return false;
    }
    // check number of selected items
    if (getNumberOfSelectedElements() == 2) {
        // Write warning
        WRITE_WARNING(TL("Only two from-to elements are allowed"));
        // abort add function
        return false;
    }
    // set TAZ
    if (getNumberOfSelectedElements() == 0) {
        myFromTAZ = TAZ;
    } else {
        myToTAZ = TAZ;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo(TL("route creation"));
    // enable or disable remove last TAZ button
    if (checkEnableLastItemButton()) {
        myRemoveLastInsertedElement->enable();
    } else {
        myRemoveLastInsertedElement->disable();
    }
    // recalculate path
    recalculatePath();
    // update info route label
    updateInfoRouteLabel();
    return true;
}


bool
GNEPlanCreator::addConsecutiveEdge(GNEEdge* edge) {
    // check if edges are allowed
    if ((myCreationMode & CONSECUTIVE_EDGES) == 0) {
        return false;
    }
    // check double edges
    if ((myConsecutiveEdges.size() > 0) && (myConsecutiveEdges.back() == edge)) {
        // Write warning
        WRITE_WARNING(TL("Double edges aren't allowed"));
        // abort add edge
        return false;
    }
    // All checks ok, then add it in selected elements
    myConsecutiveEdges.push_back(edge);
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo(TL("route creation"));
    // enable or disable remove last edge button
    if (checkEnableLastItemButton()) {
        myRemoveLastInsertedElement->enable();
    } else {
        myRemoveLastInsertedElement->disable();
    }
    // recalculate path
    recalculatePath();
    // update info route label
    updateInfoRouteLabel();
    // edge added, then return true
    return true;
}


bool
GNEPlanCreator::addEdge(GNEEdge* edge) {
    // check if edges are allowed
    if (((myCreationMode & START_EDGE) == 0) && ((myCreationMode & END_EDGE) == 0)) {
        return false;
    }
    // check double edges
    if (myFromEdge && (myFromEdge == edge)) {
        // Write warning
        WRITE_WARNING(TL("Double edges aren't allowed"));
        // abort add edge
        return false;
    }
    // check number of selected items
    if (getNumberOfSelectedElements() == 2) {
        // Write warning
        WRITE_WARNING(TL("Only two from-to elements are allowed"));
        // abort add function
        return false;
    }
    // set edge
    if (getNumberOfSelectedElements() == 0) {
        myFromEdge = edge;
    } else {
        myToEdge = edge;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo(TL("route creation"));
    // enable or disable remove last edge button
    if (checkEnableLastItemButton()) {
        myRemoveLastInsertedElement->enable();
    } else {
        myRemoveLastInsertedElement->disable();
    }
    // recalculate path
    recalculatePath();
    // update info route label
    updateInfoRouteLabel();
    // edge added, then return true
    return true;
}


bool
GNEPlanCreator::addStoppingPlace(GNEAdditional* stoppingPlace) {
    if (stoppingPlace == nullptr) {
        return false;
    }
    // check if stoppingPlaces are allowed
    if (((myCreationMode & START_BUSSTOP) == 0) && ((myCreationMode & END_BUSSTOP) == 0) &&
        ((myCreationMode & START_TRAINSTOP) == 0) && ((myCreationMode & END_TRAINSTOP) == 0)) {
        return false;
    }
    // check double stoppingPlaces
    if (myFromStoppingPlace && (myFromStoppingPlace == stoppingPlace)) {
        // Write warning
        WRITE_WARNING(TL("Double stoppingPlaces aren't allowed"));
        // abort add stopping place
        return false;
    }
    // check number of selected items
    if (getNumberOfSelectedElements() == 2) {
        // Write warning
        WRITE_WARNING(TL("Only two from-to elements are allowed"));
        // abort add function
        return false;
    }
    // add stoppingPlace
    if (getNumberOfSelectedElements() == 0) {
        myFromStoppingPlace = stoppingPlace;
    } else {
        myToStoppingPlace = stoppingPlace;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
    // enable or disable remove last stoppingPlace button
    if (getNumberOfSelectedElements() == 1) {
        myRemoveLastInsertedElement->enable();
    } else {
        myRemoveLastInsertedElement->disable();
    }
    // recalculate path
    recalculatePath();
    // update info route label
    updateInfoRouteLabel();
    // stopping place added, then return true
    return true;
}


bool
GNEPlanCreator::addRoute(GNEDemandElement* route) {
    if (route == nullptr) {
        return false;
    }
    // check if routes are allowed
    if (((myCreationMode & START_BUSSTOP) == 0) && ((myCreationMode & END_BUSSTOP) == 0) &&
        ((myCreationMode & START_TRAINSTOP) == 0) && ((myCreationMode & END_TRAINSTOP) == 0)) {
        return false;
    }
    // check double routes
    if (myFromRoute && (myFromRoute == route)) {
        // Write warning
        WRITE_WARNING(TL("Double routes aren't allowed"));
        // abort add route
        return false;
    }
    // check number of selected items
    if (getNumberOfSelectedElements() == 2) {
        // Write warning
        WRITE_WARNING(TL("Only two from-to elements are allowed"));
        // abort add function
        return false;
    }
    // add route
    myRoute = route;
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
    // enable or disable remove last route button
    if (getNumberOfSelectedElements() == 1) {
        myRemoveLastInsertedElement->enable();
    } else {
        myRemoveLastInsertedElement->disable();
    }
    // recalculate path
    recalculatePath();
    // update info route label
    updateInfoRouteLabel();
    // stopping place added, then return true
    return true;
}


const std::vector<std::string>
GNEPlanCreator::getConsecutiveEdgeIDs() const {
    std::vector<std::string> edgeIDs;
    for (const auto &edge : myConsecutiveEdges) {
        edgeIDs.push_back(edge->getID());
    }
    return edgeIDs;
}


GNEEdge*
GNEPlanCreator::getFromEdge() const {
    return myFromEdge;
}


GNEEdge*
GNEPlanCreator::getToEdge() const {
    return myToEdge;
}


GNEJunction*
GNEPlanCreator::getFromJunction() const {
    return myFromJunction;
}


GNEJunction*
GNEPlanCreator::getToJunction() const {
    return myToJunction;
}


GNEAdditional*
GNEPlanCreator::getFromTAZ() const {
    return myFromTAZ;
}


GNEAdditional*
GNEPlanCreator::getToTAZ() const {
    return myToTAZ;
}


GNEAdditional*
GNEPlanCreator::getFromBusStop() const {
    if (myFromStoppingPlace && (myFromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return myFromStoppingPlace;
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNEPlanCreator::getToBusStop() const {
    if (myToStoppingPlace && (myToStoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return myToStoppingPlace;
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNEPlanCreator::getFromTrainStop() const {
    if (myFromStoppingPlace && (myFromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP)) {
        return myFromStoppingPlace;
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNEPlanCreator::getToTrainStop() const {
    if (myFromStoppingPlace && (myFromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP)) {
        return myFromStoppingPlace;
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNEPlanCreator::getFromContainerStop() const {
    if (myFromStoppingPlace && (myFromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP)) {
        return myFromStoppingPlace;
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNEPlanCreator::getToContainerStop() const {
    if (myFromStoppingPlace && (myFromStoppingPlace->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP)) {
        return myFromStoppingPlace;
    } else {
        return nullptr;
    }
}


GNEDemandElement*
GNEPlanCreator::getRoute() const {
    return myRoute;
}


const std::vector<GNEPlanCreator::PlanPath>&
GNEPlanCreator::getPath() const {
    return myPath;
}


void
GNEPlanCreator::drawTemporalRoute(const GUIVisualizationSettings& s) const {
    const double lineWidth = 0.35;
    const double lineWidthin = 0.25;
    // Add a draw matrix
    GLHelper::pushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, GLO_MAX - 0.1);
    // check if draw bewteen junction or edges
    if (myPath.size() > 0) {
        // set first color
        GLHelper::setColor(RGBColor::GREY);
        // iterate over path
        for (int i = 0; i < (int)myPath.size(); i++) {
            // get path
            const GNEPlanCreator::PlanPath& path = myPath.at(i);
            // draw line over
            for (int j = 0; j < (int)path.getSubPath().size(); j++) {
                const GNELane* lane = path.getSubPath().at(j)->getLanes().back();
                if (((i == 0) && (j == 0)) || (j > 0)) {
                    GLHelper::drawBoxLines(lane->getLaneShape(), lineWidth);
                }
                // draw connection between lanes
                if ((j + 1) < (int)path.getSubPath().size()) {
                    const GNELane* nextLane = path.getSubPath().at(j + 1)->getLanes().back();
                    if (lane->getLane2laneConnections().exist(nextLane)) {
                        GLHelper::drawBoxLines(lane->getLane2laneConnections().getLane2laneGeometry(nextLane).getShape(), lineWidth);
                    } else {
                        GLHelper::drawBoxLines({lane->getLaneShape().back(), nextLane->getLaneShape().front()}, lineWidth);
                    }
                }
            }
        }
        glTranslated(0, 0, 0.1);
        // iterate over path again
        for (int i = 0; i < (int)myPath.size(); i++) {
            // get path
            const GNEPlanCreator::PlanPath& path = myPath.at(i);
            // set path color color
            if (path.isConflictDisconnected()) {
                GLHelper::setColor(s.candidateColorSettings.conflict);
            } else if (path.isConflictVClass()) {
                GLHelper::setColor(s.candidateColorSettings.special);
            } else {
                GLHelper::setColor(RGBColor::ORANGE);
            }
            // draw line over
            for (int j = 0; j < (int)path.getSubPath().size(); j++) {
                const GNELane* lane = path.getSubPath().at(j)->getLanes().back();
                if (((i == 0) && (j == 0)) || (j > 0)) {
                    GLHelper::drawBoxLines(lane->getLaneShape(), lineWidthin);
                }
                // draw connection between lanes
                if ((j + 1) < (int)path.getSubPath().size()) {
                    const GNELane* nextLane = path.getSubPath().at(j + 1)->getLanes().back();
                    if (lane->getLane2laneConnections().exist(nextLane)) {
                        GLHelper::drawBoxLines(lane->getLane2laneConnections().getLane2laneGeometry(nextLane).getShape(), lineWidthin);
                    } else {
                        GLHelper::drawBoxLines({ lane->getLaneShape().back(), nextLane->getLaneShape().front() }, lineWidthin);
                    }
                }
            }
        }
    } else if (myFromJunction && myToJunction) {
        // set color
        GLHelper::setColor(RGBColor::ORANGE);
        // get two points
        const Position posA = myFromJunction->getPositionInView();
        const Position posB = myToJunction->getPositionInView();
        const double rot = ((double)atan2((posB.x() - posA.x()), (posA.y() - posB.y())) * (double) 180.0 / (double)M_PI);
        const double len = posA.distanceTo2D(posB);
        // draw line
        GLHelper::drawBoxLine(posA, rot, len, 0.25);
    } else if (myFromTAZ && myToTAZ) {
        // set color
        GLHelper::setColor(RGBColor::ORANGE);
        // get two points
        const Position posA = myFromTAZ->getPositionInView();
        const Position posB = myToTAZ->getPositionInView();
        const double rot = ((double)atan2((posB.x() - posA.x()), (posA.y() - posB.y())) * (double) 180.0 / (double)M_PI);
        const double len = posA.distanceTo2D(posB);
        // draw line
        GLHelper::drawBoxLine(posA, rot, len, 0.25);
    }
    // Pop last matrix
    GLHelper::popMatrix();
}


bool
GNEPlanCreator::createPath(const bool useLastRoute) {
    // call create path implemented in frame parent
    return myFrameParent->createPath(useLastRoute);
}


void
GNEPlanCreator::abortPathCreation() {
    // first check that there is elements
    if (getNumberOfSelectedElements() > 0) {
        // unblock undo/redo
        myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->enableUndoRedo();
        // clear edges
        clearPath();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedElement->disable();
        // update info route label
        updateInfoRouteLabel();
        // update view (to see the new route)
        myFrameParent->getViewNet()->updateViewNet();
    }
}


void
GNEPlanCreator::removeLastElement() {

// check

    if (myConsecutiveEdges.size() > 1) {
        // remove special color of last selected edge
        myConsecutiveEdges.back()->resetCandidateFlags();
        // remove last edge
        myConsecutiveEdges.pop_back();
        // enable or disable remove last edge button
        if (myConsecutiveEdges.size() > 1) {
            myRemoveLastInsertedElement->enable();
        } else {
            myRemoveLastInsertedElement->disable();
        }
        // recalculate path
        recalculatePath();
        // update info route label
        updateInfoRouteLabel();
        // update view
        myFrameParent->getViewNet()->updateViewNet();
    }
}


long
GNEPlanCreator::onCmdCreatePath(FXObject*, FXSelector, void*) {
    // call create path
    return createPath(false);
}


long
GNEPlanCreator::onCmdUseLastRoute(FXObject*, FXSelector, void*) {
    // call create path with useLastRoute = true
    return createPath(true);
}

long
GNEPlanCreator::onUpdUseLastRoute(FXObject* sender, FXSelector, void*) {
    if ((myCreationMode & ROUTE) && myFrameParent->getViewNet()->getLastCreatedRoute()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

long
GNEPlanCreator::onCmdAbortPathCreation(FXObject*, FXSelector, void*) {
    // just call abort path creation
    abortPathCreation();
    return 1;
}


long
GNEPlanCreator::onCmdRemoveLastElement(FXObject*, FXSelector, void*) {
    // just call remove last element
    removeLastElement();
    return 1;
}


void
GNEPlanCreator::updateInfoRouteLabel() {
    if (myPath.size() > 0) {
        // declare variables for route info
        double length = 0;
        double speed = 0;
        int pathSize = 0;
        for (const auto& path : myPath) {
            for (const auto& edge : path.getSubPath()) {
                length += edge->getNBEdge()->getLength();
                speed += edge->getNBEdge()->getSpeed();
            }
            pathSize += (int)path.getSubPath().size();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
                << TL("- Selected edges: ") << toString(myConsecutiveEdges.size()) << "\n"
                << TL("- Path edges: ") << toString(pathSize) << "\n"
                << TL("- Length: ") << toString(length) << "\n"
                << TL("- Average speed: ") << toString(speed / pathSize);
        // set new label
        myInfoRouteLabel->setText(information.str().c_str());
    } else {
        myInfoRouteLabel->setText(TL("No edges selected"));
    }
}


void
GNEPlanCreator::clearPath() {
    // clear junction, TAZs, edges, additionals and route
    myConsecutiveEdges.clear();
    myFromEdge = nullptr;
    myToEdge = nullptr;
    myFromJunction = nullptr;
    myToJunction = nullptr;
    myFromTAZ = nullptr;
    myToTAZ = nullptr;
    myFromStoppingPlace = nullptr;
    myToStoppingPlace = nullptr;
    myFromRoute = nullptr;
    myRoute = nullptr;
    // clear path
    myPath.clear();
    // update info route label
    updateInfoRouteLabel();
}


void
GNEPlanCreator::recalculatePath() {
    // first clear path
    myPath.clear();
    // case for plan between junctions
    if (myFromJunction && myToJunction) {
        // add path between two junctions
        myPath.push_back(PlanPath(myFrameParent->getViewNet(), myVClass, myFromJunction, myToJunction));
    } else {
        // set edges
        std::vector<GNEEdge*> edges;
        // obtain from edge
        if (myFromRoute) {
            edges.push_back(myFromRoute->getParentEdges().back());
        } else if (myFromStoppingPlace) {
            edges.push_back(myFromStoppingPlace->getParentLanes().front()->getParentEdge());
        } else if (myFromEdge) {
            edges.push_back(myFromEdge);
        }
        // add consecutive edges
        for (const auto &edge : myConsecutiveEdges) {
            edges.push_back(edge);
        }
        // add route edges
        if (myRoute) {
            for (const auto &edge : myFromRoute->getParentEdges()) {
                edges.push_back(edge);
            }
        } else if (myFromStoppingPlace) {
            edges.push_back(myFromStoppingPlace->getParentLanes().front()->getParentEdge());
        } else if (myToEdge) {
            edges.push_back(myToEdge);
        }
        // fill paths
        if (edges.size() == 1) {
            myPath.push_back(PlanPath(myVClass, edges.front()));
        } else {
            // add every segment
            for (int i = 1; i < (int)edges.size(); i++) {
                myPath.push_back(PlanPath(myFrameParent->getViewNet(), myVClass, edges.at(i - 1), edges.at(i)));
            }
        }
    }
}


void
GNEPlanCreator::setSpecialCandidates(GNEEdge* originEdge) {
    // first calculate reachability for pedestrians (we use it, because pedestran can walk in almost all edges)
    myFrameParent->getViewNet()->getNet()->getPathManager()->getPathCalculator()->calculateReachability(SVC_PEDESTRIAN, originEdge);
    // change flags
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getLanes()) {
            if (lane->getReachability() > 0) {
                lane->getParentEdge()->resetCandidateFlags();
                lane->getParentEdge()->setSpecialCandidate(true);
            }
        }
    }
}

void
GNEPlanCreator::setPossibleCandidates(GNEEdge* originEdge, const SUMOVehicleClass vClass) {
    // first calculate reachability for pedestrians
    myFrameParent->getViewNet()->getNet()->getPathManager()->getPathCalculator()->calculateReachability(vClass, originEdge);
    // change flags
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getLanes()) {
            if (lane->getReachability() > 0) {
                lane->getParentEdge()->resetCandidateFlags();
                lane->getParentEdge()->setPossibleCandidate(true);
            }
        }
    }
}


int
GNEPlanCreator::getNumberOfSelectedElements() const {
    return (int)myConsecutiveEdges.size() +
           (myFromEdge != nullptr ? 1 : 0) +
           (myToEdge != nullptr ? 1 : 0) +
           (myFromJunction != nullptr ? 1 : 0) +
           (myToJunction != nullptr ? 1 : 0) +
           (myFromTAZ != nullptr ? 1 : 0) +
           (myToTAZ != nullptr ? 1 : 0) +
           (myFromStoppingPlace != nullptr ? 1 : 0) +
           (myToStoppingPlace != nullptr ? 1 : 0) +
           (myFromRoute != nullptr ? 1 : 0) +
           (myRoute != nullptr ? 1 : 0);
}


bool
GNEPlanCreator::checkEnableLastItemButton() const {
    if (myPreviousPlanElement) {
        return getNumberOfSelectedElements() == 2;
    } else {
        return getNumberOfSelectedElements() > 0;
    }
}

/****************************************************************************/
