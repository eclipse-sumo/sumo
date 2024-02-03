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
/// @file    GNEPlanCreator.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for create paths
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEPlanSelector.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPlanCreator : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEPlanCreator)

public:
    /// @brief class for path
    class PlanPath {

    public:
        /// @brief constructor for from-to edges
        PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEEdge* toEdge);

        /// @brief constructor for from edge and to junction
        PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEJunction* toJunction);

        /// @brief constructor for from junction and to edge
        PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEEdge* toEdge);

        /// @brief constructor for from-to edges
        PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEJunction* toJunction);

        /// @brief get sub path
        const std::vector<GNEEdge*>& getSubPath() const;

        /// @brief check if current path is conflict due vClass
        bool isConflictVClass() const;

        /// @brief check if current path is conflict due is disconnected
        bool isConflictDisconnected() const;

    protected:
        /// @brief sub path
        std::vector<GNEEdge*> mySubPath;

        /// @brief flag to mark this path as conflicted
        bool myConflictVClass;

        /// @brief flag to mark this path as disconnected
        bool myConflictDisconnected;

    private:
        /// @brief default constructor
        PlanPath();

        /// @brief Invalidated copy constructor.
        PlanPath(PlanPath*) = delete;

        /// @brief Invalidated assignment operator.
        PlanPath& operator=(PlanPath*) = delete;
    };

    /// @brief default constructor
    GNEPlanCreator(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEPlanCreator();

    /// @brief check if plan can be created
    bool planCanBeCreated(const GNEDemandElement* planTemplate) const;

    /// @brief show plan creator for the given tag property
    void showPlanCreatorModule(const GNEPlanSelector* planSelector, const GNEDemandElement* previousPlan);

    /// @brief show GNEPlanCreator
    void hidePathCreatorModule();

    /// @brief add route
    bool addRoute(GNEDemandElement* route);

    /// @brief add edge (clicking over lanes)
    bool addEdge(GNELane* lane);

    /// @brief add junction
    bool addJunction(GNEJunction* junction);

    /// @brief add TAZ
    bool addTAZ(GNEAdditional* taz);

    /// @brief add from to stoppingPlace
    bool addStoppingPlace(GNEAdditional* stoppingPlace);

    /// @brief get consecutive edge
    const std::vector<GNEEdge*> getConsecutiveEdges() const;

    /// @brief get consecutive edge IDs
    const std::vector<std::string> getConsecutiveEdgeIDs() const;

    /// @brief get from edge
    GNEEdge* getFromEdge() const;

    /// @brief get to edge
    GNEEdge* getToEdge() const;

    /// @brief get from junction
    GNEJunction* getFromJunction() const;

    /// @brief get to junction
    GNEJunction* getToJunction() const;

    /// @brief get from TAZ
    GNEAdditional* getFromTAZ() const;

    /// @brief get to TAZ
    GNEAdditional* getToTAZ() const;

    /// @brief get from bus stop
    GNEAdditional* getFromBusStop() const;

    /// @brief get to bus stop
    GNEAdditional* getToBusStop() const;

    /// @brief get from train stop
    GNEAdditional* getFromTrainStop() const;

    /// @brief get to train stop
    GNEAdditional* getToTrainStop() const;

    /// @brief get from container stop
    GNEAdditional* getFromContainerStop() const;

    /// @brief get to container stop
    GNEAdditional* getToContainerStop() const;

    /// @brief get route
    GNEDemandElement* getRoute() const;

    /// @brief get edge
    GNEEdge* getEdge() const;

    /// @brief get busStop
    GNEAdditional* getBusStop() const;

    /// @brief get trainStop
    GNEAdditional* getTrainStop() const;

    /// @brief get containerStop
    GNEAdditional* getContainerStop() const;

    /// @brief get clicked position over lane
    double getClickedPositionOverLane() const;

    /// @brief get path route
    const std::vector<PlanPath>& getPath() const;

    /// @brief draw temporal route
    void drawTemporalRoute(const GUIVisualizationSettings& s) const;

    /// @brief abort path creation
    void abortPathCreation();

    /// @brief remove path element
    void removeLastElement();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user click over button "Finish route creation"
    long onCmdCreatePath(FXObject*, FXSelector, void*);

    /// @brief Called when the user click over button "Use last route"
    long onCmdUseLastRoute(FXObject*, FXSelector, void*);

    /// @brief Called when update button "Use last route"
    long onUpdUseLastRoute(FXObject*, FXSelector, void*);

    /// @brief Called when the user click over button "Abort route creation"
    long onCmdAbortPathCreation(FXObject*, FXSelector, void*);

    /// @brief Called when the user click over button "Remove las inserted edge"
    long onCmdRemoveLastElement(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEPlanCreator)

    // @brief planParents
    enum PlanParents {
        CONSECUTIVE_EDGES   = 1 << 0,   // Plan is placed over consecutive edges
        ROUTE               = 1 << 1,   // Plan is placed over a single route
        EDGE                = 1 << 2,   // Plan is placed over a single edge
        BUSSTOP             = 1 << 3,   // Plan is placed over a single busStop
        TRAINSTOP           = 1 << 4,   // Plan is placed over a single trainStop
        CONTAINERSTOP       = 1 << 5,   // Plan is placed over a single containerStop
        START_EDGE          = 1 << 6,   // Plan begins in edge
        START_TAZ           = 1 << 7,   // Plan begins in TAZ
        START_JUNCTION      = 1 << 8,   // Plan begins in junction
        START_BUSSTOP       = 1 << 9,   // Plan begins in busStop
        START_TRAINSTOP     = 1 << 10,  // Plan begins in trainStop
        START_CONTAINERSTOP = 1 << 11,  // Plan begins in containerStop
        END_EDGE            = 1 << 12,  // Plan ends in edge
        END_TAZ             = 1 << 13,  // Plan ends in TAZ
        END_JUNCTION        = 1 << 14,  // Plan ends in junction
        END_BUSSTOP         = 1 << 15,  // Plan ends in busStop
        END_TRAINSTOP       = 1 << 16,  // Plan ends in trainStop
        END_CONTAINERSTOP   = 1 << 17,  // Plan ends in containerStop
    };

    /// @brief clear edges
    void clearPath();

    /// @brief recalculate path
    void recalculatePath();

    /// @brief current frame parent
    GNEFrame* myFrameParent;

    /// @brief current vClass
    SUMOVehicleClass myVClass;

    /// @brief allowed plan parents
    int myPlanParents;

    /// @brief previous person plan element
    const GNEDemandElement* myPreviousPlanElement = nullptr;

    /// @brief vector with consecutive edges
    std::vector<GNEEdge*> myConsecutiveEdges;

    /// @brief from edge
    GNEEdge* myFromEdge = nullptr;

    /// @brief to edge
    GNEEdge* myToEdge = nullptr;

    /// @brief from junction
    GNEJunction* myFromJunction = nullptr;

    /// @brief to junction
    GNEJunction* myToJunction = nullptr;

    /// @brief from TAZ
    GNEAdditional* myFromTAZ = nullptr;

    /// @brief to TAZ
    GNEAdditional* myToTAZ = nullptr;

    /// @brief from StoppingPlace
    GNEAdditional* myFromStoppingPlace = nullptr;

    /// @brief to StoppingPlace
    GNEAdditional* myToStoppingPlace = nullptr;

    /// @brief route
    GNEDemandElement* myRoute = nullptr;

    /// @brief ege
    GNEEdge* myEdge = nullptr;

    /// @brief stoppingPlace
    GNEAdditional* myStoppingPlace = nullptr;

    /// @brief clicked position over lane
    double myClickedPositionOverLane = 0;

    /// @brief vector with current path
    std::vector<PlanPath> myPath;

    /// @brief button for use last inserted route
    FXButton* myUseLastRoute;

    /// @brief button for finish route creation
    FXButton* myFinishCreationButton;

    /// @brief button for abort route creation
    FXButton* myAbortCreationButton;

    /// @brief button for removing last inserted element
    FXButton* myRemoveLastInsertedElement;

    /// @brief info label
    FXLabel* myInfoLabel;

private:
    /// @brief get number of selected elements
    int getNumberOfSelectedElements() const;

    /// @brief check if enable remove last item button
    void updateRemoveLastItemButton() const;

    /// @brief show creation buttons
    void showCreationButtons();

    /// @brief hide creation buttons
    void hideCreationButtons();

    /// @brief update info label
    void updateInfoLabel();

    /// @brief add edge
    bool addSingleEdge(GNELane* lane);

    /// @brief add stoppingPlace
    bool addSingleStoppingPlace(GNEAdditional* stoppingPlace);

    /// @brief add consecutive edge
    bool addConsecutiveEdge(GNEEdge* edge);

    /// @brief add junction
    bool addFromToJunction(GNEJunction* junction);

    /// @brief add TAZ
    bool addFromToTAZ(GNEAdditional* taz);

    /// @brief add from to edge
    bool addFromToEdge(GNEEdge* edge);

    /// @brief add from to stoppingPlace
    bool addFromToStoppingPlace(GNEAdditional* stoppingPlace);

    /// @brief Invalidated copy constructor.
    GNEPlanCreator(GNEPlanCreator*) = delete;

    /// @brief Invalidated assignment operator.
    GNEPlanCreator& operator=(GNEPlanCreator*) = delete;
};
