/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEPathManager.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2011
///
// Manager for paths in NETEDIT (routes, trips, flows...)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netbuild/NBEdge.h>
#include <netbuild/NBVehicle.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrier;
class GNELane;
class GNENet;

class GNEPathManager {

public:
    /// @brief class used to calculate paths in nets
    class PathCalculator {

    public:
        /// @brief constructor
        PathCalculator(const GNENet* net);

        /// @brief destructor
        ~PathCalculator();

        /// @brief update path calculator (called when SuperModes Demand or Data is selected)
        void updatePathCalculator();

        /// @brief calculate Dijkstra path between a list of partial edges
        std::vector<GNEEdge*> calculatePath(const SUMOVehicleClass vClass, const std::vector<GNEEdge*>& partialEdges) const;

        /// @brief calculate reachability for given edge
        void calculateReachability(const SUMOVehicleClass vClass, GNEEdge* originEdge);

        /// @brief check if exist a path between the two given consecutives edges for the given VClass
        bool consecutiveEdgesConnected(const SUMOVehicleClass vClass, const GNEEdge* from, const GNEEdge* to) const;

        /// @brief check if exist a path between the given busStop and edge (Either a valid lane or an acces) for pedestrians
        bool busStopConnected(const GNEAdditional* busStop, const GNEEdge* edge) const;

    private:
        /// @brief pointer to net
        const GNENet* myNet;

        /// @brief SUMO Abstract myDijkstraRouter
        SUMOAbstractRouter<NBRouterEdge, NBVehicle>* myDijkstraRouter;
    };

    /// @brief constructor
    GNEPathManager(const GNENet* net);

    /// @brief destructor
    ~GNEPathManager();

    /// @brief obtain instance of PathCalculator
    PathCalculator* getPathCalculator();

    /// @brief calculate path
    void calculatePath(GNEAttributeCarrier* AC, SUMOVehicleClass vClass, const bool allowedVClass, std::vector<GNELane*> lanes);

    /// @brief draw additional element path
    void drawAdditionalElementPath(const GUIVisualizationSettings& s, const GNELane* lane, const GNEAdditional* additionalElement);

    /// @brief draw demand element path
    void drawDemandElementPath(const GUIVisualizationSettings& s, const GNELane* lane, const GNEDemandElement* demandElement);

    /// @brief draw generic data path
    void drawGenericDataPath(const GUIVisualizationSettings& s, const GNELane* lane, const GNEGenericData* genericData);

    /// @brief invalidate path
    void invalidatePath(const GNELane* lane);

    /// @brief clear segments
    void clearSegments();

protected:

    class Segment {

    public:
        /// @brief constructor
        Segment(GNEPathManager* pathManager, GNEAttributeCarrier* element, const GNELane* lane);

        /// @brief destructor
        ~Segment();

        /// @brief path manager
        GNEPathManager* myPathManager;

        /// @brief parent element
        const GNEAttributeCarrier* myElement;

        /// @brief valid element
        bool myValid;

    private:
        /// @brief default constructor
        Segment();

        /// @brief Invalidated copy constructor.
        Segment(const Segment&) = delete;

        /// @brief Invalidated assignment operator.
        Segment& operator=(const Segment&) = delete;
    };

    /// @brief map with Attribute Carrier and their asociated path
    std::map<GNEAttributeCarrier*, std::vector<Segment*> > myPaths;

    /// @brief map with lane segments
    std::map<const GNELane*, std::set<Segment*> > myLaneSegments;

    /// @brief PathCalculator instance
    PathCalculator* myPathCalculator;

    /// @brief add segments int laneSegments (called by Segment constructor)
    void addSegmentInLaneSegments(Segment *segment, const GNELane *lane);

    /// @brief clear segments from laneSegments (called by Segment destructor)
    void clearSegmentFromLaneSegments(Segment *segment);

private:
    /// @brief Invalidated copy constructor.
    GNEPathManager(const GNEPathManager&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPathManager& operator=(const GNEPathManager&) = delete;
};
