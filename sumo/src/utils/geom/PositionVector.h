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
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "AbstractPoly.h"


// ===========================================================================
// class declarations
// ===========================================================================
class Line;
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
    typedef std::vector<Position> vp;

public:
    /** @brief Constructor
     *
     * Creates an empty position vector
     */
    PositionVector();


    /** @brief Constructor
     * @param[in] v The vector to copy
     */
    PositionVector(const std::vector<Position>& v);


    /// @brief Destructor
    ~PositionVector();

    using vp::iterator;
    using vp::const_iterator;
    using vp::const_reference;
    using vp::value_type;
    using vp::begin;
    using vp::end;
    using vp::push_back;
    using vp::pop_back;
    using vp::clear;
    using vp::size;
    using vp::front;
    using vp::back;
    using vp::reference;


    /// @name Adding items to the container
    /// @{


    /** @brief Appends all positions from the given vector
     * @param[in] p The vector from which values shall be appended
     */
    void push_back(const PositionVector& p);
    /// @}


    /// Puts the given position at the front of the list
    void push_front(const Position& p);

    /// Removes and returns the position at the fron of the list
    Position pop_front();

    void insertAt(int index, const Position& p);

    void replaceAt(int index, const Position& by);

    void eraseAt(int i);

    /** @brief Returns the information whether the position vector describes a polygon lying around the given point
        The optional offset is added to the polygon's bounderies */
    bool around(const Position& p, SUMOReal offset = 0) const;

    /** @brief Returns the information whether the given polygon overlaps with this
        Again a boundary may be specified */
    bool overlapsWith(const AbstractPoly& poly, SUMOReal offset = 0) const;

    /** Returns the information whether this list of points interesects the given line */
    bool intersects(const Position& p1, const Position& p2) const;

    /** Returns the information whether this list of points interesects one the given lines */
    bool intersects(const PositionVector& v1) const;

    /** Returns the position of the intersection */
    Position intersectsAtPoint(const Position& p1,
                               const Position& p2) const; // !!!

    /** Returns any intersection Points with the given line (ignoring z-coordinates) */
    PositionVector intersectionPoints2D(const Line& line) const;

    /** @brief For all intersections between this vector and other,
     * return the 2D-length of the subvector from this vectors start to the intersection */
    std::vector<SUMOReal> intersectsAtLengths2D(const PositionVector& other) const; // !!!

    /** @brief For all intersections between this vector and line,
     * return the 2D-length of the subvector from this vectors start to the intersection */
    std::vector<SUMOReal> intersectsAtLengths2D(const Line& line) const; // !!!

    /** Returns the position of the intersection */
    Position intersectsAtPoint(const PositionVector& v1) const; // !!!

    /// @brief ensures that the last position equals the first
    void closePolygon();

    /** @brief returns the position at the given index
        !!! exceptions?*/
    const Position& operator[](int index) const;
    Position& operator[](int index);

    /// Returns the position at the given length
    Position positionAtOffset(SUMOReal pos, SUMOReal lateralOffset = 0) const;

    /// Returns the position at the given length
    Position positionAtOffset2D(SUMOReal pos, SUMOReal lateralOffset = 0) const;

    /// Returns the rotation at the given length
    SUMOReal rotationDegreeAtOffset(SUMOReal pos) const;

    /// Returns the slope at the given length
    SUMOReal slopeDegreeAtOffset(SUMOReal pos) const;

    /// Returns the position between the two given point at the specified position */
    static Position positionAtOffset(const Position& p1,
                                     const Position& p2, SUMOReal pos, SUMOReal lateralOffset);

    /// Returns the position between the two given point at the specified position */
    static Position positionAtOffset2D(const Position& p1,
                                       const Position& p2, SUMOReal pos, SUMOReal lateralOffset);

    /// Returns a boundary enclosing this list of lines
    Boundary getBoxBoundary() const;

    /** @brief Returns the arithmetic of all corner points
     * @note: this is different from the centroid! */
    Position getPolygonCenter() const;

    /** @brief Returns the centroid (closes the polygon if unclosed) */
    Position getCentroid() const;

    /** @brief enlarges/shrinks the polygon by a factor based at the centroid */
    void scaleRelative(SUMOReal factor);

    /** @brief enlarges/shrinks the polygon by an absolute offset based at the centroid */
    void scaleAbsolute(SUMOReal offset);

    Position getLineCenter() const;

    /// Returns the length
    SUMOReal length() const;
    /// Returns the length
    SUMOReal length2D() const;


    /// Returns the area (0 for non-closed)
    SUMOReal area() const;

    /// Returns the information whether this polygon lies partially within the given polygon
    bool partialWithin(const AbstractPoly& poly, SUMOReal offset = 0) const;

    /// Returns the two lists made when this list vector is splitted at the given point
    std::pair<PositionVector, PositionVector> splitAt(SUMOReal where) const;

    /// Output operator
    friend std::ostream& operator<<(std::ostream& os, const PositionVector& geom);

    bool crosses(const Position& p1, const Position& p2) const;

    void add(SUMOReal xoff, SUMOReal yoff, SUMOReal zoff);

    void reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    PositionVector convexHull() const;

    int appendWithCrossingPoint(const PositionVector& v);

    // @brief append the given vector to this one
    void append(const PositionVector& v);

    PositionVector getSubpart(SUMOReal beginOffset, SUMOReal endOffset) const;

    PositionVector getSubpart2D(SUMOReal beginOffset, SUMOReal endOffset) const;

    PositionVector getSubpartByIndex(int beginIndex, int count) const;

    void sortAsPolyCWByAngle();

    void sortByIncreasingXY();

    void extrapolate(SUMOReal val);

    PositionVector reverse() const;

    void move2side(SUMOReal amount);

    Line lineAt(int pos) const;

    Line getBegLine() const;

    Line getEndLine() const;


    // @brief inserts p between the two closest positions and returns the insertion index
    int insertAtClosest(const Position& p);

    /// comparing operation
    bool operator==(const PositionVector& v2) const;

    class as_poly_cw_sorter {
    public:
        /// constructor
        as_poly_cw_sorter() {};

    public:
        /// comparing operation
        int operator()(const Position& p1, const Position& p2) const;

    };

    class increasing_x_y_sorter {
    public:
        /// constructor
        explicit increasing_x_y_sorter();

    public:
        /// comparing operation
        int operator()(const Position& p1, const Position& p2) const;

    };

    // !!!
    SUMOReal isLeft(const Position& P0, const Position& P1, const Position& P2) const;

    void pruneFromBeginAt(const Position& p);
    void pruneFromEndAt(const Position& p);

    SUMOReal beginEndAngle() const;

    SUMOReal nearest_offset_to_point2D(const Position& p, bool perpendicular = true) const;

    /* @brief index of the closest position to p
     * @note: may only be called for a non-empty vector */
    int indexOfClosest(const Position& p) const;

    // distances of all my points to s and all of s points to myself
    std::vector<SUMOReal> distances(const PositionVector& s) const;

    SUMOReal distance(const Position& p) const;

    void push_back_noDoublePos(const Position& p);
    void push_front_noDoublePos(const Position& p);

    bool isClosed() const;

    /** @brief Removes positions if too near
     * @param[in] minDist The minimum accepted distance; default: POSITION_EPS
     * @param[in] assertLength Whether the result must at least contain two points (be a line); default: false, to ensure original behaviour
     */
    void removeDoublePoints(SUMOReal minDist = POSITION_EPS, bool assertLength = false);

    void removeColinearPoints();

};


#endif

/****************************************************************************/

