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
        /// @brief constructor for single edge
        PlanPath(const SUMOVehicleClass vClass, GNEEdge* edge);

        /// @brief constructor for two edges
        PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* edgeFrom, GNEEdge* edgeTo);

        /// @brief constructor for two junctions
        PlanPath(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEJunction* junctionFrom, GNEJunction* junctionTo);

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

    /// @brief show plan creator for the given tag property
    void showPlanCreatorModule(const GNEPlanSelector* planSelector, const GNEDemandElement *previousPlan);

    /// @brief show GNEPlanCreator
    void hidePathCreatorModule();

    /// @brief get vClass
    SUMOVehicleClass getVClass() const;

    /// @brief set vClass
    void setVClass(SUMOVehicleClass vClass);

    /// @brief add junction
    bool addJunction(GNEJunction* junction);

    /// @brief add TAZ
    bool addTAZ(GNEAdditional* taz);

    /// @brief add consecutive edge
    bool addConsecutiveEdge(GNEEdge* edge);

    /// @brief add edge
    bool addEdge(GNEEdge* edge);
    
    /// @brief add stoppingPlace
    bool addStoppingPlace(GNEAdditional* stoppingPlace);

    /// @brief add route
    bool addRoute(GNEDemandElement* route);

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

    /// @brief get Route
    GNEDemandElement* getRoute() const;

    /// @brief get path route
    const std::vector<PlanPath>& getPath() const;

    /// @brief draw temporal route
    void drawTemporalRoute(const GUIVisualizationSettings& s) const;

    /// @brief create path
    bool createPath(const bool useLastRoute);

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
    FOX_CONSTRUCTOR(GNEPlanCreator)

    // @brief creation mode
    enum Mode {
        CONSECUTIVE_EDGES       = 1 << 0,   // Plan requiere consecutive edges
        ROUTE                   = 1 << 1,   // Plan uses a route
        START_EDGE              = 1 << 2,   // Plan begins in edge
        END_EDGE                = 1 << 3,   // Plan ends in edge
        START_JUNCTION          = 1 << 4,   // Plan begins in junction
        END_JUNCTION            = 1 << 5,   // Plan ends in junction
        START_TAZ               = 1 << 6,   // Plan begins in TAZ
        END_TAZ                 = 1 << 7,   // Plan ends in TAZ
        START_BUSSTOP           = 1 << 8,   // Plan begins in busStop
        END_BUSSTOP             = 1 << 9,   // Plan ends in busStop
        START_TRAINSTOP         = 1 << 10,  // Plan begins in trainStop
        END_TRAINSTOP           = 1 << 11,  // Plan ends in trainStop
        // stops and containerStops
    };

    /// @brief update InfoRouteLabel
    void updateInfoRouteLabel();

    /// @brief clear edges
    void clearPath();

    /// @brief recalculate path
    void recalculatePath();

    /// @brief set special candidates (This function will be called recursively)
    void setSpecialCandidates(GNEEdge* originEdge);

    /// @brief set edgereachability (This function will be called recursively)
    void setPossibleCandidates(GNEEdge* originEdge, const SUMOVehicleClass vClass);

    /// @brief current frame parent
    GNEFrame* myFrameParent;

    /// @brief current vClass
    SUMOVehicleClass myVClass;

    /// @brief current creation mode
    int myCreationMode;

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

    /// @brief from Route (used for connecting plans)
    GNEDemandElement* myFromRoute = nullptr;

    /// @brief route
    GNEDemandElement* myRoute = nullptr;

    /// @brief vector with current path
    std::vector<PlanPath> myPath;

    /// @brief label with route info
    FXLabel* myInfoRouteLabel;

    /// @brief button for use last inserted route
    FXButton* myUseLastRoute;

    /// @brief button for finish route creation
    FXButton* myFinishCreationButton;

    /// @brief button for abort route creation
    FXButton* myAbortCreationButton;

    /// @brief button for removing last inserted element
    FXButton* myRemoveLastInsertedElement;

    /// @brief label for backSpace information
    FXLabel* myBackSpaceLabel;

private:
    /// @brief get number of selected elements
    size_t getNumberOfSelectedElements() const;

    /// @brief check if enable remove last item button
    bool checkEnableLastItemButton() const;

    /// @brief Invalidated copy constructor.
    GNEPlanCreator(GNEPlanCreator*) = delete;

    /// @brief Invalidated assignment operator.
    GNEPlanCreator& operator=(GNEPlanCreator*) = delete;
};
