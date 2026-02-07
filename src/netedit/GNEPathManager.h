/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
// Manager for paths in netedit (routes, trips, flows...)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netbuild/NBEdge.h>
#include <netbuild/NBVehicle.h>
#include <netedit/elements/GNEContour.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>


// ===========================================================================
// class declaration
// ===========================================================================

class GNENet;
class GNEEdge;
class GNELane;
class GNEJunction;
class GNEAdditional;
class GNEPathElement;
class GNESegment;
class GUIGlObject;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPathManager {

    /// @brief friend class declaration
    friend class GNESegment;

public:

    /// @brief class used to calculate paths in nets
    class PathCalculator {

    public:
        /// @brief constructor
        PathCalculator(const GNENet* net);

        /// @brief destructor
        ~PathCalculator();

        /**@brief update DijkstraRouter (needed a good calculation of dijkstra path after modifying network)
         * @note only needed if this path calculator requiere to use the calculateDijkstraPath(...) functions
         */
        void updatePathCalculator();

        /// @brief calculate Dijkstra path between a list of edges (for example, from-via-to edges)
        std::vector<GNEEdge*> calculateDijkstraPath(const SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges) const;

        /// @brief calculate Dijkstra path between one edge
        std::vector<GNEEdge*> calculateDijkstraPath(const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEEdge* toEdge) const;

        /// @brief calculate Dijkstra path between from edge and to junction
        std::vector<GNEEdge*> calculateDijkstraPath(const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEJunction* toJunction) const;

        /// @brief calculate Dijkstra path between from junction and to edge
        std::vector<GNEEdge*> calculateDijkstraPath(const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEEdge* toEdge) const;

        /// @brief calculate Dijkstra path between two Junctions
        std::vector<GNEEdge*> calculateDijkstraPath(const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEJunction* toJunction) const;

        /// @brief calculate reachability for given edge
        void calculateReachability(const SUMOVehicleClass vClass, GNEEdge* originEdge);

        /// @brief check if exist a path between the two given consecutive edges for the given VClass
        bool consecutiveEdgesConnected(const SUMOVehicleClass vClass, const GNEEdge* from, const GNEEdge* to) const;

        /// @brief check if exist a path between the given busStop and edge (Either a valid lane or an acces) for pedestrians
        bool busStopConnected(const GNEAdditional* busStop, const GNEEdge* edge) const;

        /// @brief check if pathCalculator is updated
        bool isPathCalculatorUpdated() const;

        /// @brief invalidate pathCalculator
        void invalidatePathCalculator();

    private:
        /// @brief pointer to net
        const GNENet* myNet;

        /// @brief flag for checking if path calculator is updated
        bool myPathCalculatorUpdated;

        /// @brief SUMO Abstract myDijkstraRouter
        SUMOAbstractRouter<NBRouterEdge, NBVehicle>* myDijkstraRouter;

        /// @brief optimize junction path
        std::vector<GNEEdge*> optimizeJunctionPath(const std::vector<GNEEdge*>& edges) const;
    };

    /// @brief class used to mark path draw
    class PathDraw {

    public:
        /// @brief constructor
        PathDraw();

        /// @brief destructor
        ~PathDraw();

        /// @brief clear path draw
        void clearPathDraw();

        /// @brief check if path element geometry must be drawn in the given lane
        bool checkDrawPathGeometry(const GUIVisualizationSettings& s, const GNELane* lane, const SumoXMLTag tag, const bool isPlan);

        /// @brief check if path element geometry must be drawn in the given junction
        bool checkDrawPathGeometry(const GUIVisualizationSettings& s, const GNESegment* segment, const SumoXMLTag tag, const bool isPlan);

    private:
        /// @brief map for saving tags drawn in lanes
        std::map<const GNELane*, std::set<SumoXMLTag> > myLaneDrawedElements;

        /// @brief map for saving tags drawn in junctions
        std::map<const std::pair<const GNELane*, const GNELane*>, std::set<SumoXMLTag> > myLane2laneDrawedElements;
    };

    /// @brief constructor
    GNEPathManager(const GNENet* net);

    /// @brief destructor
    ~GNEPathManager();

    /// @brief obtain instance of PathCalculator
    PathCalculator* getPathCalculator();

    /// @brief get path element
    const GNEPathElement* getPathElement(const GUIGlObject* GLObject) const;

    /// @brief get path segments
    const std::vector<GNESegment*>& getPathElementSegments(GNEPathElement* pathElement) const;

    /// @brief obtain instance of PathDraw
    PathDraw* getPathDraw();

    /// @brief check if path element is valid
    bool isPathValid(const GNEPathElement* pathElement) const;

    /// @brief get first lane associated with path element
    const GNELane* getFirstLane(const GNEPathElement* pathElement) const;

    /// @brief calculate path between from-to edges (using dijkstra, require path calculator updated)
    void calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNELane* fromLane, GNELane* toLane);

    /// @brief calculate path between from edge and to junction(using dijkstra, require path calculator updated)
    void calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNELane* fromLane, GNEJunction* toJunction);

    /// @brief calculate path between from junction and to edge (using dijkstra, require path calculator updated)
    void calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNEJunction* fromJunction, GNELane* toLane);

    /// @brief calculate path between from junction and to junction (using dijkstra, require path calculator updated)
    void calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEJunction* toJunction);

    /// @brief calculate path lanes between list of edges (using dijkstra, require path calculator updated)
    void calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges);

    /// @brief calculate consecutive path edges
    void calculateConsecutivePathEdges(GNEPathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges,
                                       const int firstLaneIndex = -1, const int lastLaneIndex = -1);

    /// @brief calculate consecutive path lanes
    void calculateConsecutivePathLanes(GNEPathElement* pathElement, const std::vector<GNELane*>& lanes);

    /// @brief remove path
    void removePath(GNEPathElement* pathElement);

    /// @brief draw lane path elements
    void drawLanePathElements(const GUIVisualizationSettings& s, const GNELane* lane) const;

    /// @brief draw junction path elements
    void drawJunctionPathElements(const GUIVisualizationSettings& s, const GNEJunction* junction) const;

    /// @brief redraw path elements saved in gViewObjectsHandler buffer
    void redrawPathElements(const GUIVisualizationSettings& s) const;

    /// @brief invalidate lane path
    void invalidateLanePath(const GNELane* lane);

    /// @brief invalidate junction path
    void invalidateJunctionPath(const GNEJunction* junction);

    /// @brief clear segments
    void clearSegments();

protected:
    /// @brief add segments int laneSegments (called by GNESegment constructor)
    void addSegmentInLaneSegments(GNESegment* segment, const GNELane* lane);

    /// @brief add segments int junctionSegments (called by GNESegment constructor)
    void addSegmentInJunctionSegments(GNESegment* segment, const GNEJunction* junction);

    /// @brief clear segments from junction and lane Segments (called by GNESegment destructor)
    void clearSegmentFromJunctionAndLaneSegments(GNESegment* segment);

    /// @brief check if given lanes are connected
    bool connectedLanes(const GNELane* fromLane, const GNELane* toLane) const;

    /// @brief build path
    void buildPath(GNEPathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> path,
                   GNELane* fromLane, GNEJunction* fromJunction, GNELane* toLane, GNEJunction* toJunction);

    /// @brief PathCalculator instance
    PathCalculator* myPathCalculator;

    /// @brief PathDraw instance
    PathDraw* myPathDraw;

    /// @brief map with path element and their associated segments
    std::map<const GNEPathElement*, std::vector<GNESegment*> > myPaths;

    /// @brief map with lane segments
    std::map<const GNELane*, std::set<GNESegment*> > myLaneSegments;

    /// @brief map with junction segments
    std::map<const GNEJunction*, std::set<GNESegment*> > myJunctionSegments;

    /// @brief flag for clear segments quickly
    bool myCleaningSegments = false;

private:
    /// @brief mark label segment
    void markLabelSegment(const std::vector<GNESegment*>& segments) const;

    /// @brief empty segments (used in getPathElementSegments)
    const std::vector<GNESegment*> myEmptySegments;

    /// @brief Invalidated copy constructor.
    GNEPathManager(const GNEPathManager&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPathManager& operator=(const GNEPathManager&) = delete;
};
