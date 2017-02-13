/****************************************************************************/
/// @file    PositionVector.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A list of positions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PositionVector_h
#define PositionVector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <limits>
#include "AbstractPoly.h"


// ===========================================================================
// class declarations
// ===========================================================================

class Boundary;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PositionVector
 * @brief A list of positions
 */
class PositionVector : public AbstractPoly, private std::vector<Position> {

private:
    /// @brief  vector of position
    typedef std::vector<Position> vp;

public:
    /// @brief Constructor. Creates an empty position vector
    PositionVector();

    /// @brief Copy Constructor. Create a positionVector with the same elements as other positionVector
    /// @param[in] v The vector to copy
    PositionVector(const std::vector<Position>& v);

    /// @brief Parameter Constructor. Create a positionVector using a part of other positionVector
    /// @param[in] beg The begin iterator for copy
    /// @param[in] end The end iterator to copy
    PositionVector(const std::vector<Position>::const_iterator beg, const std::vector<Position>::const_iterator end);

    /// @brief Parameter Constructor used for lines
    /// @param[in] p1 the first position
    /// @param[in] p2 the second position
    PositionVector(const Position& p1, const Position& p2);

    /// @brief Destructor
    ~PositionVector();

    /// @name methode for iterate over PositionVector
    /// @{
    /// @brief iterator
    using vp::iterator;

    /// @brief constant iterator
    using vp::const_iterator;

    /// @brief contant reference
    using vp::const_reference;

    /// @brief value type
    using vp::value_type;

    /// @brief begin of position vector
    using vp::begin;

    /// @brief end of position vector
    using vp::end;

    /// @brief push a position in the back of position vector
    using vp::push_back;

    /// @brief push a position in the front of position vector
    using vp::pop_back;

    /// @brief clear all elements of position vector
    using vp::clear;
    /// @brief returns size of position vector
    using vp::size;

    /// @brief get the front element of position vector
    using vp::front;

    /// @brief get back element of position vector
    using vp::back;

    /// @brief get a reference of position vector
    using vp::reference;

    /// @brief erase a position of position vector gived by iterator
    using vp::erase;

    /// @brief insert a position in position vector gived by iterator
    using vp::insert;
    /// @}

    /// @brief Returns the information whether the position vector describes a polygon lying around the given point
    /// @note The optional offset is added to the polygon's boundaries
    bool around(const Position& p, SUMOReal offset = 0) const;

    /// @brief Returns the information whether the given polygon overlaps with this
    /// @note Again a boundary may be specified
    bool overlapsWith(const AbstractPoly& poly, SUMOReal offset = 0) const;

    /// @brief Returns the maximum overlaps between this and the given polygon (when not separated by at least zThreshold)
    SUMOReal getOverlapWith(const PositionVector& poly, SUMOReal zThreshold) const;

    /// @brief Returns the information whether this list of points interesects the given line
    bool intersects(const Position& p1, const Position& p2) const;

    /// @brief Returns the information whether this list of points interesects one the given lines
    bool intersects(const PositionVector& v1) const;

    /// @brief Returns the position of the intersection
    Position intersectionPosition2D(const Position& p1, const Position& p2, const SUMOReal withinDist = 0.) const;

    /// @brief For all intersections between this vector and other, return the 2D-length of the subvector from this vectors start to the intersection
    std::vector<SUMOReal> intersectsAtLengths2D(const PositionVector& other) const;

    /// @brief For all intersections between this vector and line, return the 2D-length of the subvector from this vectors start to the intersection
    std::vector<SUMOReal> intersectsAtLengths2D(const Position& lp1, const Position& lp2) const;

    /// @brief Returns the position of the intersection
    Position intersectionPosition2D(const PositionVector& v1) const;

    /// @brief ensures that the last position equals the first
    void closePolygon();

    /// @brief returns the constat position at the given index
    /// @ToDo !!! exceptions?
    const Position& operator[](int index) const;

    /// @brief returns the position at the given index
    /// @ToDo !!! exceptions?
    Position& operator[](int index);

    /// @brief Returns the position at the given length
    Position positionAtOffset(SUMOReal pos, SUMOReal lateralOffset = 0) const;

    /// @brief Returns the position at the given length
    Position positionAtOffset2D(SUMOReal pos, SUMOReal lateralOffset = 0) const;

    /// @brief Returns the rotation at the given length
    SUMOReal rotationAtOffset(SUMOReal pos) const;

    /// @brief Returns the rotation at the given length
    SUMOReal rotationDegreeAtOffset(SUMOReal pos) const;

    /// @brief Returns the slope at the given length
    SUMOReal slopeDegreeAtOffset(SUMOReal pos) const;

    /// @brief Returns the position between the two given point at the specified position
    static Position positionAtOffset(const Position& p1, const Position& p2, SUMOReal pos, SUMOReal lateralOffset = 0.);

    /// Returns the position between the two given point at the specified position
    static Position positionAtOffset2D(const Position& p1, const Position& p2, SUMOReal pos, SUMOReal lateralOffset = 0.);

    /// @brief Returns a boundary enclosing this list of lines
    Boundary getBoxBoundary() const;

    /// @brief Returns the arithmetic of all corner points
    /// @note: this is different from the centroid!
    Position getPolygonCenter() const;

    /// @brief Returns the centroid (closes the polygon if unclosed)
    Position getCentroid() const;

    /// @brief enlarges/shrinks the polygon by a factor based at the centroid
    void scaleRelative(SUMOReal factor);

    /// @brief enlarges/shrinks the polygon by an absolute offset based at the centroid
    void scaleAbsolute(SUMOReal offset);

    /// @brief get line center
    Position getLineCenter() const;

    /// @brief Returns the length
    SUMOReal length() const;

    /// @brief Returns the length
    SUMOReal length2D() const;

    /// @brief Returns the area (0 for non-closed)
    SUMOReal area() const;

    /// @brief Returns the information whether this polygon lies partially within the given polygon
    bool partialWithin(const AbstractPoly& poly, SUMOReal offset = 0) const;

    /// @brief Returns the two lists made when this list vector is splitted at the given point
    std::pair<PositionVector, PositionVector> splitAt(SUMOReal where) const;

    //// @brief Output operator
    friend std::ostream& operator<<(std::ostream& os, const PositionVector& geom);

    //// @brief check if two positions crosses
    bool crosses(const Position& p1, const Position& p2) const;

    //// @brief add a position
    void add(SUMOReal xoff, SUMOReal yoff, SUMOReal zoff);

    //// @brief add a position
    void add(const Position& offset);

    //// @brief mirror coordinates along the x-axis
    void mirrorX();

    //// @brief rotate all points around (0,0) in the plane by the given angle
    void rotate2D(SUMOReal angle);

    //// @brief get a convex Hull of position vector
    PositionVector convexHull() const;

    //// @brief append the given vector to this one
    void append(const PositionVector& v, SUMOReal sameThreshold = 2.0);

    /// @brief get subpart of a position vector
    PositionVector getSubpart(SUMOReal beginOffset, SUMOReal endOffset) const;

    /// @brief get subpart of a position vector in two dimensions (Z is ignored)
    PositionVector getSubpart2D(SUMOReal beginOffset, SUMOReal endOffset) const;

    /// @brief get subpart of a position vector using index and a cout
    PositionVector getSubpartByIndex(int beginIndex, int count) const;

    /// @brief short as polygon CV by angle
    void sortAsPolyCWByAngle();

    /// @brief shory by increasing X-Y Psitions
    void sortByIncreasingXY();

    /// @brief extrapolate position vector
    void extrapolate(const SUMOReal val, const bool onlyFirst = false);

    /// @brief extrapolate position vector in two dimensions (Z is ignored)
    void extrapolate2D(const SUMOReal val, const bool onlyFirst = false);

    /// @brief reverse position vector
    PositionVector reverse() const;

    /// @brief get a side position of position vector using a offset
    static Position sideOffset(const Position& beg, const Position& end, const SUMOReal amount);

    /// @brief move position vector to side using certain ammount
    void move2side(SUMOReal amount);

    /// @brief get angle  in certain position of position vector
    SUMOReal angleAt2D(int pos) const;

    /// @brief inserts p between the two closest positions and returns the insertion index
    int insertAtClosest(const Position& p);

    /// @brief removes the point closest to p and return the removal index
    int removeClosest(const Position& p);

    /// @brief comparing operation
    bool operator==(const PositionVector& v2) const;

    /// @brief clase for CW Sorter
    class as_poly_cw_sorter {
    public:
        /// @brief constructor
        as_poly_cw_sorter();

        /// @brief comparing operation for sort
        int operator()(const Position& p1, const Position& p2) const;
    };

    /// @brief clase for increasing Sorter
    class increasing_x_y_sorter {
    public:
        /// constructor
        explicit increasing_x_y_sorter();

        /// comparing operation
        int operator()(const Position& p1, const Position& p2) const;
    };

    /// @brief get left
    /// @note previously marked with "!!!"
    SUMOReal isLeft(const Position& P0, const Position& P1, const Position& P2) const;

    /// @brief returns the angle in radians of the line connecting the first and the last position
    SUMOReal beginEndAngle() const;

    /// @brief return the nearest offest to point 2D
    SUMOReal nearest_offset_to_point2D(const Position& p, bool perpendicular = true) const;

    /** @brief return position p within the length-wise coordinate system
     * defined by this position vector. The x value is the same as that returned
     * by nearest_offset_to_point2D(p) and the y value is the perpendicular distance to this
     * vector with the sign indicating the side (right is postive).
     * if extend is true, the vector is extended on both sides and the
     * x-coordinate of the result may be below 0 or above the length of the original vector
     */
    Position transformToVectorCoordinates(const Position& p, bool extend = false) const;

    /// @brief index of the closest position to p
    /// @note: may only be called for a non-empty vector
    int indexOfClosest(const Position& p) const;

    /// @brief distances of all my points to s and all of s points to myself
    /// @note if perpendicular is set to true, only the perpendicular distances are returned
    std::vector<SUMOReal> distances(const PositionVector& s, bool perpendicular = false) const;

    /// @brief closest 2D-distance to point p (or -1 if perpendicular is true and the point is beyond this vector)
    SUMOReal distance2D(const Position& p, bool perpendicular = false) const;

    /// @brief insert in back a non double position
    void push_back_noDoublePos(const Position& p);

    /// @brief insert in front a non double position
    void push_front_noDoublePos(const Position& p);

    /// @brief check if PositionVector is closed
    bool isClosed() const;

    /** @brief Removes positions if too near
     * @param[in] minDist The minimum accepted distance; default: POSITION_EPS
     * @param[in] assertLength Whether the result must at least contain two points (be a line); default: false, to ensure original behaviour
     */
    void removeDoublePoints(SUMOReal minDist = POSITION_EPS, bool assertLength = false);

    /// @brief return whether two positions differ in z-coordinate
    bool hasElevation() const;

    /// @brief return the same shape with intermediate colinear points removed
    PositionVector simplified() const;

    /** @brief return orthogonal through p (extending this vector if necessary)
     * @param[in] p The point through which to draw the orthogonal
     * @param[in] extend how long to extend this vector for finding an orthogonal
     * @param[out] distToClosest Distance between the intersection point and the closest geometry point
     */
    PositionVector getOrthogonal(const Position& p, SUMOReal extend, SUMOReal& distToClosest) const;


    /// @brief returned vector that is smoothed at the front (within dist)
    PositionVector smoothedZFront(SUMOReal dist = std::numeric_limits<SUMOReal>::max()) const;

    /// @brief return the offset at the given index
    SUMOReal offsetAtIndex2D(int index) const;

    /// @brief return the maximum grade of all segments as a fraction of zRange/length2D
    SUMOReal getMaxGrade() const;

private:
    /// @brief return whether the line segments defined by Line p11,p12 and Line p21,p22 intersect
    static bool intersects(const Position& p11, const Position& p12, const Position& p21, const Position& p22, const SUMOReal withinDist = 0., SUMOReal* x = 0, SUMOReal* y = 0, SUMOReal* mu = 0);
};


#endif

/****************************************************************************/

