/****************************************************************************/
/// @file    Boundary.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that stores a 2D geometrical boundary
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Boundary_h
#define Boundary_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utility>
#include "AbstractPoly.h"
#include "Position2D.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Boundary
 * @brief A class that stores a 2D geometrical boundary
 */
class Boundary
            : public AbstractPoly {
public:
    /// Constructor - the boundary is unset
    Boundary();

    /// Constructor - the boundary will be build using the given values
    Boundary(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2);

    /// Destructor
    ~Boundary();

    /// Resets the boundary
    void reset();

    /// Makes the boundary include the given coordinate
    void add(SUMOReal x, SUMOReal y);

    /// Makes the boundary include the given coordinate
    void add(const Position2D &p);

    /// Makes the boundary include the given boundary
    void add(const Boundary &p);

    /// Returns the center of the boundary
    Position2D getCenter() const;

    /// Returns minimum x-coordinate
    SUMOReal xmin() const;

    /// Returns maximum x-coordinate
    SUMOReal xmax() const;

    /// Returns minimum y-coordinate
    SUMOReal ymin() const;

    /// Returns maximum y-coordinate
    SUMOReal ymax() const;

    /// Returns the width of the boudary
    SUMOReal getWidth() const;

    /// Returns the height of the boundary
    SUMOReal getHeight() const;

    /// Returns whether the boundary contains the given coordinate
    bool around(const Position2D &p, SUMOReal offset=0) const;

    /// Returns whether the boundary overlaps with the given polygon
    bool overlapsWith(const AbstractPoly &poly, SUMOReal offset=0) const;

    /// Returns whether the boundary is partially within the given polygon
    bool partialWithin(const AbstractPoly &poly, SUMOReal offset=0) const;

    /// Returns whether the boundary crosses the given line
    bool crosses(const Position2D &p1, const Position2D &p2) const;


    /** @brief extends the boundary by the given amount
     *
     * The method returns a reference to the instance for further use */
    Boundary &grow(SUMOReal by);

    /// flips ymin and ymax
    void flipY();

    /// Sets the boundary to the given values
    void set(SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax);

    /// Moves the boundary by the given amount
    void moveby(SUMOReal x, SUMOReal y);

    /// Output operator
    friend std::ostream &operator<<(std::ostream &os, const Boundary &b);

private:
    /// The boundaries
    SUMOReal myXmin,myXmax, myYmin, myYmax;

    /// Information whether the boundary was initialised
    bool myWasInitialised;

};


#endif

/****************************************************************************/

