/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEGeometry.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2019
/// @version $Id$
///
// File for geometry classes and functions
/****************************************************************************/
#ifndef GNEGeometry_h
#define GNEGeometry_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/Parameterised.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#include "GNEReferenceCounter.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNELane;
class GNEEdge;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================

struct GNEGeometry {

    /// @brief struct for pack all variables related with geometry of stop
    struct Geometry {

        /// @brief constructor
        Geometry();

        /// @brief reset geometry
        void clearGeometry();

        /// @brief calculate shape rotations and lengths
        void calculateShapeRotationsAndLengths();

        /// @brief The shape of the additional element
        PositionVector shape;

        /// @brief The rotations of the single shape parts
        std::vector<double> shapeRotations;

        /// @brief The lengths of the single shape parts
        std::vector<double> shapeLengths;

    private:
        /// @brief Invalidated assignment operator
        Geometry& operator=(const Geometry& other) = delete;
    };

    /// @brief struct for pack all variables related with geometry of elemements divided in segments
    struct SegmentGeometry {

        /// @brief struct used for represent segments of element geometry
        struct Segment {
            /// @brief parameter constructor for lanes (geometry will be taked from lane)
            Segment(const GNEAttributeCarrier* _AC, const GNELane* _lane, const bool _valid);

            /// @brief parameter constructor for segments which geometry will be storaged in segment
            Segment(const GNEAttributeCarrier* _AC, const GNELane* _lane,
                    const PositionVector& shape, const std::vector<double> &shapeRotations, const std::vector<double> &shapeLengths, const bool _valid);

            /// @brief parameter constructor for lane2lane connections
            Segment(const GNEAttributeCarrier* _AC, const GNELane* currentLane, const GNELane* nextLane, const bool _valid);

            /// @brief update segment
            void update(const PositionVector& shape, const std::vector<double> &shapeRotations, const std::vector<double> &shapeLengths);

            /// @brief get lane/lane2lane shape
            const PositionVector &getShape() const;

            /// @brief get lane/lane2lane shape rotations
            const std::vector<double> &getShapeRotations() const;
            
            /// @brief get lane/lane2lane shape lenghts
            const std::vector<double> &getShapeLengths() const;

            /// @brief element
            const GNEAttributeCarrier* AC;

            /// @brief edge
            const GNEEdge* edge;

            /// @brief lane
            const GNELane* lane;

            /// @brief junction
            const GNEJunction* junction;

            /// @brief valid
            const bool valid;

        private:
            /// @brief flag to use lane shape
            bool myUseLaneShape;

            /// @brief flag to use lane2lane connection
            bool myUseLane2LaneShape;

            /// @brief segment shape
            PositionVector mySegmentShape;

            /// @brief segment rotation
            std::vector<double> mySegmentRotations;
            
            /// @brief segment lenghts
            std::vector<double> mySegmentLengths;

            /// @brief Invalidated assignment operator
            Segment& operator=(const Segment& other) = delete;
        };

        /// @brief struct used for represent segments that must be updated
        struct SegmentToUpdate {

            /// @brief constructor
            SegmentToUpdate(const int _index, const GNELane* _lane, const GNELane* _nextLane);

            /// @brief segment index
            const int index;

            // @brief lane segment
            const GNELane* lane;

            /// @brief lane segment (used for updating lane2lane segments)
            const GNELane* nextLane;

        private:
            /// @brief Invalidated assignment operator
            SegmentToUpdate& operator=(const SegmentToUpdate& other) = delete;
        };

        /// @brief constructor
        SegmentGeometry();

        /// @brief insert entire lane segment (used to avoid unnecessary calculation in calculatePartialShapeRotationsAndLengths)
        void insertLaneSegment(const GNEAttributeCarrier* AC, const GNELane* lane, const bool valid);

        /// @brief insert custom segment 
        void insertCustomSegment(const GNEAttributeCarrier* AC, const GNELane* lane,
                                 const PositionVector& laneShape, const std::vector<double> &laneShapeRotations, const std::vector<double> &laneShapeLengths, const bool valid);

        /// @brief insert entire lane2lane segment (used to avoid unnecessary calculation in calculatePartialShapeRotationsAndLengths)
        void insertLane2LaneSegment(const GNEAttributeCarrier* AC, const GNELane* currentLane, const GNELane* nextLane, const bool valid);

        /// @brief update custom segment
        void updateCustomSegment(const int segmentIndex, const PositionVector& newLaneShape, const std::vector<double> &newLaneShapeRotations, const std::vector<double> &newLaneShapeLengths);

        /// @brief update lane2Lane segment (used to avoid unnecessary calculation in calculatePartialShapeRotationsAndLengths)
        void updateLane2LaneSegment(const int segmentIndex, const GNELane* lane, const GNELane* nextLane);

        /// @brief clear element geometry
        void clearSegmentGeometry();

        /// @brief get first position (or Invalid position if segments are empty)
        const Position &getFirstPosition() const;

        /// @brief get first position (or Invalid position if segments are empty)
        const Position &getLastPosition() const;

        /// @brief get first rotation (or Invalid position if segments are empty)
        double getFirstRotation() const;
        
        /// @brief Returns a boundary enclosing all segments
        Boundary getBoxBoundary() const;

        /// @brief begin iterator
        std::vector<Segment>::const_iterator begin() const;

        /// @brief end iterator
        std::vector<Segment>::const_iterator end() const;

        /// @brief front segment
        const Segment &front() const;

        /// @brief back segment
        const Segment &back() const;

        /// @brief number of segments
        int size() const;

    private:
        /// @brief vector of segments that constitutes the shape
        std::vector<Segment> myShapeSegments;

        /// @brief Invalidated assignment operator
        SegmentGeometry& operator=(const SegmentGeometry& other) = delete;
    };

    /// @brief lane2lane struct
    struct Lane2laneConnection {

        /// @brief constructor
        Lane2laneConnection(const GNELane* originLane);

        /// @brief update
        void updateLane2laneConnection();

        /// @brief connection shape
        std::map<const GNELane*, Geometry> connectionsMap;

    private:
        /// @brief origin lane
        const GNELane* myOriginLane = nullptr;
        
        /// @brief Invalidated assignment operator
        Lane2laneConnection& operator=(const Lane2laneConnection& other) = delete;
    };

    /// @brief return angle between two points (used in geometric calculations)
    static double calculateRotation(const Position& first, const Position& second);

    /// @brief return length between two points (used in geometric calculations)
    static double calculateLength(const Position& first, const Position& second);

    /// @brief adjust start and end positions in geometric path
    static void adjustStartPosGeometricPath(double &startPos, const GNELane* startLane, double &endPos, const GNELane* endLane);

    /**@brief calculate route between edges
     * @brief edges list of edges
     * @param startPos start position in the first lane (if -1, then starts at the beginning of lane)
     * @param endPos end position in the last lane (if -1, then ends at the end of lane)
     * @param extraFirstPosition extra first position (if is Position::INVALID, then it's ignored)
     * @param extraLastPosition extra last position (if is Position::INVALID, then it's ignored)
     */
    static void calculateEdgeGeometricPath(const GNEAttributeCarrier* AC, GNEGeometry::SegmentGeometry &segmentGeometry, const std::vector<GNEEdge*> &edges, 
                                           const SUMOVehicleClass vClass, GNELane *fromLane, GNELane *toLane, double startPos = -1, double endPos = -1, 
                                           const Position &extraFirstPosition = Position::INVALID, const Position &extraLastPosition = Position::INVALID);

    /**@brief calculate route between lanes
     * @brief lanes list of lanes
     * @param startPos start position in the first lane (if -1, then starts at the beginning of lane)
     * @param endPos end position in the last lane (if -1, then ends at the end of lane)
     * @param extraFirstPosition extra first position (if is Position::INVALID, then it's ignored)
     * @param extraLastPosition extra last position (if is Position::INVALID, then it's ignored)
     */
    static void calculateLaneGeometricPath(const GNEAttributeCarrier* AC, GNEGeometry::SegmentGeometry &segmentGeometry, const std::vector<GNELane*> &lanes, 
                                           double startPos = -1, double endPos = -1, const Position &extraFirstPosition = Position::INVALID, 
                                           const Position &extraLastPosition = Position::INVALID);

    /**@brief calculate route between edges
     * @brief edges list of edges
     * @param startPos start position in the first lane (if -1, then starts at the beginning of lane)
     * @param endPos end position in the last lane (if -1, then ends at the end of lane)
     * @param extraFirstPosition extra first position (if is Position::INVALID, then it's ignored)
     * @param extraLastPosition extra last position (if is Position::INVALID, then it's ignored)
     */
    static void updateGeometricPath(GNEGeometry::SegmentGeometry &segmentGeometry, const GNEEdge* edge, double startPos = -1, double endPos = -1, 
                                    const Position &extraFirstPosition = Position::INVALID, const Position &extraLastPosition = Position::INVALID);
};

#endif

/****************************************************************************/

