/****************************************************************************/
/// @file    Position2DVector.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A list of 2D-positions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Position2DVector_h
#define Position2DVector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <queue>
#include <iostream>
#include <utils/common/VectorHelper.h>
#include "AbstractPoly.h"
#include "Boundary.h"
#include "Position2D.h"


class Line2D;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Position2DVector
 * @brief A list of 2D-positions
 */
class Position2DVector
            : public AbstractPoly {
public:
    /// Definition of the list of points
    typedef std::deque<Position2D> ContType;

public:
    /** @brief Constructor
     *
     * Creates an empty position vector
     */
    Position2DVector() throw();


    /** @brief Constructor
     *
     * Creates an empty position vector, allocates storage of the given size
     */
    Position2DVector(unsigned int fieldSize) throw();


    /// @brief Destructor
    ~Position2DVector() throw();


    /// @name Adding items to the container
    /// @{

    /** @brief Appends the given position to the list
     * @param[in] p The position to append
     */
    void push_back(const Position2D &p) throw();


    /** @brief Appends all positions from the given vector
     * @param[in] p The vector from which values shall be appended
     */
    void push_back(const Position2DVector &p) throw();
    /// @}



    /// Puts the given position at the begin of the list
    void push_front(const Position2D &p);

    /** @brief Returns the information whether the position vector describes a polygon lying around the given point
        The optional offset is added to the polygon's bounderies */
    bool around(const Position2D &p, SUMOReal offset=0) const;

    /** @brief Returns the information whether the given polygon overlaps with this
        Again a boundary may be specified */
    bool overlapsWith(const AbstractPoly &poly, SUMOReal offset=0) const;

    /** Returns the information whether this list of points interesects the given line */
    bool intersects(const Position2D &p1, const Position2D &p2) const;

    /** Returns the information whether this list of points interesects one the given lines */
    bool intersects(const Position2DVector &v1) const;

    /** Returns the position of the intersection */
    Position2D intersectsAtPoint(const Position2D &p1,
                                 const Position2D &p2) const; // !!!

    Position2DVector intersectsAtPoints(const Position2D &p1,
                                        const Position2D &p2) const; // !!!

    DoubleVector intersectsAtLengths(const Position2DVector &s) const; // !!!

    DoubleVector intersectsAtLengths(const Line2D &s) const; // !!!

    /** Returns the position of the intersection */
    Position2D intersectsAtPoint(const Position2DVector &v1) const; // !!!

    /// Removes all information from this list
    void clear();

    void closePolygon();

    /** @brief returns the position at the given index
        !!! exceptions?*/
    const Position2D &operator[](int index) const;
    Position2D &operator[](int index);

    /// returns the number of points making up the line vector
    size_t size() const;

    /// Returns the position at the given length
    Position2D positionAtLengthPosition(SUMOReal pos) const;

    /// Returns the position at the given length
    SUMOReal rotationDegreeAtLengthPosition(SUMOReal pos) const;

    /// Returns the position between the two given point at the specified position */
    static Position2D positionAtLengthPosition(const Position2D &p1,
            const Position2D &p2, SUMOReal pos);

    /// Returns a boundary enclosing this list of lines
    Boundary getBoxBoundary() const;

    /** @brief Returns the center
        !! Only for closed??? */
    Position2D getPolygonCenter() const;
    Position2D getLineCenter() const;

    Position2D pop_back();
    Position2D pop_front();

    /// Returns the length
    SUMOReal length() const;

    /// Returns the information whether this polygon lies partially within the given polygon
    bool partialWithin(const AbstractPoly &poly, SUMOReal offset=0) const;

    /// Returns the first position
    const Position2D &getBegin() const;

    /// Returns the last position
    const Position2D &getEnd() const;

    /// Returns the two lists made when this list vector is splitted at the given point
    std::pair<Position2DVector, Position2DVector> splitAt(SUMOReal where) const;

    /// Output operator
    friend std::ostream &operator<<(std::ostream &os,
                                    const Position2DVector &geom);

    bool crosses(const Position2D &p1, const Position2D &p2) const;

    void reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    Position2DVector convexHull() const;

    int appendWithCrossingPoint(const Position2DVector &v);


    const ContType &getCont() const {
        return myCont;
    }

    Position2DVector resettedBy(SUMOReal x, SUMOReal y) const;

    Position2DVector getSubpart(SUMOReal begin, SUMOReal end) const;

    void sortAsPolyCWByAngle();

    void sortByIncreasingXY();

    void extrapolate(SUMOReal val);

    Position2DVector reverse() const;

    void move2side(SUMOReal amount);
    void move2side(SUMOReal amount, int index);

    Line2D lineAt(size_t pos) const;

    Line2D getBegLine() const;

    Line2D getEndLine() const;


    void insertAt(int index, const Position2D &p);

    class as_poly_cw_sorter {
    public:
        /// constructor
        explicit as_poly_cw_sorter(Position2D center);

    public:
        /// comparing operation
        int operator()(const Position2D &p1, const Position2D &p2) const;

    private:
        /// the edge to compute the relative angle of
        Position2D myCenter;

    };

    class increasing_x_y_sorter {
    public:
        /// constructor
        explicit increasing_x_y_sorter();

    public:
        /// comparing operation
        int operator()(const Position2D &p1, const Position2D &p2) const;

    };

    void resetBy(SUMOReal x, SUMOReal y);
    void resetBy(const Position2D &by);


    // !!!
    SUMOReal isLeft(const Position2D &P0, const Position2D &P1, const Position2D &P2) const;

    void set(size_t pos, const Position2D &p);

    void pruneFromBeginAt(const Position2D &p);
    void pruneFromEndAt(const Position2D &p);

    SUMOReal beginEndAngle() const;

    void eraseAt(int i);

    SUMOReal nearest_position_on_line_to_point(const Position2D &p) const;

    DoubleVector distances(const Position2DVector &s) const;
    DoubleVector distancesExt(const Position2DVector &s) const;

    SUMOReal distance(const Position2D &p) const;

    void push_back_noDoublePos(const Position2D &p, SUMOReal eps=POSITION_EPS);
    void push_front_noDoublePos(const Position2D &p, SUMOReal eps=POSITION_EPS);

    void replaceAt(size_t index, const Position2D &by);

    bool isClosed() const;

    void removeDoublePoints();

private:

    /// The list of points
    ContType myCont;

};


#endif

/****************************************************************************/

