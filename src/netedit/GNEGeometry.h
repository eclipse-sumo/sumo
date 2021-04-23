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
/// @file    GNEGeometry.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2019
///
// File for geometry classes and functions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/Parameterised.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#include "GNEReferenceCounter.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;
class GNELane;
class GNEEdge;
class GNEAdditional;
class GNEDemandElement;
class GNEJunction;
class GNEViewNet;
class GNEHierarchicalElement;

// ===========================================================================
// class definitions
// ===========================================================================

struct GNEGeometry {

    /// @brief class for NETEDIT geometries over lanes
    class Geometry {

    public:
        /// @brief default constructor
        Geometry();

        /// @brief parameter constructor
        Geometry(const PositionVector& shape);

        /// @brief parameter constructor
        Geometry(const PositionVector& shape, const std::vector<double>& shapeRotations, const std::vector<double>& shapeLengths);

        /// @brief update entire geometry
        void updateGeometry(const PositionVector& shape);

        /// @brief update geometry (using a shape, a position over shape and a lateral offset)
        void updateGeometry(const PositionVector& shape, const double posOverShape, const double lateralOffset);

        /// @brief update geometry (using a shape, a starPos over shape, a endPos and a lateral offset)
        void updateGeometry(const PositionVector& shape, double starPosOverShape, double endPosOverShape, const double lateralOffset);

        /**@brief update geometry shape
         * @param shape Shape to be updated
         */
        void updateGeometry(const PositionVector& shape, double beginTrimPosition, double endTrimPosition,
                            const Position& extraFirstPosition, const Position& extraLastPosition);

        /// @brief update position and rotation
        void updateSinglePosGeometry(const Position& position, const double rotation);

        /// @brief scale geometry
        void scaleGeometry(const double scale);

        /// @brief The shape of the additional element
        const PositionVector& getShape() const;

        /// @brief The rotations of the single shape parts
        const std::vector<double>& getShapeRotations() const;

        /// @brief The lengths of the single shape parts
        const std::vector<double>& getShapeLengths() const;

    protected:
        /// @brief clear geometry
        void clearGeometry();

        /// @brief calculate shape rotations and lengths
        void calculateShapeRotationsAndLengths();

        /// @brief element shape
        PositionVector myShape;

        /// @brief The rotations of the shape (note: Always size = myShape.size()-1)
        std::vector<double> myShapeRotations;

        /// @brief The lengths of the shape (note: Always size = myShape.size()-1)
        std::vector<double> myShapeLengths;
    };

    /// @enum for dotted cotour type
    enum class DottedContourType {
        INSPECT,
        FRONT,
        MOVE
    };

    /// @brief class for pack all variables related with DottedGeometry color
    class DottedGeometryColor {

    public:
        /// @brief constructor
        DottedGeometryColor(const GUIVisualizationSettings& settings);

        /// @brief get inspected color (and change flag)
        const RGBColor& getInspectedColor();

        /// @brief get front color (and change flag)
        const RGBColor& getFrontColor();

        /// @brief change color
        void changeColor();

        /// @brief rest Dotted Geometry Color
        void reset();

    private:
        /// @brief pointer to GUIVisualizationSettings
        const GUIVisualizationSettings& mySettings;

        /// @brief flag to get color
        bool myColorFlag;

        /// @brief Invalidated assignment operator
        DottedGeometryColor& operator=(const DottedGeometryColor& other) = delete;
    };

    /// @brief class for pack all variables related with DottedGeometry
    class DottedGeometry {

    public:
        /// @brief dotted geometry segment
        struct Segment {
            /// @brief default constructor
            Segment();

            /// @brief constructor for a given shape
            Segment(PositionVector newShape);

            /// @brief shape
            PositionVector shape;

            /// @brief rotations
            std::vector<double> rotations;

            /// @brief lengths
            std::vector<double> lengths;

            /// @brief drawing offset (-1 or 1 only)
            double offset;
        };

        /// @brief constructor
        DottedGeometry();

        /// @brief constructor for shapes
        DottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape);

        /// @brief constructor for extremes
        DottedGeometry(const GUIVisualizationSettings& s,
                       const DottedGeometry& topDottedGeometry, const bool drawFirstExtrem,
                       const DottedGeometry& botDottedGeometry, const bool drawLastExtrem);

        /// @brief update DottedGeometry (using lane shape)
        void updateDottedGeometry(const GUIVisualizationSettings& s, const GNELane* lane);

        /// @brief update DottedGeometry (using shape)
        void updateDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape);

        /// @brief draw inspected dottedShape
        void drawInspectedDottedGeometry(DottedGeometryColor& dottedGeometryColor) const;

        /// @brief draw front inspected dottedShape
        void drawFrontDottedGeometry(DottedGeometryColor& dottedGeometryColor) const;

        /// @brief move shape to side
        void moveShapeToSide(const double value);

        /// @brief get width
        double getWidth() const;

        /// @brief change default width
        void setWidth(const double width);

        /// @brief invert offset of all segments
        void invertOffset();

    private:
        /// @brief calculate shape rotations and lengths
        void calculateShapeRotationsAndLengths();

        /// @brief geometry width
        double myWidth;

        /// @brief dotted element shape (note: It's centered in 0,0 due scaling)
        std::vector<DottedGeometry::Segment> myDottedGeometrySegments;

        /// @brief Invalidated assignment operator
        DottedGeometry& operator=(const DottedGeometry& other) = delete;
    };

    /// @brief class lane2lane connection geometry
    class Lane2laneConnection {

    public:
        /// @brief constructor
        Lane2laneConnection(const GNELane* fromLane);

        /// @brief update
        void updateLane2laneConnection();

        /// @brief check if exist a lane2lane geometry for the given tolane
        bool exist(const GNELane* toLane) const;

        /// @brief get lane2lane geometry
        const GNEGeometry::Geometry& getLane2laneGeometry(const GNELane* toLane) const;

        /// @brief get lane2lane dotted geometry
        const GNEGeometry::DottedGeometry& getLane2laneDottedGeometry(const GNELane* toLane) const;

    protected:
        /// @brief from lane
        const GNELane* myFromLane;

        /// @brief connection shape
        std::map<const GNELane*, std::pair<GNEGeometry::Geometry, GNEGeometry::DottedGeometry> > myConnectionsMap;

    private:
        /// @brief constructor
        Lane2laneConnection();

        /// @brief Invalidated assignment operator
        Lane2laneConnection& operator=(const Lane2laneConnection& other) = delete;
    };

    /// @brief class to pack all variables and functions relative to connections between hierarchical element and their children
    class HierarchicalConnections {

    private:
        /// @brief connection geometry
        class ConnectionGeometry {

        public:
            /// @brief parameter constructor
            ConnectionGeometry(GNELane* lane);

            /// @brief get lane
            const GNELane* getLane() const;

            /// @brief get position
            const Position& getPosition() const;

            /// @brief get rotation
            double getRotation() const;

        private:
            /// @brief lane
            GNELane* myLane;

            /// @brief position
            Position myPosition;

            /// @brief rotation
            double myRotation;

            /// @brief default constructor
            ConnectionGeometry();
        };

    public:
        /// @brief constructor
        HierarchicalConnections(GNEHierarchicalElement* hierarchicalElement);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void drawConnection(const GUIVisualizationSettings& s, const GNEAttributeCarrier* AC, const double exaggeration) const;

        /// @brief draw dotted connections between Parent and childrens
        void drawDottedConnection(const DottedContourType type, const GUIVisualizationSettings& s, const double exaggeration) const;

        /// @brief position and rotation of every symbol over lane
        std::vector<ConnectionGeometry> symbolsPositionAndRotation;

        /// @brief geometry connections between parents an their children
        std::vector<GNEGeometry::Geometry> connectionsGeometries;

    private:
        /// @brief pointer to hierarchical element parent
        GNEHierarchicalElement* myHierarchicalElement;
    };

    /// @brief return angle between two points (used in geometric calculations)
    static double calculateRotation(const Position& first, const Position& second);

    /// @brief return length between two points (used in geometric calculations)
    static double calculateLength(const Position& first, const Position& second);

    /// @brief adjust start and end positions in geometric path
    static void adjustStartPosGeometricPath(double& startPos, const GNELane* startLane, double& endPos, const GNELane* endLane);

    /// @brief draw lane geometry (use their own function due colors)
    static void drawLaneGeometry(const GNEViewNet* viewNet, const PositionVector& shape, const std::vector<double>& rotations,
                                 const std::vector<double>& lengths, const std::vector<RGBColor>& colors, double width, const bool onlyContour = false);

    /// @brief draw geometry
    static void drawGeometry(const GNEViewNet* viewNet, const Geometry& geometry, const double width);

    /// @brief draw contour geometry
    static void drawContourGeometry(const Geometry& geometry, const double width, const bool drawExtremes = false);

    /// @brief draw geometry points
    static void drawGeometryPoints(const GUIVisualizationSettings& s, const GNEViewNet* viewNet, const PositionVector& shape,
                                   const RGBColor& geometryPointColor, const RGBColor& textColor, const double radius, const double exaggeration);

    /// @brief draw moving hint
    static void drawMovingHint(const GUIVisualizationSettings& s, const GNEViewNet* viewNet, const PositionVector& shape,
                               const RGBColor& hintColor, const double radius, const double exaggeration);

    /// @brief draw dotted contour for the given dotted geometry (used by lanes, routes, etc.)
    static void drawDottedContourGeometry(const DottedContourType type, const GUIVisualizationSettings& s, const DottedGeometry& dottedGeometry, const double width, const bool drawFirstExtrem, const bool drawLastExtrem);

    /// @brief draw dotted contour for the given dottedGeometries (used by edges)
    static void drawDottedContourEdge(const DottedContourType type, const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFrontExtreme, const bool drawBackExtreme);

    /// @brief draw dotted contour for the given closed shape (used by Juctions, shapes and TAZs)
    static void drawDottedContourClosedShape(const DottedContourType type, const GUIVisualizationSettings& s, const PositionVector& shape, const double exaggeration);

    /// @brief draw dotted contour for the given shape (used by additionals)
    static void drawDottedContourShape(const DottedContourType type, const GUIVisualizationSettings& s, const PositionVector& shape, const double width, const double exaggeration);

    /// @brief draw dotted contour for the given Position and radius (used by Juctions and POIs)
    static void drawDottedContourCircle(const DottedContourType type, const GUIVisualizationSettings& s, const Position& pos, const double radius, const double exaggeration);

    /// @brief draw dotted squared contour (used by additionals and demand elements)
    static void drawDottedSquaredShape(const DottedContourType type, const GUIVisualizationSettings& s, const Position& pos,
                                       const double width, const double height, const double offsetX, const double offsetY, const double rot, const double exaggeration);

    /// @brief get a circle around the given position
    static PositionVector getVertexCircleAroundPosition(const Position& pos, const double width, const int steps = 8);

    /// @brief rotate over lane (used by Lock icons, detector logos, etc.)
    static void rotateOverLane(const double rot);

private:
    /// @brief Storage for precomputed sin/cos-values describing a circle
    static PositionVector myCircleCoords;

    /// @brief normalize angle for lookup in myCircleCoords
    static int angleLookup(const double angleDeg);
};
