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
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributeCarrier;
class GNELane;
class GNENet;

class GNEPathManager {

public:
    /// @brief class used for path elements
    class PathElement {

    public:
        enum Options {
            NETWORK_ELEMENT =    1 << 0,  // Network element
            ADDITIONAL_ELEMENT = 1 << 1,  // Additional element
            DEMAND_ELEMENT =     1 << 2,  // Demand element
            DATA_ELEMENT =       1 << 3,  // Data element
            FIRST_SEGMENT =      1 << 4,  // First segment
            LAST_SEGMENT  =      1 << 5,  // Last segment
        };

        /// @brief constructor
        PathElement(const Options option);

        /// @brief destructor
        ~PathElement();

        /// @brief check if pathElement is a network element
        bool isNetworkElement() const;

        /// @brief check if pathElement is an additional element
        bool isAdditionalElement() const;

        /// @brief check if pathElement is a demand element
        bool isDemandElement() const;

        /// @brief check if pathElement is a data element
        bool isDataElement() const;

        /**@brief Draws partial object (lane)
         * @param[in] s The settings for the current view (may influence drawing)
         * @param[in] lane GNELane in which draw partial
         * @param[in] drawGeometry flag to enable/disable draw geometry (lines, boxLines, etc.)
         * @param[in] options partial GL Options
         */
        virtual void drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const double offsetFront, const int options) const = 0;

        /**@brief Draws partial object (junction)
         * @param[in] s The settings for the current view (may influence drawing)
         * @param[in] fromLane from GNELane
         * @param[in] toLane to GNELane
         * @param[in] offsetFront offset for drawing element front (needed for selected elements)
         * @param[in] options partial GL Options
         */
        virtual void drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const double offsetFront, const int options) const = 0;

    private:
        /// @brief default constructor
        PathElement();

        /// @brief pathElement option
        const Options myOption;
    };

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

    /// @brief get path element size with the given PathElement
    int getPathSize(PathElement * pathElement) const;

    /// @brief get first lane associated with path element
    const GNELane* getFirstLane(const PathElement* pathElement) const;

    /// @brief calculate path for edges
    void calculateEdgesPath(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> edges);

    /// @brief calculate path for lanes
    void calculateLanesPath(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNELane*> lanes);

    /// @brief remove path
    void removePath(PathElement* pathElement);

    /// @brief draw lane path elements
    void drawLanePathElements(const GUIVisualizationSettings& s, const GNELane* lane);

    /// @brief draw junction path elements
    void drawJunctionPathElements(const GUIVisualizationSettings& s, const GNEJunction* junction);

    /// @brief invalidate path
    void invalidatePath(const GNELane* lane);

    /// @brief clear demand paths
    void clearDemandPaths();

protected:
    /// @brief segment
    class Segment {

    public:
        /// @brief constructor for lanes
        Segment(GNEPathManager* pathManager, PathElement* element, const GNELane* lane, const bool firstSegment, const bool lastSegment);

        /// @brief constructor for junctions
        Segment(GNEPathManager* pathManager, PathElement* element, const GNEJunction* junction, const GNELane* previousLane, const GNELane* nextLane);

        /// @brief destructor
        ~Segment();

        /// @brief check if segment is the first path's segment
        bool isFirstSegment() const;

        /// @brief check if segment is the last path's segment
        bool isLastSegment() const;

        /// @brief get path element
        const PathElement* getPathElement() const;

        /// @brief get lane associated with this segment
        const GNELane* getLane() const;

        /// @brief get previous lane
        const GNELane* getPreviousLane() const;

        /// @brief get next lane
        const GNELane* getNextLane() const;

        /// @brief get junction associated with this segment
        const GNEJunction* getJunction() const;

    protected:
        /// @brief path manager
        GNEPathManager* myPathManager;

        /// @brief path element
        const PathElement* myPathElement;

        /// @brief first segment
        const bool myFirstSegment;
        
        /// @brief lastSegment
        const bool myLastSegment;

        /// @brief lane associated with this segment
        const GNELane* myLane;

        /// @brief previous lane
        const GNELane *myPreviousLane;
        
        /// @brief next lane
        const GNELane* myNextLane;

        /// @brief junction associated with this segment
        const GNEJunction* myJunction;

        /// @brief valid segment
        bool myValid;

    private:
        /// @brief default constructor
        Segment();

        /// @brief Invalidated copy constructor.
        Segment(const Segment&) = delete;

        /// @brief Invalidated assignment operator.
        Segment& operator=(const Segment&) = delete;
    };

    /// @brief add segments int laneSegments (called by Segment constructor)
    void addSegmentInLaneSegments(Segment *segment, const GNELane *lane);

    /// @brief add segments int junctionSegments (called by Segment constructor)
    void addSegmentInJunctionSegments(Segment* segment, const GNEJunction* junction);

    /// @brief clear segments from junction and lane Segments (called by Segment destructor)
    void clearSegmentFromJunctionAndLaneSegments(Segment* segment);

    /// @brief clear segments
    void clearSegments();

    /// @brief PathCalculator instance
    PathCalculator* myPathCalculator;

    /// @brief map with path element and their asociated segments
    std::map<const PathElement*, std::vector<Segment*> > myPaths;

    /// @brief map with lane segments
    std::map<const GNELane*, std::set<Segment*> > myLaneSegments;

    /// @brief map with junction segments
    std::map<const GNEJunction*, std::set<Segment*> > myJunctionSegments;

private:
    /// @brief Invalidated copy constructor.
    GNEPathManager(const GNEPathManager&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPathManager& operator=(const GNEPathManager&) = delete;
};
