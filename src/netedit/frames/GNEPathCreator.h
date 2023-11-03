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
/// @file    GNEPathCreator.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for create paths
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPathCreator : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEPathCreator)

public:
    /// @brief class for path
    class Path {

    public:
        /// @brief constructor for single edge
        Path(const SUMOVehicleClass vClass, GNEEdge* edge);

        /// @brief constructor for two edges
        Path(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* edgeFrom, GNEEdge* edgeTo);

        /// @brief constructor for two junctions
        Path(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEJunction* junctionFrom, GNEJunction* junctionTo);

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
        Path();

        /// @brief Invalidated copy constructor.
        Path(Path*) = delete;

        /// @brief Invalidated assignment operator.
        Path& operator=(Path*) = delete;
    };

    /// @brief default constructor
    GNEPathCreator(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEPathCreator();

    /// @brief show GNEPathCreator for the given tag
    void showPathCreatorModule(const GNETagProperties &tagProperty, const bool consecutives);

    /// @brief show GNEPathCreator
    void hidePathCreatorModule();

    /// @brief get vClass
    SUMOVehicleClass getVClass() const;

    /// @brief set vClass
    void setVClass(SUMOVehicleClass vClass);

    /// @brief add junction
    bool addJunction(GNEJunction* junction);

    /// @brief add TAZ
    bool addTAZ(GNETAZ* taz);

    /// @brief add edge
    bool addEdge(GNEEdge* edge, const bool shiftKeyPressed, const bool controlKeyPressed);

    /// @brief get current selected edges
    const std::vector<GNEEdge*>& getSelectedEdges() const;

    /// @brief get current selected junctions
    const std::vector<GNEJunction*>& getSelectedJunctions() const;
    
    /// @brief get current selected TAZs
    const std::vector<GNETAZ*>& getSelectedTAZs() const;
    
    /// @brief add route
    bool addRoute(GNEDemandElement* route);

    /// @brief get route
    GNEDemandElement* getRoute() const;

    /// @brief get path route
    const std::vector<Path>& getPath() const;

    /// @brief draw candidate edges with special color (Only for candidates, special and conflicted)
    bool drawCandidateEdgesWithSpecialColor() const;

    /// @brief update junction colors
    void updateJunctionColors();

    /// @brief update edge colors
    void updateEdgeColors();

    /// @brief clear junction colors
    void clearJunctionColors();

    /// @brief clear edge colors
    void clearEdgeColors();

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

    /// @brief Called when the user click over check button "show candidate edges"
    long onCmdShowCandidateEdges(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(GNEPathCreator)

    // @brief creation mode
    enum Mode {
        ONLY_FROMTO              = 1 << 0,   // Path only had two elements (first and last)
        CONSECUTIVE_EDGES        = 1 << 1,   // Path's edges are consecutives
        NONCONSECUTIVE_EDGES     = 1 << 2,   // Path's edges aren't consecutives
        START_EDGE               = 1 << 3,   // Path begins in edge
        END_EDGE                 = 1 << 4,   // Path ends in edge
        START_JUNCTION           = 1 << 5,   // Path begins in junction
        END_JUNCTION             = 1 << 6,   // Path ends in junction
        START_TAZ                = 1 << 7,   // Path begins in TAZ
        END_TAZ                  = 1 << 8,   // Path ends in TAZ
        ROUTE                    = 1 << 9,   // Path is over an existent edge
        SHOW_CANDIDATE_EDGES     = 1 << 10,  // Show candidate edges
        SHOW_CANDIDATE_JUNCTIONS = 1 << 11,  // show candidate junctions
    };

    /// @brief update InfoRouteLabel
    void updateInfoRouteLabel();

    /// @brief clear edges (and restore colors)
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

    /// @brief vector with selected junctions
    std::vector<GNEJunction*> mySelectedJunctions;

    /// @brief vector with selected TAZs
    std::vector<GNETAZ*> mySelectedTAZs;

    /// @brief vector with selected edges
    std::vector<GNEEdge*> mySelectedEdges;

    /// @brief route (usually a busStop)
    GNEDemandElement* myRoute;

    /// @brief vector with current path
    std::vector<Path> myPath;

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

    /// @brief CheckBox for show candidate edges
    FXCheckButton* myShowCandidateEdges;

    /// @brief label for shift information
    FXLabel* myShiftLabel;

    /// @brief label for control information
    FXLabel* myControlLabel;

    /// @brief label for backSpace information
    FXLabel* myBackSpaceLabel;
private:
    /// @brief Invalidated copy constructor.
    GNEPathCreator(GNEPathCreator*) = delete;

    /// @brief Invalidated assignment operator.
    GNEPathCreator& operator=(GNEPathCreator*) = delete;
};
