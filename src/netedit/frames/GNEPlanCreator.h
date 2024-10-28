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

    /// @brief get plan parameters
    const CommonXMLStructure::PlanParameters& getPlanParameteres() const;

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
        STOPPINGPLACE       = 1 << 3,   // Plan is placed over a stoppingPlace
        START_EDGE          = 1 << 4,   // Plan begins in edge
        START_TAZ           = 1 << 5,   // Plan begins in TAZ
        START_JUNCTION      = 1 << 6,   // Plan begins in junction
        START_STOPPINGPLACE = 1 << 7,   // Plan begins in busStop
        END_EDGE            = 1 << 8,   // Plan ends in edge
        END_TAZ             = 1 << 9,   // Plan ends in TAZ
        END_JUNCTION        = 1 << 10,  // Plan ends in junction
        END_STOPPINGPLACE   = 1 << 11,  // Plan ends in stoppingPlace
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

    /// @brief plan parameters
    CommonXMLStructure::PlanParameters myPlanParameteres;

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
