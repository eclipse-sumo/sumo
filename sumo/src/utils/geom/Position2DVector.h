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
// Revision 1.6  2003/06/05 14:33:45  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <queue>
#include <iostream>
#include "AbstractPoly.h"
#include "Boundery.h"
#include "Position2D.h"


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

    /// Destructor
    ~Position2DVector();

    /// Appends the given position to the list
    void push_back(const Position2D &p);

    /// Puts the given position at the begin of the list
    void push_front(const Position2D &p);

    /** @brief Returns the information whether the position vector describes a polygon lying around the given point
        The optional offset is added to the polygon's bounderies */
    bool around(const Position2D &p, double offset=0) const;

    /** @brief Returns the information whether the given polygon overlaps with this
        Again a boundery may be specified */
    bool overlapsWith(const AbstractPoly &poly, double offset=0) const;

    /** Returns the information whether this list of points interesects the given line */
    bool intersects(const Position2D &p1, const Position2D &p2) const;

    /** Returns the information whether this list of points interesects one the given lines */
    bool intersects(const Position2DVector &v1) const;

    /** Returns the position of the intersection */
    Position2D intersectsAtPoint(const Position2D &p1,
        const Position2D &p2) const;

    /** Returns the position of the intersection */
    Position2D intersectsAtPoint(const Position2DVector &v1) const;

    /// Removes all information from this list
    void clear();

    /** @brief returns the position at the given index
        !!! exceptions?*/
    const Position2D &at(size_t i) const;

    /// returns the number of points making up the line vector
    size_t size() const;

    /// Returns the position at the given length
    Position2D positionAtLengthPosition(double pos) const;

    /// Returns the position between the two given point at the specified position */
    static Position2D positionAtLengthPosition(const Position2D &p1,
        const Position2D &p2, double pos);

    /// Returns a boundery enclosing this list of lines
    Boundery getBoxBoundery() const;

    /** @brief Returns the center
        !! Only for closed??? */
    Position2D center() const;

    /// Returns the length
    double length() const;

    /// Returns the information whether this polygon lies partially within the given polygon
    bool partialWithin(const AbstractPoly &poly, double offset=0) const;

    /// Returns the first position
    const Position2D &getBegin() const;

    /// Returns the last position
    const Position2D &getEnd() const;

    /// Returns the two lists made when this list vector is splitted at the given point
    std::pair<Position2DVector, Position2DVector> splitAt(double where) const;

    /// Output operator
    friend std::ostream &operator<<(std::ostream &os,
        const Position2DVector &geom);

    bool crosses(const Position2D &p1,
        const Position2D &p2) const;

	const ContType &getCont() const {
		return myCont;
	}

private:

    /// The list of points
    ContType myCont;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "Position2DVector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

