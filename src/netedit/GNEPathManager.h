/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GNENet;
class GNEAttributeCarrier;
class GNEEdge;
class GNELane;
class GNEJunction;
class GNEAdditional;

class GNEPathManager {

public:
    /// @brief class declaration
    class PathElement;

    /// @brief segment
    class Segment {

    public:
        /// @brief constructor for lanes
        Segment(GNEPathManager* pathManager, PathElement* element, const GNELane* lane,
                const bool firstSegment, const bool lastSegment);

        /// @brief constructor for junctions
        Segment(GNEPathManager* pathManager, PathElement* element, const GNEJunction* junction,
                const GNELane* previousLane, const GNELane* nextLane);

        /// @brief destructor
        ~Segment();

        /// @brief check if segment is the first path's segment
        bool isFirstSegment() const;

        /// @brief check if segment is the last path's segment
        bool isLastSegment() const;

        /// @brief get path element
        PathElement* getPathElement() const;

        /// @brief get lane associated with this segment
        const GNELane* getLane() const;

        /// @brief get previous lane
        const GNELane* getPreviousLane() const;

        /// @brief get next lane
        const GNELane* getNextLane() const;

        /// @brief get junction associated with this segment
        const GNEJunction* getJunction() const;

        /// @brief get next segment
        Segment* getNextSegment() const;

        /// @brief set next segment
        void setNextSegment(Segment* nexSegment);

        /// @brief get previous segment
        Segment* getPreviousSegment() const;

        /// @brief set previous segment
        void setPreviousSegment(Segment* nexSegment);

        /// @brief check if segment is label segment
        bool isLabelSegment() const;

        /// @brief mark segment as middle segment
        void markSegmentLabel();

    protected:
        /// @brief path manager
        GNEPathManager* myPathManager;

        /// @brief path element
        PathElement* myPathElement;

        /// @brief first segment
        const bool myFirstSegment;

        /// @brief lastSegment
        const bool myLastSegment;

        /// @brief lane associated with this segment
        const GNELane* myLane;

        /// @brief previous lane
        const GNELane* myPreviousLane;

        /// @brief next lane
        const GNELane* myNextLane;

        /// @brief junction associated with this segment
        const GNEJunction* myJunction;

        /// @brief pointer to next segment (use for draw red line)
        Segment* myNextSegment;

        /// @brief pointer to previous segment (use for draw red line)
        Segment* myPreviousSegment;

        /// @brief flag for check if this segment is a label segment
        bool myLabelSegment;

    private:
        /// @brief default constructor
        Segment();

        /// @brief Invalidated copy constructor.
        Segment(const Segment&) = delete;

        /// @brief Invalidated assignment operator.
        Segment& operator=(const Segment&) = delete;
    };

    /// @brief class used for path elements
    class PathElement : public GUIGlObject {

    public:
        enum Options {
            NETWORK_ELEMENT =       1 << 0, // Network element
            ADDITIONAL_ELEMENT =    1 << 1, // Additional element
            DEMAND_ELEMENT =        1 << 2, // Demand element
            DATA_ELEMENT =          1 << 3, // Data element
            ROUTE =                 1 << 4, // Route (needed for overlapping labels)
        };

        /// @brief constructor
        PathElement(GUIGlObjectType type, const std::string& microsimID, FXIcon* icon, const int options);

        /// @brief destructor
        virtual ~PathElement();

        /// @brief check if pathElement is a network element
        bool isNetworkElement() const;

        /// @brief check if pathElement is an additional element
        bool isAdditionalElement() const;

        /// @brief check if pathElement is a demand element
        bool isDemandElement() const;

        /// @brief check if pathElement is a data element
        bool isDataElement() const;

        /// @brief check if pathElement is a route
        bool isRoute() const;

        /// @brief compute pathElement
        virtual void computePathElement() = 0;

        /// @brief check if path element is selectd
        virtual bool isPathElementSelected() const = 0;

        /**@brief Draws partial object (lane)
         * @param[in] s The settings for the current view (may influence drawing)
         * @param[in] lane GNELane in which draw partial
         * @param[in] drawGeometry flag to enable/disable draw geometry (lines, boxLines, etc.)
         * @param[in] offsetFront extra front offset (used for drawing partial gl above other elements)
         */
        virtual void drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const = 0;

        /**@brief Draws partial object (junction)
         * @param[in] s The settings for the current view (may influence drawing)
         * @param[in] fromLane from GNELane
         * @param[in] toLane to GNELane
         * @param[in] segment PathManager segment (used for segment options)
         * @param[in] offsetFront extra front offset (used for drawing partial gl above other elements)
         */
        virtual void drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const = 0;

        /// @brief get first path lane
        virtual GNELane* getFirstPathLane() const = 0;

        /// @brief get last path lane
        virtual GNELane* getLastPathLane() const = 0;

        /// @brief get path element depart lane pos
        virtual double getPathElementDepartValue() const = 0;

        /// @brief get path element depart position
        virtual Position getPathElementDepartPos() const = 0;

        /// @brief get path element arrival lane pos
        virtual double getPathElementArrivalValue() const = 0;

        /// @brief get path element arrival position
        virtual Position getPathElementArrivalPos() const = 0;

    private:
        /// @brief pathElement option
        const int myOption;

        /// @brief invalidate default constructor
        PathElement() = delete;

        /// @brief Invalidated copy constructor.
        PathElement(const PathElement&) = delete;

        /// @brief Invalidated assignment operator.
        PathElement& operator=(const PathElement&) = delete;
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
        std::vector<GNEEdge*> calculateDijkstraPath(const SUMOVehicleClass vClass, const std::vector<GNEEdge*>& partialEdges) const;

        /// @brief calculate Dijkstra path between two Junctions
        std::vector<GNEEdge*> calculateDijkstraPath(const SUMOVehicleClass vClass, const GNEJunction* fromJunction, const GNEJunction* toJunction) const;

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
        bool drawPathGeometry(const bool dottedElement, const GNELane* lane, SumoXMLTag tag);

        /// @brief check if path element geometry must be drawn in the given junction
        bool drawPathGeometry(const bool dottedElement, const GNELane* fromLane, const GNELane* toLane, SumoXMLTag tag);

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
    const PathElement* getPathElement(const GUIGlObject* GLObject) const;

    /// @brief get path segments
    const std::vector<Segment*>& getPathElementSegments(PathElement* pathElement) const;

    /// @brief obtain instance of PathDraw
    PathDraw* getPathDraw();

    /// @brief check if path element is valid
    bool isPathValid(const PathElement* pathElement) const;

    /// @brief get first lane associated with path element
    const GNELane* getFirstLane(const PathElement* pathElement) const;

    /// @brief calculate path edges (using dijkstra, require path calculator updated)
    void calculatePathEdges(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> edges);

    /// @brief calculate path lanes (using dijkstra, require path calculator updated)
    void calculatePathLanes(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNELane*> lanes);

    /// @brief calculate path junctions (using dijkstra, require path calculator updated)
    void calculatePathJunctions(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEJunction*> junctions);

    /// @brief calculate consecutive path edges
    void calculateConsecutivePathEdges(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> edges);

    /// @brief calculate consecutive path lanes
    void calculateConsecutivePathLanes(PathElement* pathElement, const std::vector<GNELane*> lanes);

    /// @brief remove path
    void removePath(PathElement* pathElement);

    /// @brief draw lane path elements
    void drawLanePathElements(const GUIVisualizationSettings& s, const GNELane* lane);

    /// @brief draw junction path elements
    void drawJunctionPathElements(const GUIVisualizationSettings& s, const GNEJunction* junction);

    /// @brief force draw path (used carefully, ONLY when we're inspecting a path element, due slowdowns)
    void forceDrawPath(const GUIVisualizationSettings& s, const PathElement* pathElement) const;

    /// @brief invalidate lane path
    void invalidateLanePath(const GNELane* lane);

    /// @brief invalidate junction path
    void invalidateJunctionPath(const GNEJunction* junction);

    /// @brief clear demand paths
    void clearDemandPaths();

protected:
    /// @brief add segments int laneSegments (called by Segment constructor)
    void addSegmentInLaneSegments(Segment* segment, const GNELane* lane);

    /// @brief add segments int junctionSegments (called by Segment constructor)
    void addSegmentInJunctionSegments(Segment* segment, const GNEJunction* junction);

    /// @brief clear segments from junction and lane Segments (called by Segment destructor)
    void clearSegmentFromJunctionAndLaneSegments(Segment* segment);

    /// @brief clear segments
    void clearSegments();

    /// @brief check if given lanes are connected
    bool connectedLanes(const GNELane* fromLane, const GNELane* toLane) const;

    /// @brief PathCalculator instance
    PathCalculator* myPathCalculator;

    /// @brief PathDraw instance
    PathDraw* myPathDraw;

    /// @brief map with path element and their associated segments
    std::map<const PathElement*, std::vector<Segment*> > myPaths;

    /// @brief map with lane segments
    std::map<const GNELane*, std::set<Segment*> > myLaneSegments;

    /// @brief map with junction segments
    std::map<const GNEJunction*, std::set<Segment*> > myJunctionSegments;

private:
    /// @brief empty segments (used in getPathElementSegments)
    const std::vector<Segment*> myEmptySegments;

    /// @brief Invalidated copy constructor.
    GNEPathManager(const GNEPathManager&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPathManager& operator=(const GNEPathManager&) = delete;
};
