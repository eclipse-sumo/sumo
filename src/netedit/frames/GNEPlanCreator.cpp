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
#include <netedit/elements/demand/GNEDemandElementPlan.h>
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


GNEPlanCreator::PlanPath::PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEEdge* toEdge) :
    myConflictVClass(false),
    myConflictDisconnected(false) {
    // calculate subpath using given vClass
    mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(vClass, fromEdge, toEdge);
    // if subPath is empty, try it with pedestrian (i.e. ignoring vCass)
    if (mySubPath.empty()) {
        mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(SVC_PEDESTRIAN, fromEdge, toEdge);
        if (mySubPath.empty()) {
            mySubPath = {fromEdge, toEdge};
            myConflictDisconnected = true;
        } else {
            myConflictVClass = true;
        }
    }
}


GNEPlanCreator::PlanPath::PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEJunction* toJunction) :
    myConflictVClass(false),
    myConflictDisconnected(false) {
    // calculate subpath using given vClass
    mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(vClass, fromEdge, toJunction);
    // if subPath is empty, try it with pedestrian (i.e. ignoring vCass)
    if (mySubPath.empty()) {
        mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(SVC_PEDESTRIAN, fromEdge, toJunction);
        if (mySubPath.empty()) {
            mySubPath = {fromEdge};
            myConflictDisconnected = true;
        } else {
            myConflictVClass = true;
        }
    }

}


GNEPlanCreator::PlanPath::PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEEdge* toEdge) :
    myConflictVClass(false),
    myConflictDisconnected(false) {
    // calculate subpath using given vClass
    mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(vClass, fromJunction, toEdge);
    // if subPath is empty, try it with pedestrian (i.e. ignoring vCass)
    if (mySubPath.empty()) {
        mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(SVC_PEDESTRIAN, fromJunction, toEdge);
        if (mySubPath.empty()) {
            mySubPath = {toEdge};
            myConflictDisconnected = true;
        } else {
            myConflictVClass = true;
        }
    }

}


GNEPlanCreator::PlanPath::PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEJunction* toJunction) :
    myConflictVClass(false),
    myConflictDisconnected(false) {
    // calculate subpath using the given vClass
    mySubPath = viewNet->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(vClass, fromJunction, toJunction);
    // if subPath is empty, try it with pedestrian (i.e. ignoring vCass)
    if (mySubPath.empty()) {
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
    myPlanParents(0) {
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
    myRemoveLastInsertedElement = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Remove last element"), "", "", nullptr, this, MID_GNE_PATHCREATOR_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedElement->disable();
    // create info label
    myInfoLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEPlanCreator::~GNEPlanCreator() {}


bool
GNEPlanCreator::planCanBeCreated(const GNEDemandElement* planTemplate) const {
    if (planTemplate == nullptr) {
        return false;
    } else if (planTemplate->getTagProperty().isPersonTrip()) {
        return GNEDemandElementPlan::getPersonTripTagIcon(myPlanParameteres).first != SUMO_TAG_NOTHING;
    } else if (planTemplate->getTagProperty().isPlanWalk()) {
        return GNEDemandElementPlan::getWalkTagIcon(myPlanParameteres).first != SUMO_TAG_NOTHING;
    } else if (planTemplate->getTagProperty().isPlanRide()) {
        return GNEDemandElementPlan::getRideTagIcon(myPlanParameteres).first != SUMO_TAG_NOTHING;
    } else if (planTemplate->getTagProperty().isPlanTransport()) {
        return GNEDemandElementPlan::getTransportTagIcon(myPlanParameteres).first != SUMO_TAG_NOTHING;
    } else if (planTemplate->getTagProperty().isPlanTranship()) {
        return GNEDemandElementPlan::getTranshipTagIcon(myPlanParameteres).first != SUMO_TAG_NOTHING;
    } else if (planTemplate->getTagProperty().isPlanStopPerson()) {
        return GNEDemandElementPlan::getPersonStopTagIcon(myPlanParameteres).first != SUMO_TAG_NOTHING;
    } else if (planTemplate->getTagProperty().isPlanStopContainer()) {
        return GNEDemandElementPlan::getContainerStopTagIcon(myPlanParameteres).first != SUMO_TAG_NOTHING;
    } else {
        return false;
    }
}


void
GNEPlanCreator::showPlanCreatorModule(const GNEPlanSelector* planSelector, const GNEDemandElement* previousPlan) {
    // first abort creation
    abortPathCreation();
    // hide creation buttons
    hideCreationButtons();
    // reset plan parents
    myPlanParents = 0;
    // set previous plan element
    myPreviousPlanElement = previousPlan;
    // get current plan template
    const auto& planTagProperties = planSelector->getCurrentPlanTagProperties();
    // continue depending of plan selector template
    if (planTagProperties.planRoute()) {
        myPlanParents |= ROUTE;
        // show use last inserted route
        myUseLastRoute->show();
    } else {
        // hide use last inserted route
        myUseLastRoute->hide();
    }
    if (planTagProperties.planEdge()) {
        myPlanParents |= EDGE;
    }
    if (planTagProperties.planBusStop()) {
        myPlanParents |= BUSSTOP;
    }
    if (planTagProperties.planTrainStop()) {
        myPlanParents |= TRAINSTOP;
    }
    if (planTagProperties.planContainerStop()) {
        myPlanParents |= CONTAINERSTOP;
    }
    if (planTagProperties.planConsecutiveEdges()) {
        myPlanParents |= CONSECUTIVE_EDGES;
        // show creation buttons
        showCreationButtons();
    }
    if (planTagProperties.planFromEdge() || planTagProperties.planToEdge()) {
        myPlanParents |= START_EDGE;
        myPlanParents |= END_EDGE;
        // show creation buttons
        showCreationButtons();
    }
    if (planTagProperties.planFromJunction() || planTagProperties.planToJunction()) {
        myPlanParents |= START_JUNCTION;
        myPlanParents |= END_JUNCTION;
        // show creation buttons
        showCreationButtons();
    }
    if (planTagProperties.planFromTAZ() || planTagProperties.planToTAZ()) {
        myPlanParents |= START_TAZ;
        myPlanParents |= END_TAZ;
        // show creation buttons
        showCreationButtons();
    }
    if (planTagProperties.planFromBusStop() || planTagProperties.planToBusStop()) {
        myPlanParents |= START_BUSSTOP;
        myPlanParents |= END_BUSSTOP;
        // show creation buttons
        showCreationButtons();
    }
    if (planTagProperties.planFromTrainStop() || planTagProperties.planToTrainStop()) {
        myPlanParents |= START_TRAINSTOP;
        myPlanParents |= END_TRAINSTOP;
        // show creation buttons
        showCreationButtons();
    }
    if (planTagProperties.planFromContainerStop() || planTagProperties.planToContainerStop()) {
        myPlanParents |= START_CONTAINERSTOP;
        myPlanParents |= END_CONTAINERSTOP;
        // show creation buttons
        showCreationButtons();
    }
    // update info label (after setting myPlanParents)
    updateInfoLabel();
    // check if add first element
    if (myPreviousPlanElement && planTagProperties.planFromTo()) {
        const auto previousTagProperty = myPreviousPlanElement->getTagProperty();
        // add last element of previous plan
        if (previousTagProperty.planToEdge() || previousTagProperty.planEdge()) {
            addFromToEdge(myPreviousPlanElement->getParentEdges().back());
        } else if (previousTagProperty.planToJunction()) {
            addFromToJunction(myPreviousPlanElement->getParentJunctions().back());
        } else if (previousTagProperty.planToTAZ()) {
            addFromToTAZ(myPreviousPlanElement->getParentAdditionals().back());
        } else if (previousTagProperty.planToStoppingPlace() || previousTagProperty.planStoppingPlace()) {
            addFromToStoppingPlace(myPreviousPlanElement->getParentAdditionals().back());
        }
    }
    // set vClass
    if (planTagProperties.isPlanRide() || planTagProperties.isPlanContainer()) {
        myVClass = SVC_PASSENGER;
    } else {
        myVClass = SVC_PEDESTRIAN;
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


bool
GNEPlanCreator::addRoute(GNEDemandElement* route) {
    // check if routes are allowed
    if ((myPlanParents & ROUTE) == 0) {
        return false;
    }
    // add edge
    myPlanParameteres.route = route;
    // create path
    return myFrameParent->createPath(false);
}


bool
GNEPlanCreator::addEdge(GNELane* lane) {
    // continue depending of plan parent
    if (myPlanParents & CONSECUTIVE_EDGES) {
        return addConsecutiveEdge(lane->getParentEdge());
    } else if (myPlanParents & EDGE) {
        return addSingleEdge(lane);
    } else if ((myPlanParents & START_EDGE) || (myPlanParents & END_EDGE)) {
        return addFromToEdge(lane->getParentEdge());
    } else {
        return false;
    }
}


bool
GNEPlanCreator::addJunction(GNEJunction* junction) {
    if ((myPlanParents & START_JUNCTION) || (myPlanParents & END_JUNCTION)) {
        return addFromToJunction(junction);
    } else {
        return false;
    }
}


bool
GNEPlanCreator::addTAZ(GNEAdditional* taz) {
    if ((myPlanParents & START_TAZ) || (myPlanParents & END_TAZ)) {
        return addFromToTAZ(taz);
    } else {
        return false;
    }
}


bool
GNEPlanCreator::addStoppingPlace(GNEAdditional* stoppingPlace) {
    // get stoppingPlace tag
    auto stoppingPlaceTag = stoppingPlace->getTagProperty().getTag();
    if ((stoppingPlaceTag == SUMO_TAG_BUS_STOP) && (myPlanParents & BUSSTOP)) {
        return addSingleStoppingPlace(stoppingPlace);
    } else if ((stoppingPlaceTag == SUMO_TAG_BUS_STOP) && ((myPlanParents & START_BUSSTOP) || (myPlanParents & END_BUSSTOP))) {
        return addFromToStoppingPlace(stoppingPlace);
    } else if ((stoppingPlaceTag == SUMO_TAG_TRAIN_STOP) && (myPlanParents & TRAINSTOP)) {
        return addSingleStoppingPlace(stoppingPlace);
    } else if ((stoppingPlaceTag == SUMO_TAG_TRAIN_STOP) && ((myPlanParents & START_TRAINSTOP) || (myPlanParents & END_TRAINSTOP))) {
        return addFromToStoppingPlace(stoppingPlace);
    } else if ((stoppingPlaceTag == SUMO_TAG_CONTAINER_STOP) && (myPlanParents & CONTAINERSTOP)) {
        return addSingleStoppingPlace(stoppingPlace);
    } else if ((stoppingPlaceTag == SUMO_TAG_CONTAINER_STOP) && ((myPlanParents & START_CONTAINERSTOP) || (myPlanParents & END_CONTAINERSTOP))) {
        return addFromToStoppingPlace(stoppingPlace);
    } else {
        return false;
    }
}


const GNEPlanParameters&
GNEPlanCreator::getPlanParameteres() const {
    return myPlanParameteres;
}


double
GNEPlanCreator::getClickedPositionOverLane() const {
    return myClickedPositionOverLane;
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
    } else if (myPlanParameteres.fromJunction && myPlanParameteres.toJunction) {
        // set color
        GLHelper::setColor(RGBColor::ORANGE);
        // get two points
        const Position posA = myPlanParameteres.fromJunction->getPositionInView();
        const Position posB = myPlanParameteres.toJunction->getPositionInView();
        const double rot = ((double)atan2((posB.x() - posA.x()), (posA.y() - posB.y())) * (double) 180.0 / (double)M_PI);
        const double len = posA.distanceTo2D(posB);
        // draw line
        GLHelper::drawBoxLine(posA, rot, len, 0.25);
    } else if (myPlanParameteres.fromTAZ && myPlanParameteres.toTAZ) {
        // set color
        GLHelper::setColor(RGBColor::ORANGE);
        // get two points
        const Position posA = myPlanParameteres.fromTAZ->getPositionInView();
        const Position posB = myPlanParameteres.toTAZ->getPositionInView();
        const double rot = ((double)atan2((posB.x() - posA.x()), (posA.y() - posB.y())) * (double) 180.0 / (double)M_PI);
        const double len = posA.distanceTo2D(posB);
        // draw line
        GLHelper::drawBoxLine(posA, rot, len, 0.25);
    }
    // Pop last matrix
    GLHelper::popMatrix();
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
        // update view (to see the new route)
        myFrameParent->getViewNet()->updateViewNet();
    }
}


void
GNEPlanCreator::removeLastElement() {
    if (myRemoveLastInsertedElement->isEnabled()) {
        if (myPlanParameteres.consecutiveEdges.size() > 0) {
            myPlanParameteres.consecutiveEdges.pop_back();
        } else if (myPlanParameteres.toEdge) {
            myPlanParameteres.toEdge = nullptr;
        } else if (myPlanParameteres.toJunction) {
            myPlanParameteres.toJunction = nullptr;
        } else if (myPlanParameteres.toTAZ) {
            myPlanParameteres.toTAZ = nullptr;
        } else if (myPlanParameteres.toStoppingPlace) {
            myPlanParameteres.toStoppingPlace = nullptr;
        } else if (myPlanParameteres.fromEdge) {
            myPlanParameteres.fromEdge = nullptr;
        } else if (myPlanParameteres.fromJunction) {
            myPlanParameteres.fromJunction = nullptr;
        } else if (myPlanParameteres.fromTAZ) {
            myPlanParameteres.fromTAZ = nullptr;
        } else if (myPlanParameteres.fromStoppingPlace) {
            myPlanParameteres.fromStoppingPlace = nullptr;
        }
        // update remove last item button
        updateRemoveLastItemButton();
        // recalculate path
        recalculatePath();
    }
}


long
GNEPlanCreator::onCmdCreatePath(FXObject*, FXSelector, void*) {
    // call create path
    return myFrameParent->createPath(false);
}


long
GNEPlanCreator::onCmdUseLastRoute(FXObject*, FXSelector, void*) {
    // call create path using last route
    return myFrameParent->createPath(true);
}


long
GNEPlanCreator::onUpdUseLastRoute(FXObject* sender, FXSelector, void*) {
    if ((myPlanParents & ROUTE) && myFrameParent->getViewNet()->getLastCreatedRoute()) {
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
GNEPlanCreator::clearPath() {
    // clear all elements
    myPlanParameteres.clear();
    myClickedPositionOverLane = 0;
    // clear path
    myPath.clear();
}


void
GNEPlanCreator::recalculatePath() {
    // first clear path
    myPath.clear();
    // continue depending of elements
    if (myPlanParameteres.consecutiveEdges.size() > 0) {
        // add every segment
        for (int i = 1; i < (int)myPlanParameteres.consecutiveEdges.size(); i++) {
            myPath.push_back(PlanPath(myFrameParent->getViewNet(), myVClass, myPlanParameteres.consecutiveEdges.at(i - 1), myPlanParameteres.consecutiveEdges.at(i)));
        }
    } else {
        // get from edge
        GNEEdge* fromEdge = nullptr;
        if (myPlanParameteres.fromEdge) {
            fromEdge = myPlanParameteres.fromEdge;
        } else if (myPlanParameteres.fromStoppingPlace) {
            fromEdge = myPlanParameteres.fromStoppingPlace->getParentLanes().front()->getParentEdge();
        }
        // get to edge
        GNEEdge* toEdge = nullptr;
        if (myPlanParameteres.toEdge) {
            toEdge = myPlanParameteres.toEdge;
        } else if (myPlanParameteres.toStoppingPlace) {
            toEdge = myPlanParameteres.toStoppingPlace->getParentLanes().front()->getParentEdge();
        }
        // continue depending of edges and junctions
        if (fromEdge && toEdge) {
            myPath.push_back(PlanPath(myFrameParent->getViewNet(), myVClass, fromEdge, toEdge));
        } else if (fromEdge && myPlanParameteres.toJunction) {
            myPath.push_back(PlanPath(myFrameParent->getViewNet(), myVClass, fromEdge, myPlanParameteres.toJunction));
        } else if (myPlanParameteres.fromJunction && toEdge) {
            myPath.push_back(PlanPath(myFrameParent->getViewNet(), myVClass, myPlanParameteres.fromJunction, toEdge));
        } else if (myPlanParameteres.fromJunction && myPlanParameteres.toJunction) {
            myPath.push_back(PlanPath(myFrameParent->getViewNet(), myVClass, myPlanParameteres.fromJunction, myPlanParameteres.toJunction));
        }
    }
}


int
GNEPlanCreator::getNumberOfSelectedElements() const {
    return (int)myPlanParameteres.consecutiveEdges.size() +
           (myPlanParameteres.fromEdge != nullptr ? 1 : 0) +
           (myPlanParameteres.toEdge != nullptr ? 1 : 0) +
           (myPlanParameteres.fromJunction != nullptr ? 1 : 0) +
           (myPlanParameteres.toJunction != nullptr ? 1 : 0) +
           (myPlanParameteres.fromTAZ != nullptr ? 1 : 0) +
           (myPlanParameteres.toTAZ != nullptr ? 1 : 0) +
           (myPlanParameteres.fromStoppingPlace != nullptr ? 1 : 0) +
           (myPlanParameteres.toStoppingPlace != nullptr ? 1 : 0);
}


void
GNEPlanCreator::updateRemoveLastItemButton() const {
    if (myPreviousPlanElement) {
        if (getNumberOfSelectedElements() == 2) {
            myRemoveLastInsertedElement->enable();
        } else {
            myRemoveLastInsertedElement->disable();
        }
    } else {
        if (getNumberOfSelectedElements() > 0) {
            myRemoveLastInsertedElement->enable();
        } else {
            myRemoveLastInsertedElement->disable();
        }
    }
}


void
GNEPlanCreator::showCreationButtons() {
    myFinishCreationButton->show();
    myAbortCreationButton->show();
    myRemoveLastInsertedElement->show();
}


void
GNEPlanCreator::hideCreationButtons() {
    myFinishCreationButton->hide();
    myAbortCreationButton->hide();
    myRemoveLastInsertedElement->hide();
}


void
GNEPlanCreator::updateInfoLabel() {
    // declare booleans
    const bool consecutiveEdges = (myPlanParents & CONSECUTIVE_EDGES);
    const bool route = (myPlanParents & ROUTE);
    const bool edges = (myPlanParents & EDGE) ||
                       (myPlanParents & START_EDGE) ||
                       (myPlanParents & END_EDGE);
    const bool TAZs = (myPlanParents & START_TAZ) ||
                      (myPlanParents & END_TAZ);
    const bool junctions = (myPlanParents & START_JUNCTION) ||
                           (myPlanParents & END_JUNCTION);
    const bool busStops = (myPlanParents & BUSSTOP) ||
                          (myPlanParents & START_BUSSTOP) ||
                          (myPlanParents & END_BUSSTOP);
    const bool trainStops = (myPlanParents & TRAINSTOP) ||
                            (myPlanParents & START_TRAINSTOP) ||
                            (myPlanParents & END_TRAINSTOP);
    const bool containerStops = (myPlanParents & CONTAINERSTOP) ||
                                (myPlanParents & START_CONTAINERSTOP) ||
                                (myPlanParents & END_CONTAINERSTOP);

    // declare ostringstream for label and fill it
    std::ostringstream information;
    information
            << TL("Click over:") << "\n"
            << (consecutiveEdges ? "- Consecutive edges\n" : "")
            << (route ? "- Routes\n" : "")
            << (edges ? "- Edges\n" : "")
            << (TAZs ? "- TAZs\n" : "")
            << (junctions ? "- Junctions\n" : "")
            << (busStops ? "- BusStops\n" : "")
            << (trainStops ? "- TrainStops\n" : "")
            << (containerStops ? "- ContainerStops\n" : "");
    // remove last \n
    std::string informationStr = information.str();
    informationStr.pop_back();
    // set label text
    myInfoLabel->setText(informationStr.c_str());
}


bool
GNEPlanCreator::addSingleEdge(GNELane* lane) {
    // add edge
    myPlanParameteres.fromEdge = lane->getParentEdge();
    // set position over lane
    const auto clickedPos = myFrameParent->getViewNet()->getPositionInformation();
    myClickedPositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(clickedPos);
    // create path
    return myFrameParent->createPath(false);
}


bool
GNEPlanCreator::addSingleStoppingPlace(GNEAdditional* stoppingPlace) {
    // add edge
    myPlanParameteres.stoppingPlace = stoppingPlace;
    // create path
    return myFrameParent->createPath(false);
}


bool
GNEPlanCreator::addConsecutiveEdge(GNEEdge* edge) {
    // check double edges
    if ((myPlanParameteres.consecutiveEdges.size() > 0) && (myPlanParameteres.consecutiveEdges.back() == edge)) {
        // Write warning
        WRITE_WARNING(TL("Double edges aren't allowed"));
        // abort add edge
        return false;
    }
    // All checks ok, then add it in selected elements
    myPlanParameteres.consecutiveEdges.push_back(edge);
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // update remove last item button
    updateRemoveLastItemButton();
    // recalculate path
    recalculatePath();
    // edge added, then return true
    return true;
}


bool
GNEPlanCreator::addFromToJunction(GNEJunction* junction) {
    // avoid double junctions
    if (myPlanParameteres.fromJunction && (myPlanParameteres.fromJunction == junction)) {
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
        myPlanParameteres.fromJunction = junction;
    } else {
        myPlanParameteres.toJunction = junction;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // update remove last item button
    updateRemoveLastItemButton();
    // recalculate path
    recalculatePath();
    return true;
}


bool
GNEPlanCreator::addFromToTAZ(GNEAdditional* TAZ) {
    // avoid double TAZs
    if (myPlanParameteres.fromTAZ && (myPlanParameteres.fromTAZ == TAZ)) {
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
        myPlanParameteres.fromTAZ = TAZ;
    } else {
        myPlanParameteres.toTAZ = TAZ;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // update remove last item button
    updateRemoveLastItemButton();
    // recalculate path
    recalculatePath();
    return true;
}


bool
GNEPlanCreator::addFromToEdge(GNEEdge* edge) {
    // check double edges
    if (myPlanParameteres.fromEdge && (myPlanParameteres.fromEdge == edge)) {
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
        myPlanParameteres.fromEdge = edge;
    } else {
        myPlanParameteres.toEdge = edge;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // update remove last item button
    updateRemoveLastItemButton();
    // recalculate path
    recalculatePath();
    // edge added, then return true
    return true;
}


bool
GNEPlanCreator::addFromToStoppingPlace(GNEAdditional* stoppingPlace) {
    // check double stoppingPlaces
    if (myPlanParameteres.fromStoppingPlace && (myPlanParameteres.fromStoppingPlace == stoppingPlace)) {
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
        myPlanParameteres.fromStoppingPlace = stoppingPlace;
    } else {
        myPlanParameteres.toStoppingPlace = stoppingPlace;
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
    // enable or disable remove last item button
    updateRemoveLastItemButton();
    // recalculate path
    recalculatePath();
    // stopping place added, then return true
    return true;
}

/****************************************************************************/
