#ifndef Position2DVector_h
#define Position2DVector_h
//---------------------------------------------------------------------------//
//                        Position2DVector.h -
//  A list of 2D-positions
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.21  2005/09/23 06:07:01  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.20  2005/09/15 12:18:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.19  2005/07/12 12:44:17  dkrajzew
// access function improved
//
// Revision 1.18  2005/04/28 09:02:48  dkrajzew
// level3 warnings removed
//
// Revision 1.17  2004/11/23 10:34:46  dkrajzew
// debugging
//
// Revision 1.3  2004/11/22 12:53:15  dksumo
// added 'pop_back' and 'inserAt'
//
// Revision 1.2  2004/10/29 06:25:23  dksumo
// boundery renamed to boundary
//
// Revision 1.1  2004/10/22 12:50:45  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.16  2004/03/19 13:01:11  dkrajzew
// methods needed for the new selection within the gui added; some style adaptions
//
// Revision 1.15  2004/02/16 14:00:00  dkrajzew
// some further work on edge geometry
//
// Revision 1.14  2003/12/09 11:33:49  dkrajzew
// made the assignment operator and copy constructor explicite in the wish to save memory
//
// Revision 1.13  2003/11/18 14:21:20  dkrajzew
// computation of junction-inlanes geometry added
//
// Revision 1.12  2003/10/15 11:56:30  dkrajzew
// further work on vissim-import
//
// Revision 1.11  2003/10/02 14:55:57  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.10  2003/09/25 09:03:53  dkrajzew
// some methods added, needed for the computation of line rotation
//
// Revision 1.9  2003/09/05 15:27:38  dkrajzew
// changes from adding internal lanes and further work on node geometry
//
// Revision 1.8  2003/08/14 14:05:51  dkrajzew
// functions to process a nodes geometry added
//
// Revision 1.7  2003/07/16 15:38:04  dkrajzew
// some work on computation and handling of geometry information
//
// Revision 1.6  2003/06/05 14:33:45  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <queue>
#include <iostream>
#include <utils/common/DoubleVector.h>
#include "AbstractPoly.h"
#include "Boundary.h"
#include "Position2D.h"


class Line2D;

/* =========================================================================
 * class definitions
 * ======================================================================= */
class Position2DVector
        : public AbstractPoly {
public:
	/// Definition of the list of points
    typedef std::deque<Position2D> ContType;

public:
    /// Constructor
    Position2DVector();

    /// Constructor
    Position2DVector(size_t fieldSize);

    /// Constructor
    Position2DVector(const Position2DVector &s);

    /// Assignment operator
    Position2DVector &operator=(const Position2DVector &s);

    /// Destructor
    ~Position2DVector();

    /// Appends the given position to the list
    void push_back(const Position2D &p);
    void push_back(const Position2DVector &p);

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
    const Position2D &at(int i) const;

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
    Position2D center() const;

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
        int operator() (const Position2D &p1, const Position2D &p2) const;

    private:
        /// the edge to compute the relative angle of
        Position2D _center;

    };

    class increasing_x_y_sorter {
    public:
        /// constructor
        explicit increasing_x_y_sorter();

    public:
        /// comparing operation
        int operator() (const Position2D &p1, const Position2D &p2) const;

    };

    void resetBy(SUMOReal x, SUMOReal y);
    void resetBy(const Position2D &by);


	// !!!
    SUMOReal isLeft( const Position2D &P0, const Position2D &P1, const Position2D &P2 ) const;

    void set(size_t pos, const Position2D &p);

    void pruneFromBeginAt(const Position2D &p);
    void pruneFromEndAt(const Position2D &p);

    SUMOReal beginEndAngle() const;

    void eraseAt(int i);

    SUMOReal nearest_position_on_line_to_point(const Position2D &p) const;

    DoubleVector distances(const Position2DVector &s) const;
    DoubleVector distancesExt(const Position2DVector &s) const;

    SUMOReal distance(const Position2D &p) const;

private:

    /// The list of points
    ContType myCont;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

